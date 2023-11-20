/**
 * Description:  Liquid Packager
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 05, 2023
*/
#include "defines.h"
#include "liquid_package.h"
#include "platform.h"
#include "core/string.h"
#include "core/collections.h"
// #define LD_MEMORY_NO_LOG
#include "core/memory.h"

#include "parse_manifest.h"
#include "shared_buffer.h"
#include "logging.h"
#include "internal/package_audio.h"
#include "internal/write_header.h"
#include "write_package.h"

struct PlatformAPI* platform = NULL;

typedef enum : u32 {
    PACKAGER_MODE_UNKNOWN,

    PACKAGER_MODE_CREATE,
    PACKAGER_MODE_HELP,
} PackagerMode;
internal force_inline
const char* packager_mode_to_cstr( PackagerMode mode ) {
    switch( mode ) {
        case PACKAGER_MODE_UNKNOWN: return "unknown";
        case PACKAGER_MODE_CREATE:  return "create";
        case PACKAGER_MODE_HELP:    return "help";
    }
}

typedef struct {
    PackagerMode mode;
    union {
        struct {
            usize max_threads;
            b8    is_silent;
            b8    is_verbose;
            b8    show_memory_usage;
            const char* output_path;
            const char* output_header_path;
            const char* enum_name;

            const char** manifest_paths_start;
            usize        manifest_path_count;
        } create;
    };
} ParsedArgumentResult;
internal
int parse_arguments( int argc, char** argv, ParsedArgumentResult* out_result );

void print_help(void);
void print_manifest_help(void);
void print_supported_resources(void);

#define PACKAGER_DEFAULT_HEADER_PATH    "./generated_header.h"
#define PACKAGER_DEFAULT_OUTPUT_PATH    "./package.lpkg"
#define PACKAGER_MINIMUM_MANIFEST_COUNT (5)
#define PACKAGER_MINIMUM_RESOURCE_COUNT (5)

