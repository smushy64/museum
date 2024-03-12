/**
 * Description:  Liquid Packager.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 07, 2024
*/
#include "shared/defines.h"

#include "core/print.h"
#include "core/string.h"
#include "core/path.h"
#include "core/jobs.h"
#include "core/math.h"
#include "core/memory.h"
#include "core/fs.h"
#include "core/sync.h"
#include "core/rand.h"

#include "generated/package_hashes.h"

#include "package/logging.h"
#include "package/manifest.h"
#include "package/error.h"
#include "package/header.h"
#include "package/resource.h"

global const char* global_program_name = "lpkg";

#define PACKAGE_DEFAULT_OUTPUT_PATH "./package.lpkg"
#define PACKAGE_DEFAULT_HEADER_OUTPUT_PATH "./package.h"
#define PACKAGE_DEFAULT_THREAD_COUNT 4

typedef enum : u32 {
    PACKAGE_MODE_NONE,
    PACKAGE_MODE_CREATE,
    PACKAGE_MODE_MODIFY,
    PACKAGE_MODE_HELP,
} PackageMode;
const char* package_mode_to_cstr( PackageMode mode ) {
    switch( mode ) {
        case PACKAGE_MODE_CREATE: return "create";
        case PACKAGE_MODE_MODIFY: return "modify";
        case PACKAGE_MODE_HELP:   return "help";

        case PACKAGE_MODE_NONE: panic();
    }
}
const char* package_mode_description( PackageMode mode ) {
    switch( mode ) {
        case PACKAGE_MODE_CREATE: return "create a liquid package from manifest file";
        case PACKAGE_MODE_MODIFY: return "modify a liquid package and its corresponding header";
        case PACKAGE_MODE_HELP:   return "print this message or help for specified mode";

        case PACKAGE_MODE_NONE: panic();
    }
}

#define HELP_MODE_MODE       (0)
#define HELP_MODE_MANIFEST   (1 << 0)
#define HELP_MODE_FILE_TYPES (1 << 1)

#define CREATE_MODE_SILENT  (1 << 0)
#define CREATE_MODE_VERBOSE (1 << 1)

typedef struct {
    PackageMode mode;
    u32         bitflags;
    union {
        struct {
            PathSlice manifest_path;
            PathSlice output_path;
            PathSlice header_output_path;
            u32 max_thread_count;
        } create;
        struct {
            PackageMode  mode;
        } help;
    };
} Arguments;

PackageError package_mode_create(
    PathSlice manifest_path, PathSlice output_path, PathSlice header_output_path );

