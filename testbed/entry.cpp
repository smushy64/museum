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
#include <core/ldgraphics/ui.h>
#include <core/ldinput.h>

struct GameMemory {
    Transform camera_transform;
    Camera    camera;
};

extern_c usize application_query_memory_requirement() {
    return sizeof(GameMemory);
}
extern_c b32 application_init( EngineContext* ctx, void* opaque ) {

    StringView name;
#if defined(LD_PLATFORM_WINDOWS)
    name = SV("testbed-win32");
#elif defined(LD_PLATFORM_LINUX)
    name = SV("testbed-linux");
#else
    name = SV("testbed-unknown");
#endif

    engine_application_set_name( ctx, name );
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

    Timer time = engine_time( ctx );
    unused(time);

    if( input_key_press( KEY_ESCAPE ) ) {
        engine_exit();
    }

    if( input_key_press( KEY_SPACE ) ) {
        engine_surface_center( ctx );
    }

    if( input_key_press( KEY_ENTER ) ) {
        LOG_DEBUG( "Delta time: {f}", time.delta_seconds );
    }

    ui_draw_quad(
        NULL,
        v2_scalar( 0.5f ),
        v2_scalar( 0.1f ),
        UI_ANCHOR_X_LEFT, UI_ANCHOR_Y_BOTTOM,
        { 1.0f, 0.0f, 0.0f, 0.5f }
    );
    ui_draw_quad(
        NULL,
        v2_scalar( 0.5f ),
        v2_scalar( 0.1f ),
        UI_ANCHOR_X_CENTER, UI_ANCHOR_Y_CENTER,
        { 0.0f, 1.0f, 0.0f, 0.5f }
    );
    ui_draw_quad(
        NULL,
        v2_scalar( 0.5f ),
        v2_scalar( 0.1f ),
        UI_ANCHOR_X_RIGHT, UI_ANCHOR_Y_TOP,
        { 0.0f, 0.0f, 1.0f, 0.5f }
    );
    return true;
}

