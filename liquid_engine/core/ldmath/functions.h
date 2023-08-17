#if !defined(CORE_MATH_FUNCTIONS_HPP)
#define CORE_MATH_FUNCTIONS_HPP
/**
 * Description:  Math functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 08, 2023
*/
#include "defines.h"
#if LD_SIMD_WIDTH != 1
    #include "simd.h"
#endif

/// 32-bit unsigned integer rotate left
#define rotl32( bitpattern, rotate )\
    (bitpattern << rotate) | (bitpattern >> (-rotate & 31ul))
/// 32-bit unsigned integer rotate right
#define rotr32( bitpattern, rotate )\
    (bitpattern >> rotate) | (bitpattern << (32ul - rotate))

/// truncate float to signed integer
#define trunc_i64( f ) ((i64)(f))
/// truncate float to signed integer
#define trunc_i32( f ) ((i32)(f))
/// truncate float to signed integer
#define trunc_i16( f ) ((i16)(f))
/// truncate float to signed integer
#define trunc_i8( f ) ((i8)(f))

/// floor float to signed integer
#define floor_i64(f) (((f) > 0.0) ? trunc_i64(f) : trunc_i64( (f) - 0.9999999 ))
/// floor float to signed integer
#define floor_i32(f) (((f) > 0.0) ? trunc_i32(f) : trunc_i32( (f) - 0.9999999 ))
/// floor float to signed integer
#define floor_i16(f) (((f) > 0.0) ? trunc_i16(f) : trunc_i16( (f) - 0.9999999 ))
/// floor float to signed integer
#define floor_i8(f) (((f) > 0.0) ? trunc_i8(f) : trunc_i8( (f) - 0.9999999 ))

/// ceil float to signed integer
#define ceil_i64(f) (((f) > 0.0) ? trunc_i64( (f) + 0.9999999 ) : trunc_i64( (f) ))
/// ceil float to signed integer
#define ceil_i32(f) (((f) > 0.0) ? trunc_i32( (f) + 0.9999999 ) : trunc_i32( (f) ))
/// ceil float to signed integer
#define ceil_i16(f) (((f) > 0.0) ? trunc_i16( (f) + 0.9999999 ) : trunc_i16( (f) ))
/// ceil float to signed integer
#define ceil_i8(f) (((f) > 0.0) ? trunc_i8( (f) + 0.9999999 ) : trunc_i8( (f) ))

/// round float to signed integer
#define round_i64(f) (((f) > 0.0) ? trunc_i64( (f) + 0.5 ) : trunc_i64( (f) - 0.5 ))
/// round float to signed integer
#define round_i32(f) (((f) > 0.0) ? trunc_i32( (f) + 0.5 ) : trunc_i32( (f) - 0.5 ))
/// round float to signed integer
#define round_i16(f) (((f) > 0.0) ? trunc_i16( (f) + 0.5 ) : trunc_i16( (f) - 0.5 ))
/// round float to signed integer
#define round_i8(f) (((f) > 0.0) ? trunc_i8( (f) + 0.5 ) : trunc_i8( (f) - 0.5 ))

/// sign of value
#define signof( x ) (( (x) > (__typeof((x)))0 ) - ( (x) < (__typeof((x)))0 ))

/// absolute value
#define absof(x) ((x) * signof((x)))

/// smallest of two values
#define min( a, b ) ( (a) < (b) ? (a) : (b) )

/// largest of two values
#define max( a, b ) ( (a) < (b) ? (b) : (a) )

/// clamp a number between min and max, inclusive-inclusive
#define clamp( value, min, max ) \
    (\
        ((value) < (min) ? (min) : (value)) > (max) ?\
        (max) :\
        ((value) < (min) ? (min) : (value))\
    )

/// clamp a float between 0-1
#define clamp01( value ) clamp( (value), 0.0, 1.0 )

/// normalize integer -1 to 1 range
#define normalize_range_i8_f32( x ) \
    ((f32)(x) / ((x) > 0 ? (f32)I8_MAX : -((f32)I8_MIN)))

/// normalize integer -1 to 1 range
#define normalize_range_i16_f32( x )\
    ((f32)(x) / ((x) > 0 ? (f32)I16_MAX : -((f32)I16_MIN)))

