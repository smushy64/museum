/**
 * Description:  Core Library Linux implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 23, 2023
*/
#include "shared/defines.h"

#if defined(LD_PLATFORM_LINUX)

#include "core/internal/logging.h"
#include "core/internal/platform.h"
#include "core/string.h"
#include "core/fs.h"
#include "core/memory.h"
#include "core/sync.h"
#include "core/time.h"
#include "core/system.h"
#include "core/print.h"

// TODO(alicia): replace malloc/free with mmap/munmap
#include <stdlib.h>
// TODO(alicia): replace f* with open and associated syscalls
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>

#define FD_STDIN  ((PlatformFile*)0)
#define FD_STDOUT ((PlatformFile*)1)
#define FD_STDERR ((PlatformFile*)2)

PlatformFile* platform_get_stdout(void) {
    return FD_STDOUT;
}
PlatformFile* platform_get_stderr(void) {
    return FD_STDERR;
}
PlatformFile* platform_get_stdin(void) {
    return FD_STDIN;
}

char* ___make_linux_path( PathSlice path, usize* out_size ) {
    if( path.buffer[path.len - 1] == 0 ) {
        *out_size = 0;
        return path.buffer;
    }

    usize size   = path.len + 1;
    char* buffer = malloc( size );

    memory_copy( buffer, path.buffer, path.len );
    buffer[path.len] = 0;
    
    *out_size = size;
    return buffer;
}
void ___destroy_path( char* path, usize size ) {
    if( size ) {
        free( path );
    }
}

PlatformFile* platform_file_open( PathSlice path, u32 flags ) {
    string_buffer_empty( args, 64 );
    usize p_size = 0;
    char* p      = ___make_linux_path( path, &p_size );

    #define push( char )\
        assert( string_buffer_push( &args, char ) )

    if( bitfield_check( flags, FILE_OPEN_FLAG_READ | FILE_OPEN_FLAG_WRITE ) ) {
        if( bitfield_check( flags, FILE_OPEN_FLAG_CREATE ) ) {
            push( 'w' );
            push( '+' );
        } else {
            push( 'r' );
            push( '+' );
        }
    } else if( bitfield_check( flags, FILE_OPEN_FLAG_READ ) ) {
        push( 'r' );
    } else if( bitfield_check( flags, FILE_OPEN_FLAG_WRITE ) ) {
        push( 'w' );
    }

    push( 'b' );
    push( 0 );

    FILE* result = fopen( p, args.buffer );
    if( !result ) {
        core_log_error( "failed to open file '{p}'!", path );
    }

    #undef push
    ___destroy_path( p, p_size );
    return result;
}
void platform_file_close( PlatformFile* file ) {
    if( file ) {
        fclose( file );
    }
}
usize platform_file_query_size( PlatformFile* file ) {
    usize offset = ftello64( file );
    fseeko64( file, 0, SEEK_END );
    usize size = ftello64( file );
    fseeko64( file, offset, SEEK_SET );

    return size;
}
usize platform_file_query_offset( PlatformFile* file ) {
    return ftello64( file );
}
void platform_file_set_offset( PlatformFile* file, usize offset ) {
    fseeko64( file, offset, SEEK_SET );
}
b32 platform_file_read( PlatformFile* file, usize buffer_size, void* buffer ) {
    int fd = 0;
    usize file_number = (usize)file;
    if( file_number <= (usize)FD_STDERR ) {
        fd = file_number;
    } else {
        fd = fileno( file );
        if( fd < 0 ) {
            core_log_error( "failed to get file's file descriptor!" );
            return false;
        }
    }

    ssize_t result = read( fd, buffer, buffer_size );
    if( result == -1 ) {
        core_log_error( "failed to read file!" );
        return false;
    }

    return true;

}
b32 platform_file_write( PlatformFile* file, usize buffer_size, void* buffer ) {
    int fd = 0;
    usize file_number = (usize)file;
    if( file_number <= (usize)FD_STDERR ) {
        fd = file_number;
    } else {
        fd = fileno( file );
    }
    ssize_t result = write( fd, buffer, buffer_size );
    if( result == -1 ) {
        return false;
    }

    if( (size_t)result != buffer_size ) {
        return false;
    }
    return true;
}
b32 platform_delete_file( PathSlice path ) {
    usize p_size = 0;
    char* p = ___make_linux_path( path, &p_size );

    int result = remove( p );

    ___destroy_path( p, p_size );
    return !result;
}
b32 platform_file_copy_by_path(
    PathSlice dst, PathSlice src, b32 fail_if_dst_exists
) {
    if( fail_if_dst_exists ) {
        PlatformFile* dst_file = platform_file_open( dst, FILE_OPEN_FLAG_READ );
        platform_file_close( dst_file );
        if( dst_file ) {
            return false;
        }
    }

    #define TEMP_SIZE (kilobytes(10))

    void* temp = malloc( TEMP_SIZE );
    if( !temp ) {
        return false;
    }

    PlatformFile* src_file = platform_file_open( src, FILE_OPEN_FLAG_READ );
    PlatformFile* dst_file = platform_file_open( dst, FILE_OPEN_FLAG_WRITE );

    b32 result = true;

    if( !src_file || !dst_file ) {
        result = false;
        goto platform_file_copy_by_path_end;
    }

    usize remaining = platform_file_query_size( src_file );
    while( remaining ) {
        usize copy_size = TEMP_SIZE;
        if( copy_size > remaining ) {
            copy_size = remaining;
        }

        if( !platform_file_read( src_file, copy_size, temp ) ) {
            result = false;
            goto platform_file_copy_by_path_end;
        }
        if( !platform_file_write( dst_file, copy_size, temp ) ) {
            result = false;
            goto platform_file_copy_by_path_end;
        }

        remaining -= copy_size;
    }

platform_file_copy_by_path_end:
    platform_file_close( src_file );
    platform_file_close( dst_file );
    free( temp );
    #undef TEMP_SIZE
    return result;
}
b32 platform_file_move_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    b32 copy_result = platform_file_copy_by_path( dst, src, fail_if_dst_exists );
    if( !copy_result ) {
        return false;
    }

    return platform_delete_file( src );
}
b32 platform_path_is_file( PathSlice path ) {
    usize p_size = 0;
    char* p = ___make_linux_path( path, &p_size );

    struct stat path_stat;
    stat( p, &path_stat );

    b32 result = S_ISREG( path_stat.st_mode );

    ___destroy_path( p, p_size );

    return result;
}
b32 platform_path_is_directory( PathSlice path ) {
    usize p_size = 0;
    char* p = ___make_linux_path( path, &p_size );

    struct stat path_stat;
    stat( p, &path_stat );

    b32 result = S_ISDIR( path_stat.st_mode );

    ___destroy_path( p, p_size );

    return result;
}

