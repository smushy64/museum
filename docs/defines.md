<!--
 * Description:  Defines
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# Defines | [Table of Contents](./toc.md)

List of everything defined in "defines.h"

Include in game DLL:
```cpp
#include <defines.h>
```
Include in engine:
```cpp
#include "defines.h"
```

## Jump Table
- [Type Aliases](#type-aliases)
- [Attributes](#attributes)
- [Macros](#macros)
- [Constants](#constants)
- [Platform Identifiers](#platform-identifiers)

## Type Aliases 
| Type  | Description                              |
| ----- | ---------------------------------------- |
| usize | unsigned pointer-sized integer           |
| isize |          pointer-sized integer           |
| u8    | unsigned  8-bit        integer           |
| u16   | unsigned 16-bit        integer           |
| u32   | unsigned 32-bit        integer           |
| u64   | unsigned 64-bit        integer           |
| i8    |           8-bit        integer           |
| i16   |          16-bit        integer           |
| i32   |          32-bit        integer           |
| i64   |          64-bit        integer           |
| f32   | single-precision IEEE-754 floating point |
| f64   | double-precision IEEE-754 floating point |
| b8    |  8-bit boolean                           |
| b32   | 32-bit boolean                           |
| c8    | UTF-8 character                          |
| c16   | UTF-16/Windows Unicode character         |
| c32   | UTF-32 character                         |
| pvoid | void pointer                             |

## Attributes
| Attribute       | Description                                    |
| --------------- | ---------------------------------------------- |
| internal        | static - for functions in one translation unit |
| local           | static - for local static variables            |
| global          | static - for global static variables           |
| NO_OPTIMIZE     | do not optimize following function             |
| HOT_PATH        | always optimize following function             |
| FORCE_INLINE    | always inline following function               |
| NO_INLINE       | never inline following function                |
| MAKE_PACKED()   | define a struct without any padding            |
| LD_API          | export/import function                         |
| LD_API_STRUCT   | export/import struct                           |

## Macros
| Macro                               | Description                        |
| ----------------------------------- | ---------------------------------- |
| LD_MAKE_VERSION( major, minor )     | make version u32 identifier        |
| LD_GET_MAJOR( version )             | major version from u32 identifier  |
| LD_GET_MINOR( version )             | minor version from u32 identifier  |
| unused( x )                         | mark value as unused               |
| loop                                | for( ; ; ) infinite loop           |
| PANIC()                             | debugbreak/trap alias              |
| STATIC_ASSERT( condition, message ) | compile time assertion             |
| ASSERT( condition )                 | runtime assertion                  |
| CHECK_BITS( bits, mask )            | check if bits are set in bitfield  |
| TOGGLE_BITS( bits, mask )           | toggle bits in bitfield            |
| CLEAR_BIT( bits, mask )             | clear bits in bitfield             |
| SWAP( a, b )                        | swap values                        |
| KILOBYTES( num )                    | num * 1024                         |
| MEGABYTES( num )                    | KILOBYTES( num ) * 1024            |
| GIGABYTES( num )                    | MEGABYTES( num ) * 1024            |
| TO_STRING( macro )                  | macro to string                    |
| VALUE_TO_STRING( macro )            | macro value to string              |
| RGB_U32( r, g, b )                  | define a 24-bit RGB value (u32)    |
| RGBA_U32( r, g, b, a )              | define a 32-bit RGBA value         |
| STATIC_ARRAY_COUNT( array )         | number of elements in static array |
| STATIC_ARRAY_SIZE( array )          | byte size of static array          |
| LD_API_INTERNAL                     | only defined in internal source    |

## Constants
| Float 32 Constant           | Description                                    |
| --------------------------- | ---------------------------------------------- |
| F32::MIN                    | smallest finite single-precision float value   |
| F32::MIN_POS                | smallest positive single-precision float value |
| F32::MAX                    | largest finite single-precision float value    |
| F32::NAN                    | not a number                                   |
| F32::NEG_INFINITY           | negative infinity                              |
| F32::POS_INFINITY           | positive infinity                              |
| F32::PI                     | pi constant                                    |
| F32::TAU                    | tau (2 * pi) constant                          |
| F32::HALF_PI                | pi / 2 constant                                |
| F32::EPSILON                | epsilon constant                               |
| F32::SIGNIFICANT_DIGITS     | number of significant digits in base-10        |
| F32::MANTISSA_DIGITS        | number of signicant digits in base-2           |
| F32::EXPONENT_MASK          | bitmask of single-precision float exponent     |
| F32::MANTISSA_MASK          | bitmask of single-precision float mantissa     |
| F32::(ONE-TWELVE)_FACTORIAL | 1-12 factorial constants                       |

| Float 64 Constant           | Description                                    |
| --------------------------- | ---------------------------------------------- |
| F64::MIN                    | smallest finite double-precision float value   |
| F64::MIN_POS                | smallest positive double-precision float value |
| F64::MAX                    | largest finite double-precision float value    |
| F64::NAN                    | not a number                                   |
| F64::NEG_INFINITY           | negative infinity                              |
| F64::POS_INFINITY           | positive infinity                              |
| F64::PI                     | pi constant                                    |
| F64::TAU                    | tau (2 * pi) constant                          |
| F64::HALF_PI                | pi / 2 constant                                |
| F64::EPSILON                | epsilon constant                               |
| F64::SIGNIFICANT_DIGITS     | number of significant digits in base-10        |
| F64::MANTISSA_DIGITS        | number of signicant digits in base-2           |
| F64::EXPONENT_MASK          | bitmask of double-precision float exponent     |
| F64::MANTISSA_MASK          | bitmask of double-precision float mantissa     |
| F64::(ONE-TWELVE)_FACTORIAL | 1-12 factorial constants                       |

| Unsigned 8-bit Integer Constant | Description                     |
| ------------------------------- | ------------------------------- |
| U8::MIN                         | smallest unsigned 8-bit integer |
| U8::MAX                         | largest unsigned 8-bit integer  |

| Unsigned 16-bit Integer Constant | Description                      |
| -------------------------------- | -------------------------------- |
| U16::MIN                         | smallest unsigned 16-bit integer |
| U16::MAX                         | largest unsigned  16-bit integer |

| Unsigned 32-bit Integer Constant | Description                      |
| -------------------------------- | -------------------------------- |
| U32::MIN                         | smallest unsigned 32-bit integer |
| U32::MAX                         | largest unsigned  32-bit integer |

| Unsigned 64-bit Integer Constant | Description                      |
| -------------------------------- | -------------------------------- |
| U64::MIN                         | smallest unsigned 64-bit integer |
| U64::MAX                         | largest unsigned  64-bit integer |

| 8-bit Integer Constant | Description             |
| ---------------------- | ----------------------- |
| I8::MIN                | smallest 8-bit integer  |
| I8::MAX                | largest  8-bit integer  |
| I8::SIGN_MASK          | sign bitmask            |

| 16-bit Integer Constant | Description              |
| ----------------------- | ------------------------ |
| I16::MIN                | smallest  16-bit integer |
| I16::MAX                | largest   16-bit integer |
| I16::SIGN_MASK          | sign bitmask             |

| 32-bit Integer Constant | Description              |
| ----------------------- | ------------------------ |
| I32::MIN                | smallest  32-bit integer |
| I32::MAX                | largest   32-bit integer |
| I32::SIGN_MASK          | sign bitmask             |

| 64-bit Integer Constant | Description              |
| ----------------------- | ------------------------ |
| I64::MIN                | smallest  64-bit integer |
| I64::MAX                | largest   64-bit integer |
| I64::SIGN_MASK          | sign bitmask             |


## Platform Identifiers
| Compiler                | Description                               |
| ----------------------- | ----------------------------------------- |
| LD_COMPILER_GCC         | GNU Compiler Collection is in use         |
| LD_COMPILER_CLANG       | Clang compiler is in use                  |
| LD_COMPILER_MSVC[^1]    | Microsoft Visual C++ compiler is in use   |
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
| LD_ARCH_X86            | Current architecture is Intel x86 based |
| LD_ARCH_ARM[^1]        | Current architecture is ARM based       |
| LD_ARCH_WASM [^1] [^2] | Current architecture is WASM based      |

### Footnotes

[^1]: not currently supported

[^2]: not currently defined
