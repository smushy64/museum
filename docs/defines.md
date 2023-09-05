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
- [Constants](#constants)
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
/// Make a tuple struct for type.
#define MAKE_TUPLE(type) /// -> struct definition
```
```cpp
/// Make a version identifier.
#define LD_MAKE_VERSION( major, minor ) /// -> u32
```
```cpp
/// Get major version from version identifier.
#define LD_GET_MAJOR( version ) /// -> u32
```
```cpp
/// Get minor version from version identifier.
#define LD_GET_MINOR( version ) /// -> u32
```
```cpp
/// Crash program.
#define PANIC() /// noreturn
```
```cpp
/// Compile-time assertion.
#define STATIC_ASSERT( condition, message ) /// -> void
```
```cpp
/// Run-time assertion.
#define ASSERT( condition ) /// -> noreturn if failed
```
```cpp
/// Check if bits in bitmask are set in bitfield.
#define CHECK_BITS( bitfield, bitmask ) /// -> bool
```
```cpp
/// Check if ONLY bits in bitmask are set in bitfield.
#define CHECK_BITS_EXACT( bitfield, bitmask ) /// -> bool
```
```cpp
/// Toggle bits in bitfield masked by bitmask.
#define TOGGLE_BITS( bitfield, bitmask ) /// -> typeof(bitfield)
```
```cpp
/// Toggle masked bits in bitfield.
#define TOGGLE_BITS( bitfield, bitmask ) /// -> typeof(bitfield)
```
```cpp
/// Clear masked bits in bitfield.
#define CLEAR_BITS( bitfield, bitmask ) /// -> typeof(bitfield)
```
```cpp
/// Swap a and b.
#define SWAP( a, b ) /// -> void
```
```cpp
/// Kilobytes to bytes.
#define KILOBYTES( kb ) /// -> u64
```
```cpp
/// Megabytes to bytes.
#define MEGABYTES( mb ) /// -> u64
```
```cpp
/// Gigabytes to bytes.
#define GIGABYTES( gb ) /// -> u64
```
```cpp
/// Terabytes to bytes.
#define TERABYTES( tb ) /// -> u64
```
```cpp
/// Convert macro name to const char*.
#define TO_STRING( macro ) /// -> const char*
```
```cpp
/// Convert macro value to const char*.
#define VALUE_TO_STRING( macro ) /// -> const char*
```
```cpp
/// Define a 24-bit RGB value.
#define RGB_U32( r, g, b ) /// -> u32
```
```cpp
/// Define a 32-bit RGBA value.
#define RGBA_U32( r, g, b, a ) /// -> u32
```
```cpp
/// Calculate number of elements in a static array.
#define STATIC_ARRAY_COUNT( array ) /// -> usize
```
```cpp
/// Calculate size of static array.
#define STATIC_ARRAY_SIZE( array ) /// -> usize
```
```cpp
/// Reinterpret cast.
#define REINTERPRET( type, value ) /// -> typeof(type)
```
```cpp
/// Get the byte offset of field in struct.
#define offsetof( struct, field ) /// -> usize
```
```cpp
/// spread array elements
/// useful for unpacking arrays
/// for functions which take multiple elements
#define SPREAD_2( array )
#define SPREAD_3( array )
#define SPREAD_4( array )
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

## Constants

| Misc Constant      | Type        | Description                                        |
| ------------------ | ----------- | -------------------------------------------------- |
| true/false         | bool        | boolean constants (C only)                         |
| nullptr            | void*       | null constant (C only)                             |
| NULL               | void*       | null constant                                      |
| STACK_SIZE         | usize       | size of the stack and stack size of worker threads |
| LD_CONTACT_MESSAGE | const char* | contact message to display in user-facing errors   |

| Float 32 Constant           | Description                                    |
| --------------------------- | ---------------------------------------------- |
| F32_MIN                     | smallest finite single-precision float value   |
| F32_MIN_POS                 | smallest positive single-precision float value |
| F32_MAX                     | largest finite single-precision float value    |
| F32_NAN                     | not a number                                   |
| F32_NEG_INFINITY            | negative infinity                              |
| F32_POS_INFINITY            | positive infinity                              |
| F32_PI                      | pi constant                                    |
| F32_TAU                     | tau (2 * pi) constant                          |
| F32_HALF_PI                 | pi / 2 constant                                |
| F32_EPSILON                 | epsilon constant                               |
| F32_SIGNIFICANT_DIGITS      | number of significant digits in base-10        |
| F32_MANTISSA_DIGITS         | number of signicant digits in base-2           |
| F32_EXPONENT_MASK           | bitmask of single-precision float exponent     |
| F32_MANTISSA_MASK           | bitmask of single-precision float mantissa     |
| F32_(ONE-TWELVE)_FACTORIAL  | 1-12 factorial constants                       |

| Float 64 Constant           | Description                                    |
| --------------------------- | ---------------------------------------------- |
| F64_MIN                     | smallest finite double-precision float value   |
| F64_MIN_POS                 | smallest positive double-precision float value |
| F64_MAX                     | largest finite double-precision float value    |
| F64_NAN                     | not a number                                   |
| F64_NEG_INFINITY            | negative infinity                              |
| F64_POS_INFINITY            | positive infinity                              |
| F64_PI                      | pi constant                                    |
| F64_TAU                     | tau (2 * pi) constant                          |
| F64_HALF_PI                 | pi / 2 constant                                |
| F64_EPSILON                 | epsilon constant                               |
| F64_SIGNIFICANT_DIGITS      | number of significant digits in base-10        |
| F64_MANTISSA_DIGITS         | number of signicant digits in base-2           |
| F64_EXPONENT_MASK           | bitmask of double-precision float exponent     |
| F64_MANTISSA_MASK           | bitmask of double-precision float mantissa     |
| F64_(ONE-TWELVE)_FACTORIAL  | 1-12 factorial constants                       |

| Unsigned 8-bit Integer Constant | Description                     |
| ------------------------------- | ------------------------------- |
| U8_MIN                          | smallest unsigned 8-bit integer |
| U8_MAX                          | largest unsigned 8-bit integer  |

| Unsigned 16-bit Integer Constant | Description                      |
| -------------------------------- | -------------------------------- |
| U16_MIN                          | smallest unsigned 16-bit integer |
| U16_MAX                          | largest unsigned  16-bit integer |

| Unsigned 32-bit Integer Constant | Description                      |
| -------------------------------- | -------------------------------- |
| U32_MIN                          | smallest unsigned 32-bit integer |
| U32_MAX                          | largest unsigned  32-bit integer |

| Unsigned 64-bit Integer Constant | Description                      |
| -------------------------------- | -------------------------------- |
| U64_MIN                          | smallest unsigned 64-bit integer |
| U64_MAX                          | largest unsigned  64-bit integer |

| 8-bit Integer Constant | Description             |
| ---------------------- | ----------------------- |
| I8_MIN                 | smallest 8-bit integer  |
| I8_MAX                 | largest  8-bit integer  |
| I8_SIGN_MASK           | sign bitmask            |

| 16-bit Integer Constant | Description              |
| ----------------------- | ------------------------ |
| I16_MIN                 | smallest  16-bit integer |
| I16_MAX                 | largest   16-bit integer |
| I16_SIGN_MASK           | sign bitmask             |

| 32-bit Integer Constant | Description              |
| ----------------------- | ------------------------ |
| I32_MIN                 | smallest  32-bit integer |
| I32_MAX                 | largest   32-bit integer |
| I32_SIGN_MASK           | sign bitmask             |

| 64-bit Integer Constant | Description              |
| ----------------------- | ------------------------ |
| I64_MIN                 | smallest  64-bit integer |
| I64_MAX                 | largest   64-bit integer |
| I64_SIGN_MASK           | sign bitmask             |

| Pointer Integer Constant | Description              |
| ------------------------ | ------------------------ |
| ISIZE_MIN                | smallest pointer integer |
| ISIZE_MAX                | largest  pointer integer |
| ISIZE_SIGN_MASK          | sign bitmask             |

| Unsigned Pointer Integer Constant | Description                       |
| --------------------------------- | --------------------------------- |
| USIZE_MIN                         | smallest unsigned pointer integer |
| USIZE_MAX                         | largest  unsigned pointer integer |

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

