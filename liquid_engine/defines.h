/**
 * Description:  Typedefs for integral types and relevant constants
 *               Compiler independent asserts
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 24, 2023
 * Includes:     <stdint.h>
 *               <intrin.h> MSVC ONLY
 * Notes:        define LD_ASSERTIONS to enable DEBUG_ASSERT macro
*/
#if !defined(DEFINES_HPP)
#define DEFINES_HPP

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

/// 8-bit boolean
typedef uint8_t  b8;
/// 32-bit boolean
typedef uint32_t b32;

/// UTF-8 | 8-bit character
typedef i8 c8;
/// UTF-16 | 16-bit character
typedef i16 c16;
/// UTF-32 | 32-bit character
typedef i32 c32;

/// single precision IEEE-754 floating-point number
typedef float f32;
/// double precision IEEE-754 floating-point number
typedef double f64;

/// Tuple containing two single precision floats
union tuplef32 {
    struct {
        f32 f0;
        f32 f1;
    };
    f32 f[2];
};
/// Tuple containing two double precision floats
union tuplef64 {
    struct {
        f64 f0;
        f64 f1;
    };
    f64 f[2];
};

/// void* pointer alias, might come in handy at some point
typedef void* pvoid;

#define LD_CONTACT_MESSAGE \
    "Please contact me at smushybusiness@gmail.com if you see this."

#define internal static
#define local    static
#define global   static
#define loop     for( ;; )
/// mark value as unused
#define unused(x) x = x

/// Check if bits are set in bitfield
#define ARE_BITS_SET( bits, mask ) ( ((bits) & (mask)) == (mask) )

/// Convert macro to const char*
#define TO_STRING( foo ) #foo
/// Convert macro value to const char*
#define VALUE_TO_STRING( foo ) TO_STRING( foo )

/// Make a version uint32
#define LD_MAKE_VERSION( major, minor )\
    ((u32)major << 16u) | ((u32)minor)
/// Get major version from uint32 version
#define LD_GET_MAJOR( version )\
    ((u32)version >> 16u)
/// Get minor version from uint32 version
#define LD_GET_MINOR( version )\
    ((u32)minor & 0x0000FFFF)

/// Calculate number of elements in a static array
#define STATIC_ARRAY_COUNT( array ) \
    ( sizeof(array) / sizeof((array)[0]) )

/// Calculate byte size of a static array
#define STATIC_ARRAY_SIZE( array ) \
    (sizeof(array))

#define KILOBYTES(num) ( num * 1024ULL )
#define MEGABYTES(num) ( KILOBYTES( num ) * 1024ULL )
#define GIGABYTES(num) ( MEGABYTES( num ) * 1024ULL )

#define BYTES_TO_KB(bytes) ((f32)bytes / 1024.0f)
#define KB_TO_MB(kb)       ((f32)kb / 1024.0f)
#define MB_TO_GB(mb)       ((f32)mb / 1024.0f)

#define KB_TO_BYTES(kb) ((f32)kb * 1024.0f)
#define MB_TO_KB(mb)    ((f32)mb * 1024.0f)
#define GB_TO_MB(gb)    ((f32)gb * 1024.0f)

#define BYTES_TO_BEST_REPRESENTATION(bytes, result) do {\
    if( bytes >= 1024 ) {\
        result = BYTES_TO_KB(bytes);\
        if( result >= 1024.0f ) {\
            result = KB_TO_MB(result);\
            if( result >= 1024.0f ) {\
                result = MB_TO_GB(result);\
            }\
        }\
    } else {\
        result = (f32)bytes;\
    }\
} while(0)

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

