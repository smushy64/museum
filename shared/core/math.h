#if !defined(LD_SHARED_CORE_MATH_H)
#define LD_SHARED_CORE_MATH_H
/**
 * Description:  Math Library
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 14, 2023
*/
#include "defines.h"
#include "constants.h"

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

/// Sign of number.
#define signum( x ) (( (x) > 0 ) - ( (x) < 0 ))
/// Absolute value of number.
#define absolute( x ) ((x) * signum(x)) 

/// Smallest of two values
#define min( a, b ) ( (a) < (b) ? (a) : (b) )

/// Largest of two values
#define max( a, b ) ( (a) < (b) ? (b) : (a) )

/// Clamp a number between min and max, inclusive-inclusive
#define clamp( value, min, max )\
    ( (value) < (min) ? (min) : ( (value) > (max) ? (max) : (value) ) )
/// Clamp a 32-bit float to 0-1
#define clamp01( value )\
    clamp( (value), 0.0f, 1.0f )

/// Convert radians to degrees.
#define to_radians( theta )\
    ( theta * F32_TO_RAD )
/// Convert degrees to radians.
#define to_degrees( theta )\
    ( theta * F32_TO_DEG )

/// Check if float is NaN.
CORE_API b32 is_nan( f32 x );

/// Square root of x.
CORE_API f32 square_root( f32 x );

/// Reciprical of square root of x.
CORE_API f32 inv_square_root( f32 x );

/// Raise base to integer exponent.
CORE_API f32 poweri( f32 base, i32 exp );

/// Raise base to float exponent.
CORE_API f32 power( f32 base, f32 exp );

/// Float modulus.
CORE_API f32 modulus( f32 lhs, f32 rhs );

/// Sine of x.
CORE_API f32 sine( f32 x );
/// Cosine of x.
CORE_API f32 cosine( f32 x );
/// Tangent of x.
CORE_API f32 tangent( f32 x );

/// Sine and Cos of x.
/// If you need both values,
/// this is faster than calling each function individually.
CORE_API void sine_cosine( f32 x, f32* out_sin, f32* out_cos );

/// Arc-Sine of x.
CORE_API f32 arc_sine( f32 x );
/// Arc-Cosine of x.
CORE_API f32 arc_cosine( f32 x );
/// Arc-Tangent of x.
CORE_API f32 arc_tangent( f32 x );
/// Two argument arc-tangent.
CORE_API f32 arc_tangent2( f32 y, f32 x );

/// Natural logarithm.
CORE_API f32 natural_logarithm( f32 x );

/// Log2.
CORE_API f32 logarithm2( f32 x );

/// Log10.
CORE_API f32 logarithm10( f32 x );

/// e^x
CORE_API f32 e_power( f32 x );

/// Linearly interpolate a to b.
CORE_API f32 lerp( f32 a, f32 b, f32 t );
/// Get t value from value v.
CORE_API f32 inv_lerp( f32 a, f32 b, f32 v );
/// Remap value v from range imin-imax to range omin-omax.
CORE_API f32 remap( f32 imin, f32 imax, f32 omin, f32 omax, f32 v );
/// Smooth step interpolate a to b.
CORE_API f32 smooth_step( f32 a, f32 b, f32 t );
/// Smoother step interpolate a to b.
CORE_API f32 smoother_step( f32 a, f32 b, f32 t );

/// Arc-Sine of x. Does not return NaN.
header_only f32 arc_sine_no_nan( f32 x ) {
    return absolute( x ) >= 1.0f ?
        F32_HALF_PI * signum( x ) : arc_sine( x );
}

/// Wrap value to 0.0 -> 360.0 range.
header_only f32 wrap_degrees( f32 degrees ) {
    f32 result = modulus( degrees, 360.0f );
    if( result < 0.0f ) {
        result += 360.0f;
    }
    return result;
}

/// Wrap value to -PI -> PI range.
header_only f32 wrap_radians( f32 radians ) {
    return modulus( radians + F32_PI, F32_TAU ) - F32_PI;
}

/// Round float to u32
CORE_API u32 round_u32( f32 x );
/// Floor float to u32
CORE_API u32 floor_u32( f32 x );
/// Ceil float to u32
CORE_API u32 ceil_u32( f32 x );

/// Round float to i32
CORE_API i32 round_i32( f32 x );
/// Floor float to i32
CORE_API i32 floor_i32( f32 x );
/// Ceil float to i32
CORE_API i32 ceil_i32( f32 x );

/// Normalize integer -1 to 1 range
header_only f32 normalize_range_i8_f32( i8 x ) {
    return ((f32)(x) / ((x) > 0 ? (f32)I8_MAX : -((f32)I8_MIN)));
}
/// Normalize integer -1 to 1 range
header_only f32 normalize_range_i16_f32( i16 x ) {
    return ((f32)(x) / ((x) > 0 ? (f32)I16_MAX : -((f32)I16_MIN)));
}
/// Normalize integer -1 to 1 range
header_only f32 normalize_range_i32_f32( i32 x ) {
    return ((f32)(x) / ((x) > 0 ? (f32)I32_MAX : -((f32)I32_MIN)));
}
/// Normalize integer -1 to 1 range
header_only f32 normalize_range_i64_f32( i64 x ) {
    return ((f32)(x) / ((x) > 0 ? (f32)I64_MAX : -((f32)I64_MIN)));
}

/// Normalize integer 0 to 1 range
header_only f32 normalize_range_u8_f32( u8 x ) {
    return ((f32)(x) / (f32)(U8_MAX));
}
/// Normalize integer 0 to 1 range
header_only f32 normalize_range_u16_f32( u16 x ) {
    return ((f32)(x) / (f32)(U16_MAX));
}
/// Normalize integer 0 to 1 range
header_only f32 normalize_range_u32_f32( u32 x ) {
    return ((f32)(x) / (f32)(U32_MAX));
}
/// Normalize integer 0 to 1 range
header_only f32 normalize_range_u64_f32( u64 x ) {
    return ((f32)(x) / (f32)(U64_MAX));
}

