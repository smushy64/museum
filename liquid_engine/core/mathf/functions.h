#if !defined(LD_CORE_MATH_FUNCTIONS_H)
#define LD_CORE_MATH_FUNCTIONS_H
// * Description:  Math functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 05, 2023
#include "defines.h"
#include "constants.h"

/// Sign of number.
#define signum( x ) (( (x) > 0 ) - ( (x) < 0 ))
/// Absolute value of number.
#define absolute( x ) ((x) * signum(x)) 

/// Smallest of two values
#define min( a, b ) ( (a) < (b) ? (a) : (b) )

/// Largest of two values
#define max( a, b ) ( (a) < (b) ? (b) : (a) )

/// Clamp a number between min and max, inclusive-inclusive
#define clamp( value, min, max ) \
    (\
        ((value) < (min) ? (min) : (value)) > (max) ?\
        (max) :\
        ((value) < (min) ? (min) : (value))\
    )
/// Clamp a 32-bit float to 0-1
#define clamp32_01( value )\
    clamp( (value), 0.0f, 1.0f )
/// Clamp a 64-bit float to 0-1
#define clamp64_01( value )\
    clamp( (value), 0.0, 1.0 )

/// Convert radians to degrees.
#define to_rad32( theta )\
    ( theta * F32_TO_RAD )
/// Convert degrees to radians.
#define to_deg32( theta )\
    ( theta * F32_TO_DEG )
/// Convert radians to degrees.
#define to_rad64( theta )\
    ( theta * F64_TO_RAD )
/// Convert degrees to radians.
#define to_deg64( theta )\
    ( theta * F64_TO_DEG )

/// Check if float is NaN.
LD_API b32 is_nan32( f32 x );
/// Check if float is NaN.
LD_API b32 is_nan64( f64 x );

/// Square root of x.
LD_API f32 sqrt32( f32 x );
/// Square root of x.
LD_API f64 sqrt64( f64 x );

/// Reciprical of square root of x.
LD_API f32 rsqrt32( f32 x );
/// Reciprical of square root of x.
LD_API f64 rsqrt64( f64 x );

/// Raise base to integer exponent.
LD_API f32 powi32( f32 base, i32 exp );
/// Raise base to integer exponent.
LD_API f64 powi64( f64 base, i64 exp );

/// Raise base to float exponent.
LD_API f32 pow32( f32 base, f32 exp );
/// Raise base to float exponent.
LD_API f64 pow64( f64 base, f64 exp );

/// Float modulus.
LD_API f32 mod32( f32 lhs, f32 rhs );
/// Float modulus.
LD_API f64 mod64( f64 lhs, f64 rhs );

/// Sine of x.
LD_API f32 sin32( f32 x );
/// Sine of x.
LD_API f64 sin64( f64 x );
/// Cosine of x.
LD_API f32 cos32( f32 x );
/// Cosine of x.
LD_API f64 cos64( f64 x );
/// Tangent of x.
LD_API f32 tan32( f32 x );
/// Tangent of x.
LD_API f64 tan64( f64 x );

/// Sine and Cos of x.
/// If you need both values,
/// this is faster than calling each function individually.
LD_API void sincos32( f32 x, f32* out_sin, f32* out_cos );
/// Sine and Cos of x.
/// If you need both values,
/// this is faster than calling each function individually.
LD_API void sincos64( f64 x, f64* out_sin, f64* out_cos );

/// Arc-Sine of x.
LD_API f32 asin32( f32 x );
/// Arc-Sine of x.
LD_API f64 asin64( f64 x );
/// Arc-Cosine of x.
LD_API f32 acos32( f32 x );
/// Arc-Cosine of x.
LD_API f64 acos64( f64 x );
/// Arc-Tangent of x.
LD_API f32 atan32( f32 x );
/// Arc-Tangent of x.
LD_API f64 atan64( f64 x );
/// Two argument arc-tangent.
LD_API f32 atan2_32( f32 y, f32 x );
/// Two argument arc-tangent.
LD_API f64 atan2_64( f64 y, f64 x );

/// Natural logarithm.
LD_API f32 ln32( f32 x );
/// Natural logarithm.
LD_API f64 ln64( f64 x );

/// Log2.
LD_API f32 log2_32( f32 x );
/// Log2.
LD_API f64 log2_64( f64 x );

/// Log10.
LD_API f32 log10_32( f32 x );
/// Log10.
LD_API f64 log10_64( f64 x );

/// e^x
LD_API f32 exp32( f32 x );
/// e^x
LD_API f64 exp64( f64 x );

/// Linearly interpolate a to b.
LD_API f32 lerp32( f32 a, f32 b, f32 t );
/// Linearly interpolate a to b.
LD_API f64 lerp64( f64 a, f64 b, f64 t );
/// Get t value from value v.
LD_API f32 inv_lerp32( f32 a, f32 b, f32 v );
/// Get t value from value v.
LD_API f64 inv_lerp64( f64 a, f64 b, f64 v );
/// Remap value v from range imin-imax to range omin-omax.
LD_API f32 remap32( f32 imin, f32 imax, f32 omin, f32 omax, f32 v );
/// Remap value v from range imin-imax to range omin-omax.
LD_API f64 remap64( f64 imin, f64 imax, f64 omin, f64 omax, f64 v );

/// Smooth step interpolate a to b.
LD_API f32 smooth_step32( f32 a, f32 b, f32 t );
/// Smooth step interpolate a to b.
LD_API f64 smooth_step64( f64 a, f64 b, f64 t );
/// Smoother step interpolate a to b.
LD_API f32 smoother_step32( f32 a, f32 b, f32 t );
/// Smoother step interpolate a to b.
LD_API f64 smoother_step64( f64 a, f64 b, f64 t );

/// Arc-Sine of x. Does not return NaN.
header_only f32 asin32_no_nan( f32 x ) {
    return absolute( x ) >= 1.0f ?
        F32_HALF_PI * signum( x ) : asin32( x );
}
/// Arc-Sine of x. Does not return NaN.
header_only f64 asin64_no_nan( f64 x ) {
    return absolute( x ) >= 1.0f ?
        F64_HALF_PI * signum( x ) : asin64( x );
}

/// Wrap value to 0.0 -> 360.0 range.
header_only f32 wrap_deg32( f32 degrees ) {
    f32 result = mod32( degrees, 360.0f );
    if( result < 0.0f ) {
        result += 360.0f;
    }
    return result;
}
/// Wrap value to 0.0 -> 360.0 range.
header_only f64 wrap_deg64( f64 degrees ) {
    f64 result = mod64( degrees, 360.0 );
    if( result < 0.0 ) {
        result += 360.0;
    }
    return result;
}

