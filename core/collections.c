/**
 * Description:  Collections Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "shared/defines.h"
#include "core/collections.h"
#include "core/memory.h"

CORE_API void* iterator_next_enumerate( Iterator* iter, usize* out_enumerator ) {
    if( iter->current == iter->count ) {
        return NULL;
    }
    *out_enumerator = iter->current;
    void* result = (u8*)iter->buffer + ( iter->item_size * iter->current++ );
    return result;
}
CORE_API void* iterator_reverse_next_enumerate( Iterator* iter, usize* out_enumerator ) {
    if( iter->current == iter->count ) {
        return NULL;
    }

    usize index = (iter->count - iter->current) - 1;
    *out_enumerator = iter->current++;

    void* result = (u8*)iter->buffer + ( iter->item_size * index );

    return result;
}
CORE_API b32 iterator_next_value_enumerate( Iterator* iter, void* out_item, usize* out_index ) {
    if( iter->current == iter->count ) {
        return false;
    }

    *out_index = iter->current;
    void* src = (u8*)iter->buffer + ( iter->item_size * iter->current++ );
    memory_copy( out_item, src, iter->item_size );

    return true;
}
CORE_API b32 iterator_reverse_next_value_enumerate(
    Iterator* iter, void* out_item, usize* out_index
) {
    if( iter->current == iter->count ) {
        return false;
    }

    usize index = (iter->count - iter->current) - 1;
    *out_index  = iter->current++;

    void* src = (u8*)iter->buffer + ( iter->item_size * index );
    memory_copy( out_item, src, iter->item_size );

    return true;
}
CORE_API void iterator_split(
    Iterator* iter, usize index, Iterator* out_first, Iterator* out_last
) {
    out_first->buffer    = iter->buffer;
    out_first->item_size = iter->item_size;
    out_first->count     = index;
    out_first->current   = 0;

    out_last->buffer    = ((u8*)iter->buffer) + (index * iter->item_size);
    out_last->item_size = iter->item_size;
    out_last->count     = iter->count - index;
    out_last->current   = 0;
}

CORE_API b32 list_push( List* list, void* item ) {
    if( list->count == list->capacity ) {
        return false;
    }

    memory_copy(
        (u8*)list->buffer + (list->item_size * list->count++),
        item, list->item_size );

    return true;
}
CORE_API b32 list_append( List* list, usize append_count, void* append_items ) {
    if( list->count + append_count > list->capacity ) {
        return false;
    }

    memory_copy(
        (u8*)list->buffer + (list->item_size * list->count),
        append_items, list->item_size * append_count );
    list->count += append_count;

    return true;
}
CORE_API void* list_pop( List* list ) {
    if( !list->count ) {
        return NULL;
    }
    void* result = (u8*)list->buffer + ( list->item_size * --list->count );
    return result;
}
CORE_API b32 list_pop_value( List* list, void* out_item ) {
    if( !list->count ) {
        return false;
    }

    memory_copy(
        out_item,
        (u8*)list->buffer + ( list->item_size * --list->count ), list->item_size );
    return true;
}
CORE_API void* list_peek( List* list ) {
    if( !list->count ) {
        return NULL;
    }
    return (u8*)list->buffer + ( list->item_size * ( list->count - 1 ) );
}
CORE_API b32 list_insert( List* list, usize index, void* item ) {
    if( list->count == list->capacity ) {
        return false;
    }

    if( index == list->count - 1 ) {
        return list_push( list, item );
    }

    usize right_count = list->count - index;
    memory_copy_overlapped(
        (u8*)list->buffer + ( list->item_size * (index + 1) ),
        (u8*)list->buffer + ( list->item_size * index ),
        list->item_size * right_count );
    memory_copy(
        (u8*)list->buffer + ( list->item_size * index ),
        item, list->item_size );

    list->count++;

    return true;
}
CORE_API void list_remove( List* list, usize index, void* opt_out_item ) {
    assert( index < list->count );

    if( index == list->count - 1 ) {
        void* popped_item = list_pop( list );
        if( opt_out_item && popped_item ) {
            memory_copy( opt_out_item, popped_item, list->item_size );
        }
        return;
    }

    if( opt_out_item ) {
        memory_copy(
            opt_out_item, list_index( list, index ),
            list->item_size );
    }

    usize right_count = list->count - index;
    memory_copy_overlapped(
        (u8*)list->buffer + ( list->item_size * index ),
        (u8*)list->buffer + ( list->item_size * (index + 1) ),
        list->item_size * right_count );

    list->count--;
}
CORE_API void* list_index( List* list, usize index ) {
    if( index >= list->count ) {
        return NULL;
    }
    return (u8*)list->buffer + ( list->item_size * index );
}
CORE_API b32 list_index_value( List* list, usize index, void* out_item ) {
    if( index >= list->count ) {
        return false;
    }
    memory_copy(
        out_item, (u8*)list->buffer + (index * list->item_size), list->item_size );
    return true;
}
CORE_API void list_set( List* list, usize index, void* item ) {
    assert( index < list->count );
    memory_copy(
        (u8*)list->buffer + ( list->item_size * index ),
        item, list->item_size );
}
CORE_API void list_fill( List* list, void* item ) {
    for( usize i = 0; i < list->count; ++i ) {
        memory_copy(
            (u8*)list->buffer + ( list->item_size * i ),
            item, list->item_size );
    }
}
CORE_API void list_fill_to_capacity( List* list, void* item ) {
    for( usize i = 0; i < list->capacity; ++i ) {
        memory_copy(
            (u8*)list->buffer + ( list->item_size * i ),
            item, list->item_size );
    }
    list->count = list->capacity;
}
CORE_API Iterator list_iterator( List* list ) {
    Iterator result = {0};

    result.buffer    = list->buffer;
    result.item_size = list->item_size;
    result.count     = list->count;

    return result;
}

