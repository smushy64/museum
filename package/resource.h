#if !defined(LD_PACKAGE_RESOURCE_H)
#define LD_PACKAGE_RESOURCE_H
/**
 * Description:  Process resource.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 09, 2024
*/
#include "shared/defines.h"
#include "core/path.h"
#include "core/fs.h"

#include "package/manifest.h"

typedef struct GlobalProcessResourceParams {
    Manifest*    manifest;
    PathSlice    manifest_directory;
    PathSlice    output_path;
    PackageError error_code;
} GlobalProcessResourceParams;

extern GlobalProcessResourceParams* global_process_resource_params;

// NOTE(alicia): opaque_params == (usize)item index
void job_process_resource( usize thread_index, void* opaque_params );

typedef b32 ProcessResourceFN(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input_file, FileHandle* output_file, usize buffer_size, void* buffer );

#define THREAD_BUFFER_SIZE (megabytes(2))
void* thread_buffer_get( usize thread_index );

usize package_compression_stream( void* target, usize count, void* data );

#endif /* header guard */