/// Wrap value to -PI -> PI range.
header_only f32 wrap_rad32( f32 radians ) {
    return mod32( radians + F32_PI, F32_TAU ) - F32_PI;
}
/// Wrap value to -PI -> PI range.
header_only f64 wrap_rad64( f64 radians ) {
    return mod64( radians + F64_PI, F64_TAU ) - F64_PI;
}

/// Truncate float to signed integer.
#define truncate_i8( f )  ((i8)(f))
/// Truncate float to signed integer.
#define truncate_i16( f ) ((i16)(f))
/// Truncate float to signed integer.
#define truncate_i32( f ) ((i32)(f))
/// Truncate float to signed integer.
#define truncate_i64( f ) ((i64)(f))

/// Truncate float to integer.
#define truncate_u8( f )  ((u8)(f))
/// Truncate float to integer.
#define truncate_u16( f ) ((u16)(f))
/// Truncate float to integer.
#define truncate_u32( f ) ((u32)(f))
/// Truncate float to integer.
#define truncate_u64( f ) ((u64)(f))

/// Floor 32-bit float to signed integer.
#define floor32_i8( f )\
    (((f) > 0.0f) ? truncate_i8(f) : truncate_i8( (f) - 0.99999f ))
/// Floor 32-bit float to signed integer.
#define floor32_i16( f )\
    (((f) > 0.0f) ? truncate_i16(f) : truncate_i16( (f) - 0.99999f ))
/// Floor 32-bit float to signed integer.
#define floor32_i32( f )\
    (((f) > 0.0f) ? truncate_i32(f) : truncate_i32( (f) - 0.99999f ))
/// Floor 32-bit float to signed integer.
#define floor32_i64( f )\
    (((f) > 0.0f) ? truncate_i64(f) : truncate_i64( (f) - 0.99999f ))

/// Floor 32-bit float to integer.
#define floor32_u8( f )\
    (((f) > 0.0f) ? truncate_u8(f) : 0 )
/// Floor 32-bit float to integer.
#define floor32_u16( f )\
    (((f) > 0.0f) ? truncate_u16(f) : 0 )
/// Floor 32-bit float to integer.
#define floor32_u32( f )\
    (((f) > 0.0f) ? truncate_u32(f) : 0 )
/// Floor 32-bit float to integer.
#define floor32_u64( f )\
    (((f) > 0.0f) ? truncate_u64(f) : 0 )

/// Floor 64-bit float to signed integer.
#define floor64_i8( f )\
    (((f) > 0.0) ? truncate_i8(f) : truncate_i8( (f) - 0.9999999 ))
/// Floor 64-bit float to signed integer.
#define floor64_i16( f )\
    (((f) > 0.0) ? truncate_i16(f) : truncate_i16( (f) - 0.9999999 ))
/// Floor 64-bit float to signed integer.
#define floor64_i32( f )\
    (((f) > 0.0) ? truncate_i32(f) : truncate_i32( (f) - 0.9999999 ))
/// Floor 64-bit float to signed integer.
#define floor64_i64( f )\
    (((f) > 0.0) ? truncate_i64(f) : truncate_i64( (f) - 0.9999999 ))

/// Floor 64-bit float to integer.
#define floor64_u8( f )\
    (((f) > 0.0) ? truncate_u8(f) : 0 )
/// Floor 64-bit float to integer.
#define floor64_u16( f )\
    (((f) > 0.0) ? truncate_u16(f) : 0 )
/// Floor 64-bit float to integer.
#define floor64_u32( f )\
    (((f) > 0.0) ? truncate_u32(f) : 0 )
/// Floor 64-bit float to integer.
#define floor64_u64( f )\
    (((f) > 0.0) ? truncate_u64(f) : 0 )

/// Ceil 32-bit float to signed integer.
#define ceil32_i8( f )\
    (((f) > 0.0f) ? truncate_i8( (f) + 0.99999f ) : truncate_i8( (f) ) )
/// Ceil 32-bit float to signed integer.
#define ceil32_i16( f )\
    (((f) > 0.0f) ? truncate_i16( (f) + 0.99999f ) : truncate_i16( (f) ) )
/// Ceil 32-bit float to signed integer.
#define ceil32_i32( f )\
    (((f) > 0.0f) ? truncate_i32( (f) + 0.99999f ) : truncate_i32( (f) ) )
/// Ceil 32-bit float to signed integer.
#define ceil32_i64( f )\
    (((f) > 0.0f) ? truncate_i64( (f) + 0.99999f ) : truncate_i64( (f) ) )

/// Ceil 32-bit float to integer.
#define ceil32_u8( f )\
    ( truncate_u8( (f) + 0.99999f ) )
/// Ceil 32-bit float to integer.
#define ceil32_u16( f )\
    ( truncate_u16( (f) + 0.99999f ) )
/// Ceil 32-bit float to integer.
#define ceil32_u32( f )\
    ( truncate_u32( (f) + 0.99999f ) )
/// Ceil 32-bit float to integer.
#define ceil32_u64( f )\
    ( truncate_u64( (f) + 0.99999f ) )

/// Ceil 64-bit float to signed integer.
#define ceil64_i8( f )\
    (((f) > 0.0) ? truncate_i8( (f) + 0.9999999 ) : truncate_i8( (f) ) )
/// Ceil 64-bit float to signed integer.
#define ceil64_i16( f )\
    (((f) > 0.0) ? truncate_i16( (f) + 0.9999999 ) : truncate_i16( (f) ) )
/// Ceil 64-bit float to signed integer.
#define ceil64_i32( f )\
    (((f) > 0.0) ? truncate_i32( (f) + 0.9999999 ) : truncate_i32( (f) ) )
/// Ceil 64-bit float to signed integer.
#define ceil64_i64( f )\
    (((f) > 0.0) ? truncate_i64( (f) + 0.9999999 ) : truncate_i64( (f) ) )

/// Ceil 64-bit float to integer.
#define ceil64_u8( f )\
    ( truncate_u8( (f) + 0.9999999 ) )
/// Ceil 64-bit float to integer.
#define ceil64_u16( f )\
    ( truncate_u16( (f) + 0.9999999 ) )
/// Ceil 64-bit float to integer.
#define ceil64_u32( f )\
    ( truncate_u32( (f) + 0.9999999 ) )
/// Ceil 64-bit float to integer.
#define ceil64_u64( f )\
    ( truncate_u64( (f) + 0.9999999 ) )

/// Round 32-bit float to integer
#define round32_u8(f)\
    (truncate_u8( (f) + 0.5f ))
/// Round 32-bit float to integer
#define round32_u16(f)\
    (truncate_u16( (f) + 0.5f ))
