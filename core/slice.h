#if !defined(LD_CORE_SLICE_H)
#define LD_CORE_SLICE_H
/**
 * Description:  Slice types.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 05, 2024
*/
#include "shared/defines.h"

/// Slice of values.
struct CoreSlice {
    union {
        const char* str;
        char*       c;
        void*       v;
        u8*         uint8;
        u16*        uint16;
        u32*        uint32;
        u64*        uint64;
        i8*         int8;
        i16*        int16;
        i32*        int32;
        i64*        int64;
        f32*        float32;
        f64*        float64;
    };
    usize len;
};

/// Buffer of values.
struct CoreBuffer {
    union {
        const char* str;
        char*       c;
        void*       v;
        u8*         uint8;
        u16*        uint16;
        u32*        uint32;
        u64*        uint64;
        i8*         int8;
        i16*        int16;
        i32*        int32;
        i64*        int64;
        f32*        float32;
        f64*        float64;
    };
    usize len;
    usize cap;
};

/// Convert buffer to slice.
#define to_slice( buf )\
    (*((struct CoreSlice*)(buf)))

#endif /* header guard */
