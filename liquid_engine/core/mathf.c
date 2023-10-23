// * Description:  Math functions implementation.
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 06, 2023
#include "defines.h"
#include "constants.h"
#include "core/simd.h"
#include "core/mathf.h"

b32 is_nan32( f32 x ) {
    u32 bitpattern = reinterpret( u32, x );

    u32 exp = bitpattern & F32_EXPONENT_MASK;
    u32 man = bitpattern & F32_MANTISSA_MASK;

    return exp == F32_EXPONENT_MASK && man != 0;
}
b32 is_nan64( f64 x ) {
    u64 bitpattern = reinterpret( u64, x );

    u64 exp = bitpattern & F64_EXPONENT_MASK;
    u64 man = bitpattern & F64_MANTISSA_MASK;

    return exp == F64_EXPONENT_MASK && man != 0;
}

f32 sqrt32( f32 x ) {
    return lane1f_sqrt( x );
}
f64 sqrt64( f64 x ) {
    // TODO(alicia):
    return (f64)lane1f_sqrt( (f32)x );
}

f32 rsqrt32( f32 x ) {
    return lane1f_rsqrt( x );
}
f64 rsqrt64( f64 x ) {
    // TODO(alicia): 
    return (f64)lane1f_rsqrt( (f32)x );
}

f32 powi32( f32 base, i32 exp ) {
    u32 exp_abs = absolute( exp );
    f32 result       = base;
    for( u32 i = 1; i < exp_abs; ++i ) {
        result *= base;
    }
    if( exp < 0 ) {
        return 1.0f / result;
    } else {
        return result;
    }
}
f64 powi64( f64 base, i64 exp ) {
    u64 exp_abs = absolute( exp );
    f64 result       = base;
    for( u64 i = 1; i < exp_abs; ++i ) {
        result *= base;
    }
    if( exp < 0 ) {
        return 1.0 / result;
    } else {
        return result;
    }
}

f32 pow32( f32 base, f32 exp ) {
    return exp32( base * ln32( exp ) );
}
f64 pow64( f64 base, f64 exp ) {
    return exp64( base * ln64( exp ) );
}

f32 mod32( f32 lhs, f32 rhs ) {
    if( rhs == 0.0f ) {
        return lhs;
    }

    f32 m = lhs - ( rhs * (f32)floor32_i32( lhs / rhs ) );

    if( rhs > 0.0f ) {
        if( m >= rhs ) {
            return 0.0f;
        }

        if( m < 0.0f ) {
            if( (rhs + m) == rhs ) {
                return 0.0f;
            } else {
                return rhs + m;
            }
        }
    } else {
        if( m <= rhs ) {
            return 0.0f;
        }
        if( m > 0.0f ) {
            if( ( rhs + m ) == rhs ) {
                return 0.0f;
            } else {
                return rhs + m;
            }
        }
    }

    return m;
}
f64 mod64( f64 lhs, f64 rhs ) {
    if( rhs == 0.0 ) {
        return lhs;
    }

    f64 m = lhs - ( rhs * (f64)floor64_i64( lhs / rhs ) );

    if( rhs > 0.0 ) {
        if( m >= rhs ) {
            return 0.0;
        }

        if( m < 0.0 ) {
            if( (rhs + m) == rhs ) {
                return 0.0;
            } else {
                return rhs + m;
            }
        }
    } else {
        if( m <= rhs ) {
            return 0.0;
        }
        if( m > 0.0 ) {
            if( ( rhs + m ) == rhs ) {
                return 0.0;
            } else {
                return rhs + m;
            }
        }
    }

    return m;
}

f32 sin32( f32 x ) {
    x = wrap_rad32(x);

    f32 pow3  = x * x * x;
    f32 pow5  = pow3 * x * x;
    f32 pow7  = pow5 * x * x;
    f32 pow9  = pow7 * x * x;
    f32 pow11 = pow9 * x * x;

    return x -
        ( pow3  / F32_THREE_FACTORIAL ) +
        ( pow5  / F32_FIVE_FACTORIAL  ) -
        ( pow7  / F32_SEVEN_FACTORIAL ) +
        ( pow9  / F32_NINE_FACTORIAL  ) -
        ( pow11 / F32_ELEVEN_FACTORIAL );
}
f64 sin64( f64 x ) {
    x = wrap_rad64(x);

    f64 pow3  = x * x * x;
    f64 pow5  = pow3 * x * x;
    f64 pow7  = pow5 * x * x;
    f64 pow9  = pow7 * x * x;
    f64 pow11 = pow9 * x * x;

    return x -
        ( pow3  / F64_THREE_FACTORIAL ) +
        ( pow5  / F64_FIVE_FACTORIAL  ) -
        ( pow7  / F64_SEVEN_FACTORIAL ) +
        ( pow9  / F64_NINE_FACTORIAL  ) -
        ( pow11 / F64_ELEVEN_FACTORIAL );
}
f32 cos32( f32 x ) {
    x = wrap_rad32(x);

    f32 pow2  = x * x;
    f32 pow4  = pow2 * x * x;
    f32 pow6  = pow4 * x * x;
    f32 pow8  = pow6 * x * x;
    f32 pow10 = pow8 * x * x;

    return 1.0f -
        ( pow2  / F32_TWO_FACTORIAL ) +
        ( pow4  / F32_FOUR_FACTORIAL )  -
        ( pow6  / F32_SIX_FACTORIAL ) +
        ( pow8  / F32_EIGHT_FACTORIAL ) -
        ( pow10 / F32_TEN_FACTORIAL );
}
f64 cos64( f64 x ) {
    x = wrap_rad64(x);

    f64 pow2  = x * x;
    f64 pow4  = pow2 * x * x;
    f64 pow6  = pow4 * x * x;
    f64 pow8  = pow6 * x * x;
    f64 pow10 = pow8 * x * x;

    return 1.0 -
        ( pow2  / F64_TWO_FACTORIAL ) +
        ( pow4  / F64_FOUR_FACTORIAL )  -
        ( pow6  / F64_SIX_FACTORIAL ) +
        ( pow8  / F64_EIGHT_FACTORIAL ) -
        ( pow10 / F64_TEN_FACTORIAL );
}
f32 tan32( f32 x ) {
    f32 sin, cos;
    sincos32( x, &sin, &cos );
    return cos == 0.0f ? F32_NAN : sin / cos;
}
f64 tan64( f64 x ) {
    f64 sin, cos;
    sincos64( x, &sin, &cos );
    return cos == 0.0 ? F64_NAN : sin / cos;
}

void sincos32( f32 x, f32* out_sin, f32* out_cos ) {
    // TODO(alicia): 
    *out_sin = sin32( x );
    *out_cos = cos32( x );
}
void sincos64( f64 x, f64* out_sin, f64* out_cos ) {
    // TODO(alicia): 
    *out_sin = sin64( x );
    *out_cos = cos64( x );
}

