#if !defined(CORE_STRING_HPP)
#define CORE_STRING_HPP
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"
#include "core/ldvariadic.h"

/// Push character to stdout.
LD_API void char_output_stdout( char character );
/// Push character to stderr.
LD_API void char_output_stderr( char character );
/// Check if character is a whitespace character
headerfn b32 char_is_whitespace( char character ) {
    return
        character == ' ' ||
        character == '\t';
}
/// Check if character is a digit 
headerfn b32 char_is_digit( char character ) {
    return character >= '0' && character <= '9';
}
/// Calculate string length
LD_API usize str_length( const char* str );
/// Fills a character buffer with given character
/// and appends a null-terminator at the end.
LD_API void str_buffer_fill( usize buffer_size, char* buffer, char character );
/// Output a null-terminated string to the console without formatting.
LD_API void str_output_stdout( const char* str );
/// Output a null-terminated string to the console without formatting.
LD_API void str_output_stderr( const char* str );

/// Dynamic heap allocated string that owns its buffer.
typedef struct {
    char* buffer;
    usize len;
    usize capacity;
} String;

/// View into existing string. Does not own its buffer.
typedef struct {
    union {
        const char* str;
        char* buffer;
    };
    usize len;
} StringView;
/// Create a string view into const char str
headerfn StringView sv_from_str( const char* str ) {
    StringView result = {};
    result.len = str_length( str );
    result.str = str;
    return result;
}
/// Create a string view into dynamic string
headerfn StringView sv_from_string( String* string ) {
    StringView result = {};
    result.len    = string->len;
    result.buffer = string->buffer;
    return result;
}
/// Output a string view to the console without formatting.
LD_API void sv_output_stdout( StringView sv );
/// Output a string view to the console without formatting.
LD_API void sv_output_stderr( StringView sv );
/// Compare if strings are equal.
LD_API b32 sv_cmp( StringView a, StringView b );
/// Compare if strings are equal.
LD_API b32 sv_cmp_string( StringView a, String* b );
/// Format string into string view.
/// Format specifiers are in docs/format.md
LD_API u32 sv_format( StringView buffer, const char* format, ... );
/// Format string into string view using variadic list.
/// Format specifiers are in docs/format.md
LD_API u32 sv_format_va(
    StringView buffer,
    const char* format,
    va_list variadic
);
/// Trim trailing whitespace from string view.
LD_API void sv_trim_trailing_whitespace( StringView string_view );
/// Find the first instance of a character in string view.
/// Returns -1 if character is not found.
LD_API isize sv_find_first_char( StringView s, char character );
/// Parse an int32 from string view
LD_API i32 sv_parse_i32( StringView s );
/// Parse a uint32 from string view
LD_API u32 sv_parse_u32( StringView s );
/// Check if string view contains a phrase
LD_API b32 sv_contains( StringView s, StringView phrase );
/// Copy contents from src to dst. Copies up to destination length.
LD_API void sv_copy( StringView src, StringView dst );
/// Clone string view
headerfn StringView sv_clone( StringView sv ) {
    StringView result;
    result.len    = sv.len;
    result.buffer = sv.buffer;
    return result;
}

#define SV( string ) sv_from_str( string )

/// Create new dynamic string from string view.
LD_API b32 dstring_new( StringView view, String* out_string );
/// Create new empty dynamic string with given capacity.
LD_API b32 dstring_with_capacity( usize capacity, String* out_string );
/// Reallocate string with given capacity.
/// Does nothing if given capacity is smaller than string's capacity.
LD_API b32 dstring_reserve( String* string, usize new_capacity );
/// Clear a string.
/// All this does is set the length of a string to zero.
/// It does not deallocate.
FORCE_INLINE void dstring_clear( String* string ) { string->len = 0; }
/// Append a string to existing string.
/// Alloc determines if will fill to the end of the existing buffer
/// or if it will reallocate.
LD_API b32 dstring_append_string( String* string, String* append, b32 alloc );
/// Append the contents of a string view to existing string.
/// Alloc determines if will fill to the end of the existing buffer
/// or if it will reallocate.
LD_API b32 dstring_append_sv( String* string, StringView append, b32 alloc );
/// Push a character onto end of string.
/// Realloc determines how much extra buffer bytes will be allocated if
/// character is at the end of string buffer.
/// 0 means that it will not realloc string.
/// Only returns false if reallocation fails, otherwise always returns true.
LD_API b32 dstring_push_char( String* string, char character, u32 realloc );
/// Pop last character in string.
/// Returns 0 if string len is zero.
LD_API char dstring_pop_char( String* string );
/// Make a string view into string that respects string capacity.
LD_API StringView dstring_view_capacity_bounds(
    String* string, usize offset
);
/// Make a string view into string that respects string length.
LD_API StringView dstring_view_len_bounds(
    String* string, usize offset
);
/// Free a dynamic string.
LD_API void dstring_free( String* string );

/// Print to stdout.
/// Format specifiers are in docs/format.md
LD_API void print( const char* format, ... );
/// Print to stderr.
/// Format specifiers are in docs/format.md
LD_API void printerr( const char* format, ... );
/// Print to stdout using variadic list.
/// Format specifiers are in docs/format.md
LD_API void print_va( const char* format, va_list variadic );
/// Print to stderr using variadic list.
/// Format specifiers are in docs/format.md
LD_API void printerr_va( const char* format, va_list variadic );

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
#define printlnerr( format, ... )\
    printerr( format, ##__VA_ARGS__ );\
    char_output_stderr( '\n' )

/// Print to stderr with a new line at the end.
/// Variadic arguments come from va_list.
#define printlnerr_va( format, variadic )\
    printerr_va( format, variadic );\
    char_output_stderr( '\n' )

#endif
