#if !defined(MEMORY_HPP)
#define MEMORY_HPP
/**
 * Description:  Dynamic Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

/// Pointer to memory + size of that memory block
struct MemoryBlock {
    void* pointer;
    usize size;
};

/// Allocate memory on the heap.
/// All platforms must zero out memory before returning pointer.
SM_API MemoryBlock heap_alloc( usize size );
/// Reallocate memory on the heap.
/// All platforms must zero out new memory before returning pointer.
SM_API b32 heap_realloc( MemoryBlock* memory, usize new_size );
/// Free heap allocated memory.
SM_API void heap_free( MemoryBlock* memory );

/// Page allocate memory.
/// All platforms must zero out memory before returning pointer.
SM_API MemoryBlock page_alloc( usize size );
/// Free page allocated memory.
SM_API void page_free( MemoryBlock* memory );

/// Copy memory from source pointer to destination pointer.
SM_API void mem_copy( void* dst, const void* src, usize size );
/// Zero out memory.
SM_API void mem_zero( void* ptr, usize size );

/// Get how many bytes are currently allocated
SM_API usize query_heap_memory_usage();

#define KILOBYTES( num ) ( num * 1024ULL )
#define MEGABYTES( num ) ( KILOBYTES( num ) * 1024ULL )
#define GIGABYTES( num ) ( MEGABYTES( num ) * 1024ULL )

#endif