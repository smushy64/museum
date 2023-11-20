/**
 * Description:  Shared buffer implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 15, 2023
*/
#include "defines.h"
#include "core/memory.h"
#include "core/thread.h"

#include "logging.h"
#include "shared_buffer.h"

global BlockAllocator* global_allocator = NULL;
global usize  global_allocator_size      = 0;
global Mutex* global_allocator_mutex     = NULL;

b32 package_shared_buffer_initialize(void) {
    global_allocator_mutex = mutex_create();
    if( !global_allocator_mutex ) {
        lp_error( "failed to create global allocator mutex!" );
        return false;
    }

    global_allocator_size = block_allocator_memory_requirement(
        PACKAGE_SHARED_BUFFER_FREE_LIST_COUNT, PACKAGE_SHARED_BUFFER_GRANULARITY );
    global_allocator = system_alloc( global_allocator_size );
    if( !global_allocator ) {
        lp_error(
            "failed to allocate {f,.2,b} for global allocator!",
            (f64)global_allocator_size );
        return false;
    }

    global_allocator = block_allocator_create(
        PACKAGE_SHARED_BUFFER_FREE_LIST_COUNT,
        PACKAGE_SHARED_BUFFER_GRANULARITY, global_allocator );
    return true;
}
void package_shared_buffer_shutdown(void) {
    mutex_lock( global_allocator_mutex );
    read_write_fence();

    system_free( global_allocator, global_allocator_size );

    read_write_fence();
    mutex_unlock( global_allocator_mutex );
    mutex_destroy( global_allocator_mutex );
}
void* package_shared_buffer_allocate( usize size ) {
    if( !global_allocator ) {
        return NULL;
    }

    read_write_fence();
    mutex_lock( global_allocator_mutex );
    read_write_fence();

    void* result = block_allocator_alloc( global_allocator, size );

    read_write_fence();
    mutex_unlock( global_allocator_mutex );

    return result;
}
void package_shared_buffer_free( void* memory, usize size ) {
    if( !global_allocator ) {
        return;
    }

    read_write_fence();
    mutex_lock( global_allocator_mutex );
    read_write_fence();

    block_allocator_free( global_allocator, memory, size );

    read_write_fence();
    mutex_unlock( global_allocator_mutex );
}


