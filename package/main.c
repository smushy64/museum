/**
 * Description:  Liquid Package Main
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 06, 2023
*/
#include "shared/defines.h"
#include "logging.h"
#include "manifest.h"
#include "resource_header.h"
#include "resource_package.h"

#include "shared/liquid_package.h"

#include "core/string.h"
#include "core/fs.h"
#include "core/math.h"
#include "core/sync.h"
#include "core/memory.h"
#include "core/jobs.h"
#include "core/lib.h"

#include "core/compression.h"

#include "generated/package_hashes.h"

#define PACKAGE_DEFAULT_OUTPUT_PATH "./package.lpkg"
#define PACKAGE_DEFAULT_HEADER_OUTPUT_PATH "./package_generated_header.h"

typedef enum PackageError : int {
    PACKAGE_SUCCESS = 0,

    PACKAGE_ERROR_ARGS_NO_ARGUMENTS = 128,
    PACKAGE_ERROR_ARGS_UNRECOGNIZED_ARGUMENT,
    PACKAGE_ERROR_ARGS_MISSING_ARGUMENT,
    PACKAGE_ERROR_ARGS_INVALID_ARGUMENT,
    PACKAGE_ERROR_ARGS_MISSING_MANIFEST_PATH,
    PACKAGE_ERROR_OUT_OF_MEMORY,
    PACKAGE_ERROR_LOGGING_INIT,
    PACKAGE_ERROR_JOBS_SYSTEM_INIT,
    PACKAGE_ERROR_PARSE_MANIFEST,
    PACKAGE_ERROR_SYNC_OBJECT_CREATE,
    PACKAGE_ERROR_NO_TMP_PATH,
    PACKAGE_ERROR_OPEN_TMP_PATH,
} PackageError;

typedef enum PackageMode : u32 {
    PACKAGE_MODE_INVALID,
    PACKAGE_MODE_HELP,
    PACKAGE_MODE_CREATE,
} PackageMode;

typedef struct PackageParams {
    PackageMode mode;
    b32 is_silent;
    b32 is_verbose;
    union {
        struct {
            const char* manifest_path;
            const char* output_path;
            const char* header_output_path;
            usize max_threads;
        } create;
        struct {
            b32 manifest;
            b32 supported_files;
            PackageMode submode;
        } help;
    };
} PackageParams;

internal void print_help( PackageMode mode );
internal void print_manifest(void);
internal void print_supported(void);
internal PackageError parse_arguments( int argc, char** argv, PackageParams* out_params );
internal PackageError package_create( PackageParams* params );

c_linkage int application_main( int argc, char** argv ) {
    PackageParams params = {};
    PackageError error = parse_arguments( argc, argv, &params );

    if( error != PACKAGE_SUCCESS ) {
        print_help( params.mode );
        goto package_end;
    }

    if( params.mode == PACKAGE_MODE_HELP ) {
        if( params.help.manifest ) {
            print_manifest();
        } else if( params.help.supported_files ) {
            print_supported();
        } else {
            print_help( params.help.submode );
        }
        goto package_end;
    }

    if( !log_init( params.is_silent, params.is_verbose ) ) {
        println_err(
            CONSOLE_COLOR_MAGENTA "failed to initialize logging!" CONSOLE_COLOR_RESET );
        return PACKAGE_ERROR_LOGGING_INIT;
    }

    switch( params.mode ) {
        case PACKAGE_MODE_CREATE: {
            error = package_create( &params );
        } break;
        default:
            println_err(
                CONSOLE_COLOR_MAGENTA "unimplemented mode!" CONSOLE_COLOR_RESET );
            panic();
    }

package_end:
    return error;
}

