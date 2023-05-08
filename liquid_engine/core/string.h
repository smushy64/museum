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

/// Concatenate strings.
SM_API isize str_concat(
    const char* a,
    const char* b,
    usize dst_size,
    char* dst
);
/// Concatenate strings where dst overlaps with a or b.
/// Potentially a lot slower than str_concat so only use when necessary.
// SM_API isize str_overlap_concat(
//     const char* a,
//     const char* b,
//     usize dst_size,
//     char* dst
// );

/// Check if character is a whitespace character
SM_ALWAYS_INLINE b32 is_whitespace( char character ) {
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
