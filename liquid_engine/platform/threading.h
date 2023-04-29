#if !defined(THREADING_HPP)
#define THREADING_HPP
/**
 * Description:  Multi-threading functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 28, 2023
*/
#include "defines.h"

/// Opaque thread handle.
typedef void* ThreadHandle;
typedef unsigned long ThreadReturnValue;
/// Thread procedure prototype
typedef ThreadReturnValue (*ThreadProc)( void* params );

/// Create a thread.
SM_API ThreadHandle thread_create(
    ThreadProc thread_proc,
    void*      params,
    b32        run_on_creation
);
/// Resume a thread.
SM_API void thread_resume( ThreadHandle thread );

/// read-write fence
SM_API void mem_fence();
/// read fence
SM_API void read_fence();
/// write fence
SM_API void write_fence();

/// Opaque semaphore handle
typedef void* Semaphore;

/// Create a semaphore.
SM_API Semaphore semaphore_create(
    u32 initial_count,
    u32 maximum_count
);
/// Increment a semaphore.
/// Optional: get the semaphore count before incrementing
SM_API void semaphore_increment(
    Semaphore semaphore,
    u32       increment,
    u32*      opt_out_previous_count
);

/// Infinite semaphore timeout
#define TIMEOUT_INFINITE U32::MAX

/// Wait for semaphore to be incremented.
/// Decrements semaphore when it is signaled.
SM_API void semaphore_wait_for(
    Semaphore semaphore,
    u32       timeout_ms
);
/// Wait for multiple semaphores.
/// Set wait for all if waiting for all semaphores.
SM_API void semaphore_wait_for_multiple(
    usize      count,
    Semaphore* semaphores,
    b32        wait_for_all,
    u32        timeout_ms
);
/// Destroy a semaphore handle.
SM_API void semaphore_destroy( Semaphore semaphore );

SM_API u32 interlocked_increment( volatile u32* addend );
SM_API u32 interlocked_decrement( volatile u32* addend );
SM_API u32 interlocked_exchange( volatile u32* target, u32 value );
SM_API void* interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange,
    void* comperand
);
SM_API u32 interlocked_compare_exchange(
    u32 volatile* dst,
    u32 exchange,
    u32 comperand
);

#endif