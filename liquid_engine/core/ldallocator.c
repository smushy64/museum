// * Description:  Allocator Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 17, 2023
#include "core/ldallocator.h"
#include "core/ldmemory.h"
#include "core/ldlog.h"

#if defined(LD_ARCH_64_BIT)
    /// Most significant bit is set if block of memory is valid.
    #define MEMORY_BLOCK_VALID_ID ((usize)(1ull << 63ull))
#else
    /// Most significant bit is set if block of memory is valid.
    #define MEMORY_BLOCK_VALID_ID ((usize)(1u << 31u))
#endif

/// Check if most significant bit is set.
#define IS_MEMORY_BLOCK_ID_VALID( id )\
    (CHECK_BITS( (id), MEMORY_BLOCK_VALID_ID ))

/// Block of memory in memory state.
struct MemoryBlock {
    usize offset;
    usize size;
    struct MemoryBlock* next;
};
/// Get block offset.
always_inline
internal usize block_offset( struct MemoryBlock* block ) {
    return ((block->offset) & ~(MEMORY_BLOCK_VALID_ID) );
}
/// Set block offset.
always_inline
internal void block_set_offset( struct MemoryBlock* block, usize offset ) {
    block->offset = ((offset) | MEMORY_BLOCK_VALID_ID);
}
/// Get block size.
always_inline
internal usize block_size( struct MemoryBlock* block ) {
    return ((block->size) & ~(MEMORY_BLOCK_VALID_ID) );
}
/// Set block size.
always_inline
internal void block_set_size( struct MemoryBlock* block, usize size ) {
    block->size = ((size) | MEMORY_BLOCK_VALID_ID);
}

LD_API usize memory_state_required_buffer_size( usize blocks ) {
    return sizeof(struct MemoryBlock) * blocks;
}
LD_API usize memory_state_buffer_size_to_block_count( usize buffer_size ) {
    ASSERT( buffer_size % sizeof(struct MemoryBlock) == 0 );
    return buffer_size / sizeof( struct MemoryBlock );
}
LD_API MemoryState memory_state_init(
    usize max_blocks,
    void* block_buffer,
    usize memory_size
) {
    MemoryState state = {};
    state.max_blocks  = max_blocks;
    state.memory_size = memory_size;
    state.head        = block_buffer;
    state.buffer      = block_buffer;

    struct MemoryBlock* head = state.head;
    block_set_offset( head, 0 );
    block_set_size( head, memory_size );
    head->next   = NULL;

    return state;
}
internal struct MemoryBlock* find_free_block(
    usize max_blocks, struct MemoryBlock* blocks
) {
    for( usize i = 0; i < max_blocks; ++i ) {
        struct MemoryBlock* current = blocks + i;
        if( !IS_MEMORY_BLOCK_ID_VALID( current->offset ) ) {
            return current;
        }
    }
    return NULL;
}

#define reset_block( pblock )\
    ( mem_zero( pblock, sizeof(struct MemoryBlock) ) )

