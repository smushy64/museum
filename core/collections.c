/**
 * Description:  Collections Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "shared/defines.h"
#include "core/collections.h"
#include "core/memory.h"

#define HASHMAP_MAX_LINEAR_SEARCH (32)
CORE_API b32 hashmap_insert_key(
    Hashmap* hashmap, Key key, KValue value, b32 check_for_duplicate
) {
    if( check_for_duplicate ) {
        if( hashmap_contains_key( hashmap, key ) ) {
            return true;
        }
    }

    if( hashmap->count == hashmap->capacity ) {
        return false;
    }

    if( !hashmap->count || key > hashmap->largest_key ) {
        goto hashmap_insert_push;
    }

    usize index = 0;
    if( key > hashmap->keys[0] ) {
        if( hashmap->count < HASHMAP_MAX_LINEAR_SEARCH ) {
            usize i = hashmap->count;
            do {
                i--;
                Key k = hashmap->keys[i];
                if( key > k ) {
                    index = i + 1;
                    break;
                }
            } while( i );
        } else {
            // binary tree approach, maybe faster than linear lookup
            usize low  = 0;
            usize high = hashmap->count;
            while( low < high ) {
                usize mid = ( low + high ) >> 1;
                if( hashmap->keys[mid] < key ) {
                    low = mid + 1;
                } else {
                    high = mid;
                }
            }
            index = low;
        }
    }

    usize copy_count = hashmap->count - index;
    if( copy_count ) {
        usize copy_size  = copy_count * sizeof(Key);
        memory_copy_overlapped(
            hashmap->keys + index + 1, hashmap->keys + index, copy_size );
        hashmap->keys[index] = key;

        copy_size = copy_count * sizeof(KValue);
        memory_copy_overlapped(
            hashmap->values + index + 1, hashmap->values + index, copy_size );
        hashmap->values[index] = value;

        hashmap->count++;

        if( key > hashmap->largest_key ) {
            hashmap->largest_key = key;
        }
    } else {
hashmap_insert_push:
        hashmap->keys[hashmap->count]     = key;
        hashmap->values[hashmap->count++] = value;
        if( key > hashmap->largest_key ) {
            hashmap->largest_key = key;
        }
    }

    return true;
}
CORE_API KValue* hashmap_get( Hashmap* hashmap, Key key ) {
    if( !hashmap->count || key > hashmap->largest_key ) {
        return NULL;
    }

    if( hashmap->count < HASHMAP_MAX_LINEAR_SEARCH ) {
        for( usize i = 0; i < hashmap->count; ++i ) {
            if( hashmap->keys[i] == key ) {
                return hashmap->values + i;
            }
        }
    } else {
        usize low  = 0;
        usize high = hashmap->count;
        while( low < high ) {
            usize mid = (low + high) >> 1;
            Key k = hashmap->keys[mid];
            if( k == key ) {
                return hashmap->values + mid;
            } else if( k < key ) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }
    }
    return NULL;
}
CORE_API void hashmap_remove_by_index(
    Hashmap* hashmap, usize index, KValue* opt_out_value
) {
    if( opt_out_value ) {
        *opt_out_value = hashmap->values[index];
    }

    usize copy_count = hashmap->count - index;
    usize copy_size  = copy_count * sizeof(Key);
    memory_copy_overlapped(
        hashmap->keys + index, hashmap->keys + index + 1, copy_size );
    copy_size = copy_count * sizeof(KValue);
    memory_copy_overlapped(
        hashmap->values + index, hashmap->values + index + 1, copy_size );

    hashmap->count--;
}

#undef HASHMAP_MAX_LINEAR_SEARCH

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