usize platform_get_working_directory(
    usize buffer_size, char* buffer, usize* opt_out_written_bytes
) {
    char* buf = malloc( FILENAME_MAX );
    getcwd( buf, FILENAME_MAX );

    usize path_size = cstr_len( buf );
    if( !buffer ) {
        free( buf );
        return path_size;
    }
    usize max_copy  = path_size > buffer_size ? buffer_size : path_size;

    memory_copy( buffer, buf, max_copy );
    if( opt_out_written_bytes ) {
        *opt_out_written_bytes = max_copy;
    }

    free( buf );
    return path_size - max_copy;
}

PlatformSharedObject* platform_shared_object_open( const char* path ) {
    void* handle = dlopen( path, RTLD_LAZY );
    if( !handle ) {
        core_log_error( "failed to load {cc}! error: {cc}", path, dlerror() );
    }
    return handle;
}
void platform_shared_object_close( PlatformSharedObject* object ) {
    dlclose( object );
}
void* platform_shared_object_load(
    PlatformSharedObject* object, const char* function_name
) {
    return dlsym( object, function_name );
}

struct PosixThread {
    PlatformThreadProc* proc;
    void* params;
    pthread_t thread_id;
};

void* start_routine(void* params) {
    struct PosixThread* posix_thread = params;
    int ret = posix_thread->proc( posix_thread->params );
    read_write_fence();

    free( posix_thread );
    return (void*)((isize)ret);
}

PlatformThread* platform_thread_create(
    PlatformThreadProc* thread_proc, void* thread_proc_params, usize stack_size
) {
    struct PosixThread* thread = malloc( sizeof( *thread ) );
    if( !thread ) {
        core_log_error( "failed to allocate PosixThread!" );
        return NULL;
    }

    pthread_attr_t attributes = {};
    int result = pthread_attr_init( &attributes );
    if( result ) {
        core_log_error( "failed to initialize pthread_attr_t!" );
        free( thread );
        return NULL;
    }
    result = pthread_attr_setstacksize( &attributes, stack_size );
    if( result ) {
        core_log_error( "failed to set stack size! stack size: {usize}", stack_size );
        pthread_attr_destroy( &attributes );
        free( thread );
        return NULL;
    }

    thread->proc   = thread_proc;
    thread->params = thread_proc_params;

    read_write_fence();

    result = pthread_create(
        &thread->thread_id, &attributes, start_routine, thread );
    pthread_attr_destroy( &attributes );

    if( result ) {
        free( thread );
        return NULL;
    }

    return (PlatformThread*)thread;
}

