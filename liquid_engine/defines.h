#if !defined(DEFINES_HPP)
#define DEFINES_HPP
/**
 * Description:  Typedefs for integral types and relevant constants
 *               Compiler independent asserts
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 24, 2023
 * Includes:     <stdint.h>
 *               <intrin.h> MSVC ONLY
 * Notes:        define LD_ASSERTIONS to enable DEBUG_ASSERT macro
*/
/// compiler defines
#if defined(__GNUC__) || defined(__GNUG__)
    #define LD_COMPILER_GCC
#endif

#if defined(__clang__)
    #if defined(LD_COMPILER_GCC)
        #undef LD_COMPILER_GCC
    #endif
    #define LD_COMPILER_CLANG
#endif

#if defined(_MSC_VER)
    #if defined(LD_COMPILER_GCC)
        #undef LD_COMPILER_GCC
    #endif
    #if defined(LD_COMPILER_CLANG)
        #undef LD_COMPILER_CLANG
    #endif
    #define LD_COMPILER_MSVC
#endif

#if !defined(LD_COMPILER_GCC) && !defined(LD_COMPILER_CLANG) && !defined(LD_COMPILER_MSVC)
    #define LD_COMPILER_UNKNOWN
#endif

// platform defines
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WIN64__)
    #define LD_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
    #define LD_PLATFORM_LINUX
#elif defined(__ANDROID__)
    #define LD_PLATFORM_ANDROID
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if defined(TARGET_IPHONE_SIMULATION) || defined(TARGET_OS_IPHONE)
        #define LD_PLATFORM_IOS
    #elif defined(TARGET_OS_MAC)
        #define LD_PLATFORM_MACOS
    #endif
#else
    #define LD_PLATFORM_UNKNOWN
#endif

// platform cpu defines
#if defined(_M_IX86) || defined(__i386__)
    #define LD_ARCH_X86
    #define LD_ARCH_32_BIT
#endif

#if defined(__x86_64__) || defined(_M_X64_)
    #if !defined(LD_ARCH_X86)
        #define LD_ARCH_X86
    #endif
    #if defined(LD_ARCH_32_BIT)
        #undef LD_ARCH_32_BIT
    #endif
    #define LD_ARCH_64_BIT
#endif // if x86

#if defined(__arm__) || defined(_M_ARM_)
    #define LD_ARCH_ARM
    #define LD_ARCH_32_BIT
#endif // if arm

#if defined(__aarch64__)
    #if !defined(LD_ARCH_ARM)
        #define LD_ARCH_ARM
    #endif
    #if defined(LD_ARCH_32_BIT)
        #undef LD_ARCH_32_BIT
    #endif
    #define LD_ARCH_64_BIT
#endif // if arm64


#if !defined(LD_SIMD_WIDTH)
    #define LD_SIMD_WIDTH 1
#else
    #if !(LD_SIMD_WIDTH == 1 || LD_SIMD_WIDTH == 4 || LD_SIMD_WIDTH == 8)
        #error "LD_SIMD_WIDTH can only be 1, 4 or 8!!!"
    #endif
#endif // simd

#if defined(LD_ARCH_X86)

typedef unsigned char       u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char       i8;
typedef signed short     i16;
typedef signed int       i32;
typedef signed long long i64;

#if defined(LD_ARCH_64_BIT)
    typedef u64 usize;
    typedef i64 isize;
#else // if x86_64
    typedef u32 usize;
    typedef i32 isize;
#endif // if x86_32

#else // other
    #include <stdint.h>
    /// pointer-sized unsigned integer
    typedef uintptr_t usize;
    /// pointer-sized integer
    typedef intptr_t  isize;

    /// 8-bit unsigned integer
    typedef uint8_t  u8;
    /// 16-bit unsigned integer
    typedef uint16_t u16;
    /// 32-bit unsigned integer
    typedef uint32_t u32;
    /// 64-bit unsigned integer
    typedef uint64_t u64;

    /// 8-bit integer
    typedef int8_t  i8;
    /// 16-bit integer
    typedef int16_t i16;
    /// 32-bit integer
    typedef int32_t i32;
    /// 64-bit integer
    typedef int64_t i64;

#endif // if other architecture

/// 8-bit boolean
typedef u8  b8;
/// 32-bit boolean
typedef u32 b32;

/// UTF-8 | 8-bit character
typedef char  c8;
/// UTF-16 | 16-bit character
typedef short c16;
/// UTF-32 | 32-bit character
typedef int   c32;

/// single precision IEEE-754 floating-point number
typedef float f32;
/// double precision IEEE-754 floating-point number
typedef double f64;

/// Tuple containing two single precision floats
typedef union tuplef32 {
    struct {
        f32 f0;
        f32 f1;
    };
    f32 f[2];
} tuplef32;
/// Tuple containing two double precision floats
typedef union tuplef64 {
    struct {
        f64 f0;
        f64 f1;
    };
    f64 f[2];
} tuplef64;

/// void* pointer alias, might come in handy at some point
typedef void* pvoid;

#if !defined( __cplusplus )
    #define true  1
    #define false 0
#endif

#if !defined(NULL)
    #define NULL 0
#endif

#define LD_CONTACT_MESSAGE \
    "Please contact me at smushybusiness@gmail.com if you see this."


/// Convert macro to const char*
#define TO_STRING( foo ) #foo
/// Convert macro value to const char*
#define VALUE_TO_STRING( foo ) TO_STRING( foo )

/// Make a version uint32
#define LD_MAKE_VERSION( major, minor )\
    ((u32)major << 15u) | ((u32)minor)
/// Get major version from uint32 version
#define LD_GET_MAJOR( version )\
    ((u32)version >> 15u)
