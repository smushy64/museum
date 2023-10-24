#if !defined(LD_SIMD_H)
#define LD_SIMD_H
// * Description:  Cross-Platform SIMD
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 02, 2023
#include "defines.h"

#if LD_SIMD_WIDTH >= 4
    #if defined( LD_ARCH_X86 )
        /// MMX
        #include <mmintrin.h>
        /// SSE
        #include <xmmintrin.h>
        /// SSE2
        #include <emmintrin.h>
        /// SSE3
        #include <pmmintrin.h>
        /// SSSE3
        #include <tmmintrin.h>
        /// SSE4.1
        #include <smmintrin.h>
        /// SSE4.2
        #include <nmmintrin.h>
    #else
        #error "Current architecture is not currently supported!"
    #endif
#endif

#if LD_SIMD_WIDTH >= 8
    #if defined(LD_ARCH_X86)
        /// AVX and AVX-2
        #include <immintrin.h>
    #else
        #error "Current architecture is not currently supported!"
    #endif
#endif

// NOTE(alicia): scalar SIMD abstractions go here
#if LD_SIMD_WIDTH == 1 

/// Floor float to i32.
global force_inline hot
i32 lane1f_floor_i32( f32 x ) {
    return (((x) > 0.0f) ? (i32)(x) : (i32)( (x) - 0.99999f ));
}
/// Ceil float to i32.
global force_inline hot
i32 lane1f_ceil_i32( f32 x ) {
    return (((x) > 0.0f) ? (i32)( (x) + 0.99999f ) : (i32)( (x) ) );
}
/// Round float to i32.
global force_inline hot
i32 lane1f_round_i32( f32 x ) {
    return (((x) > 0.0f) ? (i32)( (x) + 0.5f ) : (i32)( (x) - 0.5f ));
}
/// Floor float to u32.
global force_inline hot
u32 lane1f_floor_u32( f32 x ) {
    return (((x) > 0.0f) ? (u32)( x ) : 0 );
}
/// Ceil float to u32.
global force_inline hot
u32 lane1f_ceil_u32( f32 x ) {
    return ( (u32)( (x) + 0.99999f ) );
}
/// Round float to u32.
global force_inline hot
u32 lane1f_round_u32( f32 x ) {
    return ((u32)( (x) + 0.5f ));
}

/// Square root of scalar.
global force_inline hot
f32 lane1f_sqrt( f32 x ) {
    if( x < 0.0f )  { return F32_NAN; }
    if( x == 0.0f ) { return 0.0f; }

    // NOTE(alicia): this is probably
    // slower than acceptable but you really
    // shouldn't be compiling without SIMD support
    f32 result = x / 2.0f;
    for( usize i = 0; i < 50; ++i ) {
        result = ( result + (x / result) ) / 2.0f;
    }
    return result;
}
/// Inverse square root of scalar.
global force_inline
f32 lane1f_rsqrt( f32 x ) {
    return 1.0f / lane1f_sqrt( x );
}

union ___internal_four_widef {
    f32 f[4];
    struct { f32 f0, f1, f2, f3; };
};
union ___internal_four_widei {
    i32 i[4];
    struct { i32 i0, i1, i2, i3; };
};
union ___internal_four_wided {
    f64 f[2];
    struct { f64 f0, f1; };
};

/// Four-wide float simd structure.
/// IMPORTANT(alicia): Do NOT directly modify elements of Lane4f.
typedef union ___internal_four_widef Lane4f;
/// Four-wide integer simd structure.
/// IMPORTANT(alicia): Do NOT directly modify elements of Lane4i.
typedef union ___internal_four_widei Lane4i;
/// Four-wide double simd structure.
/// IMPORTANT(alicia): Do NOT directly modify elements of Lane4d.
typedef union ___internal_four_wided Lane4d;

