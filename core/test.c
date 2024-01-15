/**
 * Description:  Tests for core library.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 06, 2024
*/
#include "shared/defines.h"
#include "core/print.h"
#include "core/path.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/fs.h"

#define success( format, ... )\
    println( CONSOLE_COLOR_GREEN format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )
#define error( format, ... )\
    println( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )\

#define check( predicate, format, ... ) do {\
    if( !(predicate) ) {\
        error( macro_value_to_string( __LINE__ ) ": " #predicate " | " format, ##__VA_ARGS__ );\
        error_code = 1;\
        return;\
    }\
} while(0)

int error_code = 0;

void path_tests(void);

#undef main
int main( int argc, char** argv ) {
    unused(argc + argv);

    path_tests();

    if( !error_code ) {
        success( "all tests passed!" );
    }
    return error_code;
}

void path_tests(void) {
    /* path_slice_is_relative */ {
        PathSlice slice = path_slice( "foo.txt" );
        check( path_slice_is_relative( slice ), "path should be recognized as relative!" );
#if defined(LD_PLATFORM_WINDOWS)
        slice = path_slice( "C:/Documents/somethingidk" );
#else
        slice = path_slice( "/etc/dev/null" );
#endif
        check( !path_slice_is_relative( slice ), "path should be recognized as not relative!" );
        check( path_slice_is_absolute( slice ), "path should be recognized as absolute!" );
    }
    /* path slice is file/directory */ {
        PathSlice slice = path_slice( "../core/test.c" );
        check( path_slice_is_file( slice ), "path should be recognized as a file!" );
        slice = path_slice( "../core/" );
        check( !path_slice_is_file( slice ), "path should be recognized as a directory!" );
        check( path_slice_is_directory( slice ), "path should be recognized as a directory!" );
    }
    /* path_slice_get_parent */ {
        PathSlice slice = path_slice( "/foo/bar" );
        PathSlice parent = {};
        check( path_slice_get_parent( slice, &parent ), "path should have a parent! path: '{s}'", slice );
        StringSlice parent_string = path_slice_to_string( &parent );
        StringSlice expected = string_slice( "/foo" );
        check( string_slice_cmp( parent_string, expected ), "path: '{s}' parent: '{s}' expected: '{s}'", slice, parent_string, expected );

        slice = parent;
        check( path_slice_get_parent( slice, &parent ), "path should have a parent! path: '{s}'", slice );
        parent_string = path_slice_to_string( &parent );
        // expected = string_slice( "/" );
        // check( string_slice_cmp( parent_string, expected ), "path: '{s}' parent: '{s}' expected: '{s}'", slice, parent_string, expected );

        slice = parent;
        check( !path_slice_get_parent( slice, &parent ), "path should not have a parent! path: '{s}'", slice );
    }
    /* path_slice_ancestor_count */ {
        PathSlice slice = path_slice( "./foo/bar/baz" );
        usize expected  = 3;
        usize ancestor_count = path_slice_ancestor_count( slice );

        check( expected == ancestor_count, "path: '{s}' count: {usize} expected: {usize}", slice, ancestor_count, expected );
    }
    /* path_slice_fill_ancestors */ {
        PathSlice path = path_slice( "./foo/bar/baz" );
        #define PATH_ANCESTOR_COUNT (3)
        PathSlice ancestors[PATH_ANCESTOR_COUNT] = {};
        StringSlice expected_ancestors[PATH_ANCESTOR_COUNT] = {
            string_slice( "./foo/bar" ),
            string_slice( "./foo" ),
            string_slice( "." )
        };

        path_slice_fill_ancestors( path, PATH_ANCESTOR_COUNT, ancestors );
        for( usize i = 0; i < PATH_ANCESTOR_COUNT; ++i ) {
            StringSlice slice = path_slice_to_string(ancestors + i);
            StringSlice expected = expected_ancestors[i];
            check( string_slice_cmp( slice, expected ), "ancestor: '{s}' expected: '{s}'", slice, expected);
        }

        #undef PATH_ANCESTOR_COUNT
    }
    /* path_slice_get_file_name */ {
        PathSlice path = path_slice( "./foo/bar.txt" );
        PathSlice file_name = {};
        check( path_slice_get_file_name( path, &file_name ), "path should include a file name! path: '{s}'", path );

        StringSlice file_name_string   = path_slice_to_string( &file_name );
        StringSlice expected_file_name = string_slice( "bar.txt" );
        check( string_slice_cmp( file_name_string, expected_file_name ), "file name: '{s}' expected: '{s}'", file_name, expected_file_name );
    }
    /* path_slice_get_file_stem */ {
        PathSlice path = path_slice( "./foo/bar.txt" );
        PathSlice file_stem = {};
        check( path_slice_get_file_stem( path, &file_stem ), "path should include a file name! path: '{s}'", path );

        StringSlice file_stem_string   = path_slice_to_string( &file_stem );
        StringSlice expected_file_stem = string_slice( "bar" );
        check( string_slice_cmp( file_stem_string, expected_file_stem ), "file stem: '{s}' expected: '{s}'", file_stem, expected_file_stem );
    }
    /* path_slice_pop */ {
        PathSlice path = path_slice( "./foo/bar/baz" );
        PathSlice chunk = {};

        check( path_slice_pop( path, &path, &chunk ), "should have popped! path: '{s}'", path );
        StringSlice expected_path  = string_slice( "./foo/bar" );
        StringSlice expected_chunk = string_slice( "baz" );
        check( string_slice_cmp( path_slice_to_string( &path ), expected_path ), "path: '{s}' expected: '{s}'", path, expected_path );
        check( string_slice_cmp( path_slice_to_string( &chunk ), expected_chunk ), "chunk: '{s}' expected: '{s}'", chunk, expected_chunk );

        check( path_slice_pop( path, &path, &chunk ), "should have popped! path: '{s}'", path );
        expected_path  = string_slice( "./foo" );
        expected_chunk = string_slice( "bar" );
        check( string_slice_cmp( path_slice_to_string( &path ), expected_path ), "path: '{s}' expected: '{s}'", path, expected_path );
        check( string_slice_cmp( path_slice_to_string( &chunk ), expected_chunk ), "chunk: '{s}' expected: '{s}'", chunk, expected_chunk );

        check( path_slice_pop( path, &path, &chunk ), "should have popped! path: '{s}'", path );
        expected_path  = string_slice( "." );
        expected_chunk = string_slice( "foo" );
        check( string_slice_cmp( path_slice_to_string( &path ), expected_path ), "path: '{s}' expected: '{s}'", path, expected_path );
        check( string_slice_cmp( path_slice_to_string( &chunk ), expected_chunk ), "chunk: '{s}' expected: '{s}'", chunk, expected_chunk );
    }
    /* path_slice_convert_separators */ {
        PathSlice original = path_slice( "./foo/bar/baz" );
        StringSlice expected = string_slice( ".\\foo\\bar\\baz" );
        string_buffer_empty( converted, sizeof( "./foo/bar/baz" ) - 1 );

        usize result = path_slice_convert_separators(
            string_buffer_write, &converted, original, false );
        check( string_slice_cmp( expected, string_buffer_to_slice( &converted ) ), "separator did not convert properly! expected: '{s}' converted: '{s}'", expected, *(StringSlice*)&converted );
        check( result == 0, "separator converter output more characters than necessary!" );
    }
    /* path_buffer_push */ {
        #define _check()\
            check(\
                path_slice_cmp( &buffer, &expected ),\
                "expected: '{p}'({usize}) path: '{p}'({usize})",\
                expected, expected.len, path_buffer_to_slice( &buffer ), buffer.len )
            
        path_buffer_empty( buffer, 256 );

        path_buffer_push( &buffer, path_slice( "./" ) );
        PathSlice expected = path_slice( "./" );
        _check();

        path_buffer_push( &buffer, path_slice( "foo" ) );
        expected = path_slice( "./foo" );
        _check();

        path_buffer_push( &buffer, path_slice( "bar" ) );
        expected = path_slice( "./foo/bar" );
        _check();

        path_buffer_push( &buffer, path_slice( "baz" ) );
        expected = path_slice( "./foo/bar/baz" );
        _check();

        path_buffer_set_extension( &buffer, path_slice( "txt" ) );
        expected = path_slice( "./foo/bar/baz.txt" );
        _check();

        #undef _check
    }
    /* path_buffer_pop */ {
        path_buffer( buffer, "./foo/bar/baz" );
        PathSlice chunk = {};

        check( path_buffer_pop( &buffer, &chunk ), "should have popped! path: '{s}'", path_buffer_to_slice( &buffer ) );

        StringSlice buffer_slice   = *(StringSlice*)&buffer;
        StringSlice expected_path  = string_slice( "./foo/bar" );
        StringSlice expected_chunk = string_slice( "baz" );
        check( string_slice_cmp( buffer_slice, expected_path ), "path: '{s}' expected: '{s}'", buffer_slice, expected_path );
        check( string_slice_cmp( path_slice_to_string( &chunk ), expected_chunk ), "chunk: '{s}' expected: '{s}'", chunk, expected_chunk );

        check( path_buffer_pop( &buffer, &chunk ), "should have popped! path: '{s}'", path_buffer_to_slice( &buffer ));
        buffer_slice   = *(StringSlice*)&buffer;
        expected_path  = string_slice( "./foo" );
        expected_chunk = string_slice( "bar" );
        check( string_slice_cmp( buffer_slice, expected_path ), "path: '{s}' expected: '{s}'", buffer_slice, expected_path );
        check( string_slice_cmp( path_slice_to_string( &chunk ), expected_chunk ), "chunk: '{s}' expected: '{s}'", chunk, expected_chunk );

        check( path_buffer_pop( &buffer, &chunk ), "should have popped! path: '{s}'", path_buffer_to_slice( &buffer ));
        buffer_slice   = *(StringSlice*)&buffer;
        expected_path  = string_slice( "." );
        expected_chunk = string_slice( "foo" );
        check( string_slice_cmp( buffer_slice, expected_path ), "path: '{s}' expected: '{s}'", buffer_slice, expected_path );
        check( string_slice_cmp( path_slice_to_string( &chunk ), expected_chunk ), "chunk: '{s}' expected: '{s}'", chunk, expected_chunk );
    }
    /* path_buffer_set_extension */ {
        path_buffer( buffer_0, "./foo.txt" );
        StringSlice expected_0 = string_slice( "./foo.exe" );

        path_buffer_empty( buffer_1, 64 );
        memory_copy( buffer_1.buffer, "./bar", sizeof("./bar") - 1 );
        buffer_1.len = sizeof("./bar") - 1;
        StringSlice expected_1 = string_slice( "./bar.exe" );

        check( path_buffer_set_extension( &buffer_0, path_slice( "exe" ) ), "should have been able to set extension!" );
        check( path_buffer_set_extension( &buffer_1, path_slice( "exe" ) ), "should have been able to set extension!" );

        StringSlice buffer_slice = *(StringSlice*)&buffer_0;
        check( string_slice_cmp( buffer_slice, expected_0 ), "expected: '{s}' path: '{s}'", expected_0, buffer_slice );

        buffer_slice = *(StringSlice*)&buffer_1;
        check( string_slice_cmp( buffer_slice, expected_1 ), "expected: '{s}' path: '{s}'", expected_1, buffer_slice );
    }

}

