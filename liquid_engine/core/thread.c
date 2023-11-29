// * Description:  Multi-Threading Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "constants.h"
#include "core/thread.h"
#include "core/logging.h"
#include "core/string.h"
#include "core/internal.h"

typedef struct ThreadWorkEntry {
    ThreadWorkProcFN* proc;
    void*             params;
} ThreadWorkEntry;

#define MAX_WORK_ENTRY_COUNT (128)
typedef struct ThreadWorkQueue {
    ThreadWorkEntry work_entries[MAX_WORK_ENTRY_COUNT];

    Semaphore* wake_semaphore;

    volatile usize push_entry;
    volatile usize read_entry;
    volatile usize pending_work_count;
} ThreadWorkQueue;

global ThreadWorkQueue* WORK_QUEUE = NULL;

LD_API usize thread_work_query_pending_count(void) {
    return WORK_QUEUE->pending_work_count;
}

LD_API void thread_work_queue_push(
    ThreadWorkProcFN* work_proc, void* params
) {
    // TODO(alicia): what if we're already at max work entry count?
    
    ThreadWorkEntry entry = { work_proc, params };

    usize entry_index = interlocked_increment( &WORK_QUEUE->push_entry );
    interlocked_increment( &WORK_QUEUE->pending_work_count );
    read_write_fence();

    entry_index %= MAX_WORK_ENTRY_COUNT;

    WORK_QUEUE->work_entries[entry_index] = entry;

    read_write_fence();
    semaphore_signal( WORK_QUEUE->wake_semaphore );
}
internal b32 thread_work_queue_pop( ThreadWorkEntry* out_work_entry ) {
    if( WORK_QUEUE->push_entry == WORK_QUEUE->read_entry ) {
        return false;
    }

    usize read_index = interlocked_increment( &WORK_QUEUE->read_entry );
    read_write_fence();

    read_index %= MAX_WORK_ENTRY_COUNT;

    *out_work_entry = WORK_QUEUE->work_entries[read_index];

    return true;
}
internal b32 thread_proc( void* params ) {
    unused(params);
    loop {
        ThreadWorkEntry entry = {};
        semaphore_wait( WORK_QUEUE->wake_semaphore );
        read_write_fence();

        if( thread_work_queue_pop( &entry ) ) {
            entry.proc( entry.params );
            
            read_write_fence();

            interlocked_decrement( &WORK_QUEUE->pending_work_count );
        }
    }

    return true;
}

usize THREAD_SUBSYSTEM_SIZE = sizeof(ThreadWorkQueue);
b32 thread_subsystem_init( u32 logical_processor_count, void* buffer ) {
    WORK_QUEUE = buffer;

    WORK_QUEUE->wake_semaphore = semaphore_create();
    if( !WORK_QUEUE->wake_semaphore ) {
        fatal_log( "Failed to create wake semaphore!" );
        return false;
    }

    read_write_fence();

    u32 thread_handle_count = 0;
    for( u32 i = 0; i < logical_processor_count; ++i ) {
        if( !platform->thread.create(
            thread_proc, NULL,
            STACK_SIZE
        ) ) {
#if defined(LD_LOGGING)
            StringSlice last_error;
            platform->last_error(
                &last_error.len, (const char**)&last_error.buffer );
            error_log( "{s}", last_error );
#endif
            break;
        }
        thread_handle_count++;
    }

    if( !thread_handle_count ) {
        fatal_log( "Failed to create any threads!" );
        return false;
    }

    read_write_fence();

#if !defined(LD_THREAD_SUBSYSTEM_SILENT)
    info_log( "Threading subsystem successfully initialized." );
    note_log( "Instantiated {u} threads.", thread_handle_count );
#endif
    return true;
}

char SEM_NAME_BUFFER[255] = {};
usize SEM_NAME_INDEX = 0;
LD_API Semaphore* semaphore_create(void) {
    StringSlice name;
    name.buffer   = SEM_NAME_BUFFER;
    name.len      = 0;
    name.capacity = 255;

    string_slice_fmt( &name, "sem_{usize}", SEM_NAME_INDEX++ );

    Semaphore* result =
        platform->thread.semaphore_create( SEM_NAME_BUFFER, 0 );

#if defined(LD_LOGGING)
    if( !result ) {
        StringSlice last_error;
        platform->last_error(
            &last_error.len, (const char**)&last_error.buffer );
        error_log( "{s}", last_error );
    }
#endif

    return result;
}
LD_API void semaphore_signal( Semaphore* semaphore ) {
    platform->thread.semaphore_signal( semaphore );
}
LD_API void semaphore_wait( Semaphore* semaphore ) {
    platform->thread.semaphore_wait( semaphore );
}
LD_API void semaphore_wait_timed( Semaphore* semaphore, u32 ms ) {
    platform->thread.semaphore_wait_timed( semaphore, ms );
}
LD_API void semaphore_destroy( Semaphore* semaphore ) {
    platform->thread.semaphore_destroy( semaphore );
}

LD_API Mutex* mutex_create(void) {
    return platform->thread.mutex_create();
}
LD_API void mutex_lock( Mutex* mutex ) {
    platform->thread.mutex_lock( mutex );
}
LD_API void mutex_unlock( Mutex* mutex ) {
    platform->thread.mutex_unlock( mutex );
}
LD_API void mutex_destroy( Mutex* mutex ) {
    platform->thread.mutex_destroy( mutex );
}

