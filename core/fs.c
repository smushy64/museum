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
CORE_API void fs_file_set_offset( FileHandle* file, usize offset ) {
    platform_file_set_offset( file, offset );
}
CORE_API b32 fs_file_read( FileHandle* file, usize buffer_size, void* buffer ) {
    return platform_file_read( file, buffer_size, buffer );
}
CORE_API b32 fs_file_read_offset(
    FileHandle* file, usize offset, usize buffer_size, void* buffer
) {
    usize former_offset = fs_file_query_offset( file );
    fs_file_set_offset( file, offset );
    b32 result = platform_file_read( file, buffer_size, buffer );
    fs_file_set_offset( file, former_offset );
    return result;
}
CORE_API b32 fs_file_write( FileHandle* file, usize buffer_size, void* buffer ) {
    return platform_file_write( file, buffer_size, buffer );
}
CORE_API b32 fs_file_write_offset(
    FileHandle* file, usize offset, usize buffer_size, void* buffer
) {
    usize former_offset = fs_file_query_offset( file );
    fs_file_set_offset( file, offset );
    b32 result = platform_file_write( file, buffer_size, buffer );
    fs_file_set_offset( file, former_offset );
    return result;
}
CORE_API b32 fs_delete_file( PathSlice path ) {
    return platform_delete_file( path );
}
CORE_API b32 fs_file_to_file_copy(
    FileHandle* dst, usize dst_offset,
    FileHandle* src, usize src_offset,
    usize intermediate_size, void* intermediate_buffer, usize size
) {
    usize former_dst_offset = fs_file_query_offset( dst );
    usize former_src_offset = fs_file_query_offset( src );

    fs_file_set_offset( dst, dst_offset );
    fs_file_set_offset( src, src_offset );

    b32 result = true;
    usize remaining = size;
    while( remaining ) {
        usize max_copy = intermediate_size;
        if( max_copy > remaining ) {
            max_copy = remaining;
        }
        
        if( !fs_file_read( src, max_copy, intermediate_buffer ) ) {
            result = false;
            goto fs_file_to_file_copy_end;
        }

        if( !fs_file_write( dst, max_copy, intermediate_buffer ) ) {
            result = false;
            goto fs_file_to_file_copy_end;
        }

        remaining -= max_copy;
    }

fs_file_to_file_copy_end:
    fs_file_set_offset( dst, former_dst_offset );
    fs_file_set_offset( src, former_src_offset );
    return result;
}
CORE_API b32 fs_copy_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    return platform_file_copy_by_path( dst, src, fail_if_dst_exists );
}
CORE_API b32 fs_move_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    return platform_file_move_by_path( dst, src, fail_if_dst_exists );
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

CORE_API b32 ___internal_file_write_fmt_va(
    FileHandle* file, usize format_len, const char* format, va_list va
) {
    struct FileWriteParams params = {};
    params.handle  = file;
    params.success = true;

    (void)___internal_fmt_write_va(
        ___file_write, &params, format_len, format, va );

    return params.success;
}

