#if !defined(MEMORY_HPP)
#define MEMORY_HPP
/**
 * Description:  Dynamic Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

/// Allocate memory on the heap.
/// All platforms must zero out memory before returning pointer.
SM_API void* heap_alloc( usize size );
/// Reallocate memory on the heap.
/// All platforms must zero out new memory before returning pointer.
SM_API void* heap_realloc( void* ptr, usize size );
/// Free heap allocated memory.
SM_API void heap_free( void* ptr );

/// Page allocate memory.
/// All platforms must zero out memory before returning pointer.
SM_API void* page_alloc( usize size );
/// Free page allocated memory.
SM_API void page_free( void* ptr );

/// Copy memory from source pointer to destination pointer.
SM_API void mem_copy( void* dst, const void* src, usize size );
/// Zero out memory.
SM_API void mem_zero( void* ptr, usize size );

#endif