f32 asin32( f32 x ) {
    // NOTE(alicia): don't ask me how i figured this shit out
    // i don't even know
    f32 sign_of_x = signum( x );
    f32 x_abs = x * sign_of_x;
    f32 x_sqr = x_abs * x_abs;

    const f32 magic_0 =  1.5707288f;
    const f32 magic_1 = -0.2121144f;
    const f32 magic_2 =  0.0742610f;
    const f32 magic_3 = -0.0187293f;

    f32 result = F32_HALF_PI - sqrt32( 1.0f - x_abs ) * (
        magic_0 +
        ( magic_1 * x_abs ) +
        ( magic_2 * x_sqr ) +
        ( magic_3 * ( x_sqr * x_abs ) )
    );

    return result * sign_of_x;

}
f64 asin64( f64 x ) {
    f64 sign_of_x = signum( x );
    f64 x_abs = x * sign_of_x;
    f64 x_sqr = x_abs * x_abs;

    const f64 magic_0 =  1.5707288;
    const f64 magic_1 = -0.2121144;
    const f64 magic_2 =  0.0742610;
    const f64 magic_3 = -0.0187293;

    f64 result = F64_HALF_PI - sqrt64( 1.0 - x_abs ) * (
        magic_0 +
        ( magic_1 * x_abs ) +
        ( magic_2 * x_sqr ) +
        ( magic_3 * ( x_sqr * x_abs ) )
    );

    return result * sign_of_x;

}
f32 acos32( f32 x ) {
    return -asin32( x ) + F32_HALF_PI;
}
f64 acos64( f64 x ) {
    return -asin64( x ) + F64_HALF_PI;
}
f32 atan32( f32 x ) {
    f32 pow3  = x * x * x;
    f32 pow5  = pow3 * x * x;
    f32 pow7  = pow5 * x * x;
    f32 pow9  = pow7 * x * x;
    f32 pow11 = pow9 * x * x;
    f32 pow13 = pow11 * x * x;
    return x -
        ( pow3  / 3.0f ) +
        ( pow5  / 5.0f ) -
        ( pow7  / 7.0f ) +
        ( pow9  / 9.0f ) -
        ( pow11 / 11.0f ) +
        ( pow13 / 13.0f );
}
f64 atan64( f64 x ) {
    f64 pow3  = x * x * x;
    f64 pow5  = pow3 * x * x;
    f64 pow7  = pow5 * x * x;
    f64 pow9  = pow7 * x * x;
    f64 pow11 = pow9 * x * x;
    f64 pow13 = pow11 * x * x;
    return x -
        ( pow3  / 3.0 ) +
        ( pow5  / 5.0 ) -
        ( pow7  / 7.0 ) +
        ( pow9  / 9.0 ) -
        ( pow11 / 11.0 ) +
        ( pow13 / 13.0 );
}
f32 atan2_32( f32 y, f32 x ) {
    if( y == 0.0f ) {
        if( x < 0.0f ) {
            return F32_PI;
        } else if( x == 0.0f ) {
            return F32_NAN;
        }
    }

    f32 x_sqr = x * x;
    f32 y_sqr = y * y;
    return 2.0f * atan32( y / ( sqrt32( x_sqr + y_sqr ) + x ) );
}
f64 atan2_64( f64 y, f64 x ) {
    if( y == 0.0 ) {
        if( x < 0.0 ) {
            return F64_PI;
        } else if( x == 0.0 ) {
            return F64_NAN;
        }
    }

    f64 x_sqr = x * x;
    f64 y_sqr = y * y;
    return 2.0 * atan64( y / ( sqrt64( x_sqr + y_sqr ) + x ) );
}

f32 exp32( f32 x ) {
    if( x < -4.0f ) {
        return 0.0f;
    }
    f32 p2  = x * x;
    f32 p3  = x * x * x;
    f32 p4  = x * x * x * x;
    f32 p5  = x * x * x * x * x;
    f32 p6  = x * x * x * x * x * x;
    f32 p7  = x * x * x * x * x * x * x;
    f32 p8  = x * x * x * x * x * x * x * x;
    f32 p9  = x * x * x * x * x * x * x * x * x;
    f32 p10 = x * x * x * x * x * x * x * x * x * x;
    f32 p11 = x * x * x * x * x * x * x * x * x * x * x;

    f32 r = 1 + x +
        p2  / (F32_TWO_FACTORIAL)   +
        p3  / (F32_THREE_FACTORIAL) +
        p4  / (F32_FOUR_FACTORIAL)  +
        p5  / (F32_FIVE_FACTORIAL)  +
        p6  / (F32_SIX_FACTORIAL)   +
        p7  / (F32_SEVEN_FACTORIAL) +
        p8  / (F32_EIGHT_FACTORIAL) +
        p9  / (F32_NINE_FACTORIAL)  +
        p10 / (F32_TEN_FACTORIAL)  +
        p11 / (F32_ELEVEN_FACTORIAL);

    return r;
}
f64 exp64( f64 x ) {
    if( x < -4.0 ) {
        return 0.0;
    }
    f64 p2  = x * x;
    f64 p3  = x * x * x;
    f64 p4  = x * x * x * x;
    f64 p5  = x * x * x * x * x;
    f64 p6  = x * x * x * x * x * x;
    f64 p7  = x * x * x * x * x * x * x;
    f64 p8  = x * x * x * x * x * x * x * x;
    f64 p9  = x * x * x * x * x * x * x * x * x;
    f64 p10 = x * x * x * x * x * x * x * x * x * x;
    f64 p11 = x * x * x * x * x * x * x * x * x * x * x;

    f64 r = 1 + x +
        p2  / (F64_TWO_FACTORIAL)   +
        p3  / (F64_THREE_FACTORIAL) +
        p4  / (F64_FOUR_FACTORIAL)  +
        p5  / (F64_FIVE_FACTORIAL)  +
        p6  / (F64_SIX_FACTORIAL)   +
        p7  / (F64_SEVEN_FACTORIAL) +
        p8  / (F64_EIGHT_FACTORIAL) +
        p9  / (F64_NINE_FACTORIAL)  +
        p10 / (F64_TEN_FACTORIAL)  +
        p11 / (F64_ELEVEN_FACTORIAL);

    return r;
}
f32 ln32( f32 x ) {
    if( x < 0.0f ) {
        return F32_NAN;
    }

    f32 div = ( x - 1.0f ) / ( x + 1.0f );

    f32 p3 = div * div * div;
    f32 p5 = div * div * div * div * div;
    f32 p7 = div * div * div * div * div * div * div;
    f32 p9 = div * div * div * div * div * div * div * div * div;

    f32 r3 = ( 1.0f / 3.0f ) * p3;
    f32 r5 = ( 1.0f / 5.0f ) * p5;
    f32 r7 = ( 1.0f / 7.0f ) * p7;
    f32 r9 = ( 1.0f / 9.0f ) * p9;
    
    return 2.0f * (div + r3 + r5 + r7 + r9);
}
f64 ln64( f64 x ) {
    if( x < 0.0 ) {
        return F64_NAN;
    }

    f64 div = ( x - 1.0 ) / ( x + 1.0 );

    f64 p3 = div * div * div;
    f64 p5 = div * div * div * div * div;
    f64 p7 = div * div * div * div * div * div * div;
    f64 p9 = div * div * div * div * div * div * div * div * div;

    f64 r3 = ( 1.0 / 3.0 ) * p3;
    f64 r5 = ( 1.0 / 5.0 ) * p5;
    f64 r7 = ( 1.0 / 7.0 ) * p7;
    f64 r9 = ( 1.0 / 9.0 ) * p9;
    
    return 2.0 * (div + r3 + r5 + r7 + r9);
}
f32 log2_32( f32 x ) {
    if( x < 0.0f ) {
        return F32_NAN;
    }
    if( x == 2.0f ) {
        return 1.0f;
    }
    return ln32( x ) * 1.49f;
}
f64 log2_64( f64 x ) {
    if( x < 0.0 ) {
        return F64_NAN;
    }
    if( x == 2.0 ) {
        return 1.0;
    }
    return ln64( x ) * 1.49;
}
f32 log10_32( f32 x ) {
    if( x < 0.0f ) {
        return F32_NAN;
    }
    if( x == 10.0f ) {
        return 1.0f;
    }
    return ln32( x ) / 2.3f;
}
f64 log10_64( f64 x ) {
    if( x < 0.0f ) {
        return F64_NAN;
    }
    if( x == 10.0 ) {
        return 1.0f;
    }
    return ln64( x ) / 2.3f;
}

