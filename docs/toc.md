<!--
 * Description:  Liquid Engine API Documentation Table of Contents
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 19, 2023
-->

# Liquid Engine API

Liquid Engine is my custom game engine written in C.

Notes:

All functions currently assume that architecture is Little Endian.
This may have to change if I ever target a Big Endian architecture but
that is unlikely to happen.

## Table of Contents
- [Build Guide](../BUILD.md)
    - [Requirements](../BUILD.md#requirements)
        - [Common Requirements](../BUILD.md#common-requirements)
        - [Windows Requirements](../BUILD.md#windows-requirements)
        - [Linux Requirements](../BUILD.md#linux-requirements)
        - [Optional Software](../BUILD.md#optional-software)
    - [Debug Build Steps](../BUILD.md#debug-build-steps)
    - [Release Build Steps](../BUILD.md#release-build-steps)
    - [Debugging](../BUILD.md#debugging)
        - [Entry Points](../BUILD.md#entry-points)
        - [RemedyBG](../BUILD.md#remedybg)
        - [gf](../BUILD.md#gf)
        - [Visual Studio](../BUILD.md#visual-studio)
- [Defines](./defines.md)
    - [Macro Functions](./defines.md#macro-functions)
    - [Type Aliases](./defines.md#type-aliases)
    - [Attributes](./defines.md#attributes)
    - [Conditionals](./defines.md#conditionals)
- [Constants](./constants.md)
    - [Float32](./constants.md#float32)
    - [Float64](./constants.md#float64)
    - [Uint8](./constants.md#uint8)
    - [Uint16](./constants.md#uint16)
    - [Uint32](./constants.md#uint32)
    - [Uint64](./constants.md#uint64)
    - [Int8](./constants.md#int8)
    - [Int16](./constants.md#int16)
    - [Int32](./constants.md#int32)
    - [Int64](./constants.md#int64)
    - [IntPtr](./constants.md#intptr)
    - [UintPtr](./constants.md#uintptr)
    - [Misc](./constants.md#misc)
- [String](./string.md)
    - [Types](./string.md#types)
    - [Functions](./string.md#functions)
        - [Character](./string.md#character)
        - [String Slice](./string.md#string-slice)
        - [IO](./string.md#io)
    - [Macro Functions](./string.md#macro-functions)
    - [Format Specifiers](./string.md#format-specifiers)
    - [Format Specifier Parameters](./string.md#format-specifier-parameters)
        - [Parameters for Any Specifier](./string.md#parameters-for-any-specifier)
        - [Parameters for Number Specifiers](./string.md#parameters-for-number-specifiers)
        - [Parameters for Integer Specifiers](./string.md#parameters-for-integer-specifiers)
        - [Parameters for Float Specifiers](./string.md#parameters-for-float-specifiers)
        - [Parameters for Boolean Specifiers](./string.md#parameters-for-boolean-specifiers)
    - [Formatting Examples](./string.md#formatting-examples)
- [C String](./cstr.md)
    - [Functions](./cstr.md#functions)