/// Round 32-bit float to integer
#define round32_u32(f)\
    (truncate_u32( (f) + 0.5f ))
/// Round 32-bit float to integer
#define round32_u64(f)\
    (truncate_u64( (f) + 0.5f ))

/// Round 32-bit float to signed integer
#define round32_i8(f)\
    (((f) > 0.0f) ? truncate_i8( (f) + 0.5f ) : truncate_i8( (f) - 0.5f ))
/// Round 32-bit float to signed integer
#define round32_i16(f)\
    (((f) > 0.0f) ? truncate_i16( (f) + 0.5f ) : truncate_i16( (f) - 0.5f ))
/// Round 32-bit float to signed integer
#define round32_i32(f)\
    (((f) > 0.0f) ? truncate_i32( (f) + 0.5f ) : truncate_i32( (f) - 0.5f ))
/// Round 32-bit float to signed integer
#define round32_i64(f)\
    (((f) > 0.0f) ? truncate_i64( (f) + 0.5f ) : truncate_i64( (f) - 0.5f ))

/// Round 64-bit float to integer
#define round64_u8(f)\
    (truncate_u8( (f) + 0.5 ))
/// Round 64-bit float to integer
#define round64_u16(f)\
    (truncate_u16( (f) + 0.5 ))
/// Round 64-bit float to integer
#define round64_u32(f)\
    (truncate_u32( (f) + 0.5 ))
/// Round 64-bit float to integer
#define round64_u64(f)\
    (truncate_u64( (f) + 0.5 ))

/// Round 64-bit float to signed integer
#define round64_i8(f)\
    (((f) > 0.0) ? truncate_i8( (f) + 0.5 ) : truncate_i8( (f) - 0.5 ))
/// Round 64-bit float to signed integer
#define round64_i16(f)\
    (((f) > 0.0) ? truncate_i16( (f) + 0.5 ) : truncate_i16( (f) - 0.5 ))
/// Round 64-bit float to signed integer
#define round64_i32(f)\
    (((f) > 0.0) ? truncate_i32( (f) + 0.5 ) : truncate_i32( (f) - 0.5 ))
/// Round 64-bit float to signed integer
#define round64_i64(f)\
    (((f) > 0.0) ? truncate_i64( (f) + 0.5 ) : truncate_i64( (f) - 0.5 ))

/// Normalize integer -1 to 1 range
#define normalize_range_i8_f32( x ) \
    ((f32)(x) / ((x) > 0 ? (f32)I8_MAX : -((f32)I8_MIN)))
/// Normalize integer -1 to 1 range
#define normalize_range_i16_f32( x )\
    ((f32)(x) / ((x) > 0 ? (f32)I16_MAX : -((f32)I16_MIN)))
/// Normalize integer -1 to 1 range
#define normalize_range_i32_f32( x )\
    ((f32)(x) / ((x) > 0 ? (f32)I32_MAX : -((f32)I32_MIN)))
/// Normalize integer -1 to 1 range
#define normalize_range_i64_f32( x )\
    ((f32)(x) / ((x) > 0 ? (f32)I64_MAX : -((f32)I64_MIN)))

/// Normalize integer 0 to 1 range
#define normalize_range_u8_f32( x ) \
    ((f32)(x) / (f32)(U8_MAX))
/// Normalize integer 0 to 1 range
#define normalize_range_u16_f32( x )\
    ((f32)(x) / (f32)(U16_MAX))
/// Normalize integer 0 to 1 range
#define normalize_range_u32_f32( x )\
    ((f32)(x) / (f32)(U32_MAX))
/// Normalize integer 0 to 1 range
#define normalize_range_u64_f32( x )\
    ((f32)(x) / (f32)(U64_MAX))

/// Normalize integer -1 to 1 range
#define normalize_range_i8_f64( x ) \
    ((f64)(x) / ((x) > 0 ? (f64)I8_MAX : -((f64)I8_MIN)))
/// Normalize integer -1 to 1 range
#define normalize_range_i16_f64( x )\
    ((f64)(x) / ((x) > 0 ? (f64)I16_MAX : -((f64)I16_MIN)))
/// Normalize integer -1 to 1 range
#define normalize_range_i32_f64( x )\
    ((f64)(x) / ((x) > 0 ? (f64)I32_MAX : -((f64)I32_MIN)))
/// Normalize integer -1 to 1 range
#define normalize_range_i64_f64( x )\
    ((f64)(x) / ((x) > 0 ? (f64)I64_MAX : -((f64)I64_MIN)))

/// Normalize integer 0 to 1 range
#define normalize_range_u8_f64( x ) \
    ((f64)(x) / (f64)(U8_MAX))
/// Normalize integer 0 to 1 range
#define normalize_range_u16_f64( x )\
    ((f64)(x) / (f64)(U16_MAX))
/// Normalize integer 0 to 1 range
#define normalize_range_u32_f64( x )\
    ((f64)(x) / (f64)(U32_MAX))
/// Normalize integer 0 to 1 range
#define normalize_range_u64_f64( x )\
    ((f64)(x) / (f64)(U64_MAX))

/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range32_u8( f )\
    ((u8)(absolute((f)) * (f32)U8_MAX))
/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range32_u16( f )\
    ((u16)(absolute((f)) * (f32)U16_MAX))
/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range32_u32( f )\
    ((u32)(absolute((f)) * (f32)U32_MAX))
/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range32_u64( f )\
    ((u64)(absolute((f)) * (f32)U64_MAX))

/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range32_i8( f )\
    ((i8)( absolute( (f) ) * (f32)I8_MAX ) * (i8)(signum((f))))
/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range32_i16( f )\
    ((i16)( absolute( (f) ) * (f32)I16_MAX ) * (i16)(signum((f))))
/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range32_i32( f )\
    ((i32)( absolute( (f) ) * (f32)I32_MAX ) * (i32)(signum((f))))
/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range32_i64( f )\
    ((i64)( absolute( (f) ) * (f32)I64_MAX ) * (i64)(signum((f))))

/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range64_u8( f )\
    ((u8)(absolute((f)) * (f64)U8_MAX))
/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range64_u16( f )\
    ((u16)(absolute((f)) * (f64)U16_MAX))
/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range64_u32( f )\
    ((u32)(absolute((f)) * (f64)U32_MAX))
/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
#define normalize_range64_u64( f )\
    ((u64)(absolute((f)) * (f64)U64_MAX))

/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range64_i8( f )\
    ((i8)( absolute( (f) ) * (f64)I8_MAX ) * (i8)(signum((f))))
/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range64_i16( f )\
    ((i16)( absolute( (f) ) * (f64)I16_MAX ) * (i16)(signum((f))))
/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range64_i32( f )\
    ((i32)( absolute( (f) ) * (f64)I32_MAX ) * (i32)(signum((f))))