struct timespec ms_to_timespec( u32 ms ) {
    struct timespec result = {};
    result.tv_sec  = ms / 1000;
    result.tv_nsec = (ms % 1000) * 1000 * 1000;
    return result;
}

PlatformSemaphore* platform_semaphore_create( const char* name, u32 initial_count ) {
    mode_t mode  = S_IRWXU;
    int    oflag = O_CREAT;
    errno = 0;
    sem_t* result = sem_open( name, oflag, mode, initial_count );

    if( result == SEM_FAILED ) {
        core_log_error( "failed to create semaphore {cc}!", name );
    }

    return result;
}
void platform_semaphore_destroy( PlatformSemaphore* semaphore ) {
    sem_close( semaphore );
}
void platform_semaphore_signal( PlatformSemaphore* semaphore ) {
    sem_post( semaphore );
}
b32 platform_semaphore_wait( PlatformSemaphore* semaphore, u32 timeout_ms ) {
    if( timeout_ms == PLATFORM_INFINITE_TIMEOUT ) {
        struct timespec ts = ms_to_timespec( timeout_ms );
        errno = 0;
        sem_timedwait( semaphore, &ts );
        return errno != ETIMEDOUT;
    } else {
        sem_wait( semaphore );
        return true;
    }
}

PlatformMutex* platform_mutex_create( const char* name ) {
    pthread_mutex_t* result = malloc( sizeof(*result) );
    if( !result ) {
        core_log_error( "failed to allocate mutex!" );
        return NULL;
    }

    pthread_mutexattr_t attributes = {};
    int error = pthread_mutexattr_init( &attributes );
    if( error ) {
        core_log_error( "failed to create mutex attributes!" );
        free( result );
        return NULL;
    }

    error = pthread_mutex_init( result, &attributes );
    pthread_mutexattr_destroy( &attributes );

    if( error ) {
        free( result );
        return NULL;
    }

    unused(name);
    return result;
}
void platform_mutex_destroy( PlatformMutex* mutex ) {
    pthread_mutex_destroy( mutex );
    free( mutex );
}
b32 platform_mutex_lock( PlatformMutex* mutex, u32 timeout_ms ) {
    if( timeout_ms == PLATFORM_INFINITE_TIMEOUT ) {
        struct timespec ts = ms_to_timespec( timeout_ms );
        errno = 0;
        pthread_mutex_timedlock( mutex, &ts );
        return errno != ETIMEDOUT;
    } else {
        pthread_mutex_lock( mutex );
        return true;
    }
}
void platform_mutex_unlock( PlatformMutex* mutex ) {
    pthread_mutex_unlock( mutex );
}

void platform_sleep( u32 ms ) {
    struct timespec ts = ms_to_timespec( ms );
    nanosleep( &ts, NULL );
}

void* platform_heap_alloc( usize size ) {
    return malloc( size );
}
void* platform_heap_realloc( void* memory, usize old_size, usize new_size ) {
    unused( old_size );
    return realloc( memory, new_size );
}
void platform_heap_free( void* memory, usize size ) {
    unused( size );
    free( memory );
}

global struct timespec global_start_time = {};

void platform_time_initialize(void) {
    clock_gettime( CLOCK_MONOTONIC_RAW, &global_start_time );
}
f64 platform_time_query_elapsed_seconds(void) {
    struct timespec ts = {};
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );

    time_t seconds_elapsed     = ts.tv_sec  - global_start_time.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - global_start_time.tv_nsec;

    return (f64)seconds_elapsed + ( (f64)nanoseconds_elapsed / 1000000000.0 );
}
void platform_time_record( struct TimeRecord* out_record ) {
    time_t t = time(NULL);
    struct tm* time = localtime( &t );

    out_record->year   = time->tm_year + 1900;
    out_record->month  = time->tm_mon + 1;
    out_record->day    = time->tm_mday;
    out_record->hour   = time->tm_hour;
    out_record->minute = time->tm_min;
    out_record->second = time->tm_sec;
}

void platform_system_info_query( SystemInfo* out_info ) {
    // TODO(alicia): 
    memory_copy( out_info->cpu_name, "unknown", sizeof("unknown") );
    out_info->total_memory  = gigabytes(30);
    out_info->page_size     = kilobytes(4);
    out_info->cpu_count     = 16;
    out_info->feature_flags = CPU_FEATURE_SSE_MASK | CPU_FEATURE_AVX_MASK;
}

#endif /* Platform Linux */

