<!--
 * Description:  Liquid Engine Core String Documentation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
-->

# Liquid Engine Core String | [Table of Contents](../readme.md)

Header:
```cpp
#include "core/string.h"
```

Includes:
- "defines.h"

## Jump Table
- [Types](#types)
- [Functions](#functions)
    - [Character](#character)
    - [C String](#c-string)
    - [String Slice](#string-slice)

## Types

```cpp
/// Slice of string buffer.
/// Size: 24
typedef struct StringSlice {
    char* buffer;
    usize len;
    usize capacity;
} StringSlice;
```

## Functions

### Character

```cpp
/// Returns true if character is whitespace character.
b32 char_is_whitespace( char character );
```
```cpp
/// Returns true if character is a digit.
b32 char_is_digit( char character );
```
```cpp
/// Returns true if character is a hexadecimal digit.
b32 char_is_digit_hexadecimal( char character );
```

### C String

```cpp
/// Calculate null-terminated C string length.
usize cstr_len( const char* cstr );
```
```cpp
/// Compare two null-terminated C strings.
b32 cstr_cmp( const char* a, const char* b );
```
```cpp
/// Copy null-terminated string to character buffer.
/// Optionally takes in length of source string.
/// If no length is provided, calculates length of source string.
/// Destination must be able to hold source length.
void cstr_copy( char* restrict dst, const char* restrict src, usize len );
```
```cpp
/// Copies overlapping null-terminated strings.
/// Optionally takes in length of source string.
/// If no length is provided, calculates length of source string.
/// Destination must be able to hold source length.
void cstr_copy_overlapped( char* dst, const char* src, usize len );
```

### String Slice

```cpp
/// Create a const string slice from literal.
#define string_slice_const( variable_name, literal )
```
```cpp
/// Create a mutable string slice from string literal.
/// Buffer is stack allocated so string slice is only valid in the current scope.
#define string_slice_mut( variable_name, literal )
```
```cpp
/// Create a string slice.
/// The lifetime of the string slice is the lifetime
/// of the provided buffer.
StringSlice string_slice( usize capacity, char* buffer );
```
```cpp
/// Clone a string slice.
StringSlice string_slice_clone( StringSlice* slice );
```
```cpp
/// Create a string slice from null-terminated string buffer.
/// Optionally takes in length of the string buffer.
StringSlice string_slice_from_cstr( usize opt_len, const char* cstr );
```
```cpp
/// Returns true if slice is empty.
b32 string_slice_is_empty( StringSlice* slice );
```
```cpp
/// Returns true if slice is full.
b32 string_slice_is_full( StringSlice* slice );
```
```cpp
/// Create a hash for given string.
u64 string_slice_hash( StringSlice* slice );
```
```cpp
/// Compare two string slices for equality.
b32 string_slice_cmp( StringSlice* a, StringSlice* b );
```
```cpp
/// Find phrase in string slice.
/// Returns true if phrase is found and sets out_index to result's index.
/// If out_index is NULL, just returns if phrase was found.
b32 string_slice_find( StringSlice* slice, StringSlice* phrase, usize* opt_out_index );
```
```cpp
/// Find a phrase in string slice.
/// Optionally get index of where first instance of phrase was found.
/// Counts how many times phrase appears in string.
/// Returns true if phrase was found.
b32 string_slice_find_count( StringSlice* slice, StringSlice* phrase, usize* opt_out_first_index, usize* out_count );
```
```cpp
/// Find character in string slice.
/// Returns true if character is found and sets out_index to result's index.
/// If out_index is NULL, just returns if character was found.
b32 string_slice_find_char( StringSlice* slice, char character, usize* opt_out_index );
```
```cpp
/// Find character in string slice.
/// Optionally get index of where first instance of character was found.
/// Counts how many times character appears in string.
/// Returns true if character was found.
b32 string_slice_find_char_count( StringSlice* slice, char character, usize* opt_out_first_index, usize* out_count );
```
```cpp
/// Find whitespace character in string slice.
/// Optionally get index of whitespace character.
/// Returns true if whitespace character was found.
b32 string_slice_find_whitespace( StringSlice* slice, usize* opt_out_index );
```
```cpp
/// Copy source string to destination.
/// Copies up to destination length or source length, whichever is less.
void string_slice_copy_to_len( StringSlice* dst, StringSlice* src );
```
```cpp
/// Copy source string to destination.
/// Copies up to destination capacity or source length, whichever is less.
void string_slice_copy_to_capacity( StringSlice* dst, StringSlice* src );
```
```cpp
/// Reverse contents of string slice.
void string_slice_reverse( StringSlice* slice );
```
```cpp
/// Trim trailing whitespace.
/// Note: this does not change the buffer contents,
/// it just changes the slice's length.
void string_slice_trim_trailing_whitespace( StringSlice* slice );
```
```cpp
/// Set all characters in string slice to given character up to slice length.
void string_slice_fill_to_len( StringSlice* slice, char character );
```
```cpp
/// Set all characters in string slice to given character up to slice capacity.
/// Sets slice length to capacity.
void string_slice_fill_to_capacity( StringSlice* slice, char character );
```
```cpp
/// Push character to end of string.
/// Returns true if there was enough space in buffer.
b32 string_slice_push( StringSlice* slice, char character );
```
```cpp
/// Pop character off end of string.
/// Returns true if length is not 0.
b32 string_slice_pop( StringSlice* slice, char* opt_out_character );
```
```cpp
/// Insert character in string slice.
/// Returns true if there was enough space in buffer.
b32 string_slice_insert_char( StringSlice* slice, usize index, char character );
```
```cpp
/// Prepend string to string.
/// Returns true if destination buffer has enough space.
b32 string_slice_prepend( StringSlice* slice, StringSlice* prepend );
```
```cpp
/// Insert string in string at given index.
/// Returns true if destination buffer has enough space.
b32 string_slice_insert( StringSlice* slice, usize index, StringSlice* insert );
```
```cpp
/// Append string to end of string.
/// Returns true if destination buffer has enough space.
b32 string_slice_append( StringSlice* slice, StringSlice* append );
```
```cpp
/// Prepend string to string.
/// Returns true if destination buffer has enough space.
b32 string_slice_prepend_cstr( StringSlice* slice, const char* prepend );
```
```cpp
/// Insert string in string at given index.
/// Returns true if destination buffer has enough space.
b32 string_slice_insert_cstr( StringSlice* slice, usize index, const char* insert );
```
```cpp
/// Append string to end of string.
/// Returns true if destination buffer has enough space.
b32 string_slice_append_cstr( StringSlice* slice, const char* append );
```
```cpp
/// Split string in two at given index.
void string_slice_split( StringSlice* slice, usize index, StringSlice* out_first, StringSlice* out_last );
```
```cpp
/// Split string at first instance of given character.
b32 string_slice_split_char( StringSlice* slice, char character, StringSlice* out_first, StringSlice* out_last );
```
```cpp
/// Split string at first instance of whitespace.
/// Last slice starts at first non-whitespace character.
b32 string_slice_split_whitespace( StringSlice* slice, StringSlice* out_first, StringSlice* out_last );
```
```cpp
/// Parse an integer from given string.
b32 string_slice_parse_int( StringSlice* slice, i64* out_integer );
```
```cpp
/// Parse an unsigned integer from given string.
b32 string_slice_parse_uint( StringSlice* slice, u64* out_integer );
```
```cpp
/// Parse a float from given string.
b32 string_slice_parse_float( StringSlice* slice, f64* out_float );
```
```cpp
/// Write a formatted string to string slice using variadic list.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
#define string_slice_fmt( slice, format, ... )
```
```cpp
/// Write a formatted string to string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
#define string_slice_fmt_va( slice, format, va )
```
```cpp
/// Write the value of boolean into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_bool( StringSlice* slice, b32 b, b32 binary );
```
```cpp
/// Write the value of float into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_float( StringSlice* slice, f64 f, u32 precision );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_i8( StringSlice* slice, i8 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_u8( StringSlice* slice, u8 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_i16( StringSlice* slice, i16 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_u16( StringSlice* slice, u16 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_i32( StringSlice* slice, i32 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_u32( StringSlice* slice, u32 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_i64( StringSlice* slice, i64 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_u64( StringSlice* slice, u64 i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_isize( StringSlice* slice, isize i, enum FormatInteger format );
```
```cpp
/// Write the value of integer into string slice.
/// Returns number of bytes necessary to complete write operation if
/// string slice is not large enough.
usize string_slice_fmt_usize( StringSlice* slice, usize i, enum FormatInteger format );
```
