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

#if defined(SM_COMPILER_MSVC)
    #include <malloc.h>
#endif

enum MemoryType : u64 {
    MEMTYPE_UNKNOWN,
    MEMTYPE_DYNAMIC_LIST,
    MEMTYPE_PLATFORM_DATA,
    MEMTYPE_EVENT_LISTENER_REGISTRY,
    MEMTYPE_RENDERER,
    MEMTYPE_LOGGING,

    MEMTYPE_COUNT
};
inline const char* to_string(MemoryType memtype) {
    local const char* strings[MEMTYPE_COUNT] = {
        "Unknown Memory",
        "Dynamic List Memory",
        "Platform Data Memory",
        "Event Listener Registry Memory",
        "Renderer Memory",
        "Logging Buffer Memory"
    };
    if( memtype >= MEMTYPE_COUNT ) {
        return strings[0];
    }
    return strings[memtype];
}

namespace impl {

/// Allocate memory.
SM_API void* _mem_alloc( usize size, MemoryType type );
/// Reallocate memory.
SM_API void* _mem_realloc( void* memory, usize new_size );
/// Free memory.
SM_API void _mem_free( void* memory );

/// Allocate memory.
SM_API void* _mem_alloc_trace(
    usize size,
    MemoryType type,
    const char* function,
    const char* file,
    int line
);
/// Reallocate memory.
SM_API void* _mem_realloc_trace(
    void* memory,
    usize new_size,
    const char* function,
    const char* file,
    int line
);
/// Free memory.
SM_API void _mem_free_trace(
    void* memory,
    const char* function,
    const char* file,
    int line
);

}; // namespace impl

#if defined(LD_LOGGING)
    #define mem_alloc( size, type )\
        ::impl::_mem_alloc_trace(\
            size,\
            type,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define mem_realloc( memory, new_size )\
        ::impl::_mem_realloc_trace(\
            memory,\
            new_size,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define mem_free( memory )\
        ::impl::_mem_free_trace(\
            memory,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define mem_alloc( size, type )\
        ::impl::_mem_alloc( size, type )
    #define mem_realloc( memory, new_size )\
        ::impl::_mem_realloc( memory, new_size )
    #define mem_free( memory )\
        ::impl::_mem_free( memory )
#endif

/// Query the size of a memory block
SM_API usize mem_query_size( void* memory );
/// Query the type of a memory block
SM_API MemoryType mem_query_type( void* memory );

/// Query memory usage for each memory type.
SM_API usize query_memory_usage( MemoryType memtype );
/// Query total memory usage in bytes.
SM_API usize query_total_memory_usage();

#if defined(SM_COMPILER_MSVC)
    /// Allocate memory on the stack.
    /// Does not require a free call.
    /// Not guaranteed to be zeroed out.
    #define stack_alloc(size) _alloca(size)
#else
    /// Allocate memory on the stack.
    /// Does not require a free call.
    /// Not guaranteed to be zeroed out.
    #define stack_alloc(size) __builtin_alloca(size)
#endif

/// Copy memory from source pointer to destination pointer.
SM_API void mem_copy( void* dst, const void* src, usize size );
/// Copy memory between overlapping buffers.
/// Potentially a lot slower than mem_copy so only use when necessary.
SM_API void mem_overlap_copy( void* dst, const void* src, usize size );
/// Set a range of bytes to a specific value.
SM_API void mem_set( u8 value, usize dst_size, void* dst );
/// Zero out memory.
SM_API void mem_zero( void* ptr, usize size );

#endif