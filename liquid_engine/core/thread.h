#if !defined(CORE_THREADING_HPP)
#define CORE_THREADING_HPP
/**
 * Description:  Multi-Threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

/// Thread work function.
typedef void ThreadWorkProcFN( void* params );
/// Opaque Semaphore
typedef void Semaphore;
/// Opaque Mutex
typedef void Mutex;

/// Push a new work proc into the work queue.
LD_API void thread_work_queue_push( ThreadWorkProcFN* work_proc, void* params );
/// Query how many entries are still pending.
LD_API usize thread_work_query_pending_count(void);

/// Create a semaphore.
LD_API Semaphore* semaphore_create(void);
/// Signal a semaphore.
LD_API void semaphore_signal( Semaphore* semaphore );
/// Wait for a semaphore to be signaled.
LD_API void semaphore_wait( Semaphore* semaphore );
/// Wait for a semaphore to be signaled for milliseconds.
LD_API void semaphore_wait_timed( Semaphore* semaphore, u32 ms );
/// Destroy a semaphore.
LD_API void semaphore_destroy( Semaphore* semaphore );

/// Create a mutex.
LD_API Mutex* mutex_create(void);
/// Lock a mutex.
LD_API void mutex_lock( Mutex* mutex );
/// Unlock a mutex.
LD_API void mutex_unlock( Mutex* mutex );
/// Destroy a mutex.
LD_API void mutex_destroy( Mutex* mutex );

/// Multi-Threading safe add.
/// Returns previous value of addend.
#define interlocked_add( addend, value )\
    __sync_fetch_and_add( addend, value )
/// Multi-Threading safe subtract.
/// Returns previous value of addend.
#define interlocked_sub( addend, value )\
    __sync_fetch_and_sub( addend, value )
/// Multi-Threading safe increment.
/// Returns previous value of addend.
#define interlocked_increment( addend )\
    __sync_fetch_and_add( addend, 1 )
/// Multi-Threading safe decrement.
/// Returns previous value of addend.
#define interlocked_decrement( addend )\
    __sync_fetch_and_sub( addend, 1 )
/// Multi-Threading safe exchange.
#define interlocked_exchange( target, value )\
    __sync_val_compare_and_swap( target, *(target), value )
/// Multi-Threading safe compare and exchange.
#define interlocked_compare_exchange( dst, exchange, comperand )\
    __sync_val_compare_and_swap( dst, comperand, exchange )

#if defined(LD_ARCH_X86)
    /// Complete all reads and writes before this.
    #define read_write_fence()\
        __asm__ volatile ("mfence":::"memory")
    /// Complete all reads before this.
    #define read_fence()\
        __asm__ volatile ("lfence":::"memory")
    /// Complete all writes before this.
    #define write_fence()\
        __asm__ volatile ("sfence":::"memory")
#elif defined(LD_ARCH_ARM)
    // TODO(alicia): make sure these are correct for arm

    /// Complete all reads and writes before this.
    #define read_write_fence()\
        __asm__ volatile ("dmb":::"memory")
    /// Complete all reads before this.
    #define read_fence()\
        __asm__ volatile ("dmb":::"memory")
    /// Complete all writes before this.
    #define write_fence()\
        __asm__ volatile ("dmb st":::"memory")
#else
    #error "Fences not defined for current architecture!"
#endif

#if defined(LD_API_INTERNAL)

    /// Thread subsystem size.
    extern usize THREAD_SUBSYSTEM_SIZE;
    /// Initialize thread subsystem
    b32 thread_subsystem_init( u32 logical_processor_count, void* buffer );

#endif

#endif // header guard
