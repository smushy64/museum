/*
 * Description:  Memory Functions Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 27, 2023
*/
#include "shared/defines.h"
#include "core/memory.h"
#include "core/internal/platform.h"
#include "core/internal/logging.h"
#include "shared/custom_cstd.h"

#define LOG_MEMORY_SUCCESS( title, format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_MEMORY_SUCCESS,\
        sizeof("[CORE] " "[" title " | {cc}:{u} > {cc}()] " format),\
        "[CORE] " "[" title " | {cc}:{u} > {cc}()] " format,\
        file, line, function, ##__VA_ARGS__\
    )

#define LOG_MEMORY_ERROR( title, format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_MEMORY_ERROR,\
        sizeof("[" title " | {cc}:{u} > {cc}()] " format),\
        "[" title " | {cc}:{u} > {cc}()] " format,\
        file, line, function, ##__VA_ARGS__\
    )

internal force_inline usize ___aligned_size( usize size, usize alignment ) {
    assert( alignment % 2 == 0 );
    return size + sizeof(void*) + ( alignment - 1 );
}
internal force_inline void* ___set_aligned_pointer( void* memory, usize alignment ) {
    void* result = (void**)
        ((usize)( (usize)memory + (alignment - 1) + sizeof(void*)) & ~(alignment - 1));
    ((void**) result)[-1] = memory;
    return result;
}
internal force_inline void* ___get_aligned_pointer( void* memory ) {
    return ((void**)memory)[-1];
}

CORE_API usize block_allocator_memory_requirement(
    usize block_count, usize block_size
) {
    usize allocator_size = sizeof( BlockAllocator );
    usize buffer_size    = block_count * block_size;
    usize free_list_size = block_count;

    return allocator_size + buffer_size + free_list_size;
}
CORE_API BlockAllocator* block_allocator_create(
    usize block_count, usize block_size, void* buffer
) {
    BlockAllocator* result = buffer;

    result->block_size  = block_size;
    result->block_count = block_count;

    usize allocator_memory = sizeof(BlockAllocator);
    allocator_memory      += block_count;

    result->buffer = ((u8*)buffer) + allocator_memory;

    return result;
}

internal b32 block_allocator_find_free_blocks(
    BlockAllocator* allocator, usize block_count, usize* out_head
) {
    b32   free_block_found = false;
    usize free_block_head  = 0;
    usize free_block_count = 0;
    for( usize i = 0; i < allocator->block_count; ++i ) {
        if( !allocator->free_list[i] ) {
            free_block_count++;
            if( !free_block_found ) {
                free_block_found = true;
                free_block_head  = i;
            }
        } else {
            free_block_found = false;
            free_block_count = 0;
        }

        if( free_block_count == block_count ) {
            break;
        }
    }

    if( free_block_found ) {
        *out_head = free_block_head;
        return true;
    }

    return false;
}
internal force_inline
usize ___memory_size_to_blocks( usize block_size, usize memory_size ) {
    usize block_count = memory_size / block_size;
    block_count += ( memory_size % block_size ) ? 1 : 0;
    return block_count;
}
CORE_API void* block_allocator_alloc( BlockAllocator* allocator, usize size ) {
    usize block_count = ___memory_size_to_blocks( allocator->block_size, size );

    usize head = 0;
    if( block_allocator_find_free_blocks( allocator, block_count, &head ) ) {
        memory_set( allocator->free_list + head, 1, block_count );
        return (u8*)allocator->buffer + ( head * allocator->block_size );
    }

    return NULL;
}
CORE_API void* block_allocator_alloc_aligned(
    BlockAllocator* allocator, usize size, usize alignment
) {
    usize aligned_size = ___aligned_size( size, alignment );

    void* memory = block_allocator_alloc( allocator, aligned_size );
    if( memory ) {
        void* result = ___set_aligned_pointer( memory, alignment );
        return result;
    }
    return memory;
}
CORE_API void* block_allocator_realloc(
    BlockAllocator* allocator, void* memory, usize old_size, usize new_size
) {
    assert( new_size > old_size );

    usize remainder_of_last_block = old_size % allocator->block_size;

    if( old_size + remainder_of_last_block < new_size ) {
        // currently allocated blocks have enough space
        // to hold new size 
        return memory;
    }

    usize old_block_count = ___memory_size_to_blocks( allocator->block_size, old_size );
    usize new_block_count = ___memory_size_to_blocks( allocator->block_size, new_size );

    usize additional_blocks_required = new_block_count - old_block_count;
    usize head = ((usize)allocator->buffer - (usize)memory) / allocator->block_size;

    b32 adjacent_blocks_are_free = true;
    for( usize i = 0; i < additional_blocks_required; ++i ) {
        u8 current_block_freed = !allocator->free_list[head + old_block_count + i];
        if( !current_block_freed ) {
            adjacent_blocks_are_free = false;
            break;
        }
    }

    if( adjacent_blocks_are_free ) {
        // set adjacent blocks to be freed
        memory_set(
            allocator->free_list + head + old_block_count,
            1, additional_blocks_required );
        return memory;
    }

    void* new_pointer = block_allocator_alloc( allocator, new_size );
    if( !new_pointer ) {
        // could not find any free space that can accomodate
        return NULL;
    }

    // allocate new space, copy data to new space and free old space
    memory_copy( new_pointer, memory, old_size );
    block_allocator_free( allocator, memory, old_size );
    return new_pointer;
}
CORE_API void block_allocator_free(
    BlockAllocator* allocator, void* memory, usize size
) {
    usize block_count = ___memory_size_to_blocks( allocator->block_size, size );

    usize head = ((usize)memory - (usize)allocator->buffer) / allocator->block_size;

    memory_zero( memory, size );
    memory_zero( allocator->free_list + head, block_count );
}
CORE_API void block_allocator_free_aligned(
    BlockAllocator* allocator, void* memory, usize size, usize alignment
) {
    usize aligned_size = ___aligned_size( size, alignment );
    block_allocator_free( allocator, ___get_aligned_pointer( memory ), aligned_size );
}
CORE_API void block_allocator_clear( BlockAllocator* allocator ) {
    memory_zero( allocator->free_list, allocator->block_count );
    memory_zero( allocator->buffer, allocator->block_size * allocator->block_count );
}