f32 lerp32( f32 a, f32 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
f64 lerp64( f64 a, f64 b, f64 t ) {
    return ( 1.0 - t ) * a + b * t;
}
f32 inv_lerp32( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}
f64 inv_lerp64( f64 a, f64 b, f64 v ) {
    return ( v - a ) / ( b - a );
}
f32 remap32( f32 imin, f32 imax, f32 omin, f32 omax, f32 v ) {
    f32 t = inv_lerp32( imin, imax, v );
    return lerp32( omin, omax, t );
}
f64 remap64( f64 imin, f64 imax, f64 omin, f64 omax, f64 v ) {
    f64 t = inv_lerp64( imin, imax, v );
    return lerp64( omin, omax, t );
}

f32 smooth_step32( f32 a, f32 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
f64 smooth_step64( f64 a, f64 b, f64 t ) {
    return ( b - a ) * ( 3.0 - t * 2.0 ) * t * t + a;
}
f32 smoother_step32( f32 a, f32 b, f32 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
f64 smoother_step64( f64 a, f64 b, f64 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0 - 15.0 ) + 10.0 ) * t * t * t ) + a;
}

vec2 v2_neg( vec2 v ) {
    vec2 result;
    result.x = -v.x;
    result.y = -v.y;
    return result;
}
vec2 v2_add( vec2 lhs, vec2 rhs ) {
    vec2 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;
}
vec2 v2_sub( vec2 lhs, vec2 rhs ) {
    vec2 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}
vec2 v2_mul( vec2 lhs, f32 rhs ) {
    vec2 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    return result;
}
vec2 v2_div( vec2 lhs, f32 rhs ) {
    vec2 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    return result;
}
f32 v2_hadd( vec2 v ) {
    return v.x + v.y;
}
f32 v2_hmul( vec2 v ) {
    return v.x * v.y;
}
vec2 v2_hadamard( vec2 lhs, vec2 rhs ) {
    vec2 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    return result;
}
f32 v2_aspect_ratio( vec2 v ) {
    return v.x / v.y;
}
f32 v2_dot( vec2 lhs, vec2 rhs ) {
    return v2_hadd( v2_hadamard( lhs, rhs ) );
}
vec2 v2_rotate( vec2 v, f32 theta_radians ) {
    f32 sin, cos;
    sincos32( theta_radians, &sin, &cos );
    vec2 a = {  cos, sin };
    vec2 b = { -sin, cos };

    a = v2_mul( a, v.x );
    b = v2_mul( b, v.y );

    return v2_add( a, b );
}
vec2 v2_clamp_mag( vec2 v, f32 min, f32 max ) {
    assert( min > 0.0f );
    assert( max > 0.0f );
    assert( max >= min );

    f32 mag = v2_mag( v );
    f32 new_mag = clamp( mag, min, max );

    return v2_mul( v2_div( v, mag ), new_mag );
}
b32 v2_cmp( vec2 a, vec2 b ) {
    return v2_sqrmag( v2_sub( a, b ) ) < F32_EPSILON;
}
vec2 v2_swap( vec2 v ) {
    vec2 result;
    result.x = v.y;
    result.y = v.x;
    return result;
}
f32 v2_sqrmag( vec2 v ) {
    return v2_dot( v, v );
}
f32 v2_mag( vec2 v ) {
    return sqrt32( v2_sqrmag( v ) );
}
vec2 v2_normalize( vec2 v ) {
    f32 mag = v2_mag( v );
    return mag == 0.0f ? VEC2_ZERO : v2_div( v, mag );
}
f32 v2_angle( vec2 lhs, vec2 rhs ) {
    return acos32( v2_dot( lhs, rhs ) );
}
vec2 v2_lerp( vec2 a, vec2 b, f32 t ) {
    vec2 result;
    result.x = lerp32( a.x, b.x, t );
    result.y = lerp32( a.y, b.y, t );
    return result;
}
vec2 v2_smooth_step( vec2 a, vec2 b, f32 t ) {
    vec2 result;
    result.x = smooth_step32( a.x, b.x, t );
    result.y = smooth_step32( a.y, b.y, t );
    return result;
}
vec2 v2_smoother_step( vec2 a, vec2 b, f32 t ) {
    vec2 result;
    result.x = smoother_step32( a.x, b.x, t );
    result.y = smoother_step32( a.y, b.y, t );
    return result;
}

ivec2 iv2_neg( ivec2 v ) {
    ivec2 result;
    result.x = -v.x;
    result.y = -v.y;
    return result;
}
ivec2 iv2_add( ivec2 lhs, ivec2 rhs ) {
    ivec2 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;
}
ivec2 iv2_sub( ivec2 lhs, ivec2 rhs ) {
    ivec2 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}
ivec2 iv2_mul( ivec2 lhs, i32 rhs ) {
    ivec2 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    return result;
}
ivec2 iv2_div( ivec2 lhs, i32 rhs ) {
    ivec2 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    return result;
}
i32 iv2_hadd( ivec2 v ) {
    return v.x + v.y;
}
i32 iv2_hmul( ivec2 v ) {
    return v.x * v.y;
}
ivec2 iv2_hadamard( ivec2 lhs, ivec2 rhs ) {
    ivec2 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    return result;
}
f32 iv2_aspect_ratio( ivec2 v ) {
    return (f32)v.x / (f32)v.y;
}
f32 iv2_dot( ivec2 lhs, ivec2 rhs ) {
    return (f32)iv2_hadd( iv2_hadamard( lhs, rhs ) );
}
b32 iv2_cmp( ivec2 a, ivec2 b ) {
    return a.x == b.x && a.y == b.y;
}
ivec2 iv2_swap( ivec2 v ) {
    ivec2 result;
    result.x = v.y;
    result.y = v.x;
    return result;
}
f32 iv2_sqrmag( ivec2 v ) {
    return iv2_dot( v, v );
}
f32 iv2_mag( ivec2 v ) {
    return sqrt32( iv2_sqrmag( v ) );
}
f32 iv2_angle( ivec2 lhs, ivec2 rhs ) {
    return acos32( iv2_dot( lhs, rhs ) );
}

