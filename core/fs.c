/**
 * Description:  File system operations implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 22, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"
#include "core/fmt.h"
#include "core/internal/platform.h"

CORE_API FileHandle* fs_file_open( PathSlice path, FileOpenFlags flags ) {
    return platform_file_open( path, flags );
}
CORE_API void fs_file_close( FileHandle* file ) {
    platform_file_close( file );
}
CORE_API usize fs_file_query_size( FileHandle* file ) {
    return platform_file_query_size( file );
}
CORE_API usize fs_file_query_offset( FileHandle* file ) {
    return platform_file_query_offset( file );
}
CORE_API void fs_file_set_offset(
    FileHandle* file, usize offset, b32 is_relative
) {
    platform_file_set_offset( file, offset, is_relative );
}
CORE_API void fs_file_truncate( FileHandle* file ) {
    platform_file_truncate( file );
}
CORE_API b32 fs_file_read( FileHandle* file, usize buffer_size, void* buffer ) {
    return platform_file_read( file, buffer_size, buffer );
}
CORE_API b32 fs_file_write( FileHandle* file, usize buffer_size, void* buffer ) {
    return platform_file_write( file, buffer_size, buffer );
}
CORE_API b32 fs_delete_file( PathSlice path ) {
    return platform_delete_file( path );
}
CORE_API b32 fs_file_to_file_copy(
    FileHandle* dst, FileHandle* src,
    usize intermediate_size, void* intermediate_buffer, usize size
) {
    if( !size ) {
        return true;
    }

    usize remaining = size;
    while( remaining ) {
        usize max_copy = intermediate_size;
        if( max_copy > remaining ) {
            max_copy = remaining;
        }
        
        if( !fs_file_read( src, max_copy, intermediate_buffer ) ) {
            return false;
        }

        if( !fs_file_write( dst, max_copy, intermediate_buffer ) ) {
            return false;
        }

        remaining -= max_copy;
    }

    return true;
}
CORE_API b32 fs_copy_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    return platform_file_copy_by_path( dst, src, fail_if_dst_exists );
}
CORE_API b32 fs_move_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    return platform_file_move_by_path( dst, src, fail_if_dst_exists );
}
CORE_API b32 fs_directory_create( PathSlice path ) {
    return platform_make_directory( path );
}
CORE_API b32 fs_directory_delete( PathSlice path, b32 recursive ) {
    return platform_delete_directory( path, recursive );
}
CORE_API b32 fs_directory_exists( PathSlice path )  {
    return platform_directory_exists( path );
}
CORE_API usize fs_get_working_directory( PathBuffer* buffer ) {
    if( !buffer || !buffer->v ) {
        return platform_get_working_directory( 0, 0, 0 );
    }

    usize bytes_written = 0;
    usize result = platform_get_working_directory(
        buffer->cap, buffer->v, &bytes_written );

    buffer->len = bytes_written;
    return result;
}

struct FileWriteParams {
    FileHandle* handle;
    b32         success;
};

usize ___file_write( void* target, usize count, char* characters ) {
    struct FileWriteParams* params = target;

    if( params->success ) {
        if( !fs_file_write( params->handle, count, characters ) ) {
            params->success = false;
        }
    }

    return 0;
}

CORE_API b32 fs_file_write_fmt_cstr_va(
    FileHandle* file, usize format_len, const char* format, va_list va
) {
    struct FileWriteParams params = {};
    params.handle  = file;
    params.success = true;

    (void)fmt_write_va(
        ___file_write, &params, format_len, format, va );

    return params.success;
}

