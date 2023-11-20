#if !defined(LP_SHARED_BUFFER_H)
#define LP_SHARED_BUFFER_H
/**
 * Description:  Buffer that is shared between threads when processing resources.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 15, 2023
*/
#include "defines.h"

#define PACKAGE_SHARED_BUFFER_SIZE (megabytes(256))
#define PACKAGE_SHARED_BUFFER_GRANULARITY (kilobytes(1))
#define PACKAGE_SHARED_BUFFER_FREE_LIST_COUNT\
    (PACKAGE_SHARED_BUFFER_SIZE / PACKAGE_SHARED_BUFFER_GRANULARITY)

b32 package_shared_buffer_initialize(void);
void package_shared_buffer_shutdown(void);
void* package_shared_buffer_allocate( usize size );
void package_shared_buffer_free( void* memory, usize size );

#endif /* header guard */
