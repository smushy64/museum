/**
 * Description:  String functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "string.h"
#include "logging.h"
#include "memory.h"

// TODO(alicia): custom format functions!
#include <stdio.h>
#include <stdarg.h>

inline internal b32 dstring_allocate( u32 capacity, String* out_string ) {
    void* buffer = mem_alloc( capacity, MEMTYPE_STRING );
    if( !buffer ) {
        return false;
    }
    out_string->buffer   = (char*)buffer;
    out_string->capacity = capacity;
    return true;
}
inline internal b32 dstring_reallocate( String* string, u32 new_capacity ) {
    void* new_buffer = mem_realloc( string->buffer, new_capacity );
    if( !new_buffer ) {
        return false;
    }
    string->buffer   = (char*)new_buffer;
    string->capacity = new_capacity;
    return true;
}
inline internal b32 dstring_append_internal(
    String* string,
    u32 append_len, const char* append_buffer,
    b32 alloc
) {
    u32 new_len = string->len + append_len;
    u32 max_len = new_len > string->capacity ? string->capacity : new_len;
    if( alloc ) {
        if( string->capacity < new_len ) {
            if( !dstring_reallocate( string, new_len ) ) {
                return false;
            }
            max_len = new_len;
        }
    }

    mem_copy( string->buffer + string->len, append_buffer, max_len );
    string->len = max_len;

    return true;
}

LD_API b32 dstring_new( StringView view, String* out_string ) {
    u32 capacity = view.len;
    if( !dstring_allocate( capacity, out_string ) ) {
        return false;
    }
    out_string->len = capacity;
    mem_copy( out_string->buffer, view.buffer, view.len );

    return true;
}
LD_API b32 dstring_with_capacity( u32 capacity, String* out_string ) {
    out_string->len = 0;
    return dstring_allocate( capacity, out_string );
}
LD_API b32 dstring_reserve( String* string, u32 new_capacity ) {
    return dstring_reallocate( string, new_capacity );
}
LD_API b32 dstring_append( String* string, String* append, b32 alloc ) {
    return dstring_append_internal(
        string,
        append->len,
        append->buffer,
        alloc
    );
}
LD_API b32 dstring_append( String* string, StringView append, b32 alloc ) {
    return dstring_append_internal(
        string,
        append.len,
        append.buffer,
        alloc
    );
}
LD_API void dstring_free( String* string ) {
    if( string->buffer ) {
        mem_free( string->buffer );
    }
    *string = {};
}
LD_API b32 dstring_format(
    String* string,
    b32 alloc,
    const char* format,
    ...
) {
    va_list va;
    va_start( va, format );
    int required_size = vsnprintf(
        0, 0,
        format,
        va
    );
    va_end(va);

    if( alloc ) {
        if( string->capacity < (u32)required_size ) {
            if( !dstring_reallocate( string, (u32)required_size ) ) {
                return false;
            }
        }
    }

    va_start( va, format );
    vsnprintf(
        string->buffer, string->capacity,
        format, va
    );
    va_end( va );

    string->len = str_length( string->buffer );

    return true;
}
LD_API u32 string_view_format(
    StringView string_view,
    const char* format,
    ...
) {
    va_list va;
    va_start( va, format );
    if( !string_view.len ) {
        int required_size = vsnprintf(
            0, 0,
            format,
            va
        );
        va_end(va);
        return (u32)required_size;
    } else {
        int required_size = vsnprintf(
            string_view.buffer,
            string_view.len,
            format,
            va
        );
        va_end( va );
        return (u32)required_size;
    }
}

inline internal b32 string_cmp_internal(
    u32 a_len, const char* a_buffer,
    u32 b_len, const char* b_buffer
) {
    if( a_len != b_len ) {
        return false;
    }

    // TODO(alicia): traverse u64 instead
    for( u32 i = 0; i < a_len; ++i ) {
        if( a_buffer[i] != b_buffer[i] ) {
            return false;
        }
    }

    return true;
}

LD_API b32 string_cmp( String* a, String* b ) {
    return string_cmp_internal(
        a->len, a->buffer,
        b->len, b->buffer
    );
}
LD_API b32 string_cmp( String* a, StringView  b ) {
    return string_cmp_internal(
        a->len, a->buffer,
        b.len, b.buffer
    );
}
LD_API b32 string_cmp( StringView  a, String* b ) {
    return string_cmp_internal(
        a.len, a.buffer,
        b->len, b->buffer
    );
}
LD_API b32 string_cmp( StringView  a, StringView  b ) {
    return string_cmp_internal(
        a.len, a.buffer,
        b.len, b.buffer
    );
}

inline internal void string_copy_internal(
    u32 src_len, const char* src_buffer,
    u32 dst_capacity, char* dst_buffer
) {
    u32 max_len = src_len > dst_capacity ? dst_capacity : src_len;
    mem_copy( dst_buffer, src_buffer, max_len );
}
LD_API void string_copy( String* src, String* dst ) {
    string_copy_internal(
        src->len, src->buffer,
        dst->capacity, dst->buffer
    );
    if( src->len > dst->len ) {
        dst->len = dst->capacity;
    }
}
LD_API void string_copy( StringView src, String* dst ) {
    string_copy_internal(
        src.len, src.buffer,
        dst->capacity, dst->buffer
    );
    if( src.len > dst->len ) {
        dst->len = dst->capacity;
    }
}
LD_API void string_trim_trailing_whitespace( String* string ) {
    for( u32 i = 0; i < string->len; ++i ) {
        if( char_is_whitespace( string->buffer[i] ) ) {
            string->len = i;
            return;
        }
    }
}
LD_API void string_trim_trailing_whitespace( StringView string_view ) {
    for( u32 i = 0; i < string_view.len; ++i ) {
        if( char_is_whitespace( string_view.buffer[i] ) ) {
            string_view.len = i;
            return;
        }
    }
}
LD_API b32 dstring_push_char( String* string, char character, u32 realloc ) {
    if( string->len == string->capacity ) {
        if( realloc ) {
            if( !dstring_reserve( string, string->capacity + realloc ) ) {
                return false;
            }
        } else {
            return true;
        }
    }

    string->buffer[string->len++] = character;
    return true;
}
LD_API char dstring_pop_char( String* string ) {
    if( !string->len ) {
        return 0;
    }
    return string->buffer[string->len--];
}
LD_API b32 str_view(
    u32 str_len,
    const char* str,
    u32 offset, u32 len,
    StringView* out_view
) {
    if( offset >= str_len ) {
        return false;
    }
    u32 view_end = offset + len;
    if( view_end >= str_len ) {
        return false;
    }

    out_view->buffer = (char*)(str + offset);
    out_view->len    = len;

    return true;
}

LD_API StringView dstring_view_capacity_bounds(
    String* string, u32 offset
) {
    StringView result = {};
    if( offset >= string->capacity ) {
        LOG_FATAL("Attempted to create dstring view with invalid offset!");
        LD_PANIC();
    }
    result.buffer = string->buffer + offset;
    result.len    = string->capacity;
    return result;
}
LD_API StringView dstring_view_len_bounds(
    String* string,
    u32 offset
) {
    StringView result = {};
    if( offset >= string->len ) {
        LOG_FATAL("Attempted to create dstring view with invalid offset!");
        LD_PANIC();
    }
    result.buffer = string->buffer + offset;
    result.len    = string->len - offset;
    return result;
}
LD_API b32 dstring_view_writeable(
    String* string,
    u32 offset,
    StringView* out_view
);
LD_API usize str_length( const char* string ) {
    usize result = 0;

    if( *string ) {
        do {
            result++;
        } while( *string++ );
    }

    return result;
}

LD_API void str_ascii_to_wide(
    u32         str_len,
    const char* str_buffer,
    u32         max_dst_len,
    wchar_t*    dst_buffer
) {
    u32 max = str_len > max_dst_len ? max_dst_len : str_len;

    for( u32 i = 0; i < max; ++i ) {
        dst_buffer[i] = (wchar_t)str_buffer[i];
    }
    dst_buffer[max - 1] = 0;
}
LD_API void str_buffer_fill(
    u32 buffer_size,
    char* buffer,
    char character
) {
    for( u32 i = 0; i < buffer_size; ++i ) {
        buffer[i] = character;
    }
    buffer[buffer_size - 1] = 0;
}

isize format_bytes(
    usize bytes,
    char* buffer,
    usize buffer_size
) {
    #define IS_BYTES 0
    #define IS_KB    1
    #define IS_MB    2
    #define IS_GB    3

    f32 bytes_f32 = (f32)bytes;
    int type = IS_BYTES;
    if( bytes_f32 >= 1024.0f ) {
        bytes_f32 = BYTES_TO_KB( bytes_f32 );
        type = IS_KB;
        if( bytes_f32 >= 1024.0f ) {
            bytes_f32 = KB_TO_MB( bytes_f32 );
            type = IS_MB;
            if( bytes_f32 >= 1024.0f ) {
                bytes_f32 = MB_TO_GB( bytes_f32 );
                type = IS_GB;
            }
        }
    }

    char format[] = "%10.3f XB";
    char sizes[] = { ' ', 'K', 'M', 'G' };
    format[STATIC_ARRAY_COUNT(format) - 3] = sizes[type];

    return snprintf(
        buffer,
        buffer_size,
        format,
        bytes_f32
    );
}

// LD_API void int_to_str( u32 max_buffer, char* buffer, i64 integer ) {
//     b8  is_negative = integer & I64::SIGN_MASK;
//     u64 uint = integer & ~I64::SIGN_MASK;
//
//     u32 index = 0;
//     if( is_negative ) {
//         buffer[index++] = '-';
//     }
//     for( ;index < max_buffer; ++index ) {
//
//     }
// }
// LD_API void uint_to_str( u32 max_buffer, char* buffer, u64 integer ) {
//
// }
//
// LD_API void string_format(
//     String* dst,
//     b32 alloc,
//     const char* format,
//     ...
// ) {
//     #define MAX_FORMAT_BUFFER_SIZE 32
//     char format_buffer[MAX_FORMAT_BUFFER_SIZE] = {};
//
//     u32 format_len = str_length( format );
//     u32 max_len = format_len > dst->len ? dst->len : format_len;
//     u32 realloc = alloc ? 10 : 0;
//
//     va_list variadic;
//     va_start( variadic, format );
//     for( u32 i = 0; i < max_len; ++i ) {
//         if( format[i] == '{' ) {
//             u32 next = i + 1;
//             if( next >= max_len ) {
//                 LOG_FATAL("Malformed string format argument!");
//                 LD_PANIC();
//             }
//             switch( format[next] ) {
//                 /// String or string view
//                 case 'S': {
//                     next = next + 1;
//                     if( next >= format_len ) {
//                         LOG_FATAL("Malformed string format argument!");
//                         LD_PANIC();
//                     }
//                     switch( format[next] ) {
//                         case 'V':
//                         case 'v': {
//                             StringView* arg_string_view = va_arg( variadic, StringView* );
//                             LD_ASSERT(dstring_append( dst, *arg_string_view, alloc ));
//                         } break;
//                         case '}': {
//                             String* arg_string = va_arg( variadic, String* );
//                             LD_ASSERT(dstring_append(dst, *arg_string, alloc));
//                         } break;
//                     }
//                 } break;
//                 /// const char*
//                 case 's': {
//                     const char* arg_str = va_arg( variadic, const char* );
//                     StringView arg_view = arg_str;
//                     LD_ASSERT(dstring_append( dst, arg_view, alloc ));
//                 } break;
//                 /// character
//                 case 'C':
//                 case 'c': {
//                     char arg = va_arg( variadic, int );
//                     LD_ASSERT(string_push_char( dst, arg, realloc ));
//                 } break;
//                 default: LD_PANIC();
//             }
//             next = next + 1;
//             if( next >= format_len || format[next] != '}' ) {
//                 LOG_FATAL("Malformed string format argument!");
//                 LD_PANIC();
//             }
//             i = next + 1;
//         } else {
//             LD_ASSERT(string_push_char( dst, format[i], realloc ));
//         }
//     }
//     va_end( variadic );
// }
