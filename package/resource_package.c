/**
 * Description:  Liquid Package resource package implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 12, 2023
*/
#include "shared/defines.h"
#include "package/resource_package.h"

#include "package/manifest.h"
#include "package/logging.h"
#include "package/resource_audio.h"

#include "shared/constants.h"
#include "core/sync.h"
#include "core/fs.h"
#include "core/memory.h"

void job_package_resource( usize thread_index, void* user_params ) {
    ResourcePackageParams params = *(ResourcePackageParams*)user_params;
    read_write_fence();

    *params.ready_signal = *params.ready_signal + 1;
    void* buffer = (u8*)params.buffer + (params.buffer_size * thread_index);
    ManifestItem* item = params.manifest->items + params.index;

    FSFile* file_output =
        fs_file_open( params.tmp_path, FS_FILE_WRITE | FS_FILE_SHARE_WRITE );
    if( !file_output ) {
        lp_error( "failed to open temp resource file '{cc}'!", params.tmp_path );
        return;
    }

    char new_path_buffer[U8_MAX]  = {};

    StringSlice new_path = {};
    new_path.buffer   = new_path_buffer;
    new_path.capacity = U8_MAX;

    string_slice_fmt(
        &new_path, "{s}/{cc}{0}", params.manifest->directory, item->path );

    item->path = new_path.buffer;
    FSFile* file_input =
        fs_file_open( item->path, FS_FILE_READ | FS_FILE_SHARE_READ );
    if( !file_input ) {
        lp_error( "failed to open resource file '{cc}'!", item->path );
        fs_file_close( file_output );
        return;
    }

    char temp_path_buffer[U8_MAX] = {};
    StringSlice temp_path_slice = {};
    temp_path_slice.buffer   = temp_path_buffer;
    temp_path_slice.capacity = U8_MAX;

    fs_file_generate_temp_path(
        string_slice_write, &temp_path_slice, "resource", NULL );

    FSFile* file_intermediate =
        fs_file_open( temp_path_buffer, FS_FILE_WRITE );
    if( !file_intermediate ) {
        lp_error( "failed to open intermediate file '{cc}'!", temp_path_buffer );
        fs_file_close( file_input );
        fs_file_close( file_output );
        return;
    }

    struct LiquidPackageResource resource = {};

    switch( item->type ) {
        case LIQUID_PACKAGE_RESOURCE_TYPE_TEXT: {

        } break;
        default:
            lp_error( "unrecognized item type!" );
            panic();
    }

    usize resource_offset =
        liquid_package_calculate_resource_file_offset( params.index );
    fs_file_set_offset( file_output, resource_offset );
    if( !fs_file_write( file_output, sizeof(resource), &resource ) ) {
        lp_error( "failed to write resource to output file!" );
    }

    /* copy intermediate file to output file */{
        usize intermediate_file_size = fs_file_query_size( file_intermediate );
        usize buffer_start_offset =
            sizeof( struct LiquidPackageHeader ) +
            (sizeof( struct LiquidPackageResource ) * params.manifest->item_count);
        usize dst_offset = output_file_allocate( intermediate_file_size );
        dst_offset += buffer_start_offset;

        fs_file_set_offset( file_intermediate, 0 );
        fs_file_set_offset( file_output, dst_offset );

        if( !fs_file_copy(
            file_output, file_intermediate,
            intermediate_file_size, params.buffer_size, buffer
        ) ) {
            lp_error( "failed to copy from intermediate file to output file!" );
        }
    }

    fs_file_close( file_input );
    fs_file_close( file_output );
    fs_file_close( file_intermediate );
    assert( fs_file_delete( temp_path_buffer ) );
    memory_zero( buffer, params.buffer_size );
}

global volatile u32 global_buffer_offset = 0;
usize output_file_allocate( usize size ) {
    usize result = interlocked_add( &global_buffer_offset, size );
    return result;
}

