#if !defined(PLATFORM_IO_HPP)
#define PLATFORM_IO_HPP
/**
 * Description:  Platform Input-Output functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

struct FileHandle {
    void* platform;
};

typedef u32 FileOpenFlags;

#define PLATFORM_FILE_OPEN_READ        ( 1 << 0 )
#define PLATFORM_FILE_OPEN_WRITE       ( 1 << 1 )
#define PLATFORM_FILE_OPEN_SHARE_READ  ( 1 << 2 )
#define PLATFORM_FILE_OPEN_SHARE_WRITE ( 1 << 3 )
#define PLATFORM_FILE_OPEN_EXISTING    ( 1 << 4 )

/// Open file from path.
b32 platform_file_open(
    const char* path,
    FileOpenFlags flags,
    FileHandle* out_handle
);
/// Close file handle.
void platform_file_close( FileHandle handle );
/// Read file into buffer.
b32 platform_file_read(
    FileHandle handle,
    usize read_size,
    usize buffer_size,
    void* buffer
);
/// Query size of file in bytes.
usize platform_file_query_size( FileHandle handle );
/// Query where in the file the handle is at.
usize platform_file_query_offset( FileHandle handle );
/// Set file offset. Returns true if successful.
b32 platform_file_set_offset( FileHandle handle, usize offset );

#endif // header guard