/**
 * Description:  Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "core/ldmemory.h"
#include "core/ldlog.h"
#include "ldplatform.h"

// TODO(alicia): MSVC VERSION
#define stack_alloc(size) __builtin_alloca(size)

typedef struct {
    u64 usage[MEMORY_TYPE_COUNT];
    u64 page_usage[MEMORY_TYPE_COUNT];
} MemoryUsage;

global MemoryUsage USAGE = {};

LD_API void* internal_ldalloc( usize size, MemoryType type ) {
#if defined(LD_LOGGING)
    if( type == MEMORY_TYPE_UNKNOWN ) {
        LOG_WARN( "Allocating unknown memory!" );
    }
#endif
    void* result = platform_heap_alloc( size );
    if( result ) {
        USAGE.usage[type] += size;
    }
    return result;
}
LD_API void* internal_ldalloc_aligned(
    usize size, MemoryType type, usize alignment
) {
    ASSERT( alignment % 2 == 0 );

    usize aligned_size = size + sizeof(void*) + (alignment - 1);

    void* result = internal_ldalloc( aligned_size, type );
    if( !result ) {
        return result;
    }

    void* ptr = (void**)
        ((usize)( (usize)result + (alignment - 1) + sizeof(void*)) & ~(alignment - 1));

    ((void**) ptr)[-1] = result;

    return ptr;
}
LD_API void* internal_ldrealloc(
    void* memory, usize old_size, usize new_size, MemoryType type
) {
#if defined(LD_LOGGING)
    if( type == MEMORY_TYPE_UNKNOWN ) {
        LOG_WARN( "Allocating unknown memory!" );
    }
#endif
    void* result = platform_heap_realloc( memory, new_size );
    if( result ) {
        usize additional_size = new_size - old_size;
        USAGE.usage[type] += additional_size;
    }
    return result;
}
LD_API void internal_ldfree( void* memory, usize size, MemoryType type ) {
    platform_heap_free( memory );
    USAGE.usage[type] -= size;
}
LD_API void internal_ldfree_aligned(
    void* memory, usize size, MemoryType type, usize alignment
) {
    ASSERT( alignment % 2 == 0 );
    internal_ldfree( ((void**)memory)[-1], size, type );
}

LD_API void* internal_ldalloc_trace(
    usize size, MemoryType type,
    const char* function,
    const char* file,
    int line
) {
    void* result = internal_ldalloc( size, type );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_GREEN,
            LOG_FLAG_NEW_LINE,
            "[ALLOC | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Pointer: {u64,x}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)size, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_RED,
            LOG_FLAG_NEW_LINE,
            "[ALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)size
        );
    }
    return result;
}
LD_API void* internal_ldalloc_aligned_trace(
    usize size, MemoryType type, u8 alignment,
    const char* function,
    const char* file,
    int line
) {
    void* result = internal_ldalloc_aligned( size, type, alignment );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_GREEN,
            LOG_FLAG_NEW_LINE,
            "[ALLOC | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Alignment: {u64} Pointer: {u64,x}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)size, (u64)alignment, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_RED,
            LOG_FLAG_NEW_LINE,
            "[ALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Alignment: {u64}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)size, (u64)alignment
        );
    }
    return result;
}
LD_API void* internal_ldrealloc_trace(
    void* memory, usize old_size, usize new_size, MemoryType type,
    const char* function,
    const char* file,
    int line
) {
    void* result = internal_ldrealloc( memory, old_size, new_size, type );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_GREEN,
            LOG_FLAG_NEW_LINE,
            "[REALLOC | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} -> {u64} Pointer: {u64,x}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)old_size, (u64)new_size, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_RED,
            LOG_FLAG_NEW_LINE,
            "[REALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} -> {u64} Pointer: {u64,x}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)old_size, (u64)new_size, (u64)memory
        );
    }
    return result;
}
LD_API void internal_ldfree_trace(
    void* memory, usize size, MemoryType type,
    const char* function,
    const char* file,
    int line
) {
    internal_ldfree( memory, size, type );
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_CYAN,
        LOG_FLAG_NEW_LINE,
        "[FREE | {cc}() | {cc}:{i}] "
        "{cc} Size: {u64} Pointer: {u64,x}",
        function, file, line,
        memory_type_to_string( type ),
        (u64)size, (u64)memory
    );
}
LD_API void internal_ldfree_aligned_trace(
    void* memory, usize size, MemoryType type, usize alignment,
    const char* function,
    const char* file,
    int line
) {
    internal_ldfree_aligned( memory, size, type, alignment );
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_CYAN,
        LOG_FLAG_NEW_LINE,
        "[FREE | {cc}() | {cc}:{i}] "
        "{cc} Size: {u64} Alignment: {u64} Pointer: {u64,x}",
        function, file, line,
        memory_type_to_string( type ),
        (u64)size, (u64)alignment, (u64)memory
    );
}

LD_API void* internal_ldpage_alloc( usize pages, MemoryType type ) {
    usize byte_size = pages * MEMORY_PAGE_SIZE;
    void* result = platform_page_alloc( byte_size );
    if( result ) {
        USAGE.page_usage[type] += byte_size;
    }
    return result;
}
LD_API void internal_ldpage_free( void* memory, usize pages, MemoryType type ) {
    usize byte_size = pages * MEMORY_PAGE_SIZE;
    USAGE.page_usage[type] -= byte_size;
    platform_page_free( memory );
}
LD_API void* internal_ldpage_alloc_trace(
    usize pages, MemoryType type,
    const char* function,
    const char* file, int line
) {
    void* result = internal_ldpage_alloc( pages, type );
    usize byte_size = pages * MEMORY_PAGE_SIZE;
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_GREEN,
            LOG_FLAG_NEW_LINE,
            "[PAGE ALLOC | {cc}() | {cc}:{i}] "
            "{cc} Pages: {u64} Size: {u64} Pointer: {u64,x}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)pages, (u64)byte_size, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            LOG_COLOR_RED,
            LOG_FLAG_NEW_LINE,
            "[PAGE ALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Pages: {u64} Size: {u64}",
            function, file, line,
            memory_type_to_string( type ),
            (u64)pages, (u64)byte_size
        );
    }
    return result;
}
LD_API void internal_ldpage_free_trace(
    void* memory, usize pages, MemoryType type,
    const char* function,
    const char* file, int line
) {
    internal_ldpage_free( memory, pages, type );
    usize byte_size = pages * MEMORY_PAGE_SIZE;
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        LOG_COLOR_CYAN,
        LOG_FLAG_NEW_LINE,
        "[FREE | {cc}() | {cc}:{i}] "
        "{cc} Pages: {u64} Size: {u64} Pointer: {u64,x}",
        function, file, line,
        memory_type_to_string( type ),
        (u64)pages, (u64)byte_size, (u64)memory
    );
}

LD_API usize query_memory_usage( MemoryType memtype ) {
    return USAGE.usage[memtype] + USAGE.page_usage[memtype];
}
LD_API usize query_total_memory_usage() {\
    usize result = 0;
    for( u64 i = 0; i < MEMORY_TYPE_COUNT; ++i ) {
        result += query_memory_usage( (MemoryType)i );
    }
    return result;
}

LD_API void mem_copy( void* dst, const void* src, usize size ) {
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

LD_API void mem_copy_overlapped( void* dst, const void* src, usize size ) {
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

LD_API void* mem_set( void* dst, int value, usize n ) {
    u8* bytes = (u8*)dst;
    for( usize i = 0; i < n; ++i ) {
        bytes[i] = (u8)value;
    }
    return dst;
}

LD_API void mem_zero( void* dst, usize size ) {
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
