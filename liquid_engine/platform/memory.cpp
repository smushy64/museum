/**
 * Description:  Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "memory.h"

SM_API void mem_copy(
    void* dst,
    const void* src,
    usize size
) {
    if( size % sizeof(u64) == 0 ) {
        u64* src_64   = (u64*)src;
        u64* dst_64   = (u64*)dst;
        usize size_64 = size / sizeof(u64);
        for( usize i = 0; i < size_64; ++i ) {
            dst_64[i] = src_64[i];
        }
    } else if( size % sizeof(u32) == 0 ) {
        u32* src_32   = (u32*)src;
        u32* dst_32   = (u32*)dst;
        usize size_32 = size / sizeof(u32);
        for( usize i = 0; i < size_32; ++i ) {
            dst_32[i] = src_32[i];
        }
    } else if( size % sizeof(u16) == 0 ) {
        u16* src_16   = (u16*)src;
        u16* dst_16   = (u16*)dst;
        usize size_16 = size / sizeof(u16);
        for( usize i = 0; i < size_16; ++i ) {
            dst_16[i] = src_16[i];
        }
    } else {
        u8* src_8   = (u8*)src;
        u8* dst_8   = (u8*)dst;
        for( usize i = 0; i < size; ++i ) {
            dst_8[i] = src_8[i];
        }
    }
}

SM_API void mem_zero( void* ptr, usize size ) {
    if( size % sizeof(u64) == 0 ) {
        u64* ptr_64   = (u64*)ptr;
        usize size_64 = size / sizeof(u64);
        for( usize i = 0; i < size_64; ++i ) {
            ptr_64[i] = 0ULL;
        }
    } else if( size % sizeof(u32) == 0 ) {
        u32* ptr_32   = (u32*)ptr;
        usize size_32 = size / sizeof(u32);
        for( usize i = 0; i < size_32; ++i ) {
            ptr_32[i] = 0;
        }
    } else if( size % sizeof(u16) == 0 ) {
        u16* ptr_16   = (u16*)ptr;
        usize size_16 = size / sizeof(u16);
        for( usize i = 0; i < size_16; ++i ) {
            ptr_16[i] = 0;
        }
    } else {
        u8* ptr_8 = (u8*)ptr;
        for( usize i = 0; i < size; ++i ) {
            ptr_8[i] = 0;
        }
    }
}
