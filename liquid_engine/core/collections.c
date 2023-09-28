/**
 * Description:  Collections Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "core/collections.h"
#include "core/memoryf.h"

struct ListHeader {
    usize capacity;
    usize count;
    usize item_size;
};

LD_API usize list_calculate_memory_requirement( usize capacity, usize item_size ) {
    return (capacity * item_size) + sizeof(struct ListHeader);
}
LD_API List* list_create( usize capacity, usize item_size, void* buffer ) {
    struct ListHeader* header = buffer;
    
    header->capacity  = capacity;
    header->item_size = item_size;
    header->count     = 0;

    return (u8*)buffer + sizeof( struct ListHeader );
}
LD_API void list_set_capacity( List* list, usize new_capacity ) {
    struct ListHeader* header = list_head( list );
    header->capacity = new_capacity;
}
LD_API b32 list_push( List* list, void* item ) {
    struct ListHeader* header = list_head( list );
    if( header->count == header->capacity ) {
        return false;
    }

    mem_copy(
        (u8*)list + (header->item_size * header->count),
        item, header->item_size );

    header->count++;

    return true;
}
LD_API b32 list_append( List* list, usize append_count, void* append_items ) {
    struct ListHeader* header = list_head( list );
    if( header->count + append_count > header->capacity ) {
        return false;
    }

    mem_copy(
        (u8*)list + (header->item_size * header->count),
        append_items, header->item_size * append_count );
    header->count += append_count;

    return true;
}
LD_API void* list_pop( List* list ) {
    struct ListHeader* header = list_head( list );
    if( !header->count ) {
        return NULL;
    }
    header->count--;
    void* result = (u8*)list + ( header->item_size * header->count );
    return result;
}
LD_API void* list_peek( List* list ) {
    struct ListHeader* header = list_head( list );
    if( !header->count ) {
        return NULL;
    }
    return (u8*)list + ( header->item_size * ( header->count - 1 ) );
}
LD_API b32 list_insert( List* list, usize index, void* item ) {
    struct ListHeader* header = list_head( list );
    if( header->count == header->capacity ) {
        return false;
    }

    if( index == header->count - 1 ) {
        return list_push( list, item );
    }

    usize right_count = header->count - index;
    mem_copy_overlapped(
        (u8*)list + ( header->item_size * (index + 1) ),
        (u8*)list + ( header->item_size * index ),
        header->item_size * right_count );
    mem_copy(
        (u8*)list + ( header->item_size * index ),
        item, header->item_size );

    header->count++;

    return true;
}
LD_API void list_remove( List* list, usize index, void* opt_out_item ) {
    struct ListHeader* header = list_head( list );
    assert( index < header->count );

    if( index == header->count - 1 ) {
        void* popped_item = list_pop( list );
        if( opt_out_item && popped_item ) {
            mem_copy( opt_out_item, popped_item, header->item_size );
        }
        return;
    }

    if( opt_out_item ) {
        mem_copy(
            opt_out_item, list_index( list, index ),
            header->item_size );
    }

    usize right_count = header->count - index;
    mem_copy_overlapped(
        (u8*)list + ( header->item_size * index ),
        (u8*)list + ( header->item_size * (index + 1) ),
        header->item_size * right_count );

    header->count--;
}
LD_API void* list_index( List* list, usize index ) {
    struct ListHeader* header = list_head( list );
    if( index >= header->count ) {
        return NULL;
    }
    return (u8*)list + ( header->item_size * index );
}
LD_API void list_set( List* list, usize index, void* item ) {
    struct ListHeader* header = list_head( list );
    assert( index < header->count );
    mem_copy(
        (u8*)list + ( header->item_size * index ),
        item, header->item_size );
}
LD_API void list_fill( List* list, void* item ) {
    struct ListHeader* header = list_head( list );
    for( usize i = 0; i < header->count; ++i ) {
        mem_copy(
            (u8*)list + ( header->item_size * i ),
            item, header->item_size );
    }
}
LD_API void list_fill_to_capacity( List* list, void* item ) {
    struct ListHeader* header = list_head( list );
    for( usize i = 0; i < header->capacity; ++i ) {
        mem_copy(
            (u8*)list + ( header->item_size * i ),
            item, header->item_size );
    }
    header->count = header->capacity;
}
LD_API void list_clear( List* list ) {
    struct ListHeader* header = list_head( list );
    header->count = 0;
}
LD_API void* list_head( List* list ) {
    return (u8*)list - sizeof(struct ListHeader);
}
LD_API usize list_count( List* list ) {
    struct ListHeader* header = list_head( list );
    return header->count;
}
LD_API usize list_capacity( List* list ) {
    struct ListHeader* header = list_head( list );
    return header->capacity;
}
LD_API usize list_item_size( List* list ) {
    struct ListHeader* header = list_head( list );
    return header->item_size;
}

internal isize sorting_quicksort_partition(
    isize low, isize high, usize element_size, void* buffer,
    SortLTFN* lt, void* opt_lt_params, SortSwapFN* swap_
) {
    u8* buf = buffer;
    void* pivot = buf + (high * element_size);

    isize i = (isize)low - 1;

    for( isize j = (isize)low; j <= (isize)high - 1; ++j ) {
        void* at_j = buf + (j * element_size);

        if( lt( at_j, pivot, opt_lt_params ) ) {
            ++i;
            void* at_i = buf + (i * element_size);
            swap_( at_i, at_j );
        }
    }

    swap_( buf + ((i + 1) * element_size), buf + (high * element_size) );
    return i + 1;
}

LD_API void sorting_quicksort(
    isize       low,
    isize       high,
    usize       element_size,
    void*       buffer,
    SortLTFN*   lt,
    void*       opt_lt_params,
    SortSwapFN* swap_
) {
#if 0
    if( low < high ) {
        isize partition_index = sorting_quicksort_partition(
            low, high, element_size,
            buffer, lt, opt_lt_params, swap_ );
        sorting_quicksort(
            low, partition_index - 1, element_size,
            buffer, lt, opt_lt_params, swap_ );
        sorting_quicksort(
            partition_index + 1, high, element_size,
            buffer, lt, opt_lt_params, swap_ );
    }
#else
    while( low < high ) {
        isize partition_index = sorting_quicksort_partition(
            low, high, element_size,
            buffer, lt, opt_lt_params, swap_ );
        if( partition_index - low < high - partition_index ) {
            sorting_quicksort(
                low, partition_index - 1, element_size,
                buffer, lt, opt_lt_params, swap_ );
            low = partition_index + 1;
        } else {
            sorting_quicksort(
                partition_index + 1, high, element_size,
                buffer, lt, opt_lt_params, swap_ );
            high = partition_index - 1;
        }
    }
#endif
}

