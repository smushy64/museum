#if !defined(LD_SHARED_CORE_COLLECTIONS_H)
#define LD_SHARED_CORE_COLLECTIONS_H
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "shared/defines.h"

struct StringSlice;
CORE_API u64 cstr_hash( usize opt_len, const char* str );

/// Iterator for collections.
typedef struct Iterator {
    void* buffer;
    usize item_size;
    usize count;
    usize current;
} Iterator;
/// Dynamic list.
typedef struct List {
    void* buffer;
    usize item_size;
    usize count;
    usize capacity;
} List;

/// Type of key value pair's value.
typedef enum KValueType : u64 {
    KVALUE_TYPE_U8,
    KVALUE_TYPE_U8_2,
    KVALUE_TYPE_U8_3,
    KVALUE_TYPE_U8_4,
    KVALUE_TYPE_U8_5,
    KVALUE_TYPE_U8_6,
    KVALUE_TYPE_U8_7,
    KVALUE_TYPE_U8_8,
    KVALUE_TYPE_U16,
    KVALUE_TYPE_U16_2,
    KVALUE_TYPE_U16_3,
    KVALUE_TYPE_U16_4,
    KVALUE_TYPE_U32,
    KVALUE_TYPE_U32_2,
    KVALUE_TYPE_U64,
    KVALUE_TYPE_I8,
    KVALUE_TYPE_I8_2,
    KVALUE_TYPE_I8_3,
    KVALUE_TYPE_I8_4,
    KVALUE_TYPE_I8_5,
    KVALUE_TYPE_I8_6,
    KVALUE_TYPE_I8_7,
    KVALUE_TYPE_I8_8,
    KVALUE_TYPE_I16,
    KVALUE_TYPE_I16_2,
    KVALUE_TYPE_I16_3,
    KVALUE_TYPE_I16_4,
    KVALUE_TYPE_I32,
    KVALUE_TYPE_I32_2,
    KVALUE_TYPE_I64,
    KVALUE_TYPE_F32,
    KVALUE_TYPE_F32_2,
    KVALUE_TYPE_F64,
    KVALUE_TYPE_STRING_HASH,
    KVALUE_TYPE_STRING_8,
    KVALUE_TYPE_POINTER,
    KVALUE_TYPE_USIZE,
    KVALUE_TYPE_ISIZE,
} KValueType;
header_only const char* kvalue_type_to_cstr( KValueType type, usize* opt_out_len );

/// Hashmap key.
typedef u64 Key;

/// Hashmap value.
typedef struct KValue {
    KValueType type;
    union {
        u8  uint8;
        u8  uint8_array[8];
        u16 uint16;
        u16 uint16_array[4];
        u32 uint32;
        u32 uint32_array[2];
        u64 uint64;
        i8  int8;
        i8  int8_array[8];
        i16 int16;
        i16 int16_array[4];
        i32 int32;
        i32 int32_array[2];
        i64 int64;
        f32 float32;
        f32 float32_array[2];
        f64 float64;
        u64 string_hash;
        char string8[8];
        void* pointer;
        usize uintptr;
        isize intptr;
    };
} KValue;

/// Compare two values for type and value.
header_only b32 kvalue_cmp( KValue a, KValue b ) {
    return a.type == b.type && a.uint64 == b.uint64;
}

/// Hashmap.
typedef struct Hashmap {
    Key*    keys;
    KValue* values;
    usize   count;
    usize   capacity;
    Key     largest_key;
} Hashmap;

/// Create a hashmap key from null-terminated string.
#define hashmap_key_cstr( opt_len, str ) cstr_hash( opt_len, str )
/// Create a hashmap key from string literal.
#define hashmap_key_text( text )         cstr_hash( sizeof(text) - 1, text )
/// Create a hashmap key from pointer to string slice.
#define hashmap_key_string_slice( s )    cstr_hash( (s)->len, (s)->buffer )

