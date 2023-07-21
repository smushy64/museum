#if !defined(CORE_THREADING_HPP)
#define CORE_THREADING_HPP
/**
 * Description:  Multi-Threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

#if defined(LD_PLATFORM_WINDOWS) || defined(LD_PLATFORM_LINUX)
    #define SEMAPHORE_HANDLE_SIZE (sizeof(usize))
#endif

#if defined(LD_PLATFORM_WINDOWS)
    #define MUTEX_HANDLE_SIZE (sizeof(usize))
#endif
#if defined(LD_PLATFORM_LINUX)
    /// sizeof(pthread_mutex_t)
    #define MUTEX_HANDLE_SIZE (40)
#endif

/// Opaque handle to a semaphore object.
struct SemaphoreHandle {
    u8 buffer[SEMAPHORE_HANDLE_SIZE];
};

/// Opaque handle to a mutex object.
struct MutexHandle {
    u8 buffer[MUTEX_HANDLE_SIZE];
};

/// Opaque pointer to thread info.
typedef struct ThreadInfo* pThreadInfo;
/// Thread work function.
typedef void (*ThreadWorkProcFN) ( pThreadInfo thread_info, void* user_params );
/// Thread work entry
struct ThreadWorkEntry {
    ThreadWorkProcFN thread_work_proc;
    void*            thread_work_user_params;
};
/// Push a new thread work entry into the work queue.
LD_API void thread_work_queue_push(
    struct ThreadWorkQueue* work_queue,
    ThreadWorkEntry work_entry
);
/// Query the index of the current thread.
LD_API u32 thread_info_read_index( pThreadInfo thread_info );
/// Get on frame semaphore.
LD_API SemaphoreHandle* thread_info_on_frame_update_semaphore(
    pThreadInfo thread_info
);

/// Create a semaphore.
LD_API b32 semaphore_create(
    const char* opt_name, u32 initial_count,
    SemaphoreHandle* out_semaphore_handle
);
/// Increment a semaphore.
/// Optional: read the semaphore count before incrementing.
LD_API void semaphore_increment( SemaphoreHandle* semaphore_handle );
/// Wait for semaphore to be incremented.
/// Semaphore is decremented when it is signaled.
/// If infinite timeout is true, timeout ms is ignored.
LD_API void semaphore_wait(
    SemaphoreHandle* semaphore_handle,
    b32 infinite_timeout, u32 opt_timeout_ms
);
/// Destroy a semaphore handle.
LD_API void semaphore_destroy( SemaphoreHandle* semaphore_handle );

/// Create a mutex.
LD_API b32 mutex_create( MutexHandle* out_mutex );
/// Lock a mutex. Stalls until the mutex is available if another
/// thread already has ownership of it.
LD_API void mutex_lock( MutexHandle* mutex );
/// Unlock a mutex.
LD_API void mutex_unlock( MutexHandle* mutex );
/// Destroy a mutex handle.
LD_API void mutex_destroy( MutexHandle* mutex );

/// Complete all reads/writes before this fence.
NO_OPTIMIZE LD_API void read_write_fence();
/// Complete all reads before this fence.
NO_OPTIMIZE LD_API void read_fence();
/// Complete all writes before this fence.
NO_OPTIMIZE LD_API void write_fence();

#endif // header guard
