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
global void* THREAD_HANDLE_BUFFER  = NULL;
global u32   THREAD_HANDLE_COUNT   = 0;

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
    ThreadInfo* thread_info = params;

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

usize thread_subsystem_query_size( u32 logical_processor_count ) {
    return sizeof(ThreadWorkQueue) +
        (PLATFORM_THREAD_HANDLE_SIZE * logical_processor_count);
}
b32 thread_subsystem_init( u32 logical_processor_count, void* buffer ) {
    WORK_QUEUE           = buffer;
    THREAD_HANDLE_BUFFER = (u8*)buffer + PLATFORM_THREAD_HANDLE_SIZE;

    read_write_fence();

    #define THREAD_CREATE_SUSPENDED (true)
    for( u32 i = 0; i < logical_processor_count; ++i ) {
        PlatformThread* thread_handle =
            (u8*)THREAD_HANDLE_BUFFER +
            (THREAD_HANDLE_COUNT * PLATFORM_THREAD_HANDLE_SIZE);
        if( !platform_thread_create(
            thread_proc, (void*)((usize)THREAD_HANDLE_COUNT),
            STACK_SIZE, THREAD_CREATE_SUSPENDED,
            thread_handle
        ) ) {
            break;
        }
        THREAD_HANDLE_COUNT++;
    }

    if( !THREAD_HANDLE_COUNT ) {
        LOG_FATAL( "Failed to create any threads!" );
        return false;
    }

    WORK_QUEUE->wake_semaphore = semaphore_create();
    if( !WORK_QUEUE->wake_semaphore ) {
        LOG_FATAL( "Failed to create wake semaphore!" );
        return false;
    }

    read_write_fence();

    for( u32 i = 0; i < THREAD_HANDLE_COUNT; ++i ) {
        PlatformThread* thread =
            (u8*)THREAD_HANDLE_BUFFER + (i * PLATFORM_THREAD_HANDLE_SIZE);
        platform_thread_resume( thread );
    }

    LOG_INFO( "Threading subsystem successfully initialized." );
    LOG_NOTE( "Instantiated {u} threads.", THREAD_HANDLE_COUNT );
    return true;
}
void thread_subsystem_shutdown(void) {
    for( u32 i = 0; i < THREAD_HANDLE_COUNT; ++i ) {
        PlatformThread* thread =
            (u8*)THREAD_HANDLE_BUFFER + (i * PLATFORM_THREAD_HANDLE_SIZE);
        platform_thread_kill( thread );
    }
    semaphore_destroy( WORK_QUEUE->wake_semaphore );
}
LD_API usize thread_info_query_index( ThreadInfo* generic_thread_info ) {
    return (usize)generic_thread_info;
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

