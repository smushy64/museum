// * Description:  Testbed Entry
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <core/ldengine.h>
#include <core/ldlog.h>
#include <core/ldmath.h>
#include <core/ldmemory.h>
#include <core/ldallocator.h>

const char* test_message = "Hello World! This is a test message let's goooo!";
#define MAX_BLOCKS (20)
#define DYN_SIZE (100)

EXTERNC usize application_query_memory_requirement() {
    usize memory_requirement =
        memory_state_required_buffer_size( MAX_BLOCKS );
    memory_requirement += DYN_SIZE;
    return memory_requirement;
}

EXTERNC b32 application_init( EngineContext* ctx, void* memory ) {
    engine_set_application_name( "testbed" );
    engine_surface_set_dimensions( (ivec2){ 1280, 720 } );
    engine_surface_center();

    usize memory_state_buffer_size =
        memory_state_required_buffer_size( MAX_BLOCKS );
    DynamicAllocator dyn = dynamic_allocator_from_buffer(
        memory_state_buffer_size, memory,
        DYN_SIZE, (u8*)memory + memory_state_buffer_size
    );

    usize test_size = 10;
    void* test = dynamic_allocator_allocate( &dyn, test_size );

    StringView test_message_sv = SV(test_message);
    char* buf = (char*)dynamic_allocator_allocate(
        &dyn, test_message_sv.len + 1 );

    if( !buf ) {
        LOG_ERROR( "Failed to allocate test buffer!" );
        return false;
    }

    dynamic_allocator_free( &dyn, test, test_size );

    mem_copy( buf, test_message_sv.buffer, test_message_sv.len + 1 );

    println( "{cc}", buf );

    dynamic_allocator_free( &dyn, buf, test_message_sv.len + 1 );

    engine_exit();

    unused(ctx);
    unused(memory);
    return true;
}

EXTERNC b32 application_run( EngineContext* ctx, void* memory ) {
    unused(ctx);
    unused(memory);
    return true;
}