internal PackageError package_create( PackageParams* params ) {

    /* apply defaults */ {
        if( !params->create.output_path ) {
            params->create.output_path = PACKAGE_DEFAULT_OUTPUT_PATH;
        }
        if( !params->create.header_output_path ) {
            params->create.header_output_path = PACKAGE_DEFAULT_HEADER_OUTPUT_PATH;
        }

        usize system_threads = core_query_processor_count();
        if( !params->create.max_threads ) {
            params->create.max_threads = system_threads;
        } else {
            params->create.max_threads =
                min( params->create.max_threads, system_threads );
        }
    }

    #define SHARED_BUFFER_SLICE_SIZE (megabytes(1))
    usize shared_buffer_size = SHARED_BUFFER_SLICE_SIZE * params->create.max_threads;
    void* shared_buffer = system_alloc( shared_buffer_size );
    if( !shared_buffer ) {
        lp_error(
            "unable to allocate {f,m,.2} for shared buffer!",
            (f64)shared_buffer_size );
        return PACKAGE_ERROR_OUT_OF_MEMORY;
    }

    /* startup jobs system */ {
        usize job_system_memory_requirement =
            job_system_query_memory_requirement( params->create.max_threads );
        void* job_system_buffer = system_alloc( job_system_memory_requirement );
        if( !job_system_buffer ) {
            lp_error( "failed to allocate {f,m,.2}!",
                (f64)job_system_memory_requirement );
            return PACKAGE_ERROR_OUT_OF_MEMORY;
        }

        if( !job_system_initialize(
            params->create.max_threads,
            job_system_buffer
        ) ) {
            system_free( job_system_buffer, job_system_memory_requirement );
            lp_error( "failed to initialize jobs system!" );
            return PACKAGE_ERROR_JOBS_SYSTEM_INIT;
        }

        lp_note(
            "initialized jobs system with {usize} threads.",
            params->create.max_threads );
    }

    Manifest manifest = {};
    if( !manifest_parse( params->create.manifest_path, &manifest ) ) {
        return PACKAGE_ERROR_PARSE_MANIFEST;
    }

    GenerateHeaderParams generate_header_params = {};

    generate_header_params.header_output_path = params->create.header_output_path;
    generate_header_params.manifest           = &manifest;

    if( !semaphore_create( &generate_header_params.finished ) ) {
        lp_error( "failed to create semaphore!" );
        return PACKAGE_ERROR_SYNC_OBJECT_CREATE;
    }

    read_write_fence();

    job_system_push( job_generate_header, &generate_header_params );

    // TODO(alicia): maybe there's a better way to do this?
    string_slice_mut_capacity( tmp_path, 32 );
    u32 count = 0;
    #define MAX_CHECK_TMP (9999)
    do {
        if( count ) {
            tmp_path.len = 0;
        }
        string_slice_fmt( &tmp_path, "lpkg_tmp_{u,04}.tmp{0}", count++ );
        if( count >= MAX_CHECK_TMP ) {
            break;
        }
    } while( fs_file_exists( tmp_path.buffer ) );

    if( count >= MAX_CHECK_TMP ) {
        lp_error( "no temp file paths available!" );
        return PACKAGE_ERROR_NO_TMP_PATH;
    }

    // TODO(alicia): process manifest items here

    volatile u32 signal = 0;
    u32 last_signal = signal;

    for( usize i = 0; i < manifest.item_count; ++i ) {
        ResourcePackageParams resource_package_params = {};
        resource_package_params.tmp_path     = tmp_path.buffer;
        resource_package_params.manifest     = &manifest;
        resource_package_params.index        = i;
        resource_package_params.ready_signal = &signal;
        resource_package_params.buffer_size  = SHARED_BUFFER_SLICE_SIZE;
        resource_package_params.buffer       = shared_buffer;

        read_write_fence();

        job_system_push( job_package_resource, &resource_package_params );

        while( last_signal == signal ) {}

        read_write_fence();
        last_signal = signal;
    }

    FSFile* tmp_file = fs_file_open(
        tmp_path.buffer, FS_FILE_WRITE | FS_FILE_SHARE_WRITE );
    if( !tmp_file ) {
        lp_error( "failed to open temp file!" );
        return PACKAGE_ERROR_OPEN_TMP_PATH;
    }

    struct LiquidPackageHeader header = {};
    header.identifier     = LIQUID_PACKAGE_FILE_IDENTIFIER;
    header.resource_count = manifest.item_count;

    fs_file_write( tmp_file, sizeof( header ), &header );
    fs_file_close( tmp_file );

    read_write_fence();

    job_system_wait();

    read_write_fence();
    semaphore_destroy( &generate_header_params.finished );

    if( fs_file_move( params->create.output_path, tmp_path.buffer, false ) ) {
        lp_note( "created liquid package at path '{cc}'", params->create.output_path );
    } else {
        lp_error(
            "failed to move temp package to path '{cc}'!",
            params->create.output_path );
        lp_error( "temp file path: '{cc}'", tmp_path.buffer );
    }

    job_system_shutdown();
    manifest_free( &manifest );
    return PACKAGE_SUCCESS;
}

internal PackageMode parse_mode( StringSlice* slice ) {
    u64 slice_hash = string_slice_hash( slice );

    switch( slice_hash ) {
        case HASH_TOKEN_MODE_CREATE: return PACKAGE_MODE_CREATE;
        case HASH_TOKEN_MODE_HELP:   return PACKAGE_MODE_HELP;
        default:                     return PACKAGE_MODE_INVALID;
    }
}