#define PACKAGER_SUCCESS                     (0)
#define PACKAGER_ERROR_NO_ARGUMENTS          (128)
#define PACKAGER_ERROR_UNRECOGNIZED_ARGUMENT (129)
#define PACKAGER_ERROR_INVALID_ARGUMENT      (130)
#define PACKAGER_ERROR_NO_MANIFESTS          (131)
#define PACKAGER_ERROR_OUT_OF_MEMORY         (132)
#define PACKAGER_ERROR_WRITE_FILE            (133)
#define PACKAGER_ERROR_THREAD_SUBSYSTEM      (134)
#define PACKAGER_ERROR_MANIFEST_PARSE        (135)
c_linkage int core_init( int argc, char** argv, struct PlatformAPI* in_platform ) {
    platform = in_platform;
    logging_subsystem_initialize( NULL );
    logging_set_level( LOGGING_LEVEL_ERROR | LOGGING_LEVEL_WARN );

    void* thread_subsystem_buffer = system_alloc( THREAD_SUBSYSTEM_SIZE );
    if( !thread_subsystem_buffer ) {
        lp_error( "failed to initialize thread subsystem!" );
        return PACKAGER_ERROR_THREAD_SUBSYSTEM;
    }

    ParsedArgumentResult params = {};
    int parse_result = parse_arguments( argc, argv, &params );
    if( parse_result != PACKAGER_SUCCESS || params.mode == PACKAGER_MODE_HELP ) {
        return parse_result;
    }

    if( !params.create.is_silent ) {
        LoggingLevel level = LOGGING_LEVEL_ALL_NO_TRACE;
        if( params.create.is_verbose ) {
            level |= LOGGING_LEVEL_TRACE;
        }
        logging_set_level( level );
#if defined(LD_PLATFORM_WINDOWS)
        logging_set_output_debug_string_enabled( true );
#endif /* Platform Windows */
    }

    lp_note( "selected mode: {cc}", packager_mode_to_cstr( params.mode ) );
    lp_note( "thread count:  {usize}", params.create.max_threads );

    if(
        !params.create.manifest_paths_start ||
        !params.create.manifest_path_count
    ) {
        lp_error( "at least one manifest file is required!" );
        print_help();
        return PACKAGER_ERROR_NO_MANIFESTS;
    }

    if( !thread_subsystem_init(
        params.create.max_threads, thread_subsystem_buffer
    ) ) {
        lp_error( "failed to initialize thread subsystem!" );
        return PACKAGER_ERROR_THREAD_SUBSYSTEM;
    }

    List list_manifest_resources = {};
    list_manifest_resources.item_size = sizeof( ManifestResource );
    list_manifest_resources.capacity  = PACKAGER_MINIMUM_RESOURCE_COUNT;
    list_manifest_resources.count     = 0;
    list_manifest_resources.buffer    =
        system_alloc( list_manifest_resources.item_size * list_manifest_resources.capacity );
    if( !list_manifest_resources.buffer ) {
        lp_error(
            "failed to allocate {f,.2,b} for resources list!",
            (f64)( list_manifest_resources.item_size * list_manifest_resources.capacity ) );
        return PACKAGER_ERROR_OUT_OF_MEMORY;
    }

    StackAllocator manifest_data_stack = {}; {
        usize stack_size   = 256;
        void* stack_buffer = system_alloc( stack_size );

        if( !stack_buffer ) {
            lp_error(
                "failed to allocate {f,.2,b} for manifest data stack!",
                (f64)stack_size );
            return PACKAGER_ERROR_OUT_OF_MEMORY;
        }

        manifest_data_stack = stack_allocator_create( stack_size, stack_buffer );
    }

    if( !package_shared_buffer_initialize() ) {
        return PACKAGER_ERROR_OUT_OF_MEMORY;
    }

    for( usize i = 0; i < params.create.manifest_path_count; ++i ) {
        const char* manifest_path = params.create.manifest_paths_start[i];
        if( !packager_manifest_parse(
            &list_manifest_resources, &manifest_data_stack, manifest_path
        ) ) {
            return PACKAGER_ERROR_MANIFEST_PARSE;
        }
    }
    lp_note(
        "manifest data size: {f,.2,b} / {f,.2,b}",
        (f64)manifest_data_stack.current,
        (f64)manifest_data_stack.buffer_size );

    usize resource_count = list_manifest_resources.count;

    struct WriteHeaderParams write_header_params = {};
    write_header_params.finished                = semaphore_create();
    write_header_params.output_path             = params.create.output_header_path;
    write_header_params.list_manifest_resources = &list_manifest_resources;
    write_header_params.enum_name               = params.create.enum_name;

    usize write_package_params_size =
        sizeof( struct WritePackageParams ) * resource_count;
    struct WritePackageParams* write_package_params =
        system_alloc( write_package_params_size );
    if( !write_package_params ) {
        lp_error( "failed to allocate {f,.2,b}!", write_package_params_size );
        return PACKAGER_ERROR_OUT_OF_MEMORY;
    }

    read_write_fence();

    // kick off header writing.
    thread_work_queue_push( write_header, &write_header_params );

    // kick off package writing.
    for( usize i = 0; i < resource_count; ++i ) {
        struct WritePackageParams* write_params = write_package_params + i;

        Semaphore* finished_semaphore = semaphore_create();
        assert( finished_semaphore );

        write_params->list_manifest_resources = &list_manifest_resources;
        write_params->index                   = i;
        write_params->finished                = finished_semaphore;

        read_write_fence();
        thread_work_queue_push( write_package, write_params );
    }

    /* write header */ {
        PlatformFile* output_file = platform->io.file_open(
            PACKAGER_TMP_OUTPUT_PATH, PLATFORM_FILE_SHARE_WRITE | PLATFORM_FILE_WRITE );

        struct LiquidPackageHeader package_header = {};
        package_header.identifier     = LIQUID_PACKAGE_FILE_IDENTIFIER;
        package_header.resource_count = resource_count;

        platform->io.file_write(
            output_file, sizeof( package_header ), &package_header );
        platform->io.file_close( output_file );
    }

    // wait for packaging to finish
    for( usize i = 0; i < resource_count; ++i ) {
        struct WritePackageParams* current_params = write_package_params + i;
        semaphore_wait( current_params->finished );
        read_write_fence();

        semaphore_destroy( current_params->finished );
    }

    read_write_fence();

    system_free(
        write_package_params, sizeof( struct WritePackageParams ) * resource_count );

    if( platform->io.file_copy_by_path(
        params.create.output_path, PACKAGER_TMP_OUTPUT_PATH, false
    ) ) {
        platform->io.file_delete_by_path( PACKAGER_TMP_OUTPUT_PATH );
        lp_print( "created liquid package at path '{cc}'!", params.create.output_path );
    } else {
        lp_error( "failed to write to output path '{cc}'!", params.create.output_path );
    }

    read_write_fence();
    semaphore_wait( write_header_params.finished );

    if( params.create.show_memory_usage && !params.create.is_silent ) {
        lp_print( "total memory usage: {f,.2,b}", (f64)memory_query_total_usage() );
    }

    return PACKAGER_SUCCESS;
}