PackageMode parse_package_mode( u64 arg_hash );
void print_help(void);
void print_help_mode( PackageMode mode );
void print_manifest_format(void);
void print_file_types(void);
PackageError thread_initialize( usize thread_count );
void thread_shutdown(void);
int main( int argc, char** argv ) {
    usize thread_index = 0;
    rand_reset_global_state();

    #define error( format, ... )\
        println_err( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )

    if( argc == 1 ) {
        error( "no arguments provided!" );
        print_help();
        return PACKAGE_ERROR_NO_ARGUMENTS;
    }

    global_program_name = argv[0];

    Arguments args = {};

    #define check_for_next( mode, format, ... ) do {\
        if( i + 1 > argc ) {\
            error( format, ##__VA_ARGS__ );\
            print_help_mode( mode );\
            return PACKAGE_ERROR_INVALID_ARGUMENTS;\
        }\
        arg      = string_slice_from_cstr( 0, argv[++i] );\
        arg_hash = string_slice_hash( arg );\
    } while(0)

    for( int i = 1; i < argc; ++i ) {
        StringSlice arg = string_slice_from_cstr( 0, argv[i] );
        u64 arg_hash    = string_slice_hash( arg );

        if( !args.mode ) {
            args.mode = parse_package_mode( arg_hash );
            if( args.mode ) {
                continue;
            }
        }

        switch( args.mode ) {
            case PACKAGE_MODE_HELP: {
                if( !args.help.mode ) {
                    args.help.mode = parse_package_mode( arg_hash );
                    if( args.help.mode ) {
                        continue;
                    }
                }

                switch( arg_hash ) {
                    case HASH_ARG_HELP_MANIFEST: {
                        args.bitflags |= HELP_MODE_MANIFEST;
                        continue;
                    } break;
                    case HASH_ARG_HELP_FILE_TYPES: {
                        args.bitflags |= HELP_MODE_FILE_TYPES;
                        continue;
                    } break;
                }
            } break;
            case PACKAGE_MODE_CREATE: {
                if( arg.str[0] == '-' ) {
                    switch( arg_hash ) {
                        case HASH_ARG_CREATE_OUTPUT: {
                            check_for_next(
                                PACKAGE_MODE_CREATE, "-o requires a path after it!" );
                            if( !args.create.output_path.str ) {
                                args.create.output_path = arg;

                                PathSlice ext = {};
                                if( !path_slice_get_extension(
                                    args.create.output_path, &ext ) ||
                                    !path_slice_cmp( ext, path_slice( ".lpkg" ) )
                                ) {
                                    usize capacity = args.create.output_path.len;
                                    capacity += sizeof(".lpkg") - 1;
                                    char* buffer = system_alloc( capacity );
                                    if( !buffer ) {
                                        error( "failed to allocate {f,m,.2}!", (f64)capacity );
                                        return PACKAGE_ERROR_OUT_OF_MEMORY;
                                    }

                                    PathBuffer output_path_buffer = {};
                                    output_path_buffer.str = buffer;
                                    output_path_buffer.cap = capacity;
                                    output_path_buffer.len =
                                        args.create.output_path.len - ext.len;
                                    memory_copy(
                                        buffer,
                                        args.create.output_path.str,
                                        output_path_buffer.len );

                                    assert( path_buffer_set_extension(
                                        &output_path_buffer, path_slice( "lpkg" ) ) );

                                    args.create.output_path =
                                        to_slice( &output_path_buffer );
                                }
                                continue;
                            }
                        } break;
                        case HASH_ARG_CREATE_HEADER_PATH: {
                            check_for_next(
                                PACKAGE_MODE_CREATE,
                                "--header-path requires a path after it!" );
                            if( !args.create.header_output_path.str ) {
                                args.create.header_output_path =
                                    reinterpret_cast( PathSlice, &arg );

                                PathSlice ext = {};
                                if( !path_slice_get_extension(
                                    args.create.header_output_path, &ext ) ||
                                    !path_slice_cmp( ext, path_slice( ".h" ) )
                                ) {
                                    usize capacity = args.create.header_output_path.len;
                                    capacity += sizeof(".h") - 1;
                                    char* buffer = system_alloc( capacity );
                                    if( !buffer ) {
                                        error( "failed to allocate {f,m,.2}!", (f64)capacity );
                                        return PACKAGE_ERROR_OUT_OF_MEMORY;
                                    }

                                    PathBuffer output_path_buffer = {};
                                    output_path_buffer.str = buffer;
                                    output_path_buffer.cap = capacity;
                                    output_path_buffer.len =
                                        args.create.header_output_path.len - ext.len;
                                    memory_copy(
                                        buffer,
                                        args.create.header_output_path.str,
                                        output_path_buffer.len );

                                    assert( path_buffer_set_extension(
                                        &output_path_buffer, path_slice( "h" ) ) );

                                    args.create.header_output_path =
                                        to_slice( &output_path_buffer );
                                }

                                continue;
                            }
                        } break;
                        case HASH_ARG_SILENT: {
                            args.bitflags |= CREATE_MODE_SILENT;
                            continue;
                        } break;
                        case HASH_ARG_VERBOSE: {
                            args.bitflags |= CREATE_MODE_VERBOSE;
                            continue;
                        } break;
                        case HASH_ARG_MAX_THREADS: {
                            check_for_next(
                                PACKAGE_MODE_CREATE,
                                "--max-threads requires a thread count after it!" );

                            u64 parsed_thread_count = 0;
                            if( !string_slice_parse_uint(
                                arg, &parsed_thread_count
                            ) ) {
                                error( "could not parse thread count: '{s}'", arg );
                                return PACKAGE_ERROR_INVALID_ARGUMENTS;
                            }

                            args.create.max_thread_count = parsed_thread_count;
                            continue;
                        } break;
                    }
                } else {
                    if( !args.create.manifest_path.str ) {
                        args.create.manifest_path =
                            reinterpret_cast( PathSlice, &arg );
                        continue;
                    }
                }
            } break;
            default: break;
        }

        error( "unrecognized argument: '{s}'", arg );
        print_help();

        return PACKAGE_ERROR_UNRECOGNIZED_ARGUMENT;
    }

    #undef check_for_next

    switch( args.mode ) {
        case PACKAGE_MODE_CREATE: {
            if( !args.create.manifest_path.str ) {
                error( "create mode requires a path to a manifest!" );
                print_help_mode( PACKAGE_MODE_CREATE );
                return PACKAGE_ERROR_MISSING_MANIFEST_PATH;
            }

            if( !args.create.output_path.str ) {
                args.create.output_path =
                    path_slice( PACKAGE_DEFAULT_OUTPUT_PATH );
            }
            if( !args.create.header_output_path.str ) {
                args.create.header_output_path =
                    path_slice( PACKAGE_DEFAULT_HEADER_OUTPUT_PATH );
            }

            if( args.create.max_thread_count ) {
                args.create.max_thread_count =
                    clamp( args.create.max_thread_count, 1, 16 );
            } else {
                args.create.max_thread_count = PACKAGE_DEFAULT_THREAD_COUNT;
            }

            b32 silent  = bitfield_check( args.bitflags, CREATE_MODE_SILENT );
            b32 verbose = bitfield_check( args.bitflags, CREATE_MODE_VERBOSE );

            if( !logging_initialize( verbose, silent ) ) {
                error( "fatal error: failed to initialize logging!" );
                return PACKAGE_ERROR_INITIALIZE_LOGGING;
            }

            PackageError thread_error =
                thread_initialize( args.create.max_thread_count );
            if( thread_error ) {
                error( "fatal error: failed to initialize threads!" );
                return thread_error;
            }

            log_note( "config:" );
            log_note( "    manifest path: '{s}'", args.create.manifest_path );
            log_note( "    output path:   '{s}'", args.create.output_path );
            log_note( "    header path:   '{s}'", args.create.header_output_path );
            log_note( "    max threads:   {u}", args.create.max_thread_count );
            log_note( "    silent:        {b}", silent );
            log_note( "    verbose:       {b}", verbose );

            PackageError result = package_mode_create(
                args.create.manifest_path,
                args.create.output_path,
                args.create.header_output_path );

            if( result ) {
                error( "failed to create package '{p}'", args.create.output_path );
                return result;
            }

            log_print(
                "successfully created package at path '{p}'",
                args.create.output_path );

            thread_shutdown();
        } break;
        case PACKAGE_MODE_HELP: {
            print_help_mode( args.help.mode );
            if( bitfield_check( args.bitflags, HELP_MODE_MANIFEST ) ) {
                print_manifest_format();
            }
            if( bitfield_check( args.bitflags, HELP_MODE_FILE_TYPES ) ) {
                print_file_types();
            }
        } break;
        default: {
            error( "mode '{cc}' has not been implemented yet!",
                package_mode_to_cstr( args.mode ) );
            print_help();
        } return PACKAGE_ERROR_UNIMPLEMENTED;
    }

    #undef error
    return PACKAGE_SUCCESS;
}

