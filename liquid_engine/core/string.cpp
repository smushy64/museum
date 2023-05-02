/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "string.h"
#include "logging.h"

// TODO(alicia): custom format functions!
#include <stdio.h>

usize string_length( const char* string ) {
    usize result = 0;

    if( *string ) {
        do {
            result++;
        } while( *string++ );
    }

    return result;
}

SM_INTERNAL b32 is_whitespace( char character ) {
    return
        character == ' ' ||
        character == '\t';
}

void string_trim_trailing_whitespace(
    isize buffer_size,
    char* string_buffer
) {
    for( isize i = buffer_size - 1; i > 0; --i ) {
        char current = string_buffer[i];
        if(
            !is_whitespace(current) &&
            current != 0
        ) {
            isize index = i + 1;
            if( index >= buffer_size ) {
                return;
            }
            string_buffer[i + 1] = 0;

            return;
        }
    }
}

isize format_bytes(
    usize bytes,
    char* buffer,
    usize buffer_size
) {

    #define IS_BYTES 0
    #define IS_KB    1
    #define IS_MB    2
    #define IS_GB    3

    int type = IS_BYTES;
    f32 bytes_f32 = (f32)bytes;
    if( bytes_f32 >= 1024.0f ) {
        bytes_f32 = BYTES_TO_KB( bytes_f32 );
        type = IS_KB;
        if( bytes_f32 >= 1024.0f ) {
            bytes_f32 = KB_TO_MB( bytes_f32 );
            type = IS_MB;
            if( bytes_f32 >= 1024.0f ) {
                bytes_f32 = MB_TO_GB( bytes_f32 );
                type = IS_GB;
            }
        }
    }

    const char* format = "%10.3f B";
    switch( type ) {
        case IS_KB:
            format = "%10.3f KB";
            break;
        case IS_MB:
            format = "%10.3f MB";
            break;
        case IS_GB:
            format = "%10.3f GB";
            break;
        default: break;
    }

    return snprintf(
        buffer,
        buffer_size,
        format,
        bytes_f32
    );
}
