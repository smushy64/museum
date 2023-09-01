#if !defined(CORE_STRING_HPP)
#define CORE_STRING_HPP
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"
#include "core/ldvariadic.h"
#include "core/ldallocator.h"

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
/// Calculate null-terminated string length
LD_API usize str_length( const char* str );
/// Output a null-terminated string to stdout.
LD_API void str_output_stdout( const char* str );
/// Output a null-terminated string to stderr.
LD_API void str_output_stderr( const char* str );

/// Dynamic heap allocated string that owns its buffer.
typedef struct String {
    char* buffer;
    usize len;
    usize capacity;
} String;

/// View into existing string. Does not own its buffer.
typedef struct StringView {
    union {
        const char* str;
        char* buffer;
    };
    usize len;
} StringView;

/// Create a string view from C null-terminated string.
header_only StringView sv_from_str( const char* str ) {
    StringView result = {};
    result.len = str_length( str );
    result.str = str;
    return result;
}
/// Create a string view from dynamic string.
header_only StringView sv_from_string( String string ) {
    StringView result = {};
    result.len    = string.len;
    result.buffer = string.buffer;
    return result;
}
/// Output a string view to stdout.
LD_API void sv_output_stdout( StringView sv );
/// Output a string view to stderr.
LD_API void sv_output_stderr( StringView sv );
/// Compare string views for equality.
/// Returns true if string contents are equal and lengths are equal.
LD_API b32 sv_cmp( StringView a, StringView b );
/// Compare strings for equality.
/// Returns true if string contents are equal and lengths are equal.
LD_API b32 sv_cmp_string( StringView a, String* b );
/// Format string into string view buffer.
/// Returns required size for formatting.
/// Format specifiers are in docs/format.md
LD_API u32 sv_format( StringView sv, const char* format, ... );
/// Format string into string view buffer using variadic list.
/// Returns required size for formatting.
/// Format specifiers are in docs/format.md
LD_API u32 sv_format_va(
    StringView sv, const char* format, va_list variadic );
/// Trim trailing whitespace from string view.
LD_API void sv_trim_trailing_whitespace( StringView* sv );
/// Find the first instance of a character in string view.
/// Returns -1 if character is not found.
LD_API isize sv_find_first_char( StringView sv, char character );
/// Parse an int32 from string view buffer.
LD_API i32 sv_parse_i32( StringView sv );
/// Parse a uint32 from string view buffer.
LD_API u32 sv_parse_u32( StringView sv );
/// Returns true if string view contains the given phrase.
LD_API b32 sv_contains( StringView sv, StringView phrase );
/// Copy contents of src string view buffer to
/// dst string view buffer.
/// Copies only up to dst length.
LD_API void sv_copy( StringView src, StringView dst );
/// Set all characters in string view to given character.
LD_API void sv_fill( StringView sv, char character );
/// Clone string view
header_only StringView sv_clone( StringView sv ) {
    StringView result;
    result.len    = sv.len;
    result.buffer = sv.buffer;
    return result;
}
/// Make a string view from const char* 
#define SV( str ) sv_from_str( str )

/// Create new dynamic string from string view.
LD_API b32 dstring_new(
    Allocator* allocator, StringView sv, String* out_string );
/// Create new empty dynamic string with given capacity.
LD_API b32 dstring_with_capacity(
    Allocator* allocator, usize capacity, String* out_string );
/// Reallocate string with given capacity.
/// Does nothing if given capacity is smaller than string's capacity.
LD_API b32 dstring_reserve(
    Allocator* allocator, String* string, usize new_capacity );
/// Clear a string.
/// All this does is set the length of a string to zero.
/// It does not free string buffer.
header_only void dstring_clear( String* string ) { string->len = 0; }
/// Append the contents of a string view to existing string.
/// Alloc determines if will fill to the end of the existing buffer
/// or if it will reallocate.
LD_API b32 dstring_append(
    Allocator* allocator, String* string, StringView append, b32 alloc );
/// Push a character onto end of string.
/// Realloc determines how much extra buffer bytes will be allocated if
/// character is at the end of string buffer.
/// 0 means that it will not realloc string.
/// Only returns false if reallocation fails, otherwise always returns true.
LD_API b32 dstring_push_char(
    Allocator* allocator, String* string, char character, u32 realloc );
/// Pop last character in string.
/// Returns 0 if string length is zero.
LD_API char dstring_pop_char( String* string );
/// Make a string view into string that respects string capacity.
LD_API StringView dstring_view_capacity_bounds(
    String string, usize offset );
/// Make a string view into string that respects string length.
LD_API StringView dstring_view_len_bounds(
    String string, usize offset );
/// Free a dynamic string.
LD_API void dstring_free( Allocator* allocator, String* string );

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
