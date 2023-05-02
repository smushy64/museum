/**
 * Description:  Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "memory.h"
#include "logging.h"
#include "platform/os.h"

#define MEMORY_FIELD_SIZE  0
#define MEMORY_FIELD_TYPE  1
#define MEMORY_FIELD_COUNT 2

#define MEMORY_HEADER_SIZE (sizeof(u64) * 2)

struct MemoryUsage {
    u64 usage[MEMTYPE_COUNT];
};
SM_GLOBAL MemoryUsage USAGE = {};

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

namespace internal {

void* impl_mem_alloc_trace(
    usize size,
    MemoryType type,
    const char* function,
    const char* file,
    int line
) {
    void* result = impl_mem_alloc( size, type );
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
void* impl_mem_realloc_trace(
    void* memory,
    usize new_size,
    const char* function,
    const char* file,
    int line
) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];
    void* result = impl_mem_realloc( memory, new_size );
    LOG_ALLOC(
        function,
        file,
        line,
        "Realloc | Type: %s | Size: %llu | Pointer: 0x%X",
        to_string(type),
        new_size,
        (u64)result
    );
    return impl_mem_realloc( memory, new_size );
}
void impl_mem_free_trace(
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

    impl_mem_free( memory );
}

void* impl_mem_alloc( usize size, MemoryType type ) {
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

    USAGE.usage[type] += size;

    return block + MEMORY_FIELD_COUNT;
}
void* impl_mem_realloc( void* memory, usize new_size ) {
    // TODO(alicia): allocate in different ways depending on memory type

    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    usize old_size   = header[MEMORY_FIELD_SIZE];
    MemoryType type  = (MemoryType)header[MEMORY_FIELD_TYPE];

    usize total_size = new_size + MEMORY_HEADER_SIZE;
    void* new_buffer = heap_realloc( header, total_size );
    if( !new_buffer ) {
        return nullptr;
    }
    
    header = (u64*)new_buffer;
    header[MEMORY_FIELD_SIZE] = new_size;

    if( new_size > old_size ) {
        usize diff  = new_size - old_size;
        USAGE.usage[type] += diff;
    }

    return header + MEMORY_FIELD_COUNT;
}
void impl_mem_free( void* memory ) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;

    usize      size = header[MEMORY_FIELD_SIZE];
    MemoryType type = (MemoryType)header[MEMORY_FIELD_TYPE];

    USAGE.usage[type] -= size;

    heap_free( header );
}

}

usize query_memory_usage( MemoryType memtype ) {
    return USAGE.usage[memtype];
}

usize mem_query_size( void* memory ) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    return header[MEMORY_FIELD_SIZE];
}
MemoryType mem_query_type( void* memory ) {
    u64* header = ((u64*)memory) - MEMORY_FIELD_COUNT;
    return (MemoryType)header[MEMORY_FIELD_TYPE];
}

void mem_copy(
    void* dst,
    const void* src,
    usize size
) {
    if( size % sizeof(u64) == 0 ) {
        u64* src_64   = (u64*)src;
        u64* dst_64   = (u64*)dst;
        usize size_64 = size / sizeof(u64);
        for( usize i = 0; i < size_64; ++i ) {
            dst_64[i] = src_64[i];
        }
    } else if( size % sizeof(u32) == 0 ) {
        u32* src_32   = (u32*)src;
        u32* dst_32   = (u32*)dst;
        usize size_32 = size / sizeof(u32);
        for( usize i = 0; i < size_32; ++i ) {
            dst_32[i] = src_32[i];
        }
    } else if( size % sizeof(u16) == 0 ) {
        u16* src_16   = (u16*)src;
        u16* dst_16   = (u16*)dst;
        usize size_16 = size / sizeof(u16);
        for( usize i = 0; i < size_16; ++i ) {
            dst_16[i] = src_16[i];
        }
    } else {
        u8* src_8   = (u8*)src;
        u8* dst_8   = (u8*)dst;
        for( usize i = 0; i < size; ++i ) {
            dst_8[i] = src_8[i];
        }
    }
}

void mem_overlap_copy( void* dst, const void* src, usize size ) {
    u8 intermediate[size];
    mem_copy( intermediate, src, size );
    mem_copy( dst, intermediate, size );
}

void mem_set( u8 value, usize dst_size, void* dst ) {
    u8* bytes = (u8*)dst;
    for( usize i = 0; i < dst_size; ++i ) {
        bytes[i] = value;
    }
}

void mem_zero( void* ptr, usize size ) {
    if( size % sizeof(u64) == 0 ) {
        u64* ptr_64   = (u64*)ptr;
        usize size_64 = size / sizeof(u64);
        for( usize i = 0; i < size_64; ++i ) {
            ptr_64[i] = 0ULL;
        }
    } else if( size % sizeof(u32) == 0 ) {
        u32* ptr_32   = (u32*)ptr;
        usize size_32 = size / sizeof(u32);
        for( usize i = 0; i < size_32; ++i ) {
            ptr_32[i] = 0;
        }
    } else if( size % sizeof(u16) == 0 ) {
        u16* ptr_16   = (u16*)ptr;
        usize size_16 = size / sizeof(u16);
        for( usize i = 0; i < size_16; ++i ) {
            ptr_16[i] = 0;
        }
    } else {
        u8* ptr_8 = (u8*)ptr;
        for( usize i = 0; i < size; ++i ) {
            ptr_8[i] = 0;
        }
    }
}