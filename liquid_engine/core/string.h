#if !defined(LD_CORE_STRING_H)
#define LD_CORE_STRING_H
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include <defines.h>
#include "core/collections.h"

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

/// Check if character is a whitespace character.
header_only b32 char_is_whitespace( char character ) {
    return
        character == ' ' || character == '\n' ||
        character == '\r' || character == '\t';
}
/// Check if character is a decimal digit.
header_only b32 char_is_digit( char character ) {
    return character >= '0' && character <= '9';
}
/// Check if character is a hexadecimal digit.
header_only b32 char_is_digit_hexadecimal( char character ) {
    b32 upper = character >= 'A' && character <= 'F';
    b32 lower = character >= 'a' && character <= 'f';

    return char_is_digit( character ) || upper || lower;
}
/// Push character to stdout.
LD_API void char_output_stdout( char character );
/// Push character to stderr.
LD_API void char_output_stderr( char character );

/// Calculate length of null-terminated C string.
LD_API usize cstr_len( const char* cstr );
/// Compare two null-terminated C strings.
LD_API b32 cstr_cmp( const char* a, const char* b );
/// Copy null-terminated string to character buffer.
/// Optionally takes in length of source string.
/// If no length is provided, calculates length of source string.
/// Destination must be able to hold source length.
LD_API void cstr_copy(
    char* restricted dst, const char* restricted src, usize opt_src_len );
/// Copies overlapping null-terminated strings.
/// Optionally takes in length of source string.
/// If no length is provided, calculates length of source string.
/// Destination must be able to hold source length.
LD_API void cstr_copy_overlapped(
    char* dst, const char* src, usize opt_src_len );

/// Create a const string slice from literal.
#define string_slice_const( variable_name, literal )\
    StringSlice variable_name =\
        string_slice_from_cstr( sizeof( literal ) - 1, literal )
