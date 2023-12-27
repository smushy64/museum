<!--
 * Description:  Liquid Engine Core Constants
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
-->

# Liquid Engine Shared Constants | [Table of Contents](../readme.md)

To include:
```cpp
#include "shared/constants.h"
```

## Jump Table
- [Float32](#float32)
- [Float64](#float64)
- [Uint8](#uint8)
- [Uint16](#uint16)
- [Uint32](#uint32)
- [Uint64](#uint64)
- [Int8](#int8)
- [Int16](#int16)
- [Int32](#int32)
- [Int64](#int64)
- [IntPtr](#intptr)
- [UintPtr](#uintptr)
- [Misc](#misc)

### Float32

| Name                       | Description                                    |
| -------------------------- | ---------------------------------------------- |
| F32_MIN                    | smallest finite single-precision float value   |
| F32_MIN_POS                | smallest positive single-precision float value |
| F32_MAX                    | largest finite single-precision float value    |
| F32_NAN                    | not a number                                   |
| F32_NEG_INFINITY           | negative infinity                              |
| F32_POS_INFINITY           | positive infinity                              |
| F32_PI                     | pi constant                                    |
| F32_TAU                    | tau (2 * pi) constant                          |
| F32_HALF_PI                | pi / 2 constant                                |
| F32_EPSILON                | epsilon constant                               |
| F32_SIGNIFICANT_DIGITS     | number of significant digits in base-10        |
| F32_MANTISSA_DIGITS        | number of signicant digits in base-2           |
| F32_EXPONENT_MASK          | bitmask of single-precision float exponent     |
| F32_MANTISSA_MASK          | bitmask of single-precision float mantissa     |
| F32_(ONE-TWELVE)_FACTORIAL | 1-12 factorial constants                       |

### Float64

| Name                       | Description                                    |
| -------------------------- | ---------------------------------------------- |
| F64_MIN                    | smallest finite double-precision float value   |
| F64_MIN_POS                | smallest positive double-precision float value |
| F64_MAX                    | largest finite double-precision float value    |
| F64_NAN                    | not a number                                   |
| F64_NEG_INFINITY           | negative infinity                              |
| F64_POS_INFINITY           | positive infinity                              |
| F64_PI                     | pi constant                                    |
| F64_TAU                    | tau (2 * pi) constant                          |
| F64_HALF_PI                | pi / 2 constant                                |
| F64_EPSILON                | epsilon constant                               |
| F64_SIGNIFICANT_DIGITS     | number of significant digits in base-10        |
| F64_MANTISSA_DIGITS        | number of signicant digits in base-2           |
| F64_EXPONENT_MASK          | bitmask of double-precision float exponent     |
| F64_MANTISSA_MASK          | bitmask of double-precision float mantissa     |
| F64_(ONE-TWELVE)_FACTORIAL | 1-12 factorial constants                       |

### Uint8

| Name   | Description                     |
| ------ | ------------------------------- |
| U8_MIN | smallest unsigned 8-bit integer |
| U8_MAX | largest unsigned 8-bit integer  |

### Uint16

| Name    | Description                      |
| ------- | -------------------------------- |
| U16_MIN | smallest unsigned 16-bit integer |
| U16_MAX | largest unsigned  16-bit integer |

### Uint32

| Name    | Description                      |
| ------- | -------------------------------- |
| U32_MIN | smallest unsigned 32-bit integer |
| U32_MAX | largest unsigned  32-bit integer |

### Uint64

| Name    | Description                      |
| ------- | -------------------------------- |
| U64_MIN | smallest unsigned 64-bit integer |
| U64_MAX | largest unsigned  64-bit integer |

### Int8

| Name         | Description             |
| ------------ | ----------------------- |
| I8_MIN       | smallest 8-bit integer  |
| I8_MAX       | largest  8-bit integer  |
| I8_SIGN_MASK | sign bitmask            |

### Int16

| Name           | Description              |
| -------------- | ------------------------ |
| I16_MIN        | smallest  16-bit integer |
| I16_MAX        | largest   16-bit integer |
| I16_SIGN_MASK  | sign bitmask             |

### Int32 

| Name           | Description              |
| -------------- | ------------------------ |
| I32_MIN        | smallest  32-bit integer |
| I32_MAX        | largest   32-bit integer |
| I32_SIGN_MASK  | sign bitmask             |

### Int64

| Name           | Description              |
| -------------- | ------------------------ |
| I64_MIN        | smallest  64-bit integer |
| I64_MAX        | largest   64-bit integer |
| I64_SIGN_MASK  | sign bitmask             |

### UintPtr

| Name       | Description                       |
| ---------- | --------------------------------- |
| USIZE_MIN  | smallest unsigned pointer integer |
| USIZE_MAX  | largest  unsigned pointer integer |

### IntPtr

| Name             | Description              |
| ---------------- | ------------------------ |
| ISIZE_MIN        | smallest pointer integer |
| ISIZE_MAX        | largest  pointer integer |
| ISIZE_SIGN_MASK  | sign bitmask             |

### Misc

| Name       | Type   | Description                                        |
| ---------- | ------ | -------------------------------------------------- |
| STACK_SIZE | usize  | size of the stack and stack size of worker threads |


