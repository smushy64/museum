<!--
 * Description:  String Formatting Documentation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# String Formatting | [Table of Contents](./toc.md)

Guide to string formatting functions.

Include in game DLL:
```cpp
#include <core/string.h>
```
Include in engine:
```cpp
#include "core/string.h"
```

## Jump Table
- [Functions](#functions)
    - [Formatting](#formatting)
    - [Console Output](#console-output)
- [Format Specifiers](#format-specifiers)
- [Format Specifier Parameters](#format-specifier-parameters)
    - [Parameters for Any Specifier](#parameters-for-any-specifier)
    - [Parameters for Number Specifiers](#parameters-for-number-specifiers)
    - [Parameters for Integer Specifiers](#parameters-for-integer-specifiers)
    - [Parameters for Float Specifiers](#parameters-for-float-specifiers)
    - [Parameters for Boolean Specifiers](#parameters-for-boolean-specifiers)
- [Examples](#examples)

## Functions

### Formatting
```cpp
/// Format string into string view.
LD_API u32 string_format(
    StringView buffer,
    const char* format,
    ...
);
```

```cpp
/// Format string into string view using variadic list.
LD_API u32 string_format_va(
    StringView buffer,
    const char* format,
    va_list variadic
);
```

### Console Output

```cpp
/// Print to stdout.
LD_API void print( const char* format, ... );
```

```cpp
/// Print to stderr.
LD_API void printerr( const char* format, ... );
```

```cpp
/// Print to stdout using variadic list.
LD_API void print_va( const char* format, va_list variadic );
```

```cpp
/// Print to stderr using variadic list.
LD_API void printerr_va( const char* format, va_list variadic );
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
#define printlnerr( format, ... )
```

```cpp
/// Print to stderr with a new line at the end.
/// Variadic arguments come from va_list.
#define printlnerr_va( format, variadic )
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
| b                | Boolean        | boolean                                  |
| f                | Number/Float   | float (f32 gets promoted to f64)         |
| v2/3/4           | Number/Float   | float vectors                            |
| q                | Number/Float   | quaternion                               |

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

## Examples
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

