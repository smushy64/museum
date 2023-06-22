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
};
global MemoryUsage USAGE = {};

#define LOG_ALLOC( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_GREEN,\
        LOG_FLAG_NEW_LINE,\
        "[ALLOC | %s() | %s:%i] " format,\
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
        "[FREE  | %s() | %s:%i] " format,\
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
    LOG_ALLOC(
        function,
        file,
        line,
        "Type: %s | Size: %llu | Pointer: 0x%X",
        to_string(type),
        size,
        (u64)result
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
    LOG_ALLOC(
        function,
        file,
        line,
        "Realloc | Type: %s | Size: %llu | Pointer: 0x%X",
        to_string(type),
        new_size,
        (u64)result
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

    LOG_FREE(
        function,
        file,
        line,
        "Type: %s | Size: %llu | Pointer: 0x%X",
        to_string(type),
        size,
        (u64)memory
    );

    _mem_free( memory );
}

void* _mem_alloc( usize size, MemoryType type ) {
    // TODO(alicia): allocate in different ways depending on memory type
    
    if( type == MEMTYPE_UNKNOWN ) {
        LOG_WARN(
            "Allocating memory of type unknown! "
            "All memory allocations should be categorized!"
        );
    }

    usize total_size = size + MEMORY_HEADER_SIZE;
    u64*  block      = (u64*)heap_alloc( total_size );

    block[MEMORY_FIELD_SIZE] = size;
    block[MEMORY_FIELD_TYPE] = type;

    USAGE.usage[type] += total_size;

    return block + MEMORY_FIELD_COUNT;
}
void* _mem_realloc( void* memory, usize new_size ) {
    // TODO(alicia): allocate in different ways depending on memory type

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
}
void _mem_free( void* memory ) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;

    usize      size = header[MEMORY_FIELD_SIZE];
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];

    USAGE.usage[type] -= size + MEMORY_HEADER_SIZE;

    heap_free( header );
}

} // namespace impl

usize query_memory_usage( MemoryType memtype ) {
    return USAGE.usage[memtype];
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
