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
#include "core/path.h"
#include "core/fs.h"
#include "core/memory.h"
#include "core/rand.h"

void job_package_resource( usize thread_index, void* user_params ) {
    ResourcePackageParams params = *(ResourcePackageParams*)user_params;
    read_write_fence();

    *params.ready_signal = *params.ready_signal + 1;
    void* buffer = (u8*)params.buffer + (params.buffer_size * thread_index);
    ManifestItem* item = params.manifest->items + params.index;

    FileHandle* file_output = fs_file_open(
        params.tmp_path, FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_SHARE_ACCESS_WRITE );
    if( !file_output ) {
        lp_error( "failed to open temp resource file '{s}'!", params.tmp_path );
        return;
    }

    char new_path_buffer[U8_MAX]  = {};

    StringBuffer new_path = {};
    new_path.buffer   = new_path_buffer;
    new_path.capacity = U8_MAX;

    string_buffer_fmt(
        &new_path, "{s}/{s}{0}", params.manifest->directory, item->path );

    item->path = reinterpret_cast( PathSlice, &new_path );
    FileHandle* file_input = fs_file_open(
        item->path, FILE_OPEN_FLAG_READ | FILE_OPEN_FLAG_SHARE_ACCESS_READ );
    if( !file_input ) {
        lp_error( "failed to open resource file '{s}'!", item->path );
        fs_file_close( file_output );
        return;
    }

    char tmp_path_buffer[U8_MAX] = {};
    StringBuffer tmp_path_string_buffer = {};
    tmp_path_string_buffer.buffer   = tmp_path_buffer;
    tmp_path_string_buffer.capacity = U8_MAX;

    string_buffer_fmt( &tmp_path_string_buffer, "resource_{u32}.tmp", rand_xor_u32() );

    PathSlice tmp_path = reinterpret_cast( PathSlice, &tmp_path_string_buffer );

    FileHandle* file_intermediate =
        fs_file_open( tmp_path, FILE_OPEN_FLAG_WRITE );
    if( !file_intermediate ) {
        lp_error( "failed to open intermediate file '{s}'!", tmp_path );
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

        unused(dst_offset);
    }

    fs_file_close( file_input );
    fs_file_close( file_output );
    fs_file_close( file_intermediate );
    memory_zero( buffer, params.buffer_size );
}

global volatile u32 global_buffer_offset = 0;
usize output_file_allocate( usize size ) {
    usize result = interlocked_add( &global_buffer_offset, size );
    return result;
}

