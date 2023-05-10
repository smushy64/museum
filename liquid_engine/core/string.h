#if !defined(STRING_HPP)
#define STRING_HPP
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

/// Calculate the length of a null-terminated string.
/// Result does not include null-terminator.
SM_API usize str_length( const char* string );
/// Calculate the length of a null-terminated wide string.
/// Result does not include null-terminator.
SM_API usize wstr_length( const wchar_t* string );

/// Concatenate null-terminated strings.
SM_API isize str_concat(
    const char* a,
    const char* b,
    usize dst_size,
    char* dst
);

/// Check if null-terminated strings are equal.
SM_API b32 str_cmp( const char* a, const char* b );

/// Check if character is a whitespace character
inline b32 is_whitespace( char character ) {
    return
        character == ' ' ||
        character == '\t';
}

/// Trim trailing whitespace from character buffer.
SM_API void str_trim_trailing_whitespace(
    isize buffer_size,
    char* string_buffer
);

/// format bytes.
/// determine if it should be represented as
/// bytes, kilobytes, megabytes or gigabytes and
/// format bytes into a string buffer
SM_API isize format_bytes(
    usize bytes,
    char* buffer,
    usize buffer_size
);

#endif
