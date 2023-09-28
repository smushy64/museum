#if !defined(LD_CORE_COLLECTIONS_H)
#define LD_CORE_COLLECTIONS_H
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"

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

#endif // header guard
