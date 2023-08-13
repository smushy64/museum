// * Description:  String Processing Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "str.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

b32 str_contains( const char* string, const char* phrase ) {
    u32 string_len = strlen( string );
    u32 phrase_len = strlen( phrase );

    if( phrase_len > string_len ) {
        return false;
    }

    for( u32 i = 0; i < string_len; ++i ) {
        if( string[i] == phrase[0] ) {
            u32 remaining_string_len = string_len - phrase_len;
            if( phrase_len > remaining_string_len ) {
                return false;
            }
            b32 matches = true;
            for( u32 j = 0; j < phrase_len; ++j ) {
                if( string[i + j] != phrase[j] ) {
                    matches = false;
                    break;
                }
            }
            if( !matches ) {
                return false;
            }
        }
    }

    return true;
}

b32 str_to_i32( const char* string, i32* out_result ) {
    errno = 0;
    char* end;
    const int RADIX = 10;
    i32 result = strtol( string, &end, RADIX );
    if( errno == ERANGE ) {
        return false;
    }

    *out_result = result;

    return true;
}

