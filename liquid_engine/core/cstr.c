// * Description:  C string function implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 05, 2023
#include "core/cstr.h"
#include "core/mem.h"
#include "platform.h"

LD_API usize cstr_len( const char* cstr ) {
    usize result = 0;
    if( cstr ) {
        while( *cstr++ ) {
            result++;
        }
    }
    return result;
}
LD_API b32 cstr_cmp( const char* a, const char* b ) {
    if( !a || !b ) {
        return false;
    }

    while( *a && *b ) {
        if( *a++ != *b++ ) {
            return false;
        }
        if( (*a && !*b) || (!*a && *b) ) {
            return false;
        }
    }

    return true;
}
LD_API void cstr_copy( char* dst, const char* src, usize len ) {
    usize src_len = cstr_len( src );
    usize max_len = len > src_len ? src_len : len;
    mem_copy( dst, src, max_len );
}
LD_API void cstr_output_stdout( const char* cstr ) {
    usize len = cstr_len( cstr );
    platform_write_console( platform_stdout_handle(), len, cstr );
}
LD_API void cstr_output_stderr( const char* cstr ) {
    usize len = cstr_len( cstr );
    platform_write_console( platform_stderr_handle(), len, cstr );
}