/// normalize integer -1 to 1 range
#define normalize_range_i32_f32( x )\
    ((f32)(x) / ((x) > 0 ? (f32)I32_MAX : -((f32)I32_MIN)))

/// normalize integer -1 to 1 range
#define normalize_range_i64_f32( x )\
    ((f32)(x) / ((x) > 0 ? (f32)I64_MAX : -((f32)I64_MIN)))

/// normalize integer 0 to 1 range
#define normalize_range_u8_f32( x ) \
    ((f32)(x) / (f32)(U8_MAX))

/// normalize integer 0 to 1 range
#define normalize_range_u16_f32( x )\
    ((f32)(x) / (f32)(U16_MAX))

/// normalize integer 0 to 1 range
#define normalize_range_u32_f32( x )\
    ((f32)(x) / (f32)(U32_MAX))

/// normalize integer 0 to 1 range
#define normalize_range_u64_f32( x )\
    ((f32)(x) / (f32)(U64_MAX))

/// normalize integer -1 to 1 range
#define normalize_range_i8_f64( x ) \
    ((f64)(x) / ((x) > 0 ? (f64)I8_MAX : -((f64)I8_MIN)))

/// normalize integer -1 to 1 range
#define normalize_range_i16_f64( x )\
    ((f64)(x) / ((x) > 0 ? (f64)I16_MAX : -((f64)I16_MIN)))

/// normalize integer -1 to 1 range
#define normalize_range_i32_f64( x )\
    ((f64)(x) / ((x) > 0 ? (f64)I32_MAX : -((f64)I32_MIN)))

/// normalize integer -1 to 1 range
#define normalize_range_i64_f64( x )\
    ((f64)(x) / ((x) > 0 ? (f64)I64_MAX : -((f64)I64_MIN)))

/// normalize integer 0 to 1 range
#define normalize_range_u8_f64( x ) \
    ((f64)(x) / (f64)(U8_MAX))

/// normalize integer 0 to 1 range
#define normalize_range_u16_f64( x )\
    ((f64)(x) / (f64)(U16_MAX))

/// normalize integer 0 to 1 range
#define normalize_range_u32_f64( x )\
    ((f64)(x) / (f64)(U32_MAX))

/// normalize integer 0 to 1 range
#define normalize_range_u64_f64( x )\
    ((f64)(x) / (f64)(U64_MAX))

/// normalize float to int min-max.
/// float must be in -1.0-1.0 range
#define normalize_range_f32_i8( f )\
    ((i8)( absof( (f) ) * (f32)I8_MAX ) * (i8)(signof((f))))
/// normalize float to int min-max.
/// float must be in -1.0-1.0 range
#define normalize_range_f32_i16( f )\
    ((i8)( absof( (f) ) * (f32)I16_MAX ) * (i8)(signof((f))))
/// normalize float to int min-max.
/// float must be in -1.0-1.0 range
#define normalize_range_f32_i32( f )\
    ((i8)( absof( (f) ) * (f32)I32_MAX ) * (i8)(signof((f))))
/// normalize float to int min-max.
/// float must be in -1.0-1.0 range
#define normalize_range_f32_i64( f )\
    ((i8)( absof( (f) ) * (f32)I64_MAX ) * (i8)(signof((f))))

/// square root
headerfn f32 sqrt( f32 x ) {
#if LD_SIMD_WIDTH == 1
#if defined(LD_ARCH_X86)
    f32 result;
    __asm__ inline (
        "fld dword ptr [%1]\n\t"
        "fsqrt\n\t"
        "fstp dword ptr %0"
        : "=m" (result)
        : "r" (&x)
        : "cc"
    );
    return result;   
#else // not x86
    #error "Current Architecture does not have sqrt defined!"
#endif 
#else /// no simd
    return lane1f_sqrt( x );
#endif // simd present
}
/// square root
headerfn f64 sqrt64( f64 x ) {
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
    #error "Current Architecture does not have sqrt defined!"
#endif
}
/// inverse sqrt
headerfn f32 inv_sqrt( f32 x ) {
#if LD_SIMD_WIDTH != 1
    return lane1f_inv_sqrt( x );
#else
    return 1.0f / sqrt( x );
#endif
}
/// inverse sqrt
headerfn f64 inv_sqrt64( f64 x ) {
    return 1.0 / sqrt64( x );
}

