#if !defined(LD_SHARED_CORE_MEMORY_H)
#define LD_SHARED_CORE_MEMORY_H
/**
 * Description:  Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
 * Notes:        define LD_MEMORY_NO_LOG before including to disable logging allocations
*/
#include "shared/defines.h"

/// Slice of bytes.
struct ByteSlice {
    u8*   buffer;
    usize len;
    usize capacity;
};
/// Slice of bytes.
typedef struct ByteSlice ByteSlice;

/// Create empty byte slice.
header_only ByteSlice byte_slice( usize capacity, u8* bytes ) {
    ByteSlice result = {};
    result.buffer   = bytes;
    result.capacity = capacity;
    return result;
}
/// Push a byte into byte slice.
header_only b32 byte_slice_push( ByteSlice* slice, u8 byte ) {
    if( slice->len >= slice->capacity ) {
        return false;
    }
    slice->buffer[slice->len++] = byte;
    return true;
}

/// Stack allocator.
struct StackAllocator {
    void* buffer;
    usize current;
    usize buffer_size;
};
/// Stack allocator.
typedef struct StackAllocator StackAllocator;

/// Fixed-size block allocator.
struct BlockAllocator {
    void* buffer;
    usize block_size;
    usize block_count;
    u8    free_list[];
};
/// Fixed-size block allocator.
typedef struct BlockAllocator BlockAllocator;

/// Calculate how many bytes are required for block allocator.
CORE_API usize block_allocator_memory_requirement( usize block_count, usize block_size );
/// Create a block allocator.
/// Buffer must be able to hold result from block_allocator_memory_requirement()
CORE_API BlockAllocator* block_allocator_create(
    usize block_count, usize block_size, void* buffer );
/// Allocate memory from block allocator.
CORE_API void* block_allocator_alloc( BlockAllocator* allocator, usize size );
/// Allocate aligned memory from block allocator.
CORE_API void* block_allocator_alloc_aligned(
    BlockAllocator* allocator, usize size, usize alignment );
/// Reallocate memory from block allocator.
CORE_API void* block_allocator_realloc(
    BlockAllocator* allocator, void* memory, usize old_size, usize new_size );
/// Free memory from block allocator.
CORE_API void block_allocator_free( BlockAllocator* allocator, void* memory, usize size );
/// Free aligned memory from block allocator.
CORE_API void block_allocator_free_aligned(
    BlockAllocator* allocator, void* memory, usize size, usize alignment );
/// Clears free list and zeroes out buffer.
CORE_API void block_allocator_clear( BlockAllocator* allocator );

/// Create a stack allocator.
CORE_API StackAllocator stack_allocator_create( usize buffer_size, void* buffer );
/// Push item onto stack.
/// Returns NULL if item is too large to fit in allocator.
CORE_API void* stack_allocator_push( StackAllocator* allocator, usize size );
/// Push item onto stack.
/// Pointer returned is aligned to given alignment
/// Returns NULL if item is too large to fit in allocator.
CORE_API void* stack_allocator_push_aligned(
    StackAllocator* allocator, usize size, usize alignment );
/// Pop item from stack.
CORE_API b32 stack_allocator_pop( StackAllocator* allocator, usize size );
/// Pop aligned item from stack.
CORE_API b32 stack_allocator_pop_aligned(
    StackAllocator* allocator, usize size, usize alignment );
/// Resets current pointer and zeroes out entire buffer.
CORE_API void stack_allocator_clear( StackAllocator* allocator );
/// Calculate remaining space in stack allocator.
header_only usize stack_allocator_remaining_memory( StackAllocator* allocator ) {
    return allocator->buffer_size - allocator->current;
}

/// Query how many bytes have been allocated from system heap.
CORE_API usize memory_query_heap_usage(void);
/// Query how many pages have been allocated from system.
CORE_API usize memory_query_page_usage(void);
/// Query total memory usage.
CORE_API usize memory_query_total_usage(void);

/// Calculate how many system pages are required for given size.
CORE_API usize memory_size_to_page_count( usize size );
/// Calculate memory size of pages.
CORE_API usize page_count_to_memory_size( usize pages );

/// Allocate memory from system allocator by pages.
/// Only use this if you're allocating a large amount of memory.
/// This memory should only be freed with the corresponding page free function.
CORE_API void* ___internal_system_page_alloc( usize pages );
/// Free memory allocated with page_alloc.
CORE_API void  ___internal_system_page_free( void* memory, usize pages );