GlobalProcessResourceParams* global_process_resource_params;
PackageError package_mode_create(
    PathSlice manifest_path, PathSlice output_path, PathSlice header_output_path
) {
    usize thread_index = 0;

    GlobalProcessResourceParams process_resource_params = {};
    global_process_resource_params = &process_resource_params;

    /* attempt to create output file */ {
        FileHandle* output = fs_file_open( output_path, FILE_OPEN_FLAG_READ );
        if( !output ) {
            output = fs_file_open( output_path, FILE_OPEN_FLAG_CREATE | FILE_OPEN_FLAG_WRITE );
            if( !output ) {
                log_error( "failed to open output path '{p}'!", output_path );
                return PACKAGE_ERROR_OPEN_FILE;
            }
        }
        fs_file_close( output );
    }

    /* attemp to create temp directory */ {
        PathSlice pkgtemp = path_slice( "./pkgtemp" );
        if( !fs_directory_exists( pkgtemp ) ) {
            if( !fs_directory_create( path_slice( "./pkgtemp" ) ) ) {
                log_error( "failed to create temp directory!" );
                return PACKAGE_ERROR_CREATE_TEMP_DIRECTORY;
            }
        }
    }

    Manifest manifest = {};
    PackageError result = manifest_parse( manifest_path, &manifest );
    if( result != PACKAGE_SUCCESS ) {
        return result;
    }
    log_note( "successfully parsed manifest '{s}'!", manifest_path );

    HeaderGeneratorParams header_params = {};
    header_params.manifest    = &manifest;
    header_params.output_path = header_output_path;
    read_write_fence();
    job_system_push( job_header_generate, &header_params );

    global_process_resource_params->manifest    = &manifest;
    global_process_resource_params->output_path = output_path;
    path_slice_get_parent( manifest_path,
        &global_process_resource_params->manifest_directory );
    read_write_fence();

    #define MAX_RESOURCE_PROCESS_COUNT (16)

    usize running_item_index  = 0;
    usize remaining_resources = manifest.items.count;
    while( remaining_resources ) {
        usize desired_job_count = MAX_RESOURCE_PROCESS_COUNT;
        if( remaining_resources < desired_job_count ) {
            desired_job_count = remaining_resources;
        }

        usize jobs_kicked_off = 0;
        for( usize i = 0; i < desired_job_count; ++i ) {
            usize index = i + running_item_index;
            if( !job_system_push( job_process_resource, (void*)index ) ) {
                break;
            }
            jobs_kicked_off++;
        }
        running_item_index  += jobs_kicked_off;
        remaining_resources -= jobs_kicked_off;

        if( remaining_resources ) {
            job_process_resource( 0, (void*)running_item_index );
            running_item_index++;
            remaining_resources--;
        }

        read_write_fence();
        job_system_wait();

        if( global_process_resource_params->error_code ) {
            result = global_process_resource_params->error_code;
            break;
        }
    }

    read_write_fence();
    job_system_wait();

    if( header_params.error ) {
        log_error( "failed to create header '{s}'!", header_output_path );
        result = header_params.error;
    }

    if( result == PACKAGE_SUCCESS ) {
        FileHandle* output_file =
            fs_file_open( output_path, FILE_OPEN_FLAG_WRITE );
        if( !output_file ) {
            log_error( "failed to open output file???" );
            result = PACKAGE_ERROR_OPEN_FILE;
        }

        struct PackageHeader header = {};
        header.id             = PACKAGE_ID;
        header.resource_count = manifest.items.count;

        if( !fs_file_write( output_file, sizeof(header), &header ) ) {
            log_error( "failed to write to output file???" );
            result = PACKAGE_ERROR_WRITE_FILE;
        }

        fs_file_close( output_file );
    }

    fs_directory_delete( path_slice( "./pkgtemp" ), true );

    manifest_destroy( &manifest );
    #undef MAX_RESOURCE_PROCESS_COUNT
    return result;
}