/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u8 normalize_range_f32_u8( f32 f ) {
    return ((u8)(absolute((f)) * (f32)U8_MAX));
}
/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u16 normalize_range_f32_u16( f32 f ) {
    return ((u16)(absolute((f)) * (f32)U16_MAX));
}
/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u32 normalize_range_f32_u32( f32 f ) {
    return ((u32)(absolute((f)) * (f32)U32_MAX));
}
/// Normalize 32-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u64 normalize_range_f32_u64( f32 f ) {
    return ((u64)(absolute((f)) * (f32)U64_MAX));
}

/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i8 normalize_range_f32_i8( f32 f ) {
    return ((i8)( absolute( (f) ) * (f32)I8_MAX ) * (i8)(signum((f))));
}
/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i16 normalize_range_f32_i16( f32 f ) {
    return ((i16)( absolute( (f) ) * (f32)I16_MAX ) * (i16)(signum((f))));
}
/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i32 normalize_range_f32_i32( f32 f ) {
    return ((i32)( absolute( (f) ) * (f32)I32_MAX ) * (i32)(signum((f))));
}
/// Normalize 32-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i64 normalize_range_f32_i64( f32 f ) {
    return ((i64)( absolute( (f) ) * (f32)I64_MAX ) * (i64)(signum((f))));
}

/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u8 normalize_range_f64_u8( f64 f ) {
    return ((u8)(absolute((f)) * (f64)U8_MAX));
}
/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u16 normalize_range_f64_u16( f64 f ) {
    return ((u16)(absolute((f)) * (f64)U16_MAX));
}
/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u32 normalize_range_f64_u32( f64 f ) {
    return ((u32)(absolute((f)) * (f64)U32_MAX));
}
/// Normalize 64-bit float to integer min-max.
/// Float must be in 0.0-1.0 range
header_only u64 normalize_range_f64_u64( f64 f ) {
    return ((u64)(absolute((f)) * (f64)U64_MAX));
}

/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i8 normalize_range_f64_i8( f64 f ) {
    return ((i8)( absolute( (f) ) * (f64)I8_MAX ) * (i8)(signum((f))));
}
/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i16 normalize_range_f64_i16( f64 f ) {
    return ((i16)( absolute( (f) ) * (f64)I16_MAX ) * (i16)(signum((f))));
}
/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i32 normalize_range_f64_i32( f64 f ) {
    return ((i32)( absolute( (f) ) * (f64)I32_MAX ) * (i32)(signum((f))));
}
/// Normalize 64-bit float to integer min-max.
/// Float must be in -1.0-1.0 range
header_only i64 normalize_range_f64_i64( f64 f ) {
    return ((i64)( absolute( (f) ) * (f64)I64_MAX ) * (i64)(signum((f))));
}

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

#define VEC2_COMPONENT_COUNT (2)
/// 2-component 32-bit float vector
union vec2 {
    struct { f32 x, y; };
    struct { f32 u, v; };
    struct { f32 width, height; };
    f32 c[VEC2_COMPONENT_COUNT];
};
#define VEC2_ZERO  (vec2){  0.0f,  0.0f }
#define VEC2_ONE   (vec2){  1.0f,  1.0f }
#define VEC2_LEFT  (vec2){ -1.0f,  0.0f }
#define VEC2_RIGHT (vec2){  1.0f,  0.0f }
#define VEC2_UP    (vec2){  0.0f,  1.0f }
#define VEC2_DOWN  (vec2){  0.0f, -1.0f }

/// Create zero vector.
header_only vec2 v2_zero(void) { return VEC2_ZERO; }
/// Create vector with all components set to given scalar.
header_only vec2 v2_scalar( f32 scalar ) {
    vec2 result = {scalar, scalar};
    return result;
}
/// Create vector with given components.
header_only vec2 v2( f32 x, f32 y ) {
    vec2 result = { x, y };
    return result;
}

#define IVEC2_COMPONENT_COUNT (2)
/// 2-component 32-bit signed integer vector
union ivec2 {
    struct { i32 x, y; };
    struct { i32 u, v; };
    struct { i32 width, height; };
    i32 c[IVEC2_COMPONENT_COUNT];
};
#define IVEC2_ZERO  (ivec2){  0,  0 } 
#define IVEC2_ONE   (ivec2){  1,  1 } 
#define IVEC2_LEFT  (ivec2){ -1,  0 } 
#define IVEC2_RIGHT (ivec2){  1,  0 } 
#define IVEC2_UP    (ivec2){  0,  1 } 
#define IVEC2_DOWN  (ivec2){  0, -1 } 

/// Create zero vector.
header_only ivec2 iv2_zero(void) { return IVEC2_ZERO; }
/// Create vector with all components set to given scalar.
header_only ivec2 iv2_scalar( i32 scalar ) {
    ivec2 result = { scalar, scalar };
    return result;
}
/// Create vector with given components.
header_only ivec2 iv2( i32 x, i32 y ) {
    ivec2 result = { x, y };
    return result;
}

#define VEC3_COMPONENT_COUNT (3)
/// 3-component 32-bit float vector
union vec3 {
    struct {
        union {
            struct { f32 x, y; };
            struct { f32 r, g; };
            struct { f32 hue, saturation; };
            struct { f32 width, height; };
            struct { f32 pitch, yaw; };
            vec2 xy;
            vec2 rg;
        };
        union {
            f32 z;
            f32 b;
            f32 value;
            f32 length;
            f32 roll;
        };
    };
    f32 c[VEC3_COMPONENT_COUNT];
};

#define VEC3_ZERO    (vec3){  0.0f,  0.0f,  0.0f }
#define VEC3_ONE     (vec3){  1.0f,  1.0f,  1.0f } 
#define VEC3_LEFT    (vec3){ -1.0f,  0.0f,  0.0f } 
#define VEC3_RIGHT   (vec3){  1.0f,  0.0f,  0.0f } 
#define VEC3_UP      (vec3){  0.0f,  1.0f,  0.0f } 
#define VEC3_DOWN    (vec3){  0.0f, -1.0f,  0.0f } 
#define VEC3_FORWARD (vec3){  0.0f,  0.0f,  1.0f } 
#define VEC3_BACK    (vec3){  0.0f,  0.0f, -1.0f } 