CORE_API StackAllocator stack_allocator_create( usize buffer_size, void* buffer ) {
    StackAllocator result = {0};
    result.buffer      = buffer;
    result.buffer_size = buffer_size;
    return result;
}
CORE_API void* stack_allocator_push( StackAllocator* allocator, usize size ) {
    if( (allocator->current + size) > allocator->buffer_size ) {
        return NULL;
    }
    void* result = (u8*)allocator->buffer + allocator->current;
    allocator->current += size;

    return result;
}
CORE_API void* stack_allocator_push_aligned(
    StackAllocator* allocator, usize size, usize alignment
) {
    usize aligned_size = ___aligned_size( size, alignment );
    void* memory = stack_allocator_push( allocator, aligned_size );
    if( memory ) {
        void* result = ___set_aligned_pointer( memory, alignment );
        return result;
    }

    return memory;
}
CORE_API b32 stack_allocator_pop( StackAllocator* allocator, usize size ) {
    if( size > allocator->current ) {
        return false;
    }
    allocator->current -= size;
    memory_zero( (u8*)allocator->buffer + allocator->current, size );
    return true;
}
CORE_API b32 stack_allocator_pop_aligned(
    StackAllocator* allocator, usize size, usize alignment
) {
    usize aligned_size = ___aligned_size( size, alignment );
    return stack_allocator_pop( allocator, aligned_size );
}
CORE_API void stack_allocator_clear( StackAllocator* allocator ) {
    allocator->current = 0;
    memory_zero( allocator->buffer, allocator->buffer_size );
}

CORE_API usize memory_size_to_page_count( usize size ) {
    usize page_size = platform_query_page_size();
    usize result = size / page_size;
    result += ( size % page_size ) ? 1 : 0;
    return result;
}
CORE_API usize page_count_to_memory_size( usize pages ) {
    usize page_size = platform_query_page_size();
    return pages * page_size;
}

global usize HEAP_MEMORY_USAGE = 0;
global usize PAGE_MEMORY_USAGE = 0;

CORE_API usize memory_query_heap_usage(void) {
    return HEAP_MEMORY_USAGE;
}
CORE_API usize memory_query_page_usage(void) {
    return PAGE_MEMORY_USAGE;
}
CORE_API usize memory_query_total_usage(void) {
    return HEAP_MEMORY_USAGE + (page_count_to_memory_size( PAGE_MEMORY_USAGE ));
}

CORE_API void* ___internal_system_page_alloc( usize pages ) {
    void* result = platform_heap_alloc( page_count_to_memory_size( pages ) );

    if( result ) {
        PAGE_MEMORY_USAGE += pages;
    }
    return result;
}
CORE_API void  ___internal_system_page_free( void* memory, usize pages ) {
    PAGE_MEMORY_USAGE -= pages;
    platform_heap_free( memory, page_count_to_memory_size( pages ) );
}

CORE_API void* ___internal_system_page_alloc_trace(
    usize pages, const char* function, const char* file, int line
) {
    usize memory_size = page_count_to_memory_size( pages );
    void* result = platform_heap_alloc( memory_size );

    if( result ) {
        LOG_MEMORY_SUCCESS(
            "PAGE", "Allocated {f,m,.2}. Pointer: {usize,X}",
            (f64)memory_size, (usize)result );
        PAGE_MEMORY_USAGE += pages;
    } else {
        LOG_MEMORY_ERROR(
            "PAGE", "Failed to allocate {f,m,.2}!",
            (f64)memory_size );
    }

    return result;
}
CORE_API void  ___internal_system_page_free_trace(
    void* memory, usize pages, const char* function, const char* file, int line
) {
    usize memory_size = page_count_to_memory_size( pages );
    LOG_MEMORY_SUCCESS(
        "PAGE", "Freed {f,m,.2}. Pointer: {usize,X}",
        (f64)memory_size, (usize)memory );
    PAGE_MEMORY_USAGE -= pages;
    // TODO(alicia): VirtualAlloc/mmap
    platform_heap_free( memory, memory_size );
}

