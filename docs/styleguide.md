<!--
 * Description:  Liquid Engine Style Guide
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
-->

# Liquid Engine Style Guide

Loose rules for how Liquid Engine code should be formatted.

## Variables/Constants

All variable names are lower snake case.
All constants are upper snake case.
All global variables are lower snake case with global_ prefix.

ex:
```cpp
    // constant
    #define FOO (10)
    global const u32 BAR = 20;

    // global variable
    global b32 global_bar = true;
    void foo(void) {
        // local variable
        b32 bar = true;
    }
```

## Struct, Unions, Enums

All structs, unions and enum names are upper CamelCase.
The only exception is the math library, where types are usually lower case.

```cpp
struct FooBar {
    u32 foo;
    u32 bar;
};
typedef struct FooBar FooBar;

typedef union {
    f32 f;
    u32 i;
} FloatInt;

// mathf example:
typedef union vec2 vec2;
union vec2 {
    struct { f32 x, y; };
    struct { f32 width, height; };
    f32 c[2];
};
```

All enum variants are upper snake case and start with enum name.
All enums should be explicitly typed.
Enums should generally have a count variant but it's not a hard requirement.

```cpp
typedef enum EnumExample : u32 {
    ENUM_EXAMPLE_ZERO,
    ENUM_EXAMPLE_ONE,

    ENUM_EXAMPLE_COUNT
} EnumExample;
```

Structs, Unions and Enums should usually be defined on its own unless
it is internal to a source file.
Typedefs are optional.

```cpp

// bad
typedef struct {
    b32 foo;
} Foo;

// better
struct Foo {
    b32 foo;
};

// good
typedef struct Foo {
    b32 foo;
} Foo;

```

Common Patterns:

- tagged union
```cpp
typedef enum UnionType : u32 {
    UNION_TYPE_FLOAT,
    UNION_TYPE_INT32,

    UNION_TYPE_COUNT
} UnionType;

typedef struct Union {
    UnionType type;
    // anonymous unions/structs are used all throughout the code.
    union {
        f32 f;
        i32 i;
    };
} Union;

void foo(void) {
    Union bar = {};
    bar.type = UNION_TYPE_FLOAT;
    bar.f    = 10.0f;

    switch( bar.type ) {
        case UNION_TYPE_FLOAT: {
            println( "{f}", bar.f );
        } break;
        case UNION_TYPE_INT32: {
            println( "{i}", bar.i );
        } break;
        default: panic();
    }
}

```

## Comments

All files should start with a file description with the same formatting.

ex:
```cpp
/**
 * Description:  This is a file.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
*/
```

Single line comments are allowed.
Unless the comment is of minor importance, it should generally
be formatted like so:

```cpp
// TODO(alicia): this is a todo comment.
// NOTE(alicia): this is a note comment.
// IMPORTANT(alicia): this is a very important comment.
// FIXME(alicia): this is a fix me comment.
```

Documentation comments should be single line and start with three forward slashes.

ex:
```cpp
/// This is a foo.
/// Returns true if successful.
b32 foo(void);
```

## Functions

All function names are lower snake case.
Return type should always be inline with function name.

ex:
```cpp
void function_foo(void);
```

The only exception to this rule is if a function is part of another API.

ex:
```cpp
void glSwapBuffers( void* surface );
```

When a function call/declaration is too long to fit in one line,
separate arguments by new line only when they reach the end of the line.

Closing brace should be on the same line as the last argument in most cases.

ex:
```cpp
    long_function_call(
        argument_a, argument_b, argument_c,
        argument_d, argument_e, argument_f );
```

Attributes should be inline with function name or above name if they're too long
for one line.

ex:
```cpp
LD_API void foo(void);
```

```cpp
global no_inline LD_API c_linkage
void foo(void);
```

## Macros

All macro constants should follow the same rules as regular constants
and they should contain their value in parenthesis to make them easier to work with,
the only exception being string constants.

ex:
```cpp
#define FOO (10)
#define BAR "Hello, World!\n"

```

Macro functions are formatted the same way regular functions are formatted.
Expression and arguments should be wrapped in parenthesis when being used
so that when the macro is expanded, it expands within the parenthesis.

ex:
```cpp
#define square(x) ( (x) * (x) )
```

## Typedef

Opaque handles should be typedef'd like so:
```cpp
typedef void Foo;
```

This is so that the fact that `Foo` is a void pointer is not hidden when used.

Function prototypes should be typedef'd like so:
```cpp
typedef b32 FooFN( i32 x );

FooFN* foo = NULL;

```

All function typedefs should be formatted the same as struct but with FN as a prefix.

