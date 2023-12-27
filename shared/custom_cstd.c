/**
 * Description:  Custom C-standard library function implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 20, 2023
*/
#include "defines.h"
#include "custom_cstd.h"

// TODO(alicia): SIMD?

c_linkage usize strlen( const char* str ) {
    if( !str ) {
        return 0;
    }
    const char* start = str;
    while( *str ) {
        str++;
    }
    return str - start;
}
c_linkage void* memcpy(
    void* restricted dst, const void* restricted src, usize size
) {
    if( !size ) {
        return dst;
    }

    usize count64 = size / sizeof(u64);
    for( usize i = 0; i < count64; ++i ) {
        *((u64*)dst + i) = *((u64*)src + i);
    }

    usize remainder = size % sizeof(u64);
    u8* src_remainder = (u8*)((u64*)src + count64);
    u8* dst_remainder = (u8*)((u64*)dst + count64);
    for( usize i = 0; i < remainder; ++i ) {
        *(dst_remainder + i) = *(src_remainder + i);
    }

    return dst;
}
c_linkage void* memmove(
    void* str1, const void* str2, usize n
) {
    #define INTERMEDIATE_BUFFER_SIZE (256ULL)
    u8 buf[INTERMEDIATE_BUFFER_SIZE];
    void* intermediate_buffer = buf;

    if( n <= INTERMEDIATE_BUFFER_SIZE ) {
        memcpy( intermediate_buffer, str2, n );
        memcpy( str1, intermediate_buffer, n );
        return str1;
    }

    usize iteration_count = n / INTERMEDIATE_BUFFER_SIZE;
    usize remaining_bytes = n % INTERMEDIATE_BUFFER_SIZE;

    for( usize i = 0; i < iteration_count; ++i ) {
        usize offset = i * INTERMEDIATE_BUFFER_SIZE;
        memcpy(
            intermediate_buffer,
            (u8*)str2 + offset,
            INTERMEDIATE_BUFFER_SIZE
        );
        memcpy(
            (u8*)str1 + offset,
            intermediate_buffer,
            INTERMEDIATE_BUFFER_SIZE
        );
    }

    if( remaining_bytes ) {
        usize offset = (iteration_count * INTERMEDIATE_BUFFER_SIZE);
        memcpy( intermediate_buffer, (u8*)str2 + offset, remaining_bytes );
        memcpy( (u8*)str1 + offset, intermediate_buffer, remaining_bytes );
    }

    return str1;
}
c_linkage void* memset(
    void* dst, int value, usize size
) {
    u8 value8;
    if( value < 0 ) {
        i8 signed_value8 = value;
        value8 = *(u8*)&signed_value8;
    } else {
        value8 = (u8)value;
    }

    usize size64 = size / sizeof(u64);
    union { u8 bytes[sizeof(u64)]; u64 longlong; } value64 = {
        .bytes = { value8, value8, value8, value8, value8, value8, value8, value8 } };
    for( usize i = 0; i < size64; ++i ) {
        *((u64*)dst + i) = value64.longlong;
    }

    usize remainder     = size % sizeof(u64);
    u8*   dst_remainder = (u8*)((u64*)dst + size64);
    for( usize i = 0; i < remainder; ++i ) {
        *(dst_remainder + i) = value8;
    }

    return dst;
}
c_linkage char* strcpy(
    char* restricted dest, const char* restricted src
) {
    char* dst = dest;
    *dst++ = *src;
    while( *src++ ) {
        *dst++ = *src;
    }
    return dest;
}

