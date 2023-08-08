#if !defined(CORE_MATH_RAND_HPP)
#define CORE_MATH_RAND_HPP
/**
 * Description:  Math Random Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 09, 2023
*/
#include "defines.h"
#include "functions.h"

#define RAND_INT_TO_FLOAT( x )\
    lerp(\
        -1.0f,\
        1.0f,\
        inverse_lerp(\
            (f32)I32::MIN,\
            (f32)I32::MAX,\
            (f32)(x)\
        )\
    )

#define RAND_INT_TO_FLOAT_01( x )\
    inverse_lerp(\
        (f32)U32::MIN,\
        (f32)U32::MAX,\
        (f32)(x)\
    )

/// random number generator.
/// linear congruential generator algorithm
struct RandLCG {
    i32 seed;
    i32 current;
    i32 a, b, m;

    RandLCG() :
        seed( 34634623 ),
        current( seed ),
        a( 166463 ),
        b( 235 ),
        m( 21446837 )
    {}
    RandLCG( i32 seed ) :
        seed( seed ),
        current( seed ),
        a( 166463 ),
        b( 235 ),
        m( 21446837 )
    {}
    explicit RandLCG( i32 seed, i32 a, i32 b, i32 m ) :
        seed( seed ),
        current( seed ),
        a(a),
        b(b),
        m(m)
    {}  

    /// random signed int in I32::MIN to I32::MAX range
    i32 next_i32() {
        current = ( a * -current + b ) % m;
        return current;
    }
    /// random unsigned int in 0 to U32::MAX range
    u32 next_u32() {
        i32 next = next_i32();
        return *(u32*)&next;
    }
    /// random float in -1 to 1 range
    f32 next_f32() {
        return RAND_INT_TO_FLOAT( next_i32() );
    }
    /// random float in 0 to 1 range
    f32 next_f32_01() {
        return RAND_INT_TO_FLOAT_01( next_u32() );
    }
};

/// random number generator.
/// xor shift algorithm
struct RandXOR {
    u32 seed;
    u32 current;

    RandXOR()
    : seed( 34634623 ), current(seed) {}

    explicit RandXOR( u32 seed )
    : seed( max( seed, 1u ) ), current(seed) {}

    /// random unsigned integer in range 0 to U32::MAX
    u32 next_u32() {
        ASSERT(seed);
        u32 x = this->current;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;

        return this->current = x;
    }
    /// random signed integer in range I32::MIN to I32::MAX
    i32 next_i32() {
        u32 absolute = next_u32();
        b32 sign     = next_u32() % 2;
        return (i32)absolute * (sign ? -1 : 1);
    }
    /// random float in -1 to 1 range
    f32 next_f32() {
        return RAND_INT_TO_FLOAT( next_i32() );
    }
    /// random float in 0 to 1 range
    f32 next_f32_01() {
        return RAND_INT_TO_FLOAT_01( next_u32() );
    }
};

#endif // header guard
