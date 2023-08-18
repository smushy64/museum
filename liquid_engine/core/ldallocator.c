// * Description:  Allocator Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 17, 2023
#include "core/ldallocator.h"
#include "core/ldmemory.h"
#include "core/ldlog.h"

LD_API b32 internal_stack_allocator_create(
    usize size, enum MemoryType memory_type, StackAllocator* out_allocator
) {
    void* buffer = internal_ldalloc( size, memory_type );
    if( !buffer ) {
        return false;
    }

    out_allocator->buffer      = buffer;
    out_allocator->size        = size;
    out_allocator->memory_type = memory_type;
    out_allocator->current     = 0;

    return true;
}
LD_API void internal_stack_allocator_destroy( StackAllocator* allocator ) {
    if( allocator->buffer ) {
        internal_ldfree(
            allocator->buffer,
            allocator->size,
            allocator->memory_type
        );
    }
    mem_zero( allocator, sizeof(StackAllocator) );
}
LD_API void* internal_stack_allocator_push(
    StackAllocator* allocator, usize size
) {
    usize new_current = size + allocator->current;
    if( new_current > allocator->size ) {
        return NULL;
    }
    void* result = (u8*)allocator->buffer + allocator->current;
    allocator->current = new_current;
    return result;
}
LD_API void internal_stack_allocator_pop(
    StackAllocator* allocator, usize size
) {
    ASSERT( allocator->current >= size );
    if( !allocator->current ) {
        return;
    }
    allocator->current -= size;
}

LD_API b32 internal_stack_allocator_create_trace(
    usize size, enum MemoryType memory_type, StackAllocator* out_allocator,
    const char* function,
    const char* file, int line
) {
    b32 result = internal_stack_allocator_create(
        size, memory_type, out_allocator
    );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_GREEN,
            LOG_FLAG_NEW_LINE,
            "[STACK CREATE | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Pointer: {u64,x}",
            function, file, line,
            memory_type_to_string( memory_type ),
            (u64)size, (u64)out_allocator->buffer
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_RED,
            LOG_FLAG_NEW_LINE,
            "[STACK CREATE FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64}",
            function, file, line,
            memory_type_to_string( memory_type ),
            (u64)size
        );
    }
    return result;
}
LD_API void internal_stack_allocator_destroy_trace(
    StackAllocator* allocator,
    const char* function,
    const char* file, int line
) {
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_CYAN,
        LOG_FLAG_NEW_LINE,
        "[STACK DESTROY | {cc}() | {cc}:{i}] "
        "{cc} Size: {u64} Pointer: {u64,x}",
        function, file, line,
        memory_type_to_string( allocator->memory_type ),
        (u64)allocator->size, (u64)allocator->buffer
    );
    internal_stack_allocator_destroy( allocator );
}
LD_API void* internal_stack_allocator_push_trace(
    StackAllocator* allocator, usize size,
    const char* function,
    const char* file, int line
) {
    usize current = allocator->current;
    void* result = internal_stack_allocator_push( allocator, size );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_GREEN,
            LOG_FLAG_NEW_LINE,
            "[STACK PUSH | {cc}() | {cc}:{i}] "
            "Current: {u64} Size: {u64} Pointer: {u64,x}",
            function, file, line,
            (u64)allocator->current,
            (u64)size, (u64)allocator->buffer
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_RED,
            LOG_FLAG_NEW_LINE,
            "[STACK PUSH FAILED | {cc}() | {cc}:{i}] "
            "Current: {u64} Size: {u64} Pointer: {u64,x}",
            function, file, line,
            (u64)current, (u64)allocator->size, (u64)allocator->buffer
        );
    }
    return result;
}
LD_API void internal_stack_allocator_pop_trace(
    StackAllocator* allocator, usize size,
    const char* function,
    const char* file, int line
) {
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_GREEN,
        LOG_FLAG_NEW_LINE,
        "[STACK POP | {cc}() | {cc}:{i}] "
        "Current: {u64} Size: {u64} Pointer: {u64,x}",
        function, file, line,
        (u64)allocator->current,
        (u64)size, (u64)allocator->buffer
    );
    internal_stack_allocator_pop( allocator, size );
}


