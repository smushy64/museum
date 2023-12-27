#if !defined(LD_CORE_STRING_H)
#define LD_CORE_STRING_H
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "shared/defines.h"
#include "core/collections.h"

enum FormatInteger : u32;

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
/// Check if character is a latin alphabet letter.
header_only b32 char_is_latin_letter( char character ) {
    return
        ( character >= 'A' && character <= 'Z' ) ||
        ( character >= 'a' && character <= 'z' );
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

/// Hash a string.
CORE_API u64 ___internal_hash( usize len, const char* str );

/// Hash a string.
#define cstr_hash( string )\
    ___internal_hash( sizeof(string) - 1, string )

/// Calculate length of null-terminated C string.
CORE_API usize cstr_len( const char* cstr );
/// Compare two null-terminated C strings.
CORE_API b32 cstr_cmp( const char* a, const char* b );
/// Copy null-terminated string to character buffer.
/// Optionally takes in length of source string.
/// If no length is provided, calculates length of source string.
/// Destination must be able to hold source length.
CORE_API void cstr_copy(
    char* restricted dst, const char* restricted src, usize opt_src_len );
/// Copies overlapping null-terminated strings.
/// Optionally takes in length of source string.
/// If no length is provided, calculates length of source string.
/// Destination must be able to hold source length.
CORE_API void cstr_copy_overlapped(
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
    char variable_name##_buffer[cap] = {0};\
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
CORE_API Iterator string_slice_iterator( StringSlice* slice );
/// Create a string slice from null-terminated string buffer.
/// Optionally takes in length of the string buffer.
CORE_API StringSlice string_slice_from_cstr( usize opt_len, const char* cstr );
/// Returns true if slice is empty.
header_only b32 string_slice_is_empty( StringSlice* slice ) {
    return !slice->len;
}
/// Returns true if slice is full.
header_only b32 string_slice_is_full( StringSlice* slice ) {
    return slice->len == slice->capacity;
}
/// Create a hash for given string.
header_only u64 string_slice_hash( StringSlice* slice ) {
    return ___internal_hash( slice->len, slice->buffer );
}
/// Compare two string slices for equality.
CORE_API b32 string_slice_cmp( StringSlice* a, StringSlice* b );
/// Find phrase in string slice.
/// Returns true if phrase is found and sets out_index to result's index.
/// If out_index is NULL, just returns if phrase was found.
CORE_API b32 string_slice_find(
    StringSlice* slice, StringSlice* phrase, usize* opt_out_index );
/// Find a phrase in string slice.
/// Optionally get index of where first instance of phrase was found.
/// Counts how many times phrase appears in string.
/// Returns true if phrase was found.
CORE_API b32 string_slice_find_count(
    StringSlice* slice, StringSlice* phrase,
    usize* opt_out_first_index, usize* out_count );
/// Find character in string slice.
/// Returns true if character is found and sets out_index to result's index.
/// If out_index is NULL, just returns if character was found.
CORE_API b32 string_slice_find_char(
    StringSlice* slice, char character, usize* opt_out_index );
/// Find character in string slice.
/// Optionally get index of where first instance of character was found.
/// Counts how many times character appears in string.
/// Returns true if character was found.
CORE_API b32 string_slice_find_char_count(
    StringSlice* slice, char character,
    usize* opt_out_first_index, usize* out_count );
/// Find whitespace character in string slice.
/// Optionally get index of whitespace character.
/// Returns true if whitespace character was found.
CORE_API b32 string_slice_find_whitespace( StringSlice* slice, usize* opt_out_index );
/// Copy source string to destination.
/// Copies up to destination length or source length, whichever is less.
CORE_API void string_slice_copy_to_len( StringSlice* dst, StringSlice* src );
/// Copy source string to destination.
/// Copies up to destination capacity or source length, whichever is less.
CORE_API void string_slice_copy_to_capacity( StringSlice* dst, StringSlice* src );
/// Reverse contents of string slice.
CORE_API void string_slice_reverse( StringSlice* slice );
/// Trim leading whitespace.
/// Returns a string slice that starts after the first trailing whitespace character.
CORE_API void string_slice_trim_leading_whitespace(
    StringSlice* slice, StringSlice* out_trimmed );
/// Trim trailing whitespace.
/// Returns a string slice that stops before the first trailing whitespace character.
CORE_API void string_slice_trim_trailing_whitespace(
    StringSlice* slice, StringSlice* out_trimmed );
/// Set all characters in string slice to given character up to slice length.
CORE_API void string_slice_fill_to_len( StringSlice* slice, char character );
/// Set all characters in string slice to given character up to slice capacity.
/// Sets slice length to capacity.
CORE_API void string_slice_fill_to_capacity( StringSlice* slice, char character );
/// Changes all lower case ascii characters to upper case.
CORE_API void string_slice_to_upper( StringSlice* slice );
/// Changes all upper case ascii characters to lower case.
CORE_API void string_slice_to_lower( StringSlice* slice );
/// Clip a section from source string slice into dst string slice.
/// Returns false if from is greater than to or
/// if either is outside the bounds of the source string ( 0 to src->len ).
CORE_API b32 string_slice_clip(
    StringSlice* src,
    usize from_inclusive, usize to_exclusive,
    StringSlice* out_dst );
/// Push character to end of string.
/// Returns true if there was enough space in buffer.
CORE_API b32 string_slice_push( StringSlice* slice, char character );
/// Pop character off end of string.
/// Returns true if length is not 0.
CORE_API b32 string_slice_pop( StringSlice* slice, char* opt_out_character );
/// Pop character from the start of string.
/// Returns true if length is not 0.
/// Modifies buffer pointer.
CORE_API b32 string_slice_pop_start( StringSlice* slice, char* opt_out_character );
/// Insert character in string slice.
/// Returns true if there was enough space in buffer.
CORE_API b32 string_slice_insert_char( StringSlice* slice, usize index, char character );
/// Prepend string to string.
/// Returns true if destination buffer has enough space.
CORE_API b32 string_slice_prepend( StringSlice* slice, StringSlice* prepend );
/// Insert string in string at given index.
/// Returns true if destination buffer has enough space.
CORE_API b32 string_slice_insert( StringSlice* slice, usize index, StringSlice* insert );
/// Append string to end of string.
/// Returns true if destination buffer has enough space.
CORE_API b32 string_slice_append( StringSlice* slice, StringSlice* append );
/// Prepend string to string.
/// Returns true if destination buffer has enough space.
CORE_API b32 string_slice_prepend_cstr( StringSlice* slice, const char* prepend );
/// Insert string in string at given index.
/// Returns true if destination buffer has enough space.
CORE_API b32 string_slice_insert_cstr(
    StringSlice* slice, usize index, const char* insert );
/// Append string to end of string.
/// Returns true if destination buffer has enough space.
CORE_API b32 string_slice_append_cstr( StringSlice* slice, const char* append );
/// Split string in two at given index.
CORE_API void string_slice_split(
    StringSlice* slice, usize index, StringSlice* out_first, StringSlice* out_last );
/// Split string at first instance of given character.
CORE_API b32 string_slice_split_char(
    StringSlice* slice, char character, StringSlice* out_first, StringSlice* out_last );
/// Split string at first instance of whitespace.
/// Last slice starts at first non-whitespace character.
CORE_API b32 string_slice_split_whitespace(
    StringSlice* slice, StringSlice* out_first, StringSlice* out_last );
/// Parse an integer from given string.
CORE_API b32 string_slice_parse_int( StringSlice* slice, i64* out_integer );
/// Parse an unsigned integer from given string.
CORE_API b32 string_slice_parse_uint( StringSlice* slice, u64* out_integer );
/// Parse a float from given string.
CORE_API b32 string_slice_parse_float( StringSlice* slice, f64* out_float );

/// Output string slice to standard out.
#define string_slice_output_stdout( slice )\
    print_string_stdout( (slice)->len, (slice)->buffer )
/// Output string slice to standard error.
#define string_slice_output_stderr( slice )\
    print_string_stderr( (slice)->len, (slice)->buffer )

/// Write a formatted string to string slice using variadic list.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize ___internal_string_slice_fmt_va(
    StringSlice* slice, usize format_len, const char* format, va_list va );
/// Write a formatted string to string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize ___internal_string_slice_fmt(
    StringSlice* slice, usize format_len, const char* format, ... );

#define string_slice_fmt( slice, format, ... )\
    ___internal_string_slice_fmt( slice, sizeof(format), format, ##__VA_ARGS__ )
#define string_slice_fmt_va( slice, format, va )\
    ___internal_string_slice_fmt_va( slice, sizeof(format), format, va )

/// Write the value of boolean into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_bool( StringSlice* slice, b32 b, b32 binary );
/// Write the value of float into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_float( StringSlice* slice, f64 f, u32 precision );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_i8( StringSlice* slice, i8 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_u8( StringSlice* slice, u8 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_i16( StringSlice* slice, i16 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_u16( StringSlice* slice, u16 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_i32( StringSlice* slice, i32 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_u32( StringSlice* slice, u32 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_i64( StringSlice* slice, i64 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_u64( StringSlice* slice, u64 i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_isize(
    StringSlice* slice, isize i, enum FormatInteger format );
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
CORE_API usize string_slice_fmt_usize(
    StringSlice* slice, usize i, enum FormatInteger format );

#endif