/// Calculate memory requirement of a hashmap with given capcity.
/// Result is enough to store n number of keys and values in the same buffer.
header_only usize hashmap_memory_requirement( usize capacity ) {
    return capacity * sizeof(Key) + (capacity * sizeof(KValue));
}
/// Calculate memory requirement of a hashmap with given capcity.
/// out_key_buffer_size is enough to store n number of keys.
/// out_value_buffer_size is enough to store n number of values.
/// If you want to resize the hashmap buffer,
/// the key and value buffers need to be separate unless you manually
/// relocate values after reallocating.
header_only void hashmap_memory_requirement_separate(
    usize capacity, usize* out_key_buffer_size, usize* out_value_buffer_size
) {
    *out_key_buffer_size   = capacity * sizeof(Key);
    *out_value_buffer_size = capacity * sizeof(KValue);
}
/// Create a hashmap with a shared buffer.
header_only Hashmap hashmap_create( usize capacity, void* buffer ) {
    Hashmap result = {};
    result.capacity = capacity;
    result.keys     = (Key*)buffer;
    result.values   = (KValue*)((u8*)buffer + (capacity * sizeof(Key)));
    return result;
}
/// Create a hashmap with separate buffers for keys and values.
header_only Hashmap hashmap_create_separate(
    usize capacity, void* key_buffer, void* value_buffer
) {
    Hashmap result  = {};
    result.capacity = capacity;
    result.keys     = (Key*)key_buffer;
    result.values   = (KValue*)value_buffer;
    return result;
}
/// Insert a value into hashmap using a key.
/// Returns true if there is enough space in hashmap.
/// If check_for_duplicate, also returns true if key already exists.
CORE_API b32 hashmap_insert_key(
    Hashmap* hashmap, Key key, KValue value, b32 check_for_duplicate );

/// Get value of given key.
/// Returns NULL if KV pair with given key doesn't exist.
CORE_API KValue* hashmap_get( Hashmap* hashmap, Key key );
/// Get the index of the item with given key.
header_only b32 hashmap_get_index( Hashmap* hashmap, Key key, usize* out_index ) {
    KValue* value = hashmap_get( hashmap, key );
    if( !value ) {
        return false;
    }

    *out_index = value - hashmap->values;
    return true;
}
/// Check if key exists in hashmap.
/// This is the same as hashmap_get_key except it doesn't
/// return a pointer to the key's value.
header_only b32 hashmap_contains_key( Hashmap* hashmap, Key key ) {
    return hashmap_get( hashmap, key ) != NULL;
}
/// Remove an item by its index.
/// If opt_out_value is not null, the value at the given key is copied to it.
CORE_API void hashmap_remove_by_index(
    Hashmap* hashmap, usize index, KValue* opt_out_value );