/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
#define normalize_range64_i64( f )\
    ((i64)( absolute( (f) ) * (f64)I64_MAX ) * (i64)(signum((f))))

/// Rotate left 8-bit integer.
#define rotate_left8( bitpattern, rotation )\
    (bitpattern << rotation) | (bitpattern >> (-rotation & 7))
/// Rotate right 8-bit integer.
#define rotate_right8( bitpattern, rotation )\
    (bitpattern >> rotation) | (bitpattern << (8 - rotation))
/// Rotate left 16-bit integer.
#define rotate_left16( bitpattern, rotation )\
    (bitpattern << rotation) | (bitpattern >> (-rotation & 15))
/// Rotate right 16-bit integer.
#define rotate_right16( bitpattern, rotation )\
    (bitpattern >> rotation) | (bitpattern << (16 - rotation))
/// Rotate left 32-bit integer.
#define rotate_left32( bitpattern, rotation )\
    (bitpattern << rotation) | (bitpattern >> (-rotation & 31ul))
/// Rotate right 32-bit integer.
#define rotate_right32( bitpattern, rotation )\
    (bitpattern >> rotation) | (bitpattern << (32ul - rotation))
/// Rotate left 64-bit integer.
#define rotate_left64( bitpattern, rotation )\
    (bitpattern << rotation) | (bitpattern >> (-rotation & 63ull))
/// Rotate right 64-bit integer.
#define rotate_right64( bitpattern, rotation )\
    (bitpattern >> rotation) | (bitpattern << (64ull - rotation))

#if !defined(LD_CORE_MATH_TYPES_TYPEDEF)
#define LD_CORE_MATH_TYPES_TYPEDEF
/// 2-component 32-bit float vector
typedef union vec2 vec2;
/// 3-component 32-bit float vector
typedef union vec3 vec3;
/// RGB color.
typedef union vec3 rgb;
/// HSV color.
typedef union vec3 hsv;
/// 3D rotation expressed in euler angles.
typedef union vec3 euler_angles;
/// 4-component 32-bit float vector
typedef union vec4 vec4;
/// RGBA color.
typedef union vec4 rgba;

/// 2-component 32-bit integer vector
typedef union ivec2 ivec2;
/// 3-component 32-bit integer vector
typedef union ivec3 ivec3;
/// 4-component 32-bit integer vector
typedef union ivec4 ivec4;

/// Quaternion.
typedef union quat quat;

/// Column-major 2x2 32-bit float matrix.
typedef union mat2 mat2;
/// Column-major 3x3 32-bit float matrix.
typedef union mat3 mat3;
/// Column-major 4x4 32-bit float matrix.
typedef union mat4 mat4;

/// Transform.
typedef struct Transform Transform;
#endif

/// Negate vector.
LD_API vec2 v2_neg( vec2 v );
/// Add vectors.
LD_API vec2 v2_add( vec2 lhs, vec2 rhs );
/// Sub vectors.
LD_API vec2 v2_sub( vec2 lhs, vec2 rhs );
/// Scale vector.
LD_API vec2 v2_mul( vec2 lhs, f32 rhs );
/// Divide vector.
LD_API vec2 v2_div( vec2 lhs, f32 rhs );
/// Horizontal add.
/// Add components.
LD_API f32 v2_hadd( vec2 v );
/// Horizontal multiply.
/// Multiply all components.
LD_API f32 v2_hmul( vec2 v );
/// Hadamard product.
/// Component-wise multiplication.
LD_API vec2 v2_hadamard( vec2 lhs, vec2 rhs );
/// Dot/inner product.
LD_API f32 v2_dot( vec2 lhs, vec2 rhs );
/// Rotate vector by theta(radians).
LD_API vec2 v2_rotate( vec2 v, f32 theta_radians );
/// Clamp vector's magnitude to range min-max.
LD_API vec2 v2_clamp_mag( vec2 v, f32 min, f32 max );
/// Compare vectors for equality.
LD_API b32 v2_cmp( vec2 a, vec2 b );
/// Swap x and y components.
LD_API vec2 v2_swap( vec2 v );
/// Square magnitude.
LD_API f32 v2_sqrmag( vec2 v );
/// Magnitude.
LD_API f32 v2_mag( vec2 v );
/// Normalize vector.
LD_API vec2 v2_normalize( vec2 v );
/// Angle(radians) of two vectors.
LD_API f32 v2_angle( vec2 lhs, vec2 rhs );
/// Linearly interpolate vectors.
LD_API vec2 v2_lerp( vec2 a, vec2 b, f32 t );
/// Smooth step interpolate vectors.
LD_API vec2 v2_smooth_step( vec2 a, vec2 b, f32 t );
/// Smoother step interpolate vectors.
LD_API vec2 v2_smoother_step( vec2 a, vec2 b, f32 t );

/// Negate vector.
LD_API ivec2 iv2_neg( ivec2 v );
/// Add vectors.
LD_API ivec2 iv2_add( ivec2 lhs, ivec2 rhs );
/// Sub vectors.
LD_API ivec2 iv2_sub( ivec2 lhs, ivec2 rhs );
/// Scale vector.
LD_API ivec2 iv2_mul( ivec2 lhs, i32 rhs );
/// Divide vector.
LD_API ivec2 iv2_div( ivec2 lhs, i32 rhs );
/// Horizontal add.
/// Add components.
LD_API i32 iv2_hadd( ivec2 v );
/// Horizontal multiply.
/// Multiply all components.
LD_API i32 iv2_hmul( ivec2 v );
/// Hadamard product.
/// Component-wise multiplication.
LD_API ivec2 iv2_hadamard( ivec2 lhs, ivec2 rhs );
/// Dot/inner product.
LD_API f32 iv2_dot( ivec2 lhs, ivec2 rhs );
/// Compare vectors for equality.
LD_API b32 iv2_cmp( ivec2 a, ivec2 b );
/// Swap x and y components.
LD_API ivec2 iv2_swap( ivec2 v );
/// Square magnitude.
LD_API f32 iv2_sqrmag( ivec2 v );
/// Magnitude.
LD_API f32 iv2_mag( ivec2 v );
/// Angle(radians) of two vectors.
LD_API f32 iv2_angle( ivec2 lhs, ivec2 rhs );