internal PackageError parse_arguments(
    int argc, char** argv, PackageParams* out_params
) {
    if( argc == 1 ) {
        return PACKAGE_ERROR_ARGS_NO_ARGUMENTS;
    }

    #define arg_error( format, ... )\
        println_err( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )

    #define check_next_exists( token ) do {\
        if( (i + 1) >= argc ) {\
            arg_error( "{s} requires an argument after it!", token );\
            return PACKAGE_ERROR_ARGS_MISSING_ARGUMENT;\
        }\
    } while(0)
    #define check_next( thing, token )\
    argv[++i];\
    do {\
        if( *argv[i] == '-' ) {\
            arg_error( "{s} requires a " #thing " after it!", token );\
            return PACKAGE_ERROR_ARGS_INVALID_ARGUMENT;\
        }\
    } while(0)

    PackageParams params = {};

    string_slice_const( token_create_output, "--output" );
    string_slice_const( token_create_header_output, "--header-output" );
    string_slice_const( token_create_max_threads, "--max-threads" );

    for( int i = 1; i < argc; ++i ) {
        StringSlice arg = string_slice_from_cstr( 0, argv[i] );

        switch( params.mode ) {
            case PACKAGE_MODE_INVALID: {
                PackageMode parsed_mode = parse_mode( &arg );
                if( parsed_mode != PACKAGE_MODE_INVALID ) {
                    params.mode      = parsed_mode;
                    out_params->mode = params.mode;
                    continue;
                }
            } break;
            case PACKAGE_MODE_HELP: {
                PackageMode parsed_mode = parse_mode( &arg );
                switch( parsed_mode ) {
                    case PACKAGE_MODE_INVALID: {
                        u64 arg_hash = string_slice_hash( &arg );
                        switch( arg_hash ) {
                            case HASH_TOKEN_HELP_SUPPORTED: {
                                params.help.supported_files = true;
                                goto parse_end;
                            } break;
                            case HASH_TOKEN_HELP_MANIFEST: {
                                params.help.manifest = true;
                                goto parse_end;
                            } break;
                        }
                    } break;
                    default: {
                        params.help.submode = parsed_mode;
                        goto parse_end;
                    } break;
                }
            } break;
            case PACKAGE_MODE_CREATE: {
                if( arg.buffer[0] != '-' )  {
                    if( params.create.manifest_path ) {
                        break;
                    } else {
                        params.create.manifest_path = arg.buffer;
                        continue;
                    }
                }
                u64 arg_hash = string_slice_hash( &arg );

                switch( arg_hash ) {
                    case HASH_TOKEN_CREATE_OUTPUT: {
                        check_next_exists( token_create_output );
                        const char* next = check_next( path, token_create_output );
                        params.create.output_path = next;
                        continue;
                    } break;
                    case HASH_TOKEN_CREATE_HEADER_OUTPUT: {
                        check_next_exists( token_create_header_output );
                        const char* next =
                            check_next( path, token_create_header_output );
                        params.create.header_output_path = next;
                        continue;
                    } break;
                    case HASH_TOKEN_CREATE_MAX_THREADS: {
                        check_next_exists( token_create_max_threads );
                        arg = string_slice_from_cstr( 0, argv[++i] );

                        u64 parsed_int = 0;
                        if( !string_slice_parse_uint( &arg, &parsed_int ) ) {
                            arg_error(
                                "{s} requires an unsigned integer after it!",
                                token_create_max_threads );
                            return PACKAGE_ERROR_ARGS_INVALID_ARGUMENT;
                        }

                        params.create.max_threads = parsed_int;
                        continue;
                    } break;
                    case HASH_TOKEN_CREATE_SILENT: {
                        params.is_silent = true;
                        continue;
                    } break;
                    case HASH_TOKEN_CREATE_VERBOSE: {
                        params.is_verbose = true;
                        continue;
                    } break;
                }
            } break;
            default: break;
        }

        arg_error( "Unrecognized argument '{s}'!", arg );
        return PACKAGE_ERROR_ARGS_UNRECOGNIZED_ARGUMENT;
    }

    if( params.mode == PACKAGE_MODE_CREATE ) {
        if( !params.create.manifest_path ) {
            arg_error( "no manifest path provided!" );
            return PACKAGE_ERROR_ARGS_MISSING_MANIFEST_PATH;
        }
        if( !fs_file_exists( params.create.manifest_path ) ) {
            arg_error(
                "path to manifest is invalid! '{cc}'", params.create.manifest_path );
            return PACKAGE_ERROR_ARGS_MISSING_MANIFEST_PATH;
        }
    }

parse_end:
    *out_params = params;
    return PACKAGE_SUCCESS;
}

