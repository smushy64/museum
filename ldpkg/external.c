// * Description:  Compile required files from Liquid Engine for
// *               Liquid Engine Resource Packager
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 19, 2023
#include <core/ldlog.c>
#include <core/ldmemory.c>
#include <core/ldthread.c>
#include <core/ldstring.c>
#include <core/ldallocator.c>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

PlatformFile* platform_file_open(
    const char* path, PlatformFileOpenFlag flags
) {
    const char* mode = "";
    if( CHECK_BITS( flags, PLATFORM_FILE_OPEN_READ ) ) {
        if( CHECK_BITS( flags, PLATFORM_FILE_OPEN_WRITE ) ) {
            mode = "wb+";
        } else {
            mode = "rb";
        }
    } else if( CHECK_BITS( flags, PLATFORM_FILE_OPEN_WRITE ) ) {
        mode = "wb";
    } else {
        return NULL;
    }

    FILE* file = fopen( path, mode );
    if( !file ) {
        LOG_ERROR( "Failed to open file \"{cc}\"!", path );
    }
    return file;
}
void platform_file_close( PlatformFile* file ) {
    fclose( file );
}
b32 platform_file_read(
    PlatformFile* file,
    usize read_size, usize buffer_size,
    void* buffer
) {
    ASSERT( buffer_size >= read_size );
    size_t result = fread( buffer, sizeof(u8), read_size, file );
    if( result == read_size ) {
        return true;
    } else {
        if( feof( file ) ) {
            return true;
        } else {
            return false;
        }
    }
}
b32 platform_file_write(
    PlatformFile* file,
    usize write_size, usize buffer_size,
    void* buffer
) {
    ASSERT( buffer_size >= write_size );
    size_t result = fwrite( buffer, sizeof(u8), write_size, file );
    if( result != write_size ) {
        LOG_ERROR( "Failed to write {u64} bytes!", (u64)write_size );
        return false;
    } else {
        return true;
    }
}
usize platform_file_query_size( PlatformFile* file ) {
    fseeko( file, 0, SEEK_END );
    usize size = ftello( file );
    rewind( file );
    return size;
}
usize platform_file_query_offset( PlatformFile* file ) {
    return ftello( file );
}
b32 platform_file_set_offset( PlatformFile* file, usize offset ) {
    usize result = fseeko( file, offset, SEEK_SET );
    if( result != 0 ) {
        LOG_ERROR( "Failed to set file offset!" );
        return false;
    }
    return true;
}

void* platform_stdout_handle() {
    return stdout;
}
void* platform_stderr_handle() {
    return stderr;
}

void platform_write_console(
    void* output_handle,
    u32 write_count,
    const char* buffer
) {
    fprintf( output_handle, "%.*s", write_count, buffer );
}

void* platform_heap_alloc( usize size ) {
    void* result = malloc( size );
    memset( result, 0, size );
    return result;
}
void* platform_heap_realloc( void* memory, usize new_size ) {
    return realloc( memory, new_size );
}
void platform_heap_free( void* memory ) {
    free( memory );
}

void* platform_page_alloc( usize size ) {
    void* result = malloc( size );
    memset( result, 0, size );
    return result;
}
void platform_page_free( void* memory ) {
    free( memory );
}

PlatformSemaphore* platform_semaphore_create(
    const char* opt_name, u32 initial_count
) {
    sem_t* semaphore = sem_open( opt_name, O_CREAT, S_IRWXU, initial_count );
    return semaphore;
}
void platform_semaphore_increment( PlatformSemaphore* semaphore ) {
    sem_t* sem = semaphore;
    sem_post( sem );
}
void platform_semaphore_wait(
    PlatformSemaphore* semaphore,
    b32 infinite_timeout, u32 opt_timeout_ms
) {
    sem_t* sem = semaphore;
    if( infinite_timeout ) {
        sem_wait( sem );
    } else {
        struct timespec ts;
        ts.tv_sec  = opt_timeout_ms / 1000;
        ts.tv_nsec = ( opt_timeout_ms % 1000 ) * 1000 * 1000;
        sem_timedwait( sem, &ts );
    }
}
void platform_semaphore_destroy( PlatformSemaphore* semaphore ) {
    sem_t* sem = semaphore;
    sem_close( sem );
}

PlatformMutex* platform_mutex_create() {
    pthread_mutex_t mutex;
    pthread_mutex_init( &mutex, 0 );
    return (PlatformMutex*)mutex;
}
void platform_mutex_lock( PlatformMutex* pmutex ) {
    pthread_mutex_t mutex = (pthread_mutex_t)pmutex;
    pthread_mutex_lock( &mutex );
}
void platform_mutex_unlock( PlatformMutex* pmutex ) {
    pthread_mutex_t mutex = (pthread_mutex_t)pmutex;
    pthread_mutex_unlock( &mutex );
}
void platform_mutex_destroy( PlatformMutex* pmutex ) {
    pthread_mutex_t mutex = (pthread_mutex_t)pmutex;
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
    u32 volatile* dst,
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

struct CStdThread {
    pthread_t      handle;
    PlatformMutex* suspend_mtx;
    ThreadProcFN   proc;
    void*          params;
};

void* cstd_thread_proc( void* params ) {
    struct CStdThread* thread = params;

    platform_mutex_lock( thread->suspend_mtx );

    read_write_fence();

    b32 result = thread->proc( thread->params );

    if( result ) {
        pthread_exit( (void*)0 );
    } else {
        pthread_exit( (void*)-1 );
    }

    return 0;
}

PlatformThread* platform_thread_create(
    ThreadProcFN thread_proc,
    void*        user_params,
    usize        thread_stack_size,
    b32          create_suspended
) {
    struct CStdThread* thread = ldalloc(
        sizeof(struct CStdThread), MEMORY_TYPE_USER );
    if( !thread ) {
        return NULL;
    }

    thread->proc   = thread_proc;
    thread->params = user_params;
    thread->suspend_mtx = platform_mutex_create();
    if( !thread->suspend_mtx ) {
        ldfree( thread, sizeof(struct CStdThread), MEMORY_TYPE_USER );
        return NULL;
    }

    if( create_suspended ) {
        platform_mutex_lock( thread->suspend_mtx );
    }

    pthread_attr_t attributes;
    pthread_attr_init( &attributes );
    pthread_attr_setstacksize( &attributes, thread_stack_size );
    
    read_write_fence();

    i32 create_result = pthread_create(
        &thread->handle,
        &attributes,
        cstd_thread_proc,
        thread
    );
    if( create_result != 0 ) {
        ldfree( thread, sizeof(struct CStdThread), MEMORY_TYPE_USER );
        return NULL;
    }

    return thread;
}
void platform_thread_resume( PlatformThread* t ) {
    struct CStdThread* thread = t;
    platform_mutex_unlock( thread->suspend_mtx );
}
void platform_thread_suspend( PlatformThread* t ) {
    struct CStdThread* thread = t;
    platform_mutex_lock( thread->suspend_mtx );
}
void platform_thread_kill( PlatformThread* t ) {
    struct CStdThread* thread = t;
    pthread_kill( thread->handle, SIGABRT );
    read_write_fence();
    ldfree( thread, sizeof(struct CStdThread), MEMORY_TYPE_USER );
}

#if defined(LD_PLATFORM_WINDOWS)
#include <windows.h>
void platform_win32_output_debug_string( const char* str ) {
    OutputDebugStringA( str );
}
#endif