/// Create HSV from hue, saturation and value.
LD_API hsv v3_hsv( f32 hue, f32 saturation, f32 value );
/// Convert RGB to HSV.
LD_API hsv rgb_to_hsv( rgb col );
/// Convert HSV to RGB.
LD_API rgb hsv_to_rgb( hsv col );
/// Convert linear color space to sRGB color space.
LD_API rgb linear_to_srgb( rgb linear );
/// Convert sRGB color space to linear color space.
LD_API rgb srgb_to_linear( rgb srgb );
/// Negate vector.
LD_API vec3 v3_neg( vec3 v );
/// Add vectors.
LD_API vec3 v3_add( vec3 lhs, vec3 rhs );
/// Sub vectors.
LD_API vec3 v3_sub( vec3 lhs, vec3 rhs );
/// Scale vector.
LD_API vec3 v3_mul( vec3 lhs, f32 rhs );
/// Divide vector.
LD_API vec3 v3_div( vec3 lhs, f32 rhs );
/// Horizontal add.
/// Add components.
LD_API f32 v3_hadd( vec3 v );
/// Horizontal multiply.
/// Multiply all components.
LD_API f32 v3_hmul( vec3 v );
/// Hadamard product.
/// Component-wise multiplication.
LD_API vec3 v3_hadamard( vec3 lhs, vec3 rhs );
/// Cross product.
LD_API vec3 v3_cross( vec3 lhs, vec3 rhs );
/// Dot/inner product.
LD_API f32 v3_dot( vec3 lhs, vec3 rhs );
/// Reflect direction across axis of given normal.
LD_API vec3 v3_reflect( vec3 direction, vec3 normal );
/// Clamp vector's magnitude to range min-max.
LD_API vec3 v3_clamp_mag( vec3 v, f32 min, f32 max );
/// Compare vectors for equality.
LD_API b32 v3_cmp( vec3 a, vec3 b );
/// Shift components to the left.
/// Wraps around.
LD_API vec3 v3_shift_left( vec3 v );
/// Shift components to the right.
/// Wraps around.
LD_API vec3 v3_shift_right( vec3 v );
/// Square magnitude.
LD_API f32 v3_sqrmag( vec3 v );
/// Magnitude.
LD_API f32 v3_mag( vec3 v );
/// Normalize vector.
LD_API vec3 v3_normalize( vec3 v );
/// Angle(radians) of two vectors.
LD_API f32 v3_angle( vec3 lhs, vec3 rhs );
/// Linearly interpolate vectors.
LD_API vec3 v3_lerp( vec3 a, vec3 b, f32 t );
/// Smooth step interpolate vectors.
LD_API vec3 v3_smooth_step( vec3 a, vec3 b, f32 t );
/// Smoother step interpolate vectors.
LD_API vec3 v3_smoother_step( vec3 a, vec3 b, f32 t );

/// Negate vector.
LD_API ivec3 iv3_neg( ivec3 v );
/// Add vectors.
LD_API ivec3 iv3_add( ivec3 lhs, ivec3 rhs );
/// Sub vectors.
LD_API ivec3 iv3_sub( ivec3 lhs, ivec3 rhs );
/// Scale vector.
LD_API ivec3 iv3_mul( ivec3 lhs, i32 rhs );
/// Divide vector.
LD_API ivec3 iv3_div( ivec3 lhs, i32 rhs );
/// Horizontal add.
/// Add components.
LD_API i32 iv3_hadd( ivec3 v );
/// Horizontal multiply.
/// Multiply all components.
LD_API i32 iv3_hmul( ivec3 v );
/// Hadamard product.
/// Component-wise multiplication.
LD_API ivec3 iv3_hadamard( ivec3 lhs, ivec3 rhs );
/// Dot/inner product.
LD_API f32 iv3_dot( ivec3 lhs, ivec3 rhs );
/// Compare vectors for equality.
LD_API b32 iv3_cmp( ivec3 a, ivec3 b );
/// Shift components to the left.
/// Wraps around.
LD_API ivec3 iv3_shift_left( ivec3 v );
/// Shift components to the right.
/// Wraps around.
LD_API ivec3 iv3_shift_right( ivec3 v );
/// Square magnitude.
LD_API f32 iv3_sqrmag( ivec3 v );
/// Magnitude.
LD_API f32 iv3_mag( ivec3 v );
/// Angle(radians) of two vectors.
LD_API f32 iv3_angle( ivec3 lhs, ivec3 rhs );

/// Negate vector.
LD_API vec4 v4_neg( vec4 v );
/// Add vectors.
LD_API vec4 v4_add( vec4 lhs, vec4 rhs );
/// Sub vectors.
LD_API vec4 v4_sub( vec4 lhs, vec4 rhs );
/// Scale vector.
LD_API vec4 v4_mul( vec4 lhs, f32 rhs );
/// Divide vector.
LD_API vec4 v4_div( vec4 lhs, f32 rhs );
/// Horizontal add.
/// Add components.
LD_API f32 v4_hadd( vec4 v );
/// Horizontal multiply.
/// Multiply all components.
LD_API f32 v4_hmul( vec4 v );
/// Hadamard product.
/// Component-wise multiplication.
LD_API vec4 v4_hadamard( vec4 lhs, vec4 rhs );
/// Dot/inner product.
LD_API f32 v4_dot( vec4 lhs, vec4 rhs );
/// Clamp vector's magnitude to range min-max.
LD_API vec4 v4_clamp_mag( vec4 v, f32 min, f32 max );
/// Compare vectors for equality.
LD_API b32 v4_cmp( vec4 a, vec4 b );
/// Shift components to the left.
/// Wraps around.
LD_API vec4 v4_shift_left( vec4 v );
/// Shift components to the right.
/// Wraps around.
LD_API vec4 v4_shift_right( vec4 v );
/// Square magnitude.
LD_API f32 v4_sqrmag( vec4 v );
/// Magnitude.
LD_API f32 v4_mag( vec4 v );
/// Normalize vector.
LD_API vec4 v4_normalize( vec4 v );
/// Angle(radians) of two vectors.
LD_API f32 v4_angle( vec4 lhs, vec4 rhs );
/// Linearly interpolate vectors.
LD_API vec4 v4_lerp( vec4 a, vec4 b, f32 t );
/// Smooth step interpolate vectors.
LD_API vec4 v4_smooth_step( vec4 a, vec4 b, f32 t );
/// Smoother step interpolate vectors.
LD_API vec4 v4_smoother_step( vec4 a, vec4 b, f32 t );

