/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "string.h"
#include "logging.h"
#include "memory.h"

// TODO(alicia): custom format functions!
#include <stdio.h>

usize str_length( const char* string ) {
    usize result = 0;

    if( *string ) {
        do {
            result++;
        } while( *string++ );
    }

    return result;
}

usize wstr_length( const wchar_t* string ) {
    usize result = 0;

    if( *string ) {
        do {
            result++;
        } while( *string++ );
    }

    return result;
}

internal isize str_concat_sized(
    usize a_length,
    const char* a,
    usize b_length,
    const char* b,
    usize dst_size,
    char* dst
) {
    SM_ASSERT( a && b && dst && dst_size );

    usize total_length = a_length + b_length + 1;
    
    usize remaining_dst_size = dst_size;

    if( a_length > remaining_dst_size ) {
        mem_copy( dst, a, remaining_dst_size );
        dst[dst_size - 1] = 0;
        return total_length - dst_size;
    }

    mem_copy( dst, a, a_length );
    remaining_dst_size -= a_length;

    if( b_length > remaining_dst_size ) {
        mem_copy(
            dst + a_length,
            b,
            remaining_dst_size
        );
        dst[dst_size - 1] = 0;
        return total_length - dst_size;
    }

    mem_copy(
        dst + a_length,
        b,
        b_length
    );

    remaining_dst_size -= b_length;

    dst[dst_size - 1] = 0;
    return --remaining_dst_size;
}

isize str_concat(
    const char* a,
    const char* b,
    usize dst_size,
    char* dst
) {
    return str_concat_sized(
        str_length(a),
        a,
        str_length(b),
        b,
        dst_size,
        dst
    );
}

b32 str_cmp( const char* a, const char* b ) {
    do {
        if( *a != *b ) {
            return false;
        }
    } while( *a++ && *b++ );

    return true;
}

void str_trim_trailing_whitespace(
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

void str_trim_trailing_newline(
    isize buffer_size,
    char* string_buffer
) {
    if( buffer_size < 2 ) {
        return;
    }
    if( string_buffer[buffer_size - 2] == '\n' ) {
        string_buffer[buffer_size - 2] = 0;
    }
}

const char* str_first_non_whitespace_pointer(
    usize buffer_size,
    const char* str_buffer
) {
    const char* result = str_buffer;
    for( usize i = 0; i < buffer_size; ++i ) {
        if( !is_whitespace( str_buffer[i] ) ) {
            result = &str_buffer[i];
            break;
        }
    }
    return result;
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
