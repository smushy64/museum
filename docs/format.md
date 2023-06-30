# String Formatting

```
Note: no specifier/parameter is case sensitive
```

## Format Specifiers.
use {} to wrap a format specifier and use commas to separate parameters

| Specifier        | Description                              |
| ---------------- | ---------------------------------------- |
| {{               | literal '{'                              |
| c                | 8-bit ASCII character                    |
| cc               | const char* ASCII null-terminated string |
| s                | String                                   |
| sv               | StringView                               |
| u                | unsigned int32                           |
| u8/16/32/64      | unsigned sized int                       |
| i                | int32                                    |
| i8/16/32/64      | sized int                                |
| iv2/3/4          | int vectors                              |
| b                | boolean                                  |
| f                | float (f32 gets promoted to f64)         |
| v2/3/4           | float vectors                            |
| q                | quaternion                               |

## Specifier parameters.
### Any Specifier
| Parameter | Description                                                    |
| --------- | -------------------------------------------------------------- |
| int       | number of padding spaces; can be negative to pad to right side |

### Any Number Specifier (u/i/f/v/iv/q)
| Parameter         | Description                                  |
| ----------------- | -------------------------------------------- |
| 0 followed by int | number of padding zeroes, cannot be negative |

### Int specifiers (u/i/iv)
| Parameter | Description        |
| --------- | ------------------ |
| x         | hexadecimal format |
| b         | binary format      |

### Float specifiers (f/v/q)
| Parameter                 | Description                              |
| ------------------------- | ---------------------------------------- |
| . followed by int         | fractional precision, cannot be negative |
| b (does nothing with v/q) | storage formatting (bytes/kb/mb/gb)      |

### Boolean specifiers (b)
| Parameter | Description                   |
| --------- | ----------------------------- |
| b         | use 0/1 instead of true/false |

# Examples
```cpp
print( "{i} {f} {cc}", 10, -5.5f, "Hello world" );
```
```
10 -5.500000 Hello World
```
```cpp
print( "{f}|{f,3} {f,03.3}", -2.0f, 5.0f 1.25f );
```
```
-2.000000|  5.000000 001.250
```
```cpp
print( "|{cc,-6}|", "hi" );
```
```
|hi    |
```
```cpp
print( "|{cc,6}|", "hi" );
```
```
|    hi|
```

