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

/// Memory arena that works like a stack.
/// Push new items and pop old items off the top of the stack.
typedef struct StackArena {
    void* arena;
    u32 stack_pointer;
    u32 arena_size;
} StackArena;

/// Types of memory allocations
typedef enum : u64 {
    MEMTYPE_UNKNOWN,
    MEMTYPE_ENGINE,
    MEMTYPE_DYNAMIC_LIST,
    MEMTYPE_RENDERER,
    MEMTYPE_STRING,
    MEMTYPE_USER,

    MEMTYPE_COUNT
} MemoryType;
/// Memory type to string
LD_API const char* memory_type_to_string( MemoryType type );

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

/// Page allocate memory.
LD_API void* _mem_page_alloc( usize size, MemoryType type );
/// Free page allocated memory.
LD_API void _mem_page_free( void* memory );

/// Page allocate memory.
LD_API void* _mem_page_alloc_trace(
    usize size, MemoryType type,
    const char* function,
    const char* file,
    int line
);
/// Free page allocated memory.
LD_API void _mem_page_free_trace(
    void* memory,
    const char* function,
    const char* file,
    int line
);

/// Create a stack arena.
LD_API b32 _stack_arena_create(
    u32 size, MemoryType type,
    StackArena* out_arena
);
/// Free a stack arena.
LD_API void _stack_arena_free( StackArena* arena );
/// Push an item into stack arena.
LD_API void* _stack_arena_push_item( StackArena* arena, u32 item_size );
/// Pop an item from stack arena.
LD_API void _stack_arena_pop_item( StackArena* arena, u32 item_size );

/// Create a stack arena.
LD_API b32 _stack_arena_create_trace(
    u32 size, MemoryType type,
    StackArena* out_arena,
    const char* function,
    const char* file,
    int line
);
/// Free a stack arena.
LD_API void _stack_arena_free_trace(
    StackArena* arena,
    const char* function,
    const char* file,
    int line
);
/// Push an item into stack arena.
LD_API void* _stack_arena_push_item_trace(
    StackArena* arena, u32 item_size,
    const char* function,
    const char* file,
    int line
);
/// Pop an item from stack arena.
LD_API void _stack_arena_pop_item_trace(
    StackArena* arena, u32 item_size,
    const char* function,
    const char* file,
    int line
);

#if defined(LD_LOGGING)
    #define mem_alloc( size, type )\
        _mem_alloc_trace(\
            size,\
            type,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define mem_realloc( memory, new_size )\
        _mem_realloc_trace(\
            memory,\
            new_size,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define mem_free( memory )\
        _mem_free_trace(\
            memory,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define mem_page_alloc( size, type )\
        _mem_page_alloc_trace(\
            size, type,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define mem_page_free( memory )\
        _mem_page_free_trace(\
            memory,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define stack_arena_create( size, type, out_arena )\
        _stack_arena_create_trace(\
            size, type, out_arena,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define stack_arena_free( arena )\
        _stack_arena_free_trace(\
            arena,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define stack_arena_push_item( arena, item_size )\
        _stack_arena_push_item_trace(\
            arena, item_size,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define stack_arena_pop_item( arena, item_size )\
        _stack_arena_pop_item_trace(\
            arena, item_size,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )

#else
    #define mem_alloc( size, type )\
        _mem_alloc( size, type )
    #define mem_realloc( memory, new_size )\
        _mem_realloc( memory, new_size )
    #define mem_free( memory )\
        _mem_free( memory )
    #define mem_page_alloc( size, type )\
        _mem_page_alloc( size, type )
    #define mem_page_free( memory )\
        _mem_page_free( memory )
    #define stack_arena_create( size, type, out_arena )\
        _stack_arena_create( size, type, out_arena )
    #define stack_arena_free( arena )\
        _stack_arena_free( arena )
    #define stack_arena_push_item( arena, item_size )\
        _stack_arena_push_item( arena, item_size )
    #define stack_arena_pop_item( arena, item_size )\
        _stack_arena_pop_item( arena, item_size )
#endif

#define stack_arena_push( arena, type )\
    (type*)stack_arena_push_item( &(arena), sizeof(type) )
#define stack_arena_pop( arena, type )\
    stack_arena_pop_item( &(arena), sizeof(type) )

/// Query the size of a memory block
LD_API usize mem_query_size( void* memory );
/// Query the type of a memory block
LD_API MemoryType mem_query_type( void* memory );

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
