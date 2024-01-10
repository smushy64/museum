#if !defined(LD_CORE_FS_H)
#define LD_CORE_FS_H
/**
 * Description:  File system operations
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 22, 2023
*/
#include "shared/defines.h"
#include "core/path.h"

#if !defined(FORMAT_WRITE_FN_DEFINED)
#define FORMAT_WRITE_FN_DEFINED
/// Formatting write function.
/// Target is a pointer to where-ever the formatted string should be written to.
/// Returns 0 if successful, otherwise returns number of characters
/// that could not be written.
typedef usize FormatWriteFN( void* target, usize count, char* characters );
#endif

/// Opaque handle to a file.
typedef void FileHandle;

/// Flags for opening a file.
typedef u32 FileOpenFlags;
#define FILE_OPEN_FLAG_READ               (1 << 0)
#define FILE_OPEN_FLAG_WRITE              (1 << 1)
#define FILE_OPEN_FLAG_SHARE_ACCESS_READ  (1 << 2)
#define FILE_OPEN_FLAG_SHARE_ACCESS_WRITE (1 << 3)
#define FILE_OPEN_FLAG_CREATE             (1 << 4)
#define FILE_OPEN_FLAG_TRUNCATE           (1 << 5)

/// Open a file at a given path.
/// Flags determine if file is to be opened for reading/writing,
/// access rights for other threads/processes and if file should be
/// created or truncated on open.
/// By default, write only will create and
/// open the file for writing from the first byte.
/// Read only requires an existing file.
/// Read and write will create and open the file for writing from the first byte.
CORE_API FileHandle* fs_file_open( PathSlice path, FileOpenFlags flags );
/// Close a file handle.
CORE_API void fs_file_close( FileHandle* file );
/// Query the size of a file.
CORE_API usize fs_file_query_size( FileHandle* file );
/// Query where the file offset is inside the file.
CORE_API usize fs_file_query_offset( FileHandle* file );
/// Set the file's offset.
CORE_API void fs_file_set_offset( FileHandle* file, usize offset );
/// Read from a file from the current offset.
/// Modifies the file's offset to be at the end of the read.
CORE_API b32 fs_file_read( FileHandle* file, usize buffer_size, void* buffer );
/// Read from a file from a specified offset.
/// Does not modify the file's offset after reading.
CORE_API b32 fs_file_read_offset(
    FileHandle* file, usize offset, usize buffer_size, void* buffer );
/// Write to a file from the current offset.
/// Modifies the file's offset to be at the end of the write.
CORE_API b32 fs_file_write( FileHandle* file, usize buffer_size, void* buffer );
/// Write to a file from a specified offset.
/// Does not modify the file's offset after reading.
CORE_API b32 fs_file_write_offset(
    FileHandle* file, usize offset, usize buffer_size, void* buffer );
/// Copy contents from src to dst.
/// Source file must have range of offset + size available to read and
/// must be opened with read flag.
/// Destination file must be opened with write flag.
/// intermediate_buffer must be able to hold intermediate_size.
CORE_API b32 fs_file_to_file_copy(
    FileHandle* dst, usize dst_offset,
    FileHandle* src, usize src_offset,
    usize intermediate_size, void* intermediate_buffer, usize size );
/// Delete a file pointed to by path.
CORE_API b32 fs_delete_file( PathSlice path );
/// Copy the file at source path to destination path.
CORE_API b32 fs_copy_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists );
/// Move the file at source path to destination path.
CORE_API b32 fs_move_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists );
/// Check if file at specified path exists.
/// Returns true if it does, returns false if it doesn't or if
/// path does not point to a file.
header_only b32 fs_check_if_file_exists( PathSlice path ) {
    FileHandle* file = fs_file_open(
        path, FILE_OPEN_FLAG_READ |
        FILE_OPEN_FLAG_SHARE_ACCESS_READ |
        FILE_OPEN_FLAG_SHARE_ACCESS_WRITE );
    b32 exists = file != NULL;
    fs_file_close( file );

    return exists;
}
/// Get the current working directory.
/// Returns number of bytes required to write out working directory.
/// If either path buffer or the path buffer's buffer is null,
/// returns required size of buffer.
CORE_API usize fs_get_working_directory( PathBuffer* buffer );

/// Write a formatted string directly to a file using variadic arguments.
/// Writes at the file's current offset and modifies offset.
CORE_API b32 ___internal_file_write_fmt_va(
    FileHandle* file, usize format_len, const char* format, va_list va );
/// Write a formatted string directly to a file.
/// Writes at the file's current offset and modifies offset.
header_only b32 ___internal_file_write_fmt(
    FileHandle* file, usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );
    b32 result = ___internal_file_write_fmt_va( file, format_len, format, va );
    va_end( va );
    return result;
}
/// Write a formatted string directly to a file using variadic arguments.
/// Writes at the specified offset and does not modify offset.
header_only b32 ___internal_file_write_offset_fmt_va(
    FileHandle* file, usize offset, usize format_len, const char* format, va_list va
) {
    usize former_offset = fs_file_query_offset( file );
    fs_file_set_offset( file, offset );
    b32 result = ___internal_file_write_fmt_va( file, format_len, format, va );
    fs_file_set_offset( file, former_offset );
    return result;
}
/// Write a formatted string directly to a file.
/// Writes at the specified offset and does not modify offset.
header_only b32 ___internal_file_write_offset_fmt(
    FileHandle* file, usize offset, usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );
    b32 result = ___internal_file_write_offset_fmt_va(
        file, offset, format_len, format, va );
    va_end( va );
    return result;
}

#define fs_file_write_fmt( file, format, ... )\
    ___internal_file_write_fmt( file, sizeof(format) - 1, format, ##__VA_ARGS__ )
#define fs_file_write_fmt_va( file, format, va )\
    ___internal_file_write_fmt_va( file, sizeof(format) - 1, format, va )
#define fs_file_write_offset_fmt( file, offset, format, ... )\
    ___internal_file_write_fmt_offset(\
        file, offset, sizeof(format) - 1, format, ##__VA_ARGS__ )
#define fs_file_write_offset_fmt_va( file, offset, format, va )\
    ___internal_file_write_offset_fmt_va(\
        file, offset, sizeof(format) - 1, format, va )

#endif /* header guard */
