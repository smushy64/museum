#if !defined(CORE_MATH_SIMD_HPP)
#define CORE_MATH_SIMD_HPP
// * Description:  Cross-Platform SIMD macros
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 02, 2023
#include "defines.h"

#if LD_SIMD_WIDTH == 1

#include "builtins.h"

#define lane1f_sqrt( x ) ::impl::_sqrtf_( x )

namespace impl {
    union four_wide { f32 f[4]; struct { f32 f0, f1, f2, f3; }; };
};
typedef ::impl::four_wide Lane4f;

#define lane4f_set1( f ) { f, f, f, f }
#define lane4f_set0() { 0, 0, 0, 0 }
#define lane4f_set( f0, f1, f2, f3 ) { f0, f1, f2, f3 }
#define lane4f_load( f ) { f[0], f[1], f[2], f[3] }
#define lane4f_store( f, l ) do {\
    f[0] = l.f[0];\
    f[1] = l.f[1];\
    f[2] = l.f[2];\
    f[3] = l.f[3];\
} while(0)

#define lane4f_index( a, i ) ( a.f[i] )
#define lane4f_add( a, b ) {\
    a.f[0] + b.f[0],\
    a.f[1] + b.f[1],\
    a.f[2] + b.f[2],\
    a.f[3] + b.f[3]\
}
#define lane4f_sub( a, b ) {\
    a.f[0] - b.f[0],\
    a.f[1] - b.f[1],\
    a.f[2] - b.f[2],\
    a.f[3] - b.f[3]\
}
#define lane4f_mul( a, b ) {\
    a.f[0] * b.f[0],\
    a.f[1] * b.f[1],\
    a.f[2] * b.f[2],\
    a.f[3] * b.f[3]\
}
#define lane4f_div( a, b ) {\
    a.f[0] / b.f[0],\
    a.f[1] / b.f[1],\
    a.f[2] / b.f[2],\
    a.f[3] / b.f[3]\
}
#define lane4f_sqrt( a ) {\
    ::impl::_sqrtf_(a.f[0]),\
    ::impl::_sqrtf_(a.f[1]),\
    ::impl::_sqrtf_(a.f[2]),\
    ::impl::_sqrtf_(a.f[3]),\
}

#endif // LD_SIMD_WIDTH == 1

#if LD_SIMD_WIDTH >= 4

#if defined(LD_ARCH_X86)
#include "simd_sse.h"

#define lane1f_sqrt( x ) _mm_cvtss_f32( _mm_sqrt_ss( _mm_set_ss( x ) ) )

typedef __m128  Lane4f;
typedef __m128i Lane4i;

#define lane4f_set1( f ) _mm_set1_ps( f )
#define lane4f_set0() _mm_setzero_ps()
#define lane4f_set( f0, f1, f2, f3 ) _mm_setr_ps( f0, f1, f2, f3 )
#define lane4f_load( f ) _mm_loadu_ps( f )
#define lane4f_store( f, l ) _mm_storeu_ps( f, l )

#define lane4f_index( a, i ) ((*(f32*)&a)[i])
#define lane4f_add( a, b ) _mm_add_ps( a, b )
#define lane4f_sub( a, b ) _mm_sub_ps( a, b )
#define lane4f_mul( a, b ) _mm_mul_ps( a, b )
#define lane4f_div( a, b ) _mm_div_ps( a, b )
#define lane4f_sqrt( a ) _mm_sqrt_ps( a )

#endif // ARCH_X86

#endif // SIMD width >= 4

#if LD_SIMD_WIDTH >= 8

#if defined(LD_ARCH_X86)
    // NOTE(alicia): AVX/AVX2 abstractions would go here
#endif

#endif

#endif // header guard
