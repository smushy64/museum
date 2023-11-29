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
LD_API FSFile* fs_file_open( const char* path, FSFileFlags flags );
/// Close a file handle.
LD_API void fs_file_close( FSFile* file );
/// Read a file at the current offset.
LD_API b32 fs_file_read( FSFile* file, usize buffer_size, void* buffer );
/// Read a file at a given offset.
/// Does not modify file offset.
LD_API b32 fs_file_read_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset );
/// Write in file at current offset.
LD_API b32 fs_file_write( FSFile* file, usize buffer_size, void* buffer );
/// Write in file at given offset.
/// Does not modify file offset.
LD_API b32 fs_file_write_offset(
    FSFile* file, usize buffer_size, void* buffer, usize offset );
/// Write formatted string to file.
LD_API b32 fs_file_write_fmt( FSFile* file, const char* format, ... );
/// Write formatted string to file using variadic arguments.
LD_API b32 fs_file_write_fmt_va( FSFile* file, const char* format, va_list va );
/// Write formatted string to file at current offset.
LD_API b32 fs_file_write_offset_fmt( FSFile* file, usize offset, const char* format, ... );
/// Write formatted string to file at current offset using variadic arguments.
LD_API b32 fs_file_write_offset_fmt_va(
    FSFile* file, usize offset, const char* format, va_list va );
/// Query the size of the given file.
LD_API usize fs_file_query_size( FSFile* file );
/// Query the current file offset.
LD_API usize fs_file_query_offset( FSFile* file );
/// Set file offset from start of file.
LD_API void fs_file_set_offset( FSFile* file, usize offset );
/// Set file offset relative to the current offset.
LD_API void fs_file_set_offset_relative( FSFile* file, usize offset );
/// Delete file by handle.
LD_API b32 fs_file_delete( FSFile* file );
/// Delete file by path.
LD_API b32 fs_file_delete_by_path( const char* path );
/// Copy source to destination file by handle.
LD_API b32 fs_file_copy( FSFile* dst, FSFile* src, b32 fail_if_exists );
/// Copy source to destination file by path.
LD_API b32 fs_file_copy_by_path( const char* dst, const char* src, b32 fail_if_exists );

#endif /* header guard */