/// Negate vector.
LD_API ivec4 iv4_neg( ivec4 v );
/// Add vectors.
LD_API ivec4 iv4_add( ivec4 lhs, ivec4 rhs );
/// Sub vectors.
LD_API ivec4 iv4_sub( ivec4 lhs, ivec4 rhs );
/// Scale vector.
LD_API ivec4 iv4_mul( ivec4 lhs, i32 rhs );
/// Divide vector.
LD_API ivec4 iv4_div( ivec4 lhs, i32 rhs );
/// Horizontal add.
/// Add components.
LD_API i32 iv4_hadd( ivec4 v );
/// Horizontal multiply.
/// Multiply all components.
LD_API i32 iv4_hmul( ivec4 v );
/// Hadamard product.
/// Component-wise multiplication.
LD_API ivec4 iv4_hadamard( ivec4 lhs, ivec4 rhs );
/// Dot/inner product.
LD_API f32 iv4_dot( ivec4 lhs, ivec4 rhs );
/// Compare vectors for equality.
LD_API b32 iv4_cmp( ivec4 a, ivec4 b );
/// Shift components to the left.
/// Wraps around.
LD_API ivec4 iv4_shift_left( ivec4 v );
/// Shift components to the right.
/// Wraps around.
LD_API ivec4 iv4_shift_right( ivec4 v );
/// Square magnitude.
LD_API f32 iv4_sqrmag( ivec4 v );
/// Magnitude.
LD_API f32 iv4_mag( ivec4 v );

/// Create quaternion from angle(radians) and axis.
LD_API quat q_angle_axis( f32 angle, vec3 axis );
/// Create quaternion from euler angles.
LD_API quat q_euler( f32 pitch, f32 yaw, f32 roll );
/// Create quaternion from euler angles.
LD_API quat q_euler_v3( euler_angles euler );
/// Negate quaternion.
LD_API quat q_neg( quat q );
/// Add quaternions.
LD_API quat q_add( quat lhs, quat rhs );
/// Sub quaternions.
LD_API quat q_sub( quat lhs, quat rhs );
/// Scale quaternion.
LD_API quat q_mul( quat lhs, f32 rhs );
/// Mul quaternions.
LD_API quat q_mul_q( quat lhs, quat rhs );
/// Mul quaternion and vector3.
LD_API vec3 q_mul_v3( quat lhs, vec3 rhs );
/// Div quaternion.
LD_API quat q_div( quat lhs, f32 rhs );
/// Compare quaternions for equality.
LD_API b32 q_cmp( quat a, quat b );
/// Square magnitude.
LD_API f32 q_sqrmag( quat q );
/// Magnitude.
LD_API f32 q_mag( quat q );
/// Normalize quaternion.
LD_API quat q_normalize( quat q );
/// Conjugate of quaternion.
LD_API quat q_conjugate( quat q );
/// Inverse of quaternion.
LD_API quat q_inverse( quat q );
/// Angle between quaternions.
LD_API f32 q_angle( quat lhs, quat rhs );
/// Quaternion inner product.
LD_API f32 q_dot( quat lhs, quat rhs );
/// Linear interpolation.
LD_API quat q_lerp( quat a, quat b, f32 t );
/// Spherical interpolation.
LD_API quat q_slerp( quat a, quat b, f32 t );

/// Add matrices.
LD_API mat2 m2_add( mat2 lhs, mat2 rhs );
/// Sub matrices.
LD_API mat2 m2_sub( mat2 lhs, mat2 rhs );
/// Scale matrix.
LD_API mat2 m2_mul( mat2 lhs, f32 rhs );
/// Mul matrices.
LD_API mat2 m2_mul_m2( mat2 lhs, mat2 rhs );
/// Div matrix.
LD_API mat2 m2_div( mat2 lhs, f32 rhs );
/// Transpose matrix.
LD_API mat2 m2_transpose( mat2 m );
/// Determinant of matrix.
LD_API f32 m2_determinant( mat2 m );

/// Add matrices.
LD_API mat3 m3_add( const mat3* lhs, const mat3* rhs );
/// Sub matrices.
LD_API mat3 m3_sub( const mat3* lhs, const mat3* rhs );
/// Scale matrix.
LD_API mat3 m3_mul( const mat3* lhs, f32 rhs );
/// Mul matrices.
LD_API mat3 m3_mul_m3( const mat3* lhs, const mat3* rhs );
/// Div matrix.
LD_API mat3 m3_div( const mat3* lhs, f32 rhs );
/// Transpose matrix.
LD_API mat3 m3_transpose( const mat3* m );
/// Determinant of matrix.
LD_API f32 m3_determinant( const mat3* m );

/// Create view matrix.
LD_API mat4 m4_view( vec3 position, vec3 target, vec3 up );
/// Create 2d view matrix.
LD_API mat4 m4_view_2d( vec2 position, vec2 up );
/// Create orthographic projection matrix.
LD_API mat4 m4_ortho(
    f32 left, f32 right,
    f32 bottom, f32 top,
    f32 near_clip, f32 far_clip );
/// Create perspective projection matrix.
LD_API mat4 m4_perspective(
    f32 fov_radians, f32 aspect_ratio,
    f32 near_clip, f32 far_clip );
/// Create translation matrix.
LD_API mat4 m4_translation( f32 x, f32 y, f32 z );
/// Create 2d translation matrix.
LD_API mat4 m4_translation_2d( f32 x, f32 y );
/// Create translation matrix from vector3.
LD_API mat4 m4_translation_v3( vec3 translation );
/// Create 2d translation matrix from vector2.
LD_API mat4 m4_translation_2d_v2( vec2 translation );
/// Create rotation matrix around x axis.
LD_API mat4 m4_rotation_pitch( f32 pitch_radians );
/// Create rotation matrix around y axis.
LD_API mat4 m4_rotation_yaw( f32 yaw_radians );
/// Create rotation matrix around z axis.
LD_API mat4 m4_rotation_roll( f32 roll_radians );
/// Create rotation matrix from euler angles.
LD_API mat4 m4_rotation_euler(
    f32 pitch_radians, f32 yaw_radians, f32 roll_radians );