// panic
// export/import definitions 
// static assertions
// always/never inline
// never optimize away
// packed struct declaration
#if defined(LD_COMPILER_MSVC)
    #include <intrin.h>
    #define LD_PANIC() __debugbreak()

    #define LD_STATIC_ASSERT static_assert
    #define LD_NO_OPTIMIZE _Pragma( "optimize(\"\", off)" )

    #define LD_ALWAYS_INLINE __forceinline
    #define LD_NOINLINE __declspec(noinline)

    #define LD_PACKED( declaration ) __pragma( pack(push, 1) ) declaration __pragma( pack(pop) )

    #if defined(LD_EXPORT)
        #define LD_API __declspec(dllexport)
    #else // import
        #define LD_API __declspec(dllimport)
    #endif


#else // not MSVC
    #define LD_PANIC() __builtin_trap()

    #if defined(LD_COMPILER_GCC)
        #define LD_STATIC_ASSERT _Static_assert
        #define LD_NO_OPTIMIZE __attribute__((optimize("O0")))
    #else // clang
        #define LD_STATIC_ASSERT _Static_assert
        #define LD_NO_OPTIMIZE __attribute__((optnone))
    #endif

    #define LD_ALWAYS_INLINE __attribute__((always_inline)) inline
    #define LD_NOINLINE      __attribute__((noinline))
    #define LD_PACKED( declaration ) declaration __attribute__((__packed__))

    #if defined(LD_EXPORT)
        #define LD_API __attribute__((visibility("default")))
    #else // import
        // unknown?
        #define LD_API 
    #endif

#endif

#if defined(LD_EXPORT)
    #define LD_API_INTERNAL
#endif

// assert that type sizes are correct
LD_STATIC_ASSERT(sizeof(u8)  == 1, "Expected u8 to be 1 byte!");
LD_STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes!");
LD_STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes!");
LD_STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes!");
LD_STATIC_ASSERT(sizeof(i8)  == 1, "Expected i8 to be 1 byte!");
LD_STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes!");
LD_STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes!");
LD_STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes!");
LD_STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes!");
LD_STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes!");

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
#endif

#if defined(__arm__) || defined(_M_ARM_)
    #define LD_ARCH_ARM
    #define LD_ARCH_32_BIT
#endif

#if defined(__aarch64__)
    #if !defined(LD_ARCH_ARM)
        #define LD_ARCH_ARM
    #endif
    #if defined(LD_ARCH_32_BIT)
        #undef LD_ARCH_32_BIT
    #endif
    #define LD_ARCH_64_BIT
#endif

#if defined(LD_ARCH_32_BIT)
    LD_STATIC_ASSERT(sizeof(usize) == sizeof(u32), "Expected to be running on 32 bit architecture!");
#elif defined(LD_ARCH_64_BIT)
    LD_STATIC_ASSERT(sizeof(usize) == sizeof(u64), "Expected to be running on 64 bit architecture!");
#endif

#if !defined(LD_SIMD_WIDTH)
    #define LD_SIMD_WIDTH 1
#else
    #if !(LD_SIMD_WIDTH == 1 || LD_SIMD_WIDTH == 4 || LD_SIMD_WIDTH == 8)
        #error "LD_SIMD_WIDTH can only be 1, 4 or 8!!!"
    #endif
#endif



/// debug assertions
#if defined(LD_ASSERTIONS)
    #define LD_ASSERT(condition) do {\
        if(!(condition)) {\
            LD_PANIC();\
        }\
    } while(0)
#else
    #define LD_ASSERT(condition)
#endif

/// 32-bit floating point constants
namespace F32 {
    /// Largest finite f32 value
    global const f32 MAX = 3.40282347E+38f;
    /// Smallest finite f32 value
    global const f32 MIN = -3.40282347E+38f;
    /// Not a number
    global const f32 NAN = ( 0.0f / 0.0f );
    /// Smallest positive f32 value
    global const f32 MIN_POS = 1.17549435E-38f;
    /// Positive infinity
    global const f32 POS_INFINITY = (1.0f / 0.0f);
    /// Positive infinity
    global const f32 NEG_INFINITY = (-(1.0f / 0.0f));
    /// Pi constant
    global const f32 PI = 3.141592741f;
    /// Tau constant
    global const f32 TAU = PI * 2.0f;
    /// Half Pi constant
    global const f32 HALF_PI = PI / 2.0f;
    /// Epsilon constant
    global const f32 EPSILON = 1.1920929E-7f;
    /// Approximate number of significant digits in base-10
    global const u32 SIGNIFICANT_DIGITS = 6;
    /// Number of significant digits in base-2
    global const u32 MANTISSA_DIGITS = 24;
    /// bitmask of single precision float exponent
    global const u32 EXPONENT_MASK = ~(0xFFFFFFFF << 8) << 23;
    /// bitmask of single precision float mantissa
    global const u32 MANTISSA_MASK = (1 << 23) - 1;
};

