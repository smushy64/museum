#if !defined(LD_CORE_CONSTANTS_H)
#define LD_CORE_CONSTANTS_H
// * Description:  Constants
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 12, 2023

/// Size of the stack. Must always compile with this value.
#define STACK_SIZE ((usize)(megabytes(1)))

/// 32-bit floating point constants

/// Largest finite f32 value
#define F32_MAX (3.40282347E+38f)
/// Smallest finite f32 value
#define F32_MIN (-3.40282347E+38f)
/// Not a number
#define F32_NAN ( 0.0f / 0.0f )
/// Smallest positive f32 value
#define F32_MIN_POS (1.17549435E-38f)
/// Positive infinity
#define F32_POS_INFINITY (1.0f / 0.0f)
/// Positive infinity
#define F32_NEG_INFINITY (-(1.0f / 0.0f))
/// Pi constant
#define F32_PI (3.141592741f)
/// Tau constant
#define F32_TAU (F32_PI * 2.0f)
/// Half Pi constant
#define F32_HALF_PI (F32_PI / 2.0f)
/// Euler's number.
#define F32_EULER (2.718281828459f)
/// To degrees constant.
#define F32_TO_DEG (180.0f / F32_PI)
/// To radians constant.
#define F32_TO_RAD (F32_PI / 180.0f)
/// Epsilon constant
#define F32_EPSILON (1.1920929E-7f)
/// Approximate number of significant digits in base-10
#define F32_SIGNIFICANT_DIGITS (6)
/// Number of significant digits in base-2
#define F32_MANTISSA_DIGITS (24)
/// Bitmask of single precision float exponent
#define F32_EXPONENT_MASK (~(0xFFFFFFFF << 8ul) << 23ul)
/// Bitmask of single precision float mantissa
#define F32_MANTISSA_MASK ((1ul << 23ul) - 1ul)

#define F32_ONE_FACTORIAL    ( 1.0f )
#define F32_TWO_FACTORIAL    ( 2.0f  * F32_ONE_FACTORIAL    )
#define F32_THREE_FACTORIAL  ( 3.0f  * F32_TWO_FACTORIAL    )
#define F32_FOUR_FACTORIAL   ( 4.0f  * F32_THREE_FACTORIAL  )
#define F32_FIVE_FACTORIAL   ( 5.0f  * F32_FOUR_FACTORIAL   )
#define F32_SIX_FACTORIAL    ( 6.0f  * F32_FIVE_FACTORIAL   )
#define F32_SEVEN_FACTORIAL  ( 7.0f  * F32_SIX_FACTORIAL    )
#define F32_EIGHT_FACTORIAL  ( 8.0f  * F32_SEVEN_FACTORIAL  )
#define F32_NINE_FACTORIAL   ( 9.0f  * F32_EIGHT_FACTORIAL  )
#define F32_TEN_FACTORIAL    ( 10.0f * F32_NINE_FACTORIAL   )
#define F32_ELEVEN_FACTORIAL ( 11.0f * F32_TEN_FACTORIAL    )
#define F32_TWELVE_FACTORIAL ( 12.0f * F32_ELEVEN_FACTORIAL )

/// 64-bit floating point constants

/// Largest finite f64 value
#define F64_MAX (1.7976931348623157E+308)
/// Smallest finite f64 value
#define F64_MIN (-1.7976931348623157E+308)
/// Not a number
#define F64_NAN (0.0 / 0.0)
/// Smallest positive f32 value
#define F64_MIN_POS (2.2250738585072014E-308)
/// Positive infinity
#define F64_POS_INFINITY (1.0 / 0.0)
/// Positive infinity
#define F64_NEG_INFINITY (-(1.0 / 0.0))
/// Pi constant
#define F64_PI (3.14159265358979323846)
/// Tau constant
#define F64_TAU (F64_PI * 2.0)
/// Half Pi constant
#define F64_HALF_PI (F64_PI / 2.0)
/// Euler's number.
#define F64_EULER (2.718281828459045)
/// To degrees constant.
#define F64_TO_DEG (180.0 / F64_PI)
/// To radians constant.
#define F64_TO_RAD (F64_PI / 180.0)
/// Epsilon constant
#define F64_EPSILON (2.2204460492503131E-16)
/// Approximate number of significant digits in base-10
#define F64_SIGNIFICANT_DIGITS (15)
/// Number of significant digits in base-2
#define F64_MANTISSA_DIGITS (54)
/// bitmask of double precision float exponent
#define F64_EXPONENT_MASK (0x7FFULL << 52ull)
/// bitmask of double precision float mantissa 
#define F64_MANTISSA_MASK ((1ull << 52ull) - 1ull)

