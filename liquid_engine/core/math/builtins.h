#if !defined(CORE_MATH_BUILTINS_HPP)
#define CORE_MATH_BUILTINS_HPP
// * Description:  Compiler builtin math functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 02, 2023
#include "defines.h"
#if !(defined(LD_COMPILER_CLANG) || defined(LD_COMPILER_GCC))
    #include <math.h>
#endif

namespace impl {

#if defined(LD_COMPILER_GCC) || defined(LD_COMPILER_CLANG)

    inline f32 _asinf_( f32 x ) {
        return __builtin_asinf(x);
    }
    inline f64 _asin_( f64 x ) {
        return __builtin_asin(x);
    }
    inline f32 _acosf_( f32 x ) {
        return __builtin_acosf(x);
    }
    inline f64 _acos_( f64 x ) {
        return __builtin_acos(x);
    }
    inline f32 _atanf_( f32 x ) {
        return __builtin_atanf(x);
    }
    inline f64 _atan_( f64 x ) {
        return __builtin_atan(x);
    }
    inline f32 _atan2f_( f32 y, f32 x ) {
        return __builtin_atan2f(y, x);
    }
    inline f64 _atan2_( f64 y, f64 x ) {
        return __builtin_atan2(y, x);
    }

    inline f32 _powf_( f32 base, f32 exponent ) {
        return __builtin_powf(base, exponent);
    }
    inline f64 _pow_( f64 base, f64 exponent ) {
        return __builtin_pow(base, exponent);
    }

    inline f32 _powif_( f32 base, i32 exponent ) {
        return __builtin_powif(base, exponent);
    }
    inline f64 _powi_( f64 base, i32 exponent ) {
        return __builtin_powi(base, exponent);
    }

    inline f32 _logf_( f32 x ) {
        return __builtin_logf( x );
    }
    inline f64 _log_( f64 x ) {
        return __builtin_log( x );
    }

    inline f32 _modf_( f32 numerator, f32 denominator ) {
        return __builtin_fmodf( numerator, denominator );
    }

    inline f64 _mod_( f64 numerator, f64 denominator ) {
        return __builtin_fmod( numerator, denominator );
    }

    inline f32 _sqrtf_( f32 x ) {
        return __builtin_sqrtf( x );
    }
    inline f64 _sqrt_( f64 x ) {
        return __builtin_sqrt( x );
    }

#else

    inline f32 _sinf_( f32 x ) {
        return sinf(x);
    }
    inline f64 _sin_( f64 x ) {
        return sin(x);
    }
    inline f32 _asinf_( f32 x ) {
        return asinf(x);
    }
    inline f64 _asin_( f64 x ) {
        return asin(x);
    }
    inline f32 _cosf_( f32 x ) {
        return cosf(x);
    }
    inline f64 _cos_( f64 x ) {
        return cos(x);
    }
    inline f32 _acosf_( f32 x ) {
        return acosf(x);
    }
    inline f64 _acos_( f64 x ) {
        return acos(x);
    }
    inline f32 _tanf_( f32 x ) {
        return tanf(x);
    }
    inline f64 _tan_( f64 x ) {
        return tan(x);
    }
    inline f32 _atanf_( f32 x ) {
        return atanf(x);
    }
    inline f64 _atan_( f64 x ) {
        return atan(x);
    }
    inline f32 _atan2f_( f32 y, f32 x ) {
        return atan2f(y, x);
    }
    inline f64 _atan2_( f64 y, f64 x ) {
        return atan2(y, x);
    }

    inline f32 _powf_( f32 base, f32 exponent ) {
        return powf(base, exponent);
    }
    inline f64 _pow_( f64 base, f64 exponent ) {
        return pow(base, exponent);
    }

    inline f32 _powif_( f32 base, i32 exponent ) {
        return powi(base, exponent);
    }
    inline f64 _powi_( f64 base, i32 exponent ) {
        return powi(base, exponent);
    }

    inline f32 _sqrtf_( f32 x ) {
        return sqrtf( x );
    }
    inline f64 _sqrt_( f64 x ) {
        return sqrt( x );
    }

    inline f32 _logf_( f32 x ) {
        return logf( x );
    }
    inline f64 _log_( f64 x ) {
        return log( x );
    }

    inline f32 _modf_( f32 numerator, f32 denominator ) {
        return fmodf( numerator, denominator );
    }

    inline f64 _mod_( f64 numerator, f64 denominator ) {
        return fmod( numerator, denominator );
    }

#endif

} // namespace impl

#endif // header guard
