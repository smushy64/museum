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
#include <core/ldgraphics.h>
#include <core/ldgraphics/types.h>
#include <core/ldinput.h>

struct GameMemory {
    Transform camera_transform;
    Camera    camera;
};

extern_c usize application_query_memory_requirement() {
    return sizeof(GameMemory);
}
extern_c b32 application_init( EngineContext* ctx, void* opaque ) {
    engine_application_set_name( ctx, SV("testbed") );
    engine_surface_center( ctx );

    GameMemory* memory = (GameMemory*)opaque;

    memory->camera_transform = transform_zero();
    memory->camera.transform = &memory->camera_transform;

    memory->camera.transform->matrix_dirty = true;

    unused(ctx);
    unused(memory);

    return true;
}
extern_c b32 application_run( EngineContext* ctx maybe_unused, void* opaque ) {
    GameMemory* maybe_unused memory = (GameMemory*)opaque;

    if( input_key_press( KEY_ESCAPE ) ) {
        engine_exit();
    }

    if( input_key_press( KEY_SPACE ) ) {
        engine_surface_center( ctx );
    }
    return true;
}

