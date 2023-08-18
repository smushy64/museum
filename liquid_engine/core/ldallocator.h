#if !defined(CORE_ALLOCATOR_HPP)
#define CORE_ALLOCATOR_HPP
// * Description:  Allocators
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 17, 2023
#include "defines.h"

enum MemoryType : u8;

/// Stack based allocator
typedef struct StackAllocator {
    void* buffer;
    usize size;
    usize current;
    enum MemoryType memory_type;
} StackAllocator;

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

#endif // header guard
