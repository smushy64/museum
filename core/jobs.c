/**
 * Description:  Jobs system implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 05, 2023
*/
#include "shared/defines.h"
#include "core/jobs.h"
#include "core/sync.h"
#include "core/memory.h"
#include "core/internal/logging.h"
#include "core/internal/platform.h"

/// Entry in job stack.
typedef struct JobEntry {
    JobProcFN* proc;
    void*      user_params;
} JobEntry;

#define JOB_STACK_MAX_ENTRY_COUNT (64)
typedef struct JobStack {
    u32 size;
    u32 thread_count;
    volatile u32 write_index;
    volatile u32 read_index;
    volatile u32 remaining_entries;
    volatile u32 end_count;
    volatile b32 end_signal;

    JobEntry entries[JOB_STACK_MAX_ENTRY_COUNT];

    Semaphore wake;
    Semaphore entry_completed;
    PlatformThread* threads[];
} JobStack;

global JobStack* global_job_stack         = NULL;

CORE_API usize job_system_query_memory_requirement( u32 thread_count ) {
    return sizeof( JobStack ) + (sizeof( PlatformThread* ) * thread_count);
}

internal b32 ___internal_job_system_pop( JobEntry* out_entry ) {
    if(
        global_job_stack->read_index == global_job_stack->write_index ||
        global_job_stack->end_signal
    ) {
        return false;
    }

    usize read_index = interlocked_increment( &global_job_stack->read_index );
    read_write_fence();

    read_index %= JOB_STACK_MAX_ENTRY_COUNT;

    *out_entry = global_job_stack->entries[read_index];
    return true;
}

internal int ___internal_job_system_proc( void* user_params ) {
    usize thread_index = (usize)user_params;
    loop {
        JobEntry entry = {};
        semaphore_wait( &global_job_stack->wake );
        read_write_fence();

        if( global_job_stack->end_signal ) {
            interlocked_increment( &global_job_stack->end_count );
            break;
        }

        if( ___internal_job_system_pop( &entry ) ) {
            read_write_fence();
            entry.proc( thread_index, entry.user_params );
            read_write_fence();
            interlocked_decrement( &global_job_stack->remaining_entries );
            semaphore_signal( &global_job_stack->entry_completed );
        }
    }

    return 0;
}
CORE_API b32 job_system_initialize( u32 thread_count, void* buffer ) {
    global_job_stack       = buffer;
    global_job_stack->size = job_system_query_memory_requirement( thread_count );

    if( !semaphore_create( &global_job_stack->wake ) ) {
        core_log_fatal( "failed to create job system wake semaphore!" );
        return false;
    }
    if( !semaphore_create( &global_job_stack->entry_completed ) ) {
        core_log_fatal( "failed to create job system entry completed semaphore!" );
        semaphore_destroy( &global_job_stack->wake );
        return false;
    }

    read_write_fence();

    for( u32 i = 0; i < thread_count; ++i ) {
        usize thread_index = i + 1;

        global_job_stack->threads[i] = platform_thread_create(
            ___internal_job_system_proc, (void*)thread_index, STACK_SIZE );

        if( !global_job_stack->threads[i] ) {
            core_log_fatal( "job system failed to create thread {u}!", i );
            job_system_shutdown();
            return false;
        }

        global_job_stack->thread_count++;
    }

    read_write_fence();
    return true;
}
CORE_API void job_system_shutdown(void) {
    global_job_stack->end_signal = true;
    read_write_fence();

    do {
        semaphore_signal( &global_job_stack->wake );
    } while( global_job_stack->end_count < global_job_stack->thread_count );

    read_write_fence();
    semaphore_destroy( &global_job_stack->wake );
    semaphore_destroy( &global_job_stack->entry_completed );

    memory_zero( global_job_stack, global_job_stack->size );
}

CORE_API b32 job_system_push( JobProcFN* job, void* user_params ) {
    read_write_fence();
    if( global_job_stack->remaining_entries >= JOB_STACK_MAX_ENTRY_COUNT ) {
        return false;
    }

    usize write_index = interlocked_increment( &global_job_stack->write_index );
    interlocked_increment( &global_job_stack->remaining_entries );
    read_write_fence();

    write_index %= JOB_STACK_MAX_ENTRY_COUNT;

    JobEntry entry = { job, user_params };

    global_job_stack->entries[write_index] = entry;

    read_write_fence();
    semaphore_signal( &global_job_stack->wake );
    read_write_fence();
    return true;
}

CORE_API void job_system_wait(void) {
    while( global_job_stack->remaining_entries ) {
        semaphore_wait( &global_job_stack->entry_completed );
    }
}
CORE_API b32 job_system_wait_timed( u32 ms ) {
    while( global_job_stack->remaining_entries ) {
        if( !semaphore_wait_timed( &global_job_stack->entry_completed, ms ) ) {
            return false;
        }
    }

    return true;
}

