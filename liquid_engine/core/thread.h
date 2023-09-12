#if !defined(CORE_THREADING_HPP)
#define CORE_THREADING_HPP
/**
 * Description:  Multi-Threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

/// Opaque thread info.
typedef void ThreadInfo;

/// Thread work function.
typedef void ThreadWorkProcFN( ThreadInfo* thread_info, void* params );
/// Push a new work proc into the work queue.
LD_API void thread_work_queue_push( ThreadWorkProcFN* work_proc, void* params );
/// Get the current thread's index.
LD_API usize thread_info_query_index( ThreadInfo* thread_info );

/// Opaque Semaphore
typedef void Semaphore;
/// Create a semaphore.
LD_API Semaphore* semaphore_create(void);
/// Signal a semaphore.
LD_API void semaphore_signal( Semaphore* semaphore );
/// Wait for a semaphore to be signaled.
LD_API void semaphore_wait( Semaphore* semaphore );
/// Wait for a semaphore to be signaled for milliseconds.
LD_API void semaphore_wait_for( Semaphore* semaphore, u32 ms );
/// Destroy a semaphore.
LD_API void semaphore_destroy( Semaphore* semaphore );

/// Opaque Mutex
typedef void Mutex;
/// Create a mutex.
LD_API Mutex* mutex_create(void);
/// Lock a mutex.
LD_API void mutex_lock( Mutex* mutex );
/// Unlock a mutex.
LD_API void mutex_unlock( Mutex* mutex );
/// Destroy a mutex.
LD_API void mutex_destroy( Mutex* mutex );

/// Multi-Threading safe increment.
LD_API u32 interlocked_increment_u32( volatile u32* addend );
/// Multi-Threading safe decrement.
LD_API u32 interlocked_decrement_u32( volatile u32* addend );
/// Multi-Threading safe exchange.
LD_API u32 interlocked_exchange_u32( volatile u32* target, u32 value );
/// Multi-Threading safe compare and exchange.
LD_API u32 interlocked_compare_exchange_u32(
    volatile u32* dst,
    u32 exchange, u32 comperand
);
/// Multi-Threading safe compare and exchange.
LD_API void* interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
);

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

    /// Get thread subsystem size
    usize thread_subsystem_query_size( u32 logical_processor_count );
    /// Initialize thread subsystem
    b32 thread_subsystem_init( u32 logical_processor_count, void* buffer );
    /// Shutdown thread subsystem
    void thread_subsystem_shutdown(void);

#endif

#endif // header guard
