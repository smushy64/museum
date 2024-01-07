/**
 * Description:  Path function implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 06, 2024
*/
#include "shared/defines.h"
#include "core/path.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/internal/platform.h"

internal b32 ___char_is_separator( char character ) {
#if defined(LD_PLATFORM_WINDOWS)
    return character == '/' || character == '\\';
#else
    return character == '/';
#endif
}

CORE_API PathSlice path_slice_from_cstr( usize opt_len, const char* cstr ) {
    StringSlice slice = string_slice_from_cstr( opt_len, cstr );
    return reinterpret_cast( PathSlice, &slice );
}
CORE_API b32 path_slice_is_absolute( PathSlice path ) {
#if defined(LD_PLATFORM_WINDOWS)
    if( path.len > 3 ) {
        char* buf = path.buffer;
        return
            buf[0] != '.' &&
            buf[1] == ':' &&
            ___char_is_separator( buf[2] );
    }
    return false;
#else
    return path.buffer[0] == '/';
#endif
}
CORE_API b32 path_slice_is_file( PathSlice path ) {
    return platform_path_is_file( path );
}
CORE_API b32 path_slice_is_directory( PathSlice path ) {
    return platform_path_is_directory( path );
}
CORE_API b32 path_slice_get_parent( PathSlice path, PathSlice* out_parent ) {
#if defined(LD_PLATFORM_WINDOWS)
    if(
        path.len == 3 &&
        path.buffer[0] != '.' &&
        path.buffer[1] == ':' &&
        ___char_is_separator( path.buffer[2] )
    ) {
        return false;
    }
#endif

    PathSlice sub_slice = {};

    for( usize i = path.len; i-- > 0; ) {
        if( ___char_is_separator( path.buffer[i] ) ) {
            sub_slice.buffer = path.buffer;
            sub_slice.len    = i;
            break;
        }
    }

    if( !sub_slice.buffer ) {
        return false;
    }

#if defined(LD_PLATFORM_WINDOWS)
    if( sub_slice.len == 2 && sub_slice.buffer[1] == ':' ) {
        sub_slice.len++;
    }
#endif

    *out_parent = sub_slice;
    return true;
}
CORE_API usize path_slice_ancestor_count( PathSlice path ) {
    usize count = 0;

    PathSlice sub_slice = path;
    while( path_slice_get_parent( sub_slice, &sub_slice ) ) {
        count++;
    }

    return count;
}
CORE_API void path_slice_fill_ancestors(
    PathSlice path, usize ancestor_count, PathSlice* ancestors
) {
    PathSlice sub_slice = path;
    for( usize i = 0; i < ancestor_count; ++i ) {
        assert( path_slice_get_parent( sub_slice, &sub_slice ) );
        ancestors[i] = sub_slice;
    }
}
CORE_API b32 path_slice_get_file_name( PathSlice path, PathSlice* out_file_name ) {
    if( ___char_is_separator( path.buffer[path.len - 1] ) ) {
        return false;
    }

    PathSlice file_name = {};
    for( usize i = path.len; i-- > 0; ) {
        if( ___char_is_separator( path.buffer[i] ) ) {
            usize index = i + 1;
            file_name.buffer = path.buffer + index;
            file_name.len    = path.len - index;
            break;
        }
    }
    if( !file_name.buffer ) {
        file_name = path;
    }

    *out_file_name = file_name;
    return true;
}
CORE_API b32 path_slice_get_file_stem( PathSlice path, PathSlice* out_file_stem ) {
    PathSlice file_name = {};
    if( !path_slice_get_file_name( path, &file_name ) ) {
        return false;
    }

    PathSlice ext = {};
    if( path_slice_get_extension( file_name, &ext ) ) {
        if( file_name.len < ext.len + 1 ) {
            return false;
        }
        file_name.len -= ext.len + 1;
    }

    *out_file_stem = file_name;
    return true;
}
CORE_API b32 path_slice_get_extension( PathSlice path, PathSlice* out_extension ) {
    PathSlice ext = {};
    for( usize i = path.len; i-- > 0; ) {
        if( ___char_is_separator( path.buffer[i] ) ) {
            break;
        }
        if( path.buffer[i] == '.' ) {
            usize index = i + 1;
            if( index > path.len ) {
                break;
            }

            ext.buffer = path.buffer + index;
            ext.len    = path.len - index;
            break;
        }
    }

    if( !ext.buffer ) {
        return false;
    }

    *out_extension = ext;
    return true;
}
CORE_API b32 path_slice_pop(
    PathSlice path, PathSlice* out_path, PathSlice* opt_out_chunk
) {
    if( !path.len ) {
        return false;
    }

    PathSlice parent = {};
    if( !path_slice_get_parent( path, &parent ) ) {
        return false;
    }

    *out_path = parent;

    if( opt_out_chunk ) {
        opt_out_chunk->buffer = path.buffer + parent.len;
        opt_out_chunk->len    = path.len - parent.len;

        if(
            opt_out_chunk->len > 0 &&
            ___char_is_separator( opt_out_chunk->buffer[0] )
        ) {
            opt_out_chunk->buffer++;
            opt_out_chunk->len--;
        }
    }

    return true;
}
CORE_API usize path_slice_canonicalize(
    FormatWriteFN* write, void* target, PathSlice path );
