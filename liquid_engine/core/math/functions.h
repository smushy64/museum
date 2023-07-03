#if !defined(CORE_MATH_FUNCTIONS_HPP)
#define CORE_MATH_FUNCTIONS_HPP
/**
 * Description:  Math functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 08, 2023
*/
#include "defines.h"
#include "builtins.h"
#include "simd.h"

/// truncate float to signed integer
inline i64 trunc64( f32 x ) { return (i64)x; }
/// truncate float to signed integer
inline i64 trunc64( f64 x ) { return (i64)x; }
/// truncate float to signed integer
inline i32 trunc32( f32 x ) { return (i32)x; }
/// truncate float to signed integer
inline i32 trunc32( f64 x ) { return (i32)x; }
/// truncate float to signed integer
inline i16 trunc16( f32 x ) { return (i16)x; }
/// truncate float to signed integer
inline i16 trunc16( f64 x ) { return (i16)x; }
/// truncate float to signed integer
inline i8  trunc8( f32 x )  { return (i8)x;  }
/// truncate float to signed integer
inline i8  trunc8( f64 x )  { return (i8)x;  }

/// floor float to signed integer
inline i64 floor64( f64 x ) {
    return x > 0.0 ? trunc64(x) : trunc64(x - 0.99999999);
}
/// floor float to signed integer
inline i64 floor64( f32 x ) {
    return x > 0.0f ? trunc64(x) : trunc64(x - 0.999999f);
}
/// floor float to signed integer
inline i32 floor32( f64 x ) {
    return x > 0.0 ? trunc32(x) : trunc32(x - 0.99999999);
}
/// floor float to signed integer
inline i32 floor32( f32 x ) {
    return x > 0.0f ? trunc32(x) : trunc32(x - 0.999999f);
}
/// floor float to signed integer
inline i16 floor16( f64 x ) {
    return x > 0.0 ? trunc16(x) : trunc16(x - 0.99999999);
}
/// floor float to signed integer
inline i16 floor16( f32 x ) {
    return x > 0.0f ? trunc16(x) : trunc16(x - 0.999999f);
}
/// floor float to signed integer
inline i8 floor8( f64 x ) {
    return x > 0.0 ? trunc8(x) : trunc8(x - 0.99999999);
}
/// floor float to signed integer
inline i8 floor8( f32 x ) {
    return x > 0.0f ? trunc8(x) : trunc8(x - 0.999999f);
}

/// ceil float to signed integer
inline i64 ceil64( f64 x ) {
    return x > 0.0 ? trunc64( x + 0.99999999 ) : trunc64(x);
}
/// ceil float to signed integer
inline i64 ceil64( f32 x ) {
    return x > 0.0f ? trunc64( x + 0.999999f ) : trunc64(x);
}
/// ceil float to signed integer
inline i32 ceil32( f64 x ) {
    return x > 0.0 ? trunc32( x + 0.99999999 ) : trunc32(x);
}
/// ceil float to signed integer
inline i32 ceil32( f32 x ) {
    return x > 0.0f ? trunc32( x + 0.999999f ) : trunc32(x);
}
/// ceil float to signed integer
inline i16 ceil16( f64 x ) {
    return x > 0.0 ? trunc16( x + 0.99999999 ) : trunc16(x);
}
/// ceil float to signed integer
inline i16 ceil16( f32 x ) {
    return x > 0.0f ? trunc16( x + 0.999999f ) : trunc16(x);
}
/// ceil float to signed integer
inline i8 ceil8( f64 x ) {
    return x > 0.0 ? trunc8( x + 0.99999999 ) : trunc8(x);
}
/// ceil float to signed integer
inline i8 ceil8( f32 x ) {
    return x > 0.0f ? trunc8( x + 0.999999f ) : trunc8(x);
}

