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

namespace internal {

SM_API void* impl_list_create( usize capacity, usize stride ) {
    usize total_size = (capacity * stride) + LIST_FIELDS_SIZE;
    u64* buffer = (u64*)mem_alloc(
        total_size,
        MEMTYPE_DYNAMIC_LIST
    );
    if( !buffer ) {
        return nullptr;
    }

    buffer[LIST_FIELD_CAPACITY] = capacity;
    buffer[LIST_FIELD_COUNT]    = 0;
    buffer[LIST_FIELD_STRIDE]   = stride;

    return (void*)(buffer + LIST_NUM_FIELDS);
}
SM_API void  impl_list_free( void* list ) {
    if( !list ) {
        return;
    }
    u64* list_start = ((u64*)list) - LIST_NUM_FIELDS;
    mem_free( list_start );
}

SM_API void* impl_list_realloc( void* list, usize new_capacity ) {
    u64* buffer = ((u64*)list) - LIST_NUM_FIELDS;

    usize stride      = buffer[LIST_FIELD_STRIDE];
    usize new_size    = (new_capacity * stride) + LIST_FIELDS_SIZE;
    u64* new_buffer   = (u64*)mem_realloc( buffer, new_size );

    if( !new_buffer ) {
        LOG_ERROR("Failed to realloc list!");
        return list;
    }

    new_buffer[LIST_FIELD_CAPACITY] = new_capacity;

    return new_buffer + LIST_NUM_FIELDS;
}
SM_API void* impl_list_push( void* list, const void* pvalue ) {
    u64* list_start = ((u64*)list) - LIST_NUM_FIELDS;
    usize count     = list_start[LIST_FIELD_COUNT];
    usize capacity  = list_start[LIST_FIELD_CAPACITY];
    usize stride    = list_start[LIST_FIELD_STRIDE];

    if( count == capacity ) {
        list = impl_list_realloc(
            list,
            capacity * LIST_REALLOC_FACTOR
        );
        list_start = ((u64*)list) - LIST_NUM_FIELDS;
        count     = list_start[LIST_FIELD_COUNT];
        capacity  = list_start[LIST_FIELD_CAPACITY];
        stride    = list_start[LIST_FIELD_STRIDE];
    }
    u8* bytes = (u8*)list;

    usize index = stride * count;
    
    mem_copy( bytes + index, pvalue, stride );

    list_start[LIST_FIELD_COUNT] += 1;

    return list;
}
SM_API b32 impl_list_pop( void* list, void* dst ) {
    u64* list_start = ((u64*)list) - LIST_NUM_FIELDS;
    usize count  = list_start[LIST_FIELD_COUNT];
    usize stride = list_start[LIST_FIELD_STRIDE];

    if( count == 0 ) {
        return false;
    }
    u8* bytes = (u8*)list;

    mem_copy( dst, bytes + count, stride );
    list_start[LIST_FIELD_COUNT] -= 1;

    return true;
}

SM_API usize impl_list_field_read( void* list, u32 field ) {
    u64* list_start = ((u64*)list) - LIST_NUM_FIELDS;
    return list_start[field];
}
SM_API void impl_list_field_write(
    void* list,
    u32 field,
    usize value
) {
    u64* list_start = (u64*)list - LIST_NUM_FIELDS;
    list_start[field] = value;
}

SM_API void* impl_list_remove(
    void* list,
    usize i,
    void* dst
) {
    u64* list_start  = ((u64*)list) - LIST_NUM_FIELDS;
    usize capacity   = list_start[LIST_FIELD_CAPACITY];
    usize count      = list_start[LIST_FIELD_COUNT];
    usize stride     = list_start[LIST_FIELD_STRIDE];
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

    list_start[LIST_FIELD_COUNT] -= 1;

    return list;
}
SM_API void* impl_list_insert(
    void* list,
    usize index,
    void* pvalue
) {
    u64* list_start  = ((u64*)list) - LIST_NUM_FIELDS;
    usize capacity   = list_start[LIST_FIELD_CAPACITY];
    usize count      = list_start[LIST_FIELD_COUNT];
    usize stride     = list_start[LIST_FIELD_STRIDE];

    if( index >= count ) {
        LOG_FATAL("Index outside the bounds of the list! index: %llu", index);
        SM_PANIC();
        return nullptr;
    }
    
    if( count >= capacity ) {
        list = impl_list_realloc(
            list,
            LIST_REALLOC_FACTOR * capacity
        );
        list_start = ((u64*)list) - LIST_NUM_FIELDS;
    }
    
    u64 address = (u64)list;

    if( index != count - 1 ) {
        mem_overlap_copy(
            (void*)(address + ((index + 1) * stride)),
            (void*)(address + (index * stride)),
            stride * ( count - index )
        );
    }

    mem_copy(
        (void*)(address + (index * stride)),
        pvalue,
        stride
    );

    list_start[LIST_FIELD_COUNT] += 1;

    return list;
}

} // namespace internal