/// Get minor version from uint32 version
#define LD_GET_MINOR( version )\
    ((u32)version & 0x0000FFFF)

/// Calculate number of elements in a static array
#define STATIC_ARRAY_COUNT( array ) \
    ( sizeof(array) / sizeof((array)[0]) )

/// Calculate byte size of a static array
#define STATIC_ARRAY_SIZE( array ) \
    (sizeof(array))

#if defined(__cplusplus)
    #define EXTERNC extern "C"
#else
    #define EXTERNC
#endif

// panic
// export/import definitions 
// static assertions
// always/never inline
// never optimize away
// packed struct declaration
#if defined(LD_COMPILER_MSVC)
    #include <intrin.h>
    #define PANIC() __debugbreak()

    #define STATIC_ASSERT static_assert
    // TODO(alicia): HOTPATH/NOOPTIMIZE FOR MSVC

    #define FORCE_INLINE __forceinline
    #define NO_INLINE    __declspec(noinline)

    #define MAKE_PACKED( declaration ) __pragma( pack(push, 1) ) declaration __pragma( pack(pop) )

    #if defined(LD_EXPORT)
        #define LD_API __declspec(dllexport)
    #else // import
        #define LD_API __declspec(dllimport)
    #endif

    #define unused(x) (void)(x)

#else // not MSVC
    #define PANIC() __builtin_trap()

    #if defined(LD_COMPILER_GCC)
        #define NO_OPTIMIZE __attribute__((optimize("O0")))
    #else // clang
        #define NO_OPTIMIZE __attribute__((optnone))
    #endif

    #define STATIC_ASSERT _Static_assert
    #define HOT_PATH      __attribute__((hot))
    #define FORCE_INLINE  __attribute__((always_inline)) inline
    #define NO_INLINE     __attribute__((noinline))
    #define MAKE_PACKED( declaration ) declaration __attribute__((__packed__))
    #define PACKED __attribute__((__packed__))

    #define unused(x) (void)((x))

    #if defined(LD_EXPORT)
        #if defined(LD_PLATFORM_WINDOWS)
            #define LD_API __declspec(dllexport)
        #else
            #define LD_API __attribute__((visibility("default")))
        #endif
    #else
        #if defined(LD_PLATFORM_WINDOWS)
            #define LD_API __declspec(dllimport) EXTERNC
        #else
            #define LD_API EXTERNC
        #endif
    #endif

#endif

#if defined(LD_EXPORT)
    #define LD_API_INTERNAL
#endif

// assert that type sizes are correct
STATIC_ASSERT(sizeof(u8)  == 1, "Expected u8 to be 1 byte!");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes!");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes!");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes!");
STATIC_ASSERT(sizeof(i8)  == 1, "Expected i8 to be 1 byte!");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes!");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes!");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes!");
STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes!");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes!");
STATIC_ASSERT(sizeof(c8)  == 1, "Expected c8 to be 1 byte!" );
STATIC_ASSERT(sizeof(c16) == 2, "Expected c16 to be 2 bytes!" );
STATIC_ASSERT(sizeof(c32) == 4, "Expected c32 to be 4 bytes!" );

#if defined(LD_ARCH_32_BIT)
    STATIC_ASSERT(sizeof(usize) == sizeof(u32), "Expected to be running on 32 bit architecture!");
#elif defined(LD_ARCH_64_BIT)
    STATIC_ASSERT(sizeof(usize) == sizeof(u64), "Expected to be running on 64 bit architecture!");
#endif // if arch64/32

/// debug assertions
#if defined(LD_ASSERTIONS)
    #define ASSERT(condition) do {\
        if(!(condition)) {\
            PANIC();\
        }\
    } while(0)
#else
    #define ASSERT(condition) (condition)
#endif

#define internal static
#define local    static
#define global   static

#if defined(__cplusplus)
    #define headerfn inline
#else
    #define headerfn extern inline
#endif

#define loop     for( ;; )

/// Define a 24-bit RGB value (using u32)
#define RGB_U32( r, g, b )\
    ( 255 << 24u | b << 16u | g << 8u | r )
/// Define a 32-bit RGBA value
#define RGBA_U32( r, g, b, a )\
    ( a << 24u | b << 16u | g << 8u | r )

/// Swap two values.
#define SWAP( a, b ) do {\
    __typeof(a) intermediate = a;\
    a = b;\
    b = intermediate;\
} while(0)

/// Check if bits match given mask
#define CHECK_BITS( bits, mask ) ( ( (bits) & (mask) ) == (mask) )
/// Toggle masked bits
#define TOGGLE_BITS( bits, toggle_mask ) do {\
    bits ^= (toggle_mask);\
} while(0)
/// Clear masked bits
#define CLEAR_BIT( bits, mask ) do {\
    bits &= ~(mask);\
} while(0)

#define KILOBYTES(num) ( num * 1024ULL )
#define MEGABYTES(num) ( KILOBYTES( num ) * 1024ULL )
#define GIGABYTES(num) ( MEGABYTES( num ) * 1024ULL )

/// Size of the stack. Must always compile with this value.
#define STACK_SIZE (MEGABYTES(1))

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
/// Epsilon constant
#define F32_EPSILON (1.1920929E-7f)
/// Approximate number of significant digits in base-10
#define F32_SIGNIFICANT_DIGITS (6)
/// Number of significant digits in base-2
#define F32_MANTISSA_DIGITS (24)
/// Bitmask of single precision float exponent
#define F32_EXPONENT_MASK (~(0xFFFFFFFF << 8ul) << 23ul)
/// Bitmask of single precision float mantissa
#define F32_MANTISSA_MASK ((1 << 23) - 1ul)

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

#endif