/// 64-bit floating point constants
namespace F64 {
    /// Largest finite f64 value
    global const f64 MAX = 1.7976931348623157E+308;
    /// Smallest finite f64 value
    global const f64 MIN = -1.7976931348623157E+308;
    /// Not a number
    global const f64 NAN = 0.0 / 0.0;
    /// Smallest positive f32 value
    global const f64 MIN_POS = 2.2250738585072014E-308;
    /// Positive infinity
    global const f64 POS_INFINITY = (1.0 / 0.0);
    /// Positive infinity
    global const f64 NEG_INFINITY = (-(1.0 / 0.0));
    /// Pi constant
    global const f64 PI = 3.14159265358979323846;
    /// Tau constant
    global const f64 TAU = (PI * 2.0);
    /// Half Pi constant
    global const f64 HALF_PI = (PI / 2.0);
    /// Epsilon constant
    global const f64 EPSILON = 2.2204460492503131E-16;
    /// Approximate number of significant digits in base-10
    global const u32 SIGNIFICANT_DIGITS = 15;
    /// Number of significant digits in base-2
    global const u32 MANTISSA_DIGITS = 54;
    /// bitmask of double precision float exponent
    global const u64 EXPONENT_MASK = (0x7FFULL << 52);
    /// bitmask of double precision float mantissa 
    global const u64 MANTISSA_MASK = (1ULL << 52) - 1;
};

/// 8-bit unsigned integer constants
namespace U8 {
    /// Largest u8 value
    global const u8 MAX = 255;
    /// Smallest u8 value
    global const u8 MIN = 0;
};

/// 16-bit unsigned integer constants
namespace U16 {
    /// Largest u16 value
    global const u16 MAX = 65535;
    /// Smallest u16 value
    global const u16 MIN = 0;
};

/// 32-bit unsigned integer constants
namespace U32 {
    /// Largest u32 value
    global const u32 MAX = 4294967295;
    /// Smallest u32 value
    global const u32 MIN = 0;
};

/// 64-bit unsigned integer constants
namespace U64 {
    /// Largest u64 value
    global const u64 MAX = 18446744073709551615ULL;
    /// Smallest u64 value
    global const u64 MIN = 0;
};

/// 8-bit integer constants
namespace I8 {
    /// Largest i8 value
    global const i8 MAX = 127;
    /// Smallest i8 value
    global const i8 MIN = -128;
    /// Sign mask
    global const u8 SIGN_MASK = (1 << 7);
};

/// 16-bit integer constants
namespace I16 {
    /// Largest i16 value
    global const i16 MAX = 32767;
    /// Smallest i16 value
    global const i16 MIN = -32768;
    /// Sign mask
    global const u16 SIGN_MASK = (1 << 15);
};

/// 32-bit integer constants
namespace I32 {
    /// Largest i32 value
    global const i32 MAX = 2147483647;
    /// Smallest i32 value
    global const i32 MIN = -2147483648;
    /// Sign mask
    global const u32 SIGN_MASK = (1u << 31u);
};

/// 64-bit integer constants
namespace I64 {
    /// Largest i64 value
    global const i64 MAX = 9223372036854775807;
    /// Smallest i64 value
    global const i64 MIN = -9223372036854775807 - 1;
    /// Sign mask
    global const u64 SIGN_MASK = (1ull << 63ull);
};

#endif