#define RGB_RED     (rgb){ 1.0f, 0.0f, 0.0f } 
#define RGB_GREEN   (rgb){ 0.0f, 1.0f, 0.0f } 
#define RGB_BLUE    (rgb){ 0.0f, 0.0f, 1.0f } 
#define RGB_MAGENTA (rgb){ 1.0f, 0.0f, 1.0f } 
#define RGB_YELLOW  (rgb){ 1.0f, 1.0f, 0.0f } 
#define RGB_CYAN    (rgb){ 0.0f, 1.0f, 1.0f } 
#define RGB_BLACK   (rgb){ 0.0f, 0.0f, 0.0f } 
#define RGB_WHITE   (rgb){ 1.0f, 1.0f, 1.0f } 
#define RGB_GRAY    (rgb){ 0.5f, 0.5f, 0.5f } 
#define RGB_GREY    (rgb){ 0.5f, 0.5f, 0.5f } 

/// Create zero vector.
header_only vec3 v3_zero(void) { return VEC3_ZERO; }
/// Create vector with given components.
header_only vec3 v3( f32 x, f32 y, f32 z ) {
    vec3 result = { x, y, z };
    return result;
}
/// Create vector with all components set to given scalar.
header_only vec3 v3_scalar( f32 scalar ) {
    vec3 result = { scalar, scalar, scalar };
    return result;
}

#define IVEC3_COMPONENT_COUNT (3)
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
    i32 c[IVEC3_COMPONENT_COUNT];
};

#define IVEC3_ZERO    (ivec3){  0,  0,  0 } 
#define IVEC3_ONE     (ivec3){  1,  1,  1 } 
#define IVEC3_LEFT    (ivec3){ -1,  0,  0 } 
#define IVEC3_RIGHT   (ivec3){  1,  0,  0 } 
#define IVEC3_UP      (ivec3){  0,  1,  0 } 
#define IVEC3_DOWN    (ivec3){  0, -1,  0 } 
#define IVEC3_FORWARD (ivec3){  0,  0,  1 } 
#define IVEC3_BACK    (ivec3){  0,  0, -1 } 

/// Create zero vector.
header_only ivec3 iv3_zero(void) { return IVEC3_ZERO; }
/// Create vector with all components set to given scalar.
header_only ivec3 iv3_scalar( i32 scalar ) {
    ivec3 result = { scalar, scalar, scalar };
    return result;
}
/// Create vector with given components.
header_only ivec3 iv3( i32 x, i32 y, i32 z ) {
    ivec3 result = { x, y, z };
    return result;
}

#define VEC4_COMPONENT_COUNT (4)
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
                union { f32 z; f32 b; };
            };
            vec3 xyz;
            vec3 rgb;
        };
        union { f32 w; f32 a; };
    };
    f32 c[VEC4_COMPONENT_COUNT];
};

#define VEC4_ZERO (vec4){ 0.0f, 0.0f, 0.0f, 0.0f }
#define VEC4_ONE  (vec4){ 1.0f, 1.0f, 1.0f, 1.0f }

#define RGBA_RED     (rgba){ 1.0f, 0.0f, 0.0f, 1.0f } 
#define RGBA_GREEN   (rgba){ 0.0f, 1.0f, 0.0f, 1.0f } 
#define RGBA_BLUE    (rgba){ 0.0f, 0.0f, 1.0f, 1.0f } 
#define RGBA_MAGENTA (rgba){ 1.0f, 0.0f, 1.0f, 1.0f } 
#define RGBA_YELLOW  (rgba){ 1.0f, 1.0f, 0.0f, 1.0f } 
#define RGBA_CYAN    (rgba){ 0.0f, 1.0f, 1.0f, 1.0f } 
#define RGBA_BLACK   (rgba){ 0.0f, 0.0f, 0.0f, 1.0f } 
#define RGBA_WHITE   (rgba){ 1.0f, 1.0f, 1.0f, 1.0f } 
#define RGBA_GRAY    (rgba){ 0.5f, 0.5f, 0.5f, 1.0f } 
#define RGBA_GREY    (rgba){ 0.5f, 0.5f, 0.5f, 1.0f } 
#define RGBA_CLEAR   (rgba){ 0.0f, 0.0f, 0.0f, 0.0f } 

/// Create zero vector.
header_only vec4 v4_zero(void) { return VEC4_ZERO; }
/// Create vector with given components.
header_only vec4 v4( f32 x, f32 y, f32 z, f32 w ) {
    vec4 result = { x, y, z, w };
    return result;
}
/// Create vector with all components set to given scalar.
header_only vec4 v4_scalar( f32 scalar ) {
    vec4 result = { scalar, scalar, scalar, scalar };
    return result;
}

#define IVEC4_COMPONENT_COUNT (4)
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
    i32 c[IVEC4_COMPONENT_COUNT];
};

#define IVEC4_ZERO (ivec4){ 0, 0, 0, 0 } 
#define IVEC4_ONE  (ivec4){ 1, 1, 1, 1 } 

/// Create vector with given components.
header_only ivec4 iv4( i32 x, i32 y, i32 z, i32 w ) {
    ivec4 result = { x, y, z, w };
    return result;
}
/// Create zero vector.
header_only ivec4 iv4_zero(void) { return IVEC4_ZERO; }
/// Create vector with all components set to given scalar.
header_only ivec4 iv4_scalar( i32 scalar ) {
    ivec4 result = { scalar, scalar, scalar, scalar };
    return result;
}

#define QUAT_COMPONENT_COUNT (4)
/// Quaternion.
union quat {
    struct {
        union { f32 w; f32 a; };
        union {
            struct { f32 x, y, z; };
            struct { f32 b, c, d; };
            vec3 xyz;
            vec3 bcd;
        };
    };
    f32 q[QUAT_COMPONENT_COUNT];
};

