#if !defined(CORE_THREADING_HPP)
#define CORE_THREADING_HPP
/**
 * Description:  Multi-Threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

#define MAX_SEMAPHORE_HANDLES 32
/// Handle to a semaphore object.
struct SemaphoreHandle {
    void* platform;
};
/// Create a semaphore.
SM_API b32 semaphore_create(
    u32 initial_count, u32 maximum_count,
    SemaphoreHandle* out_semaphore_handle
);
/// Increment a semaphore.
/// Optional: read the semaphore count before incrementing.
SM_API void semaphore_increment(
    SemaphoreHandle* semaphore_handle,
    u32 increment, u32* out_opt_previous_count
);
/// Wait for semaphore to be incremented.
/// Semaphore is decremented when it is signaled.
/// If infinite timeout is true, timeout ms is ignored.
SM_API void semaphore_wait(
    SemaphoreHandle* semaphore_handle,
    b32 infinite_timeout, u32 opt_timeout_ms
);
/// Wait for any/all semaphores to be incremented.
/// Signaled semaphore is decremented.
/// If infinite timeout is true, timeout ms is ignored.
SM_API void semaphore_wait_multiple(
    usize            semaphore_handle_count,
    SemaphoreHandle* semaphore_handles,
    b32 wait_for_all, b32 infinite_timeout,
    u32 opt_timeout_ms
);
/// Destroy a semaphore handle.
SM_API void semaphore_destroy( SemaphoreHandle* semaphore_handle );

/// Complete all reads/writes before this fence.
SM_NO_OPTIMIZE SM_API void read_write_fence();
/// Complete all reads before this fence.
SM_NO_OPTIMIZE SM_API void read_fence();
/// Complete all writes before this fence.
SM_NO_OPTIMIZE SM_API void write_fence();

typedef struct ThreadInfo* pThreadInfo;
typedef void (*ThreadWorkProcFN)
    ( pThreadInfo thread_info, void* user_params );

#define THREAD_WORK_ENTRY_COUNT 256
/// Thread work entry
struct ThreadWorkEntry {
    ThreadWorkProcFN thread_work_proc;
    void*            thread_work_user_params;
};
SM_API void thread_work_queue_push(
    struct ThreadWorkQueue* work_queue,
    ThreadWorkEntry work_entry
);
SM_API u32 thread_info_read_index( pThreadInfo thread_info );

#endif // header guard