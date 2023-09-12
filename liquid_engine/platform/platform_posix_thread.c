// * Description:  Subset of platform functions that use pthread
// *               to implement mt functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 03, 2023
#include "ldplatform.h"
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "core/ldmemory.h"

typedef struct CStdThread {
    ThreadProcFN* thread_proc;
    void*         params;
    pthread_t     handle;
    u32           id;
    PlatformMutex* suspend;
    b32 resumed;
} CStdThread;

internal void* cstd_thread_proc( void* params ) {
    CStdThread* thread = params;
    if( thread->suspend ) {
        platform_mutex_lock( thread->suspend );
    }

    b32 result = thread->thread_proc( thread->params );

    pthread_exit( (void*)( result ? 0ULL : -1ULL ) );
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
    thread->resumed = true;

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
        thread->suspend = platform_mutex_create();
        if( !thread->suspend ) {
            LOG_ERROR( "Failed to create suspend mutex!" );
            return false;
        }

        platform_mutex_lock( thread->suspend );
        thread->resumed = false;
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
    if( thread->resumed ) {
        LOG_WARN( "Attempted to resume an already resumed thread!" );
    } else {
        platform_mutex_unlock( thread->suspend );
        thread->resumed = true;
    }
}
void platform_thread_suspend( PlatformThread* t ) {
    CStdThread* thread = t;
    if( thread->resumed ) {
        LOG_WARN( "Attempted to pause a running thread!" );
    }
}
void platform_thread_kill( PlatformThread* t ) {
    CStdThread* thread = t;
    unused(thread);
}

maybe_unused
internal struct timespec ms_to_ts( u32 ms ) {
    struct timespec result;
    result.tv_sec  = ms / 1000;
    result.tv_nsec = (ms % 1000) * 1000 * 1000;
    return result;
}

PlatformSemaphore* platform_semaphore_create(
    const char* opt_name, u32 initial_count
) {
    mode_t mode   = S_IRWXU;
    int    oflag  = O_CREAT;
    errno = 0;
    sem_t* result = sem_open( opt_name, oflag, mode, initial_count );

#if defined(LD_LOGGING)
    if( result == SEM_FAILED ) {
        LOG_ERROR( "Failed to create semaphore" );
        LOG_ERROR( "Name:  {cc}", opt_name );
        LOG_ERROR( "mode:  {u}", mode );
        LOG_ERROR( "oflag: {i}", oflag );
        LOG_ERROR( "errno = {i}", errno );
    }
#endif

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
        #if _POSIX_C_SOURCE >= 2200112L || _XOPEN_SOURCE >= 600
            struct timespec ts = ms_to_ts( opt_timeout_ms );
            sem_timedwait( semaphore, &ts );
        #else
            sem_wait( semaphore );
            unused(opt_timeout_ms);
            LOG_WARN( "sem_timedwait is not available!" );
        #endif
        
    }
}
void platform_semaphore_destroy( PlatformSemaphore* semaphore ) {
    sem_close( semaphore );
}

#if defined(LD_PLATFORM_WINDOWS)
STATIC_ASSERT(
    sizeof(pthread_mutex_t) == sizeof(PlatformMutex*),
    "sizes don't match!!!"
);
PlatformMutex* platform_mutex_create(void) {
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
#else
PlatformMutex* platform_mutex_create(void) {
    pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
    if( pthread_mutex_init( mutex, NULL ) ) {
        return NULL;
    }
    return (PlatformMutex*)mutex;
}
void platform_mutex_lock( PlatformMutex* platform_mutex ) {
    pthread_mutex_t* mutex = (pthread_mutex_t*)platform_mutex;
    pthread_mutex_lock( mutex );
}
void platform_mutex_unlock( PlatformMutex* platform_mutex ) {
    pthread_mutex_t* mutex = (pthread_mutex_t*)platform_mutex;
    pthread_mutex_unlock( mutex );
}
void platform_mutex_destroy( PlatformMutex* platform_mutex ) {
    pthread_mutex_t* mutex = (pthread_mutex_t*)platform_mutex;
    pthread_mutex_destroy( mutex );
    free( mutex );
}
#endif

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