#define QUAT_ZERO     (quat){ 0.0f, 0.0f, 0.0f, 0.0f } 
#define QUAT_IDENTITY (quat){ 1.0f, 0.0f, 0.0f, 0.0f } 

/// Create zero quaternion.
header_only quat q_zero(void) { return QUAT_ZERO; }
/// Create quaternion with set to given scalars.
header_only quat q( f32 w, f32 x, f32 y, f32 z ) {
    quat result = { w, x, y, z };
    return result;
}

#define MAT2_CELL_COUNT   (4)
#define MAT2_COLUMN_COUNT (2)
#define MAT2_ROW_COUNT    (2)
/// Column-major 2x2 32-bit float matrix.
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
    f32  m[MAT2_COLUMN_COUNT][MAT2_ROW_COUNT];
};

#define MAT2_ZERO (mat2){\
    0.0f, 0.0f,\
    0.0f, 0.0f\
}
#define MAT2_IDENTITY (mat2){\
    1.0f, 0.0f,\
    0.0f, 1.0f\
}

/// Create zero matrix.
header_only mat2 m2_zero(void) { return MAT2_ZERO; }
/// Create matrix with given values.
header_only mat2 m2(
    f32 m00, f32 m01,
    f32 m10, f32 m11
) {
    mat2 result = { m00, m01, m10, m11 };
    return result;
}

#define MAT3_CELL_COUNT   (9)
#define MAT3_COLUMN_COUNT (3)
#define MAT3_ROW_COUNT    (3)
/// Column-major 3x3 32-bit float matrix.
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
    f32  m[MAT3_COLUMN_COUNT][MAT3_ROW_COUNT];
};

#define MAT3_ZERO (mat3){\
    0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f\
}
#define MAT3_IDENTITY (mat3){\
    1.0f, 0.0f, 0.0f,\
    0.0f, 1.0f, 0.0f,\
    0.0f, 0.0f, 1.0f\
}

/// Create zero matrix.
header_only mat3 m3_zero(void) { return MAT3_ZERO; }
/// Create matrix from given values.
header_only mat3 m3(
    f32 m00, f32 m01, f32 m02,
    f32 m10, f32 m11, f32 m12,
    f32 m20, f32 m21, f32 m22
) {
    mat3 result = {
        m00, m01, m02,
        m10, m11, m12,
        m20, m21, m22
    };
    return result;
}

#define MAT4_CELL_COUNT   (16)
#define MAT4_COLUMN_COUNT (4)
#define MAT4_ROW_COUNT    (4)
/// Column-major 4x4 32-bit float matrix.
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
    f32  m[MAT4_COLUMN_COUNT][MAT4_ROW_COUNT];
};

#define MAT4_ZERO (mat4){\
    0.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 0.0f\
}
#define MAT4_IDENTITY (mat4){\
    1.0f, 0.0f, 0.0f, 0.0f,\
    0.0f, 1.0f, 0.0f, 0.0f,\
    0.0f, 0.0f, 1.0f, 0.0f,\
    0.0f, 0.0f, 0.0f, 1.0f\
}

/// Create a zero matrix.
header_only mat4 m4_zero(void) { return MAT4_ZERO; }
/// Create a matrix from given values.
header_only mat4 m4(
    f32 m00, f32 m01, f32 m02, f32 m03,
    f32 m10, f32 m11, f32 m12, f32 m13,
    f32 m20, f32 m21, f32 m22, f32 m23,
    f32 m30, f32 m31, f32 m32, f32 m33
) {
    mat4 result = {
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    };
    return result;
}

/// Create vec2 from vec3.
header_only vec2 v2_v3( vec3 v ) {
    return v2( v.x, v.y );
}
/// Create vec2 from vec4.
header_only vec2 v2_v4( vec4 v ) {
    return v2( v.x, v.y );
}
/// Create vec2 from ivec2.
header_only vec2 v2_iv2( ivec2 v ) {
    return v2( (f32)v.x, (f32)v.y );
}

/// Create ivec2 from ivec3.
header_only ivec2 iv2_iv3( ivec3 v ) {
    return iv2( v.x, v.y );
}
/// Create ivec2 from ivec4.
header_only ivec2 iv2_iv4( ivec4 v ) {
    return iv2( v.x, v.y );
}
/// Create ivec2 from vec2.
header_only ivec2 iv2_v2( vec2 v ) {
    return iv2( (i32)v.x, (i32)v.y );
}

/// Create vec3 from vec2.
header_only vec3 v3_v2( vec2 v ) {
    return v3( v.x, v.y, 0.0f );
}
/// Create vec3 from vec4.
header_only vec3 v3_v4( vec4 v ) {
    return v3( v.x, v.y, v.z );
}
/// Create vec3 from ivec3.
header_only vec3 v3_iv3( ivec3 v ) {
    return v3( (f32)v.x, (f32)v.y, (f32)v.z );
}
/// Create rgb from rgba.
header_only rgb rgb_rgba( rgba col ) {
    return v3( col.r, col.g, col.b );
}

/// Create ivec3 from ivec2.
header_only ivec3 iv3_iv2( ivec2 v ) {
    return iv3( v.x, v.y, 0 );
}
/// Create ivec3 from ivec4.
header_only ivec3 iv3_iv4( ivec4 v ) {
    return iv3( v.x, v.y, v.z );
}
/// Create ivec3 from vec3.
header_only ivec3 iv3_v3( vec3 v ) {
    return iv3( (i32)v.x, (i32)v.y, (i32)v.z );
}

/// Create vec4 from vec2.
header_only vec4 v4_v2( vec2 v ) {
    return v4( v.x, v.y, 0.0f, 0.0f );
}
/// Create vec4 from vec3.
header_only vec4 v4_v3( vec3 v ) {
    return v4( v.x, v.y, v.z, 0.0f );
}
/// Create vec4 from quat.
header_only vec4 v4_q( quat q ) {
    return v4( q.a, q.b, q.c, q.d );
}
/// Create RGBA from RGB. Alpha set to 1
header_only rgba rgba_rgb( rgb col ) {
    return v4( col.r, col.g, col.b, 1.0f );
}
/// Create vec4 from ivec4.
header_only vec4 v4_iv4( ivec4 v ) {
    return v4( (f32)v.x, (f32)v.y, (f32)v.z, (f32)v.w );
}

