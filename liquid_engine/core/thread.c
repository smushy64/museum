// * Description:  Multi-Threading Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "constants.h"
#include "core/thread.h"
#include "core/logging.h"
#include "core/strings.h"
#include "core/internal.h"

typedef struct ThreadWorkEntry {
    ThreadWorkProcFN* proc;
    void*             params;
} ThreadWorkEntry;

#define MAX_WORK_ENTRY_COUNT (128)
typedef struct ThreadWorkQueue {
    ThreadWorkEntry work_entries[MAX_WORK_ENTRY_COUNT];

    Semaphore* wake_semaphore;

    volatile i32 push_entry;
    volatile i32 read_entry;
    volatile i32 entry_completion_count;
    volatile i32 pending_work_count;
} ThreadWorkQueue;

global ThreadWorkQueue* WORK_QUEUE = NULL;

LD_API void thread_work_queue_push(
    ThreadWorkProcFN* work_proc, void* params
) {
    ThreadWorkEntry entry = { work_proc, params };
    WORK_QUEUE->work_entries[WORK_QUEUE->push_entry] = entry;

    read_write_fence();

    WORK_QUEUE->push_entry = interlocked_increment_i32(
        &WORK_QUEUE->push_entry ) % MAX_WORK_ENTRY_COUNT;

    WORK_QUEUE->pending_work_count = interlocked_increment_i32(
        &WORK_QUEUE->pending_work_count );

    assert_log(
        WORK_QUEUE->pending_work_count < MAX_WORK_ENTRY_COUNT,
        "Exceeded thread work entry count!!" );

    read_write_fence();
    semaphore_signal( WORK_QUEUE->wake_semaphore );
}
internal b32 thread_work_queue_pop( ThreadWorkEntry* out_work_entry ) {
    if(
        WORK_QUEUE->push_entry ==
        WORK_QUEUE->read_entry
    ) {
        return false;
    }

    *out_work_entry = WORK_QUEUE->work_entries[WORK_QUEUE->read_entry];

    read_write_fence();

    WORK_QUEUE->read_entry = interlocked_increment_i32(
        &WORK_QUEUE->read_entry
    ) % MAX_WORK_ENTRY_COUNT;

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

            WORK_QUEUE->entry_completion_count =
                interlocked_increment_i32( &WORK_QUEUE->entry_completion_count );
            WORK_QUEUE->pending_work_count =
                interlocked_decrement_i32( &WORK_QUEUE->pending_work_count );
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

    info_log( "Threading subsystem successfully initialized." );
    note_log( "Instantiated {u} threads.", thread_handle_count );
    return true;
}

LD_API i32 interlocked_increment_i32( volatile i32* addend ) {
    return platform->thread.interlocked_add( addend, 1 );
}
LD_API i32 interlocked_decrement_i32( volatile i32* addend ) {
    return platform->thread.interlocked_sub( addend, 1 );
}
LD_API i32 interlocked_exchange_i32( volatile i32* target, i32 value ) {
    return platform->thread.interlocked_exchange( target, value );
}
LD_API i32 interlocked_compare_exchange_i32(
    volatile i32* dst,
    i32 exchange, i32 comperand
) {
    return platform->thread.interlocked_compare_exchange( dst, exchange, comperand );
}
LD_API void* interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
) {
    return __sync_val_compare_and_swap(
        dst, exchange, comperand );
}

char SEM_NAME_BUFFER[255] = {};
usize SEM_NAME_INDEX = 0;
LD_API Semaphore* semaphore_create(void) {
    StringSlice name;
    name.buffer   = SEM_NAME_BUFFER;
    name.len      = 0;
    name.capacity = 255;

    ss_mut_fmt( &name, "sem_{usize}", SEM_NAME_INDEX++ );

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

