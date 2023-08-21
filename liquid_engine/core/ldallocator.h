#if !defined(CORE_ALLOCATOR_HPP)
#define CORE_ALLOCATOR_HPP
// * Description:  Allocators
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 17, 2023
#include "defines.h"

enum MemoryType : u8;

/// Tracks state of a block of memory.
/// Does not actually alloc or free memory,
/// instead it provides offsets into a block of memory.
typedef struct MemoryState {
    usize max_blocks;
    usize memory_size;
    void* head;
    /// memory state buffer.
    void* buffer;
} MemoryState;
/// Calculate buffer size for memory state.
LD_API usize memory_state_required_buffer_size( usize max_blocks );
/// Calculate block count for buffer size for memory state.
LD_API usize memory_state_buffer_size_to_block_count( usize buffer_size );
/// Initialize memory state tracker.
LD_API MemoryState memory_state_init(
    usize max_blocks, void* block_buffer, usize memory_size );
/// Request a block of memory from memory state tracker.
LD_API b32 memory_state_request_block(
    MemoryState* state, usize size, usize* out_offset );
/// Free a block of memory from memory state tracker.
LD_API b32 memory_state_return_block(
    MemoryState* state, usize offset, usize size );
/// Calculate how much free space remains in memory block.
LD_API usize memory_state_calculate_free_space( MemoryState* state );

/// Dynamic allocator.
typedef struct DynamicAllocator {
    MemoryState state;
    void* buffer;
} DynamicAllocator;
/// Get dynamic allocator backing buffer size.
headerfn usize dynamic_allocator_size( DynamicAllocator* allocator ) {
    return allocator->state.memory_size;
}
/// Create dynamic allocator from existing buffers.
headerfn DynamicAllocator dynamic_allocator_from_buffer(
    usize memory_state_buffer_size, void* memory_state_buffer,
    usize backing_buffer_size, void* backing_buffer
) {
    DynamicAllocator allocator = {};
    usize max_blocks =
        memory_state_buffer_size_to_block_count( memory_state_buffer_size );
    allocator.state = memory_state_init(
        max_blocks, memory_state_buffer, backing_buffer_size );
    allocator.buffer = backing_buffer;
    return allocator;
}
/// IMPORTANT(alicia): Internal use only!
/// Allocate aligned memory from dynamic allocator.
/// Always returns zeroed memory.
LD_API void* internal_dynamic_allocator_alloc_aligned(
    DynamicAllocator* allocator, usize size, usize alignment );
/// IMPORTANT(alicia): Internal use only!
/// Free aligned memory allocd from dynamic allocator.
LD_API void internal_dynamic_allocator_free_aligned(
    DynamicAllocator* allocator, void* memory, usize size, usize alignment );
/// IMPORTANT(alicia): Internal use only!
/// Allocate aligned memory from dynamic allocator.
/// Always returns zeroed memory.
LD_API void* internal_dynamic_allocator_alloc_aligned_trace(
    DynamicAllocator* allocator, usize size, usize alignment,
    const char* function, const char* file, int line
);
/// IMPORTANT(alicia): Internal use only!
/// Free aligned memory allocd from dynamic allocator.
LD_API void internal_dynamic_allocator_free_aligned_trace(
    DynamicAllocator* allocator, void* memory, usize size, usize alignment,
    const char* function, const char* file, int line
);

#if defined(LD_LOGGING)
    #define dynamic_allocator_alloc_aligned( allocator, size, alignment )\
        internal_dynamic_allocator_alloc_aligned_trace(\
            allocator, size, alignment, __FUNCTION__, __FILE__, __LINE__ )
    #define dynamic_allocator_free_aligned( allocator, memory, size, alignment )\
        internal_dynamic_allocator_free_aligned_trace(\
            allocator, memory, size, alignment, __FUNCTION__, __FILE__, __LINE__ )
    #define dynamic_allocator_alloc( allocator, size )\
        internal_dynamic_allocator_alloc_aligned_trace(\
            allocator, size, 1, __FUNCTION__, __FILE__, __LINE__ )
    #define dynamic_allocator_free( allocator, memory, size )\
        internal_dynamic_allocator_free_aligned_trace(\
            allocator, memory, size, 1, __FUNCTION__, __FILE__, __LINE__ )
