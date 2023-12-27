/**
 * Description:  File system operations implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 22, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"
#include "core/fmt.h"
#include "core/internal/platform.h"

CORE_API FSFile* fs_file_open( const char* path, FSFileFlags flags ) {
    return platform_file_open( path, flags );
}
CORE_API void fs_file_close( FSFile* file ) {
    platform_file_close( file );
}
CORE_API b32 fs_file_read( FSFile* file, usize buffer_size, void* buffer ) {
    return platform_file_read( file, buffer_size, buffer );
}
CORE_API b32 fs_file_read_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset
) {
    usize previous_offset = platform_file_query_offset( file );
    platform_file_set_offset( file, offset );
    b32 read_result = platform_file_read( file, buffer_size, buffer );
    platform_file_set_offset( file, previous_offset );

    return read_result;
}
CORE_API b32 fs_file_write( FSFile* file, usize buffer_size, void* buffer ) {
    return platform_file_write( file, buffer_size, buffer );
}
CORE_API b32 fs_file_write_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset
) {
    return platform_file_write_offset( file, offset, buffer_size, buffer );
}
CORE_API usize fs_file_query_size( FSFile* file ) {
    return platform_file_query_size( file );
}
CORE_API usize fs_file_query_offset( FSFile* file ) {
    return platform_file_query_offset( file );
}
CORE_API void fs_file_set_offset( FSFile* file, usize offset ) {
    platform_file_set_offset( file, offset );
}
CORE_API void fs_file_set_offset_relative( FSFile* file, usize offset ) {
    usize current_offset = fs_file_query_offset( file );
    fs_file_set_offset( file, current_offset + offset );
}
CORE_API b32 fs_file_delete( const char* path ) {
    return platform_file_delete( path );
}
CORE_API b32 fs_file_copy(
    const char* dst, const char* src, b32 fail_if_exists
) {
    return platform_file_copy( dst, src, fail_if_exists );
}
CORE_API b32 fs_file_move(
    const char* dst_path, const char* src_path, b32 fail_if_exists
) {
    return platform_file_move( dst_path, src_path, fail_if_exists );
}
CORE_API b32 fs_file_exists( const char* path ) {
    return platform_file_check_if_exists( path );
}

struct FSWriteParams {
    FSFile* file;
    b32     use_offset;
    usize   offset;
};
usize ___internal_fs_write( void* target, usize count, char* characters ) {
    struct FSWriteParams* params = target;

    if( params->use_offset ) {
        return platform_file_write_offset(
            params->file, params->offset, count, characters );
    } else {
        return platform_file_write( params->file, count, characters );
    }
}

CORE_API b32 ___internal_fs_file_write_fmt(
    FSFile* file, usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );

    struct FSWriteParams params = {};
    params.file       = file;
    params.use_offset = false;
    b32 result = ___internal_fmt_write_va(
        ___internal_fs_write, &params, format_len, format, va ) != 0;

    va_end( va );

    return result;
}
CORE_API b32 ___internal_fs_file_write_fmt_va(
    FSFile* file, usize format_len, const char* format, va_list va
) {
    struct FSWriteParams params = {};
    params.file       = file;
    params.use_offset = false;
    return ___internal_fmt_write_va(
        ___internal_fs_write, &params, format_len, format, va ) != 0;
}
CORE_API b32 ___internal_fs_file_write_offset_fmt(
    FSFile* file, usize offset, usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );

    struct FSWriteParams params = {};
    params.file       = file;
    params.use_offset = true;
    params.offset     = offset;
    b32 result = ___internal_fmt_write_va(
        ___internal_fs_write, &params, format_len, format, va ) != 0;

    va_end( va );

    return result;
}
CORE_API b32 ___internal_fs_file_write_offset_fmt_va(
    FSFile* file, usize offset, usize format_len, const char* format, va_list va
) {
    struct FSWriteParams params = {};
    params.file       = file;
    params.use_offset = true;
    params.offset     = offset;
    return ___internal_fmt_write_va(
        ___internal_fs_write, &params, format_len, format, va ) != 0;
}