global usize global_thread_buffer_size   = 0;
global void* global_thread_buffer        = NULL;
global usize global_thread_buffer_offset = 0;
PackageError thread_initialize( usize thread_count ) {
    #define error( format, ... )\
        println_err( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )

    usize buffer_size = (thread_count + 1) * THREAD_BUFFER_SIZE;
    global_thread_buffer_offset =
        job_system_query_memory_requirement( thread_count );
    buffer_size += global_thread_buffer_offset;

    void* buffer = system_alloc( buffer_size );
    if( !buffer ) {
        error( "fatal error: failed to create job system" );
        error( "fatal error: could not allocate {f,m,.2}!", (f64)buffer_size );
        return PACKAGE_ERROR_OUT_OF_MEMORY;
    }

    global_thread_buffer      = buffer;
    global_thread_buffer_size = buffer_size;

    if( !job_system_initialize( thread_count, buffer ) ) {
        error( "fatal error: failed to create job system" );
        return PACKAGE_ERROR_CREATE_JOB_SYSTEM;
    }

    #undef error
    return PACKAGE_SUCCESS;
}
void thread_shutdown(void) {
    job_system_shutdown();
    system_free( global_thread_buffer, global_thread_buffer_size );
}
void* thread_buffer_get( usize thread_index ) {
    u8* start = (u8*)global_thread_buffer + global_thread_buffer_offset;

    return start + (thread_index * THREAD_BUFFER_SIZE);
}

