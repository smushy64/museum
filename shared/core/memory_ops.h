#if !defined(LD_SHARED_CORE_MEMORY_OPS_H)
#define LD_SHARED_CORE_MEMORY_OPS_H
/**
 * Description:  Low level memory operations
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 30, 2023
*/
#include "defines.h"

/// Copy from source buffer to destination buffer.
LD_API void memory_copy( void* restricted dst, const void* restricted src, usize size );
/// Copy from source buffer to destination buffer.
/// Only use this if source and destination overlap.
LD_API void memory_copy_overlapped( void* dst, const void* src, usize size );
/// Set bytes in destination to given value.
LD_API void memory_set( void* dst, u8 value, usize size );
/// Set all bytes in destination to zero.
header_only void memory_zero( void* dst, usize size ) {
    memory_set( dst, 0, size );
}
/// Compare two equally sized buffers.
LD_API b32 memory_cmp( const void* a, const void* b, usize size );

#endif /* header guard */
