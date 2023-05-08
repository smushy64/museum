/**
 * Description:  C++ math library for game development
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: March 16, 2023
 * Notes:        Define SM_SIMD_WIDTH as 4 or 8 to
 *               enable SIMD on x86
 * Includes:     "smdef.h"
 *               MSVC: math.h
*/
#if !defined(SMATH_HPP)
#define SMATH_HPP

#include "defines.h"

#if !(defined(SM_COMPILER_CLANG) || defined(SM_COMPILER_GCC))
    #include <math.h>
#endif

#if defined(SM_COMPILER_CLANG)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-braces"
#endif

namespace impl {


#if defined(SM_COMPILER_GCC) || defined(SM_COMPILER_CLANG)

    SM_ALWAYS_INLINE f32 _sinf_( f32 x ) {
        return __builtin_sinf(x);
    }
    SM_ALWAYS_INLINE f64 _sin_( f64 x ) {
        return __builtin_sin(x);
    }
    SM_ALWAYS_INLINE f32 _asinf_( f32 x ) {
        return __builtin_asinf(x);
    }
    SM_ALWAYS_INLINE f64 _asin_( f64 x ) {
        return __builtin_asin(x);
    }
    SM_ALWAYS_INLINE f32 _cosf_( f32 x ) {
        return __builtin_cosf(x);
    }
    SM_ALWAYS_INLINE f64 _cos_( f64 x ) {
        return __builtin_cos(x);
    }
    SM_ALWAYS_INLINE f32 _acosf_( f32 x ) {
        return __builtin_acosf(x);
    }
    SM_ALWAYS_INLINE f64 _acos_( f64 x ) {
        return __builtin_acos(x);
    }
    SM_ALWAYS_INLINE f32 _tanf_( f32 x ) {
        return __builtin_tanf(x);
    }
    SM_ALWAYS_INLINE f64 _tan_( f64 x ) {
        return __builtin_tan(x);
    }
    SM_ALWAYS_INLINE f32 _atanf_( f32 x ) {
        return __builtin_atanf(x);
    }
    SM_ALWAYS_INLINE f64 _atan_( f64 x ) {
        return __builtin_atan(x);
    }
    SM_ALWAYS_INLINE f32 _atan2f_( f32 y, f32 x ) {
        return __builtin_atan2f(y, x);
    }
    SM_ALWAYS_INLINE f64 _atan2_( f64 y, f64 x ) {
        return __builtin_atan2(y, x);
    }

    SM_ALWAYS_INLINE f32 _powf_( f32 base, f32 exponent ) {
        return __builtin_powf(base, exponent);
    }
    SM_ALWAYS_INLINE f64 _pow_( f64 base, f64 exponent ) {
        return __builtin_pow(base, exponent);
    }

    SM_ALWAYS_INLINE f32 _powif_( f32 base, i32 exponent ) {
        return __builtin_powif(base, exponent);
    }
    SM_ALWAYS_INLINE f64 _powi_( f64 base, i32 exponent ) {
        return __builtin_powi(base, exponent);
    }

    SM_ALWAYS_INLINE f32 _sqrtf_( f32 x ) {
        return __builtin_sqrtf( x );
    }
    SM_ALWAYS_INLINE f64 _sqrt_( f64 x ) {
        return __builtin_sqrt( x );
    }

    SM_ALWAYS_INLINE f32 _logf_( f32 x ) {
        return __builtin_logf( x );
    }
    SM_ALWAYS_INLINE f64 _log_( f64 x ) {
        return __builtin_log( x );
    }

    SM_ALWAYS_INLINE f32 _modf_( f32 numerator, f32 denominator ) {
        return __builtin_fmodf( numerator, denominator );
    }

    SM_ALWAYS_INLINE f64 _mod_( f64 numerator, f64 denominator ) {
        return __builtin_fmod( numerator, denominator );
    }

#else

    SM_ALWAYS_INLINE f32 _sinf_( f32 x ) {
        return sinf(x);
    }
    SM_ALWAYS_INLINE f64 _sin_( f64 x ) {
        return sin(x);
    }
    SM_ALWAYS_INLINE f32 _asinf_( f32 x ) {
        return asinf(x);
    }
    SM_ALWAYS_INLINE f64 _asin_( f64 x ) {
        return asin(x);
    }
    SM_ALWAYS_INLINE f32 _cosf_( f32 x ) {
        return cosf(x);
    }
    SM_ALWAYS_INLINE f64 _cos_( f64 x ) {
        return cos(x);
    }
    SM_ALWAYS_INLINE f32 _acosf_( f32 x ) {
        return acosf(x);
    }
    SM_ALWAYS_INLINE f64 _acos_( f64 x ) {
        return acos(x);
    }
    SM_ALWAYS_INLINE f32 _tanf_( f32 x ) {
        return tanf(x);
    }
    SM_ALWAYS_INLINE f64 _tan_( f64 x ) {
        return tan(x);
    }
    SM_ALWAYS_INLINE f32 _atanf_( f32 x ) {
        return atanf(x);
    }
    SM_ALWAYS_INLINE f64 _atan_( f64 x ) {
        return atan(x);
    }
    SM_ALWAYS_INLINE f32 _atan2f_( f32 y, f32 x ) {
        return atan2f(y, x);
    }
    SM_ALWAYS_INLINE f64 _atan2_( f64 y, f64 x ) {
        return atan2(y, x);
    }

    SM_ALWAYS_INLINE f32 _powf_( f32 base, f32 exponent ) {
        return powf(base, exponent);
    }
    SM_ALWAYS_INLINE f64 _pow_( f64 base, f64 exponent ) {
        return pow(base, exponent);
    }

    SM_ALWAYS_INLINE f32 _powif_( f32 base, i32 exponent ) {
        return powi(base, exponent);
    }
    SM_ALWAYS_INLINE f64 _powi_( f64 base, i32 exponent ) {
        return powi(base, exponent);
    }

    SM_ALWAYS_INLINE f32 _sqrtf_( f32 x ) {
        return sqrtf( x );
    }
    SM_ALWAYS_INLINE f64 _sqrt_( f64 x ) {
        return sqrt( x );
    }

    SM_ALWAYS_INLINE f32 _logf_( f32 x ) {
        return logf( x );
    }
    SM_ALWAYS_INLINE f64 _log_( f64 x ) {
        return log( x );
    }

    SM_ALWAYS_INLINE f32 _modf_( f32 numerator, f32 denominator ) {
        return fmodf( numerator, denominator );
    }

    SM_ALWAYS_INLINE f64 _mod_( f64 numerator, f64 denominator ) {
        return fmod( numerator, denominator );
    }

#endif

} // namespace impl

/// truncate float to signed integer
SM_ALWAYS_INLINE i64 trunc64( f32 x ) { return (i64)x; }
/// truncate float to signed integer
SM_ALWAYS_INLINE i64 trunc64( f64 x ) { return (i64)x; }
/// truncate float to signed integer
SM_ALWAYS_INLINE i32 trunc32( f32 x ) { return (i32)x; }
/// truncate float to signed integer
SM_ALWAYS_INLINE i32 trunc32( f64 x ) { return (i32)x; }
/// truncate float to signed integer
SM_ALWAYS_INLINE i16 trunc16( f32 x ) { return (i16)x; }
/// truncate float to signed integer
SM_ALWAYS_INLINE i16 trunc16( f64 x ) { return (i16)x; }
/// truncate float to signed integer
SM_ALWAYS_INLINE i8  trunc8( f32 x )  { return (i8)x;  }
/// truncate float to signed integer
SM_ALWAYS_INLINE i8  trunc8( f64 x )  { return (i8)x;  }

