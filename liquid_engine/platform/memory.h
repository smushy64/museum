#if !defined(MEMORY_HPP)
#define MEMORY_HPP
/**
 * Description:  Dynamic Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
 * Notes:        define "LD_PROFILING" to keep track of
 *                 how much memory is allocated and freed
 *                 everytime heap_alloc/realloc/free is called
*/
#include "defines.h"

/// Allocate memory on the heap.
/// All platforms must zero out memory before returning pointer.
SM_API void* heap_alloc( usize size );
/// Reallocate memory on the heap.
/// All platforms must zero out new memory before returning pointer.
SM_API void* heap_realloc( void* memory, usize new_size );
/// Free heap allocated memory.
SM_API void heap_free( void* memory );

/// Page allocate memory.
/// All platforms must zero out memory before returning pointer.
SM_API void* page_alloc( usize size );
/// Free page allocated memory.
SM_API void page_free( void* memory );

/// Copy memory from source pointer to destination pointer.
SM_API void mem_copy( void* dst, const void* src, usize size );
/// Zero out memory.
SM_API void mem_zero( void* ptr, usize size );

/// Query how many bytes are currently allocated on the heap.
SM_API usize query_heap_memory_usage();
/// Query how many bytes are currently page allocated.
SM_API usize query_page_memory_usage();

#define KILOBYTES( num ) ( num * 1024ULL )
#define MEGABYTES( num ) ( KILOBYTES( num ) * 1024ULL )
#define GIGABYTES( num ) ( MEGABYTES( num ) * 1024ULL )

#endif