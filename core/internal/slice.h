#if !defined(LD_CORE_INTERNAL_SLICE_H)
#define LD_CORE_INTERNAL_SLICE_H
/**
 * Description:  Slice types.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 18, 2024
*/
#include "shared/defines.h"

/// Slice of a string buffer.
struct StringSlice {
    char* buffer;
    usize len;
};

/// Mutable string buffer.
struct StringBuffer {
    char* buffer;
    usize len;
    usize capacity;
};

/// Convert buffer to slice.
#define to_slice( x )\
    _Generic( (x),\
        struct StringBuffer*: *(struct StringSlice*)(x)\
    )

#endif /* header guard */
