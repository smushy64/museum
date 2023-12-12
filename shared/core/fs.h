#if !defined(LD_SHARED_CORE_FS_H)
#define LD_SHARED_CORE_FS_H
/**
 * Description:  File system operations
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 22, 2023
*/
#include "defines.h"

/// Opaque handle to a file.
typedef void FSFile;
/// Flags for how to open a file.
typedef u32 FSFileFlags;

/// Open file for reading.
#define FS_FILE_READ          (1 << 0)
/// Open file for writing.
#define FS_FILE_WRITE         (1 << 1)
/// Allow other processes and threads to read.
#define FS_FILE_SHARE_READ    (1 << 2) 
/// Allow other processes and threads to write.
#define FS_FILE_SHARE_WRITE   (1 << 3)
/// Only open file if it already exists.
#define FS_FILE_ONLY_EXISTING (1 << 4)

/// Open or create file if it doesn't exist.
/// Returns handle to file, otherwise NULL if failed.
CORE_API FSFile* fs_file_open( const char* path, FSFileFlags flags );
/// Close a file handle.
CORE_API void fs_file_close( FSFile* file );
/// Read a file at the current offset.
CORE_API b32 fs_file_read( FSFile* file, usize buffer_size, void* buffer );
/// Read a file at a given offset.
/// Does not modify file offset.
CORE_API b32 fs_file_read_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset );
/// Write in file at current offset.
CORE_API b32 fs_file_write( FSFile* file, usize buffer_size, void* buffer );
/// Write in file at given offset.
/// Does not modify file offset.
CORE_API b32 fs_file_write_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset );
/// Query the size of the given file.
CORE_API usize fs_file_query_size( FSFile* file );
/// Query the current file offset.
CORE_API usize fs_file_query_offset( FSFile* file );
/// Set file offset from start of file.
CORE_API void fs_file_set_offset( FSFile* file, usize offset );
/// Set file offset relative to the current offset.
CORE_API void fs_file_set_offset_relative( FSFile* file, usize offset );
/// Delete file by path.
CORE_API b32 fs_file_delete( const char* path );
/// Copy source to destination file by path.
CORE_API b32 fs_file_copy(
    const char* dst_path, const char* src_path, b32 fail_if_exists );
/// Move source to destination file by path.
CORE_API b32 fs_file_move(
    const char* dst_path, const char* src_path, b32 fail_if_exists );
/// Check if file exists.
CORE_API b32 fs_file_exists( const char* path );

/// Write formatted string to file.
CORE_API b32 ___internal_fs_file_write_fmt(
    FSFile* file, usize format_len, const char* format, ... );
/// Write formatted string to file using variadic list.
CORE_API b32 ___internal_fs_file_write_fmt_va(
    FSFile* file, usize format_len, const char* format, va_list va );
/// Write formatted string to file at offset.
CORE_API b32 ___internal_fs_file_write_offset_fmt(
    FSFile* file, usize offset, usize format_len, const char* format, ... );
/// Write formatted string to file at offset using variadic list.
CORE_API b32 ___internal_fs_file_write_offset_fmt_va(
    FSFile* file, usize offset, usize format_len, const char* format, va_list va );

#define fs_file_write_fmt( file, format, ... )\
    ___internal_fs_file_write_fmt( file, sizeof(format) - 1, format, ##__VA_ARGS__ )
#define fs_file_write_fmt_va( file, format, va )\
    ___internal_fs_file_write_fmt_va( file, sizeof(format) - 1, format, va )
#define fs_file_write_offset_fmt( file, offset, format, ... )\
    ___internal_fs_file_write_offset_fmt( file, offset, sizeof(format) - 1, format, ##__VA_ARGS__ )
#define fs_file_write_offset_fmt_va( file, offset, format, va )\
    ___internal_fs_file_write_offset_fmt_va( file, offset, sizeof( format ) - 1, format, va )

#endif /* header guard */