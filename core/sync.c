/**
 * Description:  Thread synchronization implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"
#include "core/sync.h"
#include "core/string.h"
#include "core/internal/platform.h"

global volatile u32 global_running_semaphore_counter = 0;
global volatile u32 global_running_mutex_counter     = 0;

CORE_API b32 semaphore_create( Semaphore* out_semaphore ) {
    u32 next_number = interlocked_increment( &global_running_semaphore_counter );
    char small_buffer[64] = {};
    StringSlice slice;
    slice.buffer   = small_buffer;
    slice.capacity = 64;
    slice.len      = 0;

    string_slice_fmt( &slice, "sem{u,x}{c}", next_number, 0 );

    void* handle = platform_semaphore_create( small_buffer, 0 );
    if( !handle ) {
        return false;
    }

    out_semaphore->handle = handle;
    return true;
}
CORE_API b32 semaphore_create_named( const char* name, Semaphore* out_semaphore ) {
    void* handle = platform_semaphore_create( name, 0 );
    if( !handle ) {
        return false;
    }

    out_semaphore->handle = handle;
    return true;
}
CORE_API void semaphore_destroy( Semaphore* semaphore ) {
    platform_semaphore_destroy( semaphore->handle );
    semaphore->handle = NULL;
}
CORE_API void semaphore_signal( Semaphore* semaphore ) {
    platform_semaphore_signal( semaphore->handle );
}
CORE_API void semaphore_wait( Semaphore* semaphore ) {
    platform_semaphore_wait( semaphore->handle, PLATFORM_INFINITE_TIMEOUT );
}
CORE_API b32 semaphore_wait_timed( Semaphore* semaphore, u32 ms ) {
    return platform_semaphore_wait( semaphore->handle, ms );
}
CORE_API b32 mutex_create_named( const char* name, Mutex* out_mutex ) {
    void* handle = platform_mutex_create( name );
    if( !handle ) {
        return false;
    }

    out_mutex->handle = handle;
    return true;
}
CORE_API b32 mutex_create( Mutex* out_mutex ) {
    u32 next_number = interlocked_increment( &global_running_mutex_counter );
    char small_buffer[64] = {};
    StringSlice slice;
    slice.buffer   = small_buffer;
    slice.capacity = 64;
    slice.len      = 0;

    string_slice_fmt( &slice, "mtx{u,x}{c}", next_number, 0 );

    void* handle = platform_mutex_create( small_buffer );
    if( !handle ) {
        return false;
    }

    out_mutex->handle = handle;
    return true;
}
CORE_API void mutex_destroy( Mutex* mutex ) {
    platform_mutex_destroy( mutex->handle );
    mutex->handle = NULL;
}
CORE_API void mutex_lock( Mutex* mutex ) {
    platform_mutex_lock( mutex->handle, PLATFORM_INFINITE_TIMEOUT );
}
CORE_API b32 mutex_lock_timed( Mutex* mutex, u32 ms ) {
    return platform_mutex_lock( mutex->handle, ms );
}
CORE_API void mutex_unlock( Mutex* mutex ) {
    platform_mutex_unlock( mutex->handle );
}

CORE_API void thread_sleep( u32 ms ) {
    platform_sleep( ms );
}

CORE_API usize core_query_processor_count(void) {
    return platform_query_processor_count();
}

