<!--
 * Description:  Defines
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# Defines | [Table of Contents](./toc.md)

To include in game DLL:
```cpp
#include <defines.h>
```
To include in engine:
```cpp
#include "defines.h"
```

Includes:
- <TargetConditionals.h> APPLE ONLY

## Jump Table
- [Macro Functions](#macro-functions)
- [Type Aliases](#type-aliases)
- [Attributes](#attributes)
- [Conditionals](#conditionals)

## Macro Functions

```cpp
/// Mark value as unused.
#define unused(x) /// -> void
```
```cpp
/// Infinite loop.
#define loop for(;;)
```
```cpp
/// Make a version identifier.
#define ld_make_version( major, minor ) /// -> u32
```
```cpp
/// Get major version from version identifier.
#define ld_get_major( version ) /// -> u32
```
```cpp
/// Get minor version from version identifier.
#define ld_get_minor( version ) /// -> u32
```
```cpp
/// Crash program.
#define panic() /// noreturn
```
```cpp
/// Compile-time assertion.
#define static_assert( condition, message ) /// -> void
```
```cpp
/// Run-time assertion.
#define assert( condition ) /// -> noreturn if failed
```
```cpp
/// Check if bits in bitmask are set in bitfield.
#define bitfield_check( bitfield, bitmask ) /// -> bool
```
```cpp
/// Check if ONLY bits in bitmask are set in bitfield.
#define bitfield_check_exact( bitfield, bitmask ) /// -> bool
```
```cpp
/// Toggle bits in bitfield masked by bitmask.
#define bitfield_toggle( bitfield, bitmask ) /// -> typeof(bitfield)
```
```cpp
/// Clear masked bits in bitfield.
#define bitfield_clear( bitfield, bitmask ) /// -> typeof(bitfield)
```
```cpp
/// Swap a and b.
#define swap( a, b ) /// -> void
```
```cpp
/// Kilobytes to bytes.
#define kilobytes( kb ) /// -> u64
```
```cpp
/// Megabytes to bytes.
#define megabytes( mb ) /// -> u64
```
```cpp
/// Gigabytes to bytes.
#define gigabytes( gb ) /// -> u64
```
```cpp
/// Terabytes to bytes.
#define terabytes( tb ) /// -> u64
```
```cpp
/// Convert macro name to const char*.
#define macro_name_to_string( macro ) /// -> const char*
```
```cpp
/// Convert macro value to const char*.
#define macro_value_to_string( macro ) /// -> const char*
```
```cpp
/// Define a 24-bit RGB value.
#define rgb_u32( r, g, b ) /// -> u32
```
```cpp
/// Define a 32-bit RGBA value.
#define rgba_u32( r, g, b, a ) /// -> u32
```
```cpp
/// Calculate number of elements in a static array.
#define static_array_count( array ) /// -> usize
```
```cpp
/// Calculate size of static array.
#define static_array_size( array ) /// -> usize
```
```cpp
/// Reinterpret cast.
#define reinterpret( type, value ) /// -> typeof(type)
```
```cpp
/// Get the byte offset of field in struct.
#define offsetof( struct, field ) /// -> usize
```
```cpp
/// spread array elements
/// useful for unpacking arrays
/// for functions which take multiple elements
#define spread_2( array )
#define spread_3( array )
#define spread_4( array )
```

## Type Aliases 

| Type     | Description                              |
| -------- | ---------------------------------------- |
| usize    | unsigned pointer-sized integer           |
| isize    |          pointer-sized integer           |
| u8       | unsigned  8-bit        integer           |
| u16      | unsigned 16-bit        integer           |
| u32      | unsigned 32-bit        integer           |
| u64      | unsigned 64-bit        integer           |
| i8       |           8-bit        integer           |
| i16      |          16-bit        integer           |
| i32      |          32-bit        integer           |
| i64      |          64-bit        integer           |
| f32      | single-precision IEEE-754 floating point |
| f64      | double-precision IEEE-754 floating point |
| b8       |  8-bit boolean                           |
| b16      | 16-bit boolean                           |
| b32      | 32-bit boolean                           |
| c8       | UTF-8 character                          |
| c16      | UTF-16/Windows Unicode character         |
| c32      | UTF-32 character                         |
| pvoid    | void pointer                             |
| tuple_## | tuple structs of the types defined above |

## Attributes

| Attribute       | Description                                              |
| --------------- | -------------------------------------------------------- |
| internal        | static - function is internal to translation unit        |
| local           | static - for local static variables                      |
| global          | static - for global static variables                     |
| hot             | always optimize following function                       |
| no_optimize     | never optimize following function                        |
| packedpad       | struct is packed (no C padding)                          |
| force_inline    | always inline following function                         |
| no_inline       | never inline following function                          |
| header_only     | mark header function (extern inline in C, inline in C++) |
| extern_c        | mark function as extern "C" (C++ only)                   |
| deprecate       | mark function/struct as deprecated                       |
| transparent     | mark union as transparent                                |
| maybe_unused    | mark function/type as possibly unused                    |
| aligned(x)      | mark value to have x alignment, must be power of two     |
| LD_API          | export/import function                                   |

## Conditionals

| Misc            | Description                     |
| --------------- | ------------------------------- |
| LD_API_INTERNAL | only defined in internal source |

| Compiler                | Description                               |
| ----------------------- | ----------------------------------------- |
| LD_COMPILER_GCC         | GNU Compiler Collection is in use         |
| LD_COMPILER_CLANG       | Clang compiler is in use                  |
| LD_COMPILER_MSVC[^3]    | Microsoft Visual C++ compiler is in use   |
| LD_COMPILER_UNKNOWN[^1] | Unknown C++ compiler is in use            |

| Platform                   | Description                       |
| -------------------------- | --------------------------------- |
| LD_PLATFORM_WINDOWS        | Current target is Windows x64     |
| LD_PLATFORM_LINUX[^1]      | Current target is Linux x64/ARM64 |
| LD_PLATFORM_ANDROID[^1]    | Current target is Android         |
| LD_PLATFORM_IOS[^1]        | Current target is iOS             |
| LD_PLATFORM_MACOS[^1]      | Current target is MacOS ARM64     |
| LD_PLATFORM_WASM [^1] [^2] | Current target is WASM32/64       |
| LD_PLATFORM_UNKNOWN        | Current target is unknown         |

| Architecture           | Description                             |
| ---------------------- | --------------------------------------- |
| LD_ARCH_32_BIT[^1]     | Current architecture is 32-bit          |
| LD_ARCH_64_BIT         | Current architecture is 64-bit          |
| LD_ARCH_LITTLE_ENDIAN  | Current architecture is little endian   |
| LD_ARCH_BIG_ENDIAN     | Current architecture is big endian      |
| LD_ARCH_X86            | Current architecture is Intel x86 based |
| LD_ARCH_ARM[^1]        | Current architecture is ARM based       |
| LD_ARCH_WASM [^1] [^2] | Current architecture is WASM based      |

### Footnotes

[^1]: not currently supported

[^2]: not currently defined

[^3]: will not be supported

