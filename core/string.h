#if !defined(LD_CORE_STRING_H)
#define LD_CORE_STRING_H
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "shared/defines.h"
#include "core/internal/slice.h"

struct Iterator;
enum FormatInteger : u32;

/// Slice of a string buffer.
typedef struct StringSlice StringSlice;
/// String buffer.
typedef struct StringBuffer StringBuffer;

/// Convert String Buffer to String Slice.
#define string_buffer_to_slice( buffer )\
    (*(StringSlice*)(buffer))

/// Create a constant String Slice from literal.
#define string_slice( literal )\
    (StringSlice){ literal, sizeof(literal) - 1 }

/// Create a stack-allocated mutable String Buffer from literal.
#define string_buffer_text( buffer_name, literal )\
    char buffer_name##_buffer[] = literal;\
    StringBuffer buffer_name = (StringBuffer){ buffer_name##_buffer, sizeof(literal) - 1, sizeof(literal) }
/// Create a stack-allocated mutable String Buffer with capacity.
#define string_buffer_empty( buffer_name, capacity )\
    char buffer_name##_buffer[capacity] = {};\
    StringBuffer buffer_name = (StringBuffer){ buffer_name##_buffer, 0, capacity }

/// Check if character is a whitespace character.
header_only b32 char_is_whitespace( char character ) {
    return
        character == ' '  || character == '\n' ||
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
CORE_API u64 cstr_hash( usize opt_len, const char* string );
/// Hash a string literal.
#define text_hash( string )\
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

/// Create a String Slice from null-terminated string.
/// Optionally provide length to skip calculating it.
header_only StringSlice string_slice_from_cstr( usize opt_len, const char* cstr ) {
    StringSlice result = {};
    result.buffer = (char*)cstr;
    result.len    = opt_len ? opt_len : cstr_len( cstr );
    return result;
}
/// Returns true if slice is empty.
header_only b32 string_slice_is_empty( StringSlice slice ) {
    return slice.len == 0;
}
/// Generate hash for given String Slice.
header_only u64 string_slice_hash( StringSlice slice ) {
    return cstr_hash( slice.len, slice.buffer );
}
/// Create an iterator from String Slice.
CORE_API void string_slice_iterator( StringSlice slice, struct Iterator* out_iter );
/// Compare two string slices for equality.
CORE_API b32 string_slice_cmp( StringSlice a, StringSlice b );
/// Search for phrase in provided String Slice.
/// Returns true if phrase is found and if opt_out_index is not null,
/// provides index of the first character of phrase in slice.
CORE_API b32 string_slice_find(
    StringSlice slice, StringSlice phrase, usize* opt_out_index );
/// Search for phrase in provided String Slice.
/// Returns true if phrase is found and if opt_out_first_index is not null,
/// provides index of the first instance of phrase in slice.
/// If phrase is found, also provides number of times found in out_count.
CORE_API b32 string_slice_find_count(
    StringSlice slice, StringSlice phrase,
    usize* opt_out_first_index, usize* out_count );
/// Search for first instance of character in String Slice.
/// Returns true if character is found and if opt_out_index is not null,
/// provides the index of the found character in slice.
CORE_API b32 string_slice_find_char(
    StringSlice slice, char character, usize* opt_out_index );
/// Search for character in String Slice.
/// Returns true if character is found and if opt_out_first_index is not null,
/// provides the index of the found character in slice.
/// If character is found, also provides number of times found in out_count.
CORE_API b32 string_slice_find_char_count(
    StringSlice slice, char character,
    usize* opt_out_first_index, usize* out_count );
/// Search for first instance of a whitespace character in String Slice.
/// Returns true if whitespace is found.
/// If opt_out_index is not null, provides index of first instance of whitespace.
CORE_API b32 string_slice_find_whitespace( StringSlice slice, usize* opt_out_index );
/// Reverse contents of String Slice.
CORE_API void string_slice_reverse( StringSlice slice );
/// Trim leading whitespace from String Slice.
/// Returns trimmed String Slice.
CORE_API StringSlice string_slice_trim_leading_whitespace( StringSlice slice );
/// Trim trailing whitespace from String Slice.
/// Returns trimmed String Slice.
CORE_API StringSlice string_slice_trim_trailing_whitespace( StringSlice slice );
/// Trim leading and trailing whitespace from StringSlice.
/// Returns trimmed String Slice.
header_only StringSlice string_slice_trim_whitespace( StringSlice slice ) {
    StringSlice result = slice;
    result = string_slice_trim_leading_whitespace( result );
    return string_slice_trim_trailing_whitespace( result );
}
/// Set all characters in String Slice to provided character.
CORE_API void string_slice_set( StringSlice slice, char character );
/// Convert all lower case ASCII characters to upper case.
CORE_API void string_slice_to_upper( StringSlice slice );
/// Convert all upper case ASCII characters to lower case.
CORE_API void string_slice_to_lower( StringSlice slice );
/// Clip out slice from String Slice from index to index.
/// Range provided must be within the bounds of the slice.
CORE_API StringSlice string_slice_clip(
    StringSlice slice, usize from_inclusive, usize to_exclusive );
/// Pop last character from String Slice.
/// Returns true if successful.
/// Writes character to opt_out_character if successful.
CORE_API b32 string_slice_pop(
    StringSlice slice, StringSlice* out_slice, char* opt_out_character );
/// Pop first character from String Slice.
/// Returns true if successful.
/// Writes character to opt_out_character if successful.
CORE_API b32 string_slice_pop_start(
    StringSlice slice, StringSlice* out_slice, char* opt_out_character );
/// Split String Slice at given index.
/// Writes from start to index into opt_out_first.
/// Writes from index + 1 to len into opt_out_last.
CORE_API void string_slice_split(
    StringSlice slice, usize index,
    StringSlice* opt_out_first, StringSlice* opt_out_last );
/// Split String Slice at first instance of given character.
/// Writes from start to character index into opt_out_first.
/// Writes from character index + 1 to len into opt_out_last.
/// Returns true if character was found.
/// If character was not found, slice is not split.
CORE_API b32 string_slice_split_char(
    StringSlice slice, char character,
    StringSlice* opt_out_first, StringSlice* opt_out_last );
/// Split String Slice at first instance of whitespace.
/// Writes from start to whitespace index into opt_out_first.
/// Writes from first non-whitespace character to len into opt_out_last.
/// Returns true if whitespace was found.
/// If whitespace was not found, slice is not split.
CORE_API b32 string_slice_split_whitespace(
    StringSlice slice, StringSlice* opt_out_first, StringSlice* opt_out_last );
/// Parse a signed integer from String Slice.
/// Returns true if parse was successful.
CORE_API b32 string_slice_parse_int( StringSlice slice, i64* out_integer );
/// Parse a unsigned integer from String Slice.
/// Returns true if parse was successful.
CORE_API b32 string_slice_parse_uint( StringSlice slice, u64* out_integer );
/// Parse a float from String Slice.
/// Returns true if parse was successful.
CORE_API b32 string_slice_parse_float( StringSlice slice, f64* out_float );

/// Output string slice to standard out.
#define string_slice_output_stdout( slice )\
    print_string_stdout( (slice).len, (slice).buffer )
/// Output string slice to standard error.
#define string_slice_output_stderr( slice )\
    print_string_stderr( (slice).len, (slice).buffer )

/// Create a String Buffer from provided buffer.
header_only StringBuffer string_buffer( usize capacity, char* buffer ) {
    StringBuffer result;
    result.buffer   = buffer;
    result.capacity = capacity;
    result.len      = 0;

    return result;
}
/// Clear a String Buffer's content.
/// Does not zero-out content, just sets length to zero.
header_only void string_buffer_clear( StringBuffer* buffer ) {
    buffer->len = 0;
}
/// Returns true if String Buffer is full.
header_only b32 string_buffer_is_full( StringBuffer buffer ) {
    return buffer.len >= buffer.capacity;
}
/// Copy source String Slice to destination String Buffer.
/// Keeps copying up to destination capacity.
/// Overwrites previous destination length.
/// Returns 0 if was able to copy entire source string otherwise,
/// returns number of characters that were not copied.
CORE_API usize string_buffer_copy( StringBuffer* dst, StringSlice src );
/// Prepend source String Slice to destination String Buffer.
/// Returns true if destination has enough capacity to prepend source.
CORE_API b32 string_buffer_prepend( StringBuffer* dst, StringSlice src );
/// Append source String Slice to destination String Buffer.
/// Returns true if destination has enough capacity to append source.
CORE_API b32 string_buffer_append( StringBuffer* dst, StringSlice src );
/// Fill String Buffer with character.
/// Overwrites all characters with provided character up to buffer capacity.
/// Sets len == capacity.
CORE_API void string_buffer_fill( StringBuffer* buffer, char character );
/// Push character to the end of String Buffer.
/// Returns true if there was enough capacity to push new character.
CORE_API b32 string_buffer_push( StringBuffer* buffer, char character );
/// Pop character off end of String Buffer.
/// Returns true if buffer was not empty and if so,
/// writes popped character to opt_out_character.
CORE_API b32 string_buffer_pop( StringBuffer* buffer, char* opt_out_character );
/// Remove character from String Buffer.
/// Provided index must be within the bounds of the String Buffer.
CORE_API void string_buffer_remove(
    StringBuffer* buffer, usize index, char* opt_out_character );
/// Insert character into String Buffer.
/// Returns true if buffer had enough capacity to insert character.
CORE_API b32 string_buffer_insert(
    StringBuffer* buffer, usize index, char character );
/// Insert phrase into String Buffer.
/// Returns true if buffer had enough capacity to insert phrase.
CORE_API b32 string_buffer_insert_phrase(
    StringBuffer* buffer, usize index, StringSlice phrase );
/// Format write function for String Buffer.
/// Target pointer must point to a StringBuffer struct.
CORE_API usize string_buffer_write( void* target, usize count, char* characters );
/// Write a formatted string to String Buffer using variadic list.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize ___internal_string_buffer_fmt_va(
    StringBuffer* buffer, usize format_len, const char* format, va_list va );
/// Write a formatted string to String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize ___internal_string_buffer_fmt(
    StringBuffer* buffer, usize format_len, const char* format, ... );

#define string_buffer_fmt( buffer, format, ... )\
    ___internal_string_buffer_fmt( buffer, sizeof(format), format, ##__VA_ARGS__ )
#define string_buffer_fmt_va( buffer, format, va )\
    ___internal_string_buffer_fmt_va( buffer, sizeof(format), format, va )

/// Write the value of boolean into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_bool(
    StringBuffer* buffer, b32 b, b32 binary );
/// Write the value of float into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_float(
    StringBuffer* buffer, f64 f, u32 precision );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_i8(
    StringBuffer* buffer, i8 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_u8(
    StringBuffer* buffer, u8 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_i16(
    StringBuffer* buffer, i16 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_u16(
    StringBuffer* buffer, u16 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_i32(
    StringBuffer* buffer, i32 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_u32(
    StringBuffer* buffer, u32 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_i64(
    StringBuffer* buffer, i64 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_u64(
    StringBuffer* buffer, u64 i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_isize(
    StringBuffer* buffer, isize i, enum FormatInteger format );
/// Write the value of integer into String Buffer.
/// Returns number of bytes necessary to complete write operation if
/// String Buffer is not large enough.
CORE_API usize string_buffer_fmt_usize(
    StringBuffer* buffer, usize i, enum FormatInteger format );

#endif /* header guard */