#else
    #define dynamic_allocator_alloc_aligned( allocator, size, alignment )\
        internal_dynamic_allocator_alloc_aligned(\
            allocator, size, alignment )
    #define dynamic_allocator_free_aligned( allocator, memory, size, alignment )\
        internal_dynamic_allocator_free_aligned(\
            allocator, memory, size, alignment )
    #define dynamic_allocator_alloc( allocator, size )\
        internal_dynamic_allocator_alloc_aligned(\
            allocator, size, 1 )
    #define dynamic_allocator_free( allocator, memory, size )\
        internal_dynamic_allocator_free_aligned(\
            allocator, memory, size, 1 )
#endif

/// Stack based allocator.
typedef struct StackAllocator {
    void* buffer;
    usize size;
    usize current;
    enum MemoryType memory_type;
} StackAllocator;
/// Create a stack allocator using an external buffer.
headerfn StackAllocator stack_allocator_from_buffer(
    usize buffer_size, void* buffer, enum MemoryType memory_type
) {
    StackAllocator result = {};
    result.buffer      = buffer;
    result.size        = buffer_size;
    result.current     = 0;
    result.memory_type = memory_type;
    return result;
}
/// Calculate remaining stack allocator size.
headerfn usize stack_allocator_remaining_size( StackAllocator* allocator ) {
    return allocator->size - allocator->current;
}

// IMPORTANT(alicia): Internal use only!
// Create new stack allocator.
LD_API b32 internal_stack_allocator_create(
    usize size, enum MemoryType memory_type, StackAllocator* out_allocator );
// IMPORTANT(alicia): Internal use only!
// Destroy stack allocator.
LD_API void internal_stack_allocator_destroy( StackAllocator* allocator );
// IMPORTANT(alicia): Internal use only!
// Push new item onto stack allocator.
LD_API void* internal_stack_allocator_push(
    StackAllocator* allocator, usize size );
// IMPORTANT(alicia): Internal use only!
// Pop item from top of stack allocator.
LD_API void internal_stack_allocator_pop(
    StackAllocator* allocator, usize size );

// IMPORTANT(alicia): Internal use only!
// Create new stack allocator.
LD_API b32 internal_stack_allocator_create_trace(
    usize size, enum MemoryType memory_type, StackAllocator* out_allocator,
    const char* function,
    const char* file, int line
);
// IMPORTANT(alicia): Internal use only!
// Destroy stack allocator.
LD_API void internal_stack_allocator_destroy_trace(
    StackAllocator* allocator,
    const char* function,
    const char* file, int line
);
// IMPORTANT(alicia): Internal use only!
// Push new item onto stack allocator.
LD_API void* internal_stack_allocator_push_trace(
    StackAllocator* allocator, usize size,
    const char* function,
    const char* file, int line
);
// IMPORTANT(alicia): Internal use only!
// Pop item from top of stack allocator.
LD_API void internal_stack_allocator_pop_trace(
    StackAllocator* allocator, usize size,
    const char* function,
    const char* file, int line
);

#if defined(LD_LOGGING)
    #define stack_allocator_create( size, memory_type, out_allocator )\
        internal_stack_allocator_create_trace(\
            size, memory_type, out_allocator,\
            __FUNCTION__, __FILE__, __LINE__\
        )
    #define stack_allocator_destroy( allocator )\
        internal_stack_allocator_destroy_trace( allocator,\
            __FUNCTION__, __FILE__, __LINE__\
        )
    #define stack_allocator_push( allocator, size )\
        internal_stack_allocator_push_trace( allocator, size,\
            __FUNCTION__, __FILE__, __LINE__\
        )
    #define stack_allocator_pop( allocator, size )\
        internal_stack_allocator_pop_trace( allocator, size,\
            __FUNCTION__, __FILE__, __LINE__\
        )
#else
    #define stack_allocator_create( size, memory_type, out_allocator )\
        internal_stack_allocator_create( size, memory_type, out_allocator )
    #define stack_allocator_destroy( allocator )\
        internal_stack_allocator_destroy( allocator )
    #define stack_allocator_push( allocator, size )\
        internal_stack_allocator_push( allocator, size )
    #define stack_allocator_pop( allocator, size )\
        internal_stack_allocator_pop( allocator, size )
#endif

