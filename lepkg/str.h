#if !defined(LEPKG_STRING_HPP)
#define LEPKG_STRING_HPP
// * Description:  String Processing
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "defines.h"

b32 str_contains( const char* string, const char* phrase, usize* offset );
b32 str_to_i32( const char* string, i32* out_result );

#endif // header guard
