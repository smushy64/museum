/**
 * Description:  Process Resource Implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 09, 2024
*/
#include "shared/defines.h"
#include "core/path.h"

#include "package/manifest.h"
#include "package/resource.h"
#include "package/logging.h"

#include "core/sync.h"
#include "core/memory.h"
#include "core/fs.h"

#include "core/rand.h"

global volatile usize global_file_offset = 0;
usize allocate_output_file_space( usize size );

b32 process_resource_invalid(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* file, FileHandle* file2, usize buffer_size, void* buffer
) {
    unused( thread_index, item, out_resource, file, file2, buffer_size, buffer );
    return true;
}

b32 process_resource_audio(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input, FileHandle* output, usize buffer_size, void* buffer );
b32 process_resource_text(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input, FileHandle* output, usize buffer_size, void* buffer );
b32 process_resource_texture(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input, FileHandle* output, usize buffer_size, void* buffer );

global ProcessResourceFN* global_resource_functions[] = {
    process_resource_invalid, // invalid
    process_resource_audio,   // audio
    process_resource_invalid, // model
    process_resource_texture, // texture
    process_resource_text,    // text
};
static_assert(
    static_array_count( global_resource_functions ) ==
    PACKAGE_RESOURCE_TYPE_COUNT,
    "global_resource_functions must have the "
    "same number of functions as there are resource types!" );

void job_process_resource( usize thread_index, void* opaque_params ) {
    PackageError error = PACKAGE_SUCCESS;
    GlobalProcessResourceParams* params = global_process_resource_params;
    usize item_index   = (usize)opaque_params;
    ManifestItem* item = params->manifest->items.buffer + item_index;
    FileHandle* input_file = NULL, *temp_file = NULL, *output_file = NULL;
    path_buffer_empty( temp_path_buffer, 256 );

    usize thread_buffer_size = THREAD_BUFFER_SIZE;
    void* thread_buffer      = thread_buffer_get( thread_index );
    memory_zero( thread_buffer, thread_buffer_size );

    log_note(
        "processing '{s}'({usize}) . . .",
        item->identifier, item_index );

    PathBuffer item_path = {}; {
        usize capacity = params->manifest_directory.len + item->path.len + 1;
        char* buffer = system_alloc( capacity );
        if( !buffer ) {
            log_error( "failed to allocate full item path!" );
            error = PACKAGE_ERROR_OUT_OF_MEMORY;
            goto job_process_resource_end;
        }

        item_path.str = buffer;
        item_path.cap = capacity;
        memory_copy(
            item_path.c,
            params->manifest_directory.str,
            params->manifest_directory.len );
        item_path.len += params->manifest_directory.len;

        assert( path_buffer_push( &item_path, item->path ) );
    }

    input_file = fs_file_open(
        to_slice( &item_path ),
        FILE_OPEN_FLAG_READ | FILE_OPEN_FLAG_SHARE_ACCESS_READ );
    if( !input_file ) {
        log_error(
            "failed to open resource '{p}'!", to_slice( &item_path ) );
        error = PACKAGE_ERROR_OPEN_FILE;
        goto job_process_resource_end;
    }

    output_file = fs_file_open(
        params->output_path, FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_SHARE_ACCESS_WRITE );
    if( !output_file ) {
        log_error( "failed to open output file '{p}'!", params->output_path );
        error = PACKAGE_ERROR_OPEN_FILE;
        goto job_process_resource_end;
    }

    string_buffer_fmt( &temp_path_buffer, "./pkgtemp/{u32}.tmp", rand_xor_u32() );

    temp_file = fs_file_open(
        to_slice( &temp_path_buffer ),
        FILE_OPEN_FLAG_CREATE | FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_READ );
    if( !temp_file ) {
        log_error(
            "failed to open temp file '{s}'!",
            to_slice( &temp_path_buffer ) );
        error = PACKAGE_ERROR_OPEN_FILE;
        goto job_process_resource_end;
    }

    if( item->type > static_array_count( global_resource_functions ) ) {
        log_error( "unable to process resource, unrecognized resource type!" );
        error = PACKAGE_ERROR_PROCESS_RESOURCE;
        goto job_process_resource_end;
    }
    ProcessResourceFN* process = global_resource_functions[item->type];

    struct PackageResource resource = {};
    if( !process(
        thread_index, item, &resource, input_file, temp_file,
        thread_buffer_size, thread_buffer
    ) ) {
        error = PACKAGE_ERROR_PROCESS_RESOURCE;
        goto job_process_resource_end;
    }

    usize resource_offset = sizeof( struct PackageHeader );
    resource_offset += item_index * sizeof(struct PackageResource);

    usize temp_file_size  = fs_file_query_size( temp_file );
    if( !resource.type ) {
        temp_file_size = 0;
    }
    resource.size = temp_file_size;

    usize output_offset   = allocate_output_file_space( temp_file_size );
    usize relative_offset = output_offset;

    relative_offset += sizeof( struct PackageHeader );
    relative_offset += sizeof( struct PackageResource ) *
        params->manifest->items.count;

    resource.offset = output_offset;

    fs_file_set_offset( output_file, resource_offset, false );
    if( !fs_file_write( output_file, sizeof(resource), &resource ) ) {
        log_error(
            "failed to write {f,m,.2} to temp file!", (f64)sizeof(resource) );
        error = PACKAGE_ERROR_WRITE_FILE;
        goto job_process_resource_end;
    }

    fs_file_set_offset( output_file, relative_offset, false );
    fs_file_set_offset( temp_file, 0, false );
    if( !fs_file_to_file_copy(
        output_file, temp_file,
        thread_buffer_size, thread_buffer,
        temp_file_size
    ) ) {
        log_error( "failed to write to output file!" );
        error = PACKAGE_ERROR_WRITE_FILE;
        goto job_process_resource_end;
    }

    log_note(
        "successfully processed '{s}'({usize})!",
        item->identifier, item_index );
job_process_resource_end:
    system_free( item_path.v, item_path.len );
    fs_file_close( input_file );
    fs_file_close( output_file );
    fs_file_close( temp_file );

    if( error ) {
        params->error_code = error;
        log_error(
            "failed to process '{s}'({usize})!",
            item->identifier, item_index );
    }

    #undef RESOURCE_BUFFER_SIZE
}

usize allocate_output_file_space( usize size ) {
    usize result = interlocked_add( &global_file_offset, size );
    return result;
}

usize package_compression_stream( void* target, usize count, void* data ) {
    FileHandle* output = target;

    b32 write_result = fs_file_write( output, count, data );

    if( write_result ) {
        return 0;
    } else {
        return count;
    }
}


