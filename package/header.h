#if !defined(LD_PACKAGE_HEADER_H)
#define LD_PACKAGE_HEADER_H
/**
 * Description:  Utility Package Header Generator.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 09, 2024
*/
#include "shared/defines.h"
#include "core/path.h"

#include "package/manifest.h"
#include "package/error.h"

typedef struct HeaderGeneratorParams {
    PathSlice    output_path;
    Manifest*    manifest;
    PackageError error;
} HeaderGeneratorParams;

// NOTE(alicia): opaque_params == HeaderGeneratorParams
void job_header_generate( usize thread_index, void* opaque_params );

#endif /* header guard */
