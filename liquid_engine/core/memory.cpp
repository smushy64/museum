/**
 * Description:  Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "memory.h"
#include "logging.h"
#include "platform/platform.h"

#define MEMORY_FIELD_SIZE  0
#define MEMORY_FIELD_TYPE  1
#define MEMORY_FIELD_COUNT 2

#define MEMORY_HEADER_SIZE (sizeof(u64) * 2)

// TODO(alicia): MSVC VERSION
#define stack_alloc(size) __builtin_alloca(size)

struct MemoryUsage {
    u64 usage[MEMTYPE_COUNT];
    u64 page_usage[MEMTYPE_COUNT];
};
global MemoryUsage USAGE = {};

#define LOG_ALLOC( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_GREEN,\
        LOG_FLAG_NEW_LINE,\
        "[ALLOC | {cc}() | {cc}:{i}] " format,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

#define LOG_FREE( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_CYAN,\
        LOG_FLAG_NEW_LINE,\
        "[FREE  | {cc}() | {cc}:{i}] " format,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

#define LOG_PAGE_ALLOC( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_GREEN,\
        LOG_FLAG_NEW_LINE,\
        "[PAGE ALLOC | {cc}() | {cc}:{i}] " format,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

#define LOG_PAGE_FREE( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_CYAN,\
        LOG_FLAG_NEW_LINE,\
        "[PAGE FREE  | {cc}() | {cc}:{i}] " format,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

namespace impl {

void* _mem_alloc_trace(
    usize size,
    MemoryType type,
    const char* function,
    const char* file,
    int line
) {
    void* result = _mem_alloc( size, type );
    f32 float_size = (f32)size;
    LOG_ALLOC(
        function,
        file,
        line,
        "Type: {cc} | Size: {u64} ({f,b,.2}) | Pointer: {u64,x}",
        to_string(type),
        size,
        float_size,
        (u64)(result)
    );
    return result;
}
void* _mem_realloc_trace(
    void* memory,
    usize new_size,
    const char* function,
    const char* file,
    int line
) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];
    void* result = _mem_realloc( memory, new_size );
    f32 float_new_size = (f32)new_size;
    LOG_ALLOC(
        function,
        file,
        line,
        "Realloc | Type: {cc} | Size: {u64} ({f,b,.2}) | Pointer: {u64,x}",
        to_string(type),
        new_size, float_new_size,
        (u64)(result)
    );
    return result;
}
void _mem_free_trace(
    void* memory,
    const char* function,
    const char* file,
    int line
) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];
    usize size = header[MEMORY_FIELD_SIZE];
    f32 float_size = (f32)size;

    LOG_FREE(
        function,
        file,
        line,
        "Type: {cc} | Size: {u64} ({f,b,.2}) | Pointer: {u64,x}",
        to_string(type),
        size, float_size,
        (u64)(memory)
    );

    _mem_free( memory );
}

void* _mem_alloc( usize size, MemoryType type ) {
    if( type == MEMTYPE_UNKNOWN ) {
        LOG_WARN(
            "Allocating memory of type unknown! "
            "All memory allocations should be categorized!"
        );
    }

#if defined(LD_PROFILING)
    usize total_size = size + MEMORY_HEADER_SIZE;
    u64*  block      = (u64*)heap_alloc( total_size );

    block[MEMORY_FIELD_SIZE] = size;
    block[MEMORY_FIELD_TYPE] = type;

    USAGE.usage[type] += total_size;

    return block + MEMORY_FIELD_COUNT;
#else
    return heap_alloc( size );
#endif
}
void* _mem_realloc( void* memory, usize new_size ) {
#if defined( LD_PROFILING )
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    usize old_size   = header[MEMORY_FIELD_SIZE] + MEMORY_HEADER_SIZE;
    MemoryType type  = (MemoryType)header[MEMORY_FIELD_TYPE];

    usize total_size = new_size + MEMORY_HEADER_SIZE;
    void* new_buffer = heap_realloc( header, total_size );
    if( !new_buffer ) {
        return nullptr;
    }
    
    header = (u64*)new_buffer;
    header[MEMORY_FIELD_SIZE] = new_size;

    if( total_size > old_size ) {
        usize diff  = total_size - old_size;
        USAGE.usage[type] += diff;
    }

    return header + MEMORY_FIELD_COUNT;
#else
    return heap_realloc( memory, new_size );
#endif
}
void _mem_free( void* memory ) {
#if defined(LD_PROFILING)
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;

    usize      size = header[MEMORY_FIELD_SIZE];
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];

    USAGE.usage[type] -= size + MEMORY_HEADER_SIZE;

    heap_free( header );
#else
    heap_free( memory );
#endif
}

#if defined(LD_PLATFORM_WINDOWS)
    #define PAGE_SIZE KILOBYTES(4)
#else
    #define PAGE_SIZE KILOBYTES(4)
#endif

internal void* internal_mem_page_alloc( usize size, MemoryType type, usize* out_actual_size ) {
#if defined(LD_PROFILING)
    if( type == MEMTYPE_UNKNOWN ) {
        LOG_WARN(
            "Allocating memory of type unknown! "
            "All memory allocations should be categorized!"
        );
    }

    usize total_size            = size + ( size % PAGE_SIZE );
    usize size_plus_header_size = total_size + MEMORY_HEADER_SIZE;
    if( out_actual_size ) {
        *out_actual_size = size_plus_header_size;
    }

    u64* block = (u64*)platform_page_alloc( size_plus_header_size );

    block[MEMORY_FIELD_SIZE] = total_size;
    block[MEMORY_FIELD_TYPE] = type;

    USAGE.page_usage[type] += total_size;

    return block + MEMORY_FIELD_COUNT;
#else
    if( type == MEMTYPE_UNKNOWN ) {
        LOG_WARN(
            "Allocating memory of type unknown! "
            "All memory allocations should be categorized!"
        );
    }

    usize total_size = size + ( size % PAGE_SIZE );
    if( out_actual_size ) {
        *out_actual_size = total_size;
    }

    return platform_page_alloc( total_size );
#endif
}

LD_API void* _mem_page_alloc( usize size, MemoryType type ) {
    return internal_mem_page_alloc( size, type, nullptr );
}
LD_API void _mem_page_free( void* memory ) {
#if defined(LD_PROFILING)
    
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;

    usize      size = header[MEMORY_FIELD_SIZE];
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];

    USAGE.page_usage[type] -= size;
    platform_page_free( header );
#else
    platform_page_free( memory );
#endif
}

LD_API void* _mem_page_alloc_trace(
    usize size, MemoryType type,
    const char* function,
    const char* file,
    int line
) {
    usize actual_size = 0;
    void* result = internal_mem_page_alloc( size, type, &actual_size );
    f32 float_actual_size = (f32)actual_size;
    LOG_PAGE_ALLOC(
        function,
        file,
        line,
        "Type: {cc} | Size: {u64} ({f,b,.2}) | Pointer: {u64,x}",
        to_string(type),
        actual_size, float_actual_size,
        (u64)(result)
    );
    return result;   
}
LD_API void _mem_page_free_trace(
    void* memory,
    const char* function,
    const char* file,
    int line
) {
#if defined(LD_PROFILING)
    u64*       header     = ((u64*)memory) - MEMORY_FIELD_COUNT;
    MemoryType type       = (MemoryType)header[MEMORY_FIELD_TYPE];
    usize      size       = header[MEMORY_FIELD_SIZE];
    f32        float_size = (f32)size;

    LOG_PAGE_FREE(
        function,
        file,
        line,
        "Type: {cc} | Size: {u64}({f,b,.2}) | Pointer: {u64,x}",
        to_string(type),
        size, float_size,
        (u64)(memory)
    );
#else
    LOG_PAGE_FREE(
        function,
        file,
        line,
        "Pointer: {u64,x}",
        (u64)(memory)
    );
#endif

    _mem_page_free( memory );
}

LD_API b32 _stack_arena_create( u32 size, MemoryType type, StackArena* out_arena ) {
    void* buffer = _mem_page_alloc( size, type );
    if( !buffer ) {
        return false;
    }

    out_arena->arena         = buffer;
    out_arena->arena_size    = size;
    out_arena->stack_pointer = 0;

    return true;
}
LD_API void _stack_arena_free( StackArena* arena ) {
    if( arena->arena ) {
        _mem_page_free( arena->arena );
    }
    *arena = {};
}
LD_API void* _stack_arena_push_item( StackArena* arena, u32 item_size ) {
    u32 new_stack_pointer = arena->stack_pointer + item_size;
    if( new_stack_pointer >= arena->arena_size ) {
        u32 remaining_stack_size = arena->arena_size - arena->stack_pointer;
        f32 float_remaining_stack_size = (f32)remaining_stack_size;
        f32 float_item_size = (f32)item_size;
        LOG_ERROR(
            "Stack Arena push failed! Item Size: {u} ({f,b,.2}) | Remaining Stack: {u} ({f,b,.2})",
            item_size, float_item_size,
            remaining_stack_size, float_remaining_stack_size
        );
        return nullptr;
    }

    void* result = (u8*)arena->arena + arena->stack_pointer;
    arena->stack_pointer = new_stack_pointer;

    return result;
}
LD_API void _stack_arena_pop_item( StackArena* arena, u32 item_size ) {
    ASSERT( arena->stack_pointer );
    ASSERT( arena->stack_pointer >= item_size );
    arena->stack_pointer -= item_size;
}
LD_API b32 _stack_arena_create_trace(
    u32 size, MemoryType type,
    StackArena* out_arena,
    const char* function,
    const char* file,
    int line
) {
    b32 success = true;
    void* buffer = _mem_page_alloc_trace( size, type, function, file, line );
    if( !buffer ) {
        success = false;
    }

    out_arena->arena         = buffer;
    out_arena->arena_size    = size;
    out_arena->stack_pointer = 0;

    f32 float_size = (f32)size;
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_GREEN,
        LOG_FLAG_NEW_LINE,
        "[STACK ARENA ALLOC | {cc}() | {cc}:{i}] "
        "Arena: {u64,x} | Type: {cc} | Size: {u} ({f,b,.2})",
        function, file, line,
        (u64)(out_arena->arena),
        to_string( type ),
        size, float_size
    );
    return success;
}
LD_API void _stack_arena_free_trace(
    StackArena* arena,
    const char* function,
    const char* file,
    int line
) {
    u64* header = ((u64*)arena->arena) - MEMORY_FIELD_COUNT;
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];
    f32 float_arena_size = (f32)arena->arena_size;
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_CYAN,
        LOG_FLAG_NEW_LINE,
        "[STACK ARENA FREE | {cc}() | {cc}:{i}] "
        "Arena: {u64,x} | Type: {cc} | Size: {u} ({f,b,.2})",
        function, file, line,
        (u64)(arena->arena),
        to_string( type ),
        arena->arena_size,
        float_arena_size
    );
    _stack_arena_free( arena );
}
LD_API void* _stack_arena_push_item_trace(
    StackArena* arena, u32 item_size,
    const char* function,
    const char* file,
    int line
) {
    f32 float_item_size = (f32)item_size;
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_GREEN,
        LOG_FLAG_NEW_LINE,
        "[STACK ARENA PUSH | {cc}() | {cc}:{i}] "
        "Arena: {u64,x} | Item Size: {u} ({f,b,.2})",
        function, file, line,
        (u64)(arena->arena),
        item_size, float_item_size
    );
    return _stack_arena_push_item( arena, item_size );
}
LD_API void _stack_arena_pop_item_trace(
    StackArena* arena, u32 item_size,
    const char* function,
    const char* file,
    int line
) {
    f32 float_item_size = (f32)item_size;
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_CYAN,
        LOG_FLAG_NEW_LINE,
        "[STACK ARENA POP | {cc}() | {cc}:{i}] "
        "Arena: {u64,x} | Item Size: {u} ({f,b,.2})",
        function, file, line,
        (u64)(arena->arena),
        item_size, float_item_size
    );
    _stack_arena_pop_item( arena, item_size );
}

} // namespace impl


usize query_memory_usage( MemoryType memtype ) {
    return USAGE.usage[memtype] + USAGE.page_usage[memtype];
}
usize query_total_memory_usage() {\
    usize result = 0;
    for( u64 i = 0; i < MEMTYPE_COUNT; ++i ) {
        result += query_memory_usage( (MemoryType)i );
    }
    return result;
}

usize mem_query_size( void* memory ) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    return header[MEMORY_FIELD_SIZE];
}
MemoryType mem_query_type( void* memory ) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    return (MemoryType)header[MEMORY_FIELD_TYPE];
}

void mem_copy( void* dst, const void* src, usize size ) {
    usize count64 = size / sizeof(u64);
    for( usize i = 0; i < count64; ++i ) {
        *((u64*)dst + i) = *((u64*)src + i);
    }

    usize remainder = size % sizeof(u64);
    u8* src_remainder = (u8*)((u64*)src + count64);
    u8* dst_remainder = (u8*)((u64*)dst + count64);
    for( usize i = 0; i < remainder; ++i ) {
        *(dst_remainder + i) = *(src_remainder + i);
    }
}

void mem_copy_overlapped( void* dst, const void* src, usize size ) {
    #define INTERMEDIATE_BUFFER_SIZE 256ULL
    void* intermediate_buffer = stack_alloc( INTERMEDIATE_BUFFER_SIZE );

    if( size <= INTERMEDIATE_BUFFER_SIZE ) {
        mem_copy( intermediate_buffer, src, size );
        mem_copy( dst, intermediate_buffer, size );
        return;
    }

    usize iteration_count = size / INTERMEDIATE_BUFFER_SIZE;
    usize remaining_bytes = size % INTERMEDIATE_BUFFER_SIZE;

    for( usize i = 0; i < iteration_count; ++i ) {
        usize offset = i * INTERMEDIATE_BUFFER_SIZE;
        mem_copy(
            intermediate_buffer,
            (u8*)src + offset,
            INTERMEDIATE_BUFFER_SIZE
        );
        mem_copy(
            (u8*)dst + offset,
            intermediate_buffer,
            INTERMEDIATE_BUFFER_SIZE
        );
    }

    if( remaining_bytes ) {
        usize offset = (iteration_count * INTERMEDIATE_BUFFER_SIZE);
        mem_copy( intermediate_buffer, (u8*)src + offset, remaining_bytes );
        mem_copy( (u8*)dst + offset, intermediate_buffer, remaining_bytes );
    }

}

void mem_set( u8 value, usize dst_size, void* dst ) {
    u8* bytes = (u8*)dst;
    for( usize i = 0; i < dst_size; ++i ) {
        bytes[i] = value;
    }
}

void mem_zero( void* dst, usize size ) {
    usize count64 = size / sizeof(u64);
    for( usize i = 0; i < count64; ++i ) {
        *((u64*)dst + i) = 0ULL;
    }

    usize remainder = size % sizeof(u64);
    u8* dst_remainder = (u8*)((u64*)dst + count64);
    for( usize i = 0; i < remainder; ++i ) {
        *(dst_remainder + i) = 0;
    }

}
