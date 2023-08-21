/**
 * Description:  Collections Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "core/ldcollections.h"
#include "core/ldmemory.h"
#include "core/ldlog.h"
#include "core/ldstring.h"

LD_API u64 hash( StringView sv ) {
    local const u64 multiplier = 97;

    u64 result = 0;
    for( usize i = 0; i < sv.len; ++i ) {
        result = result * multiplier + sv.str[i];
    }

    result %= sv.len;

    return result;
}

#define LIST_NUM_FIELDS 3
#define LIST_FIELDS_SIZE (LIST_NUM_FIELDS * sizeof(u64))
#define LIST_REALLOC_FACTOR 2

#define BUFFER_TO_BASE_POINTER( list ) \
    (((u64*)list) - LIST_NUM_FIELDS)

#define BASE_TO_BUFFER_POINTER( base )\
    (((u64*)base) + LIST_NUM_FIELDS)

#define LOG_LIST_ALLOC( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        false, true,\
        LOG_COLOR_GREEN\
        "[LIST ALLOC | {cc}() | {cc}:{i}] " format\
        LOG_COLOR_RESET,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

#define LOG_LIST_REALLOC( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        false, true,\
        LOG_COLOR_GREEN\
        "[LIST REALLOC | {cc}() | {cc}:{i}] " format\
        LOG_COLOR_RESET,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

#define LOG_LIST_FREE( function, file, line, format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        false, true,\
        LOG_COLOR_CYAN\
        "[LIST FREE | {cc}() | {cc}:{i}] " format\
        LOG_COLOR_RESET,\
        function,\
        file,\
        line,\
        ##__VA_ARGS__\
    )

LD_API void* _list_create_trace(
    usize capacity,
    usize stride,
    const char* function,
    const char* file,
    int line
) {
    void* result = _list_create( capacity, stride );

    LOG_LIST_ALLOC(
        function,
        file,
        line,
        "Stride: {u64} | Capacity: {u64} | Size: {u64} | Pointer: {u,x}",
        stride,
        capacity,
        stride * capacity,
        (u64)result
    );

    return result;
}

LD_API void* _list_realloc_trace(
    void* list,
    usize new_capacity,
    const char* function,
    const char* file,
    int line
) {
    u64*  base         = BUFFER_TO_BASE_POINTER(list);
    usize old_capacity = base[LIST_FIELD_CAPACITY];
    usize stride       = base[LIST_FIELD_STRIDE];

    void* result = _list_realloc( list, new_capacity );
    LOG_LIST_REALLOC(
        function,
        file,
        line,
        "Old Capacity: {u64} | New Capacity: {u64} | "
        "Old Size: {u64} | New Size: {u64} | "
        "Pointer: {u,x}",
        old_capacity,
        new_capacity,
        stride * old_capacity,
        stride * new_capacity,
        (u64)result
    );
    return result;
}

LD_API void _list_free_trace(
    void* list,
    const char* function,
    const char* file,
    int line
) {
    u64* base = BUFFER_TO_BASE_POINTER(list);
    usize stride   = base[LIST_FIELD_STRIDE];
    usize capacity = base[LIST_FIELD_CAPACITY];

    LOG_LIST_FREE(
        function,
        file,
        line,
        "Capacity: {u64} | Size: {u64} | Pointer: {u,x}",
        capacity,
        stride * capacity,
        list
    );

    _list_free( list );
}

LD_API void* _list_create( usize capacity, usize stride ) {
    usize total_size = (capacity * stride) + LIST_FIELDS_SIZE;
    u64* base = (u64*)internal_ldalloc(
        total_size,
        MEMORY_TYPE_DYNAMIC_LIST
    );
    if( !base ) {
        return NULL;
    }

    base[LIST_FIELD_CAPACITY] = capacity;
    base[LIST_FIELD_COUNT]    = 0;
    base[LIST_FIELD_STRIDE]   = stride;

    return BASE_TO_BUFFER_POINTER(base);
}
LD_API void  _list_free( void* list ) {
    if( !list ) {
        return;
    }
    u64* base = BUFFER_TO_BASE_POINTER(list);
    u64 capacity = base[LIST_FIELD_CAPACITY];
    u64 stride   = base[LIST_FIELD_STRIDE];
    usize size = capacity * stride + LIST_FIELDS_SIZE;
    internal_ldfree( base, size, MEMORY_TYPE_DYNAMIC_LIST );
}

LD_API void* _list_realloc( void* list, usize new_capacity ) {
    u64* base = BUFFER_TO_BASE_POINTER(list);

    usize capacity = base[LIST_FIELD_CAPACITY];
    usize stride   = base[LIST_FIELD_STRIDE];
    usize new_size = (new_capacity * stride) + LIST_FIELDS_SIZE;
    usize old_size = (capacity * stride) + LIST_FIELDS_SIZE;
    u64*  new_base = (u64*)internal_ldrealloc(
        base,
        old_size, new_size,
        MEMORY_TYPE_DYNAMIC_LIST
    );

    if( !new_base ) {
        LOG_ERROR("Failed to realloc list!");
        return list;
    }

    new_base[LIST_FIELD_CAPACITY] = new_capacity;

    return BASE_TO_BUFFER_POINTER(new_base);
}

LD_API void* _list_append(
    void* list,
    usize append_count,
    const void* pvalue
) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize count    = base[LIST_FIELD_COUNT];
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( count + append_count > capacity ) {
        list = _list_realloc(
            list,
            count + append_count
        );
        base = BUFFER_TO_BASE_POINTER(list);
    }

    usize append_size = stride * append_count;
    mem_copy(
        (u8*)list + (count * stride),
        pvalue,
        append_size
    );

    base[LIST_FIELD_COUNT] = count + append_count;
    return list;
}

LD_API void* _list_append_trace(
    void* list,
    usize append_count,
    const void* pvalue,
    const char* function,
    const char* file,
    int line
) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize count    = base[LIST_FIELD_COUNT];
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( count + append_count > capacity ) {
        list = _list_realloc_trace(
            list,
            count + append_count,
            function,
            file,
            line
        );
        base = BUFFER_TO_BASE_POINTER(list);
    }

    usize append_size = stride * append_count;
    mem_copy(
        (u8*)list + (count * stride),
        pvalue,
        append_size
    );

    base[LIST_FIELD_COUNT] = count + append_count;
    return list;
}

LD_API void* _list_push( void* list, const void* pvalue ) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize count    = base[LIST_FIELD_COUNT];
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( count == capacity ) {
        list = _list_realloc(
            list,
            capacity * LIST_REALLOC_FACTOR
        );
        base  = BUFFER_TO_BASE_POINTER(list);
        count = base[LIST_FIELD_COUNT];
    }
    u8* bytes = (u8*)list;

    usize index = stride * count;
    
    mem_copy( bytes + index, pvalue, stride );

    base[LIST_FIELD_COUNT] += 1;

    return list;
}

void* _list_push_trace(
    void* list,
    const void* pvalue,
    const char* function,
    const char* file,
    int line
) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize count    = base[LIST_FIELD_COUNT];
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( count == capacity ) {
        list = _list_realloc_trace(
            list,
            capacity * LIST_REALLOC_FACTOR,
            function,
            file,
            line
        );
        base  = BUFFER_TO_BASE_POINTER(list);
        count = base[LIST_FIELD_COUNT];
    }
    u8* bytes = (u8*)list;

    usize index = stride * count;
    
    mem_copy( bytes + index, pvalue, stride );

    base[LIST_FIELD_COUNT] += 1;

    return list;
}

LD_API b32 _list_pop( void* list, void* dst ) {
    u64*  base   = BUFFER_TO_BASE_POINTER(list);
    usize count  = base[LIST_FIELD_COUNT];
    usize stride = base[LIST_FIELD_STRIDE];

    if( count == 0 ) {
        return false;
    }
    u8* bytes = (u8*)list;

    mem_copy( dst, bytes + count, stride );
    base[LIST_FIELD_COUNT] -= 1;

    return true;
}

LD_API usize _list_field_read( void* list, u32 field ) {
    u64*   base = BUFFER_TO_BASE_POINTER(list);
    return base[field];
}
LD_API void _list_field_write(
    void* list,
    u32 field,
    usize value
) {
    u64* base   = BUFFER_TO_BASE_POINTER(list);
    base[field] = value;
}

LD_API void* _list_remove(
    void* list,
    usize i,
    void* dst
) {
    u64*  base       = BUFFER_TO_BASE_POINTER(list);
    usize capacity   = base[LIST_FIELD_CAPACITY];
    usize count      = base[LIST_FIELD_COUNT];
    usize stride     = base[LIST_FIELD_STRIDE];
    usize total_size = capacity * stride;

    if( i >= capacity || i >= count ) {
        LOG_FATAL("List remove out of bounds! index: {u64}", i);
        PANIC();
    }

    u8*   bytes = (u8*)list;
    usize index = stride * i;

    if( dst ) {
        mem_copy( dst, &bytes[index], stride );
    }
    if( i != count - 1 ) {
        usize index_after = index + stride;
        usize remaining_size = total_size - index_after;
        mem_copy(
            &bytes[index],
            &bytes[index_after],
            remaining_size
        );
    }

    base[LIST_FIELD_COUNT] -= 1;

    return list;
}
LD_API void* _list_insert(
    void* list,
    usize index,
    void* pvalue
) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize count    = base[LIST_FIELD_COUNT];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( index >= count ) {
        LOG_FATAL(
            "Index outside the bounds of the list! index: {u64}",
            index
        );
        PANIC();
        return NULL;
    }
    
    if( count >= capacity ) {
        list = _list_realloc(
            list,
            LIST_REALLOC_FACTOR * capacity
        );
        base = BUFFER_TO_BASE_POINTER(list);
    }
    
    u8* buffer_bytes = (u8*)list;

    if( index != count - 1 ) {
        mem_copy_overlapped(
            buffer_bytes + ((index + 1) * stride),
            buffer_bytes + (index * stride),
            stride * ( count - index )
        );
    }

    mem_copy(
        buffer_bytes + (index * stride),
        pvalue,
        stride
    );

    base[LIST_FIELD_COUNT] += 1;

    return list;
}

LD_API void* _list_insert_trace(
    void* list,
    usize index,
    void* pvalue,
    const char* function,
    const char* file,
    int line
) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize count    = base[LIST_FIELD_COUNT];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( index >= count ) {
        LOG_FATAL(
            "Index outside the bounds of the list! index: {u64}",
            index
        );
        PANIC();
        return NULL;
    }
    
    if( count >= capacity ) {
        list = _list_realloc_trace(
            list,
            LIST_REALLOC_FACTOR * capacity,
            function,
            file,
            line
        );
        base = BUFFER_TO_BASE_POINTER(list);
    }
    
    u8* buffer_bytes = (u8*)list;

    if( index != count - 1 ) {
        mem_copy_overlapped(
            buffer_bytes + ((index + 1) * stride),
            buffer_bytes + (index * stride),
            stride * ( count - index )
        );
    }

    mem_copy(
        buffer_bytes + (index * stride),
        pvalue,
        stride
    );

    base[LIST_FIELD_COUNT] += 1;

    return list;
}
