// * Description:  Testbed Entry
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <core/engine.h>
#include <core/log.h>
#include <core/mathf.h>
#include <core/mem.h>
#include <core/allocator.h>
#include <core/thread.h>
#include <core/graphics.h>
#include <core/graphics/types.h>
#include <core/graphics/ui.h>
#include <core/input.h>

struct GameMemory {
    Transform camera_transform;
    Camera    camera;
    hsv       color;
};

extern_c usize application_query_memory_requirement() {
    return sizeof(GameMemory);
}
extern_c b32 application_init( EngineContext* ctx, void* opaque ) {

#if defined(LD_PLATFORM_WINDOWS)
    STRING( name, "testbed-win32" );
#elif defined(LD_PLATFORM_LINUX)
    STRING( name, "testbed-linux" );
#else
    STRING( name, "testbed-unknown" );
#endif

    engine_application_set_name( ctx, &name );
    engine_surface_center( ctx );

    GameMemory* memory = (GameMemory*)opaque;

    memory->camera_transform = transform_zero();
    memory->camera.transform = &memory->camera_transform;

    memory->camera.transform->matrix_dirty = true;

    memory->color = v3_hsv( 0.0f, 1.0f, 1.0f );

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

    rgba color_rgba = rgba_rgb( hsv_to_rgb( memory->color ) );
    color_rgba.a = 0.5f;

    f32 x = 0.5f, y = 0.5f;
    f32 scale = 100.0f;
    ui_draw_quad(
        NULL,
        UI_COORDINATE_NORMALIZED,
        UI_COORDINATE_PIXEL,
        v2( x, y ),
        v2_scalar( scale ),
        UI_ANCHOR_X_LEFT, UI_ANCHOR_Y_BOTTOM,
        color_rgba
    );
    ui_draw_quad(
        NULL,
        UI_COORDINATE_NORMALIZED,
        UI_COORDINATE_PIXEL,
        v2( x, y ),
        v2_scalar( scale ),
        UI_ANCHOR_X_CENTER, UI_ANCHOR_Y_CENTER,
        color_rgba
    );
    ui_draw_quad(
        NULL,
        UI_COORDINATE_NORMALIZED,
        UI_COORDINATE_PIXEL,
        v2( x, y ),
        v2_scalar( scale ),
        UI_ANCHOR_X_RIGHT, UI_ANCHOR_Y_TOP,
        color_rgba
    );

    memory->color.hue =
        wrap_deg32( memory->color.hue + time.delta_seconds * 15.0f );
    return true;
}

