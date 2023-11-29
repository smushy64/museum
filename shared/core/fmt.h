#if !defined(LD_SHARED_CORE_FMT_H)
#define LD_SHARED_CORE_FMT_H
/**
 * Description:  String formatting
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 25, 2023
*/
#include "defines.h"

/// Supported integer formats.
typedef enum FormatInteger : u32 {
    FORMAT_INTEGER_DECIMAL,
    FORMAT_INTEGER_BINARY,
    FORMAT_INTEGER_HEXADECIMAL_LOWER,
    FORMAT_INTEGER_HEXADECIMAL_UPPER,
} FormatInteger;

/// Formatting write function.
/// Target is a pointer to where-ever the formatted string should be written to.
/// Returns 0 if successful, otherwise returns number of characters
/// that could not be written.
typedef usize FormatWriteFN( void* target, usize count, char* characters );

/// Parse integer from string.
LD_API b32 fmt_read_int( usize len, char* buffer, i64* out_parsed_int );
/// Parse unsigned integer from string.
LD_API b32 fmt_read_uint( usize len, char* buffer, u64* out_parsed_int );

/// Write boolean to a target.
/// Returns number of bytes necessary to complete write operation
/// if target is not large enough.
LD_API usize fmt_write_bool(
    FormatWriteFN* write, void* target, b32 b, b32 binary );
/// Write floating point number to a target.
/// Returns number of bytes necessary to complete write operation
/// if target is not large enough.
LD_API usize fmt_write_float(
    FormatWriteFN* write, void* target, f64 f, u32 precision );
/// Write 8-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_i8(
    FormatWriteFN* write, void* target, i8 integer, FormatInteger format );
/// Write 8-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_u8(
    FormatWriteFN* write, void* target, u8 integer, FormatInteger format );
/// Write 16-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_i16(
    FormatWriteFN* write, void* target, i16 integer, FormatInteger format );
/// Write 16-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_u16(
    FormatWriteFN* write, void* target, u16 integer, FormatInteger format );
/// Write 32-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_i32(
    FormatWriteFN* write, void* target, i32 integer, FormatInteger format );
/// Write 32-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_u32(
    FormatWriteFN* write, void* target, u32 integer, FormatInteger format );
/// Write 64-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_i64(
    FormatWriteFN* write, void* target, i64 integer, FormatInteger format );
/// Write 64-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize fmt_write_u64(
    FormatWriteFN* write, void* target, u64 integer, FormatInteger format );
/// Write pointer sized integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
header_only usize fmt_write_isize(
    FormatWriteFN* write, void* target, isize integer, FormatInteger format
) {
#if defined(LD_ARCH_64_BIT)
    return fmt_write_i64( write, target, integer, format );
#else
    return fmt_write_i32( write, target, integer, format );
#endif
}
/// Write pointer sized unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
header_only usize fmt_write_usize(
    FormatWriteFN* write, void* target, usize integer, FormatInteger format
) {
#if defined(LD_ARCH_64_BIT)
    return fmt_write_u64( write, target, integer, format );
#else
    return fmt_write_u32( write, target, integer, format );
#endif
}

/// Write formatted string to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize ___internal_fmt_write(
    FormatWriteFN* write, void* target,
    usize format_len, const char* format, ... );
/// Write formatted string to a target using variadic list.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
LD_API usize ___internal_fmt_write_va(
    FormatWriteFN* write, void* target,
    usize format_len, const char* format, va_list va );

/// Write formatted string to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
#define fmt_write( write, target, format, ... )\
    ___internal_fmt_write( write, target, sizeof(format), format, ##__VA_ARGS__ )

/// Write formatted string to a target using variadic arguments.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
#define fmt_write_va( write, target, format, va )\
    ___internal_fmt_write_va( write, target, sizeof(format), format, va )

#endif /* header guard */