/// round float to signed integer
inline i64 round64( f64 x ) {
    return x > 0.0 ? trunc64( x + 0.5 ) : trunc64( x - 0.5 );
}
/// round float to signed integer
inline i64 round64( f32 x ) {
    return x > 0.0f ? trunc64( x + 0.5f ) : trunc64( x - 0.5f );
}
/// round float to signed integer
inline i32 round32( f64 x ) {
    return x > 0.0 ? trunc32( x + 0.5 ) : trunc32( x - 0.5 );
}
/// round float to signed integer
inline i32 round32( f32 x ) {
    return x > 0.0f ? trunc32( x + 0.5f ) : trunc32( x - 0.5f );
}
/// round float to signed integer
inline i16 round16( f64 x ) {
    return x > 0.0 ? trunc16( x + 0.5 ) : trunc16( x - 0.5 );
}
/// round float to signed integer
inline i16 round16( f32 x ) {
    return x > 0.0f ? trunc16( x + 0.5f ) : trunc16( x - 0.5f );
}
/// round float to signed integer
inline i8 round8( f64 x ) {
    return x > 0.0 ? trunc8( x + 0.5 ) : trunc8( x - 0.5 );
}
/// round float to signed integer
inline i8 round8( f32 x ) {
    return x > 0.0f ? trunc8( x + 0.5f ) : trunc8( x - 0.5f );
}

