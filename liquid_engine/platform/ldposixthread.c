// * Description:  Subset of platform functions that use pthread
// *               to implement mt functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 03, 2023
#include "ldplatform.h"
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "core/ldmemory.h"

typedef struct CStdThread {
    ThreadProcFN* thread_proc;
    void*         params;
    pthread_t     handle;
    u32           id;
    PlatformSemaphore* suspend_semaphore;
} CStdThread;

internal void* cstd_thread_proc( void* params ) {
    CStdThread* thread = params;
    if( thread->suspend_semaphore ) {
        platform_semaphore_wait( thread->suspend_semaphore, true, 0 );
    }

    b32 result = thread->thread_proc( thread->params );

    return result ? NULL : NULL;
}

usize PLATFORM_THREAD_HANDLE_SIZE = sizeof( CStdThread );
u32 RUNNING_THREAD_ID = 0;
b32 platform_thread_create(
    ThreadProcFN*   thread_proc,
    void*           thread_proc_params,
    usize           thread_stack_size,
    b32             create_suspended,
    PlatformThread* out_thread
) {
    CStdThread* thread  = out_thread;
    mem_zero( thread, sizeof(CStdThread) );

    thread->thread_proc = thread_proc;
    thread->params      = thread_proc_params;
    thread->id          = RUNNING_THREAD_ID++;

    pthread_attr_t attributes;
    int result = pthread_attr_init( &attributes );
    if( result ) {
        LOG_ERROR( "Failed to initialize thread attributes!" );
        return false;
    }
    result = pthread_attr_setstacksize( &attributes, thread_stack_size );
    if( result ) {
        LOG_ERROR(
            "Failed to set thread stack size! "
            "Requested stack size: {u64}",
            (u64)thread_stack_size
        );
        return false;
    }

    if( create_suspended ) {
        thread->suspend_semaphore = platform_semaphore_create( "", 0 );
        if( !thread->suspend_semaphore ) {
            LOG_ERROR( "Failed to create suspend semaphore!" );
            return false;
        }
    }

    read_write_fence();

    result = pthread_create(
        &thread->handle,
        &attributes,
        cstd_thread_proc,
        thread
    );

    read_write_fence();

    LOG_NOTE( "New thread created. ID: {u}", thread->id );

    return true;
}
void platform_thread_resume( PlatformThread* t ) {
    CStdThread* thread = t;
    platform_semaphore_increment( thread->suspend_semaphore );
}
void platform_thread_suspend( PlatformThread* t ) {
    CStdThread* thread = t;
    LOG_WARN( "pthread does not have a function for suspending a thread!" );
    LOG_WARN( "Attempted to suspend thread {u}.", thread->id );
}
void platform_thread_kill( PlatformThread* t ) {
    CStdThread* thread = t;
#if defined(SIGKILL)
    pthread_kill( thread->handle, SIGKILL );
#else
    LOG_WARN( "Current platform does not define SIGKILL!" );
    LOG_WARN(
        "Attempted to kill thread {u} on non-posix platform!",
        thread->id
    );
    unused(thread);
#endif
}

internal struct timespec ms_to_ts( u32 ms ) {
    struct timespec result;
    result.tv_sec  = ms / 1000;
    result.tv_nsec = (ms % 1000) * 1000 * 1000;
    return result;
}

PlatformSemaphore* platform_semaphore_create(
    const char* opt_name, u32 initial_count
) {
    // TODO(alicia): make sure mode is correct.
    mode_t mode   = S_IRWXU;
    int    oflag  = O_CREAT;
    sem_t* result = sem_open( opt_name, oflag, mode, initial_count );
    return result;
}
void platform_semaphore_increment( PlatformSemaphore* semaphore ) {
    sem_post( semaphore );
}
void platform_semaphore_wait(
    PlatformSemaphore* semaphore,
    b32 infinite_timeout, u32 opt_timeout_ms
) {
    if( infinite_timeout ) {
        sem_wait( semaphore );
    } else {
        struct timespec ts = ms_to_ts( opt_timeout_ms );
        sem_timedwait( semaphore, &ts );
    }
}
void platform_semaphore_destroy( PlatformSemaphore* semaphore ) {
    sem_close( semaphore );
}

STATIC_ASSERT(
    sizeof(pthread_mutex_t) == sizeof(PlatformMutex*),
    "sizes don't match!!!"
);
PlatformMutex* platform_mutex_create() {
    pthread_mutex_t mutex;
    if( pthread_mutex_init( &mutex, NULL ) ) {
        return NULL;
    }
    return (PlatformMutex*)mutex;
}
void platform_mutex_lock( PlatformMutex* platform_mutex ) {
    pthread_mutex_t mutex = (pthread_mutex_t)platform_mutex;
    pthread_mutex_lock( &mutex );
}
void platform_mutex_unlock( PlatformMutex* platform_mutex ) {
    pthread_mutex_t mutex = (pthread_mutex_t)platform_mutex;
    pthread_mutex_unlock( &mutex );
}
void platform_mutex_destroy( PlatformMutex* platform_mutex ) {
    pthread_mutex_t mutex = (pthread_mutex_t)platform_mutex;
    pthread_mutex_destroy( &mutex );
}

u32 platform_interlocked_increment_u32( volatile u32* addend ) {
    return __sync_fetch_and_add( addend, 1 );
}
u32 platform_interlocked_decrement_u32( volatile u32* addend ) {
    return __sync_fetch_and_sub( addend, 1 );
}
u32 platform_interlocked_exchange_u32( volatile u32* target, u32 value ) {
    return __sync_swap( target, value );
}
u32 platform_interlocked_compare_exchange_u32(
    volatile u32* dst,
    u32 exchange, u32 comperand
) {
    return __sync_val_compare_and_swap( dst, exchange, comperand );
}
void* platform_interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
) {
    return __sync_val_compare_and_swap( dst, exchange, comperand );
}



