/**
 * Description:  File system extensions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 20, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"
#include "fs_ex.h"

#include "shared/constants.h"
#include "core/string.h"
#include "core/rand.h"
#include "core/memory.h"

void fs_ex_generate_tmp_file_name(
    const char* opt_prefix, const char* opt_suffix, char* buffer
) {
    StringSlice slice = {};
    slice.buffer   = buffer;
    slice.capacity = U8_MAX;

    u32 random = rand_xor_u32();
    usize fmt_result = 0;

    if( opt_prefix && opt_suffix ) {
        fmt_result = string_slice_fmt(
            &slice, "{cc}_{u32,010}_{cc}.tmp{0}", opt_prefix, random, opt_suffix );
    } else if( opt_prefix ) {
        fmt_result =
            string_slice_fmt( &slice, "{cc}_{u32,010}.tmp{0}", opt_prefix, random );
    } else if( opt_suffix ) {
        fmt_result =
            string_slice_fmt( &slice, "{u32,010}_{cc}.tmp{0}", random, opt_suffix );
    } else {
        fmt_result = string_slice_fmt( &slice, "{u32,010}.tmp{0}", random );
    }

    if( fmt_result ) {
        const char* ext = ".tmp";
        memory_copy( (slice.buffer + slice.len) - sizeof(".tmp"), ext, sizeof(".tmp") );
    }
}
b32 fs_ex_file_copy_to_file(
    usize intermediate_buffer_size, void* intermediate_buffer,
    usize* opt_dst_offset, FSFile* dst,
    usize* opt_src_offset, FSFile* src,
    usize size
) {
    if( !intermediate_buffer_size ) {
        return false;
    }

    usize original_dst_offset = 0;
    usize original_src_offset = 0;
    if( opt_dst_offset ) {
        original_dst_offset = fs_file_query_offset( dst );
        fs_file_set_offset( dst, *opt_dst_offset );
    }
    if( opt_src_offset ) {
        original_src_offset = fs_file_query_offset( src );
        fs_file_set_offset( src, *opt_src_offset );
    }

    usize remaining = size;
    b32 success = true;

    while( remaining ) {
        usize max_read = intermediate_buffer_size;
        if( max_read > remaining ) {
            max_read = remaining;
        }

        if( !fs_file_read( src, max_read, intermediate_buffer ) ) {
            success = false;
            goto fs_ex_file_copy_to_file_end;
        }

        if( !fs_file_write( dst, max_read, intermediate_buffer ) ) {
            success = false;
            goto fs_ex_file_copy_to_file_end;
        }

        remaining -= max_read;
    }

fs_ex_file_copy_to_file_end:
    if( opt_dst_offset ) {
        fs_file_set_offset( dst, original_dst_offset );
    }
    if( opt_src_offset ) {
        fs_file_set_offset( src, original_src_offset );
    }

    return success;
}