LD_API b32 memory_state_request_block(
    MemoryState* state, usize size,
    usize* out_offset
) {
    // search through memory block to find a block that can fit
    // the size requested
 
    struct MemoryBlock* current = state->head;
    struct MemoryBlock* last    = NULL;

    while( current ) {
        usize current_size   = block_size( current );
        usize current_offset = block_offset( current );

        if( current_size == size ) {
            *out_offset = block_offset( current );
            struct MemoryBlock* block_to_reset = NULL;
            if( last ) {
                last->next     = current->next;
                block_to_reset = current;
            } else {
                block_to_reset = state->head;
                state->head    = current->next;
            }
            reset_block( block_to_reset );

            return true;
        } else if( current_size > size ) {
            *out_offset = block_offset( current );
            block_set_size( current, current_size - size );
            block_set_offset( current, current_offset + size );
            return true;
        }

        last    = current;
        current = current->next;
    }

    LOG_ERROR(
        "Unable to fill memory state request! "
        "Requested size: {u64}",
        (u64)size
    );
    return false;
}
LD_API b32 memory_state_return_block(
    MemoryState* state, usize offset, usize size
) {
    ASSERT( state );
    ASSERT( size );

    struct MemoryBlock* current = state->head;
    struct MemoryBlock* last    = NULL;

    if( !current ) {
        struct MemoryBlock* new_block =
            find_free_block( state->max_blocks, state->buffer );
        block_set_offset( new_block, offset );
        block_set_size( new_block, size );
        new_block->next   = NULL;

        state->head = new_block;
        return true;
    }

    while( current ) {
        usize current_offset = block_offset( current );
        usize current_size   = block_size( current );

        LOG_ASSERT(
            current_offset != offset,
            "Attempted to free an already freed memory block at offset {u64}!",
            (u64)current_offset
        );

        if( current_offset + current_size == offset ) {
            current_size = size;
            block_set_size( current, current_size );

            usize next_offset = block_offset( current->next );
            usize next_size   = block_size( current->next );

            if(
                current->next &&
                next_offset == current_offset + current_size
            ) {
                current_size += next_size;
                block_set_size( current, current_size );
                struct MemoryBlock* next = current->next;
                current->next = current->next->next;
                reset_block( next );
            }
            return true;
        } else if( current_offset > offset ) {
            struct MemoryBlock* new_block =
                find_free_block( state->max_blocks, state->buffer );
            block_set_offset( new_block, offset );
            block_set_size( new_block, size );

            if( last ) {
                last->next      = new_block;
                new_block->next = current;
            } else {
                new_block->next = current;
                state->head     = new_block;
            }

            usize new_block_offset = block_offset( new_block );
            usize new_block_size   = block_size( new_block );

            if( new_block->next ) {
                usize new_block_next_offset = block_offset( new_block->next );
                if(
                    new_block_offset + new_block_size ==
                    new_block_next_offset
                ) {
                    usize new_block_next_size = block_size( new_block->next );

                    new_block_size += new_block_next_size;
                    block_set_size( new_block, new_block_size );

                    struct MemoryBlock* block_to_reset =
                        new_block->next;
                    new_block->next = block_to_reset->next;
                    reset_block( block_to_reset );
                }
            }

            if( last ) {
                usize last_offset = block_offset( last );
                usize last_size   = block_size( last );

                if( last_offset + last_size == new_block_offset ) {
                    last_size += new_block_size;
                    block_set_size( last, last_size );

                    struct MemoryBlock* block_to_reset = new_block;
                    last->next = block_to_reset->next;

                    reset_block( block_to_reset );
                }
            }
            return true;
        }

        if( !current->next && current_offset + current_size < offset ) {
            struct MemoryBlock* new_block =
                find_free_block( state->max_blocks, state->buffer );
            block_set_offset( new_block, offset );
            block_set_size( new_block, size );
            new_block->next   = NULL;

            current->next = new_block;

            return true;
        }

        last    = current;
        current = current->next;
    }

    LOG_ERROR(
        "Unable to find memory block! "
        "Offset: {u64} Size: {u64}",
        (u64)offset, (u64)size
    );
    return false;
}
LD_API usize memory_state_calculate_free_space( MemoryState* state ) {
    struct MemoryBlock* current = state->head;
    usize total = 0;
    while( current ) {
        total   += block_size( current );
        current =  current->next;
    }

    return total;
}

LD_API void* internal_dynamic_allocator_alloc_aligned(
    DynamicAllocator* allocator, usize size, usize alignment
) {
    // TODO(alicia): ALIGNMENT
    unused(alignment);
    usize offset = 0;
    if( memory_state_request_block( &allocator->state, size, &offset ) ) {
        void* result = (u8*)allocator->buffer + offset;
        return result;
    } else {
        return NULL;
    }
}
LD_API void internal_dynamic_allocator_free_aligned(
    DynamicAllocator* allocator, void* memory, usize size, usize alignment
) {
    // TODO(alicia): ALIGNMENT
    unused(alignment);
    if( !memory || !size ) {
        LOG_WARN( "Cannot free null pointer/0 size memory!" );
        return;
    }
    mem_zero( memory, size );
    usize offset = (u8*)allocator->buffer - (u8*)memory;
    ASSERT( memory_state_return_block( &allocator->state, offset, size ) );
}
LD_API void* internal_dynamic_allocator_alloc_aligned_trace(
    DynamicAllocator* allocator, usize size, usize alignment,
    const char* function, const char* file, int line
) {
    void* result = internal_dynamic_allocator_alloc_aligned(
        allocator, size, alignment );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_GREEN
            "[DYN ALLOC | {cc}() | {cc}:{i}] "
            "Size: {u64} Alignment: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)size, (u64)alignment, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[DYN ALLOC FAILED | {cc}() | {cc}:{i}] "
            "Size: {u64} Alignment: {u64}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)size, (u64)alignment
        );
    }
    return result;
}
LD_API void internal_dynamic_allocator_free_aligned_trace(
    DynamicAllocator* allocator, void* memory, usize size, usize alignment,
    const char* function, const char* file, int line
) {
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        false, true,
        LOG_COLOR_CYAN
        "[DYN FREE | {cc}() | {cc}:{i}] "
        "Size: {u64} Alignment: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
        function, file, line,
        (u64)size, (u64)alignment, (u64)memory
    );
    internal_dynamic_allocator_free_aligned(
        allocator, memory, size, alignment );
}

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
            false, true,
            LOG_COLOR_GREEN
            "[STACK CREATE | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            memory_type_to_string( memory_type ),
            (u64)size, (u64)out_allocator->buffer
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[STACK CREATE FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64}"
            LOG_COLOR_RESET,
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
        false, true,
        LOG_COLOR_CYAN
        "[STACK DESTROY | {cc}() | {cc}:{i}] "
        "{cc} Size: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
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
            false, true,
            LOG_COLOR_GREEN
            "[STACK PUSH | {cc}() | {cc}:{i}] "
            "Current: {u64} Size: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)allocator->current,
            (u64)size, (u64)allocator->buffer
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[STACK PUSH FAILED | {cc}() | {cc}:{i}] "
            "Current: {u64} Size: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
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
        false, true,
        LOG_COLOR_CYAN
        "[STACK POP | {cc}() | {cc}:{i}] "
        "Current: {u64} Size: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
        function, file, line,
        (u64)allocator->current,
        (u64)size, (u64)allocator->buffer
    );
    internal_stack_allocator_pop( allocator, size );
}