hsv v3_hsv( f32 hue, f32 saturation, f32 value ) {
    hsv result;
    result.hue        = wrap_deg32( hue );
    result.saturation = clamp32_01( saturation );
    result.value      = clamp32_01( value );
    return result;
}
hsv rgb_to_hsv( rgb col ) {
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
rgb hsv_to_rgb( hsv col ) {
    f32 chroma    = col.value * col.saturation;
    f32 hue       = col.hue / 60.0f;
    i32 hue_index = floor32_i32( hue );

    f32 x = chroma * ( 1.0f - absolute( mod32( hue, 2.0f ) - 1.0f ) );

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
rgb linear_to_srgb( rgb linear ) {
    #define LINEAR_TO_SRGB_POW ( 1.0f / 2.2f )
    rgb result;
    result.r = pow32( linear.r, LINEAR_TO_SRGB_POW );
    result.g = pow32( linear.g, LINEAR_TO_SRGB_POW );
    result.b = pow32( linear.b, LINEAR_TO_SRGB_POW );
    return result;
}
rgb srgb_to_linear( rgb srgb ) {
    #define SRGB_TO_LINEAR_POW ( 2.2f )
    rgb result;
    result.r = pow32( srgb.r, SRGB_TO_LINEAR_POW );
    result.g = pow32( srgb.g, SRGB_TO_LINEAR_POW );
    result.b = pow32( srgb.b, SRGB_TO_LINEAR_POW );
    return result;
}
vec3 v3_neg( vec3 v ) {
    vec3 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    return result;
}
vec3 v3_add( vec3 lhs, vec3 rhs ) {
    vec3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}
vec3 v3_sub( vec3 lhs, vec3 rhs ) {
    vec3 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}
vec3 v3_mul( vec3 lhs, f32 rhs ) {
    vec3 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;
    return result;
}
vec3 v3_div( vec3 lhs, f32 rhs ) {
    vec3 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;
    return result;
}
f32 v3_hadd( vec3 v ) {
    return v.x + v.y + v.z;
}
f32 v3_hmul( vec3 v ) {
    return v.x * v.y * v.z;
}
vec3 v3_hadamard( vec3 lhs, vec3 rhs ) {
    vec3 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    return result;
}
vec3 v3_cross( vec3 lhs, vec3 rhs ) {
    vec3 result;
    result.x = ( lhs.y * rhs.z ) - ( lhs.z * rhs.y );
    result.y = ( lhs.z * rhs.x ) - ( lhs.x * rhs.z );
    result.z = ( lhs.x * rhs.y ) - ( lhs.y * rhs.x );
    return result;
}
f32 v3_dot( vec3 lhs, vec3 rhs ) {
    return v3_hadd( v3_hadamard( lhs, rhs ) );
}
vec3 v3_reflect( vec3 direction, vec3 normal ) {
    return v3_mul(
        v3_sub( normal, direction ),
        2.0f * v3_dot( direction, normal ) );
}
vec3 v3_clamp_mag( vec3 v, f32 min, f32 max ) {
    assert( min > 0.0f );
    assert( max > 0.0f );
    assert( max >= min );

    f32 mag = v3_mag( v );
    f32 new_mag = clamp( mag, min, max );

    return v3_mul( v3_div( v, mag ), new_mag );
}
b32 v3_cmp( vec3 a, vec3 b ) {
    return v3_sqrmag( v3_sub( a, b ) ) < F32_EPSILON;
}
vec3 v3_shift_left( vec3 v ) {
    vec3 result;
    result.x = v.y;
    result.y = v.z;
    result.z = v.x;
    return result;
}
vec3 v3_shift_right( vec3 v ) {
    vec3 result;
    result.x = v.z;
    result.y = v.x;
    result.z = v.y;
    return result;
}
f32 v3_sqrmag( vec3 v ) {
    return v3_hadd( v3_hadamard( v, v ) );
}
f32 v3_mag( vec3 v ) {
    return sqrt32( v3_sqrmag( v ) );
}
vec3 v3_normalize( vec3 v ) {
    f32 mag = v3_mag( v );
    return mag == 0.0f ? VEC3_ZERO : v3_div( v, mag );
}
f32 v3_angle( vec3 lhs, vec3 rhs ) {
    return acos32( v3_dot( lhs, rhs ) );
}
vec3 v3_lerp( vec3 a, vec3 b, f32 t ) {
    vec3 result;
    result.x = lerp32( a.x, b.x, t );
    result.y = lerp32( a.y, b.y, t );
    result.z = lerp32( a.z, b.z, t );
    return result;
}
vec3 v3_smooth_step( vec3 a, vec3 b, f32 t ) {
    vec3 result;
    result.x = smooth_step32( a.x, b.x, t );
    result.y = smooth_step32( a.y, b.y, t );
    result.z = smooth_step32( a.z, b.z, t );
    return result;
}
vec3 v3_smoother_step( vec3 a, vec3 b, f32 t ) {
    vec3 result;
    result.x = smoother_step32( a.x, b.x, t );
    result.y = smoother_step32( a.y, b.y, t );
    result.z = smoother_step32( a.z, b.z, t );
    return result;
}

ivec3 iv3_neg( ivec3 v ) {
    ivec3 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    return result;
}
ivec3 iv3_add( ivec3 lhs, ivec3 rhs ) {
    ivec3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}
ivec3 iv3_sub( ivec3 lhs, ivec3 rhs ) {
    ivec3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;   
}
ivec3 iv3_mul( ivec3 lhs, i32 rhs ) {
    ivec3 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;
    return result;
}
ivec3 iv3_div( ivec3 lhs, i32 rhs ) {
    ivec3 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;
    return result;
}
i32 iv3_hadd( ivec3 v ) {
    return v.x + v.y + v.z;
}
i32 iv3_hmul( ivec3 v ) {
    return v.x * v.y * v.z;
}
ivec3 iv3_hadamard( ivec3 lhs, ivec3 rhs ) {
    ivec3 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    return result;
}
f32 iv3_dot( ivec3 lhs, ivec3 rhs ) {
    return iv3_hadd( iv3_hadamard( lhs, rhs ) );
}
b32 iv3_cmp( ivec3 a, ivec3 b ) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
ivec3 iv3_shift_left( ivec3 v ) {
    ivec3 result;
    result.x = v.y;
    result.y = v.z;
    result.z = v.x;
    return result;
}
ivec3 iv3_shift_right( ivec3 v ) {
    ivec3 result;
    result.x = v.z;
    result.y = v.x;
    result.z = v.y;
    return result;
}
f32 iv3_sqrmag( ivec3 v ) {
    return iv3_hadd( iv3_hadamard( v, v ) );
}
f32 iv3_mag( ivec3 v ) {
    return sqrt32( iv3_sqrmag( v ) );
}
f32 iv3_angle( ivec3 lhs, ivec3 rhs ) {
    return acos32( iv3_dot( lhs, rhs ) );
}

vec4 v4_neg( vec4 v ) {
    vec4 result;
    result.x = -v.x;
    result.y = -v.y;
    result.z = -v.z;
    result.w = -v.w;
    return result;
}
vec4 v4_add( vec4 lhs, vec4 rhs ) {
    vec4 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;
}
vec4 v4_sub( vec4 lhs, vec4 rhs ) {
    vec4 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    result.w = lhs.w - rhs.w;
    return result;
}
vec4 v4_mul( vec4 lhs, f32 rhs ) {
    vec4 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;
    result.w = lhs.w * rhs;
    return result;
}
vec4 v4_div( vec4 lhs, f32 rhs ) {
    vec4 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;
    result.w = lhs.w / rhs;
    return result;
}
f32 v4_hadd( vec4 v ) {
    return v.x + v.y + v.z + v.w;
}
f32 v4_hmul( vec4 v ) {
    return v.x * v.y * v.z * v.w;
}
vec4 v4_hadamard( vec4 lhs, vec4 rhs ) {
    vec4 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    result.w = lhs.w * rhs.w;
    return result;
}
f32 v4_dot( vec4 lhs, vec4 rhs ) {
    return v4_hadd( v4_hadamard( lhs, rhs ) );
}
vec4 v4_clamp_mag( vec4 v, f32 min, f32 max ) {
    assert( min > 0.0f );
    assert( max > 0.0f );
    assert( max >= min );

    f32 mag = v4_mag( v );
    f32 new_mag = clamp( mag, min, max );

    return v4_mul( v4_div( v, mag ), new_mag );
}
b32 v4_cmp( vec4 a, vec4 b ) {
    return v4_sqrmag( v4_sub( a, b ) ) < F32_EPSILON;
}
vec4 v4_shift_left( vec4 v ) {
    vec4 result;
    result.x = v.y;
    result.y = v.z;
    result.z = v.w;
    result.w = v.x;
    return result;
}
vec4 v4_shift_right( vec4 v ) {
    vec4 result;
    result.x = v.w;
    result.y = v.x;
    result.z = v.y;
    result.w = v.z;
    return result;
}
f32 v4_sqrmag( vec4 v ) {
    return v4_hadd( v4_hadamard( v, v ) );
}
f32 v4_mag( vec4 v ) {
    return sqrt32( v4_sqrmag( v ) );
}
vec4 v4_normalize( vec4 v ) {
    f32 mag = v4_mag( v );
    return mag == 0.0f ? VEC4_ZERO : v4_div( v, mag );
}
f32 v4_angle( vec4 lhs, vec4 rhs ) {
    return acos32( v4_dot( lhs, rhs ) );
}
vec4 v4_lerp( vec4 a, vec4 b, f32 t ) {
    vec4 result;
    result.x = lerp32( a.x, b.x, t );
    result.y = lerp32( a.y, b.y, t );
    result.z = lerp32( a.z, b.z, t );
    result.w = lerp32( a.w, b.w, t );
    return result;
}
vec4 v4_smooth_step( vec4 a, vec4 b, f32 t ) {
    vec4 result;
    result.x = smooth_step32( a.x, b.x, t );
    result.y = smooth_step32( a.y, b.y, t );
    result.z = smooth_step32( a.z, b.z, t );
    result.w = smooth_step32( a.w, b.w, t );
    return result;
}
vec4 v4_smoother_step( vec4 a, vec4 b, f32 t ) {
    vec4 result;
    result.x = smoother_step32( a.x, b.x, t );
    result.y = smoother_step32( a.y, b.y, t );
    result.z = smoother_step32( a.z, b.z, t );
    result.w = smoother_step32( a.w, b.w, t );
    return result;
}

LD_API ivec4 iv4_neg( ivec4 v ) {
    return (ivec4){ -v.x, -v.y, -v.z, -v.w };
}
LD_API ivec4 iv4_add( ivec4 lhs, ivec4 rhs ) {
    ivec4 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;
}
LD_API ivec4 iv4_sub( ivec4 lhs, ivec4 rhs ) {
    ivec4 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    result.w = lhs.w - rhs.w;
    return result;
}
LD_API ivec4 iv4_mul( ivec4 lhs, i32 rhs ) {
    ivec4 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;
    result.w = lhs.w * rhs;
    return result;
}
LD_API ivec4 iv4_div( ivec4 lhs, i32 rhs ) {
    ivec4 result;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;
    result.w = lhs.w / rhs;
    return result;
}
LD_API i32 iv4_hadd( ivec4 v ) {
    return v.x + v.y + v.z + v.w;
}
LD_API i32 iv4_hmul( ivec4 v ) {
    return v.x * v.y * v.z * v.w;
}
LD_API ivec4 iv4_hadamard( ivec4 lhs, ivec4 rhs ) {
    ivec4 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    result.w = lhs.w * rhs.w;
    return result;   
}
LD_API f32 iv4_dot( ivec4 lhs, ivec4 rhs ) {
    return iv4_hadd( iv4_hadamard( lhs, rhs ) );
}
LD_API b32 iv4_cmp( ivec4 a, ivec4 b ) {
    return
        a.x == b.x &&
        a.y == b.y &&
        a.z == b.z &&
        a.w == b.w;
}
LD_API ivec4 iv4_shift_left( ivec4 v ) {
    ivec4 result;
    result.x = v.y;
    result.y = v.z;
    result.z = v.w;
    result.w = v.x;
    return result;
}
LD_API ivec4 iv4_shift_right( ivec4 v ) {
    ivec4 result;
    result.x = v.w;
    result.y = v.x;
    result.z = v.y;
    result.w = v.z;
    return result;
}
LD_API f32 iv4_sqrmag( ivec4 v ) {
    return iv4_hadd( iv4_hadamard( v, v ) );
}
LD_API f32 iv4_mag( ivec4 v ) {
    return sqrt32( iv4_sqrmag( v ) );
}

quat q_angle_axis( f32 angle, vec3 axis ) {
    f32 half_angle = angle / 2.0f;
    f32 sin, cos;
    sincos32( half_angle, &sin, &cos );
    quat result;
    result.w = cos;
    result.x = axis.x * sin;
    result.y = axis.y * sin;
    result.z = axis.z * sin;
    return q_normalize( result );
}
quat q_euler( f32 pitch, f32 yaw, f32 roll ) {
    f32 half_x = pitch / 2.0f;
    f32 half_y = yaw   / 2.0f;
    f32 half_z = roll  / 2.0f;

    f32 x_sin, y_sin, z_sin;
    f32 x_cos, y_cos, z_cos;

    sincos32( half_x, &x_sin, &x_cos );
    sincos32( half_y, &y_sin, &y_cos );
    sincos32( half_z, &z_sin, &z_cos );

    f32 xyz_cos = x_cos * y_cos * z_cos;
    f32 xyz_sin = x_sin * y_sin * z_sin;

    return (quat){
        ( xyz_cos ) + ( xyz_sin ),
        ( x_sin * y_cos * z_cos ) + ( x_cos * y_sin * z_sin ),
        ( x_cos * y_sin * z_cos ) + ( x_sin * y_cos * z_sin ),
        ( x_cos * y_cos * z_sin ) + ( x_sin * y_sin * z_cos )
    };
}
quat q_neg( quat q ) {
    quat result;
    result.w = -q.w;
    result.x = -q.x;
    result.y = -q.y;
    result.z = -q.z;
    return result;
}
quat q_add( quat lhs, quat rhs ) {
    quat result;
    result.w = lhs.w + rhs.w;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}
quat q_sub( quat lhs, quat rhs ) {
    quat result;
    result.w = lhs.w - rhs.w;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}
quat q_mul( quat lhs, f32 rhs ) {
    quat result;
    result.w = lhs.w * rhs;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;
    return result;
}
quat q_mul_q( quat lhs, quat rhs ) {
    const Lane4f negate_first128 =
        lane4f_set( -1.0f, 1.0f, 1.0f, 1.0f );

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

    Lane4f w = lane4f_scalar( lhs.w );
    b = lane4f_set( rhs.w, rhs.x, rhs.y, rhs.z );
    w = lane4f_mul( w, b );

    quat result;
    lane4f_store( lane4f_add( w, a ), result.q );
    return result;
}
vec3 q_mul_v3( quat lhs, vec3 rhs ) {
    vec3 t = v3_mul( v3_cross( lhs.xyz, rhs ), 2.0f );
    return
        v3_add(
            v3_add( rhs, v3_mul( t, lhs.w ) ),
            v3_cross( lhs.xyz, t )
        );
}
quat q_div( quat lhs, f32 rhs ) {
    quat result;
    result.w = lhs.w / rhs;
    result.x = lhs.x / rhs;
    result.y = lhs.y / rhs;
    result.z = lhs.z / rhs;
    return result;
}
b32 q_cmp( quat a, quat b ) {
    return q_sqrmag( q_sub( a, b ) ) < F32_EPSILON;
}
f32 q_sqrmag( quat q ) {
    vec4 result = { q.w, q.x, q.y, q.z };
    return v4_hadd( v4_hadamard( result, result ) );
}
f32 q_mag( quat q ) {
    return sqrt32( q_sqrmag( q ) );
}
quat q_normalize( quat q ) {
    f32 mag = q_mag( q );
    return mag == 0.0f ? QUAT_IDENTITY : q_div( q, mag );
}
quat q_conjugate( quat q ) {
    quat result;
    result.w =  q.w;
    result.x = -q.x;
    result.y = -q.y;
    result.z = -q.z;
    return result;
}
quat q_inverse( quat q ) {
    return q_div( q_conjugate( q ), q_sqrmag( q ) );
}
f32 q_angle( quat lhs, quat rhs ) {
    quat lmulr = q_mul_q( q_inverse( lhs ), rhs );
    return 2.0f * atan2_32( v3_mag( lmulr.xyz ), lmulr.w );
}
f32 q_dot( quat lhs, quat rhs ) {
    vec4 l = { lhs.w, lhs.x, lhs.y, lhs.z };
    vec4 r = { rhs.w, rhs.x, rhs.y, rhs.z };
    return v4_hadd( v4_hadamard( l, r ) );
}
quat q_lerp( quat a, quat b, f32 t ) {
    quat result;
    result.w = lerp32( a.w, b.w, t );
    result.x = lerp32( a.x, b.x, t );
    result.y = lerp32( a.y, b.y, t );
    result.z = lerp32( a.z, b.z, t );
    return q_normalize( result );
}
quat q_slerp( quat a, quat b, f32 t ) {
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

mat2 m2_add( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = v2_add( lhs.col0, rhs.col0 );
    result.col1 = v2_add( lhs.col1, rhs.col1 );
    return result;
}
mat2 m2_sub( mat2 lhs, mat2 rhs ) {
    mat2 result;
    result.col0 = v2_sub( lhs.col0, rhs.col0 );
    result.col1 = v2_sub( lhs.col1, rhs.col1 );
    return result;
}
mat2 m2_mul( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = v2_mul( lhs.col0, rhs );
    result.col1 = v2_mul( lhs.col1, rhs );
    return result;
}
mat2 m2_mul_m2( mat2 lhs, mat2 rhs ) {
    mat2 result = {
        (lhs.c[0] * rhs.c[0]) + (lhs.c[2] * rhs.c[1]),
        (lhs.c[1] * rhs.c[0]) + (lhs.c[3] * rhs.c[2]),

        (lhs.c[0] * rhs.c[2]) + (lhs.c[2] * rhs.c[3]),
        (lhs.c[1] * rhs.c[2]) + (lhs.c[3] * rhs.c[3])
    };
    return result;
}
mat2 m2_div( mat2 lhs, f32 rhs ) {
    mat2 result;
    result.col0 = v2_div( lhs.col0, rhs );
    result.col1 = v2_div( lhs.col1, rhs );
    return result;
}
mat2 m2_transpose( mat2 m ) {
    return (mat2){
        m.c[0], m.c[2],
        m.c[1], m.c[3]
    };
}
f32 m2_determinant( mat2 m ) {
    return ( m.c[0] * m.c[3] ) - ( m.c[2] * m.c[1] );
}

mat3 m3_add( const mat3* lhs, const mat3* rhs ) {
    mat3 result;
    result.col0 = v3_add( lhs->col0, rhs->col0 );
    result.col1 = v3_add( lhs->col1, rhs->col1 );
    result.col2 = v3_add( lhs->col2, rhs->col2 );
    return result;
}
mat3 m3_sub( const mat3* lhs, const mat3* rhs ) {
    mat3 result;
    result.col0 = v3_sub( lhs->col0, rhs->col0 );
    result.col1 = v3_sub( lhs->col1, rhs->col1 );
    result.col2 = v3_sub( lhs->col2, rhs->col2 );
    return result;
}
mat3 m3_mul( const mat3* lhs, f32 rhs ) {
    mat3 result;
    result.col0 = v3_mul( lhs->col0, rhs );
    result.col1 = v3_mul( lhs->col1, rhs );
    result.col2 = v3_mul( lhs->col2, rhs );
    return result;
}
mat3 m3_mul_m3( const mat3* lhs, const mat3* rhs ) {
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
mat3 m3_div( const mat3* lhs, f32 rhs ) {
    mat3 result;
    result.col0 = v3_div( lhs->col0, rhs );
    result.col1 = v3_div( lhs->col1, rhs );
    result.col2 = v3_div( lhs->col2, rhs );
    return result;
}
mat3 m3_transpose( const mat3* m ) {
    return (mat3){
        m->c[0], m->c[3], m->c[6],
        m->c[1], m->c[4], m->c[7],
        m->c[2], m->c[5], m->c[8]
    };
}
f32 m3_determinant( const mat3* m ) {
    return
    ( m->c[0] * ( ( m->c[4] * m->c[8] ) - ( m->c[7] * m->c[5] ) ) ) -
    ( m->c[3] * ( ( m->c[1] * m->c[8] ) - ( m->c[7] * m->c[2] ) ) ) +
    ( m->c[6] * ( ( m->c[1] * m->c[5] ) - ( m->c[4] * m->c[2] ) ) );
}

mat4 m4_view( vec3 position, vec3 target, vec3 up ) {
    vec3 z = v3_normalize( v3_sub( target, position ) );
    vec3 x = v3_normalize( v3_cross( z, up ) );
    vec3 y = v3_cross( x, z );
    z = v3_neg( z );

    f32 dx = -v3_dot( x, position );
    f32 dy = -v3_dot( y, position );
    f32 dz = -v3_dot( z, position );

    return (mat4){
        x.x, y.x, z.x, 0.0f,
        x.y, y.y, z.y, 0.0f,
        x.z, y.z, z.z, 0.0f,
         dx,  dy,  dz, 1.0f
    };
}
mat4 m4_ortho(
    f32 left, f32 right,
    f32 bottom, f32 top,
    f32 near_clip, f32 far_clip
) {
    mat4 result = MAT4_IDENTITY;
    f32 r_sub_l = right - left;
    f32 t_sub_b = top   - bottom;
    f32 f_sub_n = far_clip  - near_clip;

    result.c[ 0] =  2.0f / r_sub_l;
    result.c[ 5] =  2.0f / t_sub_b;
    result.c[10] = -2.0f / f_sub_n;
    result.c[12] = -( right + left   ) / r_sub_l;
    result.c[13] = -( top   + bottom ) / t_sub_b;
    result.c[14] = -( far_clip  + near_clip  ) / f_sub_n;
    return result;
}
mat4 m4_perspective(
    f32 fov_radians, f32 aspect_ratio,
    f32 near_clip, f32 far_clip
) {
    mat4 result = MAT4_ZERO;
    
    f32 half_fov_tan = tan32( fov_radians / 2.0f );

    result.c[ 0] = 1.0f / ( aspect_ratio * half_fov_tan );
    result.c[ 5] = 1.0f / half_fov_tan;
    result.c[10] = -( ( far_clip + near_clip ) / ( far_clip - near_clip ) );
    result.c[11] = -1.0f;
    result.c[14] =
        -( ( 2.0f * far_clip * near_clip ) / ( far_clip - near_clip ) );

    return result;
}
mat4 m4_translation( f32 x, f32 y, f32 z ) {
    return (mat4){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
           x,    y,    z, 1.0f,
    };
}
mat4 m4_rotation_pitch( f32 pitch_radians ) {
    f32 sin, cos;
    sincos32( pitch_radians, &sin, &cos );

    return (mat4){
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  cos,  sin, 0.0f,
        0.0f, -sin,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}
mat4 m4_rotation_yaw( f32 yaw_radians ) {
    f32 sin, cos;
    sincos32( yaw_radians, &sin, &cos );

    return (mat4){
         cos, 0.0f, -sin, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
         sin, 0.0f,  cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}
mat4 m4_rotation_roll( f32 roll_radians ) {
    f32 sin, cos;
    sincos32( roll_radians, &sin, &cos );

    return (mat4){
         cos,  sin, 0.0f, 0.0f,
        -sin,  cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}
mat4 m4_rotation_euler(
    f32 pitch_radians, f32 yaw_radians, f32 roll_radians
) {
    mat4 pitch = m4_rotation_pitch( pitch_radians );
    mat4 yaw   = m4_rotation_yaw( yaw_radians );
    mat4 roll  = m4_rotation_roll( roll_radians );
    mat4 yaw_mul_roll = m4_mul_m4( &yaw, &roll );
    return m4_mul_m4( &pitch, &yaw_mul_roll );
}
mat4 m4_rotation_q( quat rotation ) {
    // TODO(alicia): SIMD
    mat4 result = MAT4_IDENTITY;

    f32 _2x2 = 2.0f * (rotation.x * rotation.x);
    f32 _2y2 = 2.0f * (rotation.y * rotation.y);
    f32 _2z2 = 2.0f * (rotation.z * rotation.z);

    f32 _2xy = 2.0f * (rotation.x * rotation.y);
    f32 _2xz = 2.0f * (rotation.x * rotation.z);
    f32 _2yz = 2.0f * (rotation.y * rotation.z);
    f32 _2wx = 2.0f * (rotation.w * rotation.x);
    f32 _2wy = 2.0f * (rotation.w * rotation.y);
    f32 _2wz = 2.0f * (rotation.w * rotation.z);

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
mat4 m4_scale( f32 width, f32 height, f32 length ) {
    return (mat4){
        width,   0.0f,   0.0f, 0.0f,
         0.0f, height,   0.0f, 0.0f,
         0.0f,   0.0f, length, 0.0f,
         0.0f,   0.0f,   0.0f, 1.0f
    };
}
mat4 m4_transform( vec3 translation, quat rotation, vec3 scale ) {
    mat4 t = m4_translation_v3( translation );
    mat4 r = m4_rotation_q( rotation );
    mat4 s = m4_scale_v3( scale );
    mat4 r_mul_s = m4_mul_m4( &r, &s );
    return m4_mul_m4( &t, &r_mul_s );
}
mat4 m4_transform_euler( vec3 translation, vec3 rotation, vec3 scale ) {
    mat4 t = m4_translation_v3( translation );
    mat4 r = m4_rotation_euler_v3( rotation );
    mat4 s = m4_scale_v3( scale );
    mat4 r_mul_s = m4_mul_m4( &r, &s );
    return m4_mul_m4( &t, &r_mul_s );
}
mat4 m4_transform_2d( vec2 translation, f32 rotation_radians, vec2 scale ) {
    mat4 t = m4_translation_2d_v2( translation );
    mat4 r = m4_rotation_roll( rotation_radians );
    mat4 s = m4_scale_2d_v2( scale );
    mat4 r_mul_s = m4_mul_m4( &r, &s );
    return m4_mul_m4( &t, &r_mul_s );
}
mat4 m4_add( const mat4* lhs, const mat4* rhs ) {
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

    lane4f_store( lane4f_add( lhs0, rhs0 ), result.col0.c );
    lane4f_store( lane4f_add( lhs1, rhs1 ), result.col1.c );
    lane4f_store( lane4f_add( lhs2, rhs2 ), result.col2.c );
    lane4f_store( lane4f_add( lhs3, rhs3 ), result.col3.c );

    return result;
}
mat4 m4_sub( const mat4* lhs, const mat4* rhs ) {
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

    lane4f_store( lane4f_sub( lhs0, rhs0 ), result.col0.c );
    lane4f_store( lane4f_sub( lhs1, rhs1 ), result.col1.c );
    lane4f_store( lane4f_sub( lhs2, rhs2 ), result.col2.c );
    lane4f_store( lane4f_sub( lhs3, rhs3 ), result.col3.c );

    return result;
}
mat4 m4_mul( const mat4* lhs, f32 rhs ) {
    Lane4f lhs0, lhs1, lhs2, lhs3;
    Lane4f rhs_;

    lhs0 = lane4f_load( lhs->col0.c );
    lhs1 = lane4f_load( lhs->col1.c );
    lhs2 = lane4f_load( lhs->col2.c );
    lhs3 = lane4f_load( lhs->col3.c );

    rhs_ = lane4f_scalar( rhs );

    mat4 result;

    lane4f_store( lane4f_mul( lhs0, rhs_ ), result.col0.c );
    lane4f_store( lane4f_mul( lhs1, rhs_ ), result.col1.c );
    lane4f_store( lane4f_mul( lhs2, rhs_ ), result.col2.c );
    lane4f_store( lane4f_mul( lhs3, rhs_ ), result.col3.c );

    return result;

}
mat4 m4_mul_m4( const mat4* lhs, const mat4* rhs ) {
    mat4 result = {};

    Lane4f a, b, c;

    u32 col = 0;
    u32 row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    col = 1;
    row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    col = 2;
    row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    col = 3;
    row = 0;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 1;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 2;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    row = 3;
    a = lane4f_load( &lhs->c[row * MAT4_COLUMN_COUNT] );
    b = lane4f_mul( a, lane4f_scalar( rhs->c[(col * MAT4_COLUMN_COUNT) + row] ) );
    c = lane4f_load( &result.c[ col * MAT4_COLUMN_COUNT ] );
    lane4f_store( lane4f_add( b, c ), &result.c[col * MAT4_COLUMN_COUNT] );

    return result;
}
vec4 m4_mul_v4( const mat4* lhs, vec4 rhs ) {
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
vec3 m4_mul_v3( const mat4* lhs, vec3 rhs ) {
    vec4 rhs_v4;
    rhs_v4.xyz = rhs;
    rhs_v4.w   = 1.0f;
    vec3 result = m4_mul_v4( lhs, rhs_v4 ).xyz;
    return result;
}
mat4 m4_div( const mat4* lhs, f32 rhs ) {
    Lane4f lhs0, lhs1, lhs2, lhs3;
    Lane4f rhs_;

    lhs0 = lane4f_load( lhs->col0.c );
    lhs1 = lane4f_load( lhs->col1.c );
    lhs2 = lane4f_load( lhs->col2.c );
    lhs3 = lane4f_load( lhs->col3.c );

    rhs_ = lane4f_scalar( rhs );

    mat4 result;

    lane4f_store( lane4f_div( lhs0, rhs_ ), result.col0.c );
    lane4f_store( lane4f_div( lhs1, rhs_ ), result.col1.c );
    lane4f_store( lane4f_div( lhs2, rhs_ ), result.col2.c );
    lane4f_store( lane4f_div( lhs3, rhs_ ), result.col3.c );

    return result;
}
mat4 m4_transpose( const mat4* m ) {
    return (mat4){
        m->c[0], m->c[4], m->c[ 8], m->c[12],
        m->c[1], m->c[5], m->c[ 9], m->c[13],
        m->c[2], m->c[6], m->c[10], m->c[14],
        m->c[3], m->c[7], m->c[11], m->c[15]
    };
}
mat3 m4_submatrix( const mat4* m, usize row, usize column ) {
    mat3 result;
    usize i = 0;
    for( usize c = 0; c < 4; ++c ) {
        if( c != column ) {
            for( usize r = 0; r < 4; ++r ) {
                if( r != row ) {
                    result.c[i++] = m->m[c][r];
                }
            }
        }
    }
    return result;
}
f32 m4_minor( const mat4* m, usize row, usize column ) {
    mat3 submatrix = m4_submatrix( m, row, column );
    return m3_determinant( &submatrix );
}
f32 m4_cofactor( const mat4* m, usize row, usize column ) {
    f32 minor = m4_minor( m, row, column );
    i32 exp   = ( row + 1 ) + ( column + 1 );
    return minor * powi32( -1.0f, exp );
}
mat4 m4_cofactor_matrix( const mat4* m ) {
    mat4 result = (mat4){
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
    return m4_transpose( &result );
}
mat4 m4_adjoint( const mat4* m ) {
    mat4 cofactor = m4_cofactor_matrix( m );
    return m4_transpose( &cofactor );
}
f32 m4_determinant( const mat4* m ) {
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
b32 m4_inverse( const mat4* m, mat4* out_inverse ) {
    f32 determinant = m4_determinant( m );
    if( determinant == 0.0f ) {
        return false;
    } else {
        mat4 adjoint = m4_adjoint( m );
        *out_inverse = m4_div( &adjoint, determinant );
        return true;
    }
}
mat4 m4_inverse_unchecked( const mat4* m ) {
    mat4 adjoint     = m4_adjoint( m );
    f32  determinant = m4_determinant( m );
    return m4_div( &adjoint, determinant );
}
b32 m4_normal_matrix( const mat4* m, mat3* out_normal_matrix ) {
    mat4 inv;

    if( m4_inverse( m, &inv ) ) {
        mat4 inv_transpose = m4_transpose( &inv );
        *out_normal_matrix = m3_m4( &inv_transpose );
        return true;
    } else {
        *out_normal_matrix = MAT3_IDENTITY;
        return false;
    }
}
mat3 m4_normal_matrix_unchecked( const mat4* m ) {
    mat4 inv = m4_inverse_unchecked( m );
    mat4 inv_transpose = m4_transpose( &inv );
    return m3_m4( &inv_transpose );
}
LD_API vec3 m4_transform_position( const mat4* m ) {
    return v3( m->m30, m->m31, m->m32 );
}

euler_angles euler_q( quat q ) {
    return (euler_angles){
        atan2_32(
            2.0f * (( q.w * q.x ) + ( q.y * q.z )),
            1.0f - 2.0f * ( ( q.x * q.x ) + ( q.y * q.y ) )
        ),
        asin32_no_nan( 2.0f * (( q.w * q.y ) - ( q.z * q.x )) ),
        atan2_32(
            2.0f * (( q.w * q.z ) + ( q.x * q.y )),
            1.0f - 2.0f * ( ( q.y * q.y ) + ( q.z * q.z ) )
        ),
    };
}

mat2 m2_m3( const mat3* m ) {
    return (mat2){
        m->c[0], m->c[1],
        m->c[3], m->c[4]
    };
}
mat2 m2_m4( const mat4* m ) {
    return (mat2){
        m->c[0], m->c[1],
        m->c[4], m->c[5]
    };
}
mat3 m3_m4( const mat4* m ) {
    return (mat3){
        m->c[0], m->c[1], m->c[ 2],
        m->c[4], m->c[5], m->c[ 6],
        m->c[8], m->c[9], m->c[10]
    };
}
mat4 m4_m3( const mat3* m ) {
    return (mat4){
        m->c[0], m->c[1], m->c[2], 0.0f,
        m->c[3], m->c[4], m->c[5], 0.0f,
        m->c[6], m->c[7], m->c[8], 0.0f,
           0.0f,    0.0f,    0.0f, 0.0f,
    };
}
quat q_euler_v3( euler_angles euler ) {
    return q_euler( euler.pitch, euler.yaw, euler.roll );
}
mat4 m4_view_2d( vec2 position, vec2 up ) {
    return m4_view(
        v3( position.x, position.y, 1.0f ),
        v3( position.x, position.y, 0.0f ),
        v3( up.x, up.y, 0.0f ) );
}
mat4 m4_translation_2d( f32 x, f32 y ) {
    return m4_translation( x, y, 0.0f );
}
mat4 m4_translation_v3( vec3 translation ) {
    return m4_translation( translation.x, translation.y, translation.z );
}
mat4 m4_translation_2d_v2( vec2 translation ) {
    return m4_translation_2d( translation.x, translation.y );
}
mat4 m4_rotation_euler_v3( vec3 rotation ) {
    return m4_rotation_euler( rotation.pitch, rotation.yaw, rotation.roll );
}
mat4 m4_scale_2d( f32 width, f32 height ) {
    return m4_scale( width, height, 1.0f );
}
mat4 m4_scale_v3( vec3 scale ) {
    return m4_scale( scale.width, scale.height, scale.length );
}
mat4 m4_scale_2d_v2( vec2 scale ) {
    return m4_scale_2d( scale.width, scale.height );
}

LD_API Transform transform_create( vec3 position, quat rotation, vec3 scale ) {
    Transform result = {};

    result.position = position;
    result.rotation = rotation;
    result.scale    = scale;

    result.local_matrix_dirty = false;
    result.world_matrix_dirty = true;
    result.camera_dirty       = true;

    result.local_matrix = m4_transform( position, rotation, scale );
    result.world_matrix = MAT4_IDENTITY;

    result.parent       = NULL;

    return result;
}
LD_API mat4 transform_local_matrix( Transform* t ) {
    if( t->local_matrix_dirty ) {
        t->local_matrix = m4_transform( t->position, t->rotation, t->scale );
        t->local_matrix_dirty = false;
    }
    return t->local_matrix;
}
LD_API mat4 transform_world_matrix( Transform* t ) {
    if( t->world_matrix_dirty ) {
        mat4 local_matrix = transform_local_matrix( t );
        if( t->parent ) {
            mat4 parent_matrix = transform_world_matrix( t->parent );
            t->world_matrix = m4_mul_m4( &parent_matrix, &local_matrix );
        } else {
            t->world_matrix = local_matrix;
        }

        t->world_matrix_dirty = false;
    }
    return t->world_matrix;
}
LD_API vec3 transform_local_position( Transform* t ) {
    return t->position;
}
LD_API vec3 transform_world_position( Transform* t ) {
    mat4 world_matrix = transform_world_matrix( t );
    return m4_transform_position( &world_matrix );
}
LD_API void transform_set_position( Transform* t, vec3 position ) {
    t->position           = position;
    t->camera_dirty       = true;
    t->local_matrix_dirty = true;
    t->world_matrix_dirty = true;
}
LD_API void transform_translate( Transform* t, vec3 translation ) {
    transform_set_position( t, v3_add( t->position, translation ) );
}
LD_API quat transform_local_rotation( Transform* t ) {
    return t->rotation;
}
LD_API quat transform_world_rotation( Transform* t ) {
    quat local_ = transform_local_rotation( t );
    if( t->parent ) {
        quat parent = transform_world_rotation( t->parent );
        return q_mul_q( local_, parent );
    }
    return local_;
}
LD_API void transform_set_rotation( Transform* t, quat rotation ) {
    t->rotation           = rotation;
    t->camera_dirty       = true;
    t->local_matrix_dirty = true;
    t->world_matrix_dirty = true;
}
LD_API void transform_rotate( Transform* t, quat rotation ) {
    transform_set_rotation( t, q_mul_q( rotation, t->rotation ) );
}
LD_API vec3 transform_local_scale( Transform* t ) {
    return t->scale;
}
LD_API vec3 transform_world_scale( Transform* t ) {
    vec3 local_ = transform_local_scale( t );
    if( t->parent ) {
        vec3 parent = transform_world_scale( t );
        return v3_hadamard( local_, parent );
    }
    return local_;
}
LD_API void transform_set_scale( Transform* t, vec3 scale ) {
    t->scale              = scale;
    t->local_matrix_dirty = true;
    t->world_matrix_dirty = true;
}
LD_API void transform_scale( Transform* t, vec3 scale ) {
    transform_set_scale( t, v3_hadamard( t->scale, scale ) );
}

LD_API vec3 transform_local_forward( Transform* t ) {
    return q_mul_v3( t->rotation, VEC3_FORWARD );
}
LD_API vec3 transform_local_right( Transform* t ) {
    return q_mul_v3( t->rotation, VEC3_RIGHT );
}
LD_API vec3 transform_local_up( Transform* t ) {
    return q_mul_v3( t->rotation, VEC3_UP );
}

LD_API vec3 transform_world_forward( Transform* t ) {
    return q_mul_v3( transform_world_rotation( t ), VEC3_FORWARD );
}
LD_API vec3 transform_world_right( Transform* t ) {
    return q_mul_v3( transform_world_rotation( t ), VEC3_RIGHT );
}
LD_API vec3 transform_world_up( Transform* t ) {
    return q_mul_v3( transform_world_rotation( t ), VEC3_UP );
}