/// Set all values of Lane4f to zero.
global force_inline
Lane4f lane4f_zero(void) {
    return (Lane4f){ 0, 0, 0, 0 };
}
/// Set all values of Lane4f to scalar.
global force_inline
Lane4f lane4f_scalar( f32 scalar ) {
    return (Lane4f){ scalar, scalar, scalar, scalar };
}
/// Set values of Lane4f.
global force_inline
Lane4f lane4f_set( f32 v0, f32 v1, f32 v2, f32 v3 ) {
    return (Lane4f){ v0, v1, v2, v3 };
}
/// Load values from four-element array into Lane4f.
global force_inline
Lane4f lane4f_load( const f32 array[4] ) {
    return (Lane4f){ array[0], array[1], array[2], array[3] };
}
/// Store values from Lane4f into four-element array.
global force_inline
void lane4f_store( Lane4f lane, f32 array[4] ) {
    array[0] = lane.f[0];
    array[1] = lane.f[1];
    array[2] = lane.f[2];
    array[3] = lane.f[3];
}
/// Get value from Lane4f at index.
global force_inline
f32 lane4f_index( Lane4f lane, usize index ) {
    return lane.f[index];
}
/// Add two Lane4f.
global force_inline
Lane4f lane4f_add( Lane4f lhs, Lane4f rhs ) {
    return (Lane4f){
        lhs.f[0] + rhs.f[0],
        lhs.f[1] + rhs.f[1],
        lhs.f[2] + rhs.f[2],
        lhs.f[3] + rhs.f[3],
    };
}
/// Subtract two Lane4f.
global force_inline
Lane4f lane4f_sub( Lane4f lhs, Lane4f rhs ) {
    return (Lane4f){
        lhs.f[0] - rhs.f[0],
        lhs.f[1] - rhs.f[1],
        lhs.f[2] - rhs.f[2],
        lhs.f[3] - rhs.f[3],
    };
}
/// Multiply two Lane4f.
global force_inline
Lane4f lane4f_mul( Lane4f lhs, Lane4f rhs ) {
    return (Lane4f){
        lhs.f[0] * rhs.f[0],
        lhs.f[1] * rhs.f[1],
        lhs.f[2] * rhs.f[2],
        lhs.f[3] * rhs.f[3],
    };
}
/// Divide two Lane4f.
global force_inline
Lane4f lane4f_div( Lane4f lhs, Lane4f rhs ) {
    return (Lane4f){
        lhs.f[0] / rhs.f[0],
        lhs.f[1] / rhs.f[1],
        lhs.f[2] / rhs.f[2],
        lhs.f[3] / rhs.f[3],
    };
}

/// Square root of each element in lane4f.
global force_inline 
Lane4f lane4f_sqrt( Lane4f lane ) {
    return (Lane4f) {
        lane1f_sqrt( lane.f[0] ),
        lane1f_sqrt( lane.f[1] ),
        lane1f_sqrt( lane.f[2] ),
        lane1f_sqrt( lane.f[3] ),
    };
}
/// Inverse square root of each element in lane4f.
global force_inline
Lane4f lane4f_rsqrt( Lane4f lane ) {
    return (Lane4f) {
        lane1f_rsqrt( lane.f[0] ),
        lane1f_rsqrt( lane.f[1] ),
        lane1f_rsqrt( lane.f[2] ),
        lane1f_rsqrt( lane.f[3] ),
    };
}

#endif

// NOTE(alicia): 4/8 wide SIMD abstractions go here
#if defined(LD_ARCH_X86) && LD_SIMD_WIDTH != 1

