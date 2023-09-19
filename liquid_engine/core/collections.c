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

LD_API b32 internal_list_create(
    struct Allocator* allocator, usize capacity,
    usize stride, List* out_list
) {
    usize buffer_size = capacity * stride;
    void* buffer = internal_allocator_alloc(
        allocator, buffer_size, MEMORY_TYPE_DYNAMIC_LIST );
    if( !buffer ) {
        return false;
    }

    out_list->allocator = allocator;
    out_list->count     = 0;
    out_list->capacity  = capacity;
    out_list->stride    = stride;

    out_list->buffer = buffer;

    return true;
}
LD_API b32 internal_list_realloc( List* list, usize new_capacity ) {
    // TODO(alicia): 
    unused(list); unused(new_capacity);
    UNIMPLEMENTED();
    return false;
}
LD_API void internal_list_free( List* list ) {
    if( list->buffer ) {
        internal_allocator_free(
            list->allocator, list->buffer,
            list_buffer_size( list ), MEMORY_TYPE_DYNAMIC_LIST );
    }
    mem_zero( list, sizeof(List) );
}
LD_API b32 internal_list_push_realloc( List* list, void* item, usize realloc ) {
    if( list->count == list->capacity ) {
        if( !internal_list_realloc( list, realloc ) ) {
            return false;
        }
    }

    return list_push( list, item );
}
LD_API b32 internal_list_insert_realloc(
    List* list, void* item, usize index, usize realloc
) {
    if( list->count == list->capacity ) {
        if( !internal_list_realloc( list, realloc ) ) {
            return false;
        }
    }
    return list_insert( list, item, index );
}
LD_API b32 internal_list_create_trace(
    struct Allocator* allocator, usize capacity,
    usize stride, List* out_list,
    const char* function, const char* file, int line
) {
    b32 result = internal_list_create( allocator, capacity, stride, out_list );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_GREEN
            "[LIST CREATE | {cc}() | {cc}:{i}] "
            "Capacity: {u64} Stride: {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)capacity, (u64)stride, (u64)out_list->buffer
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[LIST CREATE FAILED | {cc}() | {cc}:{i}] "
            "Capacity: {u64} Stride: {u64}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)capacity, (u64)stride
        );
    }
    return result;
}
LD_API b32 internal_list_realloc_trace(
    List* list, usize new_capacity,
    const char* function, const char* file, int line
) {
    usize old_capacity = list->capacity;
    b32 result = internal_list_realloc( list, new_capacity );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_GREEN
            "[LIST REALLOC | {cc}() | {cc}:{i}] "
            "Capacity: {u64} -> {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)old_capacity, (u64)new_capacity, (u64)list->buffer
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
            false, true,
            LOG_COLOR_RED
            "[LIST REALLOC FAILED | {cc}() | {cc}:{i}] "
            "Capacity: {u64} -> {u64} Pointer: {u64,x}"
            LOG_COLOR_RESET,
            function, file, line,
            (u64)old_capacity, (u64)new_capacity, (u64)list->buffer
        );
    }
    return result;
}
LD_API void internal_list_free_trace(
    List* list, const char* function, const char* file, int line
) {
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,
        false, true,
        LOG_COLOR_CYAN
        "[FREE | {cc}() | {cc}:{i}] "
        "Capacity: {u64} Pointer: {u64,x}"
        LOG_COLOR_RESET,
        function, file, line,
        (u64)list->capacity, (u64)list->buffer
    );
    internal_list_free( list );
}
LD_API b32 internal_list_push_realloc_trace(
    List* list, void* item, usize realloc,
    const char* function, const char* file, int line
) {
    if( list->count == list->capacity ) {
        if( !internal_list_realloc_trace(
            list, realloc, function, file, line
        ) ) {
            return false;
        }
    }

    return list_push( list, item );
}
LD_API b32 internal_list_insert_realloc_trace(
    List* list, void* item, usize index, usize realloc,
    const char* function, const char* file, int line
) {
    if( list->count == list->capacity ) {
        if( !internal_list_realloc_trace(
            list, realloc, function, file, line
        ) ) {
            return false;
        }
    }
    return list_insert( list, item, index );
}


LD_API b32 list_push( List* list, void* item ) {
    if( list->count == list->capacity ) {
        return false;
    }
    u8* buffer = list->buffer;
    mem_copy(
        buffer + (list->stride * list->count),
        item, list->stride
    );
    list->count++;

    return true;
}
LD_API b32 list_insert( List* list, void* item, usize index ) {
    if( list->count + 1 > list->capacity ) {
        return false;
    }
    if( index >= list->count ) {
        return false;
    }
    if( index == list->count - 1 ) {
        return list_push( list, item );
    }
    void* src = (u8*)list->buffer + ( list->stride * index );
    void* dst = (u8*)list->buffer + ( list->stride * ( index + 1 ) );
    usize remaining_count = list->count - index;
    usize size = list->stride * remaining_count;
    mem_copy_overlapped( dst, src, size );

    mem_copy( dst, item, list->stride );

    return true;
}
LD_API void* list_pop( List* list ) {
    if( !list->count ) {
        return NULL;
    }
    u8* buffer = list->buffer;
    list->count--;
    return buffer + (list->count * list->stride);
}
LD_API void list_remove( List* list, usize index ) {
    assert( index < list->count );
    if( index == list->count ) {
        list_pop( list );
        return;
    }
    void* dst = (u8*)list->buffer + ( list->stride * index );
    usize remaining_count = list->count - (index + 1);
    void* src = (u8*)list->buffer + ( list->stride * (index + 1) );
    usize size = remaining_count * list->stride;
    mem_copy_overlapped( dst, src, size );
}
LD_API void* list_index( List* list, usize index ) {
    if( index >= list->count ) {
        return NULL;
    }

    return (u8*)list->buffer + (index * list->stride);
}

