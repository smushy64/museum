/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "string.h"

// TODO(alicia): custom format functions!
#include <stdio.h>

SM_API isize format_bytes(
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

    const char* format = "%5.2f B";
    switch( type ) {
        case IS_KB:
            format = "%5.2f KB";
            break;
        case IS_MB:
            format = "%5.2f MB";
            break;
        case IS_GB:
            format = "%5.2f GB";
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
