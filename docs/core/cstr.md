<!--
 * Description:  Liquid Engine Core Cstr Documentation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
-->

# C Strings | [Table of Contents](../readme.md)

To include:
```cpp
#include <core/cstr.h>
```

Includes:
- "defines.h"

## Jump Table
- [Functions](#functions)

## Functions

```cpp
/// Calculate null-terminated string length.
usize cstr_len( const char* cstr );
```
```cpp
/// Compare null-terminated strings.
b32 cstr_cmp( const char* a, const char* b );
```
```cpp
/// Copy source to destination string.
void cstr_copy( char* dst, const char* src, usize len );
```
```cpp
/// Output a null-terminated string to standard out.
void cstr_output_stdout( const char* cstr );
```
```cpp
/// Output a null-terminated string to standard error.
void cstr_output_stderr( const char* cstr );
```