CORE_API void* ___internal_system_alloc( usize size ) {
    void* result = platform_heap_alloc( size );
    if( result ) {
        HEAP_MEMORY_USAGE += size;
    }
    return result;
}
CORE_API void* ___internal_system_alloc_aligned( usize size, usize alignment ) {
    usize aligned_size = ___aligned_size( size, alignment );

    void* memory = ___internal_system_alloc( aligned_size );
    if( memory ) {
        void* result = ___set_aligned_pointer( memory, alignment );
        return result;
    }
    return memory;
}
CORE_API void* ___internal_system_realloc(
    void* memory, usize old_size, usize new_size
) {
    void* result = platform_heap_realloc( memory, old_size, new_size );
    if( result ) {
        HEAP_MEMORY_USAGE -= old_size;
        HEAP_MEMORY_USAGE += new_size;
    }
    return result;
}
CORE_API void ___internal_system_free( void* memory, usize size ) {
    HEAP_MEMORY_USAGE -= size;
    platform_heap_free( memory, size );
}
CORE_API void ___internal_system_free_aligned(
    void* memory, usize size, usize alignment
) {
    usize aligned_size = ___aligned_size( size, alignment );
    HEAP_MEMORY_USAGE -= aligned_size;
    platform_heap_free( ___get_aligned_pointer( memory ), aligned_size );
}

CORE_API void* ___internal_system_alloc_trace(
    usize size, const char* function, const char* file, int line
) {
    void* result = platform_heap_alloc( size );
    if( result ) {
        f64 sizef = (f64)size;
        LOG_MEMORY_SUCCESS(
            "HEAP", "Allocated {f,m,.2}. Pointer: {usize,X}",
            sizef, (usize)result );
        HEAP_MEMORY_USAGE += size;
    } else {
        LOG_MEMORY_ERROR( "HEAP", "Failed to allocate {f,m,.2}!", (f64)size );
    }

    return result;
}
CORE_API void* ___internal_system_alloc_aligned_trace(
    usize size, usize alignment, const char* function, const char* file, int line
) {
    void* result = ___internal_system_alloc_aligned( size, alignment );
    if( result ) {
        LOG_MEMORY_SUCCESS(
            "HEAP", "Allocated {f,m,.2}. Alignment: {usize} Pointer: {usize,X}",
            (f64)size, alignment, (usize)result );
    } else {
        LOG_MEMORY_ERROR(
            "HEAP", "Failed to allocate {f,m,.2}! Alignment: {usize}",
            (f64)size, alignment );
    }
    return result;
}
CORE_API void* ___internal_system_realloc_trace(
    void* memory, usize old_size, usize new_size,
    const char* function, const char* file, int line
) {
    void* result = platform_heap_realloc( memory, old_size, new_size );
    if( result ) {
        LOG_MEMORY_SUCCESS(
            "HEAP", "Reallocated {usize,X}. {f,m,.2} -> {f,m,.2}",
            (usize)memory, (f64)old_size, (f64)new_size );
        HEAP_MEMORY_USAGE -= old_size;
        HEAP_MEMORY_USAGE += new_size;
    } else {
        LOG_MEMORY_SUCCESS(
            "HEAP", "Failed to reallocate {usize,X}! {f,m,.2} -> {f,m,.2}",
            (usize)memory, (f64)old_size, (f64)new_size );
    }
    return result;
}
CORE_API void ___internal_system_free_trace(
    void* memory, usize size, const char* function, const char* file, int line
) {
    platform_heap_free( memory, size );
    LOG_MEMORY_SUCCESS(
        "HEAP", "Freed {f,m,.2}. Pointer: {usize,X}",
        (f64)size, (usize)memory );
    HEAP_MEMORY_USAGE -= size;
}
CORE_API void ___internal_system_free_aligned_trace(
    void* memory, usize size, usize alignment,
    const char* function, const char* file, int line
) {
    ___internal_system_free_aligned( memory, size, alignment );
    LOG_MEMORY_SUCCESS(
        "HEAP", "Freed {f,m,.2}. Alignment: {usize} Pointer: {usize,X}",
        (f64)size, alignment, (usize)memory );
}

// NOTE(alicia): Using C-standard library replacement functions
// found in shared/custom_cstd.c not actual C-standard library.

CORE_API void memory_copy(
    void* restricted dst, const void* restricted src, usize size
) {
    (void)memcpy( dst, src, size );
}
CORE_API void memory_copy_overlapped( void* dst, const void* src, usize size ) {
    (void)memmove( dst, src, size );
}
CORE_API void memory_set( void* dst, u8 value, usize size ) {
    (void)memset( dst, value, size );
}
CORE_API b32 memory_cmp( const void* a, const void* b, usize size ) {
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

