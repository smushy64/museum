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

/// truncate float to signed integer
FORCE_INLINE i64 trunc64( f32 x ) { return (i64)x; }
/// truncate float to signed integer
FORCE_INLINE i64 trunc64( f64 x ) { return (i64)x; }
/// truncate float to signed integer
FORCE_INLINE i32 trunc32( f32 x ) { return (i32)x; }
/// truncate float to signed integer
FORCE_INLINE i32 trunc32( f64 x ) { return (i32)x; }
/// truncate float to signed integer
FORCE_INLINE i16 trunc16( f32 x ) { return (i16)x; }
/// truncate float to signed integer
FORCE_INLINE i16 trunc16( f64 x ) { return (i16)x; }
/// truncate float to signed integer
FORCE_INLINE i8  trunc8( f32 x )  { return (i8)x;  }
/// truncate float to signed integer
FORCE_INLINE i8  trunc8( f64 x )  { return (i8)x;  }

/// floor float to signed integer
FORCE_INLINE i64 floor64( f64 x ) {
    return x > 0.0 ? trunc64(x) : trunc64(x - 0.99999999);
}
/// floor float to signed integer
FORCE_INLINE i64 floor64( f32 x ) {
    return x > 0.0f ? trunc64(x) : trunc64(x - 0.999999f);
}
/// floor float to signed integer
FORCE_INLINE i32 floor32( f64 x ) {
    return x > 0.0 ? trunc32(x) : trunc32(x - 0.99999999);
}
/// floor float to signed integer
FORCE_INLINE i32 floor32( f32 x ) {
    return x > 0.0f ? trunc32(x) : trunc32(x - 0.999999f);
}
/// floor float to signed integer
FORCE_INLINE i16 floor16( f64 x ) {
    return x > 0.0 ? trunc16(x) : trunc16(x - 0.99999999);
}
/// floor float to signed integer
FORCE_INLINE i16 floor16( f32 x ) {
    return x > 0.0f ? trunc16(x) : trunc16(x - 0.999999f);
}
/// floor float to signed integer
FORCE_INLINE i8 floor8( f64 x ) {
    return x > 0.0 ? trunc8(x) : trunc8(x - 0.99999999);
}
/// floor float to signed integer
FORCE_INLINE i8 floor8( f32 x ) {
    return x > 0.0f ? trunc8(x) : trunc8(x - 0.999999f);
}

/// ceil float to signed integer
FORCE_INLINE i64 ceil64( f64 x ) {
    return x > 0.0 ? trunc64( x + 0.99999999 ) : trunc64(x);
}
/// ceil float to signed integer
FORCE_INLINE i64 ceil64( f32 x ) {
    return x > 0.0f ? trunc64( x + 0.999999f ) : trunc64(x);
}
/// ceil float to signed integer
FORCE_INLINE i32 ceil32( f64 x ) {
    return x > 0.0 ? trunc32( x + 0.99999999 ) : trunc32(x);
}
/// ceil float to signed integer
FORCE_INLINE i32 ceil32( f32 x ) {
    return x > 0.0f ? trunc32( x + 0.999999f ) : trunc32(x);
}
/// ceil float to signed integer
FORCE_INLINE i16 ceil16( f64 x ) {
    return x > 0.0 ? trunc16( x + 0.99999999 ) : trunc16(x);
}
/// ceil float to signed integer
FORCE_INLINE i16 ceil16( f32 x ) {
    return x > 0.0f ? trunc16( x + 0.999999f ) : trunc16(x);
}
/// ceil float to signed integer
FORCE_INLINE i8 ceil8( f64 x ) {
    return x > 0.0 ? trunc8( x + 0.99999999 ) : trunc8(x);
}
/// ceil float to signed integer
FORCE_INLINE i8 ceil8( f32 x ) {
    return x > 0.0f ? trunc8( x + 0.999999f ) : trunc8(x);
}

