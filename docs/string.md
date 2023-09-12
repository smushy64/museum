<!--
 * Description:  String Documentation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# String | [Table of Contents](./toc.md)

To include in game DLL:
```cpp
#include <core/strings.h>
```
To include in engine:
```cpp
#include "core/strings.h"
```

Includes:
- "defines.h"
- "core/variadic.h"

## Jump Table
- [Types](#types)
- [Functions](#functions)
    - [Character](#character)
    - [String Slice](#string-slice)
    - [IO](#io)
- [Macro Functions](#macro-functions)
- [Format Specifiers](#format-specifiers)
- [Format Specifier Parameters](#format-specifier-parameters)
    - [Parameters for Any Specifier](#parameters-for-any-specifier)
    - [Parameters for Number Specifiers](#parameters-for-number-specifiers)
    - [Parameters for Integer Specifiers](#parameters-for-integer-specifiers)
    - [Parameters for Float Specifiers](#parameters-for-float-specifiers)
    - [Parameters for Boolean Specifiers](#parameters-for-boolean-specifiers)
- [Formatting Examples](#formatting-examples)

## Types

```cpp
/// Slice of string buffer.
/// Size: 24
typedef struct StringView {
    char* buffer;
    usize len;
    usize capacity;
} StringView;
```
```cpp
/// Options for formatting integers.
typedef enum : u32 {
    FMT_INT_DECIMAL,
    FMT_INT_BINARY,
    FMT_INT_HEX
} FormatInteger;
```

## Functions

### Character

```cpp
/// Push character to stdout.
void char_output_stdout( char character );
```
```cpp
/// Push character to stderr.
void char_output_stderr( char character );
```
```cpp
/// Returns true if character is whitespace character.
b32 char_is_whitespace( char character );
```
```cpp
/// Returns true if character is a digit.
b32 char_is_digit( char character );
```

### String Slice

```cpp
/// Create a string slice.
/// The lifetime of the string slice is the lifetime
/// of the provided buffer.
StringSlice ss( usize buffer_size, char* buffer );
```
```cpp
/// Clone a string slice.
StringSlice ss_clone( StringSlice* slice );
```
```cpp
/// Create a string slice from null-terminated string buffer.
/// Optionally takes in length of the string buffer but
/// this can be left out as the function can calculate it.
/// It is recommended that you use the STRING macro to create
/// string slices from string literals as that will ensure that
/// the string slice can be mutated.
/// The lifetime of a string slice of a string literal is
/// up to the end of the scope.
StringSlice ss_from_cstr( usize opt_len, const char* cstr );
```
```cpp
/// Returns true if slice is empty.
b32 ss_is_empty( StringSlice* slice );
```
```cpp
/// Returns true of slice is full.
b32 ss_is_full( StringSlice* slice );
```
```cpp
/// Create a hash for given string.
u64 ss_hash( StringSlice* slice );
```
```cpp
/// Compare two string slices for equality.
b32 ss_cmp( StringSlice* a, StringSlice* b );
```
```cpp
/// Find phrase in string slice.
/// Returns true if phrase is found and sets out_index to result's index.
/// If out_index is NULL, just returns if phrase was found.
b32 ss_find( StringSlice* slice, StringSlice* phrase, usize* opt_out_index );
```
```cpp
/// Find character in string slice.
/// Returns true if character is found and sets out_index to result's index.
/// If out_index is NULL, just returns if character was found.
b32 ss_find_char( StringSlice* slice, char character, usize* opt_out_index );
```
```cpp
/// Count how many times phrase appears in string slice.
usize ss_phrase_count( StringSlice* slice, StringSlice* phrase );
```
```cpp
/// Count how many times character appears in string slice.
usize ss_char_count( StringSlice* slice, char character );
```
```cpp
/// Copy the contents of src string slice up to
/// the capacity of dst string slice.
void ss_mut_copy( StringSlice* dst, StringSlice* src );
```
```cpp
/// Copy the contents of src string slice up to
/// the len of dst string slice.
void ss_mut_copy_to_len( StringSlice* dst, StringSlice* src );
```
```cpp
/// Copy the contents of src string up to
/// the capacity of dst string slice.
/// Optionally takes in length of the src string but
/// this can be left out as it can be calculated.
void ss_mut_copy_cstr( StringSlice* dst, usize opt_len, const char* src );
```
```cpp
/// Copy the contents of src string up to
/// the len of dst string slice.
/// Optionally takes in length of the src string but
/// this can be left out as it can be calculated.
void ss_mut_copy_cstr_to_len( StringSlice* dst, usize opt_len, const char* src );
```
```cpp
/// Reverse contents of string slice.
void ss_mut_reverse( StringSlice* slice );
```
```cpp
/// Trim trailing whitespace.
/// Note: this does not change the buffer contents,
/// it just changes the slice's length.
void ss_mut_trim_trailing_whitespace( StringSlice* slice );
```
```cpp
/// Set all characters in string slice to given character.
void ss_mut_fill( StringSlice* slice, char character );
```
```cpp
/// Set all characters in string slice to given character.
/// Goes up to the capacity of string slice rather than len.
void ss_mut_fill_to_capacity( StringSlice* slice, char character );
```
```cpp
/// Push character to end of string slice.
/// Returns true if slice had enough capacity to push character.
b32 ss_mut_push( StringSlice* slice, char character );
```
```cpp
/// Insert character into string slice.
/// Returns true if slice had enough capacity to insert character.
b32 ss_mut_insert( StringSlice* slice, char character, usize position );
```
```cpp
/// Append slice to end of slice.
/// Appends up to slice's capacity.
/// Returns true if full slice was appended.
b32 ss_mut_append( StringSlice* slice, StringSlice* append );
```
```cpp
/// Write a formatted string to string slice.
usize ss_mut_fmt( StringSlice* slice, const char* format, ... );
```
```cpp
/// Write a formatted string to string slice using variadic list.
usize ss_mut_fmt_va( StringSlice* slice, const char* format, va_list variadic );
```
```cpp
/// Split string slice at given index.
void ss_split_at( StringSlice* slice_to_split, usize index, StringSlice* out_first, StringSlice* out_last );
```
```cpp
/// Split string slice at first ocurrence of whitespace.
/// If none is found, returns false and out pointers are not modified.
b32 ss_split_at_whitespace( StringSlice* slice_to_split, StringSlice* out_first, StringSlice* out_last );
```
```cpp
/// Attempt to parse integer from string slice.
/// Returns true if successful.
b32 ss_parse_int( StringSlice* slice, i64* out_integer );
```
```cpp
/// Attempt to parse unsigned integer from string slice.
/// Returns true if successful.
b32 ss_parse_uint( StringSlice* slice, u64* out_integer );
```
```cpp
/// Attempt to parse float from string slice.
/// Returns true if successful.
b32 ss_parse_float( StringSlice* slice, f64* out_float );
```
```cpp
/// Output string slice to standard out.
void ss_output_stdout( StringSlice* slice );
```
```cpp
/// Output string slice to standard error.
void ss_output_stderr( StringSlice* slice );
```
```cpp
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_i8( StringSlice* slice, i8 value, FormatInteger fmt );
```
```cpp
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_i16( StringSlice* slice, i16 value, FormatInteger fmt );
```
```cpp
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_i32( StringSlice* slice, i32 value, FormatInteger fmt );
```
```cpp
/// Format signed integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_i64( StringSlice* slice, i64 value, FormatInteger fmt );
```
```cpp
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_u8( StringSlice* slice, u8 value, FormatInteger fmt );
```
```cpp
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_u16( StringSlice* slice, u16 value, FormatInteger fmt );
```
```cpp
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_u32( StringSlice* slice, u32 value, FormatInteger fmt );
```
```cpp
/// Format integer into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_u64( StringSlice* slice, u64 value, FormatInteger fmt );
```
```cpp
/// Format float into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_f32( StringSlice* slice, f32 value, u32 precision );
```
```cpp
/// Format float into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_f64( StringSlice* slice, f64 value, u32 precision );
```
```cpp
/// Format boolean into string slice.
/// Returns additional capacity required to fit value in slice.
usize ss_mut_fmt_b32( StringSlice* slice, b32 value );
```

### IO

```cpp
/// Print to stdout.
void print( const char* format, ... );
```

```cpp
/// Print to stderr.
void print_err( const char* format, ... );
```

```cpp
/// Print to stdout using variadic list.
void print_va( const char* format, va_list variadic );
```

```cpp
/// Print to stderr using variadic list.
void print_err_va( const char* format, va_list variadic );
```

## Macro Functions

```cpp
/// Create a mutable string slice from literal.
/// This is a bad attempt to get around the
/// limitations of the C language but whatever.
#define STRING( variable_name, literal )
```
```cpp
/// Print to stdout with a new line at the end.
#define println( format, ... )
```
```cpp
/// Print to stdout with a new line at the end.
/// Variadic arguments come from va_list.
#define println_va( format, variadic )
```
```cpp
/// Print to stderr with a new line at the end.
#define println_err( format, ... )
```
```cpp
/// Print to stderr with a new line at the end.
/// Variadic arguments come from va_list.
#define println_err_va( format, variadic )
```

## Format Specifiers.
use {} to wrap a format specifier and use commas to separate parameters

` Note: no specifier/parameter is case sensitive `

| Specifier        | Type           | Description                              |
| ---------------- | -------------- | ---------------------------------------- |
| {{               | -              | literal '{'                              |
| }}               | -              | literal '}'                              |
| c                | Character      | 8-bit ASCII character                    |
| cc               | String         | const char* ASCII null-terminated string |
| s                | String         | StringSlice                              |
| u                | Number/Integer | unsigned int32                           |
| u8/16/32/64      | Number/Integer | unsigned sized int                       |
| usize            | Number/Integer | unsigned pointer sized int               |
| i                | Number/Integer | int32                                    |
| i8/16/32/64      | Number/Integer | sized int                                |
| isize            | Number/Integer | pointer sized int                        |
| iv2/3/4          | Number/Integer | int vectors                              |
| f                | Number/Float   | float (f32 gets promoted to f64)         |
| v2/3/4           | Number/Float   | float vectors                            |
| q                | Number/Float   | quaternion                               |
| b                | Boolean        | boolean                                  |

## Format Specifier Parameters
### Parameters for Any Specifier
| Parameter | Description                                                    |
| --------- | -------------------------------------------------------------- |
| int       | number of padding spaces; can be negative to pad to right side |

### Parameters for Number Specifiers
| Parameter         | Description                                  |
| ----------------- | -------------------------------------------- |
| 0 followed by int | number of padding zeroes, cannot be negative |

### Parameters for Integer Specifiers
| Parameter | Description        |
| --------- | ------------------ |
| x         | hexadecimal format |
| b         | binary format      |

### Parameters for Float Specifiers
| Parameter                 | Description                              |
| ------------------------- | ---------------------------------------- |
| . followed by int         | fractional precision, cannot be negative |
| b (does nothing with v/q) | storage formatting (bytes/kb/mb/gb/tb)   |

### Parameters for Boolean Specifiers
| Parameter | Description                   |
| --------- | ----------------------------- |
| b         | use 0/1 instead of true/false |

## Formatting Examples
```cpp
print( "{i} {f} {cc}", 10, -5.5f, "Hello world" );
```
```
10 -5.500000 Hello World
```
---
```cpp
print( "{f}|{f,3} {f,03.3}", -2.0f, 5.0f 1.25f );
```
```
-2.000000|  5.000000 001.250
```
---
```cpp
print( "|{cc,-6}|", "hi" );
```
```
|hi    |
```
---
```cpp
print( "|{cc,6}|", "hi" );
```
```
|    hi|
```