/// Create rotation matrix from euler angles vector.
LD_API mat4 m4_rotation_euler_v3( vec3 rotation );
/// Create rotation matrix from quaternion.
LD_API mat4 m4_rotation_q( quat rotation );
/// Create scale matrix.
LD_API mat4 m4_scale( f32 width, f32 height, f32 length );
/// Create 2d scale matrix.
LD_API mat4 m4_scale_2d( f32 width, f32 height );
/// Create scale matrix from vector.
LD_API mat4 m4_scale_v3( vec3 scale );
/// Create 2d scale matrix from vector.
LD_API mat4 m4_scale_2d_v2( vec2 scale );
/// Create transform matrix.
LD_API mat4 m4_transform( vec3 translation, quat rotation, vec3 scale );
/// Create transform matrix using euler angles.
LD_API mat4 m4_transform_euler( vec3 translation, vec3 rotation, vec3 scale );
/// Create 2d transform matrix.
LD_API mat4 m4_transform_2d( vec2 translation, f32 rotation_radians, vec2 scale );
/// Add matrices.
LD_API mat4 m4_add( const mat4* lhs, const mat4* rhs );
/// Sub matrices.
LD_API mat4 m4_sub( const mat4* lhs, const mat4* rhs );
/// Scale matrix.
LD_API mat4 m4_mul( const mat4* lhs, f32 rhs );
/// Mul matrices.
LD_API mat4 m4_mul_m4( const mat4* lhs, const mat4* rhs );
/// Mul matrix vector.
LD_API vec4 m4_mul_v4( const mat4* lhs, vec4 rhs );
/// Mul matrix vector.
LD_API vec3 m4_mul_v3( const mat4* lhs, vec3 rhs );
/// Div matrix.
LD_API mat4 m4_div( const mat4* lhs, f32 rhs );
/// Transpose matrix.
LD_API mat4 m4_transpose( const mat4* m );
/// Get submatrix at given cell.
LD_API mat3 m4_submatrix( const mat4* m, usize row, usize column );
/// Get determinant of submatrix at given cell.
LD_API f32 m4_minor( const mat4* m, usize row, usize column );
/// Get the cofactor of minor at given cell.
LD_API f32 m4_cofactor( const mat4* m, usize row, usize column );
/// Construct matrix from cofactors.
LD_API mat4 m4_cofactor_matrix( const mat4* m );
/// Construct adjoint matrix.
LD_API mat4 m4_adjoint( const mat4* m );
/// Determinant of matrix.
LD_API f32 m4_determinant( const mat4* m );
/// Attempt to construct inverse matrix.
/// Returns true if successful.
LD_API b32 m4_inverse( const mat4* m, mat4* out_inverse );
/// Construct inverse matrix.
/// If unsuccessful, cells will be NaN due to division by zero!
LD_API mat4 m4_inverse_unchecked( const mat4* m );
/// Construct normal matrix.
/// Returns true if successful.
LD_API b32 m4_normal_matrix( const mat4* m, mat3* out_normal_matrix );
/// Construct normal matrix.
/// If unsuccessful, cells will be NaN!
LD_API mat3 m4_normal_matrix_unchecked( const mat4* m );
/// Extract position from transform matrix.
LD_API vec3 m4_transform_position( const mat4* m );

/// Get transform's local space matrix.
LD_API mat4 transform_local_matrix( Transform* t );
/// Get transform's world space matrix.
/// This value should be cached for performance.
LD_API mat4 transform_world_matrix( Transform* t );
/// Get transform's local space position.
LD_API vec3 transform_local_position( Transform* t );
/// Get transform's world space position.
/// This value should be chached for performance.
LD_API vec3 transform_world_position( Transform* t );
/// Set transform's position.
LD_API void transform_set_position( Transform* t, vec3 position );
/// Translate a transform.
LD_API void transform_translate( Transform* t, vec3 translation );
/// Get transform's local space rotation.
LD_API quat transform_local_rotation( Transform* t );
/// Get transform's world space rotation.
/// This value should be cached for performance.
LD_API quat transform_world_rotation( Transform* t );
/// Set transform's rotation.
LD_API void transform_set_rotation( Transform* t, quat rotation );
/// Rotate a transform.
LD_API void transform_rotate( Transform* t, quat rotation );
/// Get transform's local space scale.
LD_API vec3 transform_local_scale( Transform* t );
/// Get transform's world space scale.
/// This value should be cached for performance.
LD_API vec3 transform_world_scale( Transform* t );
/// Set transform's scale.
LD_API void transform_set_scale( Transform* t, vec3 scale );
/// Scale a transform.
LD_API void transform_scale( Transform* t, vec3 scale );

/// Calculate transform's local space forward basis vector.
LD_API vec3 transform_local_forward( Transform* t );
/// Calculate transform's local space right basis vector.
LD_API vec3 transform_local_right( Transform* t );
/// Calculate transform's local space up basis vector.
LD_API vec3 transform_local_up( Transform* t );

/// Calculate transform's world space forward basis vector.
LD_API vec3 transform_world_forward( Transform* t );
/// Calculate transform's world space right basis vector.
LD_API vec3 transform_world_right( Transform* t );
/// Calculate transform's world space up basis vector.
LD_API vec3 transform_world_up( Transform* t );

// NOTE(alicia): C++ operator overloading

#if defined(__cplusplus)

inline vec2 operator+( vec2 lhs, vec2 rhs ) {
    return v2_add( lhs, rhs );
}
inline vec2 operator-( vec2 lhs, vec2 rhs ) {
    return v2_sub( lhs, rhs );
}
inline vec2 operator*( vec2 lhs, f32 rhs ) {
    return v2_mul( lhs, rhs );
}
inline vec2 operator*( f32 lhs, vec2 rhs ) {
    return v2_mul( rhs, lhs );
}
inline vec2 operator/( vec2 lhs, f32 rhs ) {
    return v2_div( lhs, rhs );
}
inline vec2 operator-( vec2 v ) {
    return v2_neg( v );
}
inline b32 operator==( vec2 lhs, vec2 rhs ) {
    return v2_cmp( lhs, rhs );
}
inline b32 operator!=( vec2 lhs, vec2 rhs ) {
    return !v2_cmp( lhs, rhs );
}

inline ivec2 operator+( ivec2 lhs, ivec2 rhs ) {
    return iv2_add( lhs, rhs );
}
inline ivec2 operator-( ivec2 lhs, ivec2 rhs ) {
    return iv2_sub( lhs, rhs );
}
inline ivec2 operator*( ivec2 lhs, i32 rhs ) {
    return iv2_mul( lhs, rhs );
}
inline ivec2 operator*( i32 lhs, ivec2 rhs ) {
    return iv2_mul( rhs, lhs );
}
inline ivec2 operator/( ivec2 lhs, i32 rhs ) {
    return iv2_div( lhs, rhs );
}
inline ivec2 operator-( ivec2 v ) {
    return iv2_neg( v );
}
inline b32 operator==( ivec2 lhs, ivec2 rhs ) {
    return iv2_cmp( lhs, rhs );
}
inline b32 operator!=( ivec2 lhs, ivec2 rhs ) {
    return !iv2_cmp( lhs, rhs );
}

inline vec3 operator+( vec3 lhs, vec3 rhs ) {
    return v3_add( lhs, rhs );
}
inline vec3 operator-( vec3 lhs, vec3 rhs ) {
    return v3_sub( lhs, rhs );
}
inline vec3 operator*( vec3 lhs, f32 rhs ) {
    return v3_mul( lhs, rhs );
}
inline vec3 operator*( f32 lhs, vec3 rhs ) {
    return v3_mul( rhs, lhs );
}
inline vec3 operator/( vec3 lhs, f32 rhs ) {
    return v3_div( lhs, rhs );
}
inline vec3 operator-( vec3 v ) {
    return v3_neg( v );
}
inline b32 operator==( vec3 lhs, vec3 rhs ) {
    return v3_cmp( lhs, rhs );
}
inline b32 operator!=( vec3 lhs, vec3 rhs ) {
    return !v3_cmp( lhs, rhs );
}