/// get the sign of a signed integer
inline i8 sign( i8 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
inline i16 sign( i16 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
inline i32 sign( i32 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
inline i64 sign( i64 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a single-precision float
inline f32 sign( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return ((bitpattern >> 31) & 1) == 1 ? -1.0f : 1.0f;
}
/// get the sign of a double-precision float
inline f64 sign( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return ((bitpattern >> 63) & 1) == 1 ? -1.0 : 1.0;
}

/// get the absolute value of a signed integer
inline i8 absolute( i8 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
inline i16 absolute( i16 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
inline i32 absolute( i32 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
inline i64 absolute( i64 x ) { return x * sign( x ); }
/// get the absolute value of a single-precision float
inline f32 absolute( f32 x ) { return x * sign( x ); }
/// get the absolute value of a double-precision float
inline f64 absolute( f64 x ) { return x * sign( x ); }

/// clamp a signed integer between min and max, inclusive-inclusive
inline i8 clamp( i8 value, i8 min, i8 max ) {
    const i8 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
inline i16 clamp( i16 value, i16 min, i16 max ) {
    const i16 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
inline i32 clamp( i32 value, i32 min, i32 max ) {
    const i32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
inline i64 clamp( i64 value, i64 min, i64 max ) {
    const i64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
inline u8 clamp( u8 value, u8 min, u8 max ) {
    const u8 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
inline u16 clamp( u16 value, u16 min, u16 max ) {
    const u16 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
inline u32 clamp( u32 value, u32 min, u32 max ) {
    const u32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
inline u64 clamp( u64 value, u64 min, u64 max ) {
    const u64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a single-precision float between min and max, inclusive-inclusive
inline f32 clamp( f32 value, f32 min, f32 max ) {
    const f32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a double-precision float between min and max, inclusive-inclusive
inline f64 clamp( f64 value, f64 min, f64 max ) {
    const f64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a single-precision float between 0-1
inline f32 clamp01( f32 value ) { return clamp(value,0.0f,1.0f); }
/// clamp a double-precision float between 0-1
inline f64 clamp01( f64 value ) { return clamp(value,0.0,1.0); }

/// get the smallest of two values
inline i8 min( i8 a, i8 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline i16 min( i16 a, i16 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline i32 min( i32 a, i32 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline i64 min( i64 a, i64 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline u8 min( u8 a, u8 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline u16 min( u16 a, u16 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline u32 min( u32 a, u32 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline u64 min( u64 a, u64 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline f32 min( f32 a, f32 b ) { return a < b ? a : b; }
/// get the smallest of two values
inline f64 min( f64 a, f64 b ) { return a < b ? a : b; }

/// get the largest of two values
inline i8 max( i8 a, i8 b ) { return a < b ? b : a; }
/// get the largest of two values
inline i16 max( i16 a, i16 b ) { return a < b ? b : a; }
/// get the largest of two values
inline i32 max( i32 a, i32 b ) { return a < b ? b : a; }
/// get the largest of two values
inline i64 max( i64 a, i64 b ) { return a < b ? b : a; }
/// get the largest of two values
inline u8 max( u8 a, u8 b ) { return a < b ? b : a; }
/// get the largest of two values
inline u16 max( u16 a, u16 b ) { return a < b ? b : a; }
/// get the largest of two values
inline u32 max( u32 a, u32 b ) { return a < b ? b : a; }
/// get the largest of two values
inline u64 max( u64 a, u64 b ) { return a < b ? b : a; }
/// get the largest of two values
inline f32 max( f32 a, f32 b ) { return a < b ? b : a; }
/// get the largest of two values
inline f64 max( f64 a, f64 b ) { return a < b ? b : a; }

/// normalize integer -1 to 1 range
inline f32 normalize_range( i8 x ) {
    return (f32)x / (x > 0 ? (f32)I8::MAX : -((f32)I8::MIN));
}
/// normalize integer -1 to 1 range
inline f32 normalize_range( i16 x ) {
    return (f32)x / (x > 0 ? (f32)I16::MAX : -((f32)I16::MIN));
}
/// normalize integer -1 to 1 range
inline f32 normalize_range( i32 x ) {
    return (f32)x / (x > 0 ? (f32)I32::MAX : -((f32)I32::MIN));
}
/// normalize integer -1 to 1 range
inline f32 normalize_range( i64 x ) {
    return (f32)x / (x > 0 ? (f32)I64::MAX : -((f32)I64::MIN));
}
/// normalize integer 0 to 1 range
inline f32 normalize_range( u8 x ) {
    return (f32)x / (f32)U8::MAX;
}
/// normalize integer 0 to 1 range
inline f32 normalize_range( u16 x ) {
    return (f32)x / (f32)U16::MAX;
}
/// normalize integer 0 to 1 range
inline f32 normalize_range( u32 x ) {
    return (f32)x / (f32)U32::MAX;
}
/// normalize integer 0 to 1 range
inline f32 normalize_range( u64 x ) {
    return (f32)x / (f32)U64::MAX;
}
/// normalize integer -1 to 1 range
inline f64 normalize_range64( i8 x ) {
    return (f64)x / (x > 0 ? (f64)I8::MAX : -((f64)I8::MIN));
}
/// normalize integer -1 to 1 range
inline f64 normalize_range64( i16 x ) {
    return (f64)x / (x > 0 ? (f64)I16::MAX : -((f64)I16::MIN));
}
/// normalize integer -1 to 1 range
inline f64 normalize_range64( i32 x ) {
    return (f64)x / (x > 0 ? (f64)I32::MAX : -((f64)I32::MIN));
}
/// normalize integer -1 to 1 range
inline f64 normalize_range64( i64 x ) {
    return (f64)x / (x > 0 ? (f64)I64::MAX : -((f64)I64::MIN));
}
/// normalize integer 0 to 1 range
inline f64 normalize_range64( u8 x ) {
    return (f64)x / (f64)U8::MAX;
}
/// normalize integer 0 to 1 range
inline f64 normalize_range64( u16 x ) {
    return (f64)x / (f64)U16::MAX;
}
/// normalize integer 0 to 1 range
inline f64 normalize_range64( u32 x ) {
    return (f64)x / (f64)U32::MAX;
}
/// normalize integer 0 to 1 range
inline f64 normalize_range64( u64 x ) {
    return (f64)x / (f64)U64::MAX;
}

/// normalize float to i8 min-max.
/// float must be in -1.0-1.0 range
inline i8 normalize_rangei8( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i8)( x_abs * (f32)I8::MAX ) * (i8)x_sign;
}
/// normalize float to i16 min-max.
/// float must be in -1.0-1.0 range
inline i16 normalize_rangei16( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i16)( x_abs * (f32)I16::MAX ) * (i16)x_sign;
}
/// normalize float to i32 min-max.
/// float must be in -1.0-1.0 range
inline i32 normalize_rangei32( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i32)( x_abs * (f32)I32::MAX ) * (i32)x_sign;
}
/// normalize float to i64 min-max.
/// float must be in -1.0-1.0 range
inline i64 normalize_rangei64( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i64)( x_abs * (f32)I64::MAX ) * (i64)x_sign;
}

/// square root
FORCE_INLINE f32 sqrt( f32 x ) {
    return lane1f_sqrt( x );
}
/// square root
FORCE_INLINE f64 sqrt( f64 x ) {
#if defined(LD_ARCH_X86)
    f64 result;
    __asm__ inline (
        "fld qword ptr [%1]\n\t"
        "fsqrt\n\t"
        "fstp qword ptr %0"
        : "=m" (result)
        : "r" (&x)
        : "cc"
    );
    return result;
#else
    return impl::_sqrt_(x);
#endif
}

/// raise to the power, integer exponent
inline f32 powi( f32 base, i32 exponent ) {
    return impl::_powif_( base, exponent );
}
/// raise to the power, integer exponent
inline f64 powi( f64 base, i32 exponent ) {
    return impl::_powi_( base, exponent );
}
/// raise to the power, float exponent
inline f32 pow( f32 base, f32 exponent ) {
    return impl::_powf_( base, exponent );
}
/// raise to the power, float exponent
inline f64 pow( f64 base, f64 exponent ) {
    return impl::_pow_( base, exponent );
}

/// float modulus
inline f32 mod( f32 lhs, f32 rhs ) {
    return impl::_modf_( lhs, rhs );
}
/// float modulus
inline f64 mod( f64 lhs, f64 rhs ) {
    return impl::_mod_( lhs, rhs );
}

/// check if single-precision is not a number
inline b32 is_nan( f32 x ) {
    u32 bitpattern = *(u32*)&x;

    u32 exp = bitpattern & F32::EXPONENT_MASK;
    u32 man = bitpattern & F32::MANTISSA_MASK;

    return exp == F32::EXPONENT_MASK && man != 0;
}
/// check if single-precision float is zero
inline b32 is_zero( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return (bitpattern & 0x7FFFFFFF) == 0 || (bitpattern == 0x80000000);
}
/// check if double-precision float is not a number
inline b32 is_nan( f64 x ) {
    u64 bitpattern = *(u64*)&x;

    u64 exp = bitpattern & F64::EXPONENT_MASK;
    u64 man = bitpattern & F64::MANTISSA_MASK;

    return exp == F64::EXPONENT_MASK && man != 0;
}
/// check if double-precision float is zero
inline b32 is_zero( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return (bitpattern & 0x7FFFFFFFFFFFFFFF) == 0 || (bitpattern == 0x8000000000000000);
}

/// sine function
FORCE_INLINE f32 sin( f32 x ) {
#if defined(LD_ARCH_X86)
    f32 result;
    __asm__ inline (
        "fld dword ptr [%1]\n\t"
        "fsin\n\t"
        "fstp dword ptr %0"
        : "=m" (result)
        : "r" (&x)
        : "cc"
    );
    return result;
#else
    return impl::_sinf_(x);
#endif
}
/// sine function
FORCE_INLINE f64 sin( f64 x ) {
#if defined(LD_ARCH_X86)
    f64 result;
    __asm__ inline (
        "fld qword ptr [%1]\n\t"
        "fsin\n\t"
        "fstp qword ptr %0"
        : "=m" (result)
        : "r" (&x)
        : "cc"
    );
    return result;
#else
    return impl::_sin_(x);
#endif
}
/// arc-sine function
inline f32 asin( f32 x ) {
    return impl::_asinf_( x );
}
/// arc-sine function
inline f64 asin( f64 x ) {
    return impl::_asin_( x );
}

/// arc-sine function, does not return NAN
inline f32 asin_real( f32 x ) {
    return absolute(x) >= 1.0f ? F32::HALF_PI * sign(x) : asin(x);
}
/// arc-sine function, does not return NAN
inline f64 asin_real( f64 x ) {
    return absolute(x) >= 1.0 ? F64::HALF_PI * sign(x) : asin(x);
}

/// cosine function
FORCE_INLINE f32 cos( f32 x ) {
#if defined(LD_ARCH_X86)
    f32 result;
    __asm__ inline (
        "fld dword ptr [%1]\n\t"
        "fcos\n\t"
        "fstp dword ptr %0"
        : "=m" (result)
        : "r" (&x)
        : "cc"
    );
    return result;
#else
    return impl::_cosf_(x);
#endif
}
/// cosine function
FORCE_INLINE f64 cos( f64 x ) {
#if defined(LD_ARCH_X86)
    f64 result;
    __asm__ inline (
        "fld qword ptr [%1]\n\t"
        "fcos\n\t"
        "fstp qword ptr %0"
        : "=m" (result)
        : "r" (&x)
        : "cc"
    );
    return result;
#else
    return impl::_cos_(x);
#endif
}
/// arc-cosine function
inline f32 acos( f32 x ) {
    return impl::_acosf_( x );
}
/// arc-cosine function
inline f64 acos( f64 x ) {
    return impl::_acos_( x );
}

// sin-cos function
FORCE_INLINE tuplef32 sincos( f32 x ) {
    tuplef32 result;
#if defined(LD_ARCH_X86)
    __asm__ inline (
        "fld dword ptr [%2]\n\t"
        "fsincos\n\t"
        "fstp dword ptr %0\n\t"
        "fstp dword ptr %1\n\t"
        : "=m" (result.f[1]), "=m" (result.f[0])
        : "r" (&x)
        : "cc"
    );
#else
    result.f[0] = sin(x);
    result.f[1] = cos(x);
#endif
    return result;
}
// sin-cos function
FORCE_INLINE tuplef64 sincos( f64 x ) {
    tuplef64 result;
#if defined(LD_ARCH_X86)
    __asm__ inline (
        "fld qword ptr [%2]\n\t"
        "fsincos\n\t"
        "fstp qword ptr %0\n\t"
        "fstp qword ptr %1\n\t"
        : "=m" (result.f[1]), "=m" (result.f[0])
        : "r" (&x)
        : "cc"
    );
#else
    result.f[0] = sin(x);
    result.f[1] = cos(x);
#endif
    return result;
}

/// tangent function
inline f32 tan( f32 x ) {
    // TODO(alicia): FPTAN?
    tuplef32 sc = sincos( x );
    return is_zero(sc.f1) ? F32::NAN : sc.f0 / sc.f1;
}
/// tangent function
inline f64 tan( f64 x ) {
    tuplef64 sc = sincos( x );
    return is_zero(sc.f1) ? F64::NAN : sc.f0 / sc.f1;
}
/// arc-tangent function
inline f32 atan( f32 x ) {
    return impl::_atanf_( x );
}
/// arc-tangent function
inline f64 atan( f64 x ) {
    return impl::_atan_( x );
}

/// two argument arc-tangent function
inline f32 atan2( f32 y, f32 x ) {
    return impl::_atan2f_( y, x );
}
/// two argument arc-tangent function
inline f64 atan2( f64 y, f64 x ) {
    return impl::_atan2_( y, x );
}

/// convert degrees to radians
inline f32 to_rad( f32 theta ) {
    return theta * ( F32::PI / 180.0f );
}
/// convert degrees to radians
inline f64 to_rad( f64 theta ) {
    return theta * ( F64::PI / 180.0 );
}
/// convert radians to degrees 
inline f32 to_deg( f32 theta ) {
    return theta * ( 180.0f / F32::PI );
}
/// convert radians to degrees 
inline f64 to_deg( f64 theta ) {
    return theta * ( 180.0 / F64::PI );
}

/// keep degrees in 0.0-360 range
inline f32 degrees_overflow( f32 deg ) {
    f32 result = mod( deg, 360.0f );
    if( result < 0.0f ) {
        result += 360.0f;
    }
    return result;
}

/// natural logarithm
inline f32 log( f32 x ) {
    return impl::_logf_( x );
}
/// natural logarithm
inline f64 log( f64 x ) {
    return impl::_log_( x );
}

/// linear interpolation
inline f32 lerp( f32 a, f32 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation
inline f64 lerp( f64 a, f64 b, f64 t ) {
    return ( 1.0 - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
inline f32 lerp_clamped( f32 a, f32 b, f32 t ) {
    return lerp( a, b, clamp01(t) );
}
/// linear interpolation, t clamped to 0-1
inline f64 lerp_clamped( f64 a, f64 b, f64 t ) {
    return lerp( a, b, clamp01(t) );
}
/// inverse linear interpolation
inline f32 inverse_lerp( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}
/// inverse linear interpolation
inline f64 inverse_lerp( f64 a, f64 b, f64 v ) {
    return ( v - a ) / ( b - a );
}
/// remap value from input min/max to output min/max
inline f32 remap( f32 imin, f32 imax, f32 omin, f32 omax, f32 v ) {
    const f32 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}
/// remap value from input min/max to output min/max
inline f64 remap( f64 imin, f64 imax, f64 omin, f64 omax, f64 v ) {
    const f64 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}

/// smooth step interpolation
inline f32 smooth_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation
inline f64 smooth_step( f64 a, f64 b, f64 t ) {
    return ( b - a ) * ( 3.0 - t * 2.0 ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
inline f32 smooth_step_clamped( f32 a, f32 b, f32 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smooth step interpolation, t clamped to 0-1
inline f64 smooth_step_clamped( f64 a, f64 b, f64 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smoother step interpolation
inline f32 smoother_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation
inline f64 smoother_step( f64 a, f64 b, f64 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0 - 15.0 ) + 10.0 ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
inline f32 smoother_step_clamped( f32 a, f32 b, f32 t ) {
    return smoother_step( a, b, clamp01(t) );
}
/// smoother step interpolation, t clamped to 0-1
inline f64 smoother_step_clamped( f64 a, f64 b, f64 t ) {
    return smoother_step( a, b, clamp01(t) );
}

#endif // header guard