void print_help(void) {
    lp_print( "OVERVIEW: Liquid Engine Asset Packager\n" );
    lp_print( "USAGE: liquid-packager [mode] [options]\n" );
    lp_print( "MODES:" );
    lp_print( "create [paths >= 1] [options]  create a liquid engine package file using given manifests" );
    lp_print( "     [paths]                    paths to manifest files, they must be listed together (required)" );
    lp_print( "     --output [path]            set package output path (default='" PACKAGER_DEFAULT_OUTPUT_PATH "')" );
    lp_print( "     --header-output [path]     set header output path (default='" PACKAGER_DEFAULT_HEADER_PATH "')" );
    lp_print( "     --enum-name [c identifier] set name of resource id enum (default='" PACKAGER_DEFAULT_ENUM_NAME "')" );
    lp_print( "     --max-threads [uint >= 1]  limit maximum number of threads (default=system core count)" );
    lp_print( "     --silent                   don't print any messages that aren't errors" );
    lp_print( "                                takes precedence over other flags" );
    lp_print( "     --verbose                  print extra messages" );
    lp_print( "     --memory-usage             print total memory used" );
    lp_print( "help [options]     print either this message or other messages with the following flags" );
    lp_print( "     --file-types  print list supported file types" );
    lp_print( "     --manifest    print how to format a manifest file" );
}
void print_manifest_help(void) {
    lp_print( "Liquid Packager Manifest file." );
    lp_print( "Extension: .manifest" );
    lp_print( "Format:    text file" );
    lp_print( "the text 'liquid manifest' followed by new line must be at the top of the manifest file." );
    lp_print( "all empty lines are ignored." );
    lp_print( "all lines starting with '#' are considered comments and are ignored." );
    lp_print( "section headers can be specified with []." );
    lp_print( "specify a label and a path to a supported resource." );
    lp_print( "    to get a list of supported resources, run packager with --list-supported-resources flag." );
    lp_print( "label must be a valid C identifier as it will make up the id for that resource." );
    lp_print( "paths must be relative paths from the manifest's directory (ex: ./path/to/something)." );
    lp_print( "example:\n" );
    lp_print( "0: liquid manifest" );
    lp_print( "1: [audio]" );
    lp_print( "2: SOUND_EFFECT: ./path/to/audio.wav" );
    lp_print( "\nThe id for the resource labeled 'SOUND_EFFECT' is then RESOURCE_AUDIO_SOUND_EFFECT" );
}
void print_supported_resources(void) {
    lp_print( "Resources supported by Liquid Packager:" );
    lp_print( "[texture]" );
    lp_print( "  .bmp - 32bpp RGBA or RGBX" );
    lp_print( "       - 24bpp RGB" );
    lp_print( "[audio]" );
    lp_print( "  .wav - 16-bit PCM at 44.1Khz, mono or stereo only" );
    lp_print( "\nall unsupported resources are simply skipped." );
}

