#if !defined(COLLECTIONS_HPP)
#define COLLECTIONS_HPP
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"

struct StringView;
struct Allocator;

/// Hash a string view.
LD_API u64 hash( struct StringView sv );

/// Dynamically allocated list
typedef struct List {
    usize count;
    usize capacity;
    usize stride;
    void* buffer;
    struct Allocator* allocator;
} List;
/// Calculate list buffer size.
headerfn usize list_buffer_size( List* list ) {
    return list->stride * list->capacity;
}
/// Create list using existing buffer.
headerfn List list_from_buffer(
    struct Allocator* opt_allocator, usize stride,
    usize capacity, void* buffer
) {
    List result;
    result.allocator = opt_allocator;
    result.count     = 0;
    result.capacity  = capacity;
    result.stride    = stride;
    result.buffer    = buffer;
    return result;
}
// IMPORTANT(alicia): Internal use only!
/// Create list.
LD_API b32 internal_list_create(
    struct Allocator* allocator, usize capacity,
    usize stride, List* out_list );
// IMPORTANT(alicia): Internal use only!
/// Reallocate list.
LD_API b32 internal_list_realloc( List* list, usize new_capacity );
// IMPORTANT(alicia): Internal use only!
/// Free list.
LD_API void internal_list_free( List* list );
// IMPORTANT(alicia): Internal use only!
/// Push item into list.
/// If there isn't enough space, reallocate list buffer.
LD_API b32 internal_list_push_realloc( List* list, void* item, usize realloc );
// IMPORTANT(alicia): Internal use only!
/// Insert item into list at index.
/// If there isn't enough space, reallocate list buffer.
LD_API b32 internal_list_insert_realloc(
    List* list, void* item, usize index, usize realloc );

// IMPORTANT(alicia): Internal use only!
/// Create list.
LD_API b32 internal_list_create_trace(
    struct Allocator* allocator, usize capacity,
    usize stride, List* out_list,
    const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Reallocate list.
LD_API b32 internal_list_realloc_trace(
    List* list, usize new_capacity,
    const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Free list.
LD_API void internal_list_free_trace(
    List* list, const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Push item into list. If there isn't enough space, reallocate list buffer.
LD_API b32 internal_list_push_realloc_trace(
    List* list, void* item, usize realloc,
    const char* function, const char* file, int line );
// IMPORTANT(alicia): Internal use only!
/// Insert item into list at index.
/// If there isn't enough space, reallocate list buffer.
LD_API b32 internal_list_insert_realloc_trace(
    List* list, void* item, usize index, usize realloc,
    const char* function, const char* file, int line );

#if defined(LD_LOGGING)
    /// Create a new list.
    #define list_create( allocator, capacity, stride, out_list )\
        internal_list_create_trace( allocator, capacity, stride, out_list,\
            __FUNCTION__, __FILE__, __LINE__ )
    /// Create a new list. Takes a type instead of stride.
    #define list_create_typed( allocator, capacity, type, out_list )\
        internal_list_create_trace(\
            allocator, capacity, sizeof(type), out_list,\
            __FUNCTION__, __FILE__, __LINE__ )
    /// Reallocate list.
    #define list_realloc( list, new_capacity )\
        internal_list_realloc_trace( list, new_capacity,\
            __FUNCTION__, __FILE__, __LINE__ )
    /// Free list.
    #define list_free( list )\
        internal_list_free_trace( list, __FUNCTION__, __FILE__, __LINE__ )
    /// Push item into list.
    /// If there isn't enough space, reallocate list buffer.
    #define list_push_realloc( list, item, realloc )\
        internal_list_push_realloc_trace( list, item, realloc,\
            __FUNCTION__, __FILE__, __LINE__)
    /// Insert item into list at index.
    /// If there isn't enough space, reallocate list buffer.
    #define list_insert_realloc( list, item, index, realloc )\
        internal_list_insert_realloc_trace( list, item, index, realloc,\
            __FUNCTION__, __FILE__, __LINE__)
#else
    /// Create a new list.
    #define list_create( allocator, capacity, stride, out_list )\
        internal_list_create( allocator, capacity, stride, out_list )
    /// Create a new list. Takes a type instead of a stride.
    #define list_create_typed( allocator, capacity, type, out_list )\
        internal_list_create(\
            allocator, capacity, sizeof(type), out_list )
    /// Reallocate list.
    #define list_realloc( list, new_capacity )\
        internal_list_realloc( list, new_capacity )
    /// Free list.
    #define list_free( list )\
        internal_list_free( list )
    /// Push item into list.
    /// If there isn't enough space, reallocate list buffer.
    #define list_push_realloc( list, item, realloc )\
        internal_list_push_realloc( list, item, realloc )
    /// Insert item into list at index.
    /// If there isn't enough space, reallocate list buffer.
    #define list_insert_realloc( list, item, index, realloc )\
        internal_list_insert_realloc( list, item, index, realloc )
#endif

/// Push item into list.
LD_API b32 list_push( List* list, void* item );
/// Insert item into list at index.
LD_API b32 list_insert( List* list, void* item, usize index );
/// Pop last item from list.
LD_API void* list_pop( List* list );
/// Remove item from list at index.
LD_API void list_remove( List* list, usize index );
/// Get item at index from list.
LD_API void* list_index( List* list, usize index );

#endif
