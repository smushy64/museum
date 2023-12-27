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
    HASH_ERROR_WRONG_ARGUMENT_COUNT,
    HASH_ERROR_OUT_OF_MEMORY,
    HASH_ERROR_FILE_OPEN,
    HASH_ERROR_FILE_WRITE,
} ErrorCode;

internal void print_help(void);

#define hash_error( format, ... )\
    println_err( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )

struct PlatformAPI;
c_linkage int core_init( int argc, char** argv, struct PlatformAPI* in_platform ) {
    unused( in_platform );

    if( argc <= 2 ) {
        hash_error( "no arguments provided!" );
        print_help();
        return HASH_ERROR_NO_ARGUMENTS;
    }
    if( argc % 2 != 0 ) {
        hash_error( "argument count must be divisible by 2!" );
        print_help();
        return HASH_ERROR_WRONG_ARGUMENT_COUNT;
    }
    const char* output_path = argv[1];

    if( fs_file_exists( output_path ) ) {
        fs_file_delete( output_path );
    }

    FSFile* output_file = fs_file_open( output_path, FS_FILE_WRITE );
    if( !output_file ) {
        hash_error( "failed to open file '{cc}'!", output_path );
        return HASH_ERROR_FILE_OPEN;
    }

    usize hashes_count = (argc - 2) / 2;
    usize hashes_size  = sizeof(u64) * hashes_count;
    u64* hashes = system_alloc( hashes_size );
    if( !hashes ) {
        hash_error( "failed to allocate {f,.2,m}!", (f64)hashes_size );
        return HASH_ERROR_OUT_OF_MEMORY;
    }

    usize index = 0;
    for( int i = 3; i < argc; i += 2 ) {
        hashes[index++] = ___internal_hash( cstr_len( argv[i] ), argv[i] );
    }
    assert( index == hashes_count );

    ErrorCode error_code = HASH_SUCCESS;

    #define hash_write( format, ... ) do {\
        if( !fs_file_write_fmt( output_file, format "\n", ##__VA_ARGS__ ) ) {\
            hash_error( "failed to write to output file '{cc}'!", output_path );\
            error_code = HASH_ERROR_FILE_WRITE;\
            goto hash_end;\
        }\
    } while(0)

    TimeRecord time = time_record();

    rand_reset_global_state();
    u32 random = rand_xor_u32();
    hash_write( "#if !defined( GENERATED_HASHES_{u32}_H )", random );
    hash_write( "#define GENERATED_HASHES_{u32}_H ", random );
    hash_write( "/**" );
    hash_write( " * Description:  Generated String hashes header." );
    hash_write( " * Author:       Liquid Engine Utility: hash" );
    hash_write( " * File Created: {cc} {u,02}, {u,04}",
        time_month_to_cstr( time.month ), time.day, time.year );
    hash_write( "*/" );
    hash_write( "#include \"shared/defines.h\"\n" );

    int variant_index = 2;
    for( usize i = 0; i < hashes_count; ++i ) {
        const char* variant_name = argv[variant_index];
        variant_index += 2;

        hash_write( "#define HASH_{cc,u} ({u64}ULL)",
            variant_name, hashes[i] );
    }

    hash_write( "\n#endif /* header guard */" );

hash_end:
    system_free( hashes, hashes_size );
    fs_file_close( output_file );

    if( !error_code ) {
        println( "hashes generated at path {cc}", output_path );
    }

    return error_code;
}

internal void print_help(void) {
    println( "OVERVIEW: Hash Generator\n" );
    println( "USAGE: hash <output path> [ <name> <string> ] ..." );
    println( "ARGUMENTS:" );
    println( "    <output path>  path to write hashes to." );
    println( "    <name>         valid C identifier that becomes name of hash value." );
    println( "                   must be followed by the string that will be hashed" );
    println( "    <string>       string that will be hashed." );
}

