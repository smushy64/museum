#if !defined(LD_CORE_CSTR_H)
#define LD_CORE_CSTR_H
// * Description:  C string functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 05, 2023
// * Notes:        It is recommended that you use the string view
// *               family of functions instead as those functions
// *               make use of string view bounds checking.
// *               This header exists just to have C string functions
// *               available if necessary.
#include "defines.h"

/// Calculate null-terminated string length.
LD_API usize cstr_len( const char* cstr );
/// Compare null-terminated strings.
LD_API b32 cstr_cmp( const char* a, const char* b );
/// Copy source to destination string.
LD_API void cstr_copy( char* dst, const char* src, usize len );
/// Output a null-terminated string to standard out.
LD_API void cstr_output_stdout( const char* cstr );
/// Output a null-terminated string to standard error.
LD_API void cstr_output_stderr( const char* cstr );

#endif // header guard