LD_API void* internal_allocator_alloc_aligned(
    Allocator* allocator, usize size,
    enum MemoryType type, usize alignment
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            return internal_ldalloc_aligned( size, type, alignment );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            return internal_dynamic_allocator_alloc_aligned(
                allocator->dynamic, size, alignment );
        } break;
        default: UNIMPLEMENTED();
    }
    return NULL;
}
LD_API void internal_allocator_free_aligned(
    Allocator* allocator, void* memory,
    usize size, enum MemoryType type, usize alignment
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            internal_ldfree_aligned( memory, size, type, alignment );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            internal_dynamic_allocator_free_aligned(
                allocator->dynamic, memory, size, alignment );
        } break;
        default: UNIMPLEMENTED();
    }
}
LD_API void* internal_allocator_alloc(
    Allocator* allocator, usize size, enum MemoryType type
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            return internal_ldalloc( size, type );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            return internal_dynamic_allocator_alloc_aligned(
                allocator->dynamic, size, 1 );
        } break;
        case ALLOCATOR_TYPE_STACK: {
            return internal_stack_allocator_push(
                allocator->stack, size );
        } break;
        default: UNIMPLEMENTED();
    }
    return NULL;
}
LD_API void internal_allocator_free(
    Allocator* allocator, void* memory,
    usize size, enum MemoryType type
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            internal_ldfree( memory, size, type );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            internal_dynamic_allocator_free_aligned(
                allocator->dynamic, memory, size, 1 );
        } break;
        case ALLOCATOR_TYPE_STACK: {
            // NOTE(alicia): make sure stack is being properly used.
            void* stack_memory_pointer =
                (u8*)(allocator->stack->buffer) + allocator->stack->current - size;
            LOG_ASSERT_NO_TRACE( stack_memory_pointer == memory,
                "Improper use of stack allocator!");
            internal_stack_allocator_pop( allocator->stack, size );
        } break;
        default: UNIMPLEMENTED();
    }
}
LD_API void* internal_allocator_alloc_aligned_trace(
    Allocator* allocator, usize size, enum MemoryType type, usize alignment,
    const char* function, const char* file, int line
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            return internal_ldalloc_aligned_trace(
                size, type, alignment,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            return internal_dynamic_allocator_alloc_aligned_trace(
                allocator->dynamic, size, alignment,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_STACK: {
            LOG_FATAL_CUSTOM_TRACE(
                function, file, line,
                "Stack allocator cannot allocate aligned memory!" );
            PANIC();
        } break;
        default: UNIMPLEMENTED();
    }
    return NULL;
}
LD_API void internal_allocator_free_aligned_trace(
    Allocator* allocator, void* memory,
    usize size, enum MemoryType type, usize alignment,
    const char* function, const char* file, int line
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            internal_ldfree_aligned_trace(
                memory, size, type, alignment,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            internal_dynamic_allocator_free_aligned_trace(
                allocator->dynamic, memory, size, alignment,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_STACK: {
            LOG_FATAL_CUSTOM_TRACE(
                function, file, line,
                "Stack allocator cannot free aligned memory!" );
            PANIC();
        } break;
        default: UNIMPLEMENTED();
    }
}
LD_API void* internal_allocator_alloc_trace(
    Allocator* allocator, usize size, enum MemoryType type,
    const char* function, const char* file, int line
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            return internal_ldalloc_trace(
                size, type, function, file, line );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            return internal_dynamic_allocator_alloc_aligned_trace(
                allocator->dynamic, size, 1,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_STACK: {
            return internal_stack_allocator_push_trace(
                allocator->stack, size,
                function, file, line );
        } break;
        default: UNIMPLEMENTED();
    }
    return NULL;
}
LD_API void internal_allocator_free_trace(
    Allocator* allocator, void* memory, usize size, enum MemoryType type,
    const char* function, const char* file, int line
) {
    switch( allocator->type ) {
        case ALLOCATOR_TYPE_SYSTEM: {
            internal_ldfree_trace(
                memory, size, type,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_DYNAMIC: {
            internal_dynamic_allocator_free_aligned_trace(
                allocator->dynamic, memory, size, 1,
                function, file, line );
        } break;
        case ALLOCATOR_TYPE_STACK: {
            // NOTE(alicia): make sure stack is being properly used.
            void* stack_memory_pointer =
                (u8*)(allocator->stack->buffer) + allocator->stack->current - size;
            LOG_ASSERT_CUSTOM_TRACE(
                function, file, line,
                stack_memory_pointer == memory,
                "Improper use of stack allocator!");
            internal_stack_allocator_pop_trace(
                allocator->stack, size,
                function, file, line );
        } break;
        default: UNIMPLEMENTED();
    }
}

