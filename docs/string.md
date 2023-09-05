<!--
 * Description:  String Documentation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# String | [Table of Contents](./toc.md)

To include in game DLL:
```cpp
#include <core/ldstring.h>
```
To include in engine:
```cpp
#include "core/ldstring.h"
```

Includes:
- "defines.h"
- "core/ldvariadic.h"
- "core/ldallocator.h"

## Jump Table
- [Types](#types)
- [Functions](#functions)
    - [Character](#character)
    - [String View](#string-view)
    - [Dynamic String](#dynamic-string)
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
/// Dynamic heap allocated string that owns its buffer
/// Size: 24
typedef struct DynamicString {
    char* buffer;
    usize len;
    usize capacity;
} DynamicString;
```

```cpp
/// View into string buffer. Does not own its buffer.
/// Size: 16
typedef struct StringView {
    union {
        const char* str;
        char* buffer;
    };
    usize len;
} StringView;
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

### String View

```cpp
/// Create a string view from C null-terminated string.
StringView sv_from_str( const char* str );
```
```cpp
/// Create a string view from dynamic string.
StringView sv_from_string( String string );
```
```cpp
/// Output string view to stdout.
void sv_output_stdout( StringView sv );
```
```cpp
/// Output string view to stderr.
void sv_output_stderr( StringView sv );

```
```cpp
/// Compare string views for equality.
/// Returns true if string view contents are equal and lengths are equal.
b32 sv_cmp( StringView a, StringView b );
```
```cpp
/// Compare strings for equality.
/// Returns true if string contents are equal and lengths are equal.
b32 sv_cmp_string( StringView a, String* b );
```
```cpp
/// Format string into string view buffer.
/// Returns required size for formatting.
u32 sv_format( StringView sv, const char* format, ... );
```
```cpp
/// Format string into string view buffer using variadic list.
/// Returns required size for formatting.
u32 sv_format_va( StringView sv, const char* format, va_list variadic );
```
```cpp
/// Trim trailing whitespace from string view.
void sv_trim_trailing_whitespace( StringView* sv );
```
```cpp
/// Find the first instance of a character in string view.
/// Returns -1 if character is not found.
isize sv_find_first_char( StringView sv, char character );
```
```cpp
/// Parse an int32 from string view buffer.
i32 sv_parse_i32( StringView sv );
```
```cpp
/// Parse a uint32 from string view buffer.
u32 sv_parse_u32( StringView sv );
```
```cpp
/// Returns true if string view contains the given phrase.
b32 sv_contains( StringView sv, StringView phrase );
```
```cpp
/// Copy contents of src string view buffer to
/// dst string view buffer.
/// Copies only up to dst length.
void sv_copy( StringView src, StringView dst );
```
```cpp
/// Set all characters in string view to given character.
void sv_fill( StringView sv, char character );
```
```cpp
/// Clone a string view.
StringView sv_clone( StringView sv );
```

### Dynamic String

```cpp
/// Create a new dynamic string from string view.
b32 dstring_new( Allocator* allocator, StringView sv, String* out_string );
```
```cpp
/// Create a new empty dynamic string with given capacity.
b32 dstring_with_capacity( Allocator* allocator, usize capacity, String* out_string );
```
```cpp
/// Reallocate string with given capacity.
/// Does nothing if new capcity is smaller than existing capacity.
b32 dstring_reserve( Allocator* allocator, String* string, usize new_capacity );
```
```cpp
/// Clear a string.
/// All this does is set the length to zero.
/// It does not free buffer.
void dstring_clear( String* string );
```
```cpp
/// Append the contents of string view to the end of string.
/// Alloc determines if it will fill to the end of the buffer
/// or reallocate to fit string view.
b32 dstring_append( Allocator* allocator, String* string, StringView append, b32 alloc );
```
```cpp
/// Push a character onto end of string.
/// Realloc determines how much extra buffer bytes will be allocated if
/// character is at the end of string buffer.
/// 0 means that it will not realloc string.
/// Only returns false if reallocation fails, otherwise always returns true.
b32 dstring_push_char( Allocator* allocator, String* string, char character, u32 realloc );
```
```cpp
/// Pop last character in string.
/// Returns 0 if string length is zero.
char dstring_pop_char( String* string );
```
```cpp
/// Make a string view into string that respects string capacity.
StringView dstring_view_capacity_bounds( String string, usize offset );
```
```cpp
/// Make a string view into string that respects string length.
StringView dstring_view_len_bounds( String string, usize offset );
```
```cpp
/// Free a dynamic string.
void dstring_free( Allocator* allocator, String* string );
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
/// Make a string view from const char*
#define SV( str )
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
| c                | Character      | 8-bit ASCII character                    |
| cc               | String         | const char* ASCII null-terminated string |
| s                | String         | String                                   |
| sv               | String         | StringView                               |
| u                | Number/Integer | unsigned int32                           |
| u8/16/32/64      | Number/Integer | unsigned sized int                       |
| i                | Number/Integer | int32                                    |
| i8/16/32/64      | Number/Integer | sized int                                |
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
| b (does nothing with v/q) | storage formatting (bytes/kb/mb/gb)      |

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