/// Create ivec4 from ivec2.
header_only ivec4 iv4_iv2( ivec2 v ) {
    return iv4( v.x, v.y, 0, 0 );
}
/// Create ivec4 from ivec3.
header_only ivec4 iv4_iv3( ivec3 v ) {
    return iv4( v.x, v.y, v.z, 0 );
}
/// Create ivec4 from vec4.
header_only ivec4 iv4_v4( vec4 v ) {
    return iv4( (i32)v.x, (i32)v.y, (i32)v.z, (i32)v.w );
}

/// Create euler angles from quaternion.
CORE_API euler_angles euler_q( quat q );

/// Create mat2 from mat3.
CORE_API mat2 m2_m3( const mat3* m );
/// Create mat2 from mat4.
CORE_API mat2 m2_m4( const mat4* m );

/// Create mat3 from mat2.
header_only mat3 m3_m2( mat2 m ) {
    return (mat3){
        m.c[0], m.c[1], 0.0f,
        m.c[2], m.c[3], 0.0f,
          0.0f,   0.0f, 0.0f
    };
}
/// Create mat3 from mat4.
CORE_API mat3 m3_m4( const mat4* m );

/// Create mat4 from mat2.
header_only mat4 m4_m2( mat2 m ) {
    return (mat4){
        m.c[0], m.c[1], 0.0f, 0.0f,
        m.c[2], m.c[3], 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
    };
}
/// Create mat4 from mat3.
CORE_API mat4 m4_m3( const mat3* m );

/// Negate vector.
CORE_API vec2 v2_neg( vec2 v );
/// Add vectors.
CORE_API vec2 v2_add( vec2 lhs, vec2 rhs );
/// Sub vectors.
CORE_API vec2 v2_sub( vec2 lhs, vec2 rhs );
/// Scale vector.
CORE_API vec2 v2_mul( vec2 lhs, f32 rhs );
/// Divide vector.
CORE_API vec2 v2_div( vec2 lhs, f32 rhs );
/// Horizontal add.
/// Add components.
CORE_API f32 v2_hadd( vec2 v );
/// Horizontal multiply.
/// Multiply all components.
CORE_API f32 v2_hmul( vec2 v );
/// Hadamard product.
/// Component-wise multiplication.
CORE_API vec2 v2_hadamard( vec2 lhs, vec2 rhs );
/// Get the aspect ratio of x and y.
CORE_API f32 v2_aspect_ratio( vec2 v );
/// Dot/inner product.
CORE_API f32 v2_dot( vec2 lhs, vec2 rhs );
/// Rotate vector by theta(radians).
CORE_API vec2 v2_rotate( vec2 v, f32 theta_radians );
/// Clamp vector's magnitude to range min-max.
CORE_API vec2 v2_clamp_mag( vec2 v, f32 min, f32 max );
/// Compare vectors for equality.
CORE_API b32 v2_cmp( vec2 a, vec2 b );
/// Swap x and y components.
CORE_API vec2 v2_swap( vec2 v );
/// Square magnitude.
CORE_API f32 v2_sqrmag( vec2 v );
/// Magnitude.
CORE_API f32 v2_mag( vec2 v );
/// Normalize vector.
CORE_API vec2 v2_normalize( vec2 v );
/// Angle(radians) of two vectors.
CORE_API f32 v2_angle( vec2 lhs, vec2 rhs );
/// Linearly interpolate vectors.
CORE_API vec2 v2_lerp( vec2 a, vec2 b, f32 t );
/// Smooth step interpolate vectors.
CORE_API vec2 v2_smooth_step( vec2 a, vec2 b, f32 t );
/// Smoother step interpolate vectors.
CORE_API vec2 v2_smoother_step( vec2 a, vec2 b, f32 t );

/// Negate vector.
CORE_API ivec2 iv2_neg( ivec2 v );
/// Add vectors.
CORE_API ivec2 iv2_add( ivec2 lhs, ivec2 rhs );
/// Sub vectors.
CORE_API ivec2 iv2_sub( ivec2 lhs, ivec2 rhs );
/// Scale vector.
CORE_API ivec2 iv2_mul( ivec2 lhs, i32 rhs );
/// Divide vector.
CORE_API ivec2 iv2_div( ivec2 lhs, i32 rhs );
/// Horizontal add.
/// Add components.
CORE_API i32 iv2_hadd( ivec2 v );
/// Horizontal multiply.
/// Multiply all components.
CORE_API i32 iv2_hmul( ivec2 v );
/// Hadamard product.
/// Component-wise multiplication.
CORE_API ivec2 iv2_hadamard( ivec2 lhs, ivec2 rhs );
/// Get the aspect ratio of x and y.
CORE_API f32 iv2_aspect_ratio( ivec2 v );
/// Dot/inner product.
CORE_API f32 iv2_dot( ivec2 lhs, ivec2 rhs );
/// Compare vectors for equality.
CORE_API b32 iv2_cmp( ivec2 a, ivec2 b );
/// Swap x and y components.
CORE_API ivec2 iv2_swap( ivec2 v );
/// Square magnitude.
CORE_API f32 iv2_sqrmag( ivec2 v );
/// Magnitude.
CORE_API f32 iv2_mag( ivec2 v );
/// Angle(radians) of two vectors.
CORE_API f32 iv2_angle( ivec2 lhs, ivec2 rhs );