CORE_API usize path_slice_convert_separators(
    FormatWriteFN* write, void* target, PathSlice path, b32 forward_slash
) {
    char separator_to_search = forward_slash ? '\\' : '/';
    char separator_to_write  = forward_slash ? '/' : '\\';

    usize result = 0;

    StringSlice slice = {};
    slice.buffer = path.buffer;
    slice.len    = path.len;
    while( slice.len ) {
        usize separator_index = 0;
        if( string_slice_find_char( slice, separator_to_search, &separator_index ) ) {
            result += write( target, separator_index, slice.buffer );
            result += write( target, 1, &separator_to_write );

            usize move_count = separator_index + 1;
            if( move_count > slice.len ) {
                break;
            }

            slice.buffer += move_count;
            slice.len    -= move_count;
        } else {
            if( slice.len ) {
                result += write( target, slice.len, slice.buffer );
            }
            break;
        }
    }

    return result;
}

CORE_API b32 path_buffer_push( PathBuffer* path, PathSlice chunk ) {
    if( path->capacity < path->len + chunk.len ) {
        return false;
    }

    if( chunk.len == 1 ) {
        path->buffer[path->len++] = chunk.buffer[0];
        return true;
    }

    usize append_len = chunk.len + 1;
    if( path->capacity < append_len + path->len ) {
        return false;
    }

    path->buffer[path->len++] = '/';
    memory_copy( path->buffer + path->len, chunk.buffer, chunk.len );
    path->len += chunk.len;

    return true;
}
CORE_API b32 path_buffer_pop( PathBuffer* path, PathSlice* opt_out_chunk ) {
    PathSlice path_slice = path_buffer_to_slice( path );

    PathSlice new_path = {};
    PathSlice chunk    = {};
    if( path_slice_pop( path_slice, &new_path, &chunk ) ) {
        path->len      = new_path.len;
        *opt_out_chunk = chunk;
        return true;
    }

    return false;
}
CORE_API b32 path_buffer_set_extension( PathBuffer* path, PathSlice extension ) {
    PathSlice slice = path_buffer_to_slice( path );
    PathSlice ext   = {};
    if( path_slice_get_extension( slice, &ext ) ) {
        slice.len -= ext.len;
    }

    usize final_len = slice.len + extension.len + ( ext.buffer ? 0 : 1 );
    if( final_len > path->capacity ) {
        return false;
    }
    if( !ext.buffer ) {
        slice.buffer[slice.len++] = '.';
    }
    path->len = slice.len;

    memory_copy( path->buffer + path->len, extension.buffer, extension.len );
    path->len += extension.len;

    return true;
}

