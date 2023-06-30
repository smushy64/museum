#if !defined(CORE_STRING_HPP)
#define CORE_STRING_HPP
/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

LD_API usize str_length( const char* str );

/// Dynamic heap allocated string that owns its buffer.
struct String {
    char* buffer;
    u32   len, capacity;
};

/// View into existing string. Does not own its buffer.
struct StringView {
    char* buffer;
    u32 len;

    StringView() : buffer(nullptr), len(0) {}
    StringView( const char* str ) : buffer((char*)str), len(str_length(str)) {}
    StringView( String& string ) : buffer(string.buffer), len(string.len) {}

    char& operator[]( u32 i ) { return buffer[i]; }
    char operator[]( u32 i ) const { return buffer[i]; }
};

/// Create new dynamic string from string view.
LD_API b32 dstring_new( StringView view, String* out_string );
/// Create new empty dynamic string with given capacity.
LD_API b32 dstring_with_capacity( u32 capacity, String* out_string );
/// Reallocate string with given capacity.
/// Does nothing if given capacity is smaller than string's capacity.
LD_API b32 dstring_reserve( String* string, u32 new_capacity );

/// Clear a string.
/// All this does is set the length of a string to zero.
/// It does not deallocate.
LD_ALWAYS_INLINE void dstring_clear( String* string ) {
    string->len = 0;
}

/// Append a string to existing string.
/// Alloc determines if will fill to the end of the existing buffer
/// or if it will reallocate.
LD_API b32 dstring_append( String* string, String* append, b32 alloc );
/// Append the contents of a string view to existing string.
/// Alloc determines if will fill to the end of the existing buffer
/// or if it will reallocate.
LD_API b32 dstring_append( String* string, StringView append, b32 alloc );

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
    String* string, u32 offset
);
/// Make a string view into string that respects string length.
LD_API StringView dstring_view_len_bounds(
    String* string, u32 offset
);

/// Free a dynamic string.
LD_API void dstring_free( String* string );

/// Compare if strings are equal.
LD_API b32 string_cmp( String* a, String* b );
/// Compare if strings are equal.
LD_API b32 string_cmp( String* a, StringView b );
/// Compare if strings are equal.
LD_API b32 string_cmp( StringView a, String* b );
/// Compare if strings are equal.
LD_API b32 string_cmp( StringView a, StringView b );

/// Parse an int32 from string
LD_API i32 string_parse_i32( StringView s );
/// Parse a uint32 from string
LD_API u32 string_parse_u32( StringView s );

/// Find the first instance of a character in string.
/// Returns -1 if character is not found.
LD_API i64 string_find_first_char( String* string, char character );
/// Find the first instance of a character in string view.
/// Returns -1 if character is not found.
LD_API i64 string_find_first_char( StringView string_view, char character );

/// Copy contents from src to dst. Copies up to destination length.
LD_API void string_copy( String* src, String* dst );
/// Copy contents from src to dst. Copies up to destination length.
LD_API void string_copy( StringView src, String* dst );

/// Trim trailing whitespace. Does not affect buffer capacity.
LD_API void string_trim_trailing_whitespace( String* string );
/// Trim trailing whitespace from view.
LD_API void string_trim_trailing_whitespace( StringView string_view );

/// Format a dstring.
/// Alloc determines if it will reallocate string to fit formatting.
/// Returns false only if reallocation fails.
LD_API b32 dstring_format(
    String* string,
    b32 alloc,
    const char* format,
    ...
);
/// Format a string view.
/// Returns required size if string view is empty.
LD_API u32 string_view_format(
    StringView string_view,
    const char* format,
    ...
);

/// Calculate the length of a null-terminated string.
/// Result does not include null-terminator.
LD_API usize str_length( const char* string );
/// Make a string view into const char*.
LD_API b32 str_view(
    u32 str_len,
    const char* str,
    u32 offset, u32 len,
    StringView* out_view
);
/// Fills a character buffer with given character
/// and appends a null-terminator at the end.
LD_API void str_buffer_fill( u32 buffer_size, char* buffer, char character );

/// Check if character is a whitespace character
inline b32 char_is_whitespace( char character ) {
    return
        character == ' ' ||
        character == '\t';
}
/// Check if character is a digit 
inline b32 char_is_digit( char character ) {
    return character >= '0' && character <= '9';
}

/// format bytes.
/// determine if it should be represented as
/// bytes, kilobytes, megabytes or gigabytes and
/// format bytes into a string buffer
LD_API isize format_bytes(
    usize bytes,
    char* buffer,
    usize buffer_size
);

/// Format string into string view.
/// Format Specifiers.
/// * use {} to wrap a format specifier and use commas to separate parameters
/// * c  - 8-bit ASCII character
/// * cc - const char* ASCII null terminated string
/// * s  - struct String
/// * sv - struct String View
/// * u or u8,u16,u32,u64 - unsigned int32 or sized uint
/// * i or i8,i16,i32,i64 - signed int32 or sized int
/// * b - boolean
/// * f or f32,f64 - float or sized float
/// * v2,v3,v4    - float vectors
/// * iv2,iv3,iv4 - int vectors
/// * q           - float quaternion
/// * {{ - literal {    .
/// Format parameters.
/// * int, iv2/3/4
///     - 0# | number of padding zeroes
///     - #  | number of padding spaces
///     - x  | hexadecimal format
///     - b  | binary format
/// * float, v2/3/4, m2/3/4, quaternion
///     - 0#  | number of padding zeroes
///     - #   | number of padding spaces
///     - .#  | fractional precision
/// * const char, string, string view
///     - #  | number of padding spaces
/// * boolean
///     - b | use 0/1 instead of true/false
/// * ex: format( buffer, "{f32,02.2} {i}", 1.235f, -10 )
/// * output: "01.23 -10"
LD_API u32 string_format( StringView buffer, const char* format, ... );
/// Format buffer variadic.
LD_API u32 string_format_va(
    StringView buffer,
    const char* format,
    va_list variadic
);
/// Print to stdout.
LD_API void print( const char* format, ... );
/// Print to stderr.
LD_API void printerr( const char* format, ... );
/// Print to stdout variadic.
LD_API void print_va( const char* format, va_list variadic );
/// Print to stderr variadic.
LD_API void printerr_va( const char* format, va_list variadic );

/// Push character to stdout.
LD_API void stdout_push( char character );
/// Push character to stderr.
LD_API void stderr_push( char character );

/// Print to stdout with a new line at the end.
#define println( format, ... )\
    print( format, ##__VA_ARGS__ );\
    stdout_push( '\n' )

/// Print to stdout with a new line at the end.
/// Variadic arguments come from va_list.
#define println_va( format, variadic )\
    print_va( format, variadic );\
    stdout_push( '\n' )

/// Print to stderr with a new line at the end.
#define printlnerr( format, ... )\
    printerr( format, ##__VA_ARGS__ );\
    stderr_push( '\n' )

/// Print to stderr with a new line at the end.
/// Variadic arguments come from va_list.
#define printlnerr_va( format, variadic )\
    printerr_va( format, variadic );\
    stderr_push( '\n' )

#endif
