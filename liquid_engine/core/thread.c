// * Description:  Multi-Threading Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "constants.h"
#include "core/thread.h"
#include "core/mem.h"
#include "core/log.h"
#include "platform.h"

typedef struct ThreadWorkEntry {
    ThreadWorkProcFN* proc;
    void*             params;
} ThreadWorkEntry;

#define MAX_WORK_ENTRY_COUNT (128)
typedef struct ThreadWorkQueue {
    ThreadWorkEntry work_entries[MAX_WORK_ENTRY_COUNT];

    Semaphore* wake_semaphore;

    volatile u32 push_entry;
    volatile u32 read_entry;
    volatile u32 entry_completion_count;
    volatile u32 pending_work_count;
} ThreadWorkQueue;

global ThreadWorkQueue* WORK_QUEUE = NULL;

LD_API void thread_work_queue_push(
    ThreadWorkProcFN* work_proc, void* params
) {
    ThreadWorkEntry entry = { work_proc, params };
    WORK_QUEUE->work_entries[WORK_QUEUE->push_entry] = entry;

    read_write_fence();

    WORK_QUEUE->push_entry = interlocked_increment_u32(
        &WORK_QUEUE->push_entry
    ) % MAX_WORK_ENTRY_COUNT;

    WORK_QUEUE->pending_work_count = interlocked_increment_u32(
        &WORK_QUEUE->pending_work_count
    );

    LOG_ASSERT(
        WORK_QUEUE->pending_work_count < MAX_WORK_ENTRY_COUNT,
        "Exceeded thread work entry count!!"
    );

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

    WORK_QUEUE->read_entry = interlocked_increment_u32(
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
                interlocked_increment_u32( &WORK_QUEUE->entry_completion_count );
            WORK_QUEUE->pending_work_count =
                interlocked_decrement_u32( &WORK_QUEUE->pending_work_count );
        }
    }

    return true;
}

usize THREAD_SUBSYSTEM_SIZE = sizeof(ThreadWorkQueue);
b32 thread_subsystem_init( u32 logical_processor_count, void* buffer ) {
    WORK_QUEUE = buffer;

    WORK_QUEUE->wake_semaphore = semaphore_create();
    if( !WORK_QUEUE->wake_semaphore ) {
        LOG_FATAL( "Failed to create wake semaphore!" );
        return false;
    }

    read_write_fence();

    u32 thread_handle_count = 0;
    for( u32 i = 0; i < logical_processor_count; ++i ) {
        if( !platform_thread_create(
            thread_proc, NULL,
            STACK_SIZE
        ) ) {
            break;
        }
        thread_handle_count++;
    }

    if( !thread_handle_count ) {
        LOG_FATAL( "Failed to create any threads!" );
        return false;
    }

    read_write_fence();

    LOG_INFO( "Threading subsystem successfully initialized." );
    LOG_NOTE( "Instantiated {u} threads.", thread_handle_count );
    return true;
}
void thread_subsystem_shutdown(void) {
    semaphore_destroy( WORK_QUEUE->wake_semaphore );
}

LD_API u32 interlocked_increment_u32( volatile u32* addend ) {
    return platform_interlocked_increment_u32( addend );
}
LD_API u32 interlocked_decrement_u32( volatile u32* addend ) {
    return platform_interlocked_decrement_u32( addend );
}
LD_API u32 interlocked_exchange_u32( volatile u32* target, u32 value ) {
    return platform_interlocked_exchange_u32( target, value );
}
LD_API u32 interlocked_compare_exchange_u32(
    volatile u32* dst,
    u32 exchange, u32 comperand
) {
    return platform_interlocked_compare_exchange_u32(
        dst, exchange, comperand
    );
}
LD_API void* interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
) {
    return platform_interlocked_compare_exchange_pointer(
        dst, exchange, comperand
    );
}

char SEM_NAME_BUFFER[255] = {};
usize SEM_NAME_INDEX = 0;
LD_API Semaphore* semaphore_create(void) {
    StringSlice name;
    name.buffer   = SEM_NAME_BUFFER;
    name.len      = 255;
    name.capacity = 255;

    ss_mut_fmt( &name, "sem{u64}", (u64)SEM_NAME_INDEX );
    SEM_NAME_INDEX++;

    return platform_semaphore_create( SEM_NAME_BUFFER, 0 );
}
LD_API void semaphore_signal( Semaphore* semaphore ) {
    platform_semaphore_increment( semaphore );
}
LD_API void semaphore_wait( Semaphore* semaphore ) {
    platform_semaphore_wait( semaphore, true, 0 );
}
LD_API void semaphore_wait_for( Semaphore* semaphore, u32 ms ) {
    platform_semaphore_wait( semaphore, false, ms );
}
LD_API void semaphore_destroy( Semaphore* semaphore ) {
    platform_semaphore_destroy( semaphore );
}

LD_API Mutex* mutex_create(void) {
    return platform_mutex_create();
}
LD_API void mutex_lock( Mutex* mutex ) {
    platform_mutex_lock( mutex );
}
LD_API void mutex_unlock( Mutex* mutex ) {
    platform_mutex_unlock( mutex );
}
LD_API void mutex_destroy( Mutex* mutex ) {
    platform_mutex_destroy( mutex );
}

