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
    return _mem_realloc( memory, new_size );
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
            "Allocating memory of type unknown!"
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

inline void mem_copy_bytes( void* dst, const void* src, usize size ) {
    u8* end_dst = (u8*)dst + size;
    u8* end_src = (u8*)src + size;
    while( end_dst != dst ) {
        *end_dst-- = *end_src--;
    }
}

void mem_copy( void* dst, const void* src, usize size ) {
    if( size < sizeof(u64) ) {
        mem_copy_bytes( dst, src, size );
        return;
    }
    
    u64* src64 = (u64*)src;
    u64* dst64 = (u64*)dst;
    usize count64 = size / sizeof(u64);
    for( usize i = 0; i < count64; ++i ) {
        dst64[i] = src64[i];
    }

    usize remainder = size % sizeof(u64);
    if( remainder ) {
        mem_copy_bytes(
            (u8*)dst + (size - remainder - 1),
            (u8*)src + (size - remainder - 1),
            remainder
        );
    }
}

#define INTERMEDIATE_BUFFER_SIZE 128
global u8 INTERMEDIATE_BUFFER[INTERMEDIATE_BUFFER_SIZE] = {};
void mem_overlap_copy( void* dst, const void* src, usize size ) {
    if( size < INTERMEDIATE_BUFFER_SIZE ) {
        mem_copy( INTERMEDIATE_BUFFER, src, size );
        mem_copy( dst, INTERMEDIATE_BUFFER, size );
    }
    usize remainder  = size % INTERMEDIATE_BUFFER_SIZE;
    usize iterations = size / INTERMEDIATE_BUFFER_SIZE;

    u8* src_ptr = (u8*)src;
    u8* dst_ptr = (u8*)dst;
    for( usize i = 0; i < iterations; ++i ) {
        mem_copy(
            INTERMEDIATE_BUFFER,
            src_ptr + (i * INTERMEDIATE_BUFFER_SIZE),
            INTERMEDIATE_BUFFER_SIZE
        );
        mem_copy(
            dst_ptr + (i * INTERMEDIATE_BUFFER_SIZE),
            INTERMEDIATE_BUFFER,
            INTERMEDIATE_BUFFER_SIZE
        );
    }

    src_ptr = src_ptr + (iterations * INTERMEDIATE_BUFFER_SIZE);
    dst_ptr = dst_ptr + (iterations * INTERMEDIATE_BUFFER_SIZE);
    for( usize i = 0; i < remainder; ++i ) {
        INTERMEDIATE_BUFFER[i] = src_ptr[i];
    }
    for( usize i = 0; i < remainder; ++i ) {
        dst_ptr[i] = INTERMEDIATE_BUFFER[i];
    }

}

void mem_set( u8 value, usize dst_size, void* dst ) {
    u8* bytes = (u8*)dst;
    for( usize i = 0; i < dst_size; ++i ) {
        bytes[i] = value;
    }
}

void mem_zero( void* ptr, usize size ) {
    if( size < 8 ) {
        u8* end = (u8*)ptr + size;
        while( end != ptr ) {
            *end-- = 0;
        }
        return;
    }
    
    usize remainder  = size % sizeof(u64);
    usize long_size  = size - remainder;
    usize long_count = long_size / sizeof(u64);
    u64*  long_end   = (u64*)ptr + long_count;

    while( long_count-- > 0 ) {
        *long_end-- = 0ULL;
    }

    u8* byte_end = (u8*)ptr + remainder;

    while( remainder-- > 0 ) {
        *byte_end-- = 0;
    }

}
