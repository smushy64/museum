#if !defined(LD_SHARED_CORE_JOBS_H)
#define LD_SHARED_CORE_JOBS_H
/**
 * Description:  Multi-Threaded jobs system.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 05, 2023
*/
#include "defines.h"

/// Job function prototype.
typedef void JobProcFN( void* user_params );

/// Query memory requirement for job system.
CORE_API usize job_system_query_memory_requirement( u32 thread_count );
/// Intialize job system.
/// Buffer must be able to hold result from job_system_query_memory_requirement.
/// Returns false if there was an error.
CORE_API b32 job_system_initialize( u32 thread_count, void* buffer );
/// Shutdown job system.
CORE_API void job_system_shutdown(void);

// TODO(alicia): push_wait, push_wait_timed

/// Attempt to add a job to job stack.
CORE_API b32 job_system_push( JobProcFN* job, void* user_params );

/// Wait for all entries to complete.
CORE_API void job_system_wait(void);
/// Wait for all entries to complete or timeout.
/// Returns false if timedout.
CORE_API b32 job_system_wait_timed( u32 ms );

#endif /* header guard */
