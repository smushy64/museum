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
LD_API List* list_resize( void* head, usize new_capacity ) {
    struct ListHeader* header = head;
    List* result = (u8*)head + sizeof(struct ListHeader);
    header->capacity = new_capacity;

    return result;
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