/// Create HSV from hue, saturation and value.
CORE_API hsv v3_hsv( f32 hue, f32 saturation, f32 value );
/// Convert RGB to HSV.
CORE_API hsv rgb_to_hsv( rgb col );
/// Convert HSV to RGB.
CORE_API rgb hsv_to_rgb( hsv col );
/// Convert linear color space to sRGB color space.
CORE_API rgb linear_to_srgb( rgb linear );
/// Convert sRGB color space to linear color space.
CORE_API rgb srgb_to_linear( rgb srgb );
/// Negate vector.
CORE_API vec3 v3_neg( vec3 v );
/// Add vectors.
CORE_API vec3 v3_add( vec3 lhs, vec3 rhs );
/// Sub vectors.
CORE_API vec3 v3_sub( vec3 lhs, vec3 rhs );
/// Scale vector.
CORE_API vec3 v3_mul( vec3 lhs, f32 rhs );
/// Divide vector.
CORE_API vec3 v3_div( vec3 lhs, f32 rhs );
/// Horizontal add.
/// Add components.
CORE_API f32 v3_hadd( vec3 v );
/// Horizontal multiply.
/// Multiply all components.
CORE_API f32 v3_hmul( vec3 v );
/// Hadamard product.
/// Component-wise multiplication.
CORE_API vec3 v3_hadamard( vec3 lhs, vec3 rhs );
/// Cross product.
CORE_API vec3 v3_cross( vec3 lhs, vec3 rhs );
/// Dot/inner product.
CORE_API f32 v3_dot( vec3 lhs, vec3 rhs );
/// Reflect direction across axis of given normal.
CORE_API vec3 v3_reflect( vec3 direction, vec3 normal );
/// Clamp vector's magnitude to range min-max.
CORE_API vec3 v3_clamp_mag( vec3 v, f32 min, f32 max );
/// Compare vectors for equality.
CORE_API b32 v3_cmp( vec3 a, vec3 b );
/// Shift components to the left.
/// Wraps around.
CORE_API vec3 v3_shift_left( vec3 v );
/// Shift components to the right.
/// Wraps around.
CORE_API vec3 v3_shift_right( vec3 v );
/// Square magnitude.
CORE_API f32 v3_sqrmag( vec3 v );
/// Magnitude.
CORE_API f32 v3_mag( vec3 v );
/// Normalize vector.
CORE_API vec3 v3_normalize( vec3 v );
/// Angle(radians) of two vectors.
CORE_API f32 v3_angle( vec3 lhs, vec3 rhs );
/// Linearly interpolate vectors.
CORE_API vec3 v3_lerp( vec3 a, vec3 b, f32 t );
/// Smooth step interpolate vectors.
CORE_API vec3 v3_smooth_step( vec3 a, vec3 b, f32 t );
/// Smoother step interpolate vectors.
CORE_API vec3 v3_smoother_step( vec3 a, vec3 b, f32 t );

/// Negate vector.
CORE_API ivec3 iv3_neg( ivec3 v );
/// Add vectors.
CORE_API ivec3 iv3_add( ivec3 lhs, ivec3 rhs );
/// Sub vectors.
CORE_API ivec3 iv3_sub( ivec3 lhs, ivec3 rhs );
/// Scale vector.
CORE_API ivec3 iv3_mul( ivec3 lhs, i32 rhs );
/// Divide vector.
CORE_API ivec3 iv3_div( ivec3 lhs, i32 rhs );
/// Horizontal add.
/// Add components.
CORE_API i32 iv3_hadd( ivec3 v );
/// Horizontal multiply.
/// Multiply all components.
CORE_API i32 iv3_hmul( ivec3 v );
/// Hadamard product.
/// Component-wise multiplication.
CORE_API ivec3 iv3_hadamard( ivec3 lhs, ivec3 rhs );
/// Dot/inner product.
CORE_API f32 iv3_dot( ivec3 lhs, ivec3 rhs );
/// Compare vectors for equality.
CORE_API b32 iv3_cmp( ivec3 a, ivec3 b );
/// Shift components to the left.
/// Wraps around.
CORE_API ivec3 iv3_shift_left( ivec3 v );
/// Shift components to the right.
/// Wraps around.
CORE_API ivec3 iv3_shift_right( ivec3 v );
/// Square magnitude.
CORE_API f32 iv3_sqrmag( ivec3 v );
/// Magnitude.
CORE_API f32 iv3_mag( ivec3 v );
/// Angle(radians) of two vectors.
CORE_API f32 iv3_angle( ivec3 lhs, ivec3 rhs );

/// Negate vector.
CORE_API vec4 v4_neg( vec4 v );
/// Add vectors.
CORE_API vec4 v4_add( vec4 lhs, vec4 rhs );
/// Sub vectors.
CORE_API vec4 v4_sub( vec4 lhs, vec4 rhs );
/// Scale vector.
CORE_API vec4 v4_mul( vec4 lhs, f32 rhs );
/// Divide vector.
CORE_API vec4 v4_div( vec4 lhs, f32 rhs );
/// Horizontal add.
/// Add components.
CORE_API f32 v4_hadd( vec4 v );
/// Horizontal multiply.
/// Multiply all components.
CORE_API f32 v4_hmul( vec4 v );
/// Hadamard product.
/// Component-wise multiplication.
CORE_API vec4 v4_hadamard( vec4 lhs, vec4 rhs );
/// Dot/inner product.
CORE_API f32 v4_dot( vec4 lhs, vec4 rhs );
/// Clamp vector's magnitude to range min-max.
CORE_API vec4 v4_clamp_mag( vec4 v, f32 min, f32 max );
/// Compare vectors for equality.
CORE_API b32 v4_cmp( vec4 a, vec4 b );
/// Shift components to the left.
/// Wraps around.
CORE_API vec4 v4_shift_left( vec4 v );
/// Shift components to the right.
/// Wraps around.
CORE_API vec4 v4_shift_right( vec4 v );
/// Square magnitude.
CORE_API f32 v4_sqrmag( vec4 v );
/// Magnitude.
CORE_API f32 v4_mag( vec4 v );
/// Normalize vector.
CORE_API vec4 v4_normalize( vec4 v );
/// Angle(radians) of two vectors.
CORE_API f32 v4_angle( vec4 lhs, vec4 rhs );
/// Linearly interpolate vectors.
CORE_API vec4 v4_lerp( vec4 a, vec4 b, f32 t );
/// Smooth step interpolate vectors.
CORE_API vec4 v4_smooth_step( vec4 a, vec4 b, f32 t );
/// Smoother step interpolate vectors.
CORE_API vec4 v4_smoother_step( vec4 a, vec4 b, f32 t );