/// Attempt to remove an item by its key.
/// Returns true if key exists in hashmap.
/// If opt_out_value is not null, the value at the given key is copied to it.
header_only b32 hashmap_remove(
    Hashmap* hashmap, Key key, KValue* opt_out_value
) {
    usize index = 0;
    if( !hashmap_get_index( hashmap, key, &index ) ) {
        return false;
    }

    hashmap_remove_by_index( hashmap, index, opt_out_value );
    return true;
}
/// Returns true if hashmap is full.
header_only b32 hashmap_is_full( Hashmap* hashmap ) {
    return hashmap->count == hashmap->capacity;
}
/// Returns true if hashmap is empty.
header_only b32 hashmap_is_empty( Hashmap* hashmap ) {
    return hashmap->count == 0;
}
/// Clear a hashmap.
/// Does not deallocate memory, just sets hashmap count to zero.
header_only void hashmap_clear( Hashmap* hashmap ) {
    hashmap->count = 0;
}

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
CORE_API void* iterator_next_enumerate( Iterator* iter, usize* out_enumerator );
/// Get next item in iterator by reference.
/// Returns null if there are no more items.
header_only void* iterator_next( Iterator* iter ) {
    usize tmp = 0;
    return iterator_next_enumerate( iter, &tmp );
}
/// Get next item in iterator by reference in reverse order.
/// Returns null if there are no more items.
CORE_API void* iterator_reverse_next_enumerate( Iterator* iter, usize* out_enumerator );
/// Get next item in iterator by reference in reverse order.
/// Returns null if there are no more items.
header_only void* iterator_reverse_next( Iterator* iter ) {
    usize tmp = 0;
    return iterator_reverse_next_enumerate( iter, &tmp );
}
/// Get next item in iterator by value.
/// Returns false if there are no more items.
/// out_item pointer must be the same size as item size.
CORE_API b32 iterator_next_value_enumerate(
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
CORE_API b32 iterator_reverse_next_value_enumerate(
    Iterator* iter, void* out_item, usize* out_enumerator );
/// Get next item in iterator by value in reverse order.
/// Returns false if there are no more items.
/// out_item pointer must be the same size as item size.
header_only b32 iterator_reverse_next_value( Iterator* iter, void* out_item ) {
    usize tmp = 0;
    return iterator_reverse_next_value_enumerate( iter, out_item, &tmp );
}
/// Split iterator into two iterators at given index.
CORE_API void iterator_split(
    Iterator* iter, usize index, Iterator* out_first, Iterator* out_last );

/// Create a new list with the given buffer.
header_only List list_create( usize capacity, usize item_size, void* buffer ) {
    List result = {};
    result.buffer    = buffer;
    result.count     = 0;
    result.capacity  = capacity;
    result.item_size = item_size;
    return result;
}
/// Use this function after reallocating list buffer
/// to change capacity and buffer pointer.
header_only void list_resize( List* list, usize new_capacity, void* new_buffer ) {
    list->buffer   = new_buffer;
    list->capacity = new_capacity;
}
/// Set list count to zero.
header_only void list_clear( List* list ) {
    list->count = 0;
}
/// Push an item into list.
/// Returns true if there was enough space to push.
CORE_API b32 list_push( List* list, void* item );
/// Append items to the end of a list.
/// Returns true if list had enough capacity to append items.
CORE_API b32 list_append( List* list, usize append_count, void* append_items );
/// Pop the last item from list.
/// Returns a pointer to the popped item or NULL if list was already empty.
CORE_API void* list_pop( List* list );
/// Pop the last item from list.
/// Copies value to pointer out_item.
/// Returns false if list was empty.
CORE_API b32 list_pop_value( List* list, void* out_item );
/// Get a pointer to the last item of the list.
/// Returns NULL if list is empty.
/// Does not remove the item.
CORE_API void* list_peek( List* list );
/// Insert an item into list.
/// Returns true if there was enough space to insert.
CORE_API b32 list_insert( List* list, usize index, void* item );
/// Remove an item from list.
/// Index MUST be within the bounds of the list.
/// Optionally takes in a pointer to write the value of the removed item to.
CORE_API void list_remove( List* list, usize index, void* opt_out_item );
/// Get a pointer to item at given index.
/// Returns NULL if index is outside the bounds of the list.
CORE_API void* list_index( List* list, usize index );
/// Get item at given index.
/// Copies value to pointer out_item.
/// Returns false if index is outside the bounds of the list.
CORE_API b32 list_index_value( List* list, usize index, void* out_item );
/// Set item at given index to the value provided.
/// Index MUST be within the bounds of the list.
CORE_API void list_set( List* list, usize index, void* item );
/// Set all items in a list to given item.
CORE_API void list_fill( List* list, void* item );
/// Set all items in a list to given item.
/// Does so until it reaches list capacity instead of list count.
/// This function also sets list count equal to list capacity.
CORE_API void list_fill_to_capacity( List* list, void* item );
/// Create an iterator for a list.
CORE_API Iterator list_iterator( List* list );

header_only const char* kvalue_type_to_cstr( KValueType type, usize* opt_out_len ) {
    #define result( str )\
        if( opt_out_len ) *opt_out_len = sizeof(str) - 1; return str

    switch( type ) {
        case KVALUE_TYPE_U8:    result("u8");
        case KVALUE_TYPE_U8_2:  result("u8[2]");
        case KVALUE_TYPE_U8_3:  result("u8[3]");
        case KVALUE_TYPE_U8_4:  result("u8[4]");
        case KVALUE_TYPE_U8_5:  result("u8[5]");
        case KVALUE_TYPE_U8_6:  result("u8[6]");
        case KVALUE_TYPE_U8_7:  result("u8[7]");
        case KVALUE_TYPE_U8_8:  result("u8[8]");
        case KVALUE_TYPE_U16:   result("u16");
        case KVALUE_TYPE_U16_2: result("u16[2]");
        case KVALUE_TYPE_U16_3: result("u16[3]");
        case KVALUE_TYPE_U16_4: result("u16[4]");
        case KVALUE_TYPE_U32:   result("u32");
        case KVALUE_TYPE_U32_2: result("u32[2]");
        case KVALUE_TYPE_U64:   result("u64");
        case KVALUE_TYPE_I8:    result("i8");
        case KVALUE_TYPE_I8_2:  result("i8[2]");
        case KVALUE_TYPE_I8_3:  result("i8[3]");
        case KVALUE_TYPE_I8_4:  result("i8[4]");
        case KVALUE_TYPE_I8_5:  result("i8[5]");
        case KVALUE_TYPE_I8_6:  result("i8[6]");
        case KVALUE_TYPE_I8_7:  result("i8[7]");
        case KVALUE_TYPE_I8_8:  result("i8[8]");
        case KVALUE_TYPE_I16:   result("i16");
        case KVALUE_TYPE_I16_2: result("i16[2]");
        case KVALUE_TYPE_I16_3: result("i16[3]");
        case KVALUE_TYPE_I16_4: result("i16[4]");
        case KVALUE_TYPE_I32:   result("i32");
        case KVALUE_TYPE_I32_2: result("i32[2]");
        case KVALUE_TYPE_I64:   result("i64");
        case KVALUE_TYPE_F32:   result("f32");
        case KVALUE_TYPE_F32_2: result("f32[2]");
        case KVALUE_TYPE_F64:   result("f64");
        case KVALUE_TYPE_STRING_HASH: result("string hash(u64)");
        case KVALUE_TYPE_STRING_8:    result("char[8]");
        case KVALUE_TYPE_ISIZE:       result("isize");
        case KVALUE_TYPE_USIZE:       result("usize");
        case KVALUE_TYPE_POINTER:     result("void*");
          break;
    }
    #undef result
}

#define kvalue_u8( x )  (KValue){ KVALUE_TYPE_U8,  .uint8=(x) }
#define kvalue_u8_2( _0, _1 ) (KValue){ KVALUE_TYPE_U8_2, .uint8_array={ _0, _1 } }
#define kvalue_u8_3( _0, _1, _2 ) (KValue){ KVALUE_TYPE_U8_3, .uint8_array={ _0, _1, _2 } }
#define kvalue_u8_4( _0, _1, _2, _3 ) (KValue){ KVALUE_TYPE_U8_4, .uint8_array={ _0, _1, _2, _3 } }
#define kvalue_u8_5( _0, _1, _2, _3, _4 ) (KValue){ KVALUE_TYPE_U8_5, .uint8_array={ _0, _1, _2, _3, _4 } }
#define kvalue_u8_6( _0, _1, _2, _3, _4, _5 ) (KValue){ KVALUE_TYPE_U8_6, .uint8_array={ _0, _1, _2, _3, _4, _5 } }
#define kvalue_u8_7( _0, _1, _2, _3, _4, _5, _6 ) (KValue){ KVALUE_TYPE_U8_7, .uint8_array={ _0, _1, _2, _3, _4, _5, _6 } }
#define kvalue_u8_8( _0, _1, _2, _3, _4, _5, _6, _7 ) (KValue){ KVALUE_TYPE_U8_8, .uint8_array={ _0, _1, _2, _3, _4, _5, _6, _7 } }

#define kvalue_u16( x ) (KValue){ KVALUE_TYPE_U16, .uint16=(x) }
#define kvalue_u16_2( _0, _1 ) (KValue){ KVALUE_TYPE_U16_2, .uint16_array={ _0, _1 } }
#define kvalue_u16_3( _0, _1, _2 ) (KValue){ KVALUE_TYPE_U16_3, .uint16_array={ _0, _1, _2 } }
#define kvalue_u16_4( _0, _1, _2, _3 ) (KValue){ KVALUE_TYPE_U16_4, .uint16_array={ _0, _1, _2, _3 } }

#define kvalue_u32( x ) (KValue){ KVALUE_TYPE_U32, .uint32=(x) }
#define kvalue_u32_2( _0, _1 ) (KValue){ KVALUE_TYPE_U32_2, uint32_array={ _0, _1 } }

#define kvalue_u64( x ) (KValue){ KVALUE_TYPE_U64, .uint64=(x) }

#define kvalue_i8( x )  (KValue){ KVALUE_TYPE_I8,  .int8=(x) }
#define kvalue_i8_2( _0, _1 ) (KValue){ KVALUE_TYPE_I8_2, .int8_array={ _0, _1 } }
#define kvalue_i8_3( _0, _1, _2 ) (KValue){ KVALUE_TYPE_I8_3, .int8_array={ _0, _1, _2 } }
#define kvalue_i8_4( _0, _1, _2, _3 ) (KValue){ KVALUE_TYPE_I8_4, .int8_array={ _0, _1, _2, _3 } }
#define kvalue_i8_5( _0, _1, _2, _3, _4 ) (KValue){ KVALUE_TYPE_I8_5, .int8_array={ _0, _1, _2, _3, _4 } }
#define kvalue_i8_6( _0, _1, _2, _3, _4, _5 ) (KValue){ KVALUE_TYPE_I8_6, .int8_array={ _0, _1, _2, _3, _4, _5 } }
#define kvalue_i8_7( _0, _1, _2, _3, _4, _5, _6 ) (KValue){ KVALUE_TYPE_I8_7, .int8_array={ _0, _1, _2, _3, _4, _5, _6 } }
#define kvalue_i8_8( _0, _1, _2, _3, _4, _5, _6, _7 ) (KValue){ KVALUE_TYPE_I8_8, .int8_array={ _0, _1, _2, _3, _4, _5, _6, _7 } }

#define kvalue_i16( x ) (KValue){ KVALUE_TYPE_I16, .int16=(x) }
#define kvalue_i16_2( _0, _1 ) (KValue){ KVALUE_TYPE_I16_2, .int16_array={ _0, _1 } }
#define kvalue_i16_3( _0, _1, _2 ) (KValue){ KVALUE_TYPE_I16_3, .int16_array={ _0, _1, _2 } }
#define kvalue_i16_4( _0, _1, _2, _3 ) (KValue){ KVALUE_TYPE_I16_4, .int16_array={ _0, _1, _2, _3 } }

#define kvalue_i32( x )   (KValue){ KVALUE_TYPE_I32, .int32=(x) }
#define kvalue_i32_2( _0, _1 ) (KValue){ KVALUE_TYPE_I32_2, .int32_array={ _0, _1 } }

#define kvalue_i64( x ) (KValue){ KVALUE_TYPE_U64, .int64=(x) }

#define kvalue_f32( x )   (KValue){ KVALUE_TYPE_F32, .float32=(x) }
#define kvalue_f32_2( x ) (KValue){ KVALUE_TYPE_F32_2, .float32_array={ _0, _1 } }

#define kvalue_f64( x ) (KValue){ KVALUE_TYPE_F64, .float64=(x) }
#define kvalue_str8( str ) (KValue){ KVALUE_TYPE_STRING_8, .string8={ str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7] } }; static_assert( static_array_count(str) == 8, "string must be exactly 8 characters long!" )
#define kvalue_string_slice( slice ) (KValue){ KVALUE_TYPE_STRING_HASH, .string_hash=string_slice_hash( slice ) }
#define kvalue_cstr( opt_len, str ) (KValue){ KVALUE_TYPE_STRING_HASH, .string_hash=cstr_hash( opt_len, str ) }
#define kvalue_text( text ) kvalue_cstr_hash( sizeof(text), text )

#endif // header guard
