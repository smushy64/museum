#if !defined(LP_INTERNAL_WRITE_HEADER_H)
#define LP_INTERNAL_WRITE_HEADER_H
/**
 * Description:  Function for writing package resource header.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 07, 2023
*/
#include <defines.h>
#include <core/collections.h>
#include <core/thread.h>

#define PACKAGER_DEFAULT_ENUM_NAME "GeneratedResourceID"

struct WriteHeaderParams {
    const char* output_path;
    const char* enum_name;
    List*       list_manifest_resources;
    Semaphore*  finished;
};

void write_header( void* user_params );

#endif /* header guard */