#define F64_ONE_FACTORIAL    (1.0)
#define F64_TWO_FACTORIAL    (2.0  * F64_ONE_FACTORIAL   )
#define F64_THREE_FACTORIAL  (3.0  * F64_TWO_FACTORIAL   )
#define F64_FOUR_FACTORIAL   (4.0  * F64_THREE_FACTORIAL )
#define F64_FIVE_FACTORIAL   (5.0  * F64_FOUR_FACTORIAL  )
#define F64_SIX_FACTORIAL    (6.0  * F64_FIVE_FACTORIAL  )
#define F64_SEVEN_FACTORIAL  (7.0  * F64_SIX_FACTORIAL   )
#define F64_EIGHT_FACTORIAL  (8.0  * F64_SEVEN_FACTORIAL )
#define F64_NINE_FACTORIAL   (9.0  * F64_EIGHT_FACTORIAL )
#define F64_TEN_FACTORIAL    (10.0 * F64_NINE_FACTORIAL  )
#define F64_ELEVEN_FACTORIAL (11.0 * F64_TEN_FACTORIAL   )
#define F64_TWELVE_FACTORIAL (12.0 * F64_ELEVEN_FACTORIAL)

/// 8-bit unsigned integer constants

/// Largest u8 value
#define U8_MAX (255)
/// Smallest u8 value
#define U8_MIN (0)

/// 16-bit unsigned integer constants

/// Largest u16 value
#define U16_MAX (65535)
/// Smallest u16 value
#define U16_MIN (0)

/// 32-bit unsigned integer constants

/// Largest u32 value
#define U32_MAX (4294967295ul)
/// Smallest u32 value
#define U32_MIN (0ul)

/// 64-bit unsigned integer constants

/// Largest u64 value
#define U64_MAX (18446744073709551615ull)
/// Smallest u64 value
#define U64_MIN (0ull)

/// 8-bit integer constants

/// Largest i8 value
#define I8_MAX (127)
/// Smallest i8 value
#define I8_MIN (-128)
/// Sign mask
#define I8_SIGN_MASK (1 << 7)

/// 16-bit integer constants

/// Largest i16 value
#define I16_MAX (32767)
/// Smallest i16 value
#define I16_MIN (-32768)
/// Sign mask
#define I16_SIGN_MASK (1 << 15)

/// 32-bit integer constants

/// Largest i32 value
#define I32_MAX (2147483647l)
/// Smallest i32 value
#define I32_MIN (-2147483648l)
/// Sign mask
#define I32_SIGN_MASK (1u << 31u)

/// 64-bit integer constants

/// Largest i64 value
#define I64_MAX (9223372036854775807ll)
/// Smallest i64 value
#define I64_MIN (-9223372036854775807ll - 1)
/// Sign mask
#define I64_SIGN_MASK (1ull << 63ull)

#if defined(LD_ARCH_64_BIT)
    /// Largest pointer sized int value.
    #define ISIZE_MAX (I64_MAX)
    /// Smallest pointer sized int value.
    #define ISIZE_MIN (I64_MIN)
    /// Pointer sized int sign mask.
    #define ISIZE_SIGN_MASK (I64_SIGN_MASK)

    /// Largest pointer sized unsigned int value.
    #define USIZE_MAX (U64_MAX)
    /// Smallest pointer sized unsigned int value.
    #define USIZE_MIN (U64_MIN)
#else
    /// Largest pointer sized int value.
    #define ISIZE_MAX (I32_MAX)
    /// Smallest pointer sized int value.
    #define ISIZE_MIN (I32_MIN)
    /// Pointer sized int sign mask.
    #define ISIZE_SIGN_MASK (I32_SIGN_MASK)

    /// Largest pointer sized unsigned int value.
    #define USIZE_MAX (U32_MAX)
    /// Smallest pointer sized unsigned int value.
    #define USIZE_MIN (U32_MIN)
#endif

#endif // header guard
