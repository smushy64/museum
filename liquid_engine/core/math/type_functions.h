#if !defined(CORE_MATH_TYPE_FUNCTIONS_HPP)
#define CORE_MATH_TYPE_FUNCTIONS_HPP
/**
 * Description:  Math functions that require math types
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 08, 2023
*/
#include "defines.h"
#include "functions.h"
#include "types.h"

/// rotate components to the left. for vec2 it just swaps components
FORCE_INLINE vec2 rotate_left( vec2 x ) {
    vec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for vec2 it just swaps components
FORCE_INLINE vec2 rotate_right( vec2 x ) {
    return rotate_left(x);
}
/// sum all components
FORCE_INLINE f32 horizontal_add( vec2 x ) {
    return x.x + x.y;
}
/// multiply all components
FORCE_INLINE f32 horizontal_mul( vec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
FORCE_INLINE vec2 hadamard( vec2 lhs, vec2 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}
/// inner product
FORCE_INLINE f32 dot( vec2 lhs, vec2 rhs ) {
    vec2 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
FORCE_INLINE f32 sqrmag( vec2 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
FORCE_INLINE f32 mag( vec2 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
FORCE_INLINE vec2 normalize( vec2 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? VEC2::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
FORCE_INLINE f32 angle( vec2 a, vec2 b ) {
    return acos( dot( a, b ) );
}
/// linear interpolation
FORCE_INLINE vec2 lerp( vec2 a, vec2 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
FORCE_INLINE vec2 lerp_clamped( vec2 a, vec2 b, f32 t ) {
    return lerp( a, b, clamp01(t) );
}
/// smooth step interpolation
FORCE_INLINE vec2 smooth_step( vec2 a, vec2 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
FORCE_INLINE vec2 smooth_step_clamped( vec2 a, vec2 b, f32 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smoother step interpolation
FORCE_INLINE vec2 smoother_step(
    vec2 a, vec2 b, f32 t
) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
FORCE_INLINE vec2 smoother_step_clamped(
    vec2 a, vec2 b, f32 t
) {
    return smoother_step( a, b, clamp01(t) );
}
/// rotate vector by theta radians
FORCE_INLINE vec2 rotate( vec2 x, f32 theta ) {
    f32 theta_sin = sin( theta );
    f32 theta_cos = cos( theta );
    vec2 a = {  theta_cos, theta_sin };
    vec2 b = { -theta_sin, theta_cos };
    return ( x.x * a ) + ( x.y * b );
}
/// clamp a vector's magnitude
FORCE_INLINE vec2 clamp_mag( vec2 x, f32 max_magnitude ) {
    f32 max       = absolute( max_magnitude );
    f32 magnitude = mag( x );
    if( magnitude > max ) {
        vec2 result = x / magnitude;
        return result * max;
    }
    return x;
}
/// compare two vectors for equality
FORCE_INLINE b32 cmp_eq( vec2 a, vec2 b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}


/// rotate components to the left. for ivec2 it just swaps components
FORCE_INLINE ivec2 rotate_left( ivec2 x ) {
    ivec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for ivec2 it just swaps components
FORCE_INLINE ivec2 rotate_right( ivec2 x ) {
    return rotate_left(x);
}
/// sum all components
FORCE_INLINE i32 horizontal_add( ivec2 x ) {
    return x.x + x.y;
}
/// multiply all components
FORCE_INLINE i32 horizontal_mul( ivec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
FORCE_INLINE ivec2 hadamard( ivec2 lhs, ivec2 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
}
/// inner product
FORCE_INLINE f32 dot( ivec2 lhs, ivec2 rhs ) {
    ivec2 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
FORCE_INLINE f32 sqrmag( ivec2 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
FORCE_INLINE f32 mag( ivec2 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
FORCE_INLINE ivec2 normalize( ivec2 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? IVEC2::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
FORCE_INLINE f32 angle( ivec2 a, ivec2 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
FORCE_INLINE b32 cmp_eq( ivec2 a, ivec2 b ) {
    return a.x == b.x && a.y == b.y;
}

/// Create HSV from hue, saturation and value.
FORCE_INLINE hsv v3_hsv( f32 hue, f32 saturation, f32 value ) {
    return {
        degrees_overflow( hue ),
        clamp01( saturation ),
        clamp01( value )
    };
}

/// convert rgb color to hsv color
FORCE_INLINE hsv rgb_to_hsv( rgb col ) {
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
FORCE_INLINE rgb hsv_to_rgb( hsv col ) {
    
    f32 chroma    = col.value * col.saturation;
    f32 hue       = col.hue / 60.0f;
    i32 hue_index = floor32( hue );

    f32 x = chroma * ( 1.0f - absolute(
        mod( hue, 2.0f ) - 1.0f
    ) );

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
FORCE_INLINE rgb linear_to_srgb( rgb linear ) {
    static const f32 power = 1.0f / 2.2f;
    return {
        pow( linear.r, power ),
        pow( linear.g, power ),
        pow( linear.b, power ),
    };
}
/// convert srgb color space to linear color space
FORCE_INLINE rgb srgb_to_linear( rgb srgb ) {
    return {
        pow( srgb.r, 2.2f ),
        pow( srgb.g, 2.2f ),
        pow( srgb.b, 2.2f ),
    };
}

/// rotate components to the left
FORCE_INLINE vec3 rotate_left( vec3 x ) {
    vec3 result;
    result.x = x.y;
    result.y = x.z;
    result.z = x.x;
    return result;
}
/// rotate components to the right
FORCE_INLINE vec3 rotate_right( vec3 x ) {
    vec3 result;
    result.x = x.z;
    result.y = x.x;
    result.z = x.y;
    return result;
}
/// sum components
FORCE_INLINE f32 horizontal_add( vec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply components
FORCE_INLINE f32 horizontal_mul( vec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
FORCE_INLINE vec3 hadamard( vec3 lhs, vec3 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}
/// calculate square magnitude of vector
FORCE_INLINE f32 sqrmag( vec3 x ) {
    return horizontal_add( hadamard( x, x ) );
}
/// calculate magnitude of vector
FORCE_INLINE f32 mag( vec3 x ) {
    return sqrt( sqrmag( x ) );
}
/// inner product
FORCE_INLINE f32 dot( vec3 lhs, vec3 rhs ) {
    return horizontal_add( hadamard( lhs, rhs ) );
}
FORCE_INLINE vec3 normalize( vec3 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? VEC3::ZERO : x / magnitude;
}
/// cross product
FORCE_INLINE vec3 cross( vec3 lhs, vec3 rhs ) {
    return {
        ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ),
        ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ),
        ( lhs.x * rhs.y ) - ( lhs.y * rhs.x )
    };
}
/// reflect direction across axis of given normal
FORCE_INLINE vec3 reflect( vec3 direction, vec3 normal ) {
    return ( normal - direction ) * (
        2.0f *  dot( direction, normal )
    );
}
/// calculate the angle(radians) between two vectors
FORCE_INLINE f32 angle( vec3 a, vec3 b ) {
    return acos( dot( a, b ) );
}
/// linear interpolation
FORCE_INLINE vec3 lerp( vec3 a, vec3 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
FORCE_INLINE vec3 lerp_clamped( vec3 a, vec3 b, f32 t ) {
    return lerp( a, b, clamp01( t ) );
}
/// smooth step interpolation
FORCE_INLINE vec3 smooth_step( vec3 a, vec3 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
FORCE_INLINE vec3 smooth_step_clamped( vec3 a, vec3 b, f32 t ) {
    return smooth_step( a, b, clamp01( t ) );
}
/// smoother step interpolation
FORCE_INLINE vec3 smoother_step(
    vec3 a, vec3 b, f32 t
) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
FORCE_INLINE vec3 smoother_step_clamped(
    vec3 a, vec3 b, f32 t
) {
    return smoother_step( a, b, clamp01( t ) );
}
/// clamp a vector's magnitude
FORCE_INLINE vec3 clamp_mag( vec3 x, f32 max_magnitude ) {
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
FORCE_INLINE b32 cmp_eq( vec3 a, vec3 b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

/// rotate components to the left
FORCE_INLINE ivec3 rotate_left( ivec3 x ) {
    ivec3 out = x;
    out.x = x.z;
    out.y = x.x;
    out.z = x.y;
    return out;
}
/// rotate components to the right
FORCE_INLINE ivec3 rotate_right( ivec3 x ) {
    ivec3 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.x;
    return out;
}
/// sum all components
FORCE_INLINE i32 horizontal_add( ivec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply all components
FORCE_INLINE i32 horizontal_mul( ivec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
FORCE_INLINE ivec3 hadamard( ivec3 lhs, ivec3 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}
/// inner product
FORCE_INLINE f32 dot( ivec3 lhs, ivec3 rhs ) {
    ivec3 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
FORCE_INLINE f32 sqrmag( ivec3 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
FORCE_INLINE f32 mag( ivec3 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
FORCE_INLINE ivec3 normalize( ivec3 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? IVEC3::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
FORCE_INLINE f32 angle( ivec3 a, ivec3 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
FORCE_INLINE b32 cmp_eq( ivec3 a, ivec3 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/// convert linear color space to srgb color space
FORCE_INLINE rgba linear_to_srgb( rgba linear ) {
    rgba result;
    result.rgb = linear_to_srgb( linear.rgb );
    result.a = linear.a;
    return result;
}
/// convert srgb color space to linear color space
FORCE_INLINE rgba srgb_to_linear( rgba srgb ) {
    rgba result;
    result.rgb = srgb_to_linear( srgb.rgb );
    result.a   = srgb.a;
    return result;
}
/// rotate components to the left
FORCE_INLINE vec4 rotate_left( vec4 x ) {
    vec4 result;
    result.x = x.y;
    result.y = x.z;
    result.z = x.w;
    result.w = x.x;
    return result;
}
/// rotate components to the right
FORCE_INLINE vec4 rotate_right( vec4 x ) {
    vec4 result;
    result.x = x.w;
    result.y = x.x;
    result.z = x.y;
    result.w = x.z;
    return result;
}
/// sum components
FORCE_INLINE f32 horizontal_add( vec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply components
FORCE_INLINE f32 horizontal_mul( vec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
FORCE_INLINE vec4 hadamard( vec4 lhs, vec4 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// calculate square magnitude of vector
FORCE_INLINE f32 sqrmag( vec4 x ) {
    return horizontal_add( hadamard( x, x ) );
}
/// calculate magnitude of vector
FORCE_INLINE f32 mag( vec4 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
FORCE_INLINE vec4 normalize( vec4 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? VEC4::ZERO : x / magnitude;
}
/// inner product
FORCE_INLINE f32 dot( vec4 lhs, vec4 rhs ) {
    return horizontal_add( hadamard( lhs, rhs ) );
}
/// linear interpolation
FORCE_INLINE vec4 lerp( vec4 a, vec4 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
FORCE_INLINE vec4 lerp_clamped( vec4 a, vec4 b, f32 t ) {
    return lerp( a, b, clamp01( t ) );
}
/// smooth step interpolation
FORCE_INLINE vec4 smooth_step( vec4 a, vec4 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
FORCE_INLINE vec4 smooth_step_clamped( vec4 a, vec4 b, f32 t ) {
    return smooth_step( a, b, clamp01( t ) );
}
/// smoother step interpolation
FORCE_INLINE vec4 smoother_step(
    vec4 a, vec4 b, f32 t
) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
FORCE_INLINE vec4 smoother_step_clamped(
    vec4 a, vec4 b, f32 t
) {
    return smoother_step( a, b, clamp01( t ) );
}
/// clamp a vector's magnitude
FORCE_INLINE vec4 clamp_mag( vec4 x, f32 max_magnitude ) {
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
FORCE_INLINE b32 cmp_eq( vec4 a, vec4 b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

/// rotate components to the left
FORCE_INLINE ivec4 rotate_left( ivec4 x ) {
    ivec4 out = x;
    out.x = x.w;
    out.y = x.x;
    out.z = x.y;
    out.w = x.z;
    return out;
}
/// rotate components to the right
FORCE_INLINE ivec4 rotate_right( ivec4 x ) {
    ivec4 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.w;
    out.w = x.x;
    return out;
}
/// sum all components
FORCE_INLINE i32 horizontal_add( ivec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply all components
FORCE_INLINE i32 horizontal_mul( ivec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
FORCE_INLINE ivec4 hadamard( ivec4 lhs, ivec4 rhs ) {
    return {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// inner product
FORCE_INLINE f32 dot( ivec4 lhs, ivec4 rhs ) {
    ivec4 result = hadamard( lhs, rhs );
    return horizontal_add( result );
}
/// calculate square magnitude of vector
FORCE_INLINE f32 sqrmag( ivec4 x ) {
    return dot( x, x );
}
/// calculate the magnitude of vector
FORCE_INLINE f32 mag( ivec4 x ) {
    return sqrt( sqrmag( x ) );
}
/// normalize vector
FORCE_INLINE ivec4 normalize( ivec4 x ) {
    f32 magnitude = mag( x );
    return magnitude < F32::EPSILON ? IVEC4::ZERO : x / magnitude;
}
/// angle(radians) between two vectors
FORCE_INLINE f32 angle( ivec4 a, ivec4 b ) {
    return acos( dot( a, b ) );
}
/// compare two vectors for equality
FORCE_INLINE b32 cmp_eq( ivec4 a, ivec4 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/// Create quaternion from angle and axis.
FORCE_INLINE quat q( f32 theta, vec3 axis ) {
    f32 half_theta = theta / 2.0f;
    f32 sine = sin( half_theta );
    return {
        cos( half_theta ),
        axis.x * sine,
        axis.y * sine,
        axis.z * sine,
    };
}
/// Create quaternion from angle axis.
FORCE_INLINE quat q( angle_axis aa ) {
    return q( aa.theta_radians, aa.axis );
}
/// Create quaternion from euler angles.
FORCE_INLINE quat q( f32 pitch, f32 yaw, f32 roll ) {
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
/// Create quaternion from euler angles.
FORCE_INLINE quat q( euler_angles euler ) {
    return q( euler.pitch, euler.yaw, euler.roll );
}
FORCE_INLINE vec3 operator*( quat lhs, vec3 rhs ) {
    vec3 t = 2.0f * cross( lhs.xyz, rhs );
    return rhs + t * lhs.w + cross( lhs.xyz, t );
}

/// calculate square magnitude of quaternion
FORCE_INLINE f32 sqrmag( quat q ) {
    vec4 result = { q.w, q.x, q.y, q.z };
    return horizontal_add( hadamard( result, result ) );
}
/// calculate magnitude of quaternion
FORCE_INLINE f32 mag( quat q ) {
    return sqrt( sqrmag( q ) );
}
/// normalize quaternion
FORCE_INLINE quat normalize( quat q ) {
    f32 magnitude = mag( q );
    return magnitude < F32::EPSILON ? QUAT::IDENTITY : q / magnitude;
}
/// conjugate of quaternion
FORCE_INLINE quat conjugate( quat q ) {
    return { q.w, -q.x, -q.y, -q.z };
}
/// calculate inverse quaternion
FORCE_INLINE quat inverse( quat q ) {
    return conjugate( q ) / sqrmag( q );
}
/// angle between quaternions
FORCE_INLINE f32 angle( quat a, quat b ) {
    quat qd = inverse( a ) * b;
    return 2.0f * atan2( mag( qd.xyz ), qd.w );
}
/// inner product
FORCE_INLINE f32 dot( quat lhs, quat rhs ) {
    vec4 l = { lhs.w, lhs.x, lhs.y, lhs.z };
    vec4 r = { rhs.w, rhs.x, rhs.y, rhs.z };
    return horizontal_add( hadamard( l, r ) );
}
/// linear interpolation
FORCE_INLINE quat lerp( quat a, quat b, f32 t ) {
    return normalize( ( 1.0f - t ) * a + b * t );
}
/// linear interpolation, t is clamped 0-1
FORCE_INLINE quat lerp_clamped( quat a, quat b, f32 t ) {
    return lerp( a, b, clamp01( t ) );
}
/// spherical linear interpolation
FORCE_INLINE quat slerp( quat a, quat b, f32 t ) {
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
            ( sin( ( 1.0f - t ) * theta ) * a +
            sin( t * theta ) * _b )
            / sin( theta )
        );
    }
}
/// compare quaternions for equality
FORCE_INLINE b32 cmp_eq( quat a, quat b ) {
    return sqrmag( a - b ) < F32::EPSILON;
}

/// transpose matrix
FORCE_INLINE mat2 transpose( mat2 m ) {
    return {
        m.c[0], m.c[2],
        m.c[1], m.c[3]
    };
}
/// calculate determinant
FORCE_INLINE f32 determinant( mat2 m ) {
    return ( m.c[0] * m.c[3] ) - ( m.c[2] * m.c[1] );
}

/// transpose matrix
FORCE_INLINE mat3 transpose( const mat3& m ) {
    return {
        m.c[0], m.c[3], m.c[6],
        m.c[1], m.c[4], m.c[7],
        m.c[2], m.c[5], m.c[8]
    };
}
/// calculate determinant
FORCE_INLINE f32 determinant( const mat3& m ) {
    return
     ( m.c[0] * ( ( m.c[4] * m.c[8] ) - ( m.c[7] * m.c[5] ) ) ) +
    -( m.c[3] * ( ( m.c[1] * m.c[8] ) - ( m.c[7] * m.c[2] ) ) ) +
     ( m.c[6] * ( ( m.c[1] * m.c[5] ) - ( m.c[4] * m.c[2] ) ) );
}
/// create new look at matrix
FORCE_INLINE mat4 lookat(
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
FORCE_INLINE mat4 ortho(
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
FORCE_INLINE mat4 ortho(
    f32 left, f32 right,
    f32 bottom, f32 top
) {
    return ortho( left, right, bottom, top, -1.0f, 1.0f );
}
/// create orthographic matrix for 2D rendering
FORCE_INLINE mat4 projection2d( f32 aspect_ratio, f32 scale ) {
    f32 scaled_aspect = aspect_ratio * scale;
    return ortho(
        -scaled_aspect, scaled_aspect,
        -scale, scale
    );
}
/// create perspective matrix
FORCE_INLINE mat4 perspective(
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
FORCE_INLINE mat4 translate( f32 x, f32 y, f32 z ) {
    return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f,
    };
}
/// create new translation matrix
FORCE_INLINE mat4 translate( vec3 translation ) {
    return translate(
        translation.x,
        translation.y,
        translation.z
    );
}
/// create new 2d translation matrix
FORCE_INLINE mat4 translate( f32 x, f32 y ) {
    return translate( x, y, 0.0f );
}
/// create new 2d translation matrix
FORCE_INLINE mat4 translate( vec2 translation ) {
    return translate( translation.x, translation.y );
}
/// create rotation matrix around x axis
FORCE_INLINE mat4 rotate_pitch( f32 pitch ) {
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
FORCE_INLINE mat4 rotate_yaw( f32 yaw ) {
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
FORCE_INLINE mat4 rotate_roll( f32 roll ) {
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
FORCE_INLINE mat4 rotate( euler_angles r ) {
    return rotate_pitch( r.pitch ) *
        rotate_yaw( r.yaw ) *
        rotate_roll( r.roll );
}
/// create rotation matrix from quaternion
FORCE_INLINE mat4 rotate( quat q ) {
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
FORCE_INLINE mat4 rotate( f32 theta ) {
    return rotate_roll( theta );
}
/// create scale matrix
FORCE_INLINE mat4 scale( f32 width, f32 height, f32 length ) {
    return {
        width,   0.0f,   0.0f, 0.0f,
         0.0f, height,   0.0f, 0.0f,
         0.0f,   0.0f, length, 0.0f,
         0.0f,   0.0f,   0.0f, 1.0f
    };
}
/// create scale matrix
FORCE_INLINE mat4 scale( vec3 scale ) {
    return ::scale(
        scale.width,
        scale.height,
        scale.length
    );
}
/// create scale matrix for 2D
FORCE_INLINE mat4 scale( f32 width, f32 height ) {
    return scale( width, height, 1.0f );
}
/// create scale matrix for 2D
FORCE_INLINE mat4 scale( vec2 scale ) {
    return ::scale( scale.width, scale.height );
}
/// create transform matrix
FORCE_INLINE mat4 transform(
    vec3         translation,
    euler_angles rotation,
    vec3         scale
) {
    return translate( translation ) *
        rotate( rotation ) *
        ::scale( scale );
}
/// create transform matrix
FORCE_INLINE mat4 transform(
    vec3 translation,
    quat rotation,
    vec3 scale
) {
    return translate( translation ) *
        rotate( rotation ) *
        ::scale( scale );
}
/// create transform matrix for 2D
FORCE_INLINE mat4 transform(
    vec2 translation,
    f32  rotation,
    vec2 scale
) {
    return translate( translation ) *
        rotate( rotation ) *
        ::scale( scale );
}
/// transpose matrix
FORCE_INLINE mat4 transpose( const mat4& m ) {
    return {
        m.c[0], m.c[4], m.c[ 8], m.c[12],
        m.c[1], m.c[5], m.c[ 9], m.c[13],
        m.c[2], m.c[6], m.c[10], m.c[14],
        m.c[3], m.c[7], m.c[11], m.c[15]
    };
}
/// get submatrix at given coordinates
FORCE_INLINE mat3 submatrix( const mat4& m, u32 row, u32 col ) {
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
FORCE_INLINE f32 minor( const mat4& m, u32 row, u32 col ) {
    return determinant( submatrix( m, row, col ) );
}
/// calculate the cofactor of minor at given coordinates
FORCE_INLINE f32 cofactor( const mat4& m, u32 row, u32 col ) {
    f32 minor = ::minor( m, row, col );
    return minor * powi(
        -1.0f,
        ( row + 1 ) + ( col + 1 )
    );
}
/// construct matrix from cofactors
FORCE_INLINE mat4 cofactor_matrix( const mat4& m ) {
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
FORCE_INLINE mat4 adjoint( const mat4& m ) {
    return transpose( cofactor_matrix( m ) );
}
/// calculate the determinant
FORCE_INLINE f32 determinant( const mat4& m ) {
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
FORCE_INLINE b32 inverse( const mat4& m, mat4* out_matrix ) {
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
FORCE_INLINE mat4 inverse_unchecked( const mat4& m ) {
    return adjoint( m ) / determinant( m );
}

/// convert rgb to rgba
FORCE_INLINE rgba to_rgba( rgb rgb ) {
    rgba result;
    result.rgb = rgb;
    result.a   = 1.0f;
    return result;
}

/// convert quaternion to euler angles
FORCE_INLINE euler_angles to_euler( quat q ) {
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
FORCE_INLINE angle_axis to_angle_axis( quat q ) {
    f32 inv_w2_sqrt = sqrt( 1.0f - ( q.w * q.w ) );
    return {
        q.xyz / inv_w2_sqrt,
        2.0f * acos( q.w )
    };
}

FORCE_INLINE vec2 v2( vec3 v ) { return v.xy; }
FORCE_INLINE vec2 v2( vec4 v ) { return v.xy; }
FORCE_INLINE vec2 v2( ivec2 v ) {
    return { (f32)v.x, (f32)v.y };
}

FORCE_INLINE ivec2 trunc32( vec2 v ) {
    return { trunc32(v.x), trunc32(v.y) };
}
FORCE_INLINE ivec2 floor32( vec2 v ) {
    return { floor32(v.x), floor32(v.y) };
}
FORCE_INLINE ivec2 ceil32( vec2 v ) {
    return { ceil32(v.x), ceil32(v.y) };
}
FORCE_INLINE ivec2 round32( vec2 v ) {
    return { round32(v.x), round32(v.y) };
}

FORCE_INLINE ivec2 iv2( ivec3 v ) { return v.xy; }
FORCE_INLINE ivec2 iv2( ivec4 v ) { return v.xy; }
FORCE_INLINE ivec2 iv2( vec2 v )  { return trunc32( v ); }

FORCE_INLINE vec3 v3( vec2 v ) { return { v.x, v.y, 0.0f }; }
FORCE_INLINE vec3 v3( vec4 v ) { return v.xyz; }
FORCE_INLINE vec3 v3( ivec3 v ) {
    return { (f32)v.x, (f32)v.y, (f32)v.z };
}

FORCE_INLINE ivec3 trunc32( vec3 v ) {
    return { trunc32(v.x), trunc32(v.y), trunc32(v.z) };
}
FORCE_INLINE ivec3 floor32( vec3 v ) {
    return { floor32(v.x), floor32(v.y), floor32(v.z) };
}
FORCE_INLINE ivec3 ceil32( vec3 v ) {
    return { ceil32(v.x), ceil32(v.y), ceil32(v.z) };
}
FORCE_INLINE ivec3 round32( vec3 v ) {
    return { round32(v.x), round32(v.y), round32(v.z) };
}

FORCE_INLINE ivec3 iv3( ivec2 v ) { return { v.x, v.y, 0 }; }
FORCE_INLINE ivec3 iv3( ivec4 v ) { return v.xyz; }
FORCE_INLINE ivec3 iv3( vec3 v )  { return trunc32( v ); }

FORCE_INLINE vec4 v4( vec2 v ) { return { v.x, v.y, 0.0f, 0.0f }; }
FORCE_INLINE vec4 v4( vec3 v ) { return { v.x, v.y,  v.z, 0.0f }; }
FORCE_INLINE vec4 v4( ivec4 v ) {
    return { (f32)v.x, (f32)v.y, (f32)v.z, (f32)v.w };
}

FORCE_INLINE ivec4 trunc32( vec4 v ) {
    return { trunc32(v.x), trunc32(v.y), trunc32(v.z), trunc32(v.w) };
}
FORCE_INLINE ivec4 floor32( vec4 v ) {
    return { floor32(v.x), floor32(v.y), floor32(v.z), floor32(v.w) };
}
FORCE_INLINE ivec4 ceil32( vec4 v ) {
    return { ceil32(v.x), ceil32(v.y), ceil32(v.z), ceil32(v.w) };
}
FORCE_INLINE ivec4 round32( vec4 v ) {
    return { round32(v.x), round32(v.y), round32(v.z), round32(v.w) };
}

FORCE_INLINE ivec4 iv4( ivec2 v ) { return { v.x, v.y,   0, 0 }; }
FORCE_INLINE ivec4 iv4( ivec3 v ) { return { v.x, v.y, v.z, 0 }; }
FORCE_INLINE ivec4 iv4( vec4 v )  { return trunc32( v ); }

FORCE_INLINE mat3 m3( mat2 m ) {
    return {
        m.c[0], m.c[1], 0.0f,
        m.c[2], m.c[3], 0.0f,
          0.0f,   0.0f, 0.0f
    };
}
FORCE_INLINE mat3 m3( const mat4& m ) {
    return {
        m.c[0], m.c[1], m.c[ 2],
        m.c[4], m.c[5], m.c[ 6],
        m.c[8], m.c[9], m.c[10]
    };
}

FORCE_INLINE mat4 m4( mat2 m ) {
    return {
        m.c[0], m.c[1], 0.0f, 0.0f,
        m.c[2], m.c[3], 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
    };
}
FORCE_INLINE mat4 m4( const mat3& m ) {
    return {
        m.c[0], m.c[1], m.c[2], 0.0f,
        m.c[3], m.c[4], m.c[5], 0.0f,
        m.c[6], m.c[7], m.c[8], 0.0f,
          0.0f,   0.0f,   0.0f, 0.0f,
    };
}

// VEC2/MAT4 ----------------------------------------------------

/// create look at matrix for 2D rendering
FORCE_INLINE mat4 lookat( vec2 position ) {
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
FORCE_INLINE b32 normal_matrix( const mat4& m, mat3* out_matrix ) {
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
FORCE_INLINE mat3 normal_matrix_unchecked( const mat4& m ) {
    return m3( transpose( inverse_unchecked( m ) ) );
}

#if LD_SIMD_WIDTH == 1

FORCE_INLINE
quat operator*( quat lhs, quat rhs ) {
    return {
        ( lhs[0] * rhs[0] ) -
            ( lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3] ),
        ( lhs[0] * rhs[1] ) +
            ( lhs[1] * rhs[0] ) +
            ( ( lhs[2] * rhs[3] ) -
            ( lhs[3] * rhs[2] ) ),
        ( lhs[0] * rhs[2] ) +
            ( lhs[2] * rhs[0] ) +
            ( ( lhs[3] * rhs[1] ) -
            ( lhs[1] * rhs[3] ) ),
        ( lhs[0] * rhs[3] ) +
            ( lhs[3] * rhs[0] ) +
            ( ( lhs[1] * rhs[2] ) -
            ( lhs[2] * rhs[1] ) ),
    };
}

FORCE_INLINE
mat4 operator+( const mat4& lhs, const mat4& rhs ) {
    mat4 result;
    result.col0 = lhs.col0 + rhs.col0;
    result.col1 = lhs.col1 + rhs.col1;
    result.col2 = lhs.col2 + rhs.col2;
    result.col3 = lhs.col3 + rhs.col3;
    return result;
}
FORCE_INLINE
mat4 operator-( const mat4& lhs, const mat4& rhs ) {
    mat4 result;
    result.col0 = lhs.col0 - rhs.col0;
    result.col1 = lhs.col1 - rhs.col1;
    result.col2 = lhs.col2 - rhs.col2;
    result.col3 = lhs.col3 - rhs.col3;
    return result;
}
FORCE_INLINE
mat4 operator*( const mat4& lhs, f32 rhs ) {
    mat4 result;
    result.col0 = lhs.col0 * rhs;
    result.col1 = lhs.col1 * rhs;
    result.col2 = lhs.col2 * rhs;
    result.col3 = lhs.col3 * rhs;
    return result;
}
FORCE_INLINE
mat4 operator/( const mat4& lhs, f32 rhs ) {
    mat4 result;
    result.col0 = lhs.col0 / rhs;
    result.col1 = lhs.col1 / rhs;
    result.col2 = lhs.col2 / rhs;
    result.col3 = lhs.col3 / rhs;
    return result;
}
FORCE_INLINE
mat4 operator*( const mat4& lhs, const mat4& rhs ) {
    return {
        // column - 0
        ( lhs.c[0] * rhs.c[0] )      +
            ( lhs.c[4]  * rhs.c[1] ) +
            ( lhs.c[8]  * rhs.c[2] ) +
            ( lhs.c[12] * rhs.c[3] ),
        ( lhs.c[1] * rhs.c[0] )      +
            ( lhs.c[5]  * rhs.c[1] ) +
            ( lhs.c[9]  * rhs.c[2] ) +
            ( lhs.c[13] * rhs.c[3] ),
        ( lhs.c[2] * rhs.c[0] )      +
            ( lhs.c[6]  * rhs.c[1] ) +
            ( lhs.c[10] * rhs.c[2] ) +
            ( lhs.c[14] * rhs.c[3] ),
        ( lhs.c[3] * rhs.c[0] )      +
            ( lhs.c[7]  * rhs.c[1] ) +
            ( lhs.c[11] * rhs.c[2] ) +
            ( lhs.c[15] * rhs.c[3] ),
        // column - 1
        ( lhs.c[0] * rhs.c[4] )      +
            ( lhs.c[4]  * rhs.c[5] ) +
            ( lhs.c[8]  * rhs.c[6] ) +
            ( lhs.c[12] * rhs.c[7] ),
        ( lhs.c[1] * rhs.c[4] )      +
            ( lhs.c[5]  * rhs.c[5] ) +
            ( lhs.c[9]  * rhs.c[6] ) +
            ( lhs.c[13] * rhs.c[7] ),
        ( lhs.c[2] * rhs.c[4] )      +
            ( lhs.c[6]  * rhs.c[5] ) +
            ( lhs.c[10] * rhs.c[6] ) +
            ( lhs.c[14] * rhs.c[7] ),
        ( lhs.c[3] * rhs.c[4] )      +
            ( lhs.c[7]  * rhs.c[5] ) +
            ( lhs.c[11] * rhs.c[6] ) +
            ( lhs.c[15] * rhs.c[7] ),
        // column - 2
        ( lhs.c[0] * rhs.c[8] )       +
            ( lhs.c[4]  * rhs.c[9] )  +
            ( lhs.c[8]  * rhs.c[10] ) +
            ( lhs.c[12] * rhs.c[11] ),
        ( lhs.c[1] * rhs.c[8] )       +
            ( lhs.c[5]  * rhs.c[9] )  +
            ( lhs.c[9]  * rhs.c[10] ) +
            ( lhs.c[13] * rhs.c[11] ),
        ( lhs.c[2] * rhs.c[8] )       +
            ( lhs.c[6]  * rhs.c[9] )  +
            ( lhs.c[10] * rhs.c[10] ) +
            ( lhs.c[14] * rhs.c[11] ),
        ( lhs.c[3] * rhs.c[8] )       +
            ( lhs.c[7]  * rhs.c[9] )  +
            ( lhs.c[11] * rhs.c[10] ) +
            ( lhs.c[15] * rhs.c[11] ),
        // column - 3
        ( lhs.c[0] * rhs.c[12] )      +
            ( lhs.c[4]  * rhs.c[13] ) +
            ( lhs.c[8]  * rhs.c[14] ) +
            ( lhs.c[12] * rhs.c[15] ),
        ( lhs.c[1] * rhs.c[12] )      +
            ( lhs.c[5]  * rhs.c[13] ) +
            ( lhs.c[9]  * rhs.c[14] ) +
            ( lhs.c[13] * rhs.c[15] ),
        ( lhs.c[2] * rhs.c[12] )      +
            ( lhs.c[6]  * rhs.c[13] ) +
            ( lhs.c[10] * rhs.c[14] ) +
            ( lhs.c[14] * rhs.c[15] ),
        ( lhs.c[3] * rhs.c[12] )      +
            ( lhs.c[7]  * rhs.c[13] ) +
            ( lhs.c[11] * rhs.c[14] ) +
            ( lhs.c[15] * rhs.c[15] ),
    };
}

#endif // scalar

#if LD_SIMD_WIDTH >= 4

#if defined( LD_ARCH_X86 )

global inline const __m128 negate_first128 = _mm_setr_ps(
    -1.0f,
    1.0f,
    1.0f,
    1.0f
);

FORCE_INLINE
quat operator*( quat lhs, quat rhs ) {
    __m128 a = _mm_setr_ps( lhs.x, lhs.x, lhs.y, lhs.z );
    __m128 b = _mm_setr_ps( rhs.x, rhs.w, rhs.w, rhs.w );
    a = _mm_mul_ps( a, b );

    b = _mm_setr_ps( lhs.y, lhs.y, lhs.z, lhs.x );
    __m128 c = _mm_setr_ps( rhs.y, rhs.z, rhs.x, rhs.y );
    b = _mm_mul_ps( b, c );
    
    c = _mm_setr_ps( lhs.z, lhs.z, lhs.x, lhs.y );
    __m128 d = _mm_setr_ps( rhs.z, rhs.y, rhs.z, rhs.x );
    c = _mm_mul_ps( c, d );

    c = _mm_mul_ps( negate_first128, c );

    d = _mm_sub_ps( b, c );
    a = _mm_add_ps( a, d );

    a = _mm_mul_ps( negate_first128, a );

    __m128 w = _mm_set1_ps( lhs.w );
    b = _mm_setr_ps( rhs.w, rhs.x, rhs.y, rhs.z );
    w = _mm_mul_ps( w, b );

    quat result;
    _mm_storeu_ps( value_pointer(result), _mm_add_ps( w, a ) );
    return result;
}

FORCE_INLINE
mat4 operator+( const mat4& lhs, const mat4& rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs0, rhs1, rhs2, rhs3;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs0 = _mm_loadu_ps( value_pointer( rhs.col0 ) );
    rhs1 = _mm_loadu_ps( value_pointer( rhs.col1 ) );
    rhs2 = _mm_loadu_ps( value_pointer( rhs.col2 ) );
    rhs3 = _mm_loadu_ps( value_pointer( rhs.col3 ) );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_add_ps( lhs0, rhs0 ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_add_ps( lhs1, rhs1 ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_add_ps( lhs2, rhs2 ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_add_ps( lhs3, rhs3 ) );

    return result;
}
FORCE_INLINE
mat4 operator-( const mat4& lhs, const mat4& rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs0, rhs1, rhs2, rhs3;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs0 = _mm_loadu_ps( value_pointer( rhs.col0 ) );
    rhs1 = _mm_loadu_ps( value_pointer( rhs.col1 ) );
    rhs2 = _mm_loadu_ps( value_pointer( rhs.col2 ) );
    rhs3 = _mm_loadu_ps( value_pointer( rhs.col3 ) );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_sub_ps( lhs0, rhs0 ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_sub_ps( lhs1, rhs1 ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_sub_ps( lhs2, rhs2 ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_sub_ps( lhs3, rhs3 ) );

    return result;
}
FORCE_INLINE
mat4 operator*( const mat4& lhs, f32 rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs_;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs_ = _mm_set1_ps( rhs );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_mul_ps( lhs0, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_mul_ps( lhs1, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_mul_ps( lhs2, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_mul_ps( lhs3, rhs_ ) );

    return result;
}
FORCE_INLINE
mat4 operator/( const mat4& lhs, f32 rhs ) {
    __m128 lhs0, lhs1, lhs2, lhs3;
    __m128 rhs_;

    lhs0 = _mm_loadu_ps( value_pointer( lhs.col0 ) );
    lhs1 = _mm_loadu_ps( value_pointer( lhs.col1 ) );
    lhs2 = _mm_loadu_ps( value_pointer( lhs.col2 ) );
    lhs3 = _mm_loadu_ps( value_pointer( lhs.col3 ) );

    rhs_ = _mm_set1_ps( rhs );

    mat4 result;

    _mm_storeu_ps( value_pointer(result.col0), _mm_div_ps( lhs0, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col1), _mm_div_ps( lhs1, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col2), _mm_div_ps( lhs2, rhs_ ) );
    _mm_storeu_ps( value_pointer(result.col3), _mm_div_ps( lhs3, rhs_ ) );

    return result;
}
FORCE_INLINE
mat4 operator*( const mat4& lhs, const mat4& rhs ) {
    mat4 result = {};

    __m128 a, b, c;

    u32 col = 0;
    u32 row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    col = 1;
    row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    col = 2;
    row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    col = 3;
    row = 0;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 1;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 2;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    row = 3;
    a = _mm_loadu_ps( &lhs.c[row * MAT4::COLUMN_COUNT] );
    b = _mm_mul_ps( a, _mm_set1_ps( rhs.c[(col * MAT4::COLUMN_COUNT) + row] ) );
    c = _mm_loadu_ps( &result.c[ col * MAT4::COLUMN_COUNT ] );
    _mm_storeu_ps( &result.c[col * MAT4::COLUMN_COUNT], _mm_add_ps( b, c ) );

    return result;
}

#endif // x86 SSE 

#endif // 4-wide SIMD

#endif // header guard
