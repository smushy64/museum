/**
 * Description:  String hash generator.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 23, 2023
*/
#include "shared/defines.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/print.h"
#include "core/fs.h"
#include "core/time.h"
#include "core/rand.h"

typedef enum ErrorCode : int {
    HASH_SUCCESS = 0,
    HASH_ERROR_NO_ARGUMENTS = 128,
    HASH_ERROR_INVALID_ARGUMENT,
    HASH_ERROR_OUT_OF_MEMORY,
    HASH_ERROR_FILE_OPEN,
    HASH_ERROR_FILE_WRITE,
    HASH_ERROR_FILE_READ,
} ErrorCode;

internal void print_help(void);

#define hash_error( format, ... )\
    println_err( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )
#define hash_info( format, ... )\
    if( !is_silent )\
        println( format, ##__VA_ARGS__ )

#define HASH_DEFAULT_OUTPUT_PATH "./generated_hashes.h"
global const char* global_program_name = "lhash";

int main( int argc, char** argv ) {
    global_program_name = argv[0];

    if( argc <= 1 ) {
        hash_error( "no arguments provided!" );
        print_help();
        return HASH_ERROR_NO_ARGUMENTS;
    }

    PathSlice output_path = path_slice( HASH_DEFAULT_OUTPUT_PATH );
    PathSlice list_path   = {};
    const char** list_start  = NULL;
    int hash_count = 0;
    b32 is_silent  = false;

    StringSlice arg_output_path = string_slice( "--output-path" );
    StringSlice arg_help        = string_slice( "--help" );
    StringSlice arg_silent      = string_slice( "--silent" );
    StringSlice arg_list        = string_slice( "--list" );
    StringSlice arg_file        = string_slice( "--file" );

    for( int i = 1; i < argc; ++i ) {
        StringSlice arg = string_slice_from_cstr( 0, argv[i] );

        if( string_slice_cmp( arg, arg_output_path ) ) {
            if( i + 1 >= argc ) {
                hash_error( "--output-path must be followed by a path!" );
                print_help();
                return HASH_ERROR_INVALID_ARGUMENT;
            }
            arg = string_slice_from_cstr( 0, argv[++i] );

            output_path = reinterpret_cast( PathSlice, &arg );
            continue;
        } else if( string_slice_cmp( arg, arg_help ) ) {
            print_help();
            return HASH_SUCCESS;
        } else if( string_slice_cmp( arg, arg_silent ) ) {
            is_silent = true;
            continue;
        } else if( string_slice_cmp( arg, arg_file ) ) {
            if( i + 1 >= argc ) {
                hash_error( "--file must be followed by a path!" );
                print_help();
                return HASH_ERROR_INVALID_ARGUMENT;
            }
            arg = string_slice_from_cstr( 0, argv[++i] );

            list_path = reinterpret_cast( PathSlice, &arg );
            continue;
        } else if( string_slice_cmp( arg, arg_list ) ) {
            if( i + 1 >= argc ) {
                hash_error( "--list must be followed by a list!" );
                print_help();
                return HASH_ERROR_INVALID_ARGUMENT;
            }
            i++;

            int remaining_arguments = argc - i;
            if( remaining_arguments % 2 != 0 ) {
                hash_error( "--list requires a list of name string pairs!" );
                print_help();
                return HASH_ERROR_INVALID_ARGUMENT;
            }

            hash_count = remaining_arguments / 2;
            list_start = (const char**)(argv + i);

            break;
        }

        hash_error( "unrecognized argument '{s}'!", arg );
        print_help();
        return HASH_ERROR_INVALID_ARGUMENT;
    }

    if( !( list_start || list_path.buffer ) ) {
        hash_error( "no file path or list provided!" );
        print_help();
        return HASH_ERROR_INVALID_ARGUMENT;
    }

    if( fs_check_if_file_exists( output_path ) ) {
        fs_delete_file( output_path );
    }

    FileHandle* output_file = fs_file_open( output_path, FILE_OPEN_FLAG_WRITE );
    if( !output_file ) {
        hash_error( "failed to open output path '{s}'!", output_path );
        return HASH_ERROR_FILE_OPEN;
    }

    FileHandle* input_file = NULL;
    if( list_path.buffer ) {
        input_file = fs_file_open(
            list_path, FILE_OPEN_FLAG_READ | FILE_OPEN_FLAG_SHARE_ACCESS_READ );
        if( !input_file ) {
            hash_error( "failed to open list path '{s}'!", list_path );
            return HASH_ERROR_FILE_OPEN;
        }
    }

    #define output_write( format, ... ) do {\
        if( !fs_file_write_fmt( output_file, format "\n", ##__VA_ARGS__ ) ) {\
            hash_error( "failed to write to output file '{s}'!", output_path );\
            return HASH_ERROR_FILE_WRITE;\
        }\
    } while(0)

    TimeRecord time = time_record();
    rand_reset_global_state();
    u32 random_id = rand_xor_u32();

    output_write( "#if !defined( GENERATED_HASH_{u32}_H )", random_id );
    output_write( "#define GENERATED_HASH_{u32}_H", random_id );
    output_write( "/**" );
    output_write( " * Description:    Generated string hashes header." );
    output_write( " * Generated by:   Utility Hash" );
    output_write( " * File Generated: {cc} {u,02}, {u,04}",
        time_month_to_cstr( time.month ), time.day, time.year );
    output_write( "*/" );
    output_write( "#include \"shared/defines.h\"\n" );

    if( input_file ) {
        usize file_size   = fs_file_query_size( input_file );
        char* file_buffer = system_alloc( file_size );

        if( !file_buffer ) {
            hash_error( "failed to allocate {f,.2,m}!", (f64)file_size );
            return HASH_ERROR_OUT_OF_MEMORY;
        }
        if( !fs_file_read( input_file, file_size, file_buffer ) ) {
            hash_error( "failed to read file at path '{s}'!", list_path );
            return HASH_ERROR_FILE_READ;
        }

        fs_file_close( input_file );

        StringSlice file_contents = {};
        file_contents.buffer   = file_buffer;
        file_contents.len      = file_size;

        while( file_contents.len ) {
            StringSlice line = {};
            if( !string_slice_split_char( file_contents, '\n', &line, NULL ) ) {
                line.buffer = file_contents.buffer;
                line.len    = file_contents.len;
            }
            if( !line.len ) {
                file_contents.buffer++;
                file_contents.len--;
                continue;
            }
            if( line.len == 1 || line.buffer[0] == '\n' ) {
                goto skip_line;
            }

            StringSlice identifier = {};
            StringSlice string     = {};

            u64 hash = 0;

            string_slice_split_whitespace( line, &identifier, &string );
            if( !(identifier.len || string.len) ) {
                goto skip_line;
            }

            string = string_slice_trim_trailing_whitespace( string );

            if( !string.len ) {
                goto skip_line;
            }

            if( string.len > 2 ) {
                if( string.buffer[0] == '"' ) {
                    string.buffer++;
                    string.len--;
                }
                if( string.buffer[string.len - 1] == '"' ) {
                    string.len--;
                }
            }

            hash = string_slice_hash( string );
            output_write( "// \"{s}\"", string );
            output_write( "#define HASH_{s,u,30} ({u64}ULL)\n", identifier, hash );

skip_line:
            file_contents.buffer += line.len;
            file_contents.len    -= line.len;
        }
    } else {
        for( int i = 0; i < hash_count; ++i ) {
            const char* identifier = *((list_start + i) + 0);
            const char* string     = *((list_start + i) + 1);

            u64 hash = ___internal_hash( cstr_len( string ), string );

            output_write( "#define HASH_{cc,u} ({u64}ULL)", identifier, hash );
        }
    }
    output_write( "\n#endif /* header guard */" );

    hash_info( "generated hashes at path '{s}'", output_path );
    fs_file_close( output_file );
    return HASH_SUCCESS;
}

internal void print_help(void) {
    println( "OVERVIEW: Hash Generator\n" );
    println( "USAGE: {cc} <arguments>\n", global_program_name );
    println( "ARGUMENTS:" );
    println( "    --output-path <path>      change output path (default=" HASH_DEFAULT_OUTPUT_PATH ")" );
    println( "    --list [<name> <string>]  list of valid C identifiers followed by string to be hashed." );
    println( "    --file <path>             use a text file with each line containing a valid C identifier followed by string to be hashed." );
    println( "    --silent                  don't output messages to stdout (still outputs errors to stderr)" );
    println( "    --help                    print this message" );
}