internal void print_help( PackageMode mode ) {
    println( "OVERVIEW: Liquid Engine Asset Packager\n" );
    println( "USAGE: lpkg [mode] <arguments>\n" );

    switch( mode ) {
        case PACKAGE_MODE_INVALID: {
            println( "MODES:" );
            println( "   create  create a liquid engine package file." );
            println( "   help    print this help message or print help for given mode" );
        } break;
        case PACKAGE_MODE_HELP: {
            println( "ARGUMENTS: (mode = help)" );
            println( "    [mode]       print help for given mode." );
            println( "    --manifest   print how to format a manifest file." );
            println( "    --supported  print supported file types." );
        } break;
        case PACKAGE_MODE_CREATE: {
            println( "ARGUMENTS: (mode = create)" );
            println( "    [path]                      path to a manifest file. (required)" );
            println( "    --output [path]             set path to output package file. (default = '" PACKAGE_DEFAULT_OUTPUT_PATH "')" );
            println( "    --header-output [path]      set path to output header. (default = '" PACKAGE_DEFAULT_HEADER_OUTPUT_PATH "')" );
            println( "    --max-threads [uint >= 1]   limit maximum number of threads (default = system hyper-thread count)" );
            println( "    --silent                    only print errors." );
            println( "    --verbose                   print more messages (--silent takes precedence)" );
        } break;
    }
}

internal void print_manifest(void) {
    println( "Liquid Package Manifest format." );
    println( "File type:      ASCII text" );
    println( "File extension: .manifest" );
    println( "Each resource is denoted by a valid " );
    println( "C identifier followed by a colon." );
    println( "Example: 'SOME_IDENTIFIER:'" );
    println( "A resource has a number of fields that are tabbed in." );
    println( "Fields and their value are enclosed in quotes." );
    println( "All lines starting with '#' are considered comments and ignored." );
    println( "The following list contains all fields with the appropriate formatting:" );
    println( "required:" );
    println( "   path: \"./some/path/\" (must be relative to manifest's directory)" );
    println( "   type: type (valid types listed below)" );
    println( "        - audio" );
    println( "        - model" );
    println( "        - texture" );
    println( "        - text" );
    println( "\nManifest example:" );
    println( "0: manifest" );
    println( "1: count: 2" );
    println( "2: SOUND_EFFECT:" );
    println( "3:     path: \"./resources/audio.wav\"" );
    println( "4:     type: audio" );
}
internal void print_supported(void) {
    println( "Liquid Package supported files:" );
    println( "texture" );
    println( "    .bmp - 32bpp RGBA or RGBX" );
    println( "    .bmp - 24bpp RGB" );
    println( "audio" );
    println( "    .wav - 16-bit PCM at 44.1Khz, mono or stereo only." );
    println( "All unsupported file types are skipped "
             "but they are still recorded in package." );
}

global Mutex global_logging_mutex      = {0};
global b32   global_logging_is_silent  = false;
global b32   global_logging_is_verbose = false;

b32 log_init( b32 is_silent, b32 is_verbose ) {
    if( !mutex_create( &global_logging_mutex ) ) {
        return false;
    }
    global_logging_is_silent  = is_silent;
    global_logging_is_verbose = is_verbose;
    return true;
}
void ___log( LogType type, usize format_len, const char* format, ... ) {
    switch( type ) {
        case LP_LOG_TYPE_WARN:
        case LP_LOG_TYPE_NORMAL: {
            if( global_logging_is_silent ) {
                return;
            }
        } break;
        case LP_LOG_TYPE_VERBOSE: {
            if( global_logging_is_silent || !global_logging_is_verbose ) {
                return;
            }
        } break;
        default: break;
    }

    read_write_fence();

    mutex_lock( &global_logging_mutex );

    read_write_fence();

    va_list va;
    va_start( va, format );

    switch( type ) {
        case LP_LOG_TYPE_ERROR: {
            print_string_stderr( sizeof(CONSOLE_COLOR_RED), CONSOLE_COLOR_RED );
            ___internal_print_err_va( format_len, format, va );
        } break;
        case LP_LOG_TYPE_WARN:
            print_string_stdout( sizeof(CONSOLE_COLOR_YELLOW), CONSOLE_COLOR_YELLOW );
        default: {
            ___internal_print_va( format_len, format, va );
        } break;
    }

    va_end( va );

    switch( type ) {
        case LP_LOG_TYPE_WARN: {
            print_string_stdout( sizeof(CONSOLE_COLOR_RESET), CONSOLE_COLOR_RESET );
        } break;
        case LP_LOG_TYPE_ERROR: {
            print_string_stderr( sizeof(CONSOLE_COLOR_RESET), CONSOLE_COLOR_RESET );
        } break;
        default: break;
    }

    read_write_fence();

    mutex_unlock( &global_logging_mutex );
}

#include "generated_dependencies.inl"