/// round float to signed integer
FORCE_INLINE i64 round64( f64 x ) {
    return x > 0.0 ? trunc64( x + 0.5 ) : trunc64( x - 0.5 );
}
/// round float to signed integer
FORCE_INLINE i64 round64( f32 x ) {
    return x > 0.0f ? trunc64( x + 0.5f ) : trunc64( x - 0.5f );
}
/// round float to signed integer
FORCE_INLINE i32 round32( f64 x ) {
    return x > 0.0 ? trunc32( x + 0.5 ) : trunc32( x - 0.5 );
}
/// round float to signed integer
FORCE_INLINE i32 round32( f32 x ) {
    return x > 0.0f ? trunc32( x + 0.5f ) : trunc32( x - 0.5f );
}
/// round float to signed integer
FORCE_INLINE i16 round16( f64 x ) {
    return x > 0.0 ? trunc16( x + 0.5 ) : trunc16( x - 0.5 );
}
/// round float to signed integer
FORCE_INLINE i16 round16( f32 x ) {
    return x > 0.0f ? trunc16( x + 0.5f ) : trunc16( x - 0.5f );
}
/// round float to signed integer
FORCE_INLINE i8 round8( f64 x ) {
    return x > 0.0 ? trunc8( x + 0.5 ) : trunc8( x - 0.5 );
}
/// round float to signed integer
FORCE_INLINE i8 round8( f32 x ) {
    return x > 0.0f ? trunc8( x + 0.5f ) : trunc8( x - 0.5f );
}

