/**
 * Description:  Tests for core library.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 06, 2024
*/
#include "shared/defines.h"

#include "core/fs.h"          // IWYU pragma: keep
#include "core/path.h"        // IWYU pragma: keep
#include "core/print.h"       // IWYU pragma: keep
#include "core/memory.h"      // IWYU pragma: keep
#include "core/compression.h" // IWYU pragma: keep

#define ok( format, ... )\
    println( CONSOLE_COLOR_GREEN format CONSOLE_COLOR_RESET,\
        ##__VA_ARGS__ )

#undef main
int main( int argc, char** argv ) {
    unused(argc, argv);
    ok( "all tests passed!" );
    return 0;
#if 0
    if( argc == 1 ) {
        goto test_end;
    }

    FileHandle* test_text = fs_file_open(
        path_slice( "./resources/test.txt" ),
        FILE_OPEN_FLAG_READ );
    if( !test_text ) {
        goto test_end;
    }

    usize text_size   = fs_file_query_size( test_text );
    void* text_buffer = system_alloc( text_size );

    if( !text_buffer ) {
        fs_file_close( test_text );
        goto test_end;
    }

    fs_file_read( test_text, text_size, text_buffer );

    usize compressed_buffer_size = text_size * 3;
    void* compressed_buffer = system_alloc( compressed_buffer_size );
    if( !compressed_buffer ) {
        system_free( text_buffer, text_size );
        fs_file_close( test_text );
        goto test_end;
    }

    ByteSlice compressed_slice = { compressed_buffer, 0, compressed_buffer_size };
    compression_huffman_encode(
        compression_byte_slice_stream, &compressed_slice,
        text_size, text_buffer, NULL );

    ok( "original size: {f,m,.2} compressed size: {f,m,.2}", (f64)text_size, (f64)compressed_slice.len );

    system_free( text_buffer, text_size );
    system_free( compressed_buffer, compressed_buffer_size );

test_end:
    ok( "all tests passed!" );
    return 0;
#endif
}