void print_help(void) {
    println( "OVERVIEW: Utility Packager\n" );
    println( "USAGE: {cc} <mode>\n", global_program_name );
    println( "MODES:" );

    for(
        PackageMode mode = PACKAGE_MODE_NONE + 1;
        mode <= PACKAGE_MODE_HELP;
        mode++
    ) {
        println(
            "    {cc,-10}{cc}",
            package_mode_to_cstr( mode ),
            package_mode_description( mode ) );
    }
    println("");
}

void print_help_mode( PackageMode mode ) {
    if( !mode ) {
        print_help();
        return;
    }

    const char* mode_cstr = package_mode_to_cstr( mode );
    println( "OVERVIEW: Utility Packager\n" );
    println( "USAGE: {cc} {cc}\n", global_program_name, mode_cstr );
    println( "ARGUMENTS:" );

    switch( mode ) {
        case PACKAGE_MODE_NONE: break;
        case PACKAGE_MODE_CREATE: {
            println( "    <path>                path to a manifest (required)" );
            println( "    -o <path>             set output path (default=" PACKAGE_DEFAULT_OUTPUT_PATH ")" );
            println( "    --header-path <path>  set header output path (default=" PACKAGE_DEFAULT_HEADER_OUTPUT_PATH ")" );
            println( "    --max-threads <uint>  set maximum thread count (default=" macro_value_to_string( PACKAGE_DEFAULT_THREAD_COUNT ) ")" );
            println( "    --silent              do not print any non-error messages" );
            println( "    --verbose             print extra messages (silent takes precedence)" );
        } break;
        case PACKAGE_MODE_MODIFY: {
            println( "    <path>                path to package's manifest (required)" );
            println( "    --package <path>      path to package (required)" );
            println( "    --header <path>       path to package's resource id header (required)" );
            println( "    --list [<indices>]    comma separated list of indices of package resources that have changed. (required)" );
            println( "    --max-threads <uint>  set maximum thread count (default=" macro_value_to_string( PACKAGE_DEFAULT_THREAD_COUNT ) ")" );
            println( "    --silent              do not print any non-error messages" );
            println( "    --verbose             print extra messages (silent takes precedence)" );
        } break;
        case PACKAGE_MODE_HELP: {
            println( "    <mode>        display help for given mode" );
            println( "    --manifest    display valid manifest formatting" );
            println( "    --file-types  display supported file types" );
        } break;
    }
    println("");
}

void print_manifest_format(void) {
    println( " - Utility Packager Manifest Format -----------------\n" );
    println( "Package manifest must be a UTF-8 text file with .manifest extension." );
    println( "Comments are lines that start with #" );
    println( "Manifest contains Resource descriptions that start");
    println( "with a valid C identifier followed by a colon." );
    println( "Resource fields follow the identifer with a leading tab or 4 spaces." );
    println( "All resources require a 'type' field and a 'path' field." );
    println( "The following is an example of a manifest with every resource type.\n" );
    println( "1: SOUND_EFFECT_0:" );
    println( "2: \ttype: audio" );
    println( "3: \tpath: \"./some/path/foo.wav\"" );
    println( "4: TEXT_0:" );
    println( "5: \tpath:        \"./some/other/path/test.txt\"" );
    println( "6: \ttype:        text" );
    println( "7: \tcompression: rle" );
    println("");
}
void print_file_types(void) {
    println( " - Utility Packager Supported File Types ------------\n" );
    println( "audio" );
    println( "  .wav - mono or stereo 16-bit PCM @ 48000hz" );
    println( "map" );
    println( "  .map - Liquid Engine map file" );
    println( "mesh" );
    println( "  .obj" );
    println( "text" );
    println( "  any text file with UTF-8 encoding" );
    println( "texture" );
    println( "  .bmp - 24-bit RGB, 32-bit RGBX, 32-bit RGBA" );
    println("");
}

PackageMode parse_package_mode( u64 arg_hash ) {
    switch( arg_hash ) {
        case HASH_ARG_MODE_CREATE: return PACKAGE_MODE_CREATE;
        case HASH_ARG_MODE_HELP:   return PACKAGE_MODE_HELP;
        case HASH_ARG_MODE_MODIFY: return PACKAGE_MODE_MODIFY;
        default: return PACKAGE_MODE_NONE;
    }
}

#include "package/generated_dependencies.inl"
