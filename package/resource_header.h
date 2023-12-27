#if !defined(LP_RESOURCE_HEADER_H)
#define LP_RESOURCE_HEADER_H
/**
 * Description:  Resource header generator.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 11, 2023
*/
#include "shared/defines.h"
#include "core/sync.h"

struct Manifest;
typedef struct GenerateHeaderParams {
    const char*      header_output_path;
    struct Manifest* manifest;
    Semaphore        finished;
} GenerateHeaderParams;

void job_generate_header( usize thread_index, void* user_params );

#endif /* header guard */