/// get the sign of a signed integer
FORCE_INLINE i8 sign( i8 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
FORCE_INLINE i16 sign( i16 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
FORCE_INLINE i32 sign( i32 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a signed integer
FORCE_INLINE i64 sign( i64 x ) { return ( x > 0 ) - ( x < 0 ); }
/// get the sign of a single-precision float
FORCE_INLINE f32 sign( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return ((bitpattern >> 31) & 1) == 1 ? -1.0f : 1.0f;
}
/// get the sign of a double-precision float
FORCE_INLINE f64 sign( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return ((bitpattern >> 63) & 1) == 1 ? -1.0 : 1.0;
}

/// get the absolute value of a signed integer
FORCE_INLINE i8 absolute( i8 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
FORCE_INLINE i16 absolute( i16 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
FORCE_INLINE i32 absolute( i32 x ) { return x * sign( x ); }
/// get the absolute value of a signed integer
FORCE_INLINE i64 absolute( i64 x ) { return x * sign( x ); }
/// get the absolute value of a single-precision float
FORCE_INLINE f32 absolute( f32 x ) { return x * sign( x ); }
/// get the absolute value of a double-precision float
FORCE_INLINE f64 absolute( f64 x ) { return x * sign( x ); }

/// clamp a signed integer between min and max, inclusive-inclusive
FORCE_INLINE i8 clamp( i8 value, i8 min, i8 max ) {
    const i8 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
FORCE_INLINE i16 clamp( i16 value, i16 min, i16 max ) {
    const i16 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
FORCE_INLINE i32 clamp( i32 value, i32 min, i32 max ) {
    const i32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a signed integer between min and max, inclusive-inclusive
FORCE_INLINE i64 clamp( i64 value, i64 min, i64 max ) {
    const i64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
FORCE_INLINE u8 clamp( u8 value, u8 min, u8 max ) {
    const u8 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
FORCE_INLINE u16 clamp( u16 value, u16 min, u16 max ) {
    const u16 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
FORCE_INLINE u32 clamp( u32 value, u32 min, u32 max ) {
    const u32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp an unsigned integer between min and max, inclusive-inclusive
FORCE_INLINE u64 clamp( u64 value, u64 min, u64 max ) {
    const u64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a single-precision float between min and max, inclusive-inclusive
FORCE_INLINE f32 clamp( f32 value, f32 min, f32 max ) {
    const f32 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a double-precision float between min and max, inclusive-inclusive
FORCE_INLINE f64 clamp( f64 value, f64 min, f64 max ) {
    const f64 t = value < min ? min : value;
    return t > max ? max : t;
}
/// clamp a single-precision float between 0-1
FORCE_INLINE f32 clamp01( f32 value ) { return clamp(value,0.0f,1.0f); }
/// clamp a double-precision float between 0-1
FORCE_INLINE f64 clamp01( f64 value ) { return clamp(value,0.0,1.0); }

/// get the smallest of two values
FORCE_INLINE i8 min( i8 a, i8 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE i16 min( i16 a, i16 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE i32 min( i32 a, i32 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE i64 min( i64 a, i64 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE u8 min( u8 a, u8 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE u16 min( u16 a, u16 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE u32 min( u32 a, u32 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE u64 min( u64 a, u64 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE f32 min( f32 a, f32 b ) { return a < b ? a : b; }
/// get the smallest of two values
FORCE_INLINE f64 min( f64 a, f64 b ) { return a < b ? a : b; }

/// get the largest of two values
FORCE_INLINE i8 max( i8 a, i8 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE i16 max( i16 a, i16 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE i32 max( i32 a, i32 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE i64 max( i64 a, i64 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE u8 max( u8 a, u8 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE u16 max( u16 a, u16 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE u32 max( u32 a, u32 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE u64 max( u64 a, u64 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE f32 max( f32 a, f32 b ) { return a < b ? b : a; }
/// get the largest of two values
FORCE_INLINE f64 max( f64 a, f64 b ) { return a < b ? b : a; }

/// normalize integer -1 to 1 range
FORCE_INLINE f32 normalize_range( i8 x ) {
    return (f32)x / (x > 0 ? (f32)I8::MAX : -((f32)I8::MIN));
}
/// normalize integer -1 to 1 range
FORCE_INLINE f32 normalize_range( i16 x ) {
    return (f32)x / (x > 0 ? (f32)I16::MAX : -((f32)I16::MIN));
}
/// normalize integer -1 to 1 range
FORCE_INLINE f32 normalize_range( i32 x ) {
    return (f32)x / (x > 0 ? (f32)I32::MAX : -((f32)I32::MIN));
}
/// normalize integer -1 to 1 range
FORCE_INLINE f32 normalize_range( i64 x ) {
    return (f32)x / (x > 0 ? (f32)I64::MAX : -((f32)I64::MIN));
}
/// normalize integer 0 to 1 range
FORCE_INLINE f32 normalize_range( u8 x ) {
    return (f32)x / (f32)U8::MAX;
}
/// normalize integer 0 to 1 range
FORCE_INLINE f32 normalize_range( u16 x ) {
    return (f32)x / (f32)U16::MAX;
}
/// normalize integer 0 to 1 range
FORCE_INLINE f32 normalize_range( u32 x ) {
    return (f32)x / (f32)U32::MAX;
}
/// normalize integer 0 to 1 range
FORCE_INLINE f32 normalize_range( u64 x ) {
    return (f32)x / (f32)U64::MAX;
}
/// normalize integer -1 to 1 range
FORCE_INLINE f64 normalize_range64( i8 x ) {
    return (f64)x / (x > 0 ? (f64)I8::MAX : -((f64)I8::MIN));
}
/// normalize integer -1 to 1 range
FORCE_INLINE f64 normalize_range64( i16 x ) {
    return (f64)x / (x > 0 ? (f64)I16::MAX : -((f64)I16::MIN));
}
/// normalize integer -1 to 1 range
FORCE_INLINE f64 normalize_range64( i32 x ) {
    return (f64)x / (x > 0 ? (f64)I32::MAX : -((f64)I32::MIN));
}
/// normalize integer -1 to 1 range
FORCE_INLINE f64 normalize_range64( i64 x ) {
    return (f64)x / (x > 0 ? (f64)I64::MAX : -((f64)I64::MIN));
}
/// normalize integer 0 to 1 range
FORCE_INLINE f64 normalize_range64( u8 x ) {
    return (f64)x / (f64)U8::MAX;
}
/// normalize integer 0 to 1 range
FORCE_INLINE f64 normalize_range64( u16 x ) {
    return (f64)x / (f64)U16::MAX;
}
/// normalize integer 0 to 1 range
FORCE_INLINE f64 normalize_range64( u32 x ) {
    return (f64)x / (f64)U32::MAX;
}
/// normalize integer 0 to 1 range
FORCE_INLINE f64 normalize_range64( u64 x ) {
    return (f64)x / (f64)U64::MAX;
}

/// normalize float to i8 min-max.
/// float must be in -1.0-1.0 range
FORCE_INLINE i8 normalize_rangei8( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i8)( x_abs * (f32)I8::MAX ) * (i8)x_sign;
}
/// normalize float to i16 min-max.
/// float must be in -1.0-1.0 range
FORCE_INLINE i16 normalize_rangei16( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i16)( x_abs * (f32)I16::MAX ) * (i16)x_sign;
}
/// normalize float to i32 min-max.
/// float must be in -1.0-1.0 range
FORCE_INLINE i32 normalize_rangei32( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i32)( x_abs * (f32)I32::MAX ) * (i32)x_sign;
}
/// normalize float to i64 min-max.
/// float must be in -1.0-1.0 range
FORCE_INLINE i64 normalize_rangei64( f32 x ) {
    f32 x_sign = sign(x);
    f32 x_abs  = x * x_sign;
    return (i64)( x_abs * (f32)I64::MAX ) * (i64)x_sign;
}

/// square root
FORCE_INLINE f32 sqrt( f32 x ) {
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
    #error "Current Architecture does not have sqrt defined!"
#endif
}
/// inverse sqrt
FORCE_INLINE f32 inv_sqrt( f32 x ) {
#if LD_SIMD_WIDTH != 1
    return lane1f_inv_sqrt( x );
#else
    return 1.0f / sqrt( x );
#endif
}
/// inverse sqrt
FORCE_INLINE f64 inv_sqrt( f64 x ) {
    return 1.0 / sqrt( x );
}

/// raise to the power, integer exponent
FORCE_INLINE f32 powi( f32 base, i32 exponent ) {
    u32 exponent_abs = absolute( exponent );
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
FORCE_INLINE f64 powi( f64 base, i32 exponent ) {
    u32 exponent_abs = absolute( exponent );
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
FORCE_INLINE f32 pow( f32 base, f32 exponent ) {
    // TODO(alicia): REPLACE
    return powi( base, (i32)exponent );
    // return impl::_powf_( base, exponent );
}
/// raise to the power, float exponent
FORCE_INLINE f64 pow( f64 base, f64 exponent ) {
    // TODO(alicia): REPLACE
    return powi( base, (i32)exponent );
    // return impl::_pow_( base, exponent );
}

/// float modulus
FORCE_INLINE f32 mod( f32 lhs, f32 rhs ) {
    if(0.0f == rhs) {
        return lhs;
    }

    f32 m = lhs - ( rhs * (f32)floor32( lhs / rhs ) );

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
FORCE_INLINE f64 mod( f64 lhs, f64 rhs ) {
    if(0.0 == rhs) {
        return lhs;
    }

    f64 m = lhs - ( rhs * (f64)floor64( lhs / rhs ) );

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
FORCE_INLINE b32 is_nan( f32 x ) {
    u32 bitpattern = *(u32*)&x;

    u32 exp = bitpattern & F32::EXPONENT_MASK;
    u32 man = bitpattern & F32::MANTISSA_MASK;

    return exp == F32::EXPONENT_MASK && man != 0;
}
/// check if single-precision float is zero
FORCE_INLINE b32 is_zero( f32 x ) {
    u32 bitpattern = *(u32*)&x;
    return (bitpattern & 0x7FFFFFFF) == 0 || (bitpattern == 0x80000000);
}
/// check if double-precision float is not a number
FORCE_INLINE b32 is_nan( f64 x ) {
    u64 bitpattern = *(u64*)&x;

    u64 exp = bitpattern & F64::EXPONENT_MASK;
    u64 man = bitpattern & F64::MANTISSA_MASK;

    return exp == F64::EXPONENT_MASK && man != 0;
}
/// check if double-precision float is zero
FORCE_INLINE b32 is_zero( f64 x ) {
    u64 bitpattern = *(u64*)&x;
    return (bitpattern & 0x7FFFFFFFFFFFFFFF) == 0 || (bitpattern == 0x8000000000000000);
}

/// Wrap a degree value into 0.0 -> 360.0 range
FORCE_INLINE f32 wrap_degrees( f32 degrees ) {
    f32 result = mod( degrees, 360.0f );
    if( result < 0.0f ) {
        result += 360.0f;
    }
    return result;
}
/// Wrap a degree value into 0.0 -> 360.0 range
FORCE_INLINE f64 wrap_degrees( f64 degrees ) {
    f64 result = mod( degrees, 360.0 );
    if( result < 0.0 ) {
        result += 360.0;
    }
    return result;
}
/// Wrap a radians value into -Pi -> Pi range
FORCE_INLINE f32 wrap_pi( f32 radians ) {
    return mod( radians + F32::PI, F32::TAU ) - F32::PI;
}
/// Wrap a radians value into -Pi -> Pi range
FORCE_INLINE f64 wrap_pi( f64 radians ) {
    return mod( radians + F64::PI, F64::TAU ) - F64::PI;
}

/// sine function
FORCE_INLINE f32 sin( f32 x ) {
    x = wrap_pi(x);
    return x -
        ( powi( x, 3 ) / F32::THREE_FACTORIAL ) +
        ( powi( x, 5 ) / F32::FIVE_FACTORIAL  ) -
        ( powi( x, 7 ) / F32::SEVEN_FACTORIAL ) +
        ( powi( x, 9 ) / F32::NINE_FACTORIAL  );
        // - ( powi( x, 11 ) / F32::ELEVEN_FACTORIAL )
}
/// sine function
FORCE_INLINE f64 sin( f64 x ) {
    x = wrap_pi(x);
    return x -
        ( powi( x, 3 ) / F64::THREE_FACTORIAL ) +
        ( powi( x, 5 ) / F64::FIVE_FACTORIAL )  -
        ( powi( x, 7 ) / F64::SEVEN_FACTORIAL ) +
        ( powi( x, 9 ) / F64::NINE_FACTORIAL );
        // - ( powi( x, 11 ) / F64::ELEVEN_FACTORIAL )
}
/// arc-sine function
FORCE_INLINE f32 asin( f32 x ) {
    // don't ask me how i figured this shit out
    // i don't even know
    f32 sign_of_x = sign( x );
    f32 x_abs = x * sign_of_x;
    f32 x_sqr = x_abs * x_abs;

    const f32 magic_0 =  1.5707288f;
    const f32 magic_1 = -0.2121144f;
    const f32 magic_2 =  0.0742610f;
    const f32 magic_3 = -0.0187293f;

    f32 result = F32::HALF_PI - sqrt( 1.0f - x_abs ) * (
        magic_0 +
        ( magic_1 * x_abs ) +
        ( magic_2 * x_sqr ) +
        ( magic_3 * ( x_sqr * x_abs ) )
    );

    return result * sign_of_x;
}
/// arc-sine function
FORCE_INLINE f64 asin( f64 x ) {
    f64 sign_of_x = sign( x );
    f64 x_abs = x * sign_of_x;
    f64 x_sqr = x_abs * x_abs;

    const f64 magic_0 =  1.5707288;
    const f64 magic_1 = -0.2121144;
    const f64 magic_2 =  0.0742610;
    const f64 magic_3 = -0.0187293;

    f64 result = F64::HALF_PI - sqrt( 1.0 - x_abs ) * (
        magic_0 +
        ( magic_1 * x_abs ) +
        ( magic_2 * x_sqr ) +
        ( magic_3 * ( x_sqr * x_abs ) )
    );

    return result * sign_of_x;
}

/// arc-sine function, does not return NAN
FORCE_INLINE f32 asin_real( f32 x ) {
    return absolute(x) >= 1.0f ? F32::HALF_PI * sign(x) : asin(x);
}
/// arc-sine function, does not return NAN
FORCE_INLINE f64 asin_real( f64 x ) {
    return absolute(x) >= 1.0 ? F64::HALF_PI * sign(x) : asin(x);
}

/// cosine function
FORCE_INLINE f32 cos( f32 x ) {
    x = wrap_pi(x);
    return 1.0f -
        ( powi( x, 2 ) / F32::TWO_FACTORIAL ) +
        ( powi( x, 4 ) / F32::FOUR_FACTORIAL )  -
        ( powi( x, 6 ) / F32::SIX_FACTORIAL ) +
        ( powi( x, 8 ) / F32::EIGHT_FACTORIAL );
        // - ( pow( x, 10 ) / F32::TEN_FACTORIAL )
}
/// cosine function
FORCE_INLINE f64 cos( f64 x ) {
    x = wrap_pi(x);
    return 1.0 -
        ( powi( x, 2 ) / F64::TWO_FACTORIAL ) +
        ( powi( x, 4 ) / F64::FOUR_FACTORIAL )  -
        ( powi( x, 6 ) / F64::SIX_FACTORIAL ) +
        ( powi( x, 8 ) / F64::EIGHT_FACTORIAL );
        // - ( powi( x, 10) / F64::TEN_FACTORIAL )
}
/// arc-cosine function
FORCE_INLINE f32 acos( f32 x ) {
    return -asin(x) + F32::HALF_PI;
}
/// arc-cosine function
FORCE_INLINE f64 acos( f64 x ) {
    return -asin(x) + F64::HALF_PI;
}

// sin-cos function
FORCE_INLINE tuplef32 sincos( f32 x ) {
    return { sin(x), cos(x) };
}
// sin-cos function
FORCE_INLINE tuplef64 sincos( f64 x ) {
    return { sin(x), cos(x) };
}

/// tangent function
FORCE_INLINE f32 tan( f32 x ) {
    tuplef32 sc = sincos( x );
    return is_zero(sc.f1) ? F32::NaN : sc.f0 / sc.f1;
}
/// tangent function
FORCE_INLINE f64 tan( f64 x ) {
    tuplef64 sc = sincos( x );
    return is_zero(sc.f1) ? F64::NaN : sc.f0 / sc.f1;
}
/// arc-tangent function
FORCE_INLINE f32 atan( f32 x ) {
    return x -
        ( powi( x, 3 )  / 3.0f ) +
        ( powi( x, 5 )  / 5.0f ) -
        ( powi( x, 7 )  / 7.0f ) +
        ( powi( x, 9 )  / 9.0f ) -
        ( powi( x, 11 ) / 11.0f ) +
        ( powi( x, 13 ) / 13.0f );
}
/// arc-tangent function
FORCE_INLINE f64 atan( f64 x ) {
    return x -
        ( powi( x, 3 )  / 3.0 ) +
        ( powi( x, 5 )  / 5.0 ) -
        ( powi( x, 7 )  / 7.0 ) +
        ( powi( x, 9 )  / 9.0 ) -
        ( powi( x, 11 ) / 11.0 ) +
        ( powi( x, 13 ) / 13.0 );
}

/// two argument arc-tangent function
FORCE_INLINE f32 atan2( f32 y, f32 x ) {
    if( y == 0.0f ) {
        if( x < 0.0f ) {
            return F32::PI;
        } else if( x == 0.0f ) {
            return F32::NaN;
        }
    }

    f32 x_sqr = x * x;
    f32 y_sqr = y * y;
    return 2.0f * atan( y / ( sqrt( x_sqr + y_sqr ) + x ) );
}
/// two argument arc-tangent function
FORCE_INLINE f64 atan2( f64 y, f64 x ) {
    if( y == 0.0 ) {
        if( x < 0.0 ) {
            return F64::PI;
        } else if( x == 0.0 ) {
            return F64::NaN;
        }
    }

    f64 x_sqr = x * x;
    f64 y_sqr = y * y;
    return 2.0 * atan( y / ( sqrt( x_sqr + y_sqr ) + x ) );
}

/// convert degrees to radians
FORCE_INLINE f32 to_rad( f32 theta ) {
    return theta * ( F32::PI / 180.0f );
}
/// convert degrees to radians
FORCE_INLINE f64 to_rad( f64 theta ) {
    return theta * ( F64::PI / 180.0 );
}
/// convert radians to degrees 
FORCE_INLINE f32 to_deg( f32 theta ) {
    return theta * ( 180.0f / F32::PI );
}
/// convert radians to degrees 
FORCE_INLINE f64 to_deg( f64 theta ) {
    return theta * ( 180.0 / F64::PI );
}

/// natural logarithm
FORCE_INLINE f32 logarithm( f32 x ) {
    // TODO(alicia): replace with an actual function!
    return F32::NaN * x;
    // return impl::_logf_( x );
}
/// natural logarithm
FORCE_INLINE f64 logarithm( f64 x ) {
    // TODO(alicia): replace with an actual function!
    return F32::NaN * x;
    // return impl::_log_( x );
}

/// linear interpolation
FORCE_INLINE f32 lerp( f32 a, f32 b, f32 t ) {
    return ( 1.0f - t ) * a + b * t;
}
/// linear interpolation
FORCE_INLINE f64 lerp( f64 a, f64 b, f64 t ) {
    return ( 1.0 - t ) * a + b * t;
}
/// linear interpolation, t clamped to 0-1
FORCE_INLINE f32 lerp_clamped( f32 a, f32 b, f32 t ) {
    return lerp( a, b, clamp01(t) );
}
/// linear interpolation, t clamped to 0-1
FORCE_INLINE f64 lerp_clamped( f64 a, f64 b, f64 t ) {
    return lerp( a, b, clamp01(t) );
}
/// inverse linear interpolation
FORCE_INLINE f32 inverse_lerp( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}
/// inverse linear interpolation
FORCE_INLINE f64 inverse_lerp( f64 a, f64 b, f64 v ) {
    return ( v - a ) / ( b - a );
}
/// remap value from input min/max to output min/max
FORCE_INLINE f32 remap( f32 imin, f32 imax, f32 omin, f32 omax, f32 v ) {
    const f32 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}
/// remap value from input min/max to output min/max
FORCE_INLINE f64 remap( f64 imin, f64 imax, f64 omin, f64 omax, f64 v ) {
    const f64 t = inverse_lerp( imin, imax, v );
    return lerp( omin, omax, t );
}

/// smooth step interpolation
FORCE_INLINE f32 smooth_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) * ( 3.0f - t * 2.0f ) * t * t + a;
}
/// smooth step interpolation
FORCE_INLINE f64 smooth_step( f64 a, f64 b, f64 t ) {
    return ( b - a ) * ( 3.0 - t * 2.0 ) * t * t + a;
}
/// smooth step interpolation, t clamped to 0-1
FORCE_INLINE f32 smooth_step_clamped( f32 a, f32 b, f32 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smooth step interpolation, t clamped to 0-1
FORCE_INLINE f64 smooth_step_clamped( f64 a, f64 b, f64 t ) {
    return smooth_step( a, b, clamp01(t) );
}
/// smoother step interpolation
FORCE_INLINE f32 smoother_step( f32 a, f32 b, f32 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0f - 15.0f ) + 10.0f ) * t * t * t ) + a;
}
/// smoother step interpolation
FORCE_INLINE f64 smoother_step( f64 a, f64 b, f64 t ) {
    return ( b - a ) *
        ( ( t * ( t * 6.0 - 15.0 ) + 10.0 ) * t * t * t ) + a;
}
/// smoother step interpolation, t clamped to 0-1
FORCE_INLINE f32 smoother_step_clamped( f32 a, f32 b, f32 t ) {
    return smoother_step( a, b, clamp01(t) );
}
/// smoother step interpolation, t clamped to 0-1
FORCE_INLINE f64 smoother_step_clamped( f64 a, f64 b, f64 t ) {
    return smoother_step( a, b, clamp01(t) );
}

#endif // header guard
