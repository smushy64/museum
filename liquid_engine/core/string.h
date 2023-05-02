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
SM_API usize string_length( const char* string );

/// Trim trailing whitespace from character buffer.
SM_API void string_trim_trailing_whitespace(
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
