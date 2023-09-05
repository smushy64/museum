// * Description:  C string function implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 05, 2023
#include "core/ldcstr.h"
#include "core/ldmemory.h"
#include "ldplatform.h"

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
    usize a_len = cstr_len( a );
    usize b_len = cstr_len( b );

    if( a_len != b_len ) {
        return false;
    }

    for( usize i = 0; i < a_len; ++i ) {
        if( a[i] != b[i] ) {
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

