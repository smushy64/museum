#if !defined(LP_WRITE_PACKAGE_H)
#define LP_WRITE_PACKAGE_H
/**
 * Description:  Write package
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 11, 2023
*/
#include "defines.h"
#include "core/collections.h"
#include "core/thread.h"
#include "core/internal.h"

#define PACKAGER_TMP_OUTPUT_PATH "./lpkg.tmp"

struct WritePackageParams {
    usize index;
    List* list_manifest_resources;
    Semaphore* finished;
};

void write_package( void* user_params );

#endif /* header guard */
