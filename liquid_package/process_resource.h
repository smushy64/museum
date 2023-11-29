#if !defined(LP_PROCESS_RESOURCE_H)
#define LP_PROCESS_RESOURCE_H
/**
 * Description:  Resource Processing
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 22, 2023
*/
#include "defines.h"
#include "core/collections.h"
#include "core/thread.h"

#define PACKAGER_TMP_OUTPUT_PATH "./lpkg.tmp"
struct PackagerResourceProcessParams {
    usize index;
    List* manifest_resources;
    const char* tmp_output_path;
    usize stream_buffer_size;
    void* stream_buffer;
    Semaphore* finished;
};

void packager_resource_process( void* params );

#endif /* header guard */
