#if !defined(CORE_MATH_TYPE_FUNCTIONS_HPP)
#define CORE_MATH_TYPE_FUNCTIONS_HPP
/**
 * Description:  Math functions that require math types
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 08, 2023
*/
#include "defines.h"
#include "core/ldmath/functions.h"
#include "core/ldmath/types.h"
#include "core/ldmath/simd.h"

/// Negate vector
header_only vec2 v2_neg( vec2 v ) {
    return (vec2){ -v.x, -v.y };
}
/// Add two vectors
header_only vec2 v2_add( vec2 lhs, vec2 rhs ) {
    vec2 result = { lhs.x + rhs.x, lhs.y + rhs.y };
    return result;
}
/// Sub two vectors
header_only vec2 v2_sub( vec2 lhs, vec2 rhs ) {
    vec2 result = { lhs.x - rhs.x, lhs.y - rhs.y };
    return result;
}
/// Mul vector
header_only vec2 v2_mul( vec2 lhs, f32 rhs ) {
    vec2 result = { lhs.x * rhs, lhs.y * rhs };
    return result;
}
/// Div vector
header_only vec2 v2_div( vec2 lhs, f32 rhs ) {
    vec2 result = { lhs.x / rhs, lhs.y / rhs };
    return result;
}
/// rotate components to the left. for vec2 it just swaps components
header_only vec2 v2_rotate_left( vec2 x ) {
    vec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for vec2 it just swaps components
header_only vec2 v2_rotate_right( vec2 x ) {
    return v2_rotate_left(x);
}
/// sum all components
header_only f32 v2_horizontal_add( vec2 x ) {
    return x.x + x.y;
}
/// multiply all components
header_only f32 v2_horizontal_mul( vec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
header_only vec2 v2_hadamard( vec2 lhs, vec2 rhs ) {
    vec2 result = {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
    return result;
}
/// inner product
header_only f32 v2_dot( vec2 lhs, vec2 rhs ) {
    vec2 result = v2_hadamard( lhs, rhs );
    return v2_horizontal_add( result );
}
/// calculate square magnitude of vector
header_only f32 v2_sqrmag( vec2 x ) {
    return v2_dot( x, x );
}
/// calculate the magnitude of vector
header_only f32 v2_mag( vec2 x ) {
    return sqrt32( v2_sqrmag( x ) );
}
/// normalize vector
header_only vec2 v2_normalize( vec2 x ) {
    f32 magnitude = v2_mag( x );
    const vec2 ZERO = VEC2_ZERO;
    return magnitude < F32_EPSILON ? ZERO : v2_div( x, magnitude );
}
/// angle(radians) between two vectors
header_only f32 v2_angle( vec2 a, vec2 b ) {
    return acos32( v2_dot( a, b ) );
}
/// linear interpolation
header_only vec2 v2_lerp( vec2 a, vec2 b, f32 t ) {
    return
        v2_add(
            v2_mul( a, 1.0f - t ),
            v2_mul( b, t )
        );
    // return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
header_only vec2 v2_lerp_clamped( vec2 a, vec2 b, f32 t ) {
    return v2_lerp( a, b, clamp01(t) );
}
/// smooth step interpolation
header_only vec2 v2_smooth_step( vec2 a, vec2 b, f32 t ) {
    return
        v2_add(
            v2_mul(
                v2_mul(
                    v2_sub( b, a ),
                    ( 3.0f - t * 2.0f )
                ),
                t * t
            ),
            a
        );
    // return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
header_only vec2 v2_smooth_step_clamped( vec2 a, vec2 b, f32 t ) {
    return v2_smooth_step( a, b, clamp01(t) );
}
/// smoother step interpolation
header_only vec2 v2_smoother_step( vec2 a, vec2 b, f32 t ) {
    return
        v2_add(
            v2_mul(
                v2_sub( b, a ),
                ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t
            ),
            a
        );
    // return ( b - a ) *
    //     ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
header_only vec2 v2_smoother_step_clamped(
    vec2 a, vec2 b, f32 t
) {
    return v2_smoother_step( a, b, clamp01(t) );
}
/// rotate vector by theta radians
header_only vec2 v2_rotate( vec2 x, f32 theta ) {
    f32 theta_sin = sin32( theta );
    f32 theta_cos = cos32( theta );
    vec2 a = {  theta_cos, theta_sin };
    vec2 b = { -theta_sin, theta_cos };

    a = v2_mul( a, x.x );
    b = v2_mul( b, x.y );

    vec2 result = v2_add( a, b );

    return result;
}
/// clamp a vector's magnitude
header_only vec2 v2_clamp_mag( vec2 x, f32 max_magnitude ) {
    f32 max       = absof( max_magnitude );
    f32 magnitude = v2_mag( x );
    if( magnitude > max ) {
        vec2 result = v2_div( x, magnitude );
        result = v2_mul( result, max );
        return result;
    }
    return x;
}
/// compare two vectors for equality
header_only b32 v2_cmp_eq( vec2 a, vec2 b ) {
    return v2_sqrmag( v2_sub( a, b ) ) < F32_EPSILON;
}

/// Negate vector
header_only ivec2 iv2_neg( ivec2 v ) {
    return (ivec2){ -v.x, -v.y };
}
/// Add two vectors
header_only ivec2 iv2_add( ivec2 lhs, ivec2 rhs ) {
    ivec2 result = { lhs.x + rhs.x, lhs.y + rhs.y };
    return result;
}
/// Sub two vectors
header_only ivec2 iv2_sub( ivec2 lhs, ivec2 rhs ) {
    ivec2 result = { lhs.x - rhs.x, lhs.y - rhs.y };
    return result;
}
/// Mul vector
header_only ivec2 iv2_mul( ivec2 lhs, i32 rhs ) {
    ivec2 result = { lhs.x * rhs, lhs.y * rhs };
    return result;
}
/// Div vector
header_only ivec2 iv2_div( ivec2 lhs, i32 rhs ) {
    ivec2 result = { lhs.x / rhs, lhs.y / rhs };
    return result;
}
/// rotate components to the left. for ivec2 it just swaps components
header_only ivec2 iv2_rotate_left( ivec2 x ) {
    ivec2 out = x;
    out.x = x.y;
    out.y = x.x;
    return out;
}
/// rotate components to the right. for ivec2 it just swaps components
header_only ivec2 iv2_rotate_right( ivec2 x ) {
    return iv2_rotate_left(x);
}
/// sum all components
header_only i32 iv2_horizontal_add( ivec2 x ) {
    return x.x + x.y;
}
/// multiply all components
header_only i32 iv2_horizontal_mul( ivec2 x ) {
    return x.x * x.y;
}
/// component-wise multiplication
header_only ivec2 iv2_hadamard( ivec2 lhs, ivec2 rhs ) {
    ivec2 result = {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    };
    return result;
}
/// inner product
header_only f32 iv2_dot( ivec2 lhs, ivec2 rhs ) {
    ivec2 result = iv2_hadamard( lhs, rhs );
    return iv2_horizontal_add( result );
}
/// calculate square magnitude of vector
header_only f32 iv2_sqrmag( ivec2 x ) {
    return iv2_dot( x, x );
}
/// calculate the magnitude of vector
header_only f32 iv2_mag( ivec2 x ) {
    return sqrt32( iv2_sqrmag( x ) );

}
/// normalize vector
header_only ivec2 iv2_normalize( ivec2 x ) {
    f32 magnitude = iv2_mag( x );
    const ivec2 ZERO = IVEC2_ZERO;
    return magnitude < F32_EPSILON ?
        ZERO :
        iv2_div( x, magnitude );
}
/// angle(radians) between two vectors
header_only f32 iv2_angle( ivec2 a, ivec2 b ) {
    return acos32( iv2_dot( a, b ) );
}
/// compare two vectors for equality
header_only b32 iv2_cmp_eq( ivec2 a, ivec2 b ) {
    return a.x == b.x && a.y == b.y;
}

/// Negate vector
header_only vec3 v3_neg( vec3 v ) {
    return (vec3){ -v.x, -v.y, -v.z };
}
/// Add two vectors
header_only vec3 v3_add( vec3 lhs, vec3 rhs ) {
    vec3 result = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    return result;
}
/// Sub two vectors
header_only vec3 v3_sub( vec3 lhs, vec3 rhs ) {
    vec3 result = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    return result;
}
/// Mul vector
header_only vec3 v3_mul( vec3 lhs, f32 rhs ) {
    vec3 result = { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
    return result;
}
/// Div vector
header_only vec3 v3_div( vec3 lhs, f32 rhs ) {
    vec3 result = { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
    return result;
}
/// Create HSV from hue, saturation and value.
header_only hsv v3_hsv( f32 hue, f32 saturation, f32 value ) {
    hsv result = {
        wrap_degrees32( hue ),
        (f32)clamp01( saturation ),
        (f32)clamp01( value )
    };
    return result;
}
/// convert rgb color to hsv color
header_only hsv rgb_to_hsv( rgb col ) {
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
header_only rgb hsv_to_rgb( hsv col ) {
    
    f32 chroma    = col.value * col.saturation;
    f32 hue       = col.hue / 60.0f;
    i32 hue_index = floor_i32( hue );

    f32 x = chroma * ( 1.0f - absof( mod32( hue, 2.0f ) - 1.0f ) );

    rgb result = {};

    if( hue_index < 1 ) {
        result.r = chroma;
        result.g = x;
    } else if( hue_index < 2 ) {
        result.r = x;
        result.g = chroma;
    } else if( hue_index < 3 ) {
        result.g = chroma;
        result.b = x;
    } else if( hue_index < 4 ) {
        result.g = x;
        result.b = chroma;
    } else if( hue_index < 5 ) {
        result.r = x;
        result.b = chroma;
    } else {
        result.r = chroma;
        result.b = x;
    }
    
    f32 m = col.value - chroma;

    result.r += m;
    result.g += m;
    result.b += m;

    return result;
}
/// convert linear color space to srgb color space
header_only rgb linear_to_srgb( rgb linear ) {
    const f32 power = 1.0f / 2.2f;
    rgb result = {
        pow32( linear.r, power ),
        pow32( linear.g, power ),
        pow32( linear.b, power ),
    };
    return result;
}
/// convert srgb color space to linear color space
header_only rgb srgb_to_linear( rgb srgb ) {
    rgb result = {
        pow32( srgb.r, 2.2f ),
        pow32( srgb.g, 2.2f ),
        pow32( srgb.b, 2.2f ),
    };
    return result;
}
/// rotate components to the left
header_only vec3 v3_rotate_left( vec3 x ) {
    vec3 result;
    result.x = x.y;
    result.y = x.z;
    result.z = x.x;
    return result;
}
/// rotate components to the right
header_only vec3 v3_rotate_right( vec3 x ) {
    vec3 result;
    result.x = x.z;
    result.y = x.x;
    result.z = x.y;
    return result;
}
/// sum components
header_only f32 v3_horizontal_add( vec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply components
header_only f32 v3_horizontal_mul( vec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
header_only vec3 v3_hadamard( vec3 lhs, vec3 rhs ) {
    vec3 result = {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
    return result;
}
/// calculate square magnitude of vector
header_only f32 v3_sqrmag( vec3 x ) {
    return v3_horizontal_add( v3_hadamard( x, x ) );
}
/// calculate magnitude of vector
header_only f32 v3_mag( vec3 x ) {
    return sqrt32( v3_sqrmag( x ) );
}
/// inner product
header_only f32 v3_dot( vec3 lhs, vec3 rhs ) {
    return v3_horizontal_add( v3_hadamard( lhs, rhs ) );
}
header_only vec3 v3_normalize( vec3 x ) {
    f32 magnitude = v3_mag( x );
    return magnitude < F32_EPSILON ? VEC3_ONE : v3_div( x, magnitude );
}
/// cross product
header_only vec3 v3_cross( vec3 lhs, vec3 rhs ) {
    return (vec3){
        ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ),
        ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ),
        ( lhs.x * rhs.y ) - ( lhs.y * rhs.x )
    };
}
/// reflect direction across axis of given normal
header_only vec3 v3_reflect( vec3 direction, vec3 normal ) {
    return v3_mul(
        v3_sub( normal, direction ),
        2.0f * v3_dot( direction, normal )
    );
}
/// calculate the angle(radians) between two vectors
header_only f32 v3_angle( vec3 a, vec3 b ) {
    return acos32( v3_dot( a, b ) );
}
/// linear interpolation
header_only vec3 v3_lerp( vec3 a, vec3 b, f32 t ) {
    return v3_add( v3_mul( a, 1.0f - t ), v3_mul( b, t ) );
}
/// linear interpolation, t clamped to 0-1
header_only vec3 v3_lerp_clamped( vec3 a, vec3 b, f32 t ) {
    return v3_lerp( a, b, clamp01( t ) );
}
/// smooth step interpolation
header_only vec3 v3_smooth_step( vec3 a, vec3 b, f32 t ) {
    return v3_add(
        v3_mul( v3_sub( b, a ), (3.0f - t * 2.0f) * t * t ),
        a
    );
}
/// smooth step interpolation, t clamped to 0-1
header_only vec3 v3_smooth_step_clamped( vec3 a, vec3 b, f32 t ) {
    return v3_smooth_step( a, b, clamp01( t ) );
}
/// smoother step interpolation
header_only vec3 v3_smoother_step( vec3 a, vec3 b, f32 t ) {
    return v3_add( v3_mul(
        v3_sub( b, a ),
        ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t
    ), a );
}
/// smoother step interpolation, t clamped to 0-1
header_only vec3 v3_smoother_step_clamped(
    vec3 a, vec3 b, f32 t
) {
    return v3_smoother_step( a, b, clamp01( t ) );
}
/// clamp a vector's magnitude
header_only vec3 v3_clamp_mag( vec3 x, f32 max_magnitude ) {
    f32 max = absof( max_magnitude );
    f32 mag = v3_mag( x );
    if( mag > max ) {
        vec3 result = v3_div( x, mag );
        result = v3_mul( result, max );
        return result;
    }

    return x;
}
/// compare two vectors for equality
header_only b32 v3_cmp_eq( vec3 a, vec3 b ) {
    return v3_sqrmag( v3_sub( a, b ) ) < F32_EPSILON;
}

/// Negate vector
header_only ivec3 iv3_neg( ivec3 v ) {
    return (ivec3){ -v.x, -v.y, -v.z };
}
/// Add two vectors
header_only ivec3 iv3_add( ivec3 lhs, ivec3 rhs ) {
    ivec3 result = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    return result;
}
/// Sub two vectors
header_only ivec3 iv3_sub( ivec3 lhs, ivec3 rhs ) {
    ivec3 result = { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    return result;
}
/// Mul vector
header_only ivec3 iv3_mul( ivec3 lhs, i32 rhs ) {
    ivec3 result = { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs };
    return result;
}
/// Div vector
header_only ivec3 iv3_div( ivec3 lhs, i32 rhs ) {
    ivec3 result = { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs };
    return result;
}
/// rotate components to the left
header_only ivec3 iv3_rotate_left( ivec3 x ) {
    ivec3 out = x;
    out.x = x.z;
    out.y = x.x;
    out.z = x.y;
    return out;
}
/// rotate components to the right
header_only ivec3 iv3_rotate_right( ivec3 x ) {
    ivec3 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.x;
    return out;
}
/// sum all components
header_only i32 iv3_horizontal_add( ivec3 x ) {
    return x.x + x.y + x.z;
}
/// multiply all components
header_only i32 iv3_horizontal_mul( ivec3 x ) {
    return x.x * x.y * x.z;
}
/// component-wise multiplication
header_only ivec3 iv3_hadamard( ivec3 lhs, ivec3 rhs ) {
    return (ivec3){
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    };
}
/// inner product
header_only f32 iv3_dot( ivec3 lhs, ivec3 rhs ) {
    ivec3 result = iv3_hadamard( lhs, rhs );
    return iv3_horizontal_add( result );
}
/// calculate square magnitude of vector
header_only f32 iv3_sqrmag( ivec3 x ) {
    return iv3_dot( x, x );
}
/// calculate the magnitude of vector
header_only f32 iv3_mag( ivec3 x ) {
    return sqrt32( iv3_sqrmag( x ) );
}
/// normalize vector
header_only ivec3 iv3_normalize( ivec3 x ) {
    f32 magnitude = iv3_mag( x );
    return magnitude < F32_EPSILON ? IVEC3_ZERO : iv3_div( x, magnitude );
}
/// angle(radians) between two vectors
header_only f32 iv3_angle( ivec3 a, ivec3 b ) {
    return acos32( iv3_dot( a, b ) );
}
/// compare two vectors for equality
header_only b32 iv3_cmp_eq( ivec3 a, ivec3 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/// Negate vector
header_only vec4 v4_neg( vec4 v ) {
    return (vec4){ -v.x, -v.y, -v.z, -v.w };
}
/// Add two vectors
header_only vec4 v4_add( vec4 lhs, vec4 rhs ) {
    vec4 result = {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
    return result;
}
/// Sub two vectors
header_only vec4 v4_sub( vec4 lhs, vec4 rhs ) {
    vec4 result = {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
    return result;
}
/// Mul vector
header_only vec4 v4_mul( vec4 lhs, f32 rhs ) {
    vec4 result = {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
    return result;
}
/// Div vector
header_only vec4 v4_div( vec4 lhs, f32 rhs ) {
    vec4 result = {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
    return result;
}
/// convert linear color space to srgb color space
header_only rgba rgba_linear_to_srgb( rgba linear ) {
    rgba result;
    result.rgb = linear_to_srgb( linear.rgb );
    result.a = linear.a;
    return result;
}
/// convert srgb color space to linear color space
header_only rgba srgb_to_linear_rgba( rgba srgb ) {
    rgba result;
    result.rgb = srgb_to_linear( srgb.rgb );
    result.a   = srgb.a;
    return result;
}
/// rotate components to the left
header_only vec4 v4_rotate_left( vec4 x ) {
    vec4 result;
    result.x = x.y;
    result.y = x.z;
    result.z = x.w;
    result.w = x.x;
    return result;
}
/// rotate components to the right
header_only vec4 v4_rotate_right( vec4 x ) {
    vec4 result;
    result.x = x.w;
    result.y = x.x;
    result.z = x.y;
    result.w = x.z;
    return result;
}
/// sum components
header_only f32 v4_horizontal_add( vec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply components
header_only f32 v4_horizontal_mul( vec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
header_only vec4 v4_hadamard( vec4 lhs, vec4 rhs ) {
    return (vec4){
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// calculate square magnitude of vector
header_only f32 v4_sqrmag( vec4 x ) {
    return v4_horizontal_add( v4_hadamard( x, x ) );
}
/// calculate magnitude of vector
header_only f32 v4_mag( vec4 x ) {
    return sqrt32( v4_sqrmag( x ) );
}
/// normalize vector
header_only vec4 v4_normalize( vec4 x ) {
    f32 magnitude = v4_mag( x );
    return magnitude < F32_EPSILON ? VEC4_ZERO : v4_div( x, magnitude );
}
/// inner product
header_only f32 v4_dot( vec4 lhs, vec4 rhs ) {
    return v4_horizontal_add( v4_hadamard( lhs, rhs ) );
}
/// linear interpolation
header_only vec4 v4_lerp( vec4 a, vec4 b, f32 t ) {
    return v4_add( v4_mul( a, 1.0f - t ), v4_mul( b, t ) );
}
/// linear interpolation, t clamped to 0-1
header_only vec4 v4_lerp_clamped( vec4 a, vec4 b, f32 t ) {
    return v4_lerp( a, b, clamp01( t ) );
}
/// smooth step interpolation
header_only vec4 v4_smooth_step( vec4 a, vec4 b, f32 t ) {
    return v4_add(
        v4_mul( v4_sub( b, a ), (3.0f - t * 2.0f) * t * t ),
        a
    );
}
/// smooth step interpolation, t clamped to 0-1
header_only vec4 v4_smooth_step_clamped( vec4 a, vec4 b, f32 t ) {
    return v4_smooth_step( a, b, clamp01( t ) );
}
/// smoother step interpolation
header_only vec4 v4_smoother_step( vec4 a, vec4 b, f32 t ) {
    return v4_add( v4_mul(
        v4_sub( b, a ),
        ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t
    ), a );
}
/// smoother step interpolation, t clamped to 0-1
header_only vec4 v4_smoother_step_clamped(
    vec4 a, vec4 b, f32 t
) {
    return v4_smoother_step( a, b, clamp01( t ) );
}
/// clamp a vector's magnitude
header_only vec4 v4_clamp_mag( vec4 x, f32 max_magnitude ) {
    f32 max = absof( max_magnitude );
    f32 mag = v4_mag( x );
    if( mag > max ) {
        vec4 result = v4_div( x, mag );
        result = v4_mul( result, max );
        return result;
    }

    return x;
}
/// compare two vectors for equality
header_only b32 v4_cmp_eq( vec4 a, vec4 b ) {
    return v4_sqrmag( v4_sub( a, b ) ) < F32_EPSILON;
}

/// Negate vector
header_only ivec4 iv4_neg( ivec4 v ) {
    return (ivec4){ -v.x, -v.y, -v.z, -v.w };
}
/// Add two vectors
header_only ivec4 iv4_add( ivec4 lhs, ivec4 rhs ) {
    ivec4 result = {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
    return result;
}
/// Sub two vectors
header_only ivec4 iv4_sub( ivec4 lhs, ivec4 rhs ) {
    ivec4 result = {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    };
    return result;
}
/// Mul vector
header_only ivec4 iv4_mul( ivec4 lhs, i32 rhs ) {
    ivec4 result = {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    };
    return result;
}
/// Div vector
header_only ivec4 iv4_div( ivec4 lhs, i32 rhs ) {
    ivec4 result = {
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs,
        lhs.w / rhs
    };
    return result;
}
/// rotate components to the left
header_only ivec4 iv4_rotate_left( ivec4 x ) {
    ivec4 out = x;
    out.x = x.w;
    out.y = x.x;
    out.z = x.y;
    out.w = x.z;
    return out;
}
/// rotate components to the right
header_only ivec4 iv4_rotate_right( ivec4 x ) {
    ivec4 out = x;
    out.x = x.y;
    out.y = x.z;
    out.z = x.w;
    out.w = x.x;
    return out;
}
/// sum all components
header_only i32 iv4_horizontal_add( ivec4 x ) {
    return x.x + x.y + x.z + x.w;
}
/// multiply all components
header_only i32 iv4_horizontal_mul( ivec4 x ) {
    return x.x * x.y * x.z * x.w;
}
/// component-wise multiplication
header_only ivec4 iv4_hadamard( ivec4 lhs, ivec4 rhs ) {
    return (ivec4){
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    };
}
/// inner product
header_only f32 iv4_dot( ivec4 lhs, ivec4 rhs ) {
    ivec4 result = iv4_hadamard( lhs, rhs );
    return iv4_horizontal_add( result );
}
/// calculate square magnitude of vector
header_only f32 iv4_sqrmag( ivec4 x ) {
    return iv4_dot( x, x );
}
/// calculate the magnitude of vector
header_only f32 iv4_mag( ivec4 x ) {
    return sqrt32( iv4_sqrmag( x ) );
}
/// normalize vector
header_only ivec4 iv4_normalize( ivec4 x ) {
    f32 magnitude = iv4_mag( x );
    return magnitude < F32_EPSILON ? IVEC4_ZERO : iv4_div( x, magnitude );
}
/// angle(radians) between two vectors
header_only f32 iv4_angle( ivec4 a, ivec4 b ) {
    return acos32( iv4_dot( a, b ) );
}
/// compare two vectors for equality
header_only b32 iv4_cmp_eq( ivec4 a, ivec4 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/// Negate quaternion
header_only quat q_neg( quat q ) {
    return (quat){ -q.w, -q.x, -q.y, -q.z };
}
/// Add two quaternions
header_only quat q_add( quat lhs, quat rhs ) {
    quat result = {
        lhs.w + rhs.w,
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
    return result;
}
/// Sub two quaternions
header_only quat q_sub( quat lhs, quat rhs ) {
    quat result = {
        lhs.w - rhs.w,
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
    return result;
}
/// Mul a quaternion
header_only quat q_mul( quat lhs, f32 rhs ) {
    quat result = {
        lhs.w * rhs,
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    };
    return result;
}
header_only quat q_mul_q( quat lhs, quat rhs ) {
    const Lane4f negate_first128 = lane4f_set(
        -1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    Lane4f a = lane4f_set( lhs.x, lhs.x, lhs.y, lhs.z );
    Lane4f b = lane4f_set( rhs.x, rhs.w, rhs.w, rhs.w );
    a = lane4f_mul( a, b );

    b = lane4f_set( lhs.y, lhs.y, lhs.z, lhs.x );
    Lane4f c = lane4f_set( rhs.y, rhs.z, rhs.x, rhs.y );
    b = lane4f_mul( b, c );
    
    c = lane4f_set( lhs.z, lhs.z, lhs.x, lhs.y );
    Lane4f d = lane4f_set( rhs.z, rhs.y, rhs.z, rhs.x );
    c = lane4f_mul( c, d );

    c = lane4f_mul( negate_first128, c );

    d = lane4f_sub( b, c );
    a = lane4f_add( a, d );

    a = lane4f_mul( negate_first128, a );

    Lane4f w = lane4f_set1( lhs.w );
    b = lane4f_set( rhs.w, rhs.x, rhs.y, rhs.z );
    w = lane4f_mul( w, b );

    quat result;
    lane4f_store( result.q, lane4f_add( w, a ) );
    return result;
}
/// Div a quaternion
header_only quat q_div( quat lhs, f32 rhs ) {
    quat result = {
        lhs.w / rhs,
        lhs.x / rhs,
        lhs.y / rhs,
        lhs.z / rhs
    };
    return result;
}
/// Create quaternion from angle and axis.
header_only quat q_angle_axis( f32 theta, vec3 axis ) {
    f32 half_theta   = theta / 2.0f;
    tuple_f32 sinecos = sincos32( half_theta );
    return (quat){
        sinecos.v1,
        axis.x * sinecos.v0,
        axis.y * sinecos.v0,
        axis.z * sinecos.v0,
    };
}
/// Create quaternion from euler angles.
header_only quat q_euler( f32 pitch, f32 yaw, f32 roll ) {
    f32 half_x = pitch / 2.0f;
    f32 half_y = yaw   / 2.0f;
    f32 half_z = roll  / 2.0f;

    f32 x_sin = sin32( half_x );
    f32 y_sin = sin32( half_y );
    f32 z_sin = sin32( half_z );

    f32 x_cos = cos32( half_x );
    f32 y_cos = cos32( half_y );
    f32 z_cos = cos32( half_z );

    f32 xyz_cos = x_cos * y_cos * z_cos;
    f32 xyz_sin = x_sin * y_sin * z_sin;

    return (quat){
        ( xyz_cos ) + ( xyz_sin ),
        ( x_sin * y_cos * z_cos ) + ( x_cos * y_sin * z_sin ),
        ( x_cos * y_sin * z_cos ) + ( x_sin * y_cos * z_sin ),
        ( x_cos * y_cos * z_sin ) + ( x_sin * y_sin * z_cos )
    };
}
/// Create quaternion from euler angles.
header_only quat q_euler_angles( euler_angles euler ) {
    return q_euler( euler.pitch, euler.yaw, euler.roll );
}
header_only vec3 q_mul_v3( quat lhs, vec3 rhs ) {
    vec3 t = v3_mul( v3_cross( lhs.xyz, rhs ), 2.0f );
    return
        v3_add(
            v3_add( rhs, v3_mul( t, lhs.w ) ),
            v3_cross( lhs.xyz, t )
        );
}
/// calculate square magnitude of quaternion
header_only f32 q_sqrmag( quat q ) {
    vec4 result = { q.w, q.x, q.y, q.z };
    return v4_horizontal_add( v4_hadamard( result, result ) );
}
/// calculate magnitude of quaternion
header_only f32 q_mag( quat q ) {
    return sqrt32( q_sqrmag( q ) );
}
/// normalize quaternion
header_only quat q_normalize( quat q ) {
    f32 magnitude = q_mag( q );
    return magnitude < F32_EPSILON ? QUAT_IDENTITY : q_div( q, magnitude );
}
/// conjugate of quaternion
header_only quat q_conjugate( quat q ) {
    return (quat){ q.w, -q.x, -q.y, -q.z };
}
/// calculate inverse quaternion
header_only quat q_inverse( quat q ) {
    return q_div( q_conjugate( q ), q_sqrmag( q ) );
}
/// angle between quaternions
header_only f32 q_angle( quat a, quat b ) {
    quat qd = q_mul_q( q_inverse( a ), b );
    return 2.0f * atan2_32( v3_mag( qd.xyz ), qd.w );
}
/// inner product
header_only f32 q_dot( quat lhs, quat rhs ) {
    vec4 l = { lhs.w, lhs.x, lhs.y, lhs.z };
    vec4 r = { rhs.w, rhs.x, rhs.y, rhs.z };
    return v4_horizontal_add( v4_hadamard( l, r ) );
}
/// linear interpolation
header_only quat q_lerp( quat a, quat b, f32 t ) {
    return q_normalize(
        q_add(
            q_mul( a, 1.0f - t ),
            q_mul( b, t )
        )
    );
}
/// linear interpolation, t is clamped 0-1
header_only quat q_lerp_clamped( quat a, quat b, f32 t ) {
    return q_lerp( a, b, clamp01( t ) );
}
/// spherical linear interpolation
header_only quat q_slerp( quat a, quat b, f32 t ) {
    quat _b = b;
    f32 theta = q_dot(a, b);
    f32 cos_theta = cos32( theta );
    if( cos_theta < 0.0f ) {
        _b = q_neg( _b );
        cos_theta = -cos_theta;
    }
    if( cos_theta > 1.0f - F32_EPSILON ) {
        return q_lerp( a, b, t );
    } else {
        return q_normalize(
            q_div(
                q_add(
                    q_mul( a, sin32( (1.0f - t) * theta ) ),
                    q_mul( _b, sin32( t * theta ) )
                ),
                sin32( theta )
            )
        );
    }
}
/// compare quaternions for equality
header_only b32 q_cmp_eq( quat a, quat b ) {
    return q_sqrmag( q_sub( a, b ) ) < F32_EPSILON;
}
/// calculate a forward basis vector.
header_only vec3 v3_forward_basis( quat rotation ) {
    return q_mul_v3( rotation, VEC3_FORWARD );
}
/// calculate a right basis vector.
header_only vec3 v3_right_basis( quat rotation ) {
    return q_mul_v3( rotation, VEC3_RIGHT );
}
/// calculate an up basis vector.
header_only vec3 v3_up_basis( quat rotation ) {
    return q_mul_v3( rotation, VEC3_UP );
}

/// Add two matrices
header_only mat2 m2_add( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = v2_add( lhs.col0, rhs.col0 );
    result.col1 = v2_add( lhs.col1, rhs.col1 );
    return result;
}
/// Sub two matrices
header_only mat2 m2_sub( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = v2_sub( lhs.col0, rhs.col0 );
    result.col1 = v2_sub( lhs.col1, rhs.col1 );
    return result;
}
/// Mul matrix
header_only mat2 m2_mul( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = v2_mul( lhs.col0, rhs );
    result.col1 = v2_mul( lhs.col1, rhs );
    return result;
}
/// Div matrix
header_only mat2 m2_div( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = v2_div( lhs.col0, rhs );
    result.col1 = v2_div( lhs.col1, rhs );
    return result;
}
/// Mul two matrices
header_only mat2 m2_mul_m2( mat2 lhs, mat2 rhs ) {
    mat2 result = {
        (lhs.c[0] * rhs.c[0]) + (lhs.c[2] * rhs.c[1]),
        (lhs.c[1] * rhs.c[0]) + (lhs.c[3] * rhs.c[2]),

        (lhs.c[0] * rhs.c[2]) + (lhs.c[2] * rhs.c[3]),
        (lhs.c[1] * rhs.c[2]) + (lhs.c[3] * rhs.c[3])
    };
    return result;
}
/// transpose matrix
header_only mat2 m2_transpose( mat2 m ) {
    return (mat2){
        m.c[0], m.c[2],
        m.c[1], m.c[3]
    };
}
/// calculate determinant
header_only f32 m2_determinant( mat2 m ) {
    return ( m.c[0] * m.c[3] ) - ( m.c[2] * m.c[1] );
}

/// Add two matrices
header_only mat3 m3_add( const mat3* lhs, const mat3* rhs ) {
    mat3 result;
    result.col0 = v3_add( lhs->col0, rhs->col0 );
    result.col1 = v3_add( lhs->col1, rhs->col1 );
    result.col2 = v3_add( lhs->col2, rhs->col2 );
    return result;
}
/// Sub two matrices
header_only mat3 m3_sub( const mat3* lhs, const mat3* rhs ) {
    mat3 result;
    result.col0 = v3_sub( lhs->col0, rhs->col0 );
    result.col1 = v3_sub( lhs->col1, rhs->col1 );
    result.col2 = v3_sub( lhs->col2, rhs->col2 );
    return result;
}
/// Mul matrix
header_only mat3 m3_mul( const mat3* lhs, f32 rhs ) {
    mat3 result;
    result.col0 = v3_mul( lhs->col0, rhs );
    result.col1 = v3_mul( lhs->col1, rhs );
    result.col2 = v3_mul( lhs->col2, rhs );
    return result;
}
/// Div matrix
header_only mat3 m3_div( const mat3* lhs, f32 rhs ) {
    mat3 result;
    result.col0 = v3_div( lhs->col0, rhs );
    result.col1 = v3_div( lhs->col1, rhs );
    result.col2 = v3_div( lhs->col2, rhs );
    return result;
}
/// Mul two matrices
header_only mat3 m3_mul_m3( const mat3* lhs, const mat3* rhs ) {
    // TODO(alicia): SIMD?
    mat3 result = {
        // column - 0
        ( lhs->c[0] * rhs->c[0] ) +
            ( lhs->c[3] * rhs->c[1] )  +
            ( lhs->c[6] * rhs->c[2] ),
        ( lhs->c[1] * rhs->c[0] ) +
            ( lhs->c[4] * rhs->c[1] )  +
            ( lhs->c[7] * rhs->c[2] ),
        ( lhs->c[2] * rhs->c[0] ) +
            ( lhs->c[5] * rhs->c[1] )  +
            ( lhs->c[8] * rhs->c[2] ),
        // column - 1
        ( lhs->c[0] * rhs->c[3] ) +
            ( lhs->c[3] * rhs->c[4] )  +
            ( lhs->c[6] * rhs->c[5] ),
        ( lhs->c[1] * rhs->c[3] ) +
            ( lhs->c[4] * rhs->c[4] )  +
            ( lhs->c[7] * rhs->c[5] ),
        ( lhs->c[2] * rhs->c[3] ) +
            ( lhs->c[5] * rhs->c[4] )  +
            ( lhs->c[8] * rhs->c[5] ),
        // column - 2
        ( lhs->c[0] * rhs->c[6] ) +
            ( lhs->c[3] * rhs->c[7] )  +
            ( lhs->c[6] * rhs->c[8] ),
        ( lhs->c[1] * rhs->c[6] ) +
            ( lhs->c[4] * rhs->c[7] )  +
            ( lhs->c[7] * rhs->c[8] ),
        ( lhs->c[2] * rhs->c[6] ) +
            ( lhs->c[5] * rhs->c[7] )  +
            ( lhs->c[8] * rhs->c[8] ),
    };
    return result;
}
/// transpose matrix
header_only mat3 m3_transpose( const mat3* m ) {
    return (mat3){
        m->c[0], m->c[3], m->c[6],
        m->c[1], m->c[4], m->c[7],
        m->c[2], m->c[5], m->c[8]
    };
}
/// calculate determinant
header_only f32 m3_determinant( const mat3* m ) {
    return
     ( m->c[0] * ( ( m->c[4] * m->c[8] ) - ( m->c[7] * m->c[5] ) ) ) +
    -( m->c[3] * ( ( m->c[1] * m->c[8] ) - ( m->c[7] * m->c[2] ) ) ) +
     ( m->c[6] * ( ( m->c[1] * m->c[5] ) - ( m->c[4] * m->c[2] ) ) );
}

/// Transpose matrix.
header_only mat4 m4_transpose( const mat4* m ) {
    return (mat4){
        m->c[0], m->c[4], m->c[ 8], m->c[12],
        m->c[1], m->c[5], m->c[ 9], m->c[13],
        m->c[2], m->c[6], m->c[10], m->c[14],
        m->c[3], m->c[7], m->c[11], m->c[15]
    };
}
/// Add two matrices
header_only mat4 m4_add( const mat4* lhs, const mat4* rhs ) {
    Lane4f lhs0, lhs1, lhs2, lhs3;
    Lane4f rhs0, rhs1, rhs2, rhs3;

    lhs0 = lane4f_load( lhs->col0.c );
    lhs1 = lane4f_load( lhs->col1.c );
    lhs2 = lane4f_load( lhs->col2.c );
    lhs3 = lane4f_load( lhs->col3.c );

    rhs0 = lane4f_load( rhs->col0.c );
    rhs1 = lane4f_load( rhs->col1.c );
    rhs2 = lane4f_load( rhs->col2.c );
    rhs3 = lane4f_load( rhs->col3.c );

    mat4 result;

    lane4f_store( result.col0.c, lane4f_add( lhs0, rhs0 ) );
    lane4f_store( result.col1.c, lane4f_add( lhs1, rhs1 ) );
    lane4f_store( result.col2.c, lane4f_add( lhs2, rhs2 ) );
    lane4f_store( result.col3.c, lane4f_add( lhs3, rhs3 ) );

    return result;
}
/// Sub two matrices
header_only mat4 m4_sub( const mat4* lhs, const mat4* rhs ) {
    Lane4f lhs0, lhs1, lhs2, lhs3;
    Lane4f rhs0, rhs1, rhs2, rhs3;

    lhs0 = lane4f_load( lhs->col0.c );
    lhs1 = lane4f_load( lhs->col1.c );
    lhs2 = lane4f_load( lhs->col2.c );
    lhs3 = lane4f_load( lhs->col3.c );

    rhs0 = lane4f_load( rhs->col0.c );
    rhs1 = lane4f_load( rhs->col1.c );
    rhs2 = lane4f_load( rhs->col2.c );
    rhs3 = lane4f_load( rhs->col3.c );

    mat4 result;

    lane4f_store( result.col0.c, lane4f_sub( lhs0, rhs0 ) );
    lane4f_store( result.col1.c, lane4f_sub( lhs1, rhs1 ) );
    lane4f_store( result.col2.c, lane4f_sub( lhs2, rhs2 ) );
    lane4f_store( result.col3.c, lane4f_sub( lhs3, rhs3 ) );

    return result;
}
/// Mul matrix
header_only mat4 m4_mul( const mat4* lhs, f32 rhs ) {
    Lane4f lhs0, lhs1, lhs2, lhs3;
    Lane4f rhs_;

    lhs0 = lane4f_load( lhs->col0.c );
    lhs1 = lane4f_load( lhs->col1.c );
    lhs2 = lane4f_load( lhs->col2.c );
    lhs3 = lane4f_load( lhs->col3.c );

    rhs_ = lane4f_set1( rhs );

    mat4 result;

    lane4f_store( result.col0.c, lane4f_mul( lhs0, rhs_ ) );
    lane4f_store( result.col1.c, lane4f_mul( lhs1, rhs_ ) );
    lane4f_store( result.col2.c, lane4f_mul( lhs2, rhs_ ) );
    lane4f_store( result.col3.c, lane4f_mul( lhs3, rhs_ ) );

    return result;
}
/// Div matrix
header_only mat4 m4_div( const mat4* lhs, f32 rhs ) {
    Lane4f lhs0, lhs1, lhs2, lhs3;
    Lane4f rhs_;

    lhs0 = lane4f_load( lhs->col0.c );
    lhs1 = lane4f_load( lhs->col1.c );
    lhs2 = lane4f_load( lhs->col2.c );
    lhs3 = lane4f_load( lhs->col3.c );

    rhs_ = lane4f_set1( rhs );

    mat4 result;

    lane4f_store( result.col0.c, lane4f_div( lhs0, rhs_ ) );
    lane4f_store( result.col1.c, lane4f_div( lhs1, rhs_ ) );
    lane4f_store( result.col2.c, lane4f_div( lhs2, rhs_ ) );
    lane4f_store( result.col3.c, lane4f_div( lhs3, rhs_ ) );

    return result;
}
/// Mul two matrices
header_only mat4 m4_mul_m4( const mat4* lhs, const mat4* rhs ) {
    mat4 result = {};

    Lane4f a, b, c;

    u32 col = 0;
    u32 row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    col = 1;
    row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    col = 2;
    row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    col = 3;
    row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_set1( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( &result.c[col * MAT4_COLUMN_COUNT], lane4f_add( b, c ) );

    return result;
}
/// Multiply matrix with vector.
header_only vec4 m4_mul_v4( const mat4* lhs, vec4 rhs ) {
    mat4 transpose = m4_transpose( lhs );

    vec4 result;
    Lane4f rhs_  = lane4f_load( rhs.c );
    Lane4f lhs_0 = lane4f_load( transpose.col0.c );
    Lane4f lhs_1 = lane4f_load( transpose.col1.c );
    Lane4f lhs_2 = lane4f_load( transpose.col2.c );
    Lane4f lhs_3 = lane4f_load( transpose.col3.c );

    Lane4f mul_res_0 = lane4f_mul( lhs_0, rhs_ );
    Lane4f mul_res_1 = lane4f_mul( lhs_1, rhs_ );
    Lane4f mul_res_2 = lane4f_mul( lhs_2, rhs_ );
    Lane4f mul_res_3 = lane4f_mul( lhs_3, rhs_ );

    result.x =
        lane4f_index( mul_res_0, 0 ) + 
        lane4f_index( mul_res_0, 1 ) + 
        lane4f_index( mul_res_0, 2 ) + 
        lane4f_index( mul_res_0, 3 );
    result.y =
        lane4f_index( mul_res_1, 0 ) + 
        lane4f_index( mul_res_1, 1 ) + 
        lane4f_index( mul_res_1, 2 ) + 
        lane4f_index( mul_res_1, 3 );
    result.z =
        lane4f_index( mul_res_2, 0 ) + 
        lane4f_index( mul_res_2, 1 ) + 
        lane4f_index( mul_res_2, 2 ) + 
        lane4f_index( mul_res_2, 3 );
    result.w =
        lane4f_index( mul_res_3, 0 ) + 
        lane4f_index( mul_res_3, 1 ) + 
        lane4f_index( mul_res_3, 2 ) + 
        lane4f_index( mul_res_3, 3 );
    return result;
}
/// Multiply matrix with vector.
header_only vec3 m4_mul_v3( const mat4* lhs, vec3 rhs ) {
    vec4 rhs_v4;
    rhs_v4.xyz = rhs;
    rhs_v4.w   = 1.0f;
    vec4 result = m4_mul_v4( lhs, rhs_v4 );
    return result.xyz;
}
/// create new look at matrix
header_only mat4 m4_lookat( vec3 position, vec3 target, vec3 up ) {
    vec3 z = v3_normalize( v3_sub( target, position ) );
    vec3 x = v3_cross( z, up );
    vec3 y = v3_cross( x, z );
    z = v3_neg( z );

    return (mat4){
        x.x, y.x, z.x, 0.0f,
        x.y, y.y, z.y, 0.0f,
        x.z, y.z, z.z, 0.0f,

        -v3_dot( x, position ),
        -v3_dot( y, position ),
        -v3_dot( z, position ),
        1.0f
    };
}
/// create new orthographic projection matrix
header_only mat4 m4_ortho(
    f32 left, f32 right,
    f32 bottom, f32 top,
    f32 near_, f32 far_
) {
    mat4 result = MAT4_IDENTITY;
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
/// create orthographic matrix for 2D rendering
header_only mat4 m4_projection2d( f32 aspect_ratio, f32 scale ) {
    f32 scaled_aspect = aspect_ratio * scale;
    return m4_ortho(
        -scaled_aspect, scaled_aspect,
        -scale, scale,
        -1.0f, 1.0f
    );
}
/// create perspective matrix
header_only mat4 m4_perspective(
    f32 fov, f32 aspect_ratio,
    f32 near_, f32 far_
) {
    mat4 result = {};
    
    f32 half_fov_tan = tan32( fov / 2.0f );
    f32 f_sub_n      = far_ - near_;

    result.c[ 0] = 1.0f / ( aspect_ratio * half_fov_tan );
    result.c[ 5] = 1.0f / half_fov_tan;
    result.c[10] = -( ( far_ + near_ ) / f_sub_n );
    result.c[11] = -1.0f;
    result.c[14] = -( ( 2.0f * far_ * near_ ) / f_sub_n );

    return result;
}
/// create new translation matrix
header_only mat4 m4_translate( f32 x, f32 y, f32 z ) {
    return (mat4){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f,
    };
}
/// create new translation matrix
header_only mat4 m4_translate_v3( vec3 translation ) {
    return m4_translate( translation.x, translation.y, translation.z );
}
/// create new 2d translation matrix
header_only mat4 m4_translate_2d( f32 x, f32 y ) {
    return m4_translate( x, y, 0.0f );
}
/// create new 2d translation matrix
header_only mat4 m4_translate_v2( vec2 translation ) {
    return m4_translate_2d( translation.x, translation.y );
}
/// create rotation matrix around x axis
header_only mat4 m4_rotate_pitch( f32 pitch ) {
    tuple_f32 sincos = sincos32( pitch );

    return (mat4){
        1.0f,       0.0f,      0.0f, 0.0f,
        0.0f,  sincos.v1, sincos.v0, 0.0f,
        0.0f, -sincos.v0, sincos.v1, 0.0f,
        0.0f,       0.0f,      0.0f, 1.0f
    };
}
/// create rotation matrix around y axis
header_only mat4 m4_rotate_yaw( f32 yaw ) {
    tuple_f32 sincos = sincos32( yaw );

    return (mat4){
        sincos.v1, 0.0f, -sincos.v0, 0.0f,
             0.0f, 1.0f,       0.0f, 0.0f,
        sincos.v0, 0.0f,  sincos.v1, 0.0f,
             0.0f, 0.0f,       0.0f, 1.0f
    };
}
/// create rotation matrix around z axis
header_only mat4 m4_rotate_roll( f32 roll ) {
    tuple_f32 sincos = sincos32( roll );

    return (mat4){
         sincos.v1, sincos.v0, 0.0f, 0.0f,
        -sincos.v0, sincos.v1, 0.0f, 0.0f,
              0.0f,      0.0f, 1.0f, 0.0f,
              0.0f,      0.0f, 0.0f, 1.0f
    };
}
/// create rotation matrix from euler angles (radians)
header_only mat4 m4_rotate_euler( euler_angles r ) {
    mat4 pitch = m4_rotate_pitch( r.pitch );
    mat4 yaw   = m4_rotate_yaw( r.yaw );
    mat4 roll  = m4_rotate_roll( r.roll );
    mat4 yaw_mul_roll = m4_mul_m4( &yaw, &roll );
    return m4_mul_m4( &pitch, &yaw_mul_roll );
}
/// create rotation matrix from quaternion
header_only mat4 m4_rotate_q( quat q ) {
    // TODO(alicia): SIMD
    mat4 result = MAT4_IDENTITY;

    // Lane4f       q_ = lane4f_load( q.q );
    // const Lane4f _2 = lane4f_set1( 2.0f );
    //
    // Lane4f q_2 = lane4f_mul( q_, _2 );

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
header_only mat4 m4_rotate_2d( f32 theta ) {
    return m4_rotate_roll( -theta );
}
/// create scale matrix
header_only mat4 m4_scale( f32 width, f32 height, f32 length ) {
    return (mat4){
        width,   0.0f,   0.0f, 0.0f,
         0.0f, height,   0.0f, 0.0f,
         0.0f,   0.0f, length, 0.0f,
         0.0f,   0.0f,   0.0f, 1.0f
    };
}
/// create scale matrix
header_only mat4 m4_scale_v3( vec3 scale ) {
    return m4_scale(
        scale.width,
        scale.height,
        scale.length
    );
}
/// create scale matrix for 2D
header_only mat4 m4_scale_2d( f32 width, f32 height ) {
    return m4_scale( width, height, 1.0f );
}
/// create scale matrix for 2D
header_only mat4 m4_scale_v2( vec2 scale ) {
    return m4_scale_2d( scale.width, scale.height );
}
/// create transform matrix
header_only mat4 m4_transform_v3_euler_v3(
    vec3         translation,
    euler_angles rotation,
    vec3         scale
) {
    mat4 translation_m4 = m4_translate_v3( translation );
    mat4 rotation_m4    = m4_rotate_euler( rotation );
    mat4 scale_m4       = m4_scale_v3( scale );
    mat4 r_mul_s = m4_mul_m4( &rotation_m4, &scale_m4 );
    return m4_mul_m4( &translation_m4, &r_mul_s );
}
/// create transform matrix
header_only mat4 m4_transform(
    vec3 translation,
    quat rotation,
    vec3 scale
) {
    mat4 translation_m4 = m4_translate_v3( translation );
    mat4 rotation_m4    = m4_rotate_q( rotation );
    mat4 scale_m4       = m4_scale_v3( scale );
    mat4 r_mul_s = m4_mul_m4( &rotation_m4, &scale_m4 );
    return m4_mul_m4( &translation_m4, &r_mul_s );
}
/// create transform matrix for 2D
header_only mat4 m4_transform_2d(
    vec2 translation,
    f32  rotation,
    vec2 scale
) {
    mat4 translation_m4 = m4_translate_v2( translation );
    mat4 rotation_m4    = m4_rotate_2d( rotation );
    mat4 scale_m4       = m4_scale_v2( scale );
    mat4 r_mul_s = m4_mul_m4( &rotation_m4, &scale_m4 );
    return m4_mul_m4( &translation_m4, &r_mul_s );
}
/// get submatrix at given coordinates
header_only mat3 m4_submatrix( const mat4* m, u32 row, u32 col ) {
    mat3 result;
    u32 i = 0;
    for( u32 c = 0; c < 4; ++c ) {
        if( c != col ) {
            for( u32 r = 0; r < 4; ++r ) {
                if( r != row ) {
                    result.c[i++] = m->m[c][r];
                }
            }
        }
    }
    return result;
}
/// calculate determinant of submatrix at given coordinates
header_only f32 m4_minor( const mat4* m, u32 row, u32 col ) {
    mat3 submatrix = m4_submatrix( m, row, col );
    return m3_determinant( &submatrix );
}
/// calculate the cofactor of minor at given coordinates
header_only f32 m4_cofactor( const mat4* m, u32 row, u32 col ) {
    f32 minor = m4_minor( m, row, col );
    return minor * powi32(
        -1.0f,
        ( row + 1 ) + ( col + 1 )
    );
}
/// construct matrix from cofactors
header_only mat4 m4_cofactor_matrix( const mat4* m ) {
    return (mat4){
        m4_cofactor( m, 0, 0 ),
            m4_cofactor( m, 0, 1 ),
            m4_cofactor( m, 0, 2 ),
            m4_cofactor( m, 0, 3 ),
        m4_cofactor( m, 1, 0 ),
            m4_cofactor( m, 1, 1 ),
            m4_cofactor( m, 1, 2 ),
            m4_cofactor( m, 1, 3 ),
        m4_cofactor( m, 2, 0 ),
            m4_cofactor( m, 2, 1 ),
            m4_cofactor( m, 2, 2 ),
            m4_cofactor( m, 2, 3 ),
        m4_cofactor( m, 3, 0 ),
            m4_cofactor( m, 3, 1 ),
            m4_cofactor( m, 3, 2 ),
            m4_cofactor( m, 3, 3 ),
    };
}
/// calculate the adjoint matrix of given matrix
header_only mat4 m4_adjoint( const mat4* m ) {
    mat4 cofactor = m4_cofactor_matrix( m );
    return m4_transpose( &cofactor );
}
/// calculate the determinant
header_only f32 m4_determinant( const mat4* m ) {
    mat3 submatrix_0 = m4_submatrix( m, 0, 0 );
    mat3 submatrix_1 = m4_submatrix( m, 0, 1 );
    mat3 submatrix_2 = m4_submatrix( m, 0, 2 );
    mat3 submatrix_3 = m4_submatrix( m, 0, 3 );
    return
        ( m->c[ 0] * m3_determinant( &submatrix_0 ) ) -
        ( m->c[ 4] * m3_determinant( &submatrix_1 ) ) +
        ( m->c[ 8] * m3_determinant( &submatrix_2 ) ) -
        ( m->c[12] * m3_determinant( &submatrix_3 ) );
}
/// attempt to invert matrix
/// @out_matrix pointer to result,
/// set to identity if inversion is not possible
/// @return true if inversion is possible
header_only b32 m4_inverse( const mat4* m, mat4* out_matrix ) {
    f32 determinant = m4_determinant( m );
    if( determinant == 0.0f ) {
        *out_matrix = MAT4_IDENTITY;
        return false;
    } else {
        mat4 adjoint = m4_adjoint( m );
        *out_matrix  = m4_div( &adjoint, determinant );
        return true;
    }
}
/// invert matrix whether determinant is 0 or not
header_only mat4 m4_inverse_unchecked( const mat4* m ) {
    mat4 adjoint     = m4_adjoint( m );
    f32  determinant = m4_determinant( m );
    return m4_div( &adjoint, determinant );
}

/// convert rgb to rgba
header_only rgba rgb_to_rgba( rgb rgb ) {
    rgba result;
    result.rgb = rgb;
    result.a   = 1.0f;
    return result;
}

/// convert quaternion to euler angles
header_only euler_angles quat_to_euler( quat q ) {
    return (euler_angles){
        atan2_32(
            2.0f * (( q.w * q.x ) + ( q.y * q.z )),
            1.0f - 2.0f * ( ( q.x * q.x ) + ( q.y * q.y ) )
        ),
        asin32_real( 2.0f * (( q.w * q.y ) - ( q.z * q.x )) ),
        atan2_32(
            2.0f * (( q.w * q.z ) + ( q.x * q.y )),
            1.0f - 2.0f * ( ( q.y * q.y ) + ( q.z * q.z ) )
        ),
    };
}
/// convert quaternion to angle axis
header_only void quat_to_angle_axis(
    quat q,
    f32* out_angle_radians,
    vec3* out_axis
) {
    f32 inv_w2_sqrt = sqrt32( 1.0f - ( q.w * q.w ) );
    *out_axis = v3_div( q.xyz, inv_w2_sqrt );
    *out_angle_radians = 2.0f * acos32( q.w );
}

header_only vec2 v3_to_v2( vec3 v ) { return v.xy; }
header_only vec2 v4_to_v2( vec4 v ) { return v.xy; }
header_only vec2 iv2_to_v2( ivec2 v ) {
    return (vec2){ (f32)v.x, (f32)v.y };
}

header_only ivec2 v2_trunc( vec2 v ) {
    return (ivec2){ trunc_i32(v.x), trunc_i32(v.y) };
}
header_only ivec2 v2_floor( vec2 v ) {
    return (ivec2){ floor_i32(v.x), floor_i32(v.y) };
}
header_only ivec2 v2_ceil( vec2 v ) {
    return (ivec2){ ceil_i32(v.x), ceil_i32(v.y) };
}
header_only ivec2 v2_round( vec2 v ) {
    return (ivec2){ round_i32(v.x), round_i32(v.y) };
}

header_only ivec2 iv3_to_iv2( ivec3 v ) { return v.xy; }
header_only ivec2 iv4_to_iv2( ivec4 v ) { return v.xy; }
header_only ivec2 v2_to_iv2( vec2 v )  { return v2_trunc( v ); }

header_only vec3 v2_to_v3( vec2 v ) { return (vec3){ v.x, v.y, 0.0f }; }
header_only vec3 v4_to_v3( vec4 v ) { return v.xyz; }
header_only vec3 iv3_to_v3( ivec3 v ) {
    return (vec3){ (f32)v.x, (f32)v.y, (f32)v.z };
}

header_only ivec3 v3_trunc( vec3 v ) {
    return (ivec3){ trunc_i32(v.x), trunc_i32(v.y), trunc_i32(v.z) };
}
header_only ivec3 v3_floor( vec3 v ) {
    return (ivec3){ floor_i32(v.x), floor_i32(v.y), floor_i32(v.z) };
}
header_only ivec3 v3_ceil( vec3 v ) {
    return (ivec3){ ceil_i32(v.x), ceil_i32(v.y), ceil_i32(v.z) };
}
header_only ivec3 v3_round( vec3 v ) {
    return (ivec3){ round_i32(v.x), round_i32(v.y), round_i32(v.z) };
}

header_only ivec3 iv2_to_iv3( ivec2 v ) { return (ivec3){ v.x, v.y, 0 }; }
header_only ivec3 iv4_to_iv3( ivec4 v ) { return v.xyz; }
header_only ivec3 v3_to_iv3( vec3 v )  { return v3_trunc( v ); }

header_only vec4 v2_to_v4( vec2 v ) {
    return (vec4){ v.x, v.y, 0.0f, 0.0f };
}
header_only vec4 v3_to_v4( vec3 v ) {
    return (vec4){ v.x, v.y,  v.z, 0.0f };
}
header_only vec4 iv4_to_v4( ivec4 v ) {
    return (vec4){ (f32)v.x, (f32)v.y, (f32)v.z, (f32)v.w };
}

header_only ivec4 v4_trunc( vec4 v ) {
    return (ivec4){
        trunc_i32(v.x), trunc_i32(v.y),
        trunc_i32(v.z), trunc_i32(v.w)
    };
}
header_only ivec4 v4_floor( vec4 v ) {
    return (ivec4){
        floor_i32(v.x), floor_i32(v.y),
        floor_i32(v.z), floor_i32(v.w)
    };
}
header_only ivec4 v4_ceil( vec4 v ) {
    return (ivec4){
        ceil_i32(v.x), ceil_i32(v.y),
        ceil_i32(v.z), ceil_i32(v.w)
    };
}
header_only ivec4 v4_round( vec4 v ) {
    return (ivec4){
        round_i32(v.x), round_i32(v.y),
        round_i32(v.z), round_i32(v.w)
    };
}

header_only ivec4 iv2_to_iv4( ivec2 v ) {
    return (ivec4){ v.x, v.y, 0, 0 };
}
header_only ivec4 iv3_to_iv4( ivec3 v ) {
    return (ivec4){ v.x, v.y, v.z, 0 };
}
header_only ivec4 v4_to_iv4( vec4 v ) {
    return v4_trunc( v );
}

header_only mat3 m2_to_m3( mat2 m ) {
    return (mat3){
        m.c[0], m.c[1], 0.0f,
        m.c[2], m.c[3], 0.0f,
          0.0f,   0.0f, 0.0f
    };
}
header_only mat3 m4_to_m3( const mat4* m ) {
    return (mat3){
        m->c[0], m->c[1], m->c[ 2],
        m->c[4], m->c[5], m->c[ 6],
        m->c[8], m->c[9], m->c[10]
    };
}

header_only mat4 m2_to_m4( mat2 m ) {
    return (mat4){
        m.c[0], m.c[1], 0.0f, 0.0f,
        m.c[2], m.c[3], 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
          0.0f,   0.0f, 0.0f, 0.0f,
    };
}
header_only mat4 m3_to_m4( const mat3* m ) {
    return (mat4){
        m->c[0], m->c[1], m->c[2], 0.0f,
        m->c[3], m->c[4], m->c[5], 0.0f,
        m->c[6], m->c[7], m->c[8], 0.0f,
          0.0f,   0.0f,   0.0f, 0.0f,
    };
}

// VEC2/MAT4 ----------------------------------------------------

/// create look at matrix for 2D rendering
header_only mat4 m4_lookat_2d( vec2 position, vec2 up ) {
    vec3 position_3d = v3( position.x, position.y, -1.0f );
    vec3 target_3d   = v3( position.x, position.y, 0.0f );
    vec3 up_3d       = v3( up.x, up.y, 0.0f );
    return m4_lookat(
        position_3d,
        target_3d,
        up_3d
    );
}

// MAT3/MAT4 ----------------------------------------------------

/// calculate the normal matrix of transform matrix
/// @out_matrix pointer to result,
/// set to identity if not possible
/// @return true if determinant is not 0
header_only b32 m4_normal_matrix( const mat4* m, mat3* out_matrix ) {
    mat4 inv;

    if( m4_inverse( m, &inv ) ) {
        mat4 inv_transpose = m4_transpose( &inv );
        *out_matrix = m4_to_m3( &inv_transpose );
        return true;
    } else {
        *out_matrix = MAT3_IDENTITY;
        return false;
    }
}
/// calculate the normal matrix of transform matrix
/// whether the determinant is 0 or not
header_only mat3 m4_normal_matrix_unchecked( const mat4* m ) {
    mat4 inv = m4_inverse_unchecked( m );
    mat4 inv_transpose = m4_transpose( &inv );
    return m4_to_m3( &inv_transpose );
}

/// Create a default transform.
header_only Transform transform_zero(void) {
    Transform result;
    result.position = VEC3_ZERO;
    result.rotation = QUAT_IDENTITY;
    result.scale    = VEC3_ONE;

    result.matrix       = MAT4_IDENTITY;
    result.matrix_dirty = false;
    result.parent       = NULL;

    return result;
}
/// Create a transform with postion, rotation and scale.
header_only Transform transform_create( vec3 position, quat rotation, vec3 scale ) {
    Transform result;
    result.position = position;
    result.rotation = rotation;
    result.scale    = scale;
    
    result.matrix = m4_transform( position, rotation, scale );
    result.matrix_dirty = false;

    result.parent = NULL;

    return result;
}
/// Create a transform with position.
header_only Transform transform_with_position( vec3 position ) {
    return transform_create( position, QUAT_IDENTITY, VEC3_ONE );
}
/// Create a transform with rotation.
header_only Transform transform_with_rotation( quat rotation ) {
    return transform_create( VEC3_ZERO, rotation, VEC3_ONE );
}
/// Create a transform with scale.
header_only Transform transform_with_scale( vec3 scale ) {
    return transform_create( VEC3_ZERO, QUAT_IDENTITY, scale );
}
/// Set a transform's position.
header_only void transform_set_position( Transform* t, vec3 position ) {
    t->position     = position;
    t->matrix_dirty = true;
    t->camera_dirty = true;
}
/// Translate a transform.
header_only void transform_translate( Transform* t, vec3 translation ) {
    transform_set_position( t, v3_add( t->position, translation ) );
}
/// Set a transform's rotation.
header_only void transform_set_rotation( Transform* t, quat rotation ) {
    t->rotation     = rotation;
    t->matrix_dirty = true;
    t->camera_dirty = true;
}
/// Rotate a transform.
header_only void transform_rotate( Transform* t, quat rotation ) {
    transform_set_rotation( t, q_mul_q( t->rotation, rotation ) );
}
/// Set a transform's scale.
header_only void transform_set_scale( Transform* t, vec3 scale ) {
    t->scale        = scale;
    t->matrix_dirty = true;
    t->camera_dirty = true;
}
/// Scale a transform.
header_only void transform_scale( Transform* t, vec3 scale ) {
    transform_set_scale( t, v3_hadamard( t->scale, scale ) );
}
/// Get a transform's local matrix.
header_only mat4* transform_local_matrix( Transform* t ) {
    if( t->matrix_dirty ) {
        t->matrix_dirty = false;
        t->matrix = m4_transform( t->position, t->rotation, t->scale );
    }
    return &t->matrix;
}
/// Get a transform's world matrix.
/// This value should be cached for performance.
header_only mat4 transform_world_matrix( Transform* t ) {
    mat4* local_matrix = transform_local_matrix( t );
    if( t->parent ) {
        mat4 parent_matrix = transform_world_matrix( t->parent );
        return m4_mul_m4( local_matrix, &parent_matrix );
    }
    return *local_matrix;
}
/// Get transform local position.
header_only vec3 transform_local_position( Transform* t ) {
    return t->position;
}
/// Get transform local rotation.
header_only quat transform_local_rotation( Transform* t ) {
    return t->rotation;
}
/// Get transform local scale.
header_only vec3 transform_local_scale( Transform* t ) {
    return t->scale;
}
/// Get transform world position.
/// This value should be cached for performance.
header_only vec3 transform_world_position( Transform* t ) {
    vec3 local_position = transform_local_position( t );
    if( t->parent ) {
        vec3 parent_position = transform_world_position( t->parent );
        return v3_add( local_position, parent_position );
    }
    return local_position;
}
/// Get transform world rotation.
/// This value should be cached for performance.
header_only quat transform_world_rotation( Transform* t ) {
    quat local_rotation = transform_local_rotation( t );
    if( t->parent ) {
        quat parent_rotation = transform_world_rotation( t->parent );
        return q_mul_q( local_rotation, parent_rotation );
    }
    return local_rotation;
}
/// Get transform world scale.
/// This value should be cached for performance.
header_only vec3 transform_world_scale( Transform* t ) {
    vec3 local_scale = transform_local_scale( t );
    if( t->parent ) {
        vec3 parent_scale = transform_world_scale( t->parent );
        return v3_hadamard( local_scale, parent_scale );
    }
    return local_scale;
}
/// Calculate local forward basis.
header_only vec3 transform_local_forward_basis( Transform* t ) {
    return q_mul_v3( t->rotation, VEC3_FORWARD );
}
/// Calculate local right basis.
header_only vec3 transform_local_right_basis( Transform* t ) {
    return q_mul_v3( t->rotation, VEC3_RIGHT );
}
/// Calculate local up basis.
header_only vec3 transform_local_up_basis( Transform* t ) {
    return q_mul_v3( t->rotation, VEC3_UP );
}
/// Calculate world forward basis.
/// This value should be cached for performance.
header_only vec3 transform_world_forward_basis( Transform* t ) {
    return q_mul_v3(
        transform_world_rotation( t ),
        transform_local_forward_basis( t )
    );
}
/// Calculate world right basis.
/// This value should be cached for performance.
header_only vec3 transform_world_right_basis( Transform* t ) {
    return q_mul_v3(
        transform_world_rotation( t ),
        transform_local_right_basis( t )
    );
}
/// Calculate world up basis.
/// This value should be cached for performance.
header_only vec3 transform_world_up_basis( Transform* t ) {
    return q_mul_v3(
        transform_world_rotation( t ),
        transform_local_up_basis( t )
    );
}

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
    return v2_cmp_eq( lhs, rhs );
}
inline b32 operator!=( vec2 lhs, vec2 rhs ) {
    return !v2_cmp_eq( lhs, rhs );
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
    return iv2_cmp_eq( lhs, rhs );
}
inline b32 operator!=( ivec2 lhs, ivec2 rhs ) {
    return !iv2_cmp_eq( lhs, rhs );
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
    return v3_cmp_eq( lhs, rhs );
}
inline b32 operator!=( vec3 lhs, vec3 rhs ) {
    return !v3_cmp_eq( lhs, rhs );
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
    return iv3_cmp_eq( lhs, rhs );
}
inline b32 operator!=( ivec3 lhs, ivec3 rhs ) {
    return !iv3_cmp_eq( lhs, rhs );
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
    return v4_cmp_eq( lhs, rhs );
}
inline b32 operator!=( vec4 lhs, vec4 rhs ) {
    return !v4_cmp_eq( lhs, rhs );
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
    return iv4_cmp_eq( lhs, rhs );
}
inline b32 operator!=( ivec4 lhs, ivec4 rhs ) {
    return !iv4_cmp_eq( lhs, rhs );
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
    return q_cmp_eq( lhs, rhs );
}
inline b32 operator!=( quat lhs, quat rhs ) {
    return !q_cmp_eq( lhs, rhs );
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

#endif // c++ operator overloads

#endif // header guard
