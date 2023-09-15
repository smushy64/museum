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
    vec3      camera_rotation;
};

c_linkage usize application_query_memory_requirement() {
    return sizeof(GameMemory);
}
c_linkage b32 application_init( EngineContext* ctx, void* opaque ) {

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

    memory->camera_transform =
        transform_create(
            v3_mul( VEC3_FORWARD, 2.0f ),
            q_angle_axis( to_rad32(180.0f), VEC3_UP ),
            VEC3_ONE
        );
    memory->camera.transform   = &memory->camera_transform;
    memory->camera.near_clip   = 0.00001f;
    memory->camera.far_clip    = 1000.0f;
    memory->camera.fov_radians = to_rad32( 60.0f );

    memory->camera_rotation = {};

    memory->color = v3_hsv( 0.0f, 1.0f, 1.0f );

    engine_set_camera( ctx, &memory->camera );

    return true;
}
c_linkage b32 application_run(
    EngineContext* ctx maybe_unused, void* opaque
) {
    GameMemory* maybe_unused memory = (GameMemory*)opaque;

    Timer time = engine_time( ctx );
    unused(time);

    if( input_key_press( KEY_ESCAPE ) ) {
        engine_exit();
    }

    if( input_key_press( KEY_SPACE ) ) {
        engine_surface_center( ctx );
    }

    f32 move_speed   = 1.25f;
    f32 rotate_speed = 1.25f;

    b32 arrow_left  = input_is_key_down( KEY_ARROW_LEFT );
    b32 arrow_right = input_is_key_down( KEY_ARROW_RIGHT );
    b32 arrow_up    = input_is_key_down( KEY_ARROW_UP );
    b32 arrow_down  = input_is_key_down( KEY_ARROW_DOWN );
    if( arrow_left || arrow_right || arrow_up || arrow_down ) {
        f32 yrot = -((f32)arrow_right - (f32)arrow_left);
        f32 xrot = (f32)arrow_up - (f32)arrow_down;

        vec3 right      = transform_local_right( memory->camera.transform );
        quat x_rotation =
            q_angle_axis( xrot * time.delta_seconds * rotate_speed, right );

        transform_rotate( memory->camera.transform, x_rotation );

        quat y_rotation =
            q_angle_axis( yrot * time.delta_seconds * rotate_speed, VEC3_UP );
        transform_rotate( memory->camera.transform, y_rotation );
    }

    b32 key_a = input_is_key_down( KEY_A );
    b32 key_d = input_is_key_down( KEY_D );
    b32 key_w = input_is_key_down( KEY_W );
    b32 key_s = input_is_key_down( KEY_S );
    b32 key_shift = input_is_key_down( KEY_SHIFT_LEFT );
    b32 key_space = input_is_key_down( KEY_SPACE );
    if( key_a || key_d || key_w || key_s || key_shift || key_space ) {
        vec3 camera_delta = {};
        camera_delta.x = -((f32)key_d - (f32)key_a);
        camera_delta.z = (f32)key_w - (f32)key_s;

        camera_delta =
            v3_mul( camera_delta, time.delta_seconds * move_speed );
        camera_delta =
            q_mul_v3(
                transform_local_rotation( memory->camera.transform ),
                camera_delta );
        transform_translate( memory->camera.transform, camera_delta );

        f32 camera_delta_y = ((f32)key_shift - (f32)key_space) *
            time.delta_seconds * move_speed;
        transform_translate( memory->camera.transform,
            v3( 0.0f, camera_delta_y, 0.0f ) );
    }


    return true;
}

