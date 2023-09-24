// * Description:  DLL main, only really required on windows but still
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 23, 2023

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

#define unused(x) (void)((x))

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(LD_PLATFORM_WINDOWS)
#include <windows.h>

BOOL WINAPI DllMainCRTStartup(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
) {
    unused(instance);
    unused(reserved);
    switch( reason ) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

#endif

#include <stdint.h>

size_t strlen( const char* str ) {
    const char* start = str;
    while( *str ) {
        str++;
    }
    return str - start;
}

void* memcpy( void* dst, const void* src, size_t size ) {
    size_t count64 = size / sizeof(uint64_t);
    for( size_t i = 0; i < count64; ++i ) {
        *((uint64_t*)dst + i) = *((uint64_t*)src + i);
    }

    size_t remainder = size % sizeof(uint64_t);
    uint8_t* src_remainder = (uint8_t*)((uint64_t*)src + count64);
    uint8_t* dst_remainder = (uint8_t*)((uint64_t*)dst + count64);
    for( size_t i = 0; i < remainder; ++i ) {
        *(dst_remainder + i) = *(src_remainder + i);
    }
    return dst;
}
void* memmove( void* str1, const void* str2, size_t n ) {
    #define INTERMEDIATE_BUFFER_SIZE (256ULL)
    uint8_t buf[INTERMEDIATE_BUFFER_SIZE];
    void* intermediate_buffer = buf;

    if( n <= INTERMEDIATE_BUFFER_SIZE ) {
        memcpy( intermediate_buffer, str2, n );
        memcpy( str1, intermediate_buffer, n );
        return str1;
    }

    size_t iteration_count = n / INTERMEDIATE_BUFFER_SIZE;
    size_t remaining_bytes = n % INTERMEDIATE_BUFFER_SIZE;

    for( size_t i = 0; i < iteration_count; ++i ) {
        size_t offset = i * INTERMEDIATE_BUFFER_SIZE;
        memcpy(
            intermediate_buffer,
            (uint8_t*)str2 + offset,
            INTERMEDIATE_BUFFER_SIZE
        );
        memcpy(
            (uint8_t*)str1 + offset,
            intermediate_buffer,
            INTERMEDIATE_BUFFER_SIZE
        );
    }

    if( remaining_bytes ) {
        size_t offset = (iteration_count * INTERMEDIATE_BUFFER_SIZE);
        memcpy( intermediate_buffer, (uint8_t*)str2 + offset, remaining_bytes );
        memcpy( (uint8_t*)str1 + offset, intermediate_buffer, remaining_bytes );
    }

    return str1;
}
void* memset( void* ptr, int value, size_t num ) {
    uint8_t val  = *(uint8_t*)&value;
    uint8_t* dst = (uint8_t*)ptr;
    for( size_t i = 0; i < num; ++i ) {
        dst[i] = val;
    }
    return ptr;
}
char* strcpy( char* dest, const char* src ) {
    *dest++ = *src;
    while( *src++ ) {
        *dest++ = *src;
    }
    return dest;
}

#if defined(__cplusplus)
} // extern "C"
#endif

