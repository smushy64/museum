// * Description:  Multi-Threading Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "core/ldthread.h"
#include "core/ldmemory.h"
#include "core/ldlog.h"
#include "ldplatform.h"

typedef struct ThreadInfoInternal {
    PlatformThread* thread;
    u32             thread_index;
} ThreadInfoInternal;

typedef struct ThreadWorkEntry {
    ThreadWorkProcFN* proc;
    void*             params;
} ThreadWorkEntry;

#define MAX_WORK_ENTRY_COUNT (128)
typedef struct ThreadWorkQueue {
    ThreadWorkEntry work_entries[MAX_WORK_ENTRY_COUNT];

    Semaphore* wake_semaphore;

    ThreadInfoInternal* threads;
    u32                 thread_count;

    volatile u32 push_entry;
    volatile u32 read_entry;
    volatile u32 entry_completion_count;
    volatile u32 pending_work_count;
} ThreadWorkQueue;

global ThreadWorkQueue* WORK_QUEUE = NULL;

LD_API void thread_work_queue_push( ThreadWorkProcFN* work_proc, void* params ) {
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
    ThreadInfoInternal* thread_info =
        (ThreadInfoInternal*)params;

    loop {
        ThreadWorkEntry entry = {};
        semaphore_wait( WORK_QUEUE->wake_semaphore );
        read_write_fence();

        if( thread_work_queue_pop( &entry ) ) {
            entry.proc( thread_info, entry.params );
            
            read_write_fence();

            WORK_QUEUE->entry_completion_count =
                interlocked_increment_u32( &WORK_QUEUE->entry_completion_count );
            WORK_QUEUE->pending_work_count =
                interlocked_decrement_u32( &WORK_QUEUE->pending_work_count );
        }
    }

    return true;
}

b32 thread_subsystem_init(
    u32 logical_processor_count,
    void* buffer
) {
    ThreadWorkQueue* work_queue = (ThreadWorkQueue*)buffer;

    usize thread_buffer_size =
        sizeof(ThreadInfoInternal) * logical_processor_count;
    work_queue->threads = ldalloc(
        thread_buffer_size,
        MEMORY_TYPE_ENGINE
    );

    read_write_fence();

    #define THREAD_CREATE_SUSPENDED (true)
    u32 thread_count = 0;
    for( u32 i = 0; i < logical_processor_count; ++i ) {
        ThreadInfoInternal* current_thread_info =
            &work_queue->threads[thread_count];
        current_thread_info->thread_index = thread_count;

        PlatformThread* thread = platform_thread_create(
            thread_proc,
            current_thread_info,
            STACK_SIZE,
            THREAD_CREATE_SUSPENDED
        );
        if( !thread ) {
            break;
        }

        current_thread_info->thread = thread;

        thread_count++;
    }

    if( !thread_count ) {
        LOG_FATAL( "Failed to create any threads!" );
        return false;
    }

    work_queue->thread_count = thread_count;

    LOG_NOTE( "Instantiated {u} threads.", work_queue->thread_count );

    work_queue->wake_semaphore = semaphore_create();
    if( !work_queue->wake_semaphore ) {
        LOG_FATAL( "Failed to create wake semaphore!" );
        return false;
    }

    WORK_QUEUE = work_queue;
    LOG_INFO( "Threading subsystem successfully initialized." );
    read_write_fence();

    for( u32 i = 0; i < work_queue->thread_count; ++i ) {
        ThreadInfoInternal* current_thread_info =
            &work_queue->threads[i];
        platform_thread_resume( current_thread_info->thread );
    }

    return true;
}
void thread_subsystem_shutdown() {
    for( u32 i = 0; i < WORK_QUEUE->thread_count; ++i ) {
        ThreadInfoInternal* current_thread_info =
            &WORK_QUEUE->threads[i];
        platform_thread_kill( current_thread_info->thread );
    }
    semaphore_destroy( WORK_QUEUE->wake_semaphore );
}
usize thread_query_subsystem_size() {
    return sizeof(ThreadWorkQueue);
}

LD_API u32 thread_info_index( ThreadInfo* generic_thread_info ) {
    ThreadInfoInternal* thread_info =
        (ThreadInfoInternal*)generic_thread_info;
    return thread_info->thread_index;
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

LD_API Semaphore* semaphore_create() {
    return platform_semaphore_create( NULL, 0 );
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

LD_API Mutex* mutex_create() {
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

