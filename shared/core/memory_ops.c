/**
 * Description:  Low level memory operations implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 30, 2023
*/
#include "defines.h"
#include "memory_ops.h"

LD_API void memory_copy(
    void* restricted dst, const void* restricted src, usize size
) {
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
}
LD_API void memory_copy_overlapped( void* dst, const void* src, usize size ) {
    #define INTERMEDIATE_BUFFER_SIZE (256ULL)
    u8 buf[INTERMEDIATE_BUFFER_SIZE];
    void* intermediate_buffer = buf;

    if( size <= INTERMEDIATE_BUFFER_SIZE ) {
        memory_copy( intermediate_buffer, src, size );
        memory_copy( dst, intermediate_buffer, size );
        return;
    }

    usize iteration_count = size / INTERMEDIATE_BUFFER_SIZE;
    usize remaining_bytes = size % INTERMEDIATE_BUFFER_SIZE;

    for( usize i = 0; i < iteration_count; ++i ) {
        usize offset = i * INTERMEDIATE_BUFFER_SIZE;
        memory_copy(
            intermediate_buffer,
            (u8*)src + offset,
            INTERMEDIATE_BUFFER_SIZE
        );
        memory_copy(
            (u8*)dst + offset,
            intermediate_buffer,
            INTERMEDIATE_BUFFER_SIZE
        );
    }

    if( remaining_bytes ) {
        usize offset = (iteration_count * INTERMEDIATE_BUFFER_SIZE);
        memory_copy( intermediate_buffer, (u8*)src + offset, remaining_bytes );
        memory_copy( (u8*)dst + offset, intermediate_buffer, remaining_bytes );
    }
}
LD_API void memory_set( void* dst, u8 value, usize size ) {
    usize size64 = size / sizeof(u64);
    union { u8 bytes[sizeof(u64)]; u64 longlong; } value64 = {
        .bytes = { value, value, value, value, value, value, value, value } };
    for( usize i = 0; i < size64; ++i ) {
        *((u64*)dst + i) = value64.longlong;
    }

    usize remainder     = size % sizeof(u64);
    u8*   dst_remainder = (u8*)((u64*)dst + size64);
    for( usize i = 0; i < remainder; ++i ) {
        *(dst_remainder + i) = value;
    }
}
LD_API b32 memory_cmp( const void* a, const void* b, usize size ) {
    usize size64 = size / sizeof(u64);
    for( usize i = 0; i < size64; ++i ) {
        if( *((u64*)a + i) != *((u64*)b + i) ) {
            return false;
        }
    }

    usize remainder = size % sizeof(u64);
    u8* a_remainder = (u8*)((u64*)a + size64);
    u8* b_remainder = (u8*)((u64*)b + size64);
    for( usize i = 0; i < remainder; ++i ) {
        if( *(a_remainder + i) != *(b_remainder + i) ) {
            return false;
        }
    }

    return true;
}

