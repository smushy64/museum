#if !defined(STRING_HPP)
#define STRING_HPP
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

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
