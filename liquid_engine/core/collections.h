#if !defined(COLLECTIONS_HPP)
#define COLLECTIONS_HPP
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"
#include "core/strings.h"

typedef struct {
    usize from_inclusive;
    usize to_exclusive;
} Range_usize;

/// Opaque pointer to a list.
typedef void List;

/// Calculate the memory required for a list.
LD_API usize list_calculate_memory_requirement( usize capacity, usize item_size );
/// Create a new list with the given buffer.
/// Buffer must be able to hold the result of list_calculate_memory_requirement.
/// Returns a pointer to the start of the list.
/// Use list_head() to get a pointer to the list buffer.
LD_API List* list_create( usize capacity, usize item_size, void* buffer );
/// Set list capacity.
/// Only use this when list buffer is reallocated.
LD_API void list_set_capacity( List* list, usize new_capacity );
/// Push an item into list.
/// Returns true if there was enough space to push.
LD_API b32 list_push( List* list, void* item );
/// Pop the last item from list.
/// Returns a pointer to the popped item or NULL if list was already empty.
LD_API void* list_pop( List* list );
/// Get a pointer to the last item of the list.
/// Does not remove the item.
LD_API void* list_peek( List* list );
/// Insert an item into list.
/// Returns true if there was enough space to insert.
LD_API b32 list_insert( List* list, usize index, void* item );
/// Remove an item from list.
/// Index MUST be within the bounds of the list.
/// Optionally takes in a pointer to write the value of the removed item to.
LD_API void list_remove( List* list, usize index, void* opt_out_item );
/// Get a pointer to item at given index.
/// Returns NULL if index is outside the bounds of the list.
LD_API void* list_index( List* list, usize index );
/// Set item at given index to the value provided.
/// Index MUST be within the bounds of the list.
LD_API void list_set( List* list, usize index, void* item );
/// Set all items in a list to given item.
LD_API void list_fill( List* list, void* item );
/// Set all items in a list to given item.
/// Does so until it reaches list capacity instead of list count.
/// This function also sets list count equal to list capacity.
LD_API void list_fill_to_capacity( List* list, void* item );
/// Set list count to zero.
LD_API void list_clear( List* list );
/// Get a pointer to the start of the list's buffer.
LD_API void* list_head( List* list );
/// Get how many items are in a list.
LD_API usize list_count( List* list );
/// Get how many items a list can hold.
LD_API usize list_capacity( List* list );
/// Get how large each item is in a list.
LD_API usize list_item_size( List* list );

/// Less than function used for sorting.
typedef b32 SortLTFN( void* lhs, void* rhs, void* params );
/// Swap elements function used for sorting.
typedef void SortSwapFN( void* lhs, void* rhs );
/// Quicksort sorting algorithm implementation.
LD_API void sorting_quicksort(
    isize from_inclusive,
    isize to_inclusive,
    usize element_size,
    void* buffer,
    SortLTFN* lt,
    void* opt_lt_params,
    SortSwapFN* swap
);

/// Dynamic type map.
typedef struct Map {
    void* buffer;
    usize key_size;
    usize value_size;
    usize count;
    usize capacity;
} Map;
/// Create a map.
header_only Map map_create(
    usize key_size, usize value_size,
    usize buffer_size, void* buffer
) {
    usize kv_size  = key_size + value_size;
    assert( buffer_size % kv_size == 0 );
    Map result;
    result.buffer     = buffer;
    result.key_size   = key_size;
    result.value_size = value_size;
    result.capacity   = buffer_size / kv_size;
    result.count      = 0;
    return result;
}
LD_API b32 map_set( Map* map, void* key, void* new_value );
LD_API b32 map_get( Map* map, void* key, void* out_value );
LD_API b32 map_key_exists( Map* map, void* key );
LD_API b32 map_push( Map* map, void* key, void* value );
LD_API b32 map_remove( Map* map, void* key, void* opt_out_value );

typedef struct {
    u32 key, value;
} KV_u32_u32;

typedef struct {
    KV_u32_u32* pairs;
    usize count;
    usize capacity;
} Map_u32_u32;

header_only Map_u32_u32 map_u32_u32_create( usize buffer_size, void* buffer ) {
    assert( buffer_size % sizeof(KV_u32_u32) == 0 );
    usize capacity = buffer_size / sizeof(KV_u32_u32);
    Map_u32_u32 result;

    result.pairs    = (KV_u32_u32*)buffer;
    result.capacity = capacity;
    result.count    = 0;

    return result;
}
LD_API b32 map_u32_u32_set( Map_u32_u32* map, u32 key, u32 new_value );
LD_API b32 map_u32_u32_get( Map_u32_u32* map, u32 key, u32* out_value );
LD_API b32 map_u32_u32_key_exists( Map_u32_u32* map, u32 key );
LD_API b32 map_u32_u32_push( Map_u32_u32* map, u32 key, u32 value );
LD_API b32 map_u32_u32_remove( Map_u32_u32* map, u32 key, u32* opt_out_value );

typedef struct {
    StringSlice key;
    u8 value[];
} KV_StringSlice;

typedef struct {
    void* buffer;
    usize value_size;
    usize count;
    usize capacity;
} MapStringSlice;

header_only MapStringSlice map_ss_create(
    usize value_size, usize buffer_size, void* buffer
) {
    usize kv_size = sizeof(StringSlice) + value_size;
    assert( buffer_size % kv_size == 0 );
    MapStringSlice result;
    result.buffer     = buffer;
    result.value_size = value_size;
    result.capacity   = buffer_size / kv_size;
    result.count      = 0;
    return result;
}
LD_API b32 map_ss_set( MapStringSlice* map, StringSlice key, void* new_value );
LD_API b32 map_ss_get( MapStringSlice* map, StringSlice key, void* out_value );
LD_API b32 map_ss_key_exists( MapStringSlice* map, StringSlice key );
LD_API b32 map_ss_push( MapStringSlice* map, StringSlice key, void* value );
LD_API b32 map_ss_remove(
    MapStringSlice* map, StringSlice key, void* opt_out_value );

#endif // header guard