/// floor float to signed integer
SM_ALWAYS_INLINE i64 floor64( f64 x ) {
    return x > 0.0 ? trunc64(x) : trunc64(x - 0.99999999);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i64 floor64( f32 x ) {
    return x > 0.0f ? trunc64(x) : trunc64(x - 0.999999f);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i32 floor32( f64 x ) {
    return x > 0.0 ? trunc32(x) : trunc32(x - 0.99999999);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i32 floor32( f32 x ) {
    return x > 0.0f ? trunc32(x) : trunc32(x - 0.999999f);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i16 floor16( f64 x ) {
    return x > 0.0 ? trunc16(x) : trunc16(x - 0.99999999);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i16 floor16( f32 x ) {
    return x > 0.0f ? trunc16(x) : trunc16(x - 0.999999f);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i8 floor8( f64 x ) {
    return x > 0.0 ? trunc8(x) : trunc8(x - 0.99999999);
}
/// floor float to signed integer
SM_ALWAYS_INLINE i8 floor8( f32 x ) {
    return x > 0.0f ? trunc8(x) : trunc8(x - 0.999999f);
}

/// ceil float to signed integer
SM_ALWAYS_INLINE i64 ceil64( f64 x ) {
    return x > 0.0 ? trunc64( x + 0.99999999 ) : trunc64(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i64 ceil64( f32 x ) {
    return x > 0.0f ? trunc64( x + 0.999999f ) : trunc64(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i32 ceil32( f64 x ) {
    return x > 0.0 ? trunc32( x + 0.99999999 ) : trunc32(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i32 ceil32( f32 x ) {
    return x > 0.0f ? trunc32( x + 0.999999f ) : trunc32(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i16 ceil16( f64 x ) {
    return x > 0.0 ? trunc16( x + 0.99999999 ) : trunc16(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i16 ceil16( f32 x ) {
    return x > 0.0f ? trunc16( x + 0.999999f ) : trunc16(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i8 ceil8( f64 x ) {
    return x > 0.0 ? trunc8( x + 0.99999999 ) : trunc8(x);
}
/// ceil float to signed integer
SM_ALWAYS_INLINE i8 ceil8( f32 x ) {
    return x > 0.0f ? trunc8( x + 0.999999f ) : trunc8(x);
}

/// round float to signed integer
SM_ALWAYS_INLINE i64 round64( f64 x ) {
    return x > 0.0 ? trunc64( x + 0.5 ) : trunc64( x - 0.5 );
}
/// round float to signed integer
SM_ALWAYS_INLINE i64 round64( f32 x ) {
    return x > 0.0f ? trunc64( x + 0.5f ) : trunc64( x - 0.5f );
}
/// round float to signed integer
SM_ALWAYS_INLINE i32 round32( f64 x ) {
    return x > 0.0 ? trunc32( x + 0.5 ) : trunc32( x - 0.5 );
}
/// round float to signed integer
SM_ALWAYS_INLINE i32 round32( f32 x ) {
    return x > 0.0f ? trunc32( x + 0.5f ) : trunc32( x - 0.5f );
}
/// round float to signed integer
SM_ALWAYS_INLINE i16 round16( f64 x ) {
    return x > 0.0 ? trunc16( x + 0.5 ) : trunc16( x - 0.5 );
}
/// round float to signed integer
SM_ALWAYS_INLINE i16 round16( f32 x ) {
    return x > 0.0f ? trunc16( x + 0.5f ) : trunc16( x - 0.5f );
}
/// round float to signed integer
SM_ALWAYS_INLINE i8 round8( f64 x ) {
    return x > 0.0 ? trunc8( x + 0.5 ) : trunc8( x - 0.5 );
}
/// round float to signed integer
SM_ALWAYS_INLINE i8 round8( f32 x ) {
    return x > 0.0f ? trunc8( x + 0.5f ) : trunc8( x - 0.5f );
}

/// get the sign of a signed integer
SM_ALWAYS_INLINE i8 sign( i8 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
SM_ALWAYS_INLINE i16 sign( i16 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
SM_ALWAYS_INLINE i32 sign( i32 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
SM_ALWAYS_INLINE i64 sign( i64 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a single-precision float
SM_ALWAYS_INLINE f32 sign( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return ((bitpattern >> 31) & 1) == 1 ? -1.0f : 1.0f;
}
/// get the sign of a double-precision float
SM_ALWAYS_INLINE f64 sign( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return ((bitpattern >> 63) & 1) == 1 ? -1.0 : 1.0;
}

/// get the absolute value of a signed integer
SM_ALWAYS_INLINE i8 absolute( i8 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
SM_ALWAYS_INLINE i16 absolute( i16 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
SM_ALWAYS_INLINE i32 absolute( i32 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
SM_ALWAYS_INLINE i64 absolute( i64 x ) { return x * sign( x ); }
/// get the absolute value of a single-precision float
SM_ALWAYS_INLINE f32 absolute( f32 x ) { return x * sign( x ); }
/// get the absolute value of a double-precision float
SM_ALWAYS_INLINE f64 absolute( f64 x ) { return x * sign( x ); }

/// clamp a signed integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE i8 clamp( i8 value, i8 min, i8 max ) {
    const i8 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE i16 clamp( i16 value, i16 min, i16 max ) {
    const i16 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE i32 clamp( i32 value, i32 min, i32 max ) {
    const i32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE i64 clamp( i64 value, i64 min, i64 max ) {
    const i64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE u8 clamp( u8 value, u8 min, u8 max ) {
    const u8 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE u16 clamp( u16 value, u16 min, u16 max ) {
    const u16 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE u32 clamp( u32 value, u32 min, u32 max ) {
    const u32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
SM_ALWAYS_INLINE u64 clamp( u64 value, u64 min, u64 max ) {
    const u64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a single-precision float between min and max, inclusive-inclusive
SM_ALWAYS_INLINE f32 clamp( f32 value, f32 min, f32 max ) {
    const f32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a double-precision float between min and max, inclusive-inclusive
SM_ALWAYS_INLINE f64 clamp( f64 value, f64 min, f64 max ) {
    const f64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a single-precision float between 0-1
SM_ALWAYS_INLINE f32 clamp01( f32 value ) { return clamp(value,0.0f,1.0f); }
/// clamp a double-precision float between 0-1
SM_ALWAYS_INLINE f64 clamp01( f64 value ) { return clamp(value,0.0,1.0); }

/// get the smallest of two values
SM_ALWAYS_INLINE i8 min( i8 a, i8 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE i16 min( i16 a, i16 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE i32 min( i32 a, i32 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE i64 min( i64 a, i64 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE u8 min( u8 a, u8 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE u16 min( u16 a, u16 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE u32 min( u32 a, u32 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE u64 min( u64 a, u64 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE f32 min( f32 a, f32 b ) { return a < b ? a : b; }
/// get the smallest of two values
SM_ALWAYS_INLINE f64 min( f64 a, f64 b ) { return a < b ? a : b; }

/// get the largest of two values
SM_ALWAYS_INLINE i8 max( i8 a, i8 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE i16 max( i16 a, i16 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE i32 max( i32 a, i32 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE i64 max( i64 a, i64 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE u8 max( u8 a, u8 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE u16 max( u16 a, u16 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE u32 max( u32 a, u32 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE u64 max( u64 a, u64 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE f32 max( f32 a, f32 b ) { return a < b ? b : a; }
/// get the largest of two values
SM_ALWAYS_INLINE f64 max( f64 a, f64 b ) { return a < b ? b : a; }

/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( i8 x ) {
    return (f32)x / (x > 0 ? (f32)I8::MAX : -((f32)I8::MIN));
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( i16 x ) {
    return (f32)x / (x > 0 ? (f32)I16::MAX : -((f32)I16::MIN));
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( i32 x ) {
    return (f32)x / (x > 0 ? (f32)I32::MAX : -((f32)I32::MIN));
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( i64 x ) {
    return (f32)x / (x > 0 ? (f32)I64::MAX : -((f32)I64::MIN));
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( u8 x ) {
    return (f32)x / (f32)U8::MAX;
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( u16 x ) {
    return (f32)x / (f32)U16::MAX;
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( u32 x ) {
    return (f32)x / (f32)U32::MAX;
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f32 normalize_range( u64 x ) {
    return (f32)x / (f32)U64::MAX;
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( i8 x ) {
    return (f64)x / (x > 0 ? (f64)I8::MAX : -((f64)I8::MIN));
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( i16 x ) {
    return (f64)x / (x > 0 ? (f64)I16::MAX : -((f64)I16::MIN));
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( i32 x ) {
    return (f64)x / (x > 0 ? (f64)I32::MAX : -((f64)I32::MIN));
}
/// normalize integer -1 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( i64 x ) {
    return (f64)x / (x > 0 ? (f64)I64::MAX : -((f64)I64::MIN));
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( u8 x ) {
    return (f64)x / (f64)U8::MAX;
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( u16 x ) {
    return (f64)x / (f64)U16::MAX;
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( u32 x ) {
    return (f64)x / (f64)U32::MAX;
}
/// normalize integer 0 to 1 range
SM_ALWAYS_INLINE f64 normalize_range64( u64 x ) {
    return (f64)x / (f64)U64::MAX;
}

/// square root
SM_ALWAYS_INLINE f64 sqrt( f64 x ) { return impl::_sqrt_(x); }
/// square root
SM_ALWAYS_INLINE f32 sqrt( f32 x ) { return impl::_sqrtf_(x); }

/// raise to the power, integer exponent
SM_ALWAYS_INLINE f32 powi( f32 base, i32 exponent ) {
    return impl::_powif_( base, exponent );
}
/// raise to the power, integer exponent
SM_ALWAYS_INLINE f64 powi( f64 base, i32 exponent ) {
    return impl::_powi_( base, exponent );
}
/// raise to the power, float exponent
SM_ALWAYS_INLINE f32 pow( f32 base, f32 exponent ) {
    return impl::_powf_( base, exponent );
}
/// raise to the power, float exponent
SM_ALWAYS_INLINE f64 pow( f64 base, f64 exponent ) {
    return impl::_pow_( base, exponent );
}

/// float modulus
SM_ALWAYS_INLINE f32 mod( f32 lhs, f32 rhs ) {
    return impl::_modf_( lhs, rhs );
}
/// float modulus
SM_ALWAYS_INLINE f64 mod( f64 lhs, f64 rhs ) {
    return impl::_mod_( lhs, rhs );
}

/// check if single-precision is not a number
SM_ALWAYS_INLINE b32 is_nan( f32 x ) {
    u32 bitpattern = *(u32*)&x;

    u32 exp = bitpattern & F32::EXPONENT_MASK;
    u32 man = bitpattern & F32::MANTISSA_MASK;

    return exp == F32::EXPONENT_MASK && man != 0;
}
/// check if single-precision float is zero
SM_ALWAYS_INLINE b32 is_zero( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return (bitpattern & 0x7FFFFFFF) == 0 || (bitpattern == 0x80000000);
}
/// check if double-precision float is not a number
SM_ALWAYS_INLINE b32 is_nan( f64 x ) {
    u64 bitpattern = *(u64*)&x;

    u64 exp = bitpattern & F64::EXPONENT_MASK;
    u64 man = bitpattern & F64::MANTISSA_MASK;

    return exp == F64::EXPONENT_MASK && man != 0;
}
/// check if double-precision float is zero
SM_ALWAYS_INLINE b32 is_zero( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return (bitpattern & 0x7FFFFFFFFFFFFFFF) == 0 || (bitpattern == 0x8000000000000000);
}

/// sine function
SM_ALWAYS_INLINE f32 sin( f32 x ) {
    // Reduce x to the range [-pi, pi]
    i32 q = x / F32::PI;
    x -= q * F32::PI;
    if( x > F32::HALF_PI ) {
        x = F32::PI - x;
    } else if( x < -F32::HALF_PI ) {
        x = -F32::PI - x;
    }

    // Compute sine using Chebyshev polynomial approximation
    f32 x2 = x * x;
    f32 x3 = x * x2;
    f32 x5 = x3 * x2;
    f32 x7 = x5 * x2;

    f32 result = x * ( 1.0f - x2 * ( 0.16667f - x2 * ( 0.008334f - x2 * 0.000198f ) ) ) +
        x3 * ( 0.008334f - x2 * ( 0.000198f - x2 * 0.000002756f ) ) +
        x5 * 0.000198f -
        x7 * 0.000002756f;

    // Adjust result for sign and range
    if (q % 2 != 0) {
        result = -result;
    }
    return result;
}
/// sine function
SM_ALWAYS_INLINE f64 sin( f64 x ) {
    // Reduce x to the range [-pi, pi]
    i32 q = (i32)(x / F64::PI);
    x -= q * F64::PI;
    if (x > F64::HALF_PI) {
        x = F64::PI - x;
    } else if (x < -F64::HALF_PI) {
        x = -F64::PI - x;
    }

    // Compute sine using Chebyshev polynomial approximation
    f64 x2 = x * x;
    f64 x3 = x * x2;
    f64 x5 = x3 * x2;
    f64 x7 = x5 * x2;
    f64 x9 = x7 * x2;
    f64 x11 = x9 * x2;
    f64 x13 = x11 * x2;

    f64 result = x * (1.0 - x2 * (0.166666666666667 - x2 * (0.008333333333333 - x2 * (0.000198412698413 - x2 * 2.75573192239859e-06)))) +
        x3 * (0.008333333333333 - x2 * (0.000198412698413 - x2 * (2.75573192239859e-06 - x2 * 2.50521083854417e-08))) +
        x5 * (0.000198412698413 - x2 * (2.75573192239859e-06 - x2 * (2.50521083854417e-08 - x2 * 1.60590438368216e-10))) -
        x7 * (2.75573192239859e-06 - x2 * (2.50521083854417e-08 - x2 * (1.60590438368216e-10 - x2 * 7.64716373181982e-13))) +
        x9 * (2.50521083854417e-08 - x2 * (1.60590438368216e-10 - x2 * (7.64716373181982e-13 - x2 * 2.81145725434552e-15))) -
        x11 * (1.60590438368216e-10 - x2 * (7.64716373181982e-13 - x2 * (2.81145725434552e-15 - x2 * 7.64716373181982e-18))) +
        x13 * (7.64716373181982e-13 - x2 * (2.81145725434552e-15 - x2 * (7.64716373181982e-18 - x2 * 1.60590438368216e-20)));

    // Adjust result for sign and range
    if (q % 2 != 0) {
        result = -result;
    }
    return result;
}
/// arc-sine function
SM_ALWAYS_INLINE f32 asin( f32 x ) {
    return impl::_asinf_( x );
}
/// arc-sine function
SM_ALWAYS_INLINE f64 asin( f64 x ) {
    return impl::_asin_( x );
}

/// arc-sine function, does not return NAN
SM_ALWAYS_INLINE f32 asin_real( f32 x ) {
    return absolute(x) >= 1.0f ? F32::HALF_PI * sign(x) : asin(x);
}
/// arc-sine function, does not return NAN
SM_ALWAYS_INLINE f64 asin_real( f64 x ) {
    return absolute(x) >= 1.0 ? F64::HALF_PI * sign(x) : asin(x);
}

/// cosine function
SM_ALWAYS_INLINE f32 cos( f32 x ) {
    return sin( x + F32::HALF_PI );
}
/// cosine function
SM_ALWAYS_INLINE f64 cos( f64 x ) {
    return sin( x + F64::HALF_PI );
}
/// arc-cosine function
SM_ALWAYS_INLINE f32 acos( f32 x ) {
    return impl::_acosf_( x );
}
/// arc-cosine function
SM_ALWAYS_INLINE f64 acos( f64 x ) {
    return impl::_acos_( x );
}

/// tangent function
SM_ALWAYS_INLINE f32 tan( f32 x ) {
    f32 s = sin(x);
    f32 c = cos(x);

    return is_zero(c) ? F32::NAN : s / c;
}
/// tangent function
SM_ALWAYS_INLINE f64 tan( f64 x ) {
    f64 s = sin(x);
    f64 c = cos(x);

    return is_zero(c) ? F64::NAN : s / c;
}
/// arc-tangent function
SM_ALWAYS_INLINE f32 atan( f32 x ) {
    return impl::_atanf_( x );
}
/// arc-tangent function
SM_ALWAYS_INLINE f64 atan( f64 x ) {
    return impl::_atan_( x );
}

/// two argument arc-tangent function
SM_ALWAYS_INLINE f32 atan2( f32 y, f32 x ) {
    return impl::_atan2f_( y, x );
}
/// two argument arc-tangent function
SM_ALWAYS_INLINE f64 atan2( f64 y, f64 x ) {
    return impl::_atan2_( y, x );
}

/// convert degrees to radians
SM_ALWAYS_INLINE f32 to_rad( f32 theta ) {
    return theta * ( F32::PI / 180.0f );
}
/// convert degrees to radians
SM_ALWAYS_INLINE f64 to_rad( f64 theta ) {
    return theta * ( F64::PI / 180.0 );
}
/// convert radians to degrees 
SM_ALWAYS_INLINE f32 to_deg( f32 theta ) {
    return theta * ( 180.0f / F32::PI );
}
/// convert radians to degrees 
SM_ALWAYS_INLINE f64 to_deg( f64 theta ) {
    return theta * ( 180.0 / F64::PI );
}

/// keep degrees in 0.0-360 range
SM_ALWAYS_INLINE f32 degrees_overflow( f32 deg ) {
    f32 result = mod( deg, 360.0f );
    if( result < 0.0f ) {
        result += 360.0f;
    }
    return result;
}

/// natural logarithm
SM_ALWAYS_INLINE f32 log( f32 x ) {
    return impl::_logf_( x );
}
/// natural logarithm
SM_ALWAYS_INLINE f64 log( f64 x ) {
    return impl::_log_( x );
}

/// linear interpolation
SM_ALWAYS_INLINE f32 lerp( f32 a, f32 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation
SM_ALWAYS_INLINE f64 lerp( f64 a, f64 b, f64 t ) {
    return ( 1.0 - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
SM_ALWAYS_INLINE f32 lerp_clamped( f32 a, f32 b, f32 t ) {
    return lerp( a, b, clamp01(t) );
}
/// linear interpolation, t clamped to 0-1
SM_ALWAYS_INLINE f64 lerp_clamped( f64 a, f64 b, f64 t ) {
    return lerp( a, b, clamp01(t) );
}
/// inverse linear interpolation
SM_ALWAYS_INLINE f32 inverse_lerp( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}
/// inverse linear interpolation
SM_ALWAYS_INLINE f64 inverse_lerp( f64 a, f64 b, f64 v ) {
    return ( v - a ) / ( b - a );
}
/// remap value from input min/max to output min/max
SM_ALWAYS_INLINE f32 remap( f32 imin, f32 imax, f32 omin, f32 omax, f32 v ) {
    const f32 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}
/// remap value from input min/max to output min/max
SM_ALWAYS_INLINE f64 remap( f64 imin, f64 imax, f64 omin, f64 omax, f64 v ) {
    const f64 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}

/// smooth step interpolation
SM_ALWAYS_INLINE f32 smooth_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation
SM_ALWAYS_INLINE f64 smooth_step( f64 a, f64 b, f64 t ) {
    return ( b - a ) * ( 3.0 - t * 2.0 ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE f32 smooth_step_clamped( f32 a, f32 b, f32 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smooth step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE f64 smooth_step_clamped( f64 a, f64 b, f64 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smoother step interpolation
SM_ALWAYS_INLINE f32 smoother_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation
SM_ALWAYS_INLINE f64 smoother_step( f64 a, f64 b, f64 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0 - 15.0 ) + 10.0 ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE f32 smoother_step_clamped( f32 a, f32 b, f32 t ) {
    return smoother_step( a, b, clamp01(t) );
}
/// smoother step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE f64 smoother_step_clamped( f64 a, f64 b, f64 t ) {
    return smoother_step( a, b, clamp01(t) );
}

// TYPES ----------------------------------------------------------------

union vec2;
// struct dvec2;
union uvec2;
union ivec2;

union vec3;
// struct dvec3;
union uvec3;
union ivec3;

typedef vec3 rgb;
typedef vec3 hsv;
typedef vec3 euler_angles;

union vec4;
union uvec4;
union ivec4;

typedef vec4 rgba;

struct angle_axis;
union quat;
// struct dquat;

union mat2;
union mat3;
union mat4;

inline vec2 operator+( vec2 lhs, vec2 rhs );
inline vec2 operator-( vec2 lhs, vec2 rhs );
inline vec2 operator*( vec2 lhs, f32 rhs );
inline vec2 operator*( f32 lhs, vec2 rhs );
inline vec2 operator/( vec2 lhs, f32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( vec2 a, vec2 b );
static const u32 VEC2_COMPONENT_COUNT = 2;
/// 2-component 32-bit float vector
union vec2 {
    struct { f32 x, y; };
    struct { f32 u, v; };
    struct { f32 width, height; };
    f32 c[VEC2_COMPONENT_COUNT];

    f32& operator[](u32 index)       { return c[index]; }
    f32  operator[](u32 index) const { return c[index]; }

    b32 operator==( vec2 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( vec2 b ) const { return !(*this == b); }

    vec2 operator-() const { return { -this->x, -this->y }; }
    vec2& operator+=( vec2 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    vec2& operator-=( vec2 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    vec2& operator*=( f32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    vec2& operator/=( f32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const vec2& v ) { return (f32*)&v.x; }
SM_ALWAYS_INLINE vec2 v2() { return {}; }
SM_ALWAYS_INLINE vec2 v2( f32 scalar ) { return { scalar, scalar }; }
SM_ALWAYS_INLINE vec2 v2( f32 x, f32 y ) { return { x, y }; }

namespace VEC2 {
    /// { 1.0, 1.0 }
    static const vec2 ONE   = {  1.0f,  1.0f };
    /// { 0.0, 0.0 }
    static const vec2 ZERO  = {  0.0f,  0.0f };
    /// { -1.0, 0.0 }
    static const vec2 LEFT  = { -1.0f,  0.0f };
    /// { 1.0, 0.0 }
    static const vec2 RIGHT = {  1.0f,  0.0f };
    /// { 0.0, 1.0 }
    static const vec2 UP    = {  0.0f,  1.0f };
    /// { 0.0, -1.0 }
    static const vec2 DOWN  = {  0.0f, -1.0f };
} // namespace smath::VEC2

inline vec2 operator+( vec2 lhs, vec2 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}
inline vec2 operator-( vec2 lhs, vec2 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}
inline vec2 operator*( vec2 lhs, f32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs
    };
}
inline vec2 operator*( f32 lhs, vec2 rhs ) {
    return operator*( rhs, lhs );
}
inline vec2 operator/( vec2 lhs, f32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs
    };
}

/// rotate components to the left. for vec2 it just swaps components
SM_ALWAYS_INLINE vec2 rotate_left( vec2 x ) {
    vec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for vec2 it just swaps components
SM_ALWAYS_INLINE vec2 rotate_right( vec2 x ) {
    return rotate_left(x);
}
/// sum all components
SM_ALWAYS_INLINE f32 horizontal_add( vec2 x ) {
    return x.x + x.y;
}
/// multiply all components
SM_ALWAYS_INLINE f32 horizontal_mul( vec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
SM_ALWAYS_INLINE vec2 hadamard( vec2 lhs, vec2 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( vec2 lhs, vec2 rhs ) {
    vec2 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( vec2 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( vec2 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE vec2 normalize( vec2 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? VEC2::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( vec2 a, vec2 b ) {
    return acos( dot( a, b ) );
}
/// linear interpolation
SM_ALWAYS_INLINE vec2 lerp( vec2 a, vec2 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec2 lerp_clamped( vec2 a, vec2 b, f32 t ) {
    return lerp( a, b, clamp01(t) );
}
/// smooth step interpolation
SM_ALWAYS_INLINE vec2 smooth_step( vec2 a, vec2 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec2 smooth_step_clamped( vec2 a, vec2 b, f32 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smoother step interpolation
SM_ALWAYS_INLINE vec2 smoother_step(
    vec2 a, vec2 b, f32 t
) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec2 smoother_step_clamped(
    vec2 a, vec2 b, f32 t
) {
    return smoother_step( a, b, clamp01(t) );
}
/// rotate vector by theta radians
SM_ALWAYS_INLINE vec2 rotate( vec2 x, f32 theta ) {
    f32 theta_sin = sin( theta );
    f32 theta_cos = cos( theta );
    vec2 a = {  theta_cos, theta_sin };
    vec2 b = { -theta_sin, theta_cos };
    return ( x.x * a ) + ( x.y * b );
}
/// clamp a vector's magnitude
SM_ALWAYS_INLINE vec2 clamp_mag( vec2 x, f32 max_magnitude ) {
    f32 max       = absolute( max_magnitude );
    f32 magnitude = mag( x );
    if( magnitude > max ) {
        vec2 result = x / magnitude;
        return result * max;
    }
    return x;
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( vec2 a, vec2 b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

inline ivec2 operator+( ivec2 lhs, ivec2 rhs );
inline ivec2 operator-( ivec2 lhs, ivec2 rhs );
inline ivec2 operator*( ivec2 lhs, i32 rhs );
inline ivec2 operator*( i32 lhs, ivec2 rhs );
inline ivec2 operator/( ivec2 lhs, i32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( ivec2 a, ivec2 b );
/// 2-component 32-bit signed integer vector
union ivec2 {
    struct { i32 x, y; };
    struct { i32 u, v; };
    struct { i32 width, height; };
    i32 c[VEC2_COMPONENT_COUNT];

    i32& operator[](u32 index)       { return c[index]; }
    i32  operator[](u32 index) const { return c[index]; }

    b32 operator==( ivec2 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( ivec2 b ) const { return !(*this == b); }

    ivec2 operator-() const { return { -this->x, -this->y }; }
    ivec2& operator+=( ivec2 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    ivec2& operator-=( ivec2 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    ivec2& operator*=( i32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    ivec2& operator/=( i32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE i32* value_pointer( const ivec2& v ) { return (i32*)&v.x; }
SM_ALWAYS_INLINE ivec2 iv2() { return {}; }
SM_ALWAYS_INLINE ivec2 iv2( i32 scalar ) { return { scalar, scalar }; }
SM_ALWAYS_INLINE ivec2 iv2( i32 x, i32 y ) { return { x, y }; }

namespace IVEC2 {
    /// { 1, 1 }
    static const ivec2 ONE   = {  1,  1 };
    /// { 0, 0 }
    static const ivec2 ZERO  = {  0,  0 };
    /// { -1, 0 }
    static const ivec2 LEFT  = { -1,  0 };
    /// { 1, 0 }
    static const ivec2 RIGHT = {  1,  0 };
    /// { 0, 1 }
    static const ivec2 UP    = {  0,  1 };
    /// { 0, -1 }
    static const ivec2 DOWN  = {  0, -1 };
} // namespace smath::VEC2

inline ivec2 operator+( ivec2 lhs, ivec2 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}
inline ivec2 operator-( ivec2 lhs, ivec2 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}
inline ivec2 operator*( ivec2 lhs, i32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs
    };
}
inline ivec2 operator*( i32 lhs, ivec2 rhs ) {
    return operator*( rhs, lhs );
}
inline ivec2 operator/( ivec2 lhs, i32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs
    };
}

/// rotate components to the left. for ivec2 it just swaps components
SM_ALWAYS_INLINE ivec2 rotate_left( ivec2 x ) {
    ivec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for ivec2 it just swaps components
SM_ALWAYS_INLINE ivec2 rotate_right( ivec2 x ) {
    return rotate_left(x);
}
/// sum all components
SM_ALWAYS_INLINE i32 horizontal_add( ivec2 x ) {
    return x.x + x.y;
}
/// multiply all components
SM_ALWAYS_INLINE i32 horizontal_mul( ivec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
SM_ALWAYS_INLINE ivec2 hadamard( ivec2 lhs, ivec2 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( ivec2 lhs, ivec2 rhs ) {
    ivec2 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( ivec2 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( ivec2 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE ivec2 normalize( ivec2 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? IVEC2::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( ivec2 a, ivec2 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( ivec2 a, ivec2 b ) {
    return a.x == b.x && a.y == b.y;
}

inline uvec2 operator+( uvec2 lhs, uvec2 rhs );
inline uvec2 operator-( uvec2 lhs, uvec2 rhs );
inline uvec2 operator*( uvec2 lhs, u32 rhs );
inline uvec2 operator*( u32 lhs, uvec2 rhs );
inline uvec2 operator/( uvec2 lhs, u32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( uvec2 a, uvec2 b );
/// 2-component 32-bit unsigned integer vector
union uvec2 {
    struct { u32 x, y; };
    struct { u32 u, v; };
    struct { u32 width, height; };
    u32 c[VEC2_COMPONENT_COUNT];

    u32& operator[](u32 index)       { return c[index]; }
    u32  operator[](u32 index) const { return c[index]; }

    b32 operator==( uvec2 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( uvec2 b ) const { return !(*this == b); }

    uvec2& operator+=( uvec2 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    uvec2& operator-=( uvec2 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    uvec2& operator*=( u32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    uvec2& operator/=( u32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE u32* value_pointer( const uvec2& v ) { return (u32*)&v.x; }
SM_ALWAYS_INLINE uvec2 uv2() { return {}; }
SM_ALWAYS_INLINE uvec2 uv2( u32 scalar ) { return { scalar, scalar }; }
SM_ALWAYS_INLINE uvec2 uv2( u32 x, u32 y ) { return { x, y }; }

namespace UVEC2 {
    /// { 1, 1 }
    static const uvec2 ONE   = {  1,  1 };
    /// { 0, 0 }
    static const uvec2 ZERO  = {  0,  0 };
} // namespace smath::UVEC2

inline uvec2 operator+( uvec2 lhs, uvec2 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}
inline uvec2 operator-( uvec2 lhs, uvec2 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}
inline uvec2 operator*( uvec2 lhs, u32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs
    };
}
inline uvec2 operator*( u32 lhs, uvec2 rhs ) {
    return operator*( rhs, lhs );
}
inline uvec2 operator/( uvec2 lhs, u32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs
    };
}

/// rotate components to the left. for uvec2 it just swaps components
SM_ALWAYS_INLINE uvec2 rotate_left( uvec2 x ) {
    uvec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for uvec2 it just swaps components
SM_ALWAYS_INLINE uvec2 rotate_right( uvec2 x ) {
    return rotate_left(x);
}
/// sum all components
SM_ALWAYS_INLINE u32 horizontal_add( uvec2 x ) {
    return x.x + x.y;
}
/// multiply all components
SM_ALWAYS_INLINE u32 horizontal_mul( uvec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
SM_ALWAYS_INLINE uvec2 hadamard( uvec2 lhs, uvec2 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( uvec2 lhs, uvec2 rhs ) {
    uvec2 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( uvec2 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( uvec2 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE uvec2 normalize( uvec2 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? UVEC2::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( uvec2 a, uvec2 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( uvec2 a, uvec2 b ) {
    return a.x == b.x && a.y == b.y;
}

inline vec3 operator+( vec3 lhs, vec3 rhs );
inline vec3 operator-( vec3 lhs, vec3 rhs );
inline vec3 operator*( vec3 lhs, f32 rhs );
inline vec3 operator*( f32 lhs, vec3 rhs );
inline vec3 operator/( vec3 lhs, f32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( vec3 a, vec3 b );
static const u32 VEC3_COMPONENT_COUNT = 3;
/// 3-component 32-bit float vector
union vec3 {
    struct {
        union {
            struct { f32 x, y; };
            struct { f32 r, g; };
            struct { f32 hue, value; };
            struct { f32 width, height; };
            struct { f32 pitch, yaw; };
            vec2 xy;
            vec2 rg;
        };
        union {
            f32 z;
            f32 b;
            f32 saturation;
            f32 length;
            f32 roll;
        };
    };
    f32 c[VEC3_COMPONENT_COUNT];

    f32& operator[](u32 index)       { return c[index]; }
    f32  operator[](u32 index) const { return c[index]; }

    b32 operator==( vec3 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( vec3 b ) const { return !(*this == b); }

    vec3 operator-() const { return { -this->x, -this->y, -this->z }; }
    vec3& operator+=( vec3 rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    vec3& operator-=( vec3 rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    vec3& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    vec3& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const vec3& v ) { return (f32*)&v.x; }
SM_ALWAYS_INLINE vec3 v3() { return {}; }
SM_ALWAYS_INLINE vec3 v3( f32 scalar ) {
    return { scalar, scalar, scalar };
}
SM_ALWAYS_INLINE vec3 v3( f32 x, f32 y, f32 z ) {
    return { x, y, z };
}
SM_ALWAYS_INLINE hsv v3_hsv( f32 hue, f32 saturation, f32 value ) {
    return {
        degrees_overflow( hue ),
        clamp01( saturation ),
        clamp01( value )
    };
}

namespace VEC3 {
    /// { 1.0, 1.0, 1.0 }
    static const vec3 ONE     = {  1.0f,  1.0f,  1.0f };
    /// { 0.0, 0.0, 0.0 }
    static const vec3 ZERO    = {  0.0f,  0.0f,  0.0f };
    /// { -1.0, 0.0, 0.0 }
    static const vec3 LEFT    = { -1.0f,  0.0f,  0.0f };
    /// { 1.0, 0.0, 0.0 }
    static const vec3 RIGHT   = {  1.0f,  0.0f,  0.0f };
    /// { 0.0, 1.0, 0.0 }
    static const vec3 UP      = {  0.0f,  1.0f,  0.0f };
    /// { 0.0, -1.0, 0.0 }
    static const vec3 DOWN    = {  0.0f, -1.0f,  0.0f };
    /// { 0.0, 0.0, 1.0 }
    static const vec3 FORWARD = {  0.0f,  0.0f,  1.0f };
    /// { 0.0, 0.0, -1.0 }
    static const vec3 BACK    = {  0.0f,  0.0f, -1.0f };
} // namespace smath::VEC3

namespace RGB {
    /// { 1.0, 0.0, 0.0 }
    static const rgb RED     = { 1.0f, 0.0f, 0.0f };
    /// { 0.0, 1.0, 0.0 }
    static const rgb GREEN   = { 0.0f, 1.0f, 0.0f };
    /// { 0.0, 0.0, 1.0 }
    static const rgb BLUE    = { 0.0f, 0.0f, 1.0f };
    /// { 1.0, 0.0, 1.0 }
    static const rgb MAGENTA = { 1.0f, 0.0f, 1.0f };
    /// { 1.0, 1.0, 0.0 }
    static const rgb YELLOW  = { 1.0f, 1.0f, 0.0f };
    /// { 0.0, 1.0, 1.0 }
    static const rgb CYAN    = { 0.0f, 1.0f, 1.0f };
    /// { 0.0, 0.0, 0.0 }
    static const rgb BLACK   = { 0.0f, 0.0f, 0.0f };
    /// { 1.0, 1.0, 1.0 }
    static const rgb WHITE   = { 1.0f, 1.0f, 1.0f };
    /// { 0.5, 0.5, 0.5 }
    static const rgb GRAY    = { 0.5f, 0.5f, 0.5f };
    /// { 0.5, 0.5, 0.5 }
    static const rgb GREY    = { 0.5f, 0.5f, 0.5f };
} // namespace smath::RGB

inline vec3 operator+( vec3 lhs, vec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline vec3 operator-( vec3 lhs, vec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline vec3 operator*( vec3 lhs, f32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline vec3 operator*( f32 lhs, vec3 rhs ) {
    return operator*( rhs, lhs );
}
inline vec3 operator/( vec3 lhs, f32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}
/// convert rgb color to hsv color
SM_ALWAYS_INLINE hsv rgb_to_hsv( rgb col ) {
    f32 x_max = col.r > col.g ?
        ( col.r > col.b ? col.r : col.b ) :
        ( col.g > col.b ? col.g : col.b );
    f32 x_min = col.r < col.g ?
        ( col.r < col.b ? col.r : col.b ) :
        ( col.g < col.b ? col.g : col.b );
    f32 chroma = x_max - x_min;

    f32 hue;
    if( chroma == 0.0f ) {
        hue = 0.0f;
    } else if( x_max == col.r ) {
        hue = 60.0f * ( 0.0f + ( ( col.g - col.b ) / chroma ) );
    } else if( x_max == col.g ) {
        hue = 60.0f * ( 2.0f + ( ( col.b - col.r ) / chroma ) );
    } else {
        hue = 60.0f * ( 4.0f + ( ( col.r - col.g ) / chroma ) );
    }

    f32 saturation = x_max == 0.0f ? 0.0f : chroma / x_max;

    return v3_hsv( hue, saturation, x_max );
}
/// convert hsv color to rgb color
SM_ALWAYS_INLINE rgb hsv_to_rgb( hsv col ) {
    
    f32 chroma    = col.value * col.saturation;
    f32 hue       = col.hue / 60.0f;
    i32 hue_index = floor32( hue );

    f32 x = chroma * ( 1.0f - absolute( mod( hue, 2.0f ) - 1.0f ) );

    rgb result;

    if( hue_index < 1 ) {
        result = { chroma, x, 0.0f };
    } else if( hue_index < 2 ) {
        result = { x, chroma, 0.0f };
    } else if( hue_index < 3 ) {
        result = { 0.0f, chroma, x };
    } else if( hue_index < 4 ) {
        result = { 0.0f, x, chroma };
    } else if( hue_index < 5 ) {
        result = { x, 0.0f, chroma };
    } else {
        result = { chroma, 0.0f, x };
    }
    
    f32 m = col.value - chroma;

    return {
        result.r + m,
        result.g + m,
        result.b + m
    };
}
/// convert linear color space to srgb color space
SM_ALWAYS_INLINE rgb linear_to_srgb( rgb linear ) {
    static const f32 power = 1.0f / 2.2f;
    return {
        pow( linear.r, power ),
        pow( linear.g, power ),
        pow( linear.b, power ),
    };
}
/// convert srgb color space to linear color space
SM_ALWAYS_INLINE rgb srgb_to_linear( rgb srgb ) {
    return {
        pow( srgb.r, 2.2f ),
        pow( srgb.g, 2.2f ),
        pow( srgb.b, 2.2f ),
    };
}

/// rotate components to the left
SM_ALWAYS_INLINE vec3 rotate_left( vec3 x ) {
    vec3 result;
    result.x = x.y;
    result.y = x.z;
    result.z = x.x;
    return result;
}
/// rotate components to the right
SM_ALWAYS_INLINE vec3 rotate_right( vec3 x ) {
    vec3 result;
    result.x = x.z;
    result.y = x.x;
    result.z = x.y;
    return result;
}
/// sum components
SM_ALWAYS_INLINE f32 horizontal_add( vec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply components
SM_ALWAYS_INLINE f32 horizontal_mul( vec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
SM_ALWAYS_INLINE vec3 hadamard( vec3 lhs, vec3 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( vec3 x ) {
    return horizontal_add( hadamard( x, x ) );
}
/// calculate magnitude of vector
SM_ALWAYS_INLINE f32 mag( vec3 x ) {
    return sqrt( sqrmag( x ) );
}
/// inner product
SM_ALWAYS_INLINE f32 dot( vec3 lhs, vec3 rhs ) {
    return horizontal_add( hadamard( lhs, rhs ) );
}
SM_ALWAYS_INLINE vec3 normalize( vec3 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? VEC3::ZERO : x / magnitude;
}
/// cross product
SM_ALWAYS_INLINE vec3 cross( vec3 lhs, vec3 rhs ) {
    return {
        ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ),
        ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ),
        ( lhs.x * rhs.y ) - ( lhs.y * rhs.x )
    };
}
/// reflect direction across axis of given normal
SM_ALWAYS_INLINE vec3 reflect( vec3 direction, vec3 normal ) {
    return ( normal - direction ) * ( 2.0f *  dot( direction, normal ) );
}
/// calculate the angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( vec3 a, vec3 b ) {
    return acos( dot( a, b ) );
}
/// linear interpolation
SM_ALWAYS_INLINE vec3 lerp( vec3 a, vec3 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec3 lerp_clamped( vec3 a, vec3 b, f32 t ) {
    return lerp( a, b, clamp01( t ) );
}
/// smooth step interpolation
SM_ALWAYS_INLINE vec3 smooth_step( vec3 a, vec3 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec3 smooth_step_clamped( vec3 a, vec3 b, f32 t ) {
    return smooth_step( a, b, clamp01( t ) );
}
/// smoother step interpolation
SM_ALWAYS_INLINE vec3 smoother_step(
    vec3 a, vec3 b, f32 t
) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec3 smoother_step_clamped(
    vec3 a, vec3 b, f32 t
) {
    return smoother_step( a, b, clamp01( t ) );
}
/// clamp a vector's magnitude
SM_ALWAYS_INLINE vec3 clamp_mag( vec3 x, f32 max_magnitude ) {
    f32 max = absolute( max_magnitude );
    f32 mag = ::mag( x );
    if( mag > max ) {
        vec3 result = x / mag;
        result *= max;
        return result;
    }

    return x;
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( vec3 a, vec3 b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

inline ivec3 operator+( ivec3 lhs, ivec3 rhs );
inline ivec3 operator-( ivec3 lhs, ivec3 rhs );
inline ivec3 operator*( ivec3 lhs, i32 rhs );
inline ivec3 operator*( i32 lhs, ivec3 rhs );
inline ivec3 operator/( ivec3 lhs, i32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( ivec3 a, ivec3 b );
/// 3-component 32-bit signed integer vector
union ivec3 {
    struct {
        union {
            struct { i32 x, y; };
            struct { i32 width, height; };
            ivec2 xy;
        };
        union {
            i32 z;
            i32 depth;
        };
    };
    i32 c[VEC3_COMPONENT_COUNT];

    i32& operator[](u32 index)       { return c[index]; }
    i32  operator[](u32 index) const { return c[index]; }

    b32 operator==( ivec3 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( ivec3 b ) const { return !(*this == b); }

    ivec3 operator-() const { return { -this->x, -this->y, -this->z }; }
    ivec3& operator+=( ivec3 rhs ) {
        return *this = operator+(*this, rhs);
    }
    ivec3& operator-=( ivec3 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    ivec3& operator*=( i32 rhs ) {
        return *this = operator*(*this, rhs);
    }
    ivec3& operator/=( i32 rhs ) {
        return *this = operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE i32* value_pointer( const ivec3& v ) { return (i32*)&v.x; }
SM_ALWAYS_INLINE ivec3 iv3() { return {}; }
SM_ALWAYS_INLINE ivec3 iv3( i32 scalar ) { return { scalar, scalar, scalar }; }
SM_ALWAYS_INLINE ivec3 iv3( i32 x, i32 y, i32 z ) { return { x, y, z }; }

namespace IVEC3 {
    /// { 1, 1, 1 }
    static const ivec3 ONE     = {  1,  1,  1 };
    /// { 0, 0, 0 }
    static const ivec3 ZERO    = {  0,  0,  0 };
    /// { -1, 0, 0 }
    static const ivec3 LEFT    = { -1,  0,  0 };
    /// { 1, 0, 0 }
    static const ivec3 RIGHT   = {  1,  0,  0 };
    /// { 0, 1, 0 }
    static const ivec3 UP      = {  0,  1,  0 };
    /// { 0, -1, 0 }
    static const ivec3 DOWN    = {  0, -1,  0 };
    /// { 0, 0, 1 }
    static const ivec3 FORWARD = {  0,  0,  1 };
    /// { 0, 0, -1 }
    static const ivec3 BACK    = {  0,  0, -1 };
} // namespace smath::IVEC3

inline ivec3 operator+( ivec3 lhs, ivec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline ivec3 operator-( ivec3 lhs, ivec3 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}
inline ivec3 operator*( ivec3 lhs, i32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline ivec3 operator*( i32 lhs, ivec3 rhs ) {
    return operator*( rhs, lhs );
}
inline ivec3 operator/( ivec3 lhs, i32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}

/// rotate components to the left
SM_ALWAYS_INLINE ivec3 rotate_left( ivec3 x ) {
    ivec3 out = x;
    out.x = x.z;
    out.y = x.x;
    out.z = x.y;
    return out;
}
/// rotate components to the right
SM_ALWAYS_INLINE ivec3 rotate_right( ivec3 x ) {
    ivec3 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.x;
    return out;
}
/// sum all components
SM_ALWAYS_INLINE i32 horizontal_add( ivec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply all components
SM_ALWAYS_INLINE i32 horizontal_mul( ivec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
SM_ALWAYS_INLINE ivec3 hadamard( ivec3 lhs, ivec3 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( ivec3 lhs, ivec3 rhs ) {
    ivec3 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( ivec3 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( ivec3 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE ivec3 normalize( ivec3 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? IVEC3::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( ivec3 a, ivec3 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( ivec3 a, ivec3 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline uvec3 operator+( uvec3 lhs, uvec3 rhs );
inline uvec3 operator-( uvec3 lhs, uvec3 rhs );
inline uvec3 operator*( uvec3 lhs, u32 rhs );
inline uvec3 operator*( u32 lhs, uvec3 rhs );
inline uvec3 operator/( uvec3 lhs, u32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( uvec3 a, uvec3 b );
/// 3-component 32-bit unsigned integer vector
union uvec3 {
    struct {
        union {
            struct { u32 x, y; };
            struct { u32 width, height; };
            uvec2 xy;
        };
        union {
            u32 z;
            u32 depth;
        };
    };
    u32 c[VEC3_COMPONENT_COUNT];

    u32& operator[](u32 index)       { return c[index]; }
    u32  operator[](u32 index) const { return c[index]; }

    b32 operator==( uvec3 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( uvec3 b ) const { return !(*this == b); }

    uvec3& operator+=( uvec3 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    uvec3& operator-=( uvec3 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    uvec3& operator*=( u32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    uvec3& operator/=( u32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE u32* value_pointer( const uvec3& v ) { return (u32*)&v.x; }
SM_ALWAYS_INLINE uvec3 uv3() { return {}; }
SM_ALWAYS_INLINE uvec3 uv3( u32 scalar ) { return { scalar, scalar, scalar }; }
SM_ALWAYS_INLINE uvec3 uv3( u32 x, u32 y, u32 z ) { return { x, y, z }; }

namespace UVEC3 {
    /// { 1, 1, 1 }
    static const uvec3 ONE  = {  1,  1,  1 };
    /// { 0, 0, 0 }
    static const uvec3 ZERO = {  0,  0,  0 };
} // namespace smath::UVEC3

inline uvec3 operator+( uvec3 lhs, uvec3 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline uvec3 operator-( uvec3 lhs, uvec3 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}
inline uvec3 operator*( uvec3 lhs, u32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline uvec3 operator*( u32 lhs, uvec3 rhs ) {
    return operator*( rhs, lhs );
}
inline uvec3 operator/( uvec3 lhs, u32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}

/// rotate components to the left
SM_ALWAYS_INLINE uvec3 rotate_left( uvec3 x ) {
    uvec3 out = x;
    out.x = x.z;
    out.y = x.x;
    out.z = x.y;
    return out;
}
/// rotate components to the right
SM_ALWAYS_INLINE uvec3 rotate_right( uvec3 x ) {
    uvec3 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.x;
    return out;
}
/// sum all components
SM_ALWAYS_INLINE u32 horizontal_add( uvec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply all components
SM_ALWAYS_INLINE u32 horizontal_mul( uvec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
SM_ALWAYS_INLINE uvec3 hadamard( uvec3 lhs, uvec3 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( uvec3 lhs, uvec3 rhs ) {
    uvec3 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( uvec3 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( uvec3 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE uvec3 normalize( uvec3 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? UVEC3::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( uvec3 a, uvec3 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( uvec3 a, uvec3 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline vec4 operator+( vec4 lhs, vec4 rhs );
inline vec4 operator-( vec4 lhs, vec4 rhs );
inline vec4 operator*( vec4 lhs, f32 rhs );
inline vec4 operator*( f32 lhs, vec4 rhs );
inline vec4 operator/( vec4 lhs, f32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( vec4 a, vec4 b );
static const u32 VEC4_COMPONENT_COUNT = 4;
/// 4-component 32-bit float vector
union vec4 {
    struct {
        union {
            struct {
                union {
                    struct { f32 x, y; };
                    struct { f32 r, g; };
                    vec2 xy;
                    vec2 rg;
                };
                union {
                    f32 z;
                    f32 b;
                };
            };
            vec3 xyz;
            vec3 rgb;
        };
        union {
            f32 w;
            f32 a;
        };
    };
    f32 c[VEC4_COMPONENT_COUNT];

    f32& operator[](u32 index)       { return c[index]; }
    f32  operator[](u32 index) const { return c[index]; }

    b32 operator==( vec4 rhs ) const { return cmp_eq( *this, rhs ); }
    b32 operator!=( vec4 rhs ) const { return !(*this == rhs); }

    vec4 operator-() const {
        return { -this->x, -this->y, -this->z, -this->w };
    }
    vec4& operator+=( vec4 rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    vec4& operator-=( vec4 rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    vec4& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    vec4& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const vec4& v ) { return (f32*)&v.x; }
SM_ALWAYS_INLINE vec4 v4() { return {}; }
SM_ALWAYS_INLINE vec4 v4( f32 scalar ) {
    return { scalar, scalar, scalar, scalar };
}
SM_ALWAYS_INLINE vec4 v4( f32 x, f32 y, f32 z, f32 w ) {
    return { x, y, z, w };
}

namespace VEC4 {
    /// { 1.0, 1.0, 1.0, 1.0 }
    static const vec4 ONE  = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// { 0.0, 0.0, 0.0, 0.0 }
    static const vec4 ZERO = { 0.0f, 0.0f, 0.0f, 0.0f };
} // namespace smath::VEC4

namespace RGBA {
    /// { 1.0, 0.0, 0.0, 1.0 }
    static const rgba RED     = { 1.0f, 0.0f, 0.0f, 1.0f };
    /// { 0.0, 1.0, 0.0, 1.0 }
    static const rgba GREEN   = { 0.0f, 1.0f, 0.0f, 1.0f };
    /// { 0.0, 0.0, 1.0, 1.0 }
    static const rgba BLUE    = { 0.0f, 0.0f, 1.0f, 1.0f };
    /// { 1.0, 0.0, 1.0, 1.0 }
    static const rgba MAGENTA = { 1.0f, 0.0f, 1.0f, 1.0f };
    /// { 1.0, 1.0, 0.0, 1.0 }
    static const rgba YELLOW  = { 1.0f, 1.0f, 0.0f, 1.0f };
    /// { 0.0, 1.0, 1.0, 1.0 }
    static const rgba CYAN    = { 0.0f, 1.0f, 1.0f, 1.0f };
    /// { 0.0, 0.0, 0.0, 1.0 }
    static const rgba BLACK   = { 0.0f, 0.0f, 0.0f, 1.0f };
    /// { 1.0, 1.0, 1.0, 1.0 }
    static const rgba WHITE   = { 1.0f, 1.0f, 1.0f, 1.0f };
    /// { 0.5, 0.5, 0.5, 1.0 }
    static const rgba GRAY    = { 0.5f, 0.5f, 0.5f, 1.0f };
    /// { 0.5, 0.5, 0.5, 1.0 }
    static const rgba GREY    = { 0.5f, 0.5f, 0.5f, 1.0f };
    /// { 0.0, 0.0, 0.0, 0.0 }
    static const rgba CLEAR   = { 0.0f, 0.0f, 0.0f, 0.0f };
} // namespace smath::RGBA

inline vec4 operator+( vec4 lhs, vec4 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
}
inline vec4 operator-( vec4 lhs, vec4 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
}
inline vec4 operator*( vec4 lhs, f32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
}
inline vec4 operator*( f32 lhs, vec4 rhs ) {
    return operator*( rhs, lhs );
}
inline vec4 operator/( vec4 lhs, f32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
}

/// convert linear color space to srgb color space
SM_ALWAYS_INLINE rgba linear_to_srgb( rgba linear ) {
    rgba result;
    result.rgb = linear_to_srgb( linear.rgb );
    result.a = linear.a;
    return result;
}
/// convert srgb color space to linear color space
SM_ALWAYS_INLINE rgba srgb_to_linear( rgba srgb ) {
    rgba result;
    result.rgb = srgb_to_linear( srgb.rgb );
    result.a   = srgb.a;
    return result;
}
/// rotate components to the left
SM_ALWAYS_INLINE vec4 rotate_left( vec4 x ) {
    vec4 result;
    result.x = x.y;
    result.y = x.z;
    result.z = x.w;
    result.w = x.x;
    return result;
}
/// rotate components to the right
SM_ALWAYS_INLINE vec4 rotate_right( vec4 x ) {
    vec4 result;
    result.x = x.w;
    result.y = x.x;
    result.z = x.y;
    result.w = x.z;
    return result;
}
/// sum components
SM_ALWAYS_INLINE f32 horizontal_add( vec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply components
SM_ALWAYS_INLINE f32 horizontal_mul( vec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
SM_ALWAYS_INLINE vec4 hadamard( vec4 lhs, vec4 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( vec4 x ) {
    return horizontal_add( hadamard( x, x ) );
}
/// calculate magnitude of vector
SM_ALWAYS_INLINE f32 mag( vec4 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE vec4 normalize( vec4 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? VEC4::ZERO : x / magnitude;
}
/// inner product
SM_ALWAYS_INLINE f32 dot( vec4 lhs, vec4 rhs ) {
    return horizontal_add( hadamard( lhs, rhs ) );
}
/// linear interpolation
SM_ALWAYS_INLINE vec4 lerp( vec4 a, vec4 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec4 lerp_clamped( vec4 a, vec4 b, f32 t ) {
    return lerp( a, b, clamp01( t ) );
}
/// smooth step interpolation
SM_ALWAYS_INLINE vec4 smooth_step( vec4 a, vec4 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec4 smooth_step_clamped( vec4 a, vec4 b, f32 t ) {
    return smooth_step( a, b, clamp01( t ) );
}
/// smoother step interpolation
SM_ALWAYS_INLINE vec4 smoother_step(
    vec4 a, vec4 b, f32 t
) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
SM_ALWAYS_INLINE vec4 smoother_step_clamped(
    vec4 a, vec4 b, f32 t
) {
    return smoother_step( a, b, clamp01( t ) );
}
/// clamp a vector's magnitude
SM_ALWAYS_INLINE vec4 clamp_mag( vec4 x, f32 max_magnitude ) {
    f32 max = absolute( max_magnitude );
    f32 mag = ::mag( x );
    if( mag > max ) {
        vec4 result = x / mag;
        result *= max;
        return result;
    }

    return x;
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( vec4 a, vec4 b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

inline ivec4 operator+( ivec4 lhs, ivec4 rhs );
inline ivec4 operator-( ivec4 lhs, ivec4 rhs );
inline ivec4 operator*( ivec4 lhs, i32 rhs );
inline ivec4 operator*( i32 lhs, ivec4 rhs );
inline ivec4 operator/( ivec4 lhs, i32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( ivec4 a, ivec4 b );
/// 4-component 32-bit signed integer vector
union ivec4 {
    struct {
        union {
            struct {
                union {
                    struct { i32 x, y; };
                    ivec2 xy;
                };
                i32 z;
            };
            ivec3 xyz;
        };
        i32 w;
    };
    i32 c[VEC4_COMPONENT_COUNT];

    i32& operator[](u32 index)       { return c[index]; }
    i32  operator[](u32 index) const { return c[index]; }

    b32 operator==( ivec4 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( ivec4 b ) const { return !(*this == b); }

    ivec4 operator-() const { return { -this->x, -this->y, -this->z, -this->w }; }
    ivec4& operator+=( ivec4 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    ivec4& operator-=( ivec4 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    ivec4& operator*=( i32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    ivec4& operator/=( i32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE i32* value_pointer( const ivec4& v ) { return (i32*)&v.x; }
SM_ALWAYS_INLINE ivec4 iv4() { return {}; }
SM_ALWAYS_INLINE ivec4 iv4( i32 scalar ) { return { scalar, scalar, scalar, scalar }; }
SM_ALWAYS_INLINE ivec4 iv4( i32 x, i32 y, i32 z, i32 w ) { return { x, y, z, w }; }

namespace IVEC4 {
    /// { 1, 1, 1, 1 }
    static const ivec4 ONE     = { 1, 1, 1, 1 };
    /// { 0, 0, 0, 0 }
    static const ivec4 ZERO    = { 0, 0, 0, 0 };
} // namespace smath::IVEC4

inline ivec4 operator+( ivec4 lhs, ivec4 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
}
inline ivec4 operator-( ivec4 lhs, ivec4 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
}
inline ivec4 operator*( ivec4 lhs, i32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
}
inline ivec4 operator*( i32 lhs, ivec4 rhs ) {
    return operator*( rhs, lhs );
}
inline ivec4 operator/( ivec4 lhs, i32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
}

/// rotate components to the left
SM_ALWAYS_INLINE ivec4 rotate_left( ivec4 x ) {
    ivec4 out = x;
    out.x = x.w;
    out.y = x.x;
    out.z = x.y;
    out.w = x.z;
    return out;
}
/// rotate components to the right
SM_ALWAYS_INLINE ivec4 rotate_right( ivec4 x ) {
    ivec4 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.w;
    out.w = x.x;
    return out;
}
/// sum all components
SM_ALWAYS_INLINE i32 horizontal_add( ivec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply all components
SM_ALWAYS_INLINE i32 horizontal_mul( ivec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
SM_ALWAYS_INLINE ivec4 hadamard( ivec4 lhs, ivec4 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( ivec4 lhs, ivec4 rhs ) {
    ivec4 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( ivec4 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( ivec4 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE ivec4 normalize( ivec4 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? IVEC4::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( ivec4 a, ivec4 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( ivec4 a, ivec4 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline uvec4 operator+( uvec4 lhs, uvec4 rhs );
inline uvec4 operator-( uvec4 lhs, uvec4 rhs );
inline uvec4 operator*( uvec4 lhs, u32 rhs );
inline uvec4 operator*( u32 lhs, uvec4 rhs );
inline uvec4 operator/( uvec4 lhs, u32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( uvec4 a, uvec4 b );
/// 4-component 32-bit unsigned integer vector
union uvec4 {
    struct {
        union {
            struct {
                union {
                    struct { u32 x, y; };
                    uvec2 xy;
                };
                u32 z;
            };
            uvec3 xyz;
        };
        u32 w;
    };
    u32 c[VEC4_COMPONENT_COUNT];

    u32& operator[](u32 index)       { return c[index]; }
    u32  operator[](u32 index) const { return c[index]; }

    b32 operator==( uvec4 b ) const { return cmp_eq( *this, b ); }
    b32 operator!=( uvec4 b ) const { return !(*this == b); }

    uvec4& operator+=( uvec4 rhs ) {
        return *this = ::operator+(*this, rhs);
    }
    uvec4& operator-=( uvec4 rhs ) {
        return *this = ::operator-(*this, rhs);
    }
    uvec4& operator*=( u32 rhs ) {
        return *this = ::operator*(*this, rhs);
    }
    uvec4& operator/=( u32 rhs ) {
        return *this = ::operator/(*this, rhs);
    }
};
/// get pointer to value
SM_ALWAYS_INLINE u32* value_pointer( const uvec4& v ) { return (u32*)&v.x; }
SM_ALWAYS_INLINE uvec4 uv4() { return {}; }
SM_ALWAYS_INLINE uvec4 uv4( u32 scalar ) { return { scalar, scalar, scalar, scalar }; }
SM_ALWAYS_INLINE uvec4 uv4( u32 x, u32 y, u32 z, u32 w ) { return { x, y, z, w }; }

namespace UVEC4 {
    /// { 1, 1, 1, 1 }
    static const uvec4 ONE  = { 1, 1, 1, 1 };
    /// { 0, 0, 0, 0 }
    static const uvec4 ZERO = { 0, 0, 0, 0 };
} // namespace smath::UVEC4

inline uvec4 operator+( uvec4 lhs, uvec4 rhs ) {
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
}
inline uvec4 operator-( uvec4 lhs, uvec4 rhs ) {
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
}
inline uvec4 operator*( uvec4 lhs, u32 rhs ) {
    return {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
}
inline uvec4 operator*( u32 lhs, uvec4 rhs ) {
    return operator*( rhs, lhs );
}
inline uvec4 operator/( uvec4 lhs, u32 rhs ) {
    return {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
}

/// rotate components to the left
SM_ALWAYS_INLINE uvec4 rotate_left( uvec4 x ) {
    uvec4 out = x;
    out.x = x.w;
    out.y = x.x;
    out.z = x.y;
    out.w = x.z;
    return out;
}
/// rotate components to the right
SM_ALWAYS_INLINE uvec4 rotate_right( uvec4 x ) {
    uvec4 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.w;
    out.w = x.x;
    return out;
}
/// sum all components
SM_ALWAYS_INLINE i32 horizontal_add( uvec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply all components
SM_ALWAYS_INLINE i32 horizontal_mul( uvec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
SM_ALWAYS_INLINE uvec4 hadamard( uvec4 lhs, uvec4 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// inner product
SM_ALWAYS_INLINE f32 dot( uvec4 lhs, uvec4 rhs ) {
    uvec4 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
SM_ALWAYS_INLINE f32 sqrmag( uvec4 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
SM_ALWAYS_INLINE f32 mag( uvec4 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
SM_ALWAYS_INLINE uvec4 normalize( uvec4 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? UVEC4::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
SM_ALWAYS_INLINE f32 angle( uvec4 a, uvec4 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
SM_ALWAYS_INLINE b32 cmp_eq( uvec4 a, uvec4 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/// 3D rotation represented as angle and axis
struct angle_axis {
    vec3 axis;
    f32  theta;
};

inline quat operator+( quat lhs, quat rhs );
inline quat operator-( quat lhs, quat rhs );
inline quat operator*( quat lhs, f32 rhs );
inline quat operator/( quat lhs, f32 rhs );
SM_ALWAYS_INLINE b32 cmp_eq( quat a, quat b );
SM_API quat operator*( quat lhs, quat rhs );
static const u32 QUATERNION_COMPONENT_COUNT = 4;
/// 3D rotation represented as a quaternion
union quat {
    struct {
        union {
            f32 w;
            f32 a;
        };
        union {
            struct { f32 x, y, z; };
            struct { f32 b, c, d; };
            vec3 xyz;
        };
    };
    f32 q[QUATERNION_COMPONENT_COUNT];

    f32& operator[](u32 index)       { return q[index]; }
    f32  operator[](u32 index) const { return q[index]; }

    b32 operator==( quat rhs ) const { return cmp_eq( *this, rhs ); }
    b32 operator!=( quat rhs ) const { return !(*this == rhs); }

    quat operator-() const {
        return {
            -this->w, -this->x,
            -this->y, -this->z
        };
    }
    quat& operator+=( quat rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    quat& operator-=( quat rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    quat& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    quat& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const quat& v ) { return (f32*)&v.w; }
SM_ALWAYS_INLINE quat q() { return {}; }
SM_ALWAYS_INLINE quat q( f32 w, f32 x, f32 y, f32 z ) {
    return { w, x, y, z };
}
SM_ALWAYS_INLINE quat q( f32 theta, vec3 axis ) {
    f32 half_theta = theta / 2.0f;
    f32 sine = sin( half_theta );
    return {
        cos( half_theta ),
        axis.x * sine,
        axis.y * sine,
        axis.z * sine,
    };
}
SM_ALWAYS_INLINE quat q( angle_axis aa ) {
    return q( aa.theta, aa.axis );
}
SM_ALWAYS_INLINE quat q( f32 pitch, f32 yaw, f32 roll ) {
    // TODO(alicia): SIMD ?
    f32 half_x = pitch / 2.0f;
    f32 half_y = yaw   / 2.0f;
    f32 half_z = roll  / 2.0f;

    f32 x_sin = sin( half_x );
    f32 y_sin = sin( half_y );
    f32 z_sin = sin( half_z );

    f32 x_cos = cos( half_x );
    f32 y_cos = cos( half_y );
    f32 z_cos = cos( half_z );

    f32 xyz_cos = x_cos * y_cos * z_cos;
    f32 xyz_sin = x_sin * y_sin * z_sin;

    return {
        ( xyz_cos ) + ( xyz_sin ),
        ( x_sin * y_cos * z_cos ) + ( x_cos * y_sin * z_sin ),
        ( x_cos * y_sin * z_cos ) + ( x_sin * y_cos * z_sin ),
        ( x_cos * y_cos * z_sin ) + ( x_sin * y_sin * z_cos )
    };
}
SM_ALWAYS_INLINE quat q( euler_angles euler ) {
    return q( euler.pitch, euler.yaw, euler.roll );
}

namespace QUAT {
    /// { 0.0, 0.0, 0.0, 0.0 }
    static const quat ZERO     = { 0.0f, 0.0f, 0.0f, 0.0f };
    /// { 1.0, 0.0, 0.0, 0.0 }
    static const quat IDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f };
} // namespace smath::QUAT

inline quat operator+( quat lhs, quat rhs ) {
    return {
        lhs.w + rhs.w,
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}
inline quat operator-( quat lhs, quat rhs ) {
    return {
        lhs.w - rhs.w,
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}
inline quat operator*( quat lhs, f32 rhs ) {
    return {
        lhs.w * rhs,
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
}
inline quat operator*( f32 lhs, quat rhs ) {
    return operator*( rhs, lhs );
}
inline quat operator/( quat lhs, f32 rhs ) {
    return {
        lhs.w / rhs,
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
}
inline vec3 operator*( quat lhs, vec3 rhs ) {
    vec3 t = 2.0f * cross( lhs.xyz, rhs );
    return rhs + t * lhs.w + cross( lhs.xyz, t );
}

/// calculate square magnitude of quaternion
SM_ALWAYS_INLINE f32 sqrmag( quat q ) {
    vec4 result = { q.w, q.x, q.y, q.z };
    return horizontal_add( hadamard( result, result ) );
}
/// calculate magnitude of quaternion
SM_ALWAYS_INLINE f32 mag( quat q ) {
    return sqrt( sqrmag( q ) );
}
/// normalize quaternion
SM_ALWAYS_INLINE quat normalize( quat q ) {
    f32 magnitude = mag( q );
    return magnitude < F32::EPSILON ? QUAT::IDENTITY : q / magnitude;
}
/// conjugate of quaternion
SM_ALWAYS_INLINE quat conjugate( quat q ) {
    return { q.w, -q.x, -q.y, -q.z };
}
/// calculate inverse quaternion
SM_ALWAYS_INLINE quat inverse( quat q ) {
    return conjugate( q ) / sqrmag( q );
}
/// angle between quaternions
SM_ALWAYS_INLINE f32 angle( quat a, quat b ) {
    quat qd = inverse( a ) * b;
    return 2.0f * atan2( mag( qd.xyz ), qd.w );
}
/// inner product
SM_ALWAYS_INLINE f32 dot( quat lhs, quat rhs ) {
    vec4 l = { lhs.w, lhs.x, lhs.y, lhs.z };
    vec4 r = { rhs.w, rhs.x, rhs.y, rhs.z };
    return horizontal_add( hadamard( l, r ) );
}
/// linear interpolation
SM_ALWAYS_INLINE quat lerp( quat a, quat b, f32 t ) {
    return normalize( ( 1.0f - t ) * a + b * t );
}
/// linear interpolation, t is clamped 0-1
SM_ALWAYS_INLINE quat lerp_clamped( quat a, quat b, f32 t ) {
    return lerp( a, b, clamp01( t ) );
}
/// spherical linear interpolation
SM_ALWAYS_INLINE quat slerp( quat a, quat b, f32 t ) {
    quat _b = b;
    f32 theta = dot(a, b);
    f32 cos_theta = cos( theta );
    if( cos_theta < 0.0f ) {
        _b = -_b;
        cos_theta = -cos_theta;
    }
    if( cos_theta > 1.0f - F32::EPSILON ) {
        return lerp( a, b, t );
    } else {
        return normalize(
            ( sin( ( 1.0f - t ) * theta ) * a + sin( t * theta ) * _b )
            / sin( theta )
        );
    }
}
/// compare quaternions for equality
SM_ALWAYS_INLINE b32 cmp_eq( quat a, quat b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

inline mat2 operator+( mat2 lhs, mat2 rhs );
inline mat2 operator-( mat2 lhs, mat2 rhs );
inline mat2 operator*( mat2 lhs, f32 rhs );
inline mat2 operator/( mat2 lhs, f32 rhs );
static const u32 MAT2_CELL_COUNT   = 4;
static const u32 MAT2_COLUMN_COUNT = 2;
/// column-major 2x2 32-bit float matrix
union mat2 {
    struct {
        union {
            struct { f32 m00, m01; };
            vec2 col0;
        };
        union {
            struct { f32 m10, m11; };
            vec2 col1;
        };
    };
    vec2 v[MAT2_COLUMN_COUNT];
    f32  c[MAT2_CELL_COUNT];

    vec2& operator[]( u32 col ) { return v[col]; }
    const vec2& operator[](u32 col) const { return v[col]; }

    vec2 column( u32 col ) const { return (*this)[col]; }
    vec2 row( u32 row ) const { return { this->c[row], this->c[row + 2] }; }

    mat2& operator+=( mat2 rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    mat2& operator-=( mat2 rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    mat2& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    mat2& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const mat2& m ) { return (f32*)&m.m00; }
SM_ALWAYS_INLINE mat2 m2() { return {}; }
SM_ALWAYS_INLINE mat2 m2( f32 m00, f32 m01, f32 m10, f32 m11 ) {
    return { m00, m01, m10, m11 };
}

namespace MAT2 {
    /// all zero
    static const mat2 ZERO     = { 0.0f, 0.0f, 0.0f, 0.0f };
    /// identity matrix
    static const mat2 IDENTITY = {
        1.0f, 0.0f,
        0.0f, 1.0f
    };
} // namespace smath::MAT2

inline mat2 operator+( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 + rhs.col0;
    result.col1 = lhs.col1 + rhs.col1;
    return result;
}
inline mat2 operator-( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 - rhs.col0;
    result.col1 = lhs.col1 - rhs.col1;
    return result;
}
inline mat2 operator*( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 * rhs;
    result.col1 = lhs.col1 * rhs;
    return result;
}
inline mat2 operator*( f32 lhs, mat2 rhs ) {
    return operator*( rhs, lhs );
}
inline mat2 operator/( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = lhs.col0 / rhs;
    result.col1 = lhs.col1 / rhs;
    return result;
}
inline mat2 operator*( mat2 lhs, mat2 rhs ) {
    return {
        (lhs.c[0] * rhs.c[0]) + (lhs.c[2] * rhs.c[1]),
        (lhs.c[1] * rhs.c[0]) + (lhs.c[3] * rhs.c[2]),

        (lhs.c[0] * rhs.c[2]) + (lhs.c[2] * rhs.c[3]),
        (lhs.c[1] * rhs.c[2]) + (lhs.c[3] * rhs.c[3])
    };
}
/// transpose matrix
SM_ALWAYS_INLINE mat2 transpose( mat2 m ) {
    return {
        m.c[0], m.c[2],
        m.c[1], m.c[3]
    };
}
/// calculate determinant
SM_ALWAYS_INLINE f32 determinant( mat2 m ) {
    return ( m.c[0] * m.c[3] ) - ( m.c[2] * m.c[1] );
}

inline mat3 operator+( const mat3& lhs, const mat3& rhs );
inline mat3 operator-( const mat3& lhs, const mat3& rhs );
inline mat3 operator*( const mat3& lhs, f32 rhs );
inline mat3 operator/( const mat3& lhs, f32 rhs );
static const u32 MAT3_CELL_COUNT   = 9;
static const u32 MAT3_COLUMN_COUNT = 3;
/// column-major 3x3 32-bit float matrix
union mat3 {
    struct {
        union {
            struct { f32 m00, m01, m02; };
            vec3 col0;
        };
        union {
            struct { f32 m10, m11, m12; };
            vec3 col1;
        };
        union {
            struct { f32 m20, m21, m22; };
            vec3 col2;
        };
    };
    vec3 v[MAT3_COLUMN_COUNT];
    f32  c[MAT3_CELL_COUNT];

    vec3&       operator[]( u32 col )       { return v[col]; }
    const vec3& operator[]( u32 col ) const { return v[col]; }

    vec3 column( u32 col ) const { return (*this)[col]; }
    vec3 row( u32 row )    const {
        return {
            this->c[row],
            this->c[row + 3],
            this->c[row + 6],
        };
    }

    mat3& operator+=( const mat3& rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    mat3& operator-=( const mat3& rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    mat3& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    mat3& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const mat3& m ) { return (f32*)&m.m00; }
SM_ALWAYS_INLINE mat3 m3() { return {}; }
SM_ALWAYS_INLINE mat3 m3(
    f32 m00, f32 m01, f32 m02,
    f32 m10, f32 m11, f32 m12,
    f32 m20, f32 m21, f32 m22
) {
    return {
        m00, m01, m02,
        m10, m11, m12,
        m20, m21, m22
    };
}

namespace MAT3 {
    /// all zero
    static const mat3 ZERO = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };
    /// identity matrix
    static const mat3 IDENTITY = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
} // namespace smath::MAT3

inline mat3 operator+( const mat3& lhs, const mat3& rhs ) {
    mat3 result;
    result.col0 = lhs.col0 + rhs.col0;
    result.col1 = lhs.col1 + rhs.col1;
    result.col2 = lhs.col2 + rhs.col2;
    return result;
}
inline mat3 operator-( const mat3& lhs, const mat3& rhs ) {
    mat3 result;
    result.col0 = lhs.col0 - rhs.col0;
    result.col1 = lhs.col1 - rhs.col1;
    result.col2 = lhs.col2 - rhs.col2;
    return result;
}
inline mat3 operator*( const mat3& lhs, f32 rhs ) {
    mat3 result;
    result.col0 = lhs.col0 * rhs;
    result.col1 = lhs.col1 * rhs;
    result.col2 = lhs.col2 * rhs;
    return result;
}
inline mat3 operator*( f32 lhs, const mat3& rhs ) {
    return operator*( rhs, lhs );
}
inline mat3 operator/( const mat3& lhs, f32 rhs ) {
    mat3 result;
    result.col0 = lhs.col0 / rhs;
    result.col1 = lhs.col1 / rhs;
    result.col2 = lhs.col2 / rhs;
    return result;
}
inline mat3 operator*( const mat3& lhs, const mat3& rhs ) {
    // TODO(alicia): SIMD?
    return {
        // column - 0
        ( lhs.c[0] * rhs.c[0] )     +
            ( lhs.c[3] * rhs.c[1] ) +
            ( lhs.c[6] * rhs.c[2] ),
        ( lhs.c[1] * rhs.c[0] )     +
            ( lhs.c[4] * rhs.c[1] ) +
            ( lhs.c[7] * rhs.c[2] ),
        ( lhs.c[2] * rhs.c[0] )     +
            ( lhs.c[5] * rhs.c[1] ) +
            ( lhs.c[8] * rhs.c[2] ),
        // column - 1
        ( lhs.c[0] * rhs.c[3] )     +
            ( lhs.c[3] * rhs.c[4] ) +
            ( lhs.c[6] * rhs.c[5] ),
        ( lhs.c[1] * rhs.c[3] )     +
            ( lhs.c[4] * rhs.c[4] ) +
            ( lhs.c[7] * rhs.c[5] ),
        ( lhs.c[2] * rhs.c[3] )     +
            ( lhs.c[5] * rhs.c[4] ) +
            ( lhs.c[8] * rhs.c[5] ),
        // column - 2
        ( lhs.c[0] * rhs.c[6] )     +
            ( lhs.c[3] * rhs.c[7] ) +
            ( lhs.c[6] * rhs.c[8] ),
        ( lhs.c[1] * rhs.c[6] )     +
            ( lhs.c[4] * rhs.c[7] ) +
            ( lhs.c[7] * rhs.c[8] ),
        ( lhs.c[2] * rhs.c[6] )     +
            ( lhs.c[5] * rhs.c[7] ) +
            ( lhs.c[8] * rhs.c[8] ),
    };
}
/// transpose matrix
SM_ALWAYS_INLINE mat3 transpose( const mat3& m ) {
    return {
        m.c[0], m.c[3], m.c[6],
        m.c[1], m.c[4], m.c[7],
        m.c[2], m.c[5], m.c[8]
    };
}
/// calculate determinant
SM_ALWAYS_INLINE f32 determinant( const mat3& m ) {
    return
     ( m.c[0] * ( ( m.c[4] * m.c[8] ) - ( m.c[7] * m.c[5] ) ) ) +
    -( m.c[3] * ( ( m.c[1] * m.c[8] ) - ( m.c[7] * m.c[2] ) ) ) +
     ( m.c[6] * ( ( m.c[1] * m.c[5] ) - ( m.c[4] * m.c[2] ) ) );
}

SM_API mat4 operator+( const mat4& lhs, const mat4& rhs );
SM_API mat4 operator-( const mat4& lhs, const mat4& rhs );
SM_API mat4 operator*( const mat4& lhs, f32 rhs );
SM_API mat4 operator/( const mat4& lhs, f32 rhs );
static const u32 MAT4_CELL_COUNT   = 16;
static const u32 MAT4_COLUMN_COUNT = 4;
/// column-major 4x4 32-bit float matrix
union mat4 {
    struct {
        union {
            struct { f32 m00, m01, m02, m03; };
            vec4 col0;
        };
        union {
            struct { f32 m10, m11, m12, m13; };
            vec4 col1;
        };
        union {
            struct { f32 m20, m21, m22, m23; };
            vec4 col2;
        };
        union {
            struct { f32 m30, m31, m32, m33; };
            vec4 col3;
        };
    };
    vec4 v[MAT4_COLUMN_COUNT];
    f32  c[MAT4_CELL_COUNT];

    vec4&       operator[]( u32 col )       { return v[col]; }
    const vec4& operator[]( u32 col ) const { return v[col]; }

    vec4 column( u32 col ) const { return (*this)[col]; }
    vec4 row( u32 row )    const {
        return {
            this->c[row],
            this->c[row + 4],
            this->c[row + 8],
            this->c[row + 12]
        };
    }

    mat4& operator+=( const mat4& rhs ) {
        return *this = ::operator+( *this, rhs );
    }
    mat4& operator-=( const mat4& rhs ) {
        return *this = ::operator-( *this, rhs );
    }
    mat4& operator*=( f32 rhs ) {
        return *this = ::operator*( *this, rhs );
    }
    mat4& operator/=( f32 rhs ) {
        return *this = ::operator/( *this, rhs );
    }
};
/// get pointer to value
SM_ALWAYS_INLINE f32* value_pointer( const mat4& m ) {
    return (f32*)&m.m00;
}
SM_ALWAYS_INLINE mat4 m4() { return {}; }
SM_ALWAYS_INLINE mat4 m4(
    f32 m00, f32 m01, f32 m02, f32 m03,
    f32 m10, f32 m11, f32 m12, f32 m13,
    f32 m20, f32 m21, f32 m22, f32 m23,
    f32 m30, f32 m31, f32 m32, f32 m33
) {
    return {
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    };
}

namespace MAT4 {
    static const mat4 ZERO = {
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };
    static const mat4 IDENTITY = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
} // namespace smath::MAT4

SM_ALWAYS_INLINE mat4 operator*( f32 lhs, const mat4& rhs ) {
    return operator*( rhs, lhs );
}
mat4 operator*( const mat4& lhs, const mat4& rhs );
SM_ALWAYS_INLINE vec3 operator*( const mat4& lhs, vec3 rhs ) {
    // TODO(alicia): SIMD?
    return {
        ( lhs.c[0] * rhs[0] )     +
            ( lhs.c[4] * rhs[1] ) +
            ( lhs.c[8] * rhs[2] ) +
            lhs.c[12],
        ( lhs.c[1] * rhs[0] )     +
            ( lhs.c[5] * rhs[1] ) +
            ( lhs.c[9] * rhs[2] ) +
            lhs.c[13],
        ( lhs.c[2] * rhs[0] )      +
            ( lhs.c[6]  * rhs[1] ) +
            ( lhs.c[10] * rhs[2] ) +
            lhs.c[14],
    };
}
SM_ALWAYS_INLINE vec4 operator*( const mat4& lhs, vec4 rhs ) {
    // TODO(alicia): SIMD?
    return {
        ( lhs.c[0] * rhs[0] )      +
            ( lhs.c[4]  * rhs[1] ) +
            ( lhs.c[8]  * rhs[2] ) +
            ( lhs.c[12] * rhs[3] ),
        ( lhs.c[1] * rhs[0] )      +
            ( lhs.c[5]  * rhs[1] ) +
            ( lhs.c[9]  * rhs[2] ) +
            ( lhs.c[13] * rhs[3] ),
        ( lhs.c[2] * rhs[0] )      +
            ( lhs.c[6]  * rhs[1] ) +
            ( lhs.c[10] * rhs[2] ) +
            ( lhs.c[14] * rhs[3] ),
        ( lhs.c[3] * rhs[0] )      +
            ( lhs.c[7]  * rhs[1] ) +
            ( lhs.c[11] * rhs[2] ) +
            ( lhs.c[15] * rhs[3] ),
    };
}

/// create new look at matrix
SM_ALWAYS_INLINE mat4 lookat(
    vec3 position,
    vec3 target,
    vec3 up
) {
    vec3 z = normalize( target - position );
    vec3 x = cross( z, up );
    vec3 y = cross( x, z );
    z = -z;

    return {
        x.x, y.x, z.x, 0.0f,
        x.y, y.y, z.y, 0.0f,
        x.z, y.z, z.z, 0.0f,

        -dot( x, position ),
        -dot( y, position ),
        -dot( z, position ),
        1.0f
    };
}
/// create new orthographic projection matrix
SM_ALWAYS_INLINE mat4 ortho(
    f32 left, f32 right,
    f32 bottom, f32 top,
    f32 near_, f32 far_
) {
    mat4 result = MAT4::IDENTITY;
    f32 r_sub_l = right - left;
    f32 t_sub_b = top   - bottom;
    f32 f_sub_n = far_  - near_;

    result.c[ 0] =  2.0f / r_sub_l;
    result.c[ 5] =  2.0f / t_sub_b;
    result.c[10] = -2.0f / f_sub_n;
    result.c[12] = -( right + left   ) / r_sub_l;
    result.c[13] = -( top   + bottom ) / t_sub_b;
    result.c[14] = -( far_  + near_  ) / f_sub_n;
    return result;
}
/// create new orthographic projection matrix
SM_ALWAYS_INLINE mat4 ortho(
    f32 left, f32 right,
    f32 bottom, f32 top
) {
    return ortho( left, right, bottom, top, -1.0f, 1.0f );
}
/// create orthographic matrix for 2D rendering
SM_ALWAYS_INLINE mat4 projection2d( f32 aspect_ratio, f32 scale ) {
    f32 scaled_aspect = aspect_ratio * scale;
    return ortho(
        -scaled_aspect, scaled_aspect,
        -scale, scale
    );
}
/// create perspective matrix
SM_ALWAYS_INLINE mat4 perspective(
    f32 fov, f32 aspect_ratio,
    f32 near_, f32 far_
) {
    mat4 result = {};
    
    f32 half_fov_tan = tan( fov / 2.0f );
    f32 f_sub_n      = far_ - near_;

    result.c[ 0] = 1.0f / ( aspect_ratio * half_fov_tan );
    result.c[ 5] = 1.0f / half_fov_tan;
    result.c[10] = -( ( far_ + near_ ) / f_sub_n );
    result.c[11] = -1.0f;
    result.c[14] = -( ( 2.0f * far_ * near_ ) / f_sub_n );

    return result;
}
/// create new translation matrix
SM_ALWAYS_INLINE mat4 translate( f32 x, f32 y, f32 z ) {
    return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f,
    };
}
/// create new translation matrix
SM_ALWAYS_INLINE mat4 translate( vec3 translation ) {
    return translate(
        translation.x,
        translation.y,
        translation.z
    );
}
/// create new 2d translation matrix
SM_ALWAYS_INLINE mat4 translate( f32 x, f32 y ) {
    return translate( x, y, 0.0f );
}
/// create new 2d translation matrix
SM_ALWAYS_INLINE mat4 translate( vec2 translation ) {
    return translate( translation.x, translation.y );
}
/// create rotation matrix around x axis
SM_ALWAYS_INLINE mat4 rotate_pitch( f32 pitch ) {
    f32 cosine = cos( pitch );
    f32 sine   = sin( pitch );

    return {
        1.0f,   0.0f,   0.0f, 0.0f,
        0.0f, cosine,   sine, 0.0f,
        0.0f,  -sine, cosine, 0.0f,
        0.0f,   0.0f,   0.0f, 1.0f
    };
}
/// create rotation matrix around y axis
SM_ALWAYS_INLINE mat4 rotate_yaw( f32 yaw ) {
    f32 cosine = cos( yaw );
    f32 sine   = sin( yaw );

    return {
        cosine, 0.0f,  -sine, 0.0f,
          0.0f, 1.0f,   0.0f, 0.0f,
          sine, 0.0f, cosine, 0.0f,
          0.0f, 0.0f,   0.0f, 1.0f
    };
}
/// create rotation matrix around z axis
SM_ALWAYS_INLINE mat4 rotate_roll( f32 roll ) {
    f32 cosine = cos( roll );
    f32 sine   = sin( roll );

    return {
        cosine,   sine, 0.0f, 0.0f,
         -sine, cosine, 0.0f, 0.0f,
          0.0f,   0.0f, 1.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 1.0f
    };
}
/// create rotation matrix from euler angles (radians)
SM_ALWAYS_INLINE mat4 rotate( euler_angles r ) {
    return rotate_pitch( r.pitch ) *
        rotate_yaw( r.yaw ) *
        rotate_roll( r.roll );
}
/// create rotation matrix from quaternion
SM_ALWAYS_INLINE mat4 rotate( quat q ) {
    // TODO(alicia): SIMD?
    mat4 result = MAT4::IDENTITY;

    f32 _2x2 = 2.0f * (q.x * q.x);
    f32 _2y2 = 2.0f * (q.y * q.y);
    f32 _2z2 = 2.0f * (q.z * q.z);

    f32 _2xy = 2.0f * (q.x * q.y);
    f32 _2xz = 2.0f * (q.x * q.z);
    f32 _2yz = 2.0f * (q.y * q.z);
    f32 _2wx = 2.0f * (q.w * q.x);
    f32 _2wy = 2.0f * (q.w * q.y);
    f32 _2wz = 2.0f * (q.w * q.z);

    result.c[0] = 1.0f - _2y2 - _2z2;
    result.c[1] = _2xy + _2wz;
    result.c[2] = _2xz - _2wy;

    result.c[4] = _2xy - _2wz;
    result.c[5] = 1.0f - _2x2 - _2z2;
    result.c[6] = _2yz + _2wx;

    result.c[8]  = _2xz + _2wy;
    result.c[9]  = _2yz - _2wx;
    result.c[10] = 1.0f - _2x2 - _2y2;

    return result;
}
/// create rotation matrix for 2D
SM_ALWAYS_INLINE mat4 rotate( f32 theta ) {
    return rotate_roll( theta );
}
/// create scale matrix
SM_ALWAYS_INLINE mat4 scale( f32 width, f32 height, f32 length ) {
    return {
        width,   0.0f,   0.0f, 0.0f,
         0.0f, height,   0.0f, 0.0f,
         0.0f,   0.0f, length, 0.0f,
         0.0f,   0.0f,   0.0f, 1.0f
    };
}
/// create scale matrix
SM_ALWAYS_INLINE mat4 scale( vec3 scale ) {
    return ::scale( scale.width, scale.height, scale.length );
}
/// create scale matrix for 2D
SM_ALWAYS_INLINE mat4 scale( f32 width, f32 height ) {
    return scale( width, height, 1.0f );
}
/// create scale matrix for 2D
SM_ALWAYS_INLINE mat4 scale( vec2 scale ) {
    return ::scale( scale.width, scale.height );
}
/// create transform matrix
SM_ALWAYS_INLINE mat4 transform(
    vec3         translation,
    euler_angles rotation,
    vec3         scale
) {
    return translate( translation ) *
        rotate( rotation ) *
        ::scale( scale );
}
/// create transform matrix
SM_ALWAYS_INLINE mat4 transform(
    vec3 translation,
    quat rotation,
    vec3 scale
) {
    return translate( translation ) *
        rotate( rotation ) *
        ::scale( scale );
}
/// create transform matrix for 2D
SM_ALWAYS_INLINE mat4 transform(
    vec2 translation,
    f32  rotation,
    vec2 scale
) {
    return translate( translation ) *
        rotate( rotation ) *
        ::scale( scale );
}
/// transpose matrix
SM_ALWAYS_INLINE mat4 transpose( const mat4& m ) {
    return {
        m.c[0], m.c[4], m.c[ 8], m.c[12],
        m.c[1], m.c[5], m.c[ 9], m.c[13],
        m.c[2], m.c[6], m.c[10], m.c[14],
        m.c[3], m.c[7], m.c[11], m.c[15]
    };
}
/// get submatrix at given coordinates
SM_ALWAYS_INLINE mat3 submatrix( const mat4& m, u32 row, u32 col ) {
    mat3 result;
    u32 i = 0;
    for( u32 c = 0; c < 4; ++c ) {
        if( c != col ) {
            for( u32 r = 0; r < 4; ++r ) {
                if( r != row ) {
                    result.c[i++] = m[c][r];
                }
            }
        }
    }
    return result;
}
/// calculate determinant of submatrix at given coordinates
SM_ALWAYS_INLINE f32 minor( const mat4& m, u32 row, u32 col ) {
    return determinant( submatrix( m, row, col ) );
}
/// calculate the cofactor of minor at given coordinates
SM_ALWAYS_INLINE f32 cofactor( const mat4& m, u32 row, u32 col ) {
    f32 minor = ::minor( m, row, col );
    return minor * powi( -1.0f, ( row + 1 ) + ( col + 1 ) );
}
/// construct matrix from cofactors
SM_ALWAYS_INLINE mat4 cofactor_matrix( const mat4& m ) {
    return {
        cofactor( m, 0, 0 ),
            cofactor( m, 0, 1 ),
            cofactor( m, 0, 2 ),
            cofactor( m, 0, 3 ),
        cofactor( m, 1, 0 ),
            cofactor( m, 1, 1 ),
            cofactor( m, 1, 2 ),
            cofactor( m, 1, 3 ),
        cofactor( m, 2, 0 ),
            cofactor( m, 2, 1 ),
            cofactor( m, 2, 2 ),
            cofactor( m, 2, 3 ),
        cofactor( m, 3, 0 ),
            cofactor( m, 3, 1 ),
            cofactor( m, 3, 2 ),
            cofactor( m, 3, 3 ),
    };
}
/// calculate the adjoint matrix of given matrix
SM_ALWAYS_INLINE mat4 adjoint( const mat4& m ) {
    return transpose( cofactor_matrix( m ) );
}
/// calculate the determinant
SM_ALWAYS_INLINE f32 determinant( const mat4& m ) {
    return
        ( m.c[ 0] * determinant( submatrix( m, 0, 0 ) ) ) -
        ( m.c[ 4] * determinant( submatrix( m, 0, 1 ) ) ) +
        ( m.c[ 8] * determinant( submatrix( m, 0, 2 ) ) ) -
        ( m.c[12] * determinant( submatrix( m, 0, 3 ) ) );
}
/// attempt to invert matrix
/// @out_matrix pointer to result,
/// set to identity if inversion is not possible
/// @return true if inversion is possible
SM_ALWAYS_INLINE b32 inverse( const mat4& m, mat4* out_matrix ) {
    f32 det = determinant( m );
    if( det == 0.0f ) {
        *out_matrix = MAT4::IDENTITY;
        return false;
    } else {
        *out_matrix = adjoint( m ) / det;
        return true;
    }
}
/// invert matrix whether determinant is 0 or not
SM_ALWAYS_INLINE mat4 inverse_unchecked( const mat4& m ) {
    return adjoint( m ) / determinant( m );
}

/// random number generator.
/// linear congruential generator algorithm
struct rand_lcg {
    i32 seed;
    i32 current;
    i32 a, b, m;

    rand_lcg() :
        seed( 34634623 ),
        current( seed ),
        a( 166463 ),
        b( 235 ),
        m( 21446837 )
    {}
    explicit rand_lcg( i32 seed ) :
        seed( seed ),
        current( seed ),
        a( 166463 ),
        b( 235 ),
        m( 21446837 )
    {}

    /// random signed int in I32::MIN to I32::MAX range
    i32 next_i32() {
        current = ( a * -current + b ) % m;
        return current;
    }
    /// random unsigned int in 0-U32::MAX range
    u32 next_u32() {
        i32 next = next_i32();
        return *(u32*)&next;
    }
    /// random float in -1 to 1 range
    f32 next_f32() {
        return normalize_range( next_i32() );
    }
    /// random float in 0 to 1 range
    f32 next_f3201() {
        return (next_f32() + 1.0f) / 2.0f;
    }
    /// random normalized vec2
    vec2 next_v2() {
        vec2 result = v2( next_f32(), next_f32() );
        return normalize( result );
    }
    /// random ivec2
    ivec2 next_iv2() {
        return { next_i32(), next_i32() };
    }
    /// random uvec2
    uvec2 next_uv2() {
        return { next_u32(), next_u32() };
    }
    /// random normalized vec3
    vec3 next_v3() {
        vec3 result = v3( next_f32(), next_f32(), next_f32() );
        return normalize( result );
    }
    /// random ivec3
    ivec3 next_iv3() {
        return { next_i32(), next_i32(), next_i32() };
    }
    /// random uvec3
    uvec3 next_uv3() {
        return { next_u32(), next_u32(), next_u32() };
    }
    /// random rgb
    rgb next_rgb() {
        return { next_f3201(), next_f3201(), next_f3201() };
    }
    /// random normalized vec4
    vec4 next_v4() {
        vec4 result = v4( next_f32(), next_f32(), next_f32(), next_f32() );
        return normalize( result );
    }
    /// random ivec4
    ivec4 next_iv4() {
        return { next_i32(), next_i32(), next_i32(), next_i32() };
    }
    /// random uvec4
    uvec4 next_uv4() {
        return { next_u32(), next_u32(), next_u32(), next_u32() };
    }
    /// random rgba, with alpha set to 1
    rgba next_rgba() {
        return { next_f3201(), next_f3201(), next_f3201(), 1.0f };
    }
    /// random quaternion
    /// TODO(alicia): make sure this provides reasonable rotations
    quat next_quat() {
        vec4 v = next_v4();
        quat result = *(quat*)&v;
        return normalize( result );
    }
};

/// random number generator.
/// xor shift algorithm
struct rand_xor {
    u32 seed;
    u32 current;

    rand_xor()
    : seed( 34634623 ), current( seed ) {}

    explicit rand_xor( u32 seed )
    : seed( max( seed, 1u ) ) {
        current = this->seed;
    }

    /// random unsigned integer in range 0 to U32::MAX
    u32 next_u32() {
        u32 result = current;
        result ^= result << 13;
        result ^= result >> 17;
        result ^= result << 5;
        current = result;
        return result;
    }
    /// random signed integer in range I32::MIN to I32::MAX
    i32 next_i32() {
        i32 result = next_u32();
        return *(i32*)&result;
    }
    /// random float in -1 to 1 range
    f32 next_f32() {
        return normalize_range( next_i32() );
    }
    /// random float in 0 to 1 range
    f32 next_f3201() {
        return (next_f32() + 1.0f) / 2.0f;
    }
    /// random normalized vec2
    vec2 next_v2() {
        vec2 result = v2( next_f32(), next_f32() );
        return normalize( result );
    }
    /// random ivec2
    ivec2 next_iv2() {
        return { next_i32(), next_i32() };
    }
    /// random uvec2
    uvec2 next_uv2() {
        return { next_u32(), next_u32() };
    }
    /// random normalized vec3
    vec3 next_v3() {
        vec3 result = v3( next_f32(), next_f32(), next_f32() );
        return normalize( result );
    }
    /// random rgb
    rgb next_rgb() {
        return { next_f3201(), next_f3201(), next_f3201() };
    }
    /// random ivec3
    ivec3 next_iv3() {
        return { next_i32(), next_i32(), next_i32() };
    }
    /// random uvec3
    uvec3 next_uv3() {
        return { next_u32(), next_u32(), next_u32() };
    }
    /// random normalized vec4
    vec4 next_v4() {
        vec4 result = v4( next_f32(), next_f32(), next_f32(), next_f32() );
        return normalize( result );
    }
    /// random ivec4
    ivec4 next_iv4() {
        return { next_i32(), next_i32(), next_i32(), next_i32() };
    }
    /// random uvec4
    uvec4 next_uv4() {
        return { next_u32(), next_u32(), next_u32(), next_u32() };
    }
    /// random rgba with alpha set to 1
    rgba next_rgba() {
        return { next_f3201(), next_f3201(), next_f3201(), 1.0f };
    }
    /// random quaternion
    /// TODO(alicia): make sure this provides reasonable rotations
    quat next_quat() {
        vec4 v = next_v4();
        quat result = *(quat*)&v;
        return normalize( result );
    }
};

// CONVERSIONS --------------------------------------------------

/// convert rgb to rgba
SM_ALWAYS_INLINE rgba to_rgba( rgb rgb ) {
    rgba result;
    result.rgb = rgb;
    result.a   = 1.0f;
    return result;
}

/// convert quaternion to euler angles
SM_ALWAYS_INLINE euler_angles to_euler( quat q ) {
    return {
        atan2(
            2.0f * (( q.w * q.x ) + ( q.y * q.z )),
            1.0f - 2.0f * ( ( q.x * q.x ) + ( q.y * q.y ) )
        ),
        asin_real( 2.0f * (( q.w * q.y ) - ( q.z * q.x )) ),
        atan2(
            2.0f * (( q.w * q.z ) + ( q.x * q.y )),
            1.0f - 2.0f * ( ( q.y * q.y ) + ( q.z * q.z ) )
        ),
    };
}
/// convert quaternion to angle axis
SM_ALWAYS_INLINE angle_axis to_angle_axis( quat q ) {
    f32 inv_w2_sqrt = sqrt( 1.0f - ( q.w * q.w ) );
    return {
        q.xyz / inv_w2_sqrt,
        2.0f * acos( q.w )
    };
}

SM_ALWAYS_INLINE vec2 v2( vec3 v ) { return v.xy; }
SM_ALWAYS_INLINE vec2 v2( vec4 v ) { return v.xy; }
SM_ALWAYS_INLINE vec2 v2( ivec2 v ) {
    return { (f32)v.x, (f32)v.y };
}
SM_ALWAYS_INLINE vec2 v2( uvec2 v ) {
    return { (f32)v.x, (f32)v.y };
}

SM_ALWAYS_INLINE ivec2 trunc32( vec2 v ) {
    return { trunc32(v.x), trunc32(v.y) };
}
SM_ALWAYS_INLINE ivec2 floor32( vec2 v ) {
    return { floor32(v.x), floor32(v.y) };
}
SM_ALWAYS_INLINE ivec2 ceil32( vec2 v ) {
    return { ceil32(v.x), ceil32(v.y) };
}
SM_ALWAYS_INLINE ivec2 round32( vec2 v ) {
    return { round32(v.x), round32(v.y) };
}

SM_ALWAYS_INLINE ivec2 iv2( ivec3 v ) { return v.xy; }
SM_ALWAYS_INLINE ivec2 iv2( ivec4 v ) { return v.xy; }
SM_ALWAYS_INLINE ivec2 iv2( vec2 v )  { return trunc32( v ); }
SM_ALWAYS_INLINE ivec2 iv2( uvec2 v ) {
    return { (i32)v.x, (i32)v.y };
}

SM_ALWAYS_INLINE uvec2 uv2( uvec3 v ) { return v.xy; }
SM_ALWAYS_INLINE uvec2 uv2( uvec4 v ) { return v.xy; }
SM_ALWAYS_INLINE uvec2 uv2( ivec2 v ) { return { (u32)v.x, (u32)v.y }; }
SM_ALWAYS_INLINE uvec2 uv2( vec2 v )  { return uv2(trunc32( v )); }

SM_ALWAYS_INLINE vec3 v3( vec2 v ) { return { v.x, v.y, 0.0f }; }
SM_ALWAYS_INLINE vec3 v3( vec4 v ) { return v.xyz; }
SM_ALWAYS_INLINE vec3 v3( ivec3 v ) {
    return { (f32)v.x, (f32)v.y, (f32)v.z };
}
SM_ALWAYS_INLINE vec3 v3( uvec3 v ) {
    return { (f32)v.x, (f32)v.y, (f32)v.z };
}

SM_ALWAYS_INLINE ivec3 trunc32( vec3 v ) {
    return { trunc32(v.x), trunc32(v.y), trunc32(v.z) };
}
SM_ALWAYS_INLINE ivec3 floor32( vec3 v ) {
    return { floor32(v.x), floor32(v.y), floor32(v.z) };
}
SM_ALWAYS_INLINE ivec3 ceil32( vec3 v ) {
    return { ceil32(v.x), ceil32(v.y), ceil32(v.z) };
}
SM_ALWAYS_INLINE ivec3 round32( vec3 v ) {
    return { round32(v.x), round32(v.y), round32(v.z) };
}

SM_ALWAYS_INLINE ivec3 iv3( ivec2 v ) { return { v.x, v.y, 0 }; }
SM_ALWAYS_INLINE ivec3 iv3( ivec4 v ) { return v.xyz; }
SM_ALWAYS_INLINE ivec3 iv3( vec3 v )  { return trunc32( v ); }
SM_ALWAYS_INLINE ivec3 iv3( uvec3 v ) {
    return { (i32)v.x, (i32)v.y, (i32)v.z };
}

SM_ALWAYS_INLINE uvec3 uv3( uvec2 v ) { return { v.x, v.y, 0 }; }
SM_ALWAYS_INLINE uvec3 uv3( uvec4 v ) { return v.xyz; }
SM_ALWAYS_INLINE uvec3 uv3( ivec3 v ) { return { (u32)v.x, (u32)v.y, (u32)v.z }; }
SM_ALWAYS_INLINE uvec3 uv3( vec3 v )  { return uv3(trunc32( v )); }

SM_ALWAYS_INLINE vec4 v4( vec2 v ) { return { v.x, v.y, 0.0f, 0.0f }; }
SM_ALWAYS_INLINE vec4 v4( vec3 v ) { return { v.x, v.y,  v.z, 0.0f }; }
SM_ALWAYS_INLINE vec4 v4( ivec4 v ) {
    return { (f32)v.x, (f32)v.y, (f32)v.z, (f32)v.w };
}
SM_ALWAYS_INLINE vec4 v4( uvec4 v ) {
    return { (f32)v.x, (f32)v.y, (f32)v.z, (f32)v.w };
}

SM_ALWAYS_INLINE ivec4 trunc32( vec4 v ) {
    return { trunc32(v.x), trunc32(v.y), trunc32(v.z), trunc32(v.w) };
}
SM_ALWAYS_INLINE ivec4 floor32( vec4 v ) {
    return { floor32(v.x), floor32(v.y), floor32(v.z), floor32(v.w) };
}
SM_ALWAYS_INLINE ivec4 ceil32( vec4 v ) {
    return { ceil32(v.x), ceil32(v.y), ceil32(v.z), ceil32(v.w) };
}
SM_ALWAYS_INLINE ivec4 round32( vec4 v ) {
    return { round32(v.x), round32(v.y), round32(v.z), round32(v.w) };
}

SM_ALWAYS_INLINE ivec4 iv4( ivec2 v ) { return { v.x, v.y,   0, 0 }; }
SM_ALWAYS_INLINE ivec4 iv4( ivec3 v ) { return { v.x, v.y, v.z, 0 }; }
SM_ALWAYS_INLINE ivec4 iv4( vec4 v )  { return trunc32( v ); }
SM_ALWAYS_INLINE ivec4 iv4( uvec4 v ) {
    return { (i32)v.x, (i32)v.y, (i32)v.z, (i32)v.w };
}

SM_ALWAYS_INLINE uvec4 uv4( uvec2 v ) { return { v.x, v.y,   0, 0 }; }
SM_ALWAYS_INLINE uvec4 uv4( uvec3 v ) { return { v.x, v.y, v.z, 0 }; }
SM_ALWAYS_INLINE uvec4 uv4( ivec4 v ) {
    return { (u32)v.x, (u32)v.y, (u32)v.z, (u32)v.w };
}
SM_ALWAYS_INLINE uvec4 uv4( vec4 v )  { return uv4(trunc32( v )); }

SM_ALWAYS_INLINE mat3 m3( mat2 m ) {
    return {
        m.c[0], m.c[1], 0.0f,
        m.c[2], m.c[3], 0.0f,
          0.0f,   0.0f, 0.0f
    };
}
SM_ALWAYS_INLINE mat3 m3( const mat4& m ) {
    return {
        m.c[0], m.c[1], m.c[ 2],
        m.c[4], m.c[5], m.c[ 6],
        m.c[8], m.c[9], m.c[10]
    };
}

SM_ALWAYS_INLINE mat4 m4( mat2 m ) {
    return {
        m.c[0], m.c[1], 0.0f, 0.0f,
        m.c[2], m.c[3], 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
    };
}
SM_ALWAYS_INLINE mat4 m4( const mat3& m ) {
    return {
        m.c[0], m.c[1], m.c[2], 0.0f,
        m.c[3], m.c[4], m.c[5], 0.0f,
        m.c[6], m.c[7], m.c[8], 0.0f,
          0.0f,   0.0f,   0.0f, 0.0f,
    };
}

// VEC2/MAT4 ----------------------------------------------------

/// create look at matrix for 2D rendering
SM_ALWAYS_INLINE mat4 lookat( vec2 position ) {
    return lookat(
        v3(position) + VEC3::FORWARD,
        v3(position),
        VEC3::UP
    );
}

// MAT3/MAT4 ----------------------------------------------------

/// calculate the normal matrix of transform matrix
/// @out_matrix pointer to result,
/// set to identity if not possible
/// @return true if determinant is not 0
SM_ALWAYS_INLINE b32 normal_matrix( const mat4& m, mat3* out_matrix ) {
    mat4 inv;
    if( inverse( m, &inv ) ) {
        *out_matrix = m3( transpose( inv ) );
        return true;
    } else {
        *out_matrix = MAT3::IDENTITY;
        return false;
    }
}
/// calculate the normal matrix of transform matrix
/// whether the determinant is 0 or not
SM_ALWAYS_INLINE mat3 normal_matrix_unchecked( const mat4& m ) {
    return m3( transpose( inverse_unchecked( m ) ) );
}

#if defined(SM_COMPILER_CLANG) && !defined(_CLANGD)
    #pragma clang diagnostic pop
#endif

#endif

