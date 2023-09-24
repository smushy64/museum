/**
 * Description:  Memory Functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "core/mem.h"
#include "core/log.h"
#include "internal.h"

#if !defined(PLATFORM_MEMORY_PAGE_SIZE)
    #define PLATFORM_MEMORY_PAGE_SIZE (kilobytes(4))
#endif

usize MEMORY_PAGE_SIZE = PLATFORM_MEMORY_PAGE_SIZE;

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
    void* result = platform->memory.heap_alloc( size );
    if( result ) {
        USAGE.usage[type] += size;
    }
    return result;
}
LD_API void* internal_ldalloc_aligned(
    usize size, MemoryType type, usize alignment
) {
    assert( alignment % 2 == 0 );

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
    void* result = platform->memory.heap_realloc(
        memory, old_size, new_size );
    if( result ) {
        usize additional_size = new_size - old_size;
        USAGE.usage[type] += additional_size;
    }
    return result;
}
LD_API void internal_ldfree( void* memory, usize size, MemoryType type ) {
    platform->memory.heap_free( size, memory );
    USAGE.usage[type] -= size;
}
LD_API void internal_ldfree_aligned(
    void* memory, usize size, MemoryType type, usize alignment
) {
    assert( alignment % 2 == 0 );
    usize aligned_size = size + sizeof(void*) + (alignment - 1);
    internal_ldfree( ((void**)memory)[-1], aligned_size, type );
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
            false, true,
            LOG_COLOR_GREEN
            "[ALLOC | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            memory_type_to_string( type ),
            (u64)size, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[ALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64}"
            LOG_COLOR_RESET,
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
            false, true,
            LOG_COLOR_GREEN
            "[ALLOC | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Alignment: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            memory_type_to_string( type ),
            (u64)size, (u64)alignment, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[ALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} Alignment: {u64}"
            LOG_COLOR_RESET,
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
            false, true,
            LOG_COLOR_GREEN
            "[REALLOC | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} -> {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            memory_type_to_string( type ),
            (u64)old_size, (u64)new_size, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[REALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Size: {u64} -> {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
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
        false, true,
        LOG_COLOR_CYAN
        "[FREE | {cc}() | {cc}:{i}] "
        "{cc} Size: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
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
        false, true,
        LOG_COLOR_CYAN
        "[FREE | {cc}() | {cc}:{i}] "
        "{cc} Size: {u64} Alignment: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
        function, file, line,
        memory_type_to_string( type ),
        (u64)size, (u64)alignment, (u64)memory
    );
}

LD_API void* internal_ldpage_alloc( usize pages, MemoryType type ) {
    usize byte_size = pages * MEMORY_PAGE_SIZE;
    void* result = platform->memory.page_alloc( byte_size );
    if( result ) {
        USAGE.page_usage[type] += byte_size;
    }
    return result;
}
LD_API void internal_ldpage_free( void* memory, usize pages, MemoryType type ) {
    usize byte_size = pages * MEMORY_PAGE_SIZE;
    USAGE.page_usage[type] -= byte_size;

    platform->memory.page_free( byte_size, memory );
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
            false, true,
            LOG_COLOR_GREEN
            "[PAGE ALLOC | {cc}() | {cc}:{i}] "
            "{cc} Pages: {u64} Size: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            memory_type_to_string( type ),
            (u64)pages, (u64)byte_size, (u64)result
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[PAGE ALLOC FAILED | {cc}() | {cc}:{i}] "
            "{cc} Pages: {u64} Size: {u64}"
            LOG_COLOR_RESET,
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
        false, true,
        LOG_COLOR_CYAN
        "[FREE | {cc}() | {cc}:{i}] "
        "{cc} Pages: {u64} Size: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
        function, file, line,
        memory_type_to_string( type ),
        (u64)pages, (u64)byte_size, (u64)memory
    );
}

LD_API usize query_memory_usage( MemoryType memtype ) {
    return USAGE.usage[memtype] + USAGE.page_usage[memtype];
}
LD_API usize query_total_memory_usage(void) {\
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
    #define INTERMEDIATE_BUFFER_SIZE (256ULL)
    u8 buf[INTERMEDIATE_BUFFER_SIZE];
    void* intermediate_buffer = buf;

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
LD_API b32 mem_cmp( void* a, void* b, usize max_size ) {
    usize count_64 = max_size / sizeof(u64);
    for( usize i = 0; i < count_64; ++i ) {
        if( *((u64*)a + i) != *((u64*)b + i) ) {
            return false;
        }
    }

    usize remainder = max_size % sizeof(u64);
    u8* a_remainder = (u8*)((u64*)a + count_64);
    u8* b_remainder = (u8*)((u64*)b + count_64);
    for( usize i = 0; i < remainder; ++i ) {
        if( *(a_remainder + i) != *(b_remainder + i) ) {
            return false;
        }
    }

    return true;
}

