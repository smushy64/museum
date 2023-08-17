#if !defined(LEPKG_DEFINES_HPP)
#define LEPKG_DEFINES_HPP
// * Description:  Defines for asset packer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 11, 2023
#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef uintptr_t usize;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef intptr_t isize;

typedef u8  b8;
typedef u32 b32;

typedef char    c8;
typedef wchar_t c16;
typedef u32     c32;

#define true  1
#define false 0

typedef float  f32;
typedef double f64;

typedef void* pvoid;

#define println( format, ... ) \
    printf( format "\n", ##__VA_ARGS__ )

#define printerrln( format, ... ) \
    fprintf( stderr, format "\n", ##__VA_ARGS__ )

/// compiler defines
#if defined(__GNUC__) || defined(__GNUG__)
    #define LEPKG_COMPILER_GCC
#endif

#if defined(__clang__)
    #if defined(LEPKG_COMPILER_GCC)
        #undef LEPKG_COMPILER_GCC
    #endif
    #define LEPKG_COMPILER_CLANG
#endif

#if defined(_MSC_VER)
    #if defined(LEPKG_COMPILER_GCC)
        #undef LEPKG_COMPILER_GCC
    #endif
    #if defined(LEPKG_COMPILER_CLANG)
        #undef LEPKG_COMPILER_CLANG
    #endif
    #define LEPKG_COMPILER_MSVC
#endif

#if !defined(LEPKG_COMPILER_GCC) && !defined(LEPKG_COMPILER_CLANG) && !defined(LEPKG_COMPILER_MSVC)
    #define LEPKG_COMPILER_UNKNOWN
#endif

// platform defines
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__WIN64__)
    #define LEPKG_PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__gnu_linux__)
    #define LEPKG_PLATFORM_LINUX
#elif defined(__ANDROID__)
    #define LEPKG_PLATFORM_ANDROID
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if defined(TARGET_IPHONE_SIMULATION) || defined(TARGET_OS_IPHONE)
        #define LEPKG_PLATFORM_IOS
    #elif defined(TARGET_OS_MAC)
        #define LEPKG_PLATFORM_MACOS
    #endif
#else
    #define LEPKG_PLATFORM_UNKNOWN
#endif

// platform cpu defines
#if defined(_M_IX86) || defined(__i386__)
    #define LEPKG_ARCH_X86
    #define LEPKG_ARCH_32_BIT
#endif

#if defined(__x86_64__) || defined(_M_X64_)
    #if !defined(LEPKG_ARCH_X86)
        #define LEPKG_ARCH_X86
    #endif
    #if defined(LEPKG_ARCH_32_BIT)
        #undef LEPKG_ARCH_32_BIT
    #endif
    #define LEPKG_ARCH_64_BIT
#endif // if x86

#if defined(__arm__) || defined(_M_ARM_)
    #define LEPKG_ARCH_ARM
    #define LEPKG_ARCH_32_BIT
#endif // if arm

#if defined(__aarch64__)
    #if !defined(LEPKG_ARCH_ARM)
        #define LEPKG_ARCH_ARM
    #endif
    #if defined(LEPKG_ARCH_32_BIT)
        #undef LEPKG_ARCH_32_BIT
    #endif
    #define LEPKG_ARCH_64_BIT
#endif // if arm64

/// Calculate number of elements in a static array
#define STATIC_ARRAY_COUNT( array ) \
    ( sizeof(array) / sizeof((array)[0]) )

/// Calculate byte size of a static array
#define STATIC_ARRAY_SIZE( array ) \
    (sizeof(array))

#if !defined(LEPKG_COMPILER_MSVC) // not MSVC
    
    #define PANIC() __builtin_trap()

    #if defined(LEPKG_COMPILER_GCC)
        #define NO_OPTIMIZE __attribute__((optimize("O0")))
    #else // clang
        #define NO_OPTIMIZE __attribute__((optnone))
    #endif

    #define STATIC_ASSERT _Static_assert
    #define HOT_PATH      __attribute__((hot))
    #define FORCE_INLINE  __attribute__((always_inline)) inline
    #define NO_INLINE     __attribute__((noinline))
    #define MAKE_PACKED( declaration ) declaration __attribute__((__packed__))

    #define unused(x) (void)((x))
    #define UNUSED_PARAM(x) x __attribute__((__unused__))

#endif

#define PACKED __attribute__((__packed__))

STATIC_ASSERT( sizeof(c8)  == 1, "Expected UTF-8 to be 1 byte!" );
STATIC_ASSERT( sizeof(c16) == 2, "Expected UTF-16 to be 2 bytes!" );
STATIC_ASSERT( sizeof(c32) == 4, "Expected UTF-32 to be 4 bytes!" );

#define internal static
#define local static
#define global static

#define ASSERT(condition) do {\
    if(!(condition)) {\
        PANIC();\
    }\
} while(0)

/// Swap two values.
#define SWAP( a, b ) do {\
    __typeof(a) intermediate = a;\
    a = b;\
    b = intermediate;\
} while(0)


#endif // header guard