/// Negate vector.
CORE_API ivec4 iv4_neg( ivec4 v );
/// Add vectors.
CORE_API ivec4 iv4_add( ivec4 lhs, ivec4 rhs );
/// Sub vectors.
CORE_API ivec4 iv4_sub( ivec4 lhs, ivec4 rhs );
/// Scale vector.
CORE_API ivec4 iv4_mul( ivec4 lhs, i32 rhs );
/// Divide vector.
CORE_API ivec4 iv4_div( ivec4 lhs, i32 rhs );
/// Horizontal add.
/// Add components.
CORE_API i32 iv4_hadd( ivec4 v );
/// Horizontal multiply.
/// Multiply all components.
CORE_API i32 iv4_hmul( ivec4 v );
/// Hadamard product.
/// Component-wise multiplication.
CORE_API ivec4 iv4_hadamard( ivec4 lhs, ivec4 rhs );
/// Dot/inner product.
CORE_API f32 iv4_dot( ivec4 lhs, ivec4 rhs );
/// Compare vectors for equality.
CORE_API b32 iv4_cmp( ivec4 a, ivec4 b );
/// Shift components to the left.
/// Wraps around.
CORE_API ivec4 iv4_shift_left( ivec4 v );
/// Shift components to the right.
/// Wraps around.
CORE_API ivec4 iv4_shift_right( ivec4 v );
/// Square magnitude.
CORE_API f32 iv4_sqrmag( ivec4 v );
/// Magnitude.
CORE_API f32 iv4_mag( ivec4 v );

/// Create quaternion from angle(radians) and axis.
CORE_API quat q_angle_axis( f32 angle, vec3 axis );
/// Create quaternion from euler angles.
CORE_API quat q_euler( f32 pitch, f32 yaw, f32 roll );
/// Create quaternion from euler angles.
CORE_API quat q_euler_v3( euler_angles euler );
/// Negate quaternion.
CORE_API quat q_neg( quat q );
/// Add quaternions.
CORE_API quat q_add( quat lhs, quat rhs );
/// Sub quaternions.
CORE_API quat q_sub( quat lhs, quat rhs );
/// Scale quaternion.
CORE_API quat q_mul( quat lhs, f32 rhs );
/// Mul quaternions.
CORE_API quat q_mul_q( quat lhs, quat rhs );
/// Mul quaternion and vector3.
CORE_API vec3 q_mul_v3( quat lhs, vec3 rhs );
/// Div quaternion.
CORE_API quat q_div( quat lhs, f32 rhs );
/// Compare quaternions for equality.
CORE_API b32 q_cmp( quat a, quat b );
/// Square magnitude.
CORE_API f32 q_sqrmag( quat q );
/// Magnitude.
CORE_API f32 q_mag( quat q );
/// Normalize quaternion.
CORE_API quat q_normalize( quat q );
/// Conjugate of quaternion.
CORE_API quat q_conjugate( quat q );
/// Inverse of quaternion.
CORE_API quat q_inverse( quat q );
/// Angle between quaternions.
CORE_API f32 q_angle( quat lhs, quat rhs );
/// Quaternion inner product.
CORE_API f32 q_dot( quat lhs, quat rhs );
/// Linear interpolation.
CORE_API quat q_lerp( quat a, quat b, f32 t );
/// Spherical interpolation.
CORE_API quat q_slerp( quat a, quat b, f32 t );

/// Add matrices.
CORE_API mat2 m2_add( mat2 lhs, mat2 rhs );
/// Sub matrices.
CORE_API mat2 m2_sub( mat2 lhs, mat2 rhs );
/// Scale matrix.
CORE_API mat2 m2_mul( mat2 lhs, f32 rhs );
/// Mul matrices.
CORE_API mat2 m2_mul_m2( mat2 lhs, mat2 rhs );
/// Div matrix.
CORE_API mat2 m2_div( mat2 lhs, f32 rhs );
/// Transpose matrix.
CORE_API mat2 m2_transpose( mat2 m );
/// Determinant of matrix.
CORE_API f32 m2_determinant( mat2 m );

/// Add matrices.
CORE_API mat3 m3_add( const mat3* lhs, const mat3* rhs );
/// Sub matrices.
CORE_API mat3 m3_sub( const mat3* lhs, const mat3* rhs );
/// Scale matrix.
CORE_API mat3 m3_mul( const mat3* lhs, f32 rhs );
/// Mul matrices.
CORE_API mat3 m3_mul_m3( const mat3* lhs, const mat3* rhs );
/// Div matrix.
CORE_API mat3 m3_div( const mat3* lhs, f32 rhs );
/// Transpose matrix.
CORE_API mat3 m3_transpose( const mat3* m );
/// Determinant of matrix.
CORE_API f32 m3_determinant( const mat3* m );

/// Create view matrix.
CORE_API mat4 m4_view( vec3 position, vec3 target, vec3 up );
/// Create 2d view matrix.
CORE_API mat4 m4_view_2d( vec2 position, vec2 up );
/// Create orthographic projection matrix.
CORE_API mat4 m4_ortho(
    f32 left, f32 right,
    f32 bottom, f32 top,
    f32 near_clip, f32 far_clip );
/// Create perspective projection matrix.
CORE_API mat4 m4_perspective(
    f32 fov_radians, f32 aspect_ratio,
    f32 near_clip, f32 far_clip );
/// Create translation matrix.
CORE_API mat4 m4_translation( f32 x, f32 y, f32 z );
/// Create 2d translation matrix.
CORE_API mat4 m4_translation_2d( f32 x, f32 y );
/// Create translation matrix from vector3.
CORE_API mat4 m4_translation_v3( vec3 translation );
/// Create 2d translation matrix from vector2.
CORE_API mat4 m4_translation_2d_v2( vec2 translation );
/// Create rotation matrix around x axis.
CORE_API mat4 m4_rotation_pitch( f32 pitch_radians );
/// Create rotation matrix around y axis.
CORE_API mat4 m4_rotation_yaw( f32 yaw_radians );
/// Create rotation matrix around z axis.
CORE_API mat4 m4_rotation_roll( f32 roll_radians );
/// Create rotation matrix from euler angles.
CORE_API mat4 m4_rotation_euler(
    f32 pitch_radians, f32 yaw_radians, f32 roll_radians );
