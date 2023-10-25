#if !defined(LD_CORE_COLLECTIONS_H)
#define LD_CORE_COLLECTIONS_H
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"

/// Iterator for collections.
typedef struct Iterator {
    void* buffer;
    usize item_size;
    usize count;
    usize current;
} Iterator;
/// Opaque pointer to a list.
typedef void List;

/// Create an iterator for a buffer.
header_only Iterator iterator_create( usize item_size, usize count, void* buffer ) {
    Iterator result;
    result.buffer    = buffer;
    result.item_size = item_size;
    result.count     = count;
    result.current   = 0;
    return result;
}
/// Reset iterator.
header_only void iterator_reset( Iterator* iter ) {
    iter->current = 0;
}
/// Get next item in iterator by reference.
/// Returns null if there are no more items.
LD_API void* iterator_next_enumerate( Iterator* iter, usize* out_enumerator );
/// Get next item in iterator by reference.
/// Returns null if there are no more items.
header_only void* iterator_next( Iterator* iter ) {
    usize tmp = 0;
    return iterator_next_enumerate( iter, &tmp );
}
/// Get next item in iterator by reference in reverse order.
/// Returns null if there are no more items.
LD_API void* iterator_reverse_next_enumerate( Iterator* iter, usize* out_enumerator );
/// Get next item in iterator by reference in reverse order.
/// Returns null if there are no more items.
header_only void* iterator_reverse_next( Iterator* iter ) {
    usize tmp = 0;
    return iterator_reverse_next_enumerate( iter, &tmp );
}
/// Get next item in iterator by value.
/// Returns false if there are no more items.
/// out_item pointer must be the same size as item size.
LD_API b32 iterator_next_value_enumerate(
    Iterator* iter, void* out_item, usize* out_enumerator );
/// Get next item in iterator by value.
/// Returns false if there are no more items.
/// out_item pointer must be the same size as item size.
header_only b32 iterator_next_value( Iterator* iter, void* out_item ) {
    usize tmp = 0;
    return iterator_next_value_enumerate( iter, out_item, &tmp );
}
/// Get next item in iterator by value in reverse order.
/// Returns false if there are no more items.
/// out_item pointer must be the same size as item size.
LD_API b32 iterator_reverse_next_value_enumerate(
    Iterator* iter, void* out_item, usize* out_enumerator );
/// Get next item in iterator by value in reverse order.
/// Returns false if there are no more items.
/// out_item pointer must be the same size as item size.
header_only b32 iterator_reverse_next_value( Iterator* iter, void* out_item ) {
    usize tmp = 0;
    return iterator_reverse_next_value_enumerate( iter, out_item, &tmp );
}
/// Split iterator into two iterators at given index.
LD_API void iterator_split(
    Iterator* iter, usize index, Iterator* out_first, Iterator* out_last );

/// Calculate the memory required for a list.
LD_API usize list_calculate_memory_requirement( usize capacity, usize item_size );
/// Create a new list with the given buffer.
/// Buffer must be able to hold the result of list_calculate_memory_requirement.
/// Returns a pointer to the start of the list.
/// Use list_head() to get a pointer to the list buffer.
LD_API List* list_create( usize capacity, usize item_size, void* buffer );
/// Set list capacity and head.
/// Only use this when list buffer is reallocated.
LD_API List* list_resize( void* head, usize new_capacity );
/// Push an item into list.
/// Returns true if there was enough space to push.
LD_API b32 list_push( List* list, void* item );
/// Append items to the end of a list.
/// Returns true if list had enough capacity to append items.
LD_API b32 list_append( List* list, usize append_count, void* append_items );
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
/// This pointer must be used when reallocating/freeing list buffer.
LD_API void* list_head( List* list );
/// Get how many items are in a list.
LD_API usize list_count( List* list );
/// Get how many items a list can hold.
LD_API usize list_capacity( List* list );
/// Get how large each item is in a list.
LD_API usize list_item_size( List* list );
/// Create an iterator for a list.
LD_API Iterator list_iterator( List* list );

#endif // header guard
