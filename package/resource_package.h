#if !defined(LP_RESOURCE_PACKAGE_H)
#define LP_RESOURCE_PACKAGE_H
/**
 * Description:  Liquid Package resource package.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 11, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"

struct Manifest;

typedef struct ResourcePackageParams {
    const char*         tmp_path;
    struct Manifest*    manifest;
    usize               index;
    volatile u32*       ready_signal;
    usize               buffer_size;
    void*               buffer;
} ResourcePackageParams;

void job_package_resource( usize thread_index, void* user_params );

/// Allocate given bytes in output file.
/// Returns offset into output file.
usize output_file_allocate( usize size );

#endif /* header guard */
