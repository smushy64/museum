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

#if defined(LD_COMPILER_MSVC)
    #include <malloc.h>
#endif

enum MemoryType : u64 {
    MEMTYPE_UNKNOWN,
    MEMTYPE_APPLICATION,
    MEMTYPE_DYNAMIC_LIST,
    MEMTYPE_PLATFORM,
    MEMTYPE_EVENT_LISTENER_REGISTRY,
    MEMTYPE_RENDERER,
    MEMTYPE_LOGGING,
    MEMTYPE_THREADING,
    MEMTYPE_STRING,
    MEMTYPE_USER,

    MEMTYPE_COUNT
};
inline const char* to_string(MemoryType memtype) {
    local const char* strings[MEMTYPE_COUNT] = {
        "Unknown Memory",
        "Application Memory",
        "Dynamic List Memory",
        "Platform Memory",
        "Event Listener Registry Memory",
        "Renderer Memory",
        "Logging Buffer Memory",
        "Threading Memory",
        "String Memory",
        "User Memory"
    };
    if( memtype >= MEMTYPE_COUNT ) {
        return strings[0];
    }
    return strings[memtype];
}

namespace impl {

/// Allocate memory.
LD_API void* _mem_alloc( usize size, MemoryType type );
/// Reallocate memory.
LD_API void* _mem_realloc( void* memory, usize new_size );
/// Free memory.
LD_API void _mem_free( void* memory );

/// Allocate memory.
LD_API void* _mem_alloc_trace(
    usize size,
    MemoryType type,
    const char* function,
    const char* file,
    int line
);
/// Reallocate memory.
LD_API void* _mem_realloc_trace(
    void* memory,
    usize new_size,
    const char* function,
    const char* file,
    int line
);
/// Free memory.
LD_API void _mem_free_trace(
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
LD_API usize mem_query_size( void* memory );
/// Query the type of a memory block
LD_API MemoryType mem_query_type( void* memory );

/// Query memory usage for each memory type.
LD_API usize query_memory_usage( MemoryType memtype );
/// Query total memory usage in bytes.
LD_API usize query_total_memory_usage();

#if defined(LD_COMPILER_MSVC)
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
LD_API void mem_copy( void* dst, const void* src, usize size );
/// Copy memory between overlapping buffers.
/// Potentially a lot slower than mem_copy so only use when necessary.
LD_API void mem_copy_overlapped( void* dst, const void* src, usize size );
/// Set a range of bytes to a specific value.
LD_API void mem_set( u8 value, usize dst_size, void* dst );
/// Zero out memory.
LD_API void mem_zero( void* ptr, usize size );

#endif
