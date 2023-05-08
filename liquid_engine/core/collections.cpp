/**
 * Description:  Collections Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "collections.h"
#include "memory.h"
#include "logging.h"

#define LIST_NUM_FIELDS 3
#define LIST_FIELDS_SIZE (LIST_NUM_FIELDS * sizeof(u64))
#define LIST_REALLOC_FACTOR 2

#define BUFFER_TO_BASE_POINTER( list ) \
    (((u64*)list) - LIST_NUM_FIELDS)

#define BASE_TO_BUFFER_POINTER( base )\
    (((u64*)base) + LIST_NUM_FIELDS)

namespace impl {

SM_API void* _list_create( usize capacity, usize stride ) {
    usize total_size = (capacity * stride) + LIST_FIELDS_SIZE;
    u64* base = (u64*)mem_alloc(
        total_size,
        MEMTYPE_DYNAMIC_LIST
    );
    if( !base ) {
        return nullptr;
    }

    base[LIST_FIELD_CAPACITY] = capacity;
    base[LIST_FIELD_COUNT]    = 0;
    base[LIST_FIELD_STRIDE]   = stride;

    return BASE_TO_BUFFER_POINTER(base);
}
SM_API void  _list_free( void* list ) {
    if( !list ) {
        return;
    }
    u64* base = BUFFER_TO_BASE_POINTER(list);
    mem_free( base );
}

SM_API void* _list_realloc( void* list, usize new_capacity ) {
    u64* base = BUFFER_TO_BASE_POINTER(list);

    usize stride   = base[LIST_FIELD_STRIDE];
    usize new_size = (new_capacity * stride) + LIST_FIELDS_SIZE;
    u64*  new_base = (u64*)mem_realloc( base, new_size );

    if( !new_base ) {
        LOG_ERROR("Failed to realloc list!");
        return list;
    }

    new_base[LIST_FIELD_CAPACITY] = new_capacity;

    return BASE_TO_BUFFER_POINTER(new_base);
}
SM_API void* _list_push( void* list, const void* pvalue ) {
    u64*  base     = BUFFER_TO_BASE_POINTER(list);
    usize count    = base[LIST_FIELD_COUNT];
    usize capacity = base[LIST_FIELD_CAPACITY];
    usize stride   = base[LIST_FIELD_STRIDE];

    if( count == capacity ) {
        list = _list_realloc(
            list,
            capacity * LIST_REALLOC_FACTOR
        );
        base     = BUFFER_TO_BASE_POINTER(list);
        count    = base[LIST_FIELD_COUNT];
        capacity = base[LIST_FIELD_CAPACITY];
        stride   = base[LIST_FIELD_STRIDE];
    }
    u8* bytes = (u8*)list;

    usize index = stride * count;
    
    mem_copy( bytes + index, pvalue, stride );

    base[LIST_FIELD_COUNT] += 1;

    return list;
}
SM_API b32 _list_pop( void* list, void* dst ) {
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

SM_API usize _list_field_read( void* list, u32 field ) {
    u64*   base = BUFFER_TO_BASE_POINTER(list);
    return base[field];
}
SM_API void _list_field_write(
    void* list,
    u32 field,
    usize value
) {
    u64* base   = BUFFER_TO_BASE_POINTER(list);
    base[field] = value;
}

SM_API void* _list_remove(
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
        LOG_FATAL("List remove out of bounds! index: %llu", i);
        SM_PANIC();
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
SM_API void* _list_insert(
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
            "Index outside the bounds of the list! index: %llu",
            index
        );
        SM_PANIC();
        return nullptr;
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
        mem_overlap_copy(
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

} // namespace impl
