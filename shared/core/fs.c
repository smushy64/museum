/**
 * Description:  File system operations implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 22, 2023
*/
#include "defines.h"
#include "core/fs.h"
#include "core/fmt.h"
#include "core/internal.h"

LD_API FSFile* fs_file_open( const char* path, FSFileFlags flags ) {
    return platform->io.file_open( path, flags );
}
LD_API void fs_file_close( FSFile* file ) {
    platform->io.file_close( file );
}
LD_API b32 fs_file_read( FSFile* file, usize buffer_size, void* buffer ) {
    return platform->io.file_read( file, buffer_size, buffer );
}
LD_API b32 fs_file_read_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset
) {
    usize previous_offset = platform->io.file_query_offset( file );
    platform->io.file_set_offset( file, offset );
    b32 read_result = platform->io.file_read( file, buffer_size, buffer );
    platform->io.file_set_offset( file, previous_offset );

    return read_result;
}
LD_API b32 fs_file_write( FSFile* file, usize buffer_size, void* buffer ) {
    return platform->io.file_write( file, buffer_size, buffer );
}
LD_API b32 fs_file_write_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset
) {
    return platform->io.file_write_offset( file, buffer_size, buffer, offset );
}
LD_API usize fs_file_query_size( FSFile* file ) {
    return platform->io.file_query_size( file );
}
LD_API usize fs_file_query_offset( FSFile* file ) {
    return platform->io.file_query_offset( file );
}
LD_API void fs_file_set_offset( FSFile* file, usize offset ) {
    platform->io.file_set_offset( file, offset );
}
LD_API void fs_file_set_offset_relative( FSFile* file, usize offset ) {
    usize current_offset = fs_file_query_offset( file );
    fs_file_set_offset( file, current_offset + offset );
}
LD_API b32 fs_file_delete( const char* path ) {
    return platform->io.file_delete_by_path( path );
}
LD_API b32 fs_file_copy(
    const char* dst, const char* src, b32 fail_if_exists
) {
    return platform->io.file_copy_by_path( dst, src, fail_if_exists );
}

struct FSWriteParams {
    FSFile* file;
    b32     use_offset;
    usize   offset;
};
usize ___internal_fs_write( void* target, usize count, char* characters ) {
    struct FSWriteParams* params = target;

    if( params->use_offset ) {
        return platform->io.file_write_offset(
            params->file, count, characters, params->offset );
    } else {
        return platform->io.file_write( params->file, count, characters );
    }
}

LD_API b32 ___internal_fs_file_write_fmt(
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
LD_API b32 ___internal_fs_file_write_fmt_va(
    FSFile* file, usize format_len, const char* format, va_list va
) {
    struct FSWriteParams params = {};
    params.file       = file;
    params.use_offset = false;
    return ___internal_fmt_write_va(
        ___internal_fs_write, &params, format_len, format, va ) != 0;
}
LD_API b32 ___internal_fs_file_write_offset_fmt(
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
LD_API b32 ___internal_fs_file_write_offset_fmt_va(
    FSFile* file, usize offset, usize format_len, const char* format, va_list va
) {
    struct FSWriteParams params = {};
    params.file       = file;
    params.use_offset = true;
    params.offset     = offset;
    return ___internal_fmt_write_va(
        ___internal_fs_write, &params, format_len, format, va ) != 0;
}

