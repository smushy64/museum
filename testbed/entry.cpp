// * Description:  Testbed Entry
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <core/ldengine.h>
#include <core/ldlog.h>
#include <core/ldmath.h>
#include <core/ldmemory.h>
#include <core/ldallocator.h>

EXTERNC usize application_query_memory_requirement() {
    return 1;
}

EXTERNC b32 application_init( EngineContext* ctx, void* memory ) {
    engine_set_application_name( "testbed" );
    engine_surface_set_dimensions( (ivec2){ 1280, 720 } );
    engine_surface_center();
    unused(ctx);
    unused(memory);
    return true;
}

EXTERNC b32 application_run( EngineContext* ctx, void* memory ) {
    unused(ctx);
    unused(memory);
    return true;
}

