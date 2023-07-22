#if !defined(CORE_THREADING_HPP)
#define CORE_THREADING_HPP
/**
 * Description:  Multi-Threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

#define MAX_SEMAPHORE_SIZE (sizeof(usize))
/// Opaque Semaphore.
struct Semaphore {
    u8 buffer[MAX_SEMAPHORE_SIZE];
};
/// Create a semaphore.
LD_API b32 semaphore_create( Semaphore* out_semaphore );
/// Signal a semaphore.
LD_API void semaphore_signal( Semaphore* semaphore );
/// Wait for a semaphore to be signaled.
LD_API void semaphore_wait( Semaphore* semaphore );
/// Wait for a semaphore to be signaled for milliseconds.
LD_API void semaphore_wait_for( Semaphore* semaphore, u32 ms );
/// Destroy a semaphore.
LD_API void semaphore_destroy( Semaphore* semaphore );

#define MAX_MUTEX_SIZE (64)
/// Opaque Mutex.
struct Mutex {
    u8 buffer[MAX_MUTEX_SIZE];
};
/// Create a mutex.
LD_API b32 mutex_create( Mutex* out_mutex );
/// Lock a mutex.
LD_API void mutex_lock( Mutex* mutex );
/// Unlock a mutex.
LD_API void mutex_unlock( Mutex* mutex );
/// Destroy a mutex.
LD_API void mutex_destroy( Mutex* mutex );

/// Opaque thread info.
typedef void ThreadInfo;
/// Thread work function.
typedef void (*ThreadWorkProcFN)( ThreadInfo* thread_info, void* params );

/// Push a new work proc into the work queue.
LD_API void thread_work_queue_push( ThreadWorkProcFN work_proc, void* params );
/// Get the current thread's index.
LD_API u32 thread_info_index( ThreadInfo* thread_info );

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
    STATIC_ASSERT(false, "Fences not defined for current architecture!")
#endif

#if defined(LD_API_INTERNAL)

u32 query_threading_subsystem_size();
b32 threading_init(
    u32   logical_processor_count,
    void* buffer
);
void threading_shutdown();

#endif

#endif // header guard
