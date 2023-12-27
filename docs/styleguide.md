<!--
 * Description:  Liquid Engine Style Guide
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
-->

# Liquid Engine Style Guide

Loose rules for how Liquid Engine code should be formatted.

## Typedef

UpperCamelCase formatting

ex:
```cpp
typedef u32 UnsignedInteger32;
```

Opaque handles should be defined as void so that the fact that they're pointers is not obscured.

ex:
```cpp
typedef void FooHandle;

FooHandle* foo = NULL;
```

Function prototypes should be UpperCamelCase followed by 'FN' suffix.

ex:
```cpp
typedef b32 FooFN( i32 x );

FooFN* foo = my_foo;
```

## Variables

lower_snake_case formatting.

ex:
```cpp
void foo(void) {
    u32 this_is_an_integer = 10;
}
```

Globals should be prefixed with 'global_' and *global* attribute.

ex:
```cpp
global u32 global_foo = 10;

void foo(void) {
    u32 bar = global_foo;
}
```

## Constants

UPPER_SNAKE_CASE formatting.

ex:
```cpp
#define THIS_IS_A_CONSTANT (10)
global const u32 THIS_IS_ALSO_A_CONSTANT = 20;
```

## Structs

UpperCamelCase formatting.

When defined in a header, it should also be typedef'd.

ex:
```cpp
struct FooBar {
    u32 foo;
    u32 bar;
};
typedef struct FooBar FooBar;

typedef union BarBaz {
    i32 foo;
    f32 bar;
} BarBaz;
```

## Enums

UpperCamelCase formatting for type.

UPPER_SNAKE_CASE formatting for variants.

Enums should be explicitly typed.

Most enums should end with a COUNT variant.

Variants should start with UPPER_SNAKE_CASE version of its type name.

Variants should have a trailing comma except for COUNT variant.

ex:
```cpp
typedef enum EnumExample : u32 {
    ENUM_EXAMPLE_ZERO,
    ENUM_EXAMPLE_ONE,

    ENUM_EXAMPLE_COUNT,
} EnumExample;
```

## Functions

lower_snake_case formatting.

Return type should be inline with function prototype.

ex:
```cpp
void foo(void);
```

External functions should follow their API's conventions.

ex:
```cpp
/// Formatted the same way other OpenGL functions are formatted.
void glSwapBuffers( void* surface );
```

Separate function arguments into new lines when they're too long
to fit on the same line.

ex:
```cpp
void long_function_prototype(
    int argument_a, int argument_b, int argument_c,
    int argument_d, int argument_e, int argument_f );
```

Function attributes should be inline or directly above function prototype.

ex:
```cpp
LD_API void foo(void);

global no_inline LD_API c_linkage
void foo(void);
```

## Macros

Macro functions follow the same rules as regular functions.

Macro function arguments should be inside parenthesis when used in
function body unless there's a requirement that they shouldn't be.

ex:
```cpp
#define square( x ) ( (x) * (x) )
```

Macro constants follow the same rules as regular constants.

ex:
```cpp
#define U8_MAX (255)
```

With the exception of string macro constants, value should be inside parenthesis.

ex:
```cpp
#define U8_MAX      (255)
#define SOME_STRING "hello, world!\n"
```

Macro functions and constants defined within a scope should be undefined at the end of the scope.

ex:
```cpp
void foo(void) {
    #define A_FUNCTION(x) ( (x) * 10 )
    #undef A_FUNCTION
}
```

## Comments

All files should start with a file description with the following formatting.

ex:
```cpp
/**
 * Description:  This is a file.
 * Author:       *author's name* (*author's email*)
 * File Created: *Month* *day (1-31)*, *year*
*/
```

Common comment formatting:

ex:
```cpp
// TODO(alicia): this comment acknowledges something that's yet to be done.
// NOTE(alicia): this comment denotes something other developers should know.
// IMPORTANT(alicia): this comment denotes something very important.
// FIXME(alicia): this comment denotes a buggy line/function.
```

Documentation comments should be single line comments and start with three forward slashes.

ex:
```cpp
/// This is a foo.
/// Returns true if successful.
b32 foo(void);
```

