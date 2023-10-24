#if !defined(LD_CORE_DEFINES_H)
#define LD_CORE_DEFINES_H
/**
 * Description:  Typedefs for integral types and common macro functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 24, 2023
 *
 * Includes:     <TargetConditionals.h> (APPLE ONLY)
 *
 * Notes:        define LD_ASSERTIONS to enable assert macro
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
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        #define LD_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define LD_PLATFORM_MACOS
    #endif
#else
    #define LD_PLATFORM_UNKNOWN
#endif

// platform cpu defines
#if defined(_M_IX86) || defined(__i386__)
    #define LD_ARCH_X86
    #define LD_ARCH_32_BIT

    #if !defined(LD_ARCH_LITTLE_ENDIAN)
        #define LD_ARCH_LITTLE_ENDIAN
    #endif
#endif

#if defined(__x86_64__) || defined(_M_X64_)
    #if !defined(LD_ARCH_X86)
        #define LD_ARCH_X86
    #endif
    #if defined(LD_ARCH_32_BIT)
        #undef LD_ARCH_32_BIT
    #endif
    #define LD_ARCH_64_BIT

    #if !defined(LD_ARCH_LITTLE_ENDIAN)
        #define LD_ARCH_LITTLE_ENDIAN
    #endif
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

#if defined(LD_ARCH_BIG_ENDIAN)
    #error "Big endian architectures are not currently supported!"
#endif

#if defined(LD_ARCH_X86)
    typedef unsigned char       u8;
    typedef unsigned short     u16;
    typedef unsigned int       u32;
    typedef unsigned long long u64;

    typedef signed char       i8;
    typedef signed short     i16;
    typedef signed int       i32;
    typedef signed long long i64;

#else // other
    #include <stdint.h>

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

#if defined(LD_ARCH_64_BIT)
    typedef u64 usize;
    typedef i64 isize;
#else // if x86_64
    typedef u32 usize;
    typedef i32 isize;
#endif // if x86_32

/// 8-bit boolean
typedef u8  b8;
/// 16-bit boolean
typedef u16 b16;
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

/// void* pointer alias
/// useful for macros that name things
/// based on type names
typedef void* pvoid;

#if !defined( __cplusplus )
    #define true  1
    #define false 0
    #define nullptr ((void*)0)
#endif

#if !defined(NULL)
    // NOTE(alicia): i hate c++
    #if defined( __cplusplus )
        #define NULL nullptr
    #else
        #define NULL ((void*)0)
    #endif
#endif

#define LD_CONTACT_MESSAGE \
    "Please contact me at smushybusiness@gmail.com if you see this."

/// Convert macro name to const char*
#define macro_name_to_string( foo ) #foo
/// Convert macro value to const char*
#define macro_value_to_string( foo ) macro_name_to_string( foo )

/// Make a version uint32
#define ld_make_version( major, minor )\
    ((u32)( major ) << 15u) | ((u32)( minor ))
/// Get major version from uint32 version
#define ld_get_major( version )\
    ((u32)( version ) >> 15u)
/// Get minor version from uint32 version
#define ld_get_minor( version )\
    ((u32)( version ) & 0x0000FFFF)

/// Calculate number of elements in a static array
#define static_array_count( array ) \
    ( sizeof(( array )) / sizeof((array)[0]) )

/// Calculate byte size of a static array
#define static_array_size( array ) \
    (sizeof(( array )))

#if !defined(__cplusplus)
    /// Reinterpret value as a different type
    #define reinterpret_cast( type, pointer )\
        (*(type*)( pointer ))
#endif

#if defined(__cplusplus)
    /// External C linkage
    #define c_linkage extern "C"
#else
    /// External C linkage
    #define c_linkage extern
#endif

/// Crash the program >:)
#define panic() __builtin_trap()

/// Mark value as unused.
#define unused(x) (void)((x))

/// Compile-time assertion
#if !defined(__cplusplus)
    #define static_assert _Static_assert
#endif

#if defined(LD_ASSERTIONS)
    /// Runtime assertion
    #define assert(condition) do {\
        if(!(condition)) {\
            panic();\
        }\
    } while(0)
#else
    /// Runtime assertion
    #define assert(condition) unused((condition))
#endif

/// Always optimize regardless of optimization level.
#define hot __attribute__((hot))
/// Always inline function.
#define force_inline __attribute__((always_inline)) inline
/// Never inline function.
#define no_inline __attribute__((noinline))
/// Don't introduce padding to struct.
#define no_padding __attribute__((__packed__))
/// Function is internal to translation unit.
#define internal static
/// Value is local to function.
#define local static
/// Value is global.
#define global static
/// Mark function/struct as deprecated.
#define deprecate __attribute__((deprecated))
/// Mark function/type as possibly unused.
#define maybe_unused __attribute__((unused))
/// Mark value with alignment.
/// Alignment must be a power of two.
#define aligned(alignment) __attribute__((aligned (alignment)))

#if defined(__cplusplus)
    #define restricted __restrict
#else
    #define restricted restrict
#endif

#if defined(LD_COMPILER_GCC)
    /// Do not optimize regardless of optimization level
    #define no_optimize __attribute__((optimize("O0")))
#else // clang
    /// Do not optimize regardless of optimization level
    #define no_optimize __attribute__((optnone))
#endif

#if defined(__cplusplus)
    /// Header inline function
    #define header_only inline
#else
    /// Header inline function
    #define header_only static inline
#endif

/// Infinite loop.
#define loop for( ;; )

/// No-op attribute
#define attribute_none

typedef __builtin_va_list va_list;
#define va_arg   __builtin_va_arg
#define va_start __builtin_va_start
#define va_end   __builtin_va_end

/// Define a 24-bit RGB value (using u32)
#define rgb_u32( r, g, b )\
    ( 255 << 24u | b << 16u | g << 8u | r )
/// Define a 32-bit RGBA value
#define rgba_u32( r, g, b, a )\
    ( a << 24u | b << 16u | g << 8u | r )
/// Spread an array with 2 elements
#define spread_2( array )\
    (array)[0], (array)[1]
/// Spread an array with 3 elements
#define spread_3( array )\
    (array)[0], (array)[1], (array)[2]
/// Spread an array with four elements.
#define spread_4( array )\
    (array)[0], (array)[1], (array)[2], (array)[3]

#if !defined(offsetof)
    /// Get the byte offset of field in struct.
    #define offsetof( struct, field )\
        (usize)((&(((struct*)NULL)->field)))
#endif

/// Swap a and b.
#define swap( a, b ) do {\
    __typeof(a) intermediate = a;\
    a = b;\
    b = intermediate;\
} while(0)

/// Check if bitfield has bits in bitmask set.
#define bitfield_check( bitfield, bitmask )\
    (((bitfield) & (bitmask)) == (bitmask))
/// Check if only bits in bitmask are set.
#define bitfield_check_exact( bitfield, bitmask )\
    (((bitfield) & (bitmask)) == (bitfield))
/// Toggle bits in bitfield using bitmask.
#define bitfield_toggle( bitfield, bitmask )\
    ((bitfield) ^ (bitmask))
/// Clear bits in bitfield using bitmask.
#define bitfield_clear( bitfield, bitmask )\
    ((bitfield) & ~(bitmask))

/// Kilobytes to bytes
#define kilobytes(kb) ( (kb) * 1024ULL )
/// Megabytes to bytes
#define megabytes(mb) ( kilobytes( (mb) ) * 1024ULL )
/// Gigabytes to bytes
#define gigabytes(gb) ( megabytes( (gb) ) * 1024ULL )
/// Terabytes to bytes
#define terabytes(tb) ( terabytes( (tb) ) * 1024ULL )

#if !defined(LD_API)
    #if defined(LD_EXPORT)
        #if defined(LD_PLATFORM_WINDOWS)
            /// Import/Export function
            #define LD_API __declspec(dllexport)
        #else
            /// Import/Export function
            #define LD_API __attribute__((visibility("default")))
        #endif
    #else
        #if defined(LD_PLATFORM_WINDOWS)
            /// Import/Export function
            #define LD_API __declspec(dllimport) c_linkage
        #else
            /// Import/Export function
            #define LD_API c_linkage
        #endif
    #endif
#endif

#if defined(LD_COMPILER_MSVC)
    #error "MSVC compiler is not supported!"
#endif

#if defined(LD_EXPORT)
    /// Internal definition
    #define LD_API_INTERNAL
#endif

// assert that type sizes are correct
static_assert(sizeof(u8)  == 1, "Expected u8 to be 1 byte!");
static_assert(sizeof(u16) == 2, "Expected u16 to be 2 bytes!");
static_assert(sizeof(u32) == 4, "Expected u32 to be 4 bytes!");
static_assert(sizeof(u64) == 8, "Expected u64 to be 8 bytes!");
static_assert(sizeof(i8)  == 1, "Expected i8 to be 1 byte!");
static_assert(sizeof(i16) == 2, "Expected i16 to be 2 bytes!");
static_assert(sizeof(i32) == 4, "Expected i32 to be 4 bytes!");
static_assert(sizeof(i64) == 8, "Expected i64 to be 8 bytes!");
static_assert(sizeof(f32) == 4, "Expected f32 to be 4 bytes!");
static_assert(sizeof(f64) == 8, "Expected f64 to be 8 bytes!");
static_assert(sizeof(c8)  == 1, "Expected c8 to be 1 byte!" );
static_assert(sizeof(c16) == 2, "Expected c16 to be 2 bytes!" );
static_assert(sizeof(c32) == 4, "Expected c32 to be 4 bytes!" );

#if defined(LD_ARCH_32_BIT)
    static_assert(sizeof(isize) == sizeof(i32), "Expected to be running on 32 bit architecture!");
    static_assert(sizeof(usize) == sizeof(u32), "Expected to be running on 32 bit architecture!");
#elif defined(LD_ARCH_64_BIT)
    static_assert(sizeof(isize) == sizeof(i64), "Expected to be running on 64 bit architecture!");
    static_assert(sizeof(usize) == sizeof(u64), "Expected to be running on 64 bit architecture!");
#endif // if arch64/32

#endif
