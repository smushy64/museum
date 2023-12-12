<!--*
 * Description:  Liquid Engine Core String Formatting Documentation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 29, 2023
-->

# Liquid Engine Core String Formatting | [Table of Contents](../readme.md)

Header:
```cpp
#include "core/fmt.h"
```

Includes:
- "defines.h"

## Jump Table
- [Types](#types)
- [Functions](#functions)
- [Formatting](#formatting)
    - [Parenthesis](#parenthesis)
    - [Null Terminator](#null-terminator)
    - [Boolean](#boolean)
    - [Char](#char)
    - [C-String](#c-string)
    - [Float](#float)
    - [Integer](#integer)
    - [String Slice](#string-slice)

## Types
```cpp
/// Supported integer formats.
typedef enum FormatInteger : u32 {
    FORMAT_INTEGER_DECIMAL,
    FORMAT_INTEGER_BINARY,
    FORMAT_INTEGER_HEXADECIMAL_LOWER,
    FORMAT_INTEGER_HEXADECIMAL_UPPER,
} FormatInteger;
```
```cpp
/// Formatting write function.
/// Target is a pointer to where-ever the formatted string should be written to.
/// Returns 0 if successful, otherwise returns number of characters
/// that could not be written.
typedef usize FormatWriteFN( void* target, usize count, char* characters );
```

## Functions

```cpp
/// Parse integer from string.
b32 fmt_read_int( usize len, char* buffer, i64* out_parsed_int );
```
```cpp
/// Parse unsigned integer from string.
b32 fmt_read_uint( usize len, char* buffer, u64* out_parsed_int );
```
```cpp
/// Write boolean to a target.
/// Returns number of bytes necessary to complete write operation
/// if target is not large enough.
usize fmt_write_bool( FormatWriteFN* write, void* target, b32 b, b32 binary );
```
```cpp
/// Write floating point number to a target.
/// Returns number of bytes necessary to complete write operation
/// if target is not large enough.
usize fmt_write_float( FormatWriteFN* write, void* target, f64 f, u32 precision );
```
```cpp
/// Write 8-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_i8( FormatWriteFN* write, void* target, i8 integer, FormatInteger format );
```
```cpp
/// Write 8-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_u8( FormatWriteFN* write, void* target, u8 integer, FormatInteger format );
```
```cpp
/// Write 16-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_i16( FormatWriteFN* write, void* target, i16 integer, FormatInteger format );
```
```cpp
/// Write 16-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_u16( FormatWriteFN* write, void* target, u16 integer, FormatInteger format );
```
```cpp
/// Write 32-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_i32( FormatWriteFN* write, void* target, i32 integer, FormatInteger format );
```
```cpp
/// Write 32-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_u32( FormatWriteFN* write, void* target, u32 integer, FormatInteger format );
```
```cpp
/// Write 64-bit integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_i64( FormatWriteFN* write, void* target, i64 integer, FormatInteger format );
```
```cpp
/// Write 64-bit unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_u64( FormatWriteFN* write, void* target, u64 integer, FormatInteger format );
```
```cpp
/// Write pointer sized integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_isize( FormatWriteFN* write, void* target, isize integer, FormatInteger format );
```
```cpp
/// Write pointer sized unsigned integer to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
usize fmt_write_usize( FormatWriteFN* write, void* target, usize integer, FormatInteger format );
```
```cpp
/// Write formatted string to a target.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
#define fmt_write( write, target, format, ... )
```
```cpp
/// Write formatted string to a target using variadic arguments.
/// Returns number of bytes necessary to complete write operation if
/// target is not large enough.
#define fmt_write_va( write, target, format, va )
```

# Formatting

Format specifiers follow this prototype:

{specifier,arguments}

Data is assumed to be passed in by value unless the * argument is used except
for C-strings, which are always passed as a pointer.

When two arguments are conflicting, the last argument will take precedence.

## Parenthesis

Description: Open parenthesis, two are needed to escape formatting.

### Parenthesis - Specifiers

{{ -

## Null Terminator

Description: Write a null character.

### Null Terminator - Specifiers

{0} - Null character

## Boolean

Description: 32-bit boolean.

### Boolean - Specifiers

{b} - 32-bit boolean

### Boolean - Arguments

- *[positive integer, optional]
    - Indicates that value is passed in by pointer or is an array.
    - Integer indicates how many boolean values to write.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- b
    - Write boolean as a binary number rather than text.

## Char

Description: 8-bit ASCII character.

### Char - Specifiers

{c} - 8-bit character

### Char - Arguments

- *[positive integer, optional]
    - Indicates that value is passed in by pointer or is an array.
    - Integer indicates how many chars to write.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- u
    - Convert letter to upper-case.
- l
    - Convert letter to lower-case.

## C-String

Description: Null-terminated ASCII C-String.

### C-String - Specifiers

{cc} - pointer to null-terminated character array

### C-String - Arguments

- *[positive integer]
    - Only print up to n characters or null-terminator.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- u
    - Convert letters to upper-case.
- l
    - Convert letters to lower-case.

## Float

Description: 32-bit or 64-bit IEEE-754 floating point number.

### Float - Specifiers

{f} - 32-bit IEEE-754 floating point

{f32} - 32-bit IEEE-754 floating point

{f64} - 64-bit IEEE-754 floating point

### Float - Arguments

- *[positive integer, optional]
    - Indicates that value is passed in by pointer or is an array.
    - Integer indicates how many floats to write.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- [0 followed by positive integer]
    - Left side padding with 0 as the padding character.
- s
    - Separate every 3 digits by a comma.
- m
    - Memory formatting. Assumes value is bytes, converts to most convenient unit.
    - Ex: 1280.0f -> "1.25 KB"

## Float Vector

Description: 32-bit IEEE-754 floating point vector.

### Float - Specifiers

{v2} - 2-component float vector

{v3} - 3-component float vector

{v4} - 4-component float vector

### Float - Arguments

- *[positive integer, optional]
    - Indicates that value is passed in by pointer or is an array.
    - Integer indicates how many vectors to write.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- [0 followed by positive integer]
    - Left side padding with 0 as the padding character.
- s
    - Separate every 3 digits by a comma.
- m
    - Memory formatting. Assumes value is bytes, converts to most convenient unit.
    - Ex: 1280.0f -> "1.25 KB"

## Integer

Description: 8-bit, 16-bit, 32-bit, 64-bit or pointer sized signed or unsigned integer.

### Integer - Specifiers

{i} - 32-bit signed integer

{i8} - 8-bit signed integer

{i16} - 16-bit signed integer

{i32} - 32-bit signed integer

{i64} - 64-bit signed integer

{u} - 32-bit unsigned integer

{u8} - 8-bit unsigned integer

{u16} - 16-bit unsigned integer

{u32} - 32-bit unsigned integer

{u64} - 64-bit unsigned integer

{isize} - pointer sized signed integer

{usize} - pointer sized unsigned integer

### Integer - Arguments

- *[positive integer, optional]
    - Indicates that value is passed in by pointer or is an array.
    - Integer indicates how many integers to write.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- [0 followed by positive integer]
    - Left side padding with 0 as the padding character.
- x
    - Lower-case hexadecimal formatting.
- X
    - Upper-case hexadecimal formatting.
- b
    - Binary formatting.
- f
    - Write to full integer width (hex and binary only).
- s
    - Binary: Separate every 8 digits by a tick.
    - Decimal: Separate every 3 digits by a comma.
    - Hexadecimal: Separate every 4 digits by a tick.

## String Slice

Description: Slice of character buffer.

### String Slice - Specifiers

{s} - struct StringSlice passed by value.

### String Slice - Arguments

- *[positive integer]
    - Only print up to n characters or capacity.
- [positive integer]
    - Left side padding, only one type of padding is allowed.
- [negative integer]
    - Right side padding, only one type of padding is allowed.
- u
    - Convert letters to upper-case.
- l
    - Convert letters to lower-case.