/// Generic Allocator type.
typedef enum AllocatorType : u8 {
    ALLOCATOR_TYPE_SYSTEM,
    ALLOCATOR_TYPE_DYNAMIC,
    ALLOCATOR_TYPE_STACK,

    ALLOCATOR_TYPE_COUNT
} AllocatorType;
headerfn const char* allocator_type_to_string( AllocatorType allocator ) {
    const char* strings[ALLOCATOR_TYPE_COUNT] = {
        "System Allocator",
        "Dynamic Allocator",
        "Stack Allocator"
    };
    ASSERT( allocator < ALLOCATOR_TYPE_COUNT );
    return strings[allocator];
}

/// Generic Allocator.
typedef struct Allocator {
    AllocatorType type;
    union {
        DynamicAllocator* dynamic;
        StackAllocator*   stack;
    };
} Allocator;
/// Create generic allocator that uses system allocator.
headerfn Allocator allocator_system() {
    Allocator result = {};
    result.type = ALLOCATOR_TYPE_SYSTEM;
    return result;
}
/// Create generic allocator from stack allocator.
headerfn Allocator allocator_from_stack( StackAllocator* stack ) {
    Allocator result;
    result.type  = ALLOCATOR_TYPE_STACK;
    result.stack = stack;
    return result;
}
/// Create generic allocator from dynamic allocator.
headerfn Allocator allocator_from_dynamic( DynamicAllocator* dynamic ) {
    Allocator result;
    result.type    = ALLOCATOR_TYPE_DYNAMIC;
    result.dynamic = dynamic;
    return result;
}

// IMPORTANT(alicia): Internal use only!
/// Allocate aligned memory from a generic allocator.
LD_API void* internal_allocator_alloc_aligned(
    Allocator* allocator, usize size, enum MemoryType type, usize alignment );
// IMPORTANT(alicia): Internal use only!
/// Free aligned memory from a generic allocator.
LD_API void internal_allocator_free_aligned(
    Allocator* allocator, void* memory,
    usize size, enum MemoryType type, usize alignment );
// IMPORTANT(alicia): Internal use only!
/// Allocate memory from a generic allocator.
LD_API void* internal_allocator_alloc(
    Allocator* allocator, usize size, enum MemoryType type );
// IMPORTANT(alicia): Internal use only!
/// Free memory from a generic allocator.
LD_API void internal_allocator_free(
    Allocator* allocator, void* memory, usize size, enum MemoryType type );

// IMPORTANT(alicia): Internal use only!
/// Allocate aligned memory from a generic allocator.
LD_API void* internal_allocator_alloc_aligned_trace(
    Allocator* allocator, usize size, enum MemoryType type, usize alignment,
    const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Free aligned memory from a generic allocator.
LD_API void internal_allocator_free_aligned_trace(
    Allocator* allocator, void* memory,
    usize size, enum MemoryType type, usize alignment,
    const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Allocate memory from a generic allocator.
LD_API void* internal_allocator_alloc_trace(
    Allocator* allocator, usize size, enum MemoryType type,
    const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Free memory from a generic allocator.
LD_API void internal_allocator_free_trace(
    Allocator* allocator, void* memory, usize size, enum MemoryType type,
    const char* function, const char* file, int line );

#if defined(LD_LOGGING)
    #define allocator_alloc_aligned( allocator, size, type, alignment )\
        internal_allocator_alloc_aligned_trace(\
            allocator, size, type, alignment,\
            __FUNCTION__, __FILE__, __LINE__)
    #define allocator_alloc( allocator, size, type )\
        internal_allocator_alloc_trace(\
            allocator, size, type,\
            __FUNCTION__, __FILE__, __LINE__)
    #define allocator_free_aligned( allocator, memory, size, type, alignment )\
        internal_allocator_free_aligned_trace(\
            allocator, memory, size, type, alignment,\
            __FUNCTION__, __FILE__, __LINE__)
    #define allocator_free( allocator, memory, size, type )\
        internal_allocator_free_trace(\
            allocator, memory, size, type,\
            __FUNCTION__, __FILE__, __LINE__)
#else
    #define allocator_alloc_aligned( allocator, size, type, alignment )\
        internal_allocator_alloc_aligned( allocator, size, type, alignment )
    #define allocator_alloc( allocator, size, type )\
        internal_allocator_alloc( allocator, size, type )
    #define allocator_free_aligned( allocator, memory, size, type, alignment )\
        internal_allocator_free_aligned(\
            allocator, memory, size, type, alignment )
    #define allocator_free( allocator, memory, size, type )\
        internal_allocator_free(\
            allocator, memory, size, type )
#endif

#endif // header guard
