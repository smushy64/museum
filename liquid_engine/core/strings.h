#if !defined(CORE_STRING_HPP)
#define CORE_STRING_HPP
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"
#include "core/variadic.h"

/// Push character to stdout.
LD_API void char_output_stdout( char character );
/// Push character to stderr.
LD_API void char_output_stderr( char character );
/// Returns true if character is a whitespace character
header_only b32 char_is_whitespace( char character ) {
    return
        character == ' ' ||
        character == '\t';
}
/// Returns true if character is a digit.
header_only b32 char_is_digit( char character ) {
    return character >= '0' && character <= '9';
}

/// Options for formatting integers.
typedef enum : u32 {
    FMT_INT_DECIMAL,
    FMT_INT_BINARY,
    FMT_INT_HEX
} FormatInteger;

/// Slice of string buffer.
struct StringSlice {
    char* buffer;
    usize len;
    usize capacity;
};
/// Slice of string buffer.
typedef struct StringSlice StringSlice;

/// Create a mutable string slice from literal.
/// This is a bad attempt to get around the
/// limitations of the C language but whatever.
#define STRING( variable_name, literal )\
    char variable_name##_buffer[] = literal;\
    StringSlice variable_name = ss_from_cstr(\
        sizeof( variable_name##_buffer ) - 1, variable_name##_buffer )

/// Create a string slice.
/// The lifetime of the string slice is the lifetime
/// of the provided buffer.
header_only StringSlice ss( usize buffer_size, char* buffer ) {
    StringSlice result;
    result.buffer   = buffer;
    result.capacity = buffer_size;
    result.len      = 0;
    return result;
}
/// Clone a string slice.
header_only StringSlice ss_clone( StringSlice* slice ) {
    StringSlice result;
    result.buffer   = slice->buffer;
    result.capacity = slice->capacity;
    result.len      = slice->len;
    return result;
}
/// Create a string slice from null-terminated string buffer.
/// Optionally takes in length of the string buffer but
/// this can be left out as the function can calculate it.
/// It is recommended that you use the STRING macro to create
/// string slices from string literals as that will ensure that
/// the string slice can be mutated.
/// The lifetime of a string slice of a string literal is
/// up to the end of the scope.
LD_API StringSlice ss_from_cstr( usize opt_len, const char* cstr );
/// Returns true if slice is empty.
header_only b32 ss_is_empty( StringSlice* slice ) {
    return !slice->len;
}
/// Returns true of slice is full.
header_only b32 ss_is_full( StringSlice* slice ) {
    return slice->len == slice->capacity;
}
/// Create a hash for given string.
LD_API u64 ss_hash( StringSlice* slice );
/// Compare two string slices for equality.
LD_API b32 ss_cmp( StringSlice* a, StringSlice* b );
/// Find phrase in string slice.
/// Returns true if phrase is found and sets out_index to result's index.
/// If out_index is NULL, just returns if phrase was found.
LD_API b32 ss_find(
    StringSlice* slice, StringSlice* phrase, usize* opt_out_index );
/// Find character in string slice.
/// Returns true if character is found and sets out_index to result's index.
/// If out_index is NULL, just returns if character was found.
LD_API b32 ss_find_char(
    StringSlice* slice, char character, usize* opt_out_index );
/// Count how many times phrase appears in string slice.
LD_API usize ss_phrase_count( StringSlice* slice, StringSlice* phrase );
/// Count how many times character appears in string slice.
LD_API usize ss_char_count( StringSlice* slice, char character );
/// Copy the contents of src string slice up to
/// the capacity of dst string slice.
LD_API void ss_mut_copy( StringSlice* dst, StringSlice* src );
/// Copy the contents of src string slice up to
/// the len of dst string slice.
LD_API void ss_mut_copy_to_len( StringSlice* dst, StringSlice* src );
/// Copy the contents of src string up to
/// the capacity of dst string slice.
/// Optionally takes in length of the src string but
/// this can be left out as it can be calculated.
LD_API void ss_mut_copy_cstr(
    StringSlice* dst, usize opt_len, const char* src );
/// Copy the contents of src string up to
/// the len of dst string slice.
/// Optionally takes in length of the src string but
/// this can be left out as it can be calculated.
LD_API void ss_mut_copy_cstr_to_len(
    StringSlice* dst, usize opt_len, const char* src );
/// Reverse contents of string slice.
LD_API void ss_mut_reverse( StringSlice* slice );
/// Trim trailing whitespace.
/// Note: this does not change the buffer contents,
/// it just changes the slice's length.
LD_API void ss_mut_trim_trailing_whitespace( StringSlice* slice );
/// Set all characters in string slice to given character.
LD_API void ss_mut_fill( StringSlice* slice, char character );
/// Set all characters in string slice to given character.
/// Goes up to the capacity of string slice rather than len.
LD_API void ss_mut_fill_to_capacity( StringSlice* slice, char character );
/// Push character to end of string slice.
/// Returns true if slice had enough capacity to push character.
LD_API b32 ss_mut_push( StringSlice* slice, char character );
/// Insert character into string slice.
/// Returns true if slice had enough capacity to insert character.
LD_API b32 ss_mut_insert( StringSlice* slice, char character, usize position );
/// Append slice to end of slice.
/// Appends up to slice's capacity.
/// Returns true if full slice was appended.
LD_API b32 ss_mut_append( StringSlice* slice, StringSlice* append );
/// Write a formatted string to string slice.
LD_API usize ss_mut_fmt( StringSlice* slice, const char* format, ... );
/// Write a formatted string to string slice using variadic list.
LD_API usize ss_mut_fmt_va(
    StringSlice* slice, const char* format, va_list variadic );
/// Split string slice at given index.
LD_API void ss_split_at(
    StringSlice* slice_to_split, usize index,
    StringSlice* out_first, StringSlice* out_last );
/// Split string slice at first ocurrence of whitespace.
/// If none is found, returns false and out pointers are not modified.
LD_API b32 ss_split_at_whitespace(
    StringSlice* slice_to_split,
    StringSlice* out_first, StringSlice* out_last );
/// Attempt to parse integer from string slice.
/// Returns true if successful.
LD_API b32 ss_parse_int( StringSlice* slice, i64* out_integer );
/// Attempt to parse unsigned integer from string slice.
/// Returns true if successful.
LD_API b32 ss_parse_uint( StringSlice* slice, u64* out_integer );
/// Attempt to parse float from string slice.
/// Returns true if successful.
LD_API b32 ss_parse_float( StringSlice* slice, f64* out_float );
/// Output string slice to standard out.
LD_API void ss_output_stdout( StringSlice* slice );
/// Output string slice to standard error.
LD_API void ss_output_stderr( StringSlice* slice );
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_i8( StringSlice* slice, i8 value, FormatInteger fmt );
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_i16( StringSlice* slice, i16 value, FormatInteger fmt );
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_i32( StringSlice* slice, i32 value, FormatInteger fmt );
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_i64( StringSlice* slice, i64 value, FormatInteger fmt );
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_u8( StringSlice* slice, u8 value, FormatInteger fmt );
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_u16( StringSlice* slice, u16 value, FormatInteger fmt );
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_u32( StringSlice* slice, u32 value, FormatInteger fmt );
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_u64( StringSlice* slice, u64 value, FormatInteger fmt );
/// Format float into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_f32( StringSlice* slice, f32 value, u32 precision );
/// Format float into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_f64( StringSlice* slice, f64 value, u32 precision );
/// Format boolean into string slice.
/// Returns additional capacity required to fit value in slice.
LD_API usize ss_mut_fmt_b32( StringSlice* slice, b32 value );

/// Print to stdout.
/// Format specifiers are in docs/format.md
LD_API void print( const char* format, ... );
/// Print to stderr.
/// Format specifiers are in docs/format.md
LD_API void print_err( const char* format, ... );
/// Print to stdout using variadic list.
/// Format specifiers are in docs/format.md
LD_API void print_va( const char* format, va_list variadic );
/// Print to stderr using variadic list.
/// Format specifiers are in docs/format.md
LD_API void print_err_va( const char* format, va_list variadic );

/// Print to stdout with a new line at the end.
#define println( format, ... )\
    print( format, ##__VA_ARGS__ );\
    char_output_stdout( '\n' )

/// Print to stdout with a new line at the end.
/// Variadic arguments come from va_list.
#define println_va( format, variadic )\
    print_va( format, variadic );\
    char_output_stdout( '\n' )

/// Print to stderr with a new line at the end.
#define println_err( format, ... )\
    print_err( format, ##__VA_ARGS__ );\
    char_output_stderr( '\n' )

/// Print to stderr with a new line at the end.
/// Variadic arguments come from va_list.
#define println_err_va( format, variadic )\
    print_err_va( format, variadic );\
    char_output_stderr( '\n' )

#endif
