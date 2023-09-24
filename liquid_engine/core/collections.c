/**
 * Description:  Collections Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "core/collections.h"
#include "core/allocator.h"
#include "core/mem.h"
#include "core/log.h"
#include "core/strings.h"

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
LD_API void* list_pop( List* list ) {
    struct ListHeader* header = list_head( list );
    if( !header->count ) {
        return NULL;
    }
    header->count--;
    void* result = (u8*)list + ( header->item_size * header->count );
    return result;
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

LD_API b32 map_set( Map* map, void* key, void* new_value ) {
    usize stride = map->key_size + map->value_size;
    u8* buffer   = map->buffer;

    u8* kv = NULL;
    for( usize i = 0; i < map->count; ++i ) {
        u8* current = buffer + ( i * stride );
        if( mem_cmp( current, key, map->key_size ) ) {
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }

    mem_copy( kv + map->key_size, new_value, map->value_size );

    return true;
}
LD_API b32 map_get( Map* map, void* key, void* out_value ) {
    usize stride = map->key_size + map->value_size;
    u8* buffer   = map->buffer;

    u8* kv = NULL;
    for( usize i = 0; i < map->count; ++i ) {
        u8* current = buffer + ( i * stride );
        if( mem_cmp( current, key, map->key_size ) ) {
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }

    mem_copy( out_value, kv + map->key_size, map->value_size );

    return true;
}
LD_API b32 map_key_exists( Map* map, void* key ) {
    usize stride = map->key_size + map->value_size;
    u8* buffer   = map->buffer;

    for( usize i = 0; i < map->count; ++i ) {
        u8* current = buffer + ( i * stride );
        if( mem_cmp( current, key, map->key_size ) ) {
            return true;
        }
    }
    return false;
}
LD_API b32 map_push( Map* map, void* key, void* value ) {
    if( map->count == map->capacity ) {
        return false;
    }

    u8* kv = (u8*)map->buffer + map->count++;

    mem_copy( kv, key, map->key_size );
    mem_copy( kv + map->key_size, value, map->value_size );

    return true;
}
LD_API b32 map_remove( Map* map, void* key, void* opt_out_value ) {
    usize stride = map->key_size + map->value_size;
    u8*   buffer = map->buffer;

    u8* kv = NULL;
    usize remaining_count = map->count;
    for( usize i = 0; i < map->count; ++i ) {
        u8* current = buffer + ( i * stride );
        remaining_count--;
        if( mem_cmp( current, key, map->key_size ) ) {
            if( i == map->count - 1 ) {
                map->count--;
                if( opt_out_value ) {
                    mem_copy(
                        opt_out_value,
                        current + map->key_size,
                        map->value_size
                    );
                }
                return true;
            }
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }
    if( opt_out_value ) {
        mem_copy( opt_out_value, kv + map->key_size, map->value_size );
    }

    usize size = remaining_count * stride;
    mem_copy_overlapped( kv, kv + stride, size );
    map->count--;

    return true;
}
LD_API b32 map_u32_u32_set( Map_u32_u32* map, u32 key, u32 new_value ) {
    KV_u32_u32* kv = NULL;
    for( usize i = 0; i < map->count; ++i ) {
        KV_u32_u32* current = map->pairs + i;
        if( current->key == key ) {
            kv = current;
            break;
        }
    }
    if( !kv ) {
        return false;
    }
    kv->value = new_value;
    return true;
}
LD_API b32 map_u32_u32_get( Map_u32_u32* map, u32 key, u32* out_value ) {
    for( usize i = 0; i < map->count; ++i ) {
        KV_u32_u32* current = map->pairs + i;
        if( current->key == key ) {
            *out_value = current->value;
            return true;
        }
    }
    return false;
}
LD_API b32 map_u32_u32_key_exists( Map_u32_u32* map, u32 key ) {
    for( usize i = 0; i < map->count; ++i ) {
        if( (map->pairs + i)->key == key ) {
            return true;
        }
    }
    return false;
}
LD_API b32 map_u32_u32_push( Map_u32_u32* map, u32 key, u32 value ) {
    if( map->count == map->capacity ) {
        return false;
    }
    map->pairs[map->count++] = (KV_u32_u32){ key, value };
    return true;
}
LD_API b32 map_u32_u32_remove(
    Map_u32_u32* map, u32 key, u32* opt_out_value
) {
    KV_u32_u32* kv = NULL;
    usize remaining_count = map->count;
    for( usize i = 0; i < map->count; ++i ) {
        KV_u32_u32* current = map->pairs + i;
        remaining_count--;
        if( current->key == key ) {
            if( i == map->count - 1 ) {
                map->count--;
                if( opt_out_value ) {
                    *opt_out_value = current->value;
                }
                return true;
            }
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }
    if( opt_out_value ) {
        *opt_out_value = kv->value;
    }

    usize size = (remaining_count) * sizeof(KV_u32_u32);
    mem_copy_overlapped( kv, kv + 1, size );
    map->count--;

    return true;
}
LD_API b32 map_ss_set( MapStringSlice* map, StringSlice key, void* new_value ) {
    usize stride = sizeof(StringSlice) + map->value_size;
    u8* buffer   = map->buffer;

    KV_StringSlice* kv = NULL;
    for( usize i = 0; i < map->count; ++i ) {
        KV_StringSlice* current = (KV_StringSlice*)(buffer + ( i * stride ));
        if( ss_cmp( &current->key, &key ) ) {
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }

    mem_copy( kv->value, new_value, map->value_size );

    return true;
}
LD_API b32 map_ss_get( MapStringSlice* map, StringSlice key, void* out_value ) {
    usize stride = sizeof(StringSlice) + map->value_size;
    u8*   buffer = map->buffer;

    KV_StringSlice* kv = NULL;
    for( usize i = 0; i < map->count; ++i ) {
        KV_StringSlice* current = (KV_StringSlice*)( buffer + ( i * stride ) );
        if( ss_cmp( &current->key, &key ) ) {
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }

    mem_copy( out_value, kv->value, map->value_size );
    return true;
}
LD_API b32 map_ss_key_exists( MapStringSlice* map, StringSlice key ) {
    usize stride = sizeof(StringSlice) + map->value_size;
    u8*   buffer = map->buffer;

    for( usize i = 0; i < map->count; ++i ) {
        KV_StringSlice* current = (KV_StringSlice*)( buffer + ( i * stride ) );
        if( ss_cmp( &current->key, &key ) ) {
            return true;
        }
    }

    return false;
}
LD_API b32 map_ss_push( MapStringSlice* map, StringSlice key, void* value ) {
    if( map->count == map->capacity ) {
        return false;
    }

    KV_StringSlice* kv = (KV_StringSlice*)((u8*)map->buffer + map->count++);

    kv->key = ss_clone( &key );
    mem_copy( kv->value, value, map->value_size );

    return true;
}
LD_API b32 map_ss_remove(
    MapStringSlice* map, StringSlice key, void* opt_out_value
) {
    usize stride = sizeof(StringSlice) + map->value_size;
    u8*   buffer = map->buffer;

    KV_StringSlice* kv = NULL;
    usize remaining_count = map->count;
    for( usize i = 0; i < map->count; ++i ) {
        KV_StringSlice* current = (KV_StringSlice*)(buffer + ( i * stride ));
        remaining_count--;
        if( ss_cmp( &current->key, &key ) ) {
            if( i == map->count - 1 ) {
                map->count--;
                if( opt_out_value ) {
                    mem_copy(
                        opt_out_value,
                        current->value,
                        map->value_size
                    );
                }
                return true;
            }
            kv = current;
            break;
        }
    }

    if( !kv ) {
        return false;
    }
    if( opt_out_value ) {
        mem_copy( opt_out_value, kv->value, map->value_size );
    }

    usize size = remaining_count * stride;
    mem_copy_overlapped( kv, (u8*)kv + stride, size );
    map->count--;

    return true;
}