inline ivec3 operator+( ivec3 lhs, ivec3 rhs ) {
    return iv3_add( lhs, rhs );
}
inline ivec3 operator-( ivec3 lhs, ivec3 rhs ) {
    return iv3_sub( lhs, rhs );
}
inline ivec3 operator*( ivec3 lhs, i32 rhs ) {
    return iv3_mul( lhs, rhs );
}
inline ivec3 operator*( i32 lhs, ivec3 rhs ) {
    return iv3_mul( rhs, lhs );
}
inline ivec3 operator/( ivec3 lhs, i32 rhs ) {
    return iv3_div( lhs, rhs );
}
inline ivec3 operator-( ivec3 v ) {
    return iv3_neg( v );
}
inline b32 operator==( ivec3 lhs, ivec3 rhs ) {
    return iv3_cmp( lhs, rhs );
}
inline b32 operator!=( ivec3 lhs, ivec3 rhs ) {
    return !iv3_cmp( lhs, rhs );
}

inline vec4 operator+( vec4 lhs, vec4 rhs ) {
    return v4_add( lhs, rhs );
}
inline vec4 operator-( vec4 lhs, vec4 rhs ) {
    return v4_sub( lhs, rhs );
}
inline vec4 operator*( vec4 lhs, f32 rhs ) {
    return v4_mul( lhs, rhs );
}
inline vec4 operator*( f32 lhs, vec4 rhs ) {
    return v4_mul( rhs, lhs );
}
inline vec4 operator/( vec4 lhs, f32 rhs ) {
    return v4_div( lhs, rhs );
}
inline vec4 operator-( vec4 v ) {
    return v4_neg( v );
}
inline b32 operator==( vec4 lhs, vec4 rhs ) {
    return v4_cmp( lhs, rhs );
}
inline b32 operator!=( vec4 lhs, vec4 rhs ) {
    return !v4_cmp( lhs, rhs );
}

inline ivec4 operator+( ivec4 lhs, ivec4 rhs ) {
    return iv4_add( lhs, rhs );
}
inline ivec4 operator-( ivec4 lhs, ivec4 rhs ) {
    return iv4_sub( lhs, rhs );
}
inline ivec4 operator*( ivec4 lhs, i32 rhs ) {
    return iv4_mul( lhs, rhs );
}
inline ivec4 operator*( i32 lhs, ivec4 rhs ) {
    return iv4_mul( rhs, lhs );
}
inline ivec4 operator/( ivec4 lhs, i32 rhs ) {
    return iv4_div( lhs, rhs );
}
inline ivec4 operator-( ivec4 v ) {
    return iv4_neg( v );
}
inline b32 operator==( ivec4 lhs, ivec4 rhs ) {
    return iv4_cmp( lhs, rhs );
}
inline b32 operator!=( ivec4 lhs, ivec4 rhs ) {
    return !iv4_cmp( lhs, rhs );
}

inline quat operator+( quat lhs, quat rhs ) {
    return q_add( lhs, rhs );
}
inline quat operator-( quat lhs, quat rhs ) {
    return q_sub( lhs, rhs );
}
inline quat operator*( quat lhs, f32 rhs ) {
    return q_mul( lhs, rhs );
}
inline quat operator*( f32 lhs, quat rhs ) {
    return q_mul( rhs, lhs );
}
inline quat operator*( quat lhs, quat rhs ) {
    return q_mul_q( lhs, rhs );
}
inline vec3 operator*( quat lhs, vec3 rhs ) {
    return q_mul_v3( lhs, rhs );
}
inline quat operator/( quat lhs, f32 rhs ) {
    return q_div( lhs, rhs );
}
inline quat operator-( quat v ) {
    return q_neg( v );
}
inline b32 operator==( quat lhs, quat rhs ) {
    return q_cmp( lhs, rhs );
}
inline b32 operator!=( quat lhs, quat rhs ) {
    return !q_cmp( lhs, rhs );
}

inline mat2 operator+( mat2 lhs, mat2 rhs ) {
    return m2_add( lhs, rhs );
}
inline mat2 operator-( mat2 lhs, mat2 rhs ) {
    return m2_sub( lhs, rhs );
}
inline mat2 operator*( mat2 lhs, f32 rhs ) {
    return m2_mul( lhs, rhs );
}
inline mat2 operator*( f32 lhs, mat2 rhs ) {
    return m2_mul( rhs, lhs );
}
inline mat2 operator*( mat2 lhs, mat2 rhs ) {
    return m2_mul_m2( lhs, rhs );
}
inline mat2 operator/( mat2 lhs, f32 rhs ) {
    return m2_div( lhs, rhs );
}

inline mat3 operator+( const mat3& lhs, const mat3& rhs ) {
    return m3_add( &lhs, &rhs );
}
inline mat3 operator-( const mat3& lhs, const mat3& rhs ) {
    return m3_sub( &lhs, &rhs );
}
inline mat3 operator*( const mat3& lhs, f32 rhs ) {
    return m3_mul( &lhs, rhs );
}
inline mat3 operator*( f32 lhs, const mat3& rhs ) {
    return m3_mul( &rhs, lhs );
}
inline mat3 operator*( const mat3& lhs, const mat3& rhs ) {
    return m3_mul_m3( &lhs, &rhs );
}
inline mat3 operator/( const mat3& lhs, f32 rhs ) {
    return m3_div( &lhs, rhs );
}

inline mat4 operator+( const mat4& lhs, const mat4& rhs ) {
    return m4_add( &lhs, &rhs );
}
inline mat4 operator-( const mat4& lhs, const mat4& rhs ) {
    return m4_sub( &lhs, &rhs );
}
inline mat4 operator*( const mat4& lhs, f32 rhs ) {
    return m4_mul( &lhs, rhs );
}
inline mat4 operator*( f32 lhs, const mat4& rhs ) {
    return m4_mul( &rhs, lhs );
}
inline mat4 operator*( const mat4& lhs, const mat4& rhs ) {
    return m4_mul_m4( &lhs, &rhs );
}
inline vec3 operator*( const mat4& lhs, vec3 rhs ) {
    return m4_mul_v3( &lhs, rhs );
}
inline vec4 operator*( const mat4& lhs, vec4 rhs ) {
    return m4_mul_v4( &lhs, rhs );
}
inline mat4 operator/( const mat4& lhs, f32 rhs ) {
    return m4_div( &lhs, rhs );
}

#endif


#endif // header guard
