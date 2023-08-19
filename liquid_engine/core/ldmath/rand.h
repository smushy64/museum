#if !defined(CORE_MATH_RAND_HPP)
#define CORE_MATH_RAND_HPP
/**
 * Description:  Math Random Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 09, 2023
*/
#include "defines.h"
#include "core/ldmath/functions.h"

#define RAND_INT_TO_FLOAT( x )\
    lerp32(\
        -1.0f,\
        1.0f,\
        inverse_lerp32(\
            (f32)I32_MIN,\
            (f32)I32_MAX,\
            (f32)(x)\
        )\
    )

#define RAND_INT_TO_FLOAT_01( x )\
    inverse_lerp32(\
        (f32)U32_MIN,\
        (f32)U32_MAX,\
        (f32)(x)\
    )

/// random number generator lcg state
typedef struct RandLCG {
    i32 seed;
    i32 current;
    i32 a, b, m;
} RandLCG;
/// create new rand lcg state
headerfn RandLCG rand_lcg_new( i32 seed ) {
    RandLCG result = {};
    result.seed    = seed;
    result.current = result.seed;
    result.a       = 166463;
    result.b       = 235;
    result.m       = 21446837;
    return result;
}
/// random signed int in I32_MIN to I32_MAX range
headerfn i32 rand_lcg_next_i32( RandLCG* state ) {
    state->current = ( state->a * state->current + state->b ) % state->m;
    return state->current;
}
/// random unsigned int in 0 to U32_MAX range
headerfn u32 rand_lcg_next_u32( RandLCG* state ) {
    i32 next = rand_lcg_next_i32( state );
    return *(i32*)&next;
}
/// random float in -1 to 1 range
headerfn f32 rand_lcg_next_f32( RandLCG* state ) {
    return RAND_INT_TO_FLOAT( rand_lcg_next_i32( state ) );
}
/// random float in 0 to 1 range
headerfn f32 rand_lcg_next_f32_01( RandLCG* state ) {
    return RAND_INT_TO_FLOAT_01( rand_lcg_next_u32( state ) );
}

/// random number generator xor shift state
typedef struct RandXOR {
    u32 seed;
    u32 current;
} RandXOR;
/// create new rand xor state
headerfn RandXOR rand_xor_new( u32 seed ) {
    RandXOR result = {};
    result.seed    = seed;
    result.current = result.seed;
    return result;
}
/// random unsigned int in 0 to U32_MAX range
headerfn u32 rand_xor_next_u32( RandXOR* state ) {
    ASSERT(state->seed);
    u32 x = state->current;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return state->current = x;
}
/// random signed int in I32_MIN to I32_MAX range
headerfn i32 rand_xor_next_i32( RandXOR* state ) {
    u32 absolute = rand_xor_next_u32( state );
    return (i32)absolute * ( absolute % 2 ? -1 : 1 );
}
/// random float in -1 to 1 range
headerfn f32 rand_xor_next_f32( RandXOR* state ) {
    return RAND_INT_TO_FLOAT( rand_xor_next_i32( state ) );
}
/// random float in 0 to 1 range
headerfn f32 rand_xor_next_f32_01( RandXOR* state ) {
    return RAND_INT_TO_FLOAT_01( rand_xor_next_u32( state ) );
}

#endif // header guard