internal
int parse_arguments( int argc, char** argv, ParsedArgumentResult* out_result ) {
    assert( argc >= 0 );

    ParsedArgumentResult result = {};
    string_slice_const( token_mode_create, "create" );
    string_slice_const( token_mode_help, "help" );

    if( argc <= 1 ) {
        lp_error( "no arguments provided!" );
        print_help();
        return PACKAGER_ERROR_NO_ARGUMENTS;
    }

    // determine mode
    StringSlice argument = string_slice_from_cstr( 0, argv[1] );
    if( string_slice_cmp( &argument, &token_mode_create ) ) {
        result.mode = PACKAGER_MODE_CREATE;
    } else if( string_slice_cmp( &argument, &token_mode_help ) ) {
        result.mode = PACKAGER_MODE_HELP;
    }

    if( result.mode == PACKAGER_MODE_UNKNOWN ) {
        lp_error( "must provide a valid mode!" );
        print_help();
        return PACKAGER_ERROR_INVALID_ARGUMENT;
    }

    // initialize default options

    usize processor_count = 1;
    switch( result.mode ) {
        case PACKAGER_MODE_CREATE: {
            result.create.output_path        = PACKAGER_DEFAULT_OUTPUT_PATH;
            result.create.output_header_path = PACKAGER_DEFAULT_HEADER_PATH;
            result.create.enum_name          = PACKAGER_DEFAULT_ENUM_NAME;

            result.create.max_threads = 1;

            PlatformInfo* platform_info = platform->query_info();
            processor_count             = platform_info->logical_processor_count;
            result.create.max_threads   = processor_count;
        } break;

        case PACKAGER_MODE_HELP: {
            out_result->mode = PACKAGER_MODE_HELP;
        } break;

        // unreachable case
        case PACKAGER_MODE_UNKNOWN: panic();
    }

    #define check_for_following_arg( token, data ) do {\
        if( (i + 1) >= (usize)argc ) {\
            lp_error( "{s} must be followed by " data "!", token );\
            print_help();\
            return PACKAGER_ERROR_INVALID_ARGUMENT;\
        }\
    } while(0)

    for( usize i = 2; i < (usize)argc; ++i ) {
        argument = string_slice_from_cstr( 0, argv[i] );

        // parse mode argument
        if( argument.buffer[0] == '-' ) {
            switch( result.mode ) {
                case PACKAGER_MODE_CREATE: {
                    string_slice_const( token_create_output, "--output" );
                    string_slice_const( token_create_header_output, "--header-output" );
                    string_slice_const( token_create_enum_name, "--enum-name" );
                    string_slice_const( token_create_max_threads, "--max-threads" );
                    string_slice_const( token_create_silent, "--silent" );
                    string_slice_const( token_create_verbose, "--verbose" );
                    string_slice_const( token_create_memory_usage, "--memory-usage" );

                    if( string_slice_cmp( &argument, &token_create_output ) ) {
                        check_for_following_arg( token_create_output, "path" );
                        result.create.output_path = argv[++i];
                        continue;
                    }

                    if( string_slice_cmp( &argument, &token_create_header_output ) ) {
                        check_for_following_arg( token_create_header_output, "path" );
                        result.create.output_header_path = argv[++i];
                        continue;
                    }

                    if( string_slice_cmp( &argument, &token_create_enum_name ) ) {
                        check_for_following_arg(
                            token_create_enum_name, "valid c identifier" );
                        result.create.enum_name = argv[++i];
                        continue;
                    }

                    if( string_slice_cmp( &argument, &token_create_max_threads ) ) {
                        check_for_following_arg( token_create_max_threads, "uint >= 1" );
                        argument = string_slice_from_cstr( 0, argv[++i] );

                        u64 parsed_int = 0;
                        if( !string_slice_parse_uint( &argument, &parsed_int ) ) {
                            lp_error(
                                "{s} must be followed by an unsigned integer!",
                                token_create_max_threads );
                            print_help();
                            return PACKAGER_ERROR_INVALID_ARGUMENT;
                        }

                        if( parsed_int < 1 ) {
                            lp_error(
                                "{s} must be followed by an "
                                "unsigned integer greater than or equals to 1!",
                                token_create_max_threads );
                            lp_error( "parsed integer: {u64}", parsed_int );
                            print_help();
                            return PACKAGER_ERROR_INVALID_ARGUMENT;
                        }

                        if( parsed_int > processor_count ) {
                            lp_warn(
                                "{s} followed by integer greater than "
                                "the system's processor count!",
                                token_create_max_threads );
                        }

                        result.create.max_threads = parsed_int;
                        continue;
                    }

                    if( string_slice_cmp( &argument, &token_create_silent ) ) {
                        result.create.is_silent = true;
                        continue;
                    }

                    if( string_slice_cmp( &argument, &token_create_verbose ) ) {
                        result.create.is_verbose = true;
                        continue;
                    }

                    if( string_slice_cmp( &argument, &token_create_memory_usage ) ) {
                        result.create.show_memory_usage = true;
                        continue;
                    }
                } break;
                case PACKAGER_MODE_HELP: {
                    string_slice_const( token_help_file_types, "--file-types" );
                    string_slice_const( token_help_manifest, "--manifest" );

                    if( string_slice_cmp( &argument, &token_help_file_types ) ) {
                        print_supported_resources();
                        return PACKAGER_SUCCESS;
                    }

                    if( string_slice_cmp( &argument, &token_help_manifest ) ) {
                        print_manifest_help();
                        return PACKAGER_SUCCESS;
                    }
                } break;

                // unreachable case
                case PACKAGER_MODE_UNKNOWN: panic();
            }

            lp_error( "unrecognized argument '{s}'!", argument );
            print_help();
            return PACKAGER_ERROR_UNRECOGNIZED_ARGUMENT;
        }

        // if not -, then it's a path or invalid argument
        switch( result.mode ) {
            case PACKAGER_MODE_CREATE: {
                if( !result.create.manifest_paths_start ) {
                    result.create.manifest_paths_start = (const char**)(argv + i);
                }
                result.create.manifest_path_count++;
            } break;
            case PACKAGER_MODE_HELP: {
                lp_error( "unrecognized argument '{s}'!", argument );
                print_help();
            } return PACKAGER_ERROR_UNRECOGNIZED_ARGUMENT;

            // unreachable case
            case PACKAGER_MODE_UNKNOWN: panic();
        }
    }

    if( result.mode == PACKAGER_MODE_HELP ) {
        print_help();
    }

    *out_result = result;
    return PACKAGER_SUCCESS;
}

#include "packager_generated_dependencies.inl"

