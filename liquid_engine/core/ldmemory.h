#if !defined(CORE_MEMORY_HPP)
#define CORE_MEMORY_HPP
/**
 * Description:  
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
 * Notes:        define "LD_PROFILING" to keep track of
 *                 how much memory is allocated and freed
 *                 everytime alloc/realloc/free is called
*/
#include "defines.h"

// TODO(alicia): check to see if this needs to be changed for other platforms
#define MEMORY_PAGE_SIZE (KILOBYTES(4))

/// Types of memory allocations
typedef enum MemoryType : u8 {
    MEMORY_TYPE_UNKNOWN,
    MEMORY_TYPE_ENGINE,
    MEMORY_TYPE_DYNAMIC_LIST,
    MEMORY_TYPE_RENDERER,
    MEMORY_TYPE_STRING,
    MEMORY_TYPE_USER,

    MEMORY_TYPE_COUNT
} MemoryType;
/// Memory type to string
headerfn const char* memory_type_to_string( MemoryType type ) {
    const char* strings[MEMORY_TYPE_COUNT] = {
        "Unknown Memory",
        "Engine Memory",
        "Dynamic List Memory",
        "Renderer Memory",
        "String Memory",
        "User Memory"
    };
    if( type >= MEMORY_TYPE_COUNT ) {
        return strings[0];
    }
    return strings[type];
}

/// IMPORTANT(alicia): Internal use only!
/// Allocate memory.
LD_API void* internal_ldalloc( usize size, MemoryType type );
// IMPORTANT(alicia): Internal use only!
// Allocate aligned memory.
LD_API void* internal_ldalloc_aligned(
    usize size, MemoryType type, usize alignment );
// IMPORTANT(alicia): Internal use only!
// Reallocate memory.
LD_API void* internal_ldrealloc(
    void* memory, usize old_size, usize new_size, MemoryType type );
// IMPORTANT(alicia): Internal use only!
// Free allocated memory.
LD_API void internal_ldfree( void* memory, usize size, MemoryType type );
// IMPORTANT(alicia): Internal use only!
// Free allocated aligned memory.
LD_API void internal_ldfree_aligned(
    void* memory, usize size, MemoryType type, usize alignment );

/// IMPORTANT(alicia): Internal use only!
/// Allocate memory.
LD_API void* internal_ldalloc_trace(
    usize size, MemoryType type,
    const char* function,
    const char* file,
    int line
);
// IMPORTANT(alicia): Internal use only!
// Allocate aligned memory.
LD_API void* internal_ldalloc_aligned_trace(
    usize size, MemoryType type, u8 alignment,
    const char* function,
    const char* file,
    int line
);
// IMPORTANT(alicia): Internal use only!
// Reallocate memory.
LD_API void* internal_ldrealloc_trace(
    void* memory, usize old_size, usize new_size, MemoryType type,
    const char* function,
    const char* file,
    int line
);
// IMPORTANT(alicia): Internal use only!
// Free allocated memory.
LD_API void internal_ldfree_trace(
    void* memory, usize size, MemoryType type,
    const char* function,
    const char* file,
    int line
);
// IMPORTANT(alicia): Internal use only!
// Free allocated aligned memory.
LD_API void internal_ldfree_aligned_trace(
    void* memory, usize size, MemoryType type, usize alignment,
    const char* function,
    const char* file,
    int line
);

#if defined(LD_LOGGING)
    #define ldalloc( size, type )\
        internal_ldalloc_trace( size, type, __FUNCTION__, __FILE__, __LINE__ )
    #define ldalloc_aligned( size, type )\
        internal_ldalloc_aligned_trace(\
            size, type, __FUNCTION__, __FILE__, __LINE__ )
    #define ldrealloc( memory, old_size, new_size, type )\
        internal_ldrealloc_trace(\
            memory, old_size, new_size, type, __FUNCTION__, __FILE__, __LINE__ )
    #define ldfree( memory, size, type )\
        internal_ldfree_trace(\
            memory, size, type, __FUNCTION__, __FILE__, __LINE__ )
    #define ldfree_aligned( memory, size, type, alignment )\
        internal_ldfree_trace(\
            memory, size, type, alignment, __FUNCTION__, __FILE__, __LINE__ )
#else
    #define ldalloc( size, type ) internal_ldalloc( size, type )
    #define ldalloc_aligned( size, type )\
        internal_ldalloc_aligned( size, type )
    #define ldrealloc( memory, old_size, new_size, type )\
        internal_ldrealloc( memory, old_size, new_size, type )
    #define ldfree( memory, size, type )\
        internal_ldfree( memory, size, type )
    #define ldfree_aligned( memory, size, type, alignment )\
        internal_ldfree_aligned( memory, size, type, alignment )
#endif

/// Calculate number of pages required for bytes.
headerfn usize calculate_page_size( usize byte_size ) {
    usize required = byte_size / MEMORY_PAGE_SIZE;
    required += byte_size % MEMORY_PAGE_SIZE ? 1 : 0;
    return required;
}
// IMPORTANT(alicia): Internal use only!
/// Allocate memory by pages.
/// Size of each page is defined as MEMORY_PAGE_SIZE.
LD_API void* internal_ldpage_alloc( usize pages, MemoryType type );
// IMPORTANT(alicia): Internal use only!
/// Free memory pages.
LD_API void internal_ldpage_free( void* memory, usize pages, MemoryType type );
// IMPORTANT(alicia): Internal use only!
/// Allocate memory by pages.
/// Size of each page is defined as MEMORY_PAGE_SIZE.
LD_API void* internal_ldpage_alloc_trace(
    usize pages, MemoryType type,
    const char* function,
    const char* file,
    int line
);
// IMPORTANT(alicia): Internal use only!
/// Free memory pages.
LD_API void internal_ldpage_free_trace(
    void* memory, usize pages, MemoryType type,
    const char* function,
    const char* file,
    int line
);

#if defined(LD_LOGGING)
    #define ldpage_alloc( pages, type )\
        internal_ldpage_alloc_trace(\
            pages, type, __FUNCTION__, __FILE__, __LINE__ )
    #define ldpage_free( memory, pages, type )\
        internal_ldpage_free_trace(\
            memory, pages, type, __FUNCTION__, __FILE__, __LINE__ )
#else
    #define ldpage_alloc( pages, type )\
        internal_ldpage_alloc( pages, type )
    #define ldpage_free( memory, pages, type )\
        internal_ldpage_free( memory, pages, type )
#endif

/// Query memory usage for each memory type.
LD_API usize query_memory_usage( MemoryType memtype );
/// Query total memory usage in bytes.
LD_API usize query_total_memory_usage();

/// Copy memory from source pointer to destination pointer.
LD_API void mem_copy( void* dst, const void* src, usize size );
/// Copy memory between overlapping buffers.
/// Potentially a lot slower than mem_copy so only use when necessary.
LD_API void mem_copy_overlapped( void* dst, const void* src, usize size );
/// Set a range of bytes to a specific value.
LD_API void* mem_set( void* dst, int value, usize n );
/// Zero out memory.
LD_API void mem_zero( void* ptr, usize size );

#endif