/// Allocate memory from system allocator by pages.
/// Only use this if you're allocating a large amount of memory.
/// This memory should only be freed with the corresponding page free function.
CORE_API void* ___internal_system_page_alloc_trace(
    usize pages, const char* function, const char* file, int line );
/// Free memory allocated with page_alloc.
CORE_API void  ___internal_system_page_free_trace(
    void* memory, usize pages, const char* function, const char* file, int line );

/// Allocate memory from system allocator.
CORE_API void* ___internal_system_alloc( usize size );
/// Allocate aligned memory from system allocator.
/// Must be freed with system_free_aligned!
CORE_API void* ___internal_system_alloc_aligned( usize size, usize alignment );
/// Reallocate memory from system allocator.
CORE_API void* ___internal_system_realloc( void* memory, usize old_size, usize new_size );
/// Free allocated memory from system allocator.
CORE_API void ___internal_system_free( void* memory, usize size );
/// Free aligned allocated memory from system allocator.
CORE_API void ___internal_system_free_aligned( void* memory, usize size, usize alignment );

/// Allocate memory from system allocator.
CORE_API void* ___internal_system_alloc_trace(
    usize size, const char* function, const char* file, int line );
/// Allocate aligned memory from system allocator.
/// Must be freed with system_free_aligned!
CORE_API void* ___internal_system_alloc_aligned_trace(
    usize size, usize alignment, const char* function, const char* file, int line );
/// Reallocate memory from system allocator.
CORE_API void* ___internal_system_realloc_trace(
    void* memory, usize old_size, usize new_size,
    const char* function, const char* file, int line );
/// Free allocated memory from system allocator.
CORE_API void ___internal_system_free_trace(
    void* memory, usize size, const char* function, const char* file, int line );
/// Free aligned allocated memory from system allocator.
CORE_API void ___internal_system_free_aligned_trace(
    void* memory, usize size, usize alignment,
    const char* function, const char* file, int line );

#if defined(LD_LOGGING) && !defined( LD_MEMORY_NO_LOG )
    #define system_alloc( size )\
        ___internal_system_alloc_trace( size, __FUNCTION__, __FILE__, __LINE__ )
    #define system_alloc_aligned( size, alignment )\
        ___internal_system_alloc_aligned_trace(\
            size, alignment, __FUNCTION__, __FILE__, __LINE__ )
    #define system_realloc( memory, old_size, new_size )\
        ___internal_system_realloc_trace(\
            memory, old_size, new_size, __FUNCTION__, __FILE__, __LINE__ )
    #define system_free( memory, size )\
        ___internal_system_free_trace(\
            memory, size, __FUNCTION__, __FILE__, __LINE__ )
    #define system_free_aligned( memory, size, alignment )\
        ___internal_system_free_aligned_trace(\
            memory, size, alignment, __FUNCTION__, __FILE__, __LINE__ )
    #define system_page_alloc( pages )\
        ___internal_system_page_alloc_trace( pages, __FUNCTION__, __FILE__, __LINE__ )
    #define system_page_free( memory, pages )\
        ___internal_system_page_free_trace(\
            memory, pages, __FUNCTION__, __FILE__, __LINE__ )
#else
    #define system_alloc( size )\
        ___internal_system_alloc( size )
    #define system_alloc_aligned( size, alignment )\
        ___internal_system_alloc_aligned( size, alignment )
    #define system_realloc( memory, old_size, new_size )\
        ___internal_system_realloc( memory, old_size, new_size )
    #define system_free( memory, size )\
        ___internal_system_free( memory, size )
    #define system_free_aligned( memory, size, alignment )\
        ___internal_system_free_aligned( memory, size, alignment )
    #define system_page_alloc( pages )\
        ___internal_system_page_alloc( pages )
    #define system_page_free( memory, pages )\
        ___internal_system_page_free( memory, pages )
#endif

/// Copy from source buffer to destination buffer.
CORE_API void memory_copy( void* restricted dst, const void* restricted src, usize size );
/// Copy from source buffer to destination buffer.
/// Only use this if source and destination overlap.
CORE_API void memory_copy_overlapped( void* dst, const void* src, usize size );
/// Set bytes in destination to given value.
CORE_API void memory_set( void* dst, u8 value, usize size );
/// Set all bytes in destination to zero.
header_only void memory_zero( void* dst, usize size ) {
    memory_set( dst, 0, size );
}
/// Compare two equally sized buffers.
CORE_API b32 memory_cmp( const void* a, const void* b, usize size );

/// Align a pointer to a given alignment.
#define memory_align( ptr, alignment )\
    (void*)(( ((usize)(ptr)) - 1ull + (alignment) ) & -(alignment))

#endif /* header guard */
