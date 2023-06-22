#if !defined(COLLECTIONS_HPP)
#define COLLECTIONS_HPP
/**
 * Description:  Collections
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"

// LIST | BEGIN -------------------------------------------

namespace impl {

#define LIST_FIELD_CAPACITY 0
#define LIST_FIELD_COUNT    1
#define LIST_FIELD_STRIDE   2

LD_API void* _list_create( usize capacity, usize stride );
LD_API void* _list_realloc( void* list, usize new_capacity );
LD_API void  _list_free( void* list );

LD_API void* _list_append(
    void* list,
    usize append_count,
    const void* pvalue
);

LD_API void* _list_push( void* list, const void* pvalue );
LD_API b32   _list_pop( void* list, void* dst );

LD_API usize _list_field_read( void* list, u32 field );
LD_API void _list_field_write(
    void* list,
    u32 field,
    usize value
);

LD_API void* _list_remove(
    void* list,
    usize index,
    void* opt_dst
);

LD_API void* _list_insert(
    void* list,
    usize index,
    void* pvalue
);

LD_API void* _list_create_trace(
    usize capacity,
    usize stride,
    const char* function,
    const char* file,
    int line
);

LD_API void* _list_realloc_trace(
    void* list,
    usize new_capacity,
    const char* function,
    const char* file,
    int line
);

LD_API void* _list_push_trace(
    void* list,
    const void* pvalue,
    const char* function,
    const char* file,
    int line
);

LD_API void* _list_append_trace(
    void* list,
    usize append_count,
    const void* pvalue,
    const char* function,
    const char* file,
    int line
);

LD_API void* _list_insert_trace(
    void* list,
    usize index,
    void* pvalue,
    const char* function,
    const char* file,
    int line
);

LD_API void _list_free_trace(
    void* list,
    const char* function,
    const char* file,
    int line
);

} // namespace impl

#define LIST_DEFAULT_CAPACITY 1

#if defined(LD_LOGGING)
    #define list_create(type)\
        (type*) ::impl::_list_create_trace(\
            LIST_DEFAULT_CAPACITY,\
            sizeof(type),\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )

    #define list_realloc(list, new_capacity)\
        (__typeof(list)) ::impl::_list_realloc_trace(\
            list,\
            new_capacity,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )

    #define list_free(list)\
        ::impl::_list_free_trace(\
            list,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )

    #define list_reserve(type, capacity)\
        (type*) ::impl::_list_create_trace(\
            capacity,\
            sizeof(type),\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )

    #define list_push(list, value) do {\
        __typeof(value) temp = value;\
        list = (__typeof(list)) ::impl::_list_push_trace(\
            list,\
            &temp,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
    } while(0)

    #define list_append(list, append_count, pvalue) do {\
        list = (__typeof(list)) ::impl::_list_append_trace(\
            list,\
            append_count,\
            pvalue,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
    } while(0)

    #define list_insert(list, index, value) do {\
        __typeof(value) temp = value;\
        list = (__typeof(list)) ::impl::_list_insert_trace(\
            list,\
            index,\
            &temp,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
    } while(0)

#else

    #define list_create(type)\
        (type*) ::impl::_list_create(\
            LIST_DEFAULT_CAPACITY,\
            sizeof(type)\
        )

    #define list_realloc(list, new_capacity)\
        ::impl::_list_realloc( list, new_capacity )

    #define list_free(list)\
        ::impl::_list_free(list)

    #define list_reserve(type, capacity)\
        (type*) ::impl::_list_create(\
            capacity,\
            sizeof(type)\
        )

    #define list_push(list, value) do {\
        __typeof(value) temp = value;\
        list = (__typeof(list)) ::impl::_list_push(\
            list, &temp);\
    } while(0)

    #define list_append(list, append_count, pvalue) do {\
        list = (__typeof(list)) ::impl::_list_append(\
            list,\
            append_count,\
            pvalue\
        );\
    } while(0)

    #define list_insert(list, index, value) do {\
        __typeof(value) temp = value;\
        list = (__typeof(list)) ::impl::_list_insert(\
            list,\
            index,\
            &temp\
        );\
    } while(0)
#endif



#define list_pop(list, pvalue)\
    ::impl::_list_pop( list, pvalue )

#define list_remove(list, index, opt_dst)\
    ::impl::_list_remove( list, index, opt_dst )

#define list_clear(list)\
    ::impl::_list_field_write(\
        list,\
        ::impl::LIST_FIELD_COUNT,\
        0\
    )\

#define list_count(list)\
    ::impl::_list_field_read(\
        list,\
        ::impl::LIST_FIELD_COUNT\
    )

#define list_capacity(list)\
    ::impl::_list_field_read(\
        list,\
        ::impl::LIST_FIELD_CAPACITY\
    )

#define list_stride(list)\
    ::impl::_list_field_read(\
        list,\
        ::impl::LIST_FIELD_STRIDE\
    )

#define list_size(list)\
    (list_capacity(list) * list_stride(list))

#define list_set_count(list, value)\
    ::impl::_list_field_write(\
        list,\
        ::impl::LIST_FIELD_COUNT,\
        value\
    )

// LIST | END ---------------------------------------------

#endif
