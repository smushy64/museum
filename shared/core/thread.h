#if !defined(LD_SHARED_CORE_THREAD_H)
#define LD_SHARED_CORE_THREAD_H
/**
 * Description:  Threads
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 12, 2023
*/
#include "defines.h"

/// Thread procedure prototype.
typedef int ThreadProcFN( void* user_params );

/// Create a thread.
/// Returns false if failed.
CORE_API b32 thread_create( ThreadProcFN* thread_proc, void* user_params );

#endif /* header guard */