/// Create rotation matrix from euler angles vector.
CORE_API mat4 m4_rotation_euler_v3( vec3 rotation );
/// Create rotation matrix from quaternion.
CORE_API mat4 m4_rotation_q( quat rotation );
/// Create scale matrix.
CORE_API mat4 m4_scale( f32 width, f32 height, f32 length );
/// Create 2d scale matrix.
CORE_API mat4 m4_scale_2d( f32 width, f32 height );
/// Create scale matrix from vector.
CORE_API mat4 m4_scale_v3( vec3 scale );
/// Create 2d scale matrix from vector.
CORE_API mat4 m4_scale_2d_v2( vec2 scale );
/// Create transform matrix.
CORE_API mat4 m4_transform( vec3 translation, quat rotation, vec3 scale );
/// Create transform matrix using euler angles.
CORE_API mat4 m4_transform_euler( vec3 translation, vec3 rotation, vec3 scale );
/// Create 2d transform matrix.
CORE_API mat4 m4_transform_2d( vec2 translation, f32 rotation_radians, vec2 scale );
/// Add matrices.
CORE_API mat4 m4_add( const mat4* lhs, const mat4* rhs );
/// Sub matrices.
CORE_API mat4 m4_sub( const mat4* lhs, const mat4* rhs );
/// Scale matrix.
CORE_API mat4 m4_mul( const mat4* lhs, f32 rhs );
/// Mul matrices.
CORE_API mat4 m4_mul_m4( const mat4* lhs, const mat4* rhs );
/// Mul matrix vector.
CORE_API vec4 m4_mul_v4( const mat4* lhs, vec4 rhs );
/// Mul matrix vector.
CORE_API vec3 m4_mul_v3( const mat4* lhs, vec3 rhs );
/// Div matrix.
CORE_API mat4 m4_div( const mat4* lhs, f32 rhs );
/// Transpose matrix.
CORE_API mat4 m4_transpose( const mat4* m );
/// Get submatrix at given cell.
CORE_API mat3 m4_submatrix( const mat4* m, usize row, usize column );
/// Get determinant of submatrix at given cell.
CORE_API f32 m4_minor( const mat4* m, usize row, usize column );
/// Get the cofactor of minor at given cell.
CORE_API f32 m4_cofactor( const mat4* m, usize row, usize column );
/// Construct matrix from cofactors.
CORE_API mat4 m4_cofactor_matrix( const mat4* m );
/// Construct adjoint matrix.
CORE_API mat4 m4_adjoint( const mat4* m );
/// Determinant of matrix.
CORE_API f32 m4_determinant( const mat4* m );
/// Attempt to construct inverse matrix.
/// Returns true if successful.
CORE_API b32 m4_inverse( const mat4* m, mat4* out_inverse );
/// Construct inverse matrix.
/// If unsuccessful, cells will be NaN due to division by zero!
CORE_API mat4 m4_inverse_unchecked( const mat4* m );
/// Construct normal matrix.
/// Returns true if successful.
CORE_API b32 m4_normal_matrix( const mat4* m, mat3* out_normal_matrix );
/// Construct normal matrix.
/// If unsuccessful, cells will be NaN!
CORE_API mat3 m4_normal_matrix_unchecked( const mat4* m );
/// Extract position from transform matrix.
CORE_API vec3 m4_transform_position( const mat4* m );

/// Transform.
/// You should never directly modify any
/// of the transform's components!
struct Transform {
    vec3 position;
    quat rotation;
    vec3 scale;
    struct { b8 local_matrix_dirty, world_matrix_dirty, camera_dirty; };

    mat4 local_matrix;
    mat4 world_matrix;
    struct Transform* parent;
};

/// Create a transform.
CORE_API Transform transform_create( vec3 position, quat rotation, vec3 scale );
/// Create a default transform.
header_only Transform transform_zero(void) {
    return transform_create( v3_zero(), QUAT_IDENTITY, VEC3_ONE );
}
/// Create a transform with defaults except for position.
header_only Transform transform_with_position( vec3 position ) {
    return transform_create( position, QUAT_IDENTITY, VEC3_ONE );
}
/// Create a transform with defaults except for rotation.
header_only Transform transform_with_rotation( quat rotation ) {
    return transform_create( VEC3_ZERO, rotation, VEC3_ONE );
}
/// Create a transform with defaults except for scale.
header_only Transform transform_with_scale( vec3 scale ) {
    return transform_create( VEC3_ZERO, QUAT_IDENTITY, scale );
}
CORE_API mat4 transform_local_matrix( Transform* t );
CORE_API mat4 transform_world_matrix( Transform* t );
CORE_API vec3 transform_local_position( Transform* t );
CORE_API vec3 transform_world_position( Transform* t );
CORE_API void transform_set_position( Transform* t, vec3 position );
CORE_API void transform_translate( Transform* t, vec3 translation );
CORE_API quat transform_local_rotation( Transform* t );
CORE_API quat transform_world_rotation( Transform* t );
CORE_API void transform_set_rotation( Transform* t, quat rotation );
CORE_API void transform_rotate( Transform* t, quat rotation );
CORE_API vec3 transform_local_scale( Transform* t );
CORE_API vec3 transform_world_scale( Transform* t );
CORE_API void transform_set_scale( Transform* t, vec3 scale );
CORE_API void transform_scale( Transform* t, vec3 scale );
CORE_API vec3 transform_local_forward( Transform* t );
CORE_API vec3 transform_local_right( Transform* t );
CORE_API vec3 transform_local_up( Transform* t );
CORE_API vec3 transform_world_forward( Transform* t );
CORE_API vec3 transform_world_right( Transform* t );
CORE_API vec3 transform_world_up( Transform* t );

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

#endif /* C++ operator overloads */

#endif /* header guard */
