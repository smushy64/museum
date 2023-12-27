#if !defined(LP_FS_EX_H)
#define LP_FS_EX_H
/**
 * Description:  File system extensions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 20, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"

/// Generate a temporary file name.
/// Buffer must be able to hold 255 characters!
void fs_ex_generate_tmp_file_name(
    const char* opt_prefix, const char* opt_suffix, char* buffer );
/// Copy bytes from source file to destination file.
/// If no offsets are provided, uses current file offset.
/// If offsets are provided, file's offset will not change after calling this function.
/// Returns true if successful.
b32 fs_ex_file_copy_to_file(
    usize intermediate_buffer_size, void* intermediate_buffer,
    usize* opt_dst_offset, FSFile* dst,
    usize* opt_src_offset, FSFile* src,
    usize size );

#endif /* header guard */