/// Floor float to i32.
global force_inline hot
i32 lane1f_floor_i32( f32 x ) {
    return _mm_cvtss_si32( _mm_floor_ps( _mm_set_ss( x ) ) );
}
/// Ceil float to i32.
global force_inline hot
i32 lane1f_ceil_i32( f32 x ) {
    return _mm_cvtss_si32( _mm_ceil_ps( _mm_set_ss( x ) ) );
}
/// Round float to i32.
global force_inline hot
i32 lane1f_round_i32( f32 x ) {
    return _mm_cvtss_si32( _mm_round_ps( _mm_set_ss( x ),
        _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC ) );
}
/// Floor float to u32.
global force_inline hot
u32 lane1f_floor_u32( f32 x ) {
    return (u32)_mm_cvtss_si32( _mm_floor_ps( _mm_set_ss( x ) ) );
}
/// Ceil float to u32.
global force_inline hot
u32 lane1f_ceil_u32( f32 x ) {
    return (u32)_mm_cvtss_si32( _mm_ceil_ps( _mm_set_ss( x ) ) );
}
/// Round float to u32.
global force_inline hot
u32 lane1f_round_u32( f32 x ) {
    return (u32)_mm_cvtss_si32( _mm_round_ps( _mm_set_ss( x ),
        _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC ) );
}
/// Square root of scalar.
global force_inline
f32 lane1f_sqrt( f32 x ) {
    return _mm_cvtss_f32( _mm_sqrt_ss( _mm_set_ss( x ) ) );
}
/// Inverse square root of scalar.
global force_inline
f32 lane1f_rsqrt( f32 x ) {
    return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ss( x ) ) );
}

/// Four-wide float simd structure.
/// IMPORTANT(alicia): Do NOT directly modify elements of Lane4f.
typedef __m128  Lane4f;
/// Four-wide integer simd structure.
/// IMPORTANT(alicia): Do NOT directly modify elements of Lane4i.
typedef __m128i Lane4i;
/// Four-wide double simd structure.
/// IMPORTANT(alicia): Do NOT directly modify elements of Lane4d.
typedef __m128d Lane4d;

/// Set all values of Lane4f to zero.
global force_inline
Lane4f lane4f_zero(void) {
    return _mm_setzero_ps();
}
/// Set all values of Lane4f to scalar.
global force_inline
Lane4f lane4f_scalar( f32 scalar ) {
    return _mm_set1_ps( scalar );
}
/// Set values of Lane4f.
global force_inline
Lane4f lane4f_set( f32 v0, f32 v1, f32 v2, f32 v3 ) {
    return _mm_setr_ps( v0, v1, v2, v3 );
}
/// Load values from four-element array into Lane4f.
global force_inline
Lane4f lane4f_load( const f32 array[4] ) {
    return _mm_loadu_ps( array );
}
/// Store values from Lane4f into four-element array.
global force_inline
void lane4f_store( Lane4f lane, f32 array[4] ) {
    _mm_storeu_ps( array, lane );
}
/// Get value from Lane4f at index.
global force_inline
f32 lane4f_index( Lane4f lane, usize index ) {
    return *(((f32*)&lane) + index);
}
/// Add two Lane4f.
global force_inline
Lane4f lane4f_add( Lane4f lhs, Lane4f rhs ) {
    return _mm_add_ps( lhs, rhs );
}
/// Subtract two Lane4f.
global force_inline
Lane4f lane4f_sub( Lane4f lhs, Lane4f rhs ) {
    return _mm_sub_ps( lhs, rhs );
}
/// Multiply two Lane4f.
global force_inline
Lane4f lane4f_mul( Lane4f lhs, Lane4f rhs ) {
    return _mm_mul_ps( lhs, rhs );
}
/// Divide two Lane4f.
global force_inline
Lane4f lane4f_div( Lane4f lhs, Lane4f rhs ) {
    return _mm_div_ps( lhs, rhs );
}

/// Square root of each element in lane4f.
global force_inline 
Lane4f lane4f_sqrt( Lane4f lane ) {
    return _mm_sqrt_ps( lane );
}
/// Inverse square root of each element in lane4f.
global force_inline
Lane4f lane4f_rsqrt( Lane4f lane ) {
    return _mm_rsqrt_ps( lane );
}

#endif

#endif // header guard
