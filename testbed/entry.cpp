// * Description:  Testbed Entry
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <core/ldengine.h>
#include <core/ldlog.h>
#include <core/ldmath.h>
#include <core/ldmemory.h>
#include <core/ldallocator.h>
#include <core/ldthread.h>

void thread_work( ThreadInfo* thread_info, void* params ) {
    usize thread_index = thread_info_query_index( thread_info );
    LOG_INFO( "Hello from thread {u64}", (u64)thread_index );
    unused( params );
}

extern_c usize application_query_memory_requirement() {
    return 1;
}
extern_c b32 application_init( EngineContext* ctx, void* memory ) {
    engine_application_set_name( ctx, SV("testbed") );
    engine_surface_set_dimensions( (ivec2){ 1280, 720 } );
    engine_surface_center();

    for( usize i = 0; i < 30; ++i ) {
        thread_work_queue_push( thread_work, nullptr );
    }

    unused(ctx);
    unused(memory);
    return true;
}
extern_c b32 application_run( EngineContext* ctx, void* memory ) {
    unused(ctx);
    unused(memory);
    return true;
}

