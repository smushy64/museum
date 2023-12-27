/**
 * Description:  Threads implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 12, 2023
*/
#include "shared/defines.h"
#include "core/thread.h"
#include "core/internal/platform.h"

CORE_API b32 thread_create( ThreadProcFN* thread_proc, void* user_params ) {
    PlatformThread* thread =
        platform_thread_create( thread_proc, user_params, STACK_SIZE );
    return thread != NULL;
}