/// Create a mutable string slice from string literal.
/// Buffer is stack allocated so string slice is only valid in the current scope.
#define string_slice_mut( variable_name, literal )\
    char variable_name##_buffer[] = literal;\
    StringSlice variable_name = string_slice_from_cstr(\
        sizeof( variable_name##_buffer ) - 1, variable_name##_buffer )
/// Create a mutable string slice with given capacity.
/// Buffer is stack allocated so string slice is only valid in the current scope.
#define string_slice_mut_capacity( variable_name, cap )\
    char variable_name##_buffer[cap] = {};\
    StringSlice variable_name;\
    variable_name.buffer   = variable_name##_buffer;\
    variable_name.len      = 0;\
    variable_name.capacity = cap

/// Create a string slice.
/// The lifetime of the string slice is the lifetime
/// of the provided buffer.
header_only StringSlice string_slice( usize capacity, char* buffer ) {
    StringSlice result;
    result.buffer   = buffer;
    result.capacity = capacity;
    result.len      = 0;
    return result;
}
/// Clone a string slice.
header_only StringSlice string_slice_clone( StringSlice* slice ) {
    StringSlice result;
    result.buffer   = slice->buffer;
    result.capacity = slice->capacity;
    result.len      = slice->len;
    return result;
}
/// Create an iterator for a string slice.
LD_API Iterator string_slice_iterator( StringSlice* slice );
/// Create a string slice from null-terminated string buffer.
/// Optionally takes in length of the string buffer.
LD_API StringSlice string_slice_from_cstr( usize opt_len, const char* cstr );
/// Returns true if slice is empty.
header_only b32 string_slice_is_empty( StringSlice* slice ) {
    return !slice->len;
}
/// Returns true if slice is full.
header_only b32 string_slice_is_full( StringSlice* slice ) {
    return slice->len == slice->capacity;
}
/// Create a hash for given string.
LD_API u64 string_slice_hash( StringSlice* slice );
/// Compare two string slices for equality.
LD_API b32 string_slice_cmp( StringSlice* a, StringSlice* b );
/// Find phrase in string slice.
/// Returns true if phrase is found and sets out_index to result's index.
/// If out_index is NULL, just returns if phrase was found.
LD_API b32 string_slice_find(
    StringSlice* slice, StringSlice* phrase, usize* opt_out_index );
/// Find a phrase in string slice.
/// Optionally get index of where first instance of phrase was found.
/// Counts how many times phrase appears in string.
/// Returns true if phrase was found.
LD_API b32 string_slice_find_count(
    StringSlice* slice, StringSlice* phrase,
    usize* opt_out_first_index, usize* out_count );
/// Find character in string slice.
/// Returns true if character is found and sets out_index to result's index.
/// If out_index is NULL, just returns if character was found.
LD_API b32 string_slice_find_char(
    StringSlice* slice, char character, usize* opt_out_index );
/// Find character in string slice.
/// Optionally get index of where first instance of character was found.
/// Counts how many times character appears in string.
/// Returns true if character was found.
LD_API b32 string_slice_find_char_count(
    StringSlice* slice, char character,
    usize* opt_out_first_index, usize* out_count );
/// Find whitespace character in string slice.
/// Optionally get index of whitespace character.
/// Returns true if whitespace character was found.
LD_API b32 string_slice_find_whitespace( StringSlice* slice, usize* opt_out_index );
/// Copy source string to destination.
/// Copies up to destination length or source length, whichever is less.
LD_API void string_slice_copy_to_len( StringSlice* dst, StringSlice* src );
/// Copy source string to destination.
/// Copies up to destination capacity or source length, whichever is less.
LD_API void string_slice_copy_to_capacity( StringSlice* dst, StringSlice* src );
/// Reverse contents of string slice.
LD_API void string_slice_reverse( StringSlice* slice );
/// Trim leading whitespace.
/// Returns a string slice that starts after the first trailing whitespace character.
LD_API void string_slice_trim_leading_whitespace(
    StringSlice* slice, StringSlice* out_trimmed );
/// Trim trailing whitespace.
/// Returns a string slice that stops before the first trailing whitespace character.
LD_API void string_slice_trim_trailing_whitespace(
    StringSlice* slice, StringSlice* out_trimmed );
/// Set all characters in string slice to given character up to slice length.
LD_API void string_slice_fill_to_len( StringSlice* slice, char character );
/// Set all characters in string slice to given character up to slice capacity.
/// Sets slice length to capacity.
LD_API void string_slice_fill_to_capacity( StringSlice* slice, char character );
/// Push character to end of string.
/// Returns true if there was enough space in buffer.
LD_API b32 string_slice_push( StringSlice* slice, char character );
/// Pop character off end of string.
/// Returns true if length is not 0.
LD_API b32 string_slice_pop( StringSlice* slice, char* opt_out_character );
/// Insert character in string slice.
/// Returns true if there was enough space in buffer.
LD_API b32 string_slice_insert_char( StringSlice* slice, usize index, char character );
/// Prepend string to string.
/// Returns true if destination buffer has enough space.
LD_API b32 string_slice_prepend( StringSlice* slice, StringSlice* prepend );
/// Insert string in string at given index.
/// Returns true if destination buffer has enough space.
LD_API b32 string_slice_insert( StringSlice* slice, usize index, StringSlice* insert );
/// Append string to end of string.
/// Returns true if destination buffer has enough space.
LD_API b32 string_slice_append( StringSlice* slice, StringSlice* append );
/// Prepend string to string.
/// Returns true if destination buffer has enough space.
LD_API b32 string_slice_prepend_cstr( StringSlice* slice, const char* prepend );
/// Insert string in string at given index.
/// Returns true if destination buffer has enough space.
LD_API b32 string_slice_insert_cstr(
    StringSlice* slice, usize index, const char* insert );
/// Append string to end of string.
/// Returns true if destination buffer has enough space.
LD_API b32 string_slice_append_cstr( StringSlice* slice, const char* append );
/// Split string in two at given index.
LD_API void string_slice_split(
    StringSlice* slice, usize index, StringSlice* out_first, StringSlice* out_last );
/// Split string at first instance of given character.
LD_API b32 string_slice_split_char(
    StringSlice* slice, char character, StringSlice* out_first, StringSlice* out_last );
/// Split string at first instance of whitespace.
/// Last slice starts at first non-whitespace character.
LD_API b32 string_slice_split_whitespace(
    StringSlice* slice, StringSlice* out_first, StringSlice* out_last );
/// Parse an integer from given string.
LD_API b32 string_slice_parse_int( StringSlice* slice, i64* out_integer );
/// Parse an unsigned integer from given string.
LD_API b32 string_slice_parse_uint( StringSlice* slice, u64* out_integer );
/// Parse a float from given string.
LD_API b32 string_slice_parse_float( StringSlice* slice, f64* out_float );
/// Write a formatted string to string slice.
/// Returns additional space required if slice could not hold formatted string.
LD_API usize string_slice_fmt( StringSlice* slice, const char* format, ... );
/// Write a formatted string to string slice using variadic list.
/// Returns additional space required if slice could not hold formatted string.
LD_API usize string_slice_fmt_va(
    StringSlice* slice, const char* format, va_list variadic );
/// Format an integer into string slice.
LD_API usize string_slice_fmt_int(
    StringSlice* slice, i64 value, FormatInteger formatting );
/// Format an unsigned integer into string slice.
LD_API usize string_slice_fmt_uint(
    StringSlice* slice, u64 value, FormatInteger formatting );
/// Format a float into string slice.
LD_API usize string_slice_fmt_float(
    StringSlice* slice, f64 value, u32 precision );
/// Format a boolean into string slice.
LD_API usize string_slice_fmt_bool( StringSlice* slice, b32 value );
/// Output string slice to standard out.
LD_API void string_slice_output_stdout( StringSlice* slice );
/// Output string slice to standard error.
LD_API void string_slice_output_stderr( StringSlice* slice );

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