/// raise to the power, integer exponent
headerfn f32 powi( f32 base, i32 exponent ) {
    u32 exponent_abs = absof( exponent );
    f32 result = base;
    for( u32 i = 1; i < exponent_abs; ++i ) {
        result *= base;
    }
    if( exponent < 0 ) {
        return 1.0f / result;
    } else {
        return result;
    }
}
/// raise to the power, integer exponent
headerfn f64 powi64( f64 base, i32 exponent ) {
    u32 exponent_abs = absof( exponent );
    f64 result = base;
    for( u32 i = 1; i < exponent_abs; ++i ) {
        result *= base;
    }
    if( exponent < 0 ) {
        return 1.0 / result;
    } else {
        return result;
    }
}

/// raise to the power, float exponent
headerfn f32 pow( f32 base, f32 exponent ) {
    // TODO(alicia): REPLACE
    return powi( base, (i32)exponent );
    // return impl::_powf_( base, exponent );
}
/// raise to the power, float exponent
headerfn f64 pow64( f64 base, f64 exponent ) {
    // TODO(alicia): REPLACE
    return powi64( base, (i32)exponent );
    // return impl::_pow_( base, exponent );
}

/// float modulus
headerfn f32 mod( f32 lhs, f32 rhs ) {
    if(0.0f == rhs) {
        return lhs;
    }

    f32 m = lhs - ( rhs * (f32)floor_i32( lhs / rhs ) );

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
            if( (rhs + m) == rhs ) {
                return 0.0f;
            } else {
                return rhs + m;
            }
        }
    }

    return m;
}
/// float modulus
headerfn f64 mod64( f64 lhs, f64 rhs ) {
    if(0.0 == rhs) {
        return lhs;
    }

    f64 m = lhs - ( rhs * (f64)floor_i64( lhs / rhs ) );

    if( rhs > 0.0 ) {
        if( m >= rhs ) {
            return 0.0;
        }

        if( m < 0.0 ) {
            if( rhs + m == rhs ) {
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
            if( rhs + m == rhs ) {
                return 0.0;
            } else {
                return rhs + m;
            }
        }
    }

    return m;
}

/// check if single-precision is not a number
headerfn b32 is_nan( f32 x ) {
    u32 bitpattern = *(u32*)&x;

    u32 exp = bitpattern & F32_EXPONENT_MASK;
    u32 man = bitpattern & F32_MANTISSA_MASK;

    return exp == F32_EXPONENT_MASK && man != 0;
}
/// check if single-precision float is zero
headerfn b32 is_zero( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return (bitpattern & 0x7FFFFFFF) == 0 || (bitpattern == 0x80000000);
}
/// check if double-precision float is not a number
headerfn b32 is_nan64( f64 x ) {
    u64 bitpattern = *(u64*)&x;

    u64 exp = bitpattern & F64_EXPONENT_MASK;
    u64 man = bitpattern & F64_MANTISSA_MASK;

    return exp == F64_EXPONENT_MASK && man != 0;
}
/// check if double-precision float is zero
headerfn b32 is_zero64( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return (bitpattern & 0x7FFFFFFFFFFFFFFF) == 0 || (bitpattern == 0x8000000000000000);
}

/// Wrap a degree value into 0.0 -> 360.0 range
headerfn f32 wrap_degrees( f32 degrees ) {
    f32 result = mod( degrees, 360.0f );
    if( result < 0.0f ) {
        result += 360.0f;
    }
    return result;
}
/// Wrap a degree value into 0.0 -> 360.0 range
headerfn f64 wrap_degrees64( f64 degrees ) {
    f64 result = mod64( degrees, 360.0 );
    if( result < 0.0 ) {
        result += 360.0;
    }
    return result;
}
/// Wrap a radians value into -Pi -> Pi range
headerfn f32 wrap_pi( f32 radians ) {
    return mod( radians + F32_PI, F32_TAU ) - F32_PI;
}
/// Wrap a radians value into -Pi -> Pi range
headerfn f64 wrap_pi64( f64 radians ) {
    return mod64( radians + F64_PI, F64_TAU ) - F64_PI;
}

/// sine function
headerfn f32 sin( f32 x ) {
    x = wrap_pi(x);
    return x -
        ( powi( x, 3 ) / F32_THREE_FACTORIAL ) +
        ( powi( x, 5 ) / F32_FIVE_FACTORIAL  ) -
        ( powi( x, 7 ) / F32_SEVEN_FACTORIAL ) +
        ( powi( x, 9 ) / F32_NINE_FACTORIAL  );
        // - ( powi( x, 11 ) / F32_ELEVEN_FACTORIAL )
}
/// sine function
headerfn f64 sin64( f64 x ) {
    x = wrap_pi64(x);
    return x -
        ( powi64( x, 3 ) / F64_THREE_FACTORIAL ) +
        ( powi64( x, 5 ) / F64_FIVE_FACTORIAL )  -
        ( powi64( x, 7 ) / F64_SEVEN_FACTORIAL ) +
        ( powi64( x, 9 ) / F64_NINE_FACTORIAL );
        // - ( powi( x, 11 ) / F64_ELEVEN_FACTORIAL )
}
/// arc-sine function
headerfn f32 asin( f32 x ) {
    // don't ask me how i figured this shit out
    // i don't even know
    f32 sign_of_x = signof( x );
    f32 x_abs = x * sign_of_x;
    f32 x_sqr = x_abs * x_abs;

    const f32 magic_0 =  1.5707288f;
    const f32 magic_1 = -0.2121144f;
    const f32 magic_2 =  0.0742610f;
    const f32 magic_3 = -0.0187293f;

    f32 result = F32_HALF_PI - sqrt( 1.0f - x_abs ) * (
        magic_0 +
        ( magic_1 * x_abs ) +
        ( magic_2 * x_sqr ) +
        ( magic_3 * ( x_sqr * x_abs ) )
    );

    return result * sign_of_x;
}
/// arc-sine function
headerfn f64 asin64( f64 x ) {
    f64 sign_of_x = signof( x );
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

/// arc-sine function, does not return NAN
headerfn f32 asin_real( f32 x ) {
    return absof(x) >= 1.0f ? F32_HALF_PI * signof(x) : asin(x);
}
/// arc-sine function, does not return NAN
headerfn f64 asin_real64( f64 x ) {
    return absof(x) >= 1.0 ? F64_HALF_PI * signof(x) : asin64(x);
}

/// cosine function
headerfn f32 cos( f32 x ) {
    x = wrap_pi(x);
    return 1.0f -
        ( powi( x, 2 ) / F32_TWO_FACTORIAL ) +
        ( powi( x, 4 ) / F32_FOUR_FACTORIAL )  -
        ( powi( x, 6 ) / F32_SIX_FACTORIAL ) +
        ( powi( x, 8 ) / F32_EIGHT_FACTORIAL );
        // - ( pow( x, 10 ) / F32_TEN_FACTORIAL )
}
/// cosine function
headerfn f64 cos64( f64 x ) {
    x = wrap_pi64(x);
    return 1.0 -
        ( powi64( x, 2 ) / F64_TWO_FACTORIAL ) +
        ( powi64( x, 4 ) / F64_FOUR_FACTORIAL )  -
        ( powi64( x, 6 ) / F64_SIX_FACTORIAL ) +
        ( powi64( x, 8 ) / F64_EIGHT_FACTORIAL );
        // - ( powi( x, 10) / F64_TEN_FACTORIAL )
}
/// arc-cosine function
headerfn f32 acos( f32 x ) {
    return -asin(x) + F32_HALF_PI;
}
/// arc-cosine function
headerfn f64 acos64( f64 x ) {
    return -asin64(x) + F64_HALF_PI;
}

// sin-cos function
headerfn tuplef32 sincos( f32 x ) {
    tuplef32 result = { sin(x), cos(x) };
    return result;
}
// sin-cos function
headerfn tuplef64 sincos64( f64 x ) {
    tuplef64 result = { sin64(x), cos64(x) };
    return result;
}

/// tangent function
headerfn f32 tan( f32 x ) {
    tuplef32 sc = sincos( x );
    return is_zero(sc.f1) ? F32_NAN : sc.f0 / sc.f1;
}
/// tangent function
headerfn f64 tan64( f64 x ) {
    tuplef64 sc = sincos64( x );
    return is_zero64(sc.f1) ? F64_NAN : sc.f0 / sc.f1;
}
/// arc-tangent function
headerfn f32 atan( f32 x ) {
    return x -
        ( powi( x, 3 )  / 3.0f ) +
        ( powi( x, 5 )  / 5.0f ) -
        ( powi( x, 7 )  / 7.0f ) +
        ( powi( x, 9 )  / 9.0f ) -
        ( powi( x, 11 ) / 11.0f ) +
        ( powi( x, 13 ) / 13.0f );
}
/// arc-tangent function
headerfn f64 atan64( f64 x ) {
    return x -
        ( powi64( x, 3 )  / 3.0 ) +
        ( powi64( x, 5 )  / 5.0 ) -
        ( powi64( x, 7 )  / 7.0 ) +
        ( powi64( x, 9 )  / 9.0 ) -
        ( powi64( x, 11 ) / 11.0 ) +
        ( powi64( x, 13 ) / 13.0 );
}

/// two argument arc-tangent function
headerfn f32 atan2( f32 y, f32 x ) {
    if( y == 0.0f ) {
        if( x < 0.0f ) {
            return F32_PI;
        } else if( x == 0.0f ) {
            return F32_NAN;
        }
    }

    f32 x_sqr = x * x;
    f32 y_sqr = y * y;
    return 2.0f * atan( y / ( sqrt( x_sqr + y_sqr ) + x ) );
}
/// two argument arc-tangent function
headerfn f64 atan264( f64 y, f64 x ) {
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

/// convert degrees to radians
headerfn f32 to_rad( f32 theta ) {
    return theta * ( F32_PI / 180.0f );
}
/// convert degrees to radians
headerfn f64 to_rad64( f64 theta ) {
    return theta * ( F64_PI / 180.0 );
}
/// convert radians to degrees 
headerfn f32 to_deg( f32 theta ) {
    return theta * ( 180.0f / F32_PI );
}
/// convert radians to degrees 
headerfn f64 to_deg64( f64 theta ) {
    return theta * ( 180.0 / F64_PI );
}

/// natural logarithm
headerfn f32 logarithm( f32 x ) {
    // TODO(alicia): replace with an actual function!
    return F32_NAN * x;
    // return impl::_logf_( x );
}
/// natural logarithm
headerfn f64 logarithm64( f64 x ) {
    // TODO(alicia): replace with an actual function!
    return F64_NAN * x;
    // return impl::_log_( x );
}

/// linear interpolation
headerfn f32 lerp( f32 a, f32 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation
headerfn f64 lerp64( f64 a, f64 b, f64 t ) {
    return ( 1.0 - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
headerfn f32 lerp_clamped( f32 a, f32 b, f32 t ) {
    return lerp( a, b, clamp01(t) );
}
/// linear interpolation, t clamped to 0-1
headerfn f64 lerp_clamped64( f64 a, f64 b, f64 t ) {
    return lerp64( a, b, clamp01(t) );
}
/// inverse linear interpolation
headerfn f32 inverse_lerp( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}
/// inverse linear interpolation
headerfn f64 inverse_lerp64( f64 a, f64 b, f64 v ) {
    return ( v - a ) / ( b - a );
}
/// remap value from input min/max to output min/max
headerfn f32 remap( f32 imin, f32 imax, f32 omin, f32 omax, f32 v ) {
    const f32 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}
/// remap value from input min/max to output min/max
headerfn f64 remap64( f64 imin, f64 imax, f64 omin, f64 omax, f64 v ) {
    const f64 t = inverse_lerp( imin, imax, v );
    return lerp64( omin, omax, t );
}

/// smooth step interpolation
headerfn f32 smooth_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation
headerfn f64 smooth_step64( f64 a, f64 b, f64 t ) {
    return ( b - a ) * ( 3.0 - t * 2.0 ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
headerfn f32 smooth_step_clamped( f32 a, f32 b, f32 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smooth step interpolation, t clamped to 0-1
headerfn f64 smooth_step_clamped64( f64 a, f64 b, f64 t ) {
    return smooth_step64( a, b, clamp01(t) );
}
/// smoother step interpolation
headerfn f32 smoother_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation
headerfn f64 smoother_step64( f64 a, f64 b, f64 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0 - 15.0 ) + 10.0 ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
headerfn f32 smoother_step_clamped( f32 a, f32 b, f32 t ) {
    return smoother_step( a, b, clamp01(t) );
}
/// smoother step interpolation, t clamped to 0-1
headerfn f64 smoother_step_clamped64( f64 a, f64 b, f64 t ) {
    return smoother_step64( a, b, clamp01(t) );
}

#endif // header guard
