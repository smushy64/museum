#if !defined(COLLECTIONS_HPP)
#define COLLECTIONS_HPP
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"

namespace internal {

SM_GLOBAL const u32 LIST_FIELD_CAPACITY = 0;
SM_GLOBAL const u32 LIST_FIELD_COUNT    = 1;
SM_GLOBAL const u32 LIST_FIELD_STRIDE   = 2;

SM_API void* impl_list_create( usize capacity, usize stride );
SM_API void* impl_list_realloc( void* list, usize new_capacity );
SM_API void  impl_list_free( void* list );

SM_API void* impl_list_push( void* list, const void* pvalue );
SM_API b32   impl_list_pop( void* list, void* dst );

SM_API usize impl_list_field_read( void* list, u32 field );
SM_API void impl_list_field_write(
    void* list,
    u32 field,
    usize value
);

SM_API void* impl_list_remove(
    void* list,
    usize index,
    void* dst
);
SM_API void* impl_list_insert(
    void* list,
    usize index,
    void* pvalue
);

} // namespace internal

#define LIST_DEFAULT_CAPACITY 1

#define list_create(type)\
    (type*) ::internal::impl_list_create(\
        LIST_DEFAULT_CAPACITY,\
        sizeof(type)\
    )
#define list_realloc(list, new_capacity)\
    ::internal::impl_list_realloc( list, new_capacity )
#define list_free(list)\
    ::internal::impl_list_free(list)

#define list_reserve(type, capacity)\
    (type*) ::internal::impl_list_create(\
        capacity,\
        sizeof(type)\
    )

#define list_push(list, value) do {\
        __typeof(value) temp = value;\
        list = (__typeof(value)*) ::internal::impl_list_push(list, &temp);\
    } while(0)

#define list_pop(list, pvalue)\
    ::internal::impl_list_pop( list, pvalue )

#define list_insert(list, index, value) do {\
        __typeof(value) temp = value;\
        list = (__typeof(list)) ::internal::impl_list_insert(\
            list,\
            index,\
            &temp\
        );\
    } while(0)

#define list_remove(list, index, opt_dst)\
    ::internal::impl_list_remove( list, index, opt_dst )

#define list_clear(list)\
    ::internal::impl_list_field_write(\
        list,\
        ::internal::LIST_FIELD_COUNT,\
        0\
    )\

#define list_count(list)\
    ::internal::impl_list_field_read(\
        list,\
        ::internal::LIST_FIELD_COUNT\
    )

#define list_capacity(list)\
    ::internal::impl_list_field_read(\
        list,\
        ::internal::LIST_FIELD_CAPACITY\
    )

#define list_stride(list)\
    ::internal::impl_list_field_read(\
        list,\
        ::internal::LIST_FIELD_STRIDE\
    )

#define list_size(list)\
    (list_capacity(list) * list_stride(list))

#define list_set_count(list, value)\
    ::internal::impl_list_field_write(\
        list,\
        ::internal::LIST_FIELD_COUNT,\
        value\
    )

#endif