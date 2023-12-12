#if !defined(LD_SHARED_CORE_SYNC_H)
#define LD_SHARED_CORE_SYNC_H
/**
 * Description:  Thread synchronization.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "defines.h"

/// Semaphore handle.
typedef struct Semaphore {
    void* handle;
} Semaphore;
/// Mutex handle.
typedef struct Mutex {
    void* handle;
} Mutex;

/// Create a semaphore.
CORE_API b32 semaphore_create( Semaphore* out_semaphore );
/// Create a named semaphore.
CORE_API b32 semaphore_create_named( const char* name, Semaphore* out_semaphore );
/// Destroy a semaphore.
CORE_API void semaphore_destroy( Semaphore* semaphore );
/// Signal a semaphore.
CORE_API void semaphore_signal( Semaphore* semaphore );
/// Wait for a semaphore to be signaled indefinitely.
CORE_API void semaphore_wait( Semaphore* semaphore );
/// Wait for a semaphore to be signaled for specified milliseconds.
/// Returns false if timed out.
CORE_API b32 semaphore_wait_timed( Semaphore* semaphore, u32 ms );

/// Create a mutex.
CORE_API b32 mutex_create( Mutex* out_mutex );
/// Create a named mutex.
CORE_API b32 mutex_create_named( const char* name, Mutex* out_mutex );
/// Destroy a mutex.
CORE_API void mutex_destroy( Mutex* mutex );
/// Lock a mutex, wait indefinitely.
CORE_API void mutex_lock( Mutex* mutex );
/// Lock a mutex, wait for specified milliseconds.
/// Returns false if timed out.
CORE_API b32 mutex_lock_timed( Mutex* mutex, u32 ms );
/// Unlock a mutex.
CORE_API void mutex_unlock( Mutex* mutex );

/// Sleep thread for given milliseconds.
CORE_API void thread_sleep( u32 ms );

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

#endif /* header guard */
