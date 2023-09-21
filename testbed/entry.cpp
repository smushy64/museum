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
#include <core/collections.h>

struct GameMemory {
    Transform camera_transform;
    Camera    camera;
    hsv       color;
    vec3      camera_rotation;

    Transform cube0;
    Transform cube1;
    Transform triangle_transform;

    mat4 floor;

    vec3 cube_rotation;

    RenderID triangle;
    RenderID triangle_diffuse;
};

struct Vertex3D triangle_vertices[] = {
    { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, { VEC3_FORWARD }, { RGBA_WHITE }, { VEC3_RIGHT } },
    { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, { VEC3_FORWARD }, { RGBA_WHITE }, { VEC3_RIGHT } },
    { {  0.0f,  0.5f, 0.0f }, { 0.0f, 1.0f }, { VEC3_FORWARD }, { RGBA_WHITE }, { VEC3_RIGHT } },
};

u32 triangle_indices[] = {
    0, 1, 2
};

u8 triangle_diffuse[] = {
    255, 255, 255
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
            QUAT_IDENTITY,
            VEC3_ONE
        );
    memory->camera.transform   = &memory->camera_transform;
    memory->camera.near_clip   = 0.01f;
    memory->camera.far_clip    = 1000.0f;
    memory->camera.fov_radians = to_rad32( 90.0f );

    memory->camera_rotation = {};

    memory->color = v3_hsv( 0.0f, 1.0f, 1.0f );

    memory->floor = m4_transform( VEC3_DOWN, QUAT_IDENTITY, v3( 100.0f, 1.0f, 100.0f ) );
    memory->cube0 = transform_create( v3( 0.0f, 1.2f, 0.0f ), QUAT_IDENTITY, VEC3_ONE );
    memory->cube1 = transform_create( v3( 0.0f, 0.75f, 0.0f ), QUAT_IDENTITY, v3_mul( VEC3_ONE, 0.5f ) );
    memory->cube1.parent = &memory->cube0;

    engine_set_camera( ctx, &memory->camera );

    memory->triangle = graphics_generate_mesh(
        static_array_count(triangle_vertices), triangle_vertices,
        static_array_count(triangle_indices), triangle_indices );
    memory->triangle_diffuse = graphics_generate_texture_2d(
        GRAPHICS_TEXTURE_FORMAT_RGB,
        GRAPHICS_TEXTURE_BASE_TYPE_UINT8,
        GRAPHICS_TEXTURE_WRAP_CLAMP,
        GRAPHICS_TEXTURE_WRAP_CLAMP,
        GRAPHICS_TEXTURE_FILTER_NEAREST,
        GRAPHICS_TEXTURE_FILTER_NEAREST,
        1, 1, static_array_size( triangle_diffuse ),
        triangle_diffuse
    );

    memory->triangle_transform =
        transform_create( VEC3_ZERO, QUAT_IDENTITY, VEC3_ONE );

    graphics_set_directional_light( v3(-1.0f, -1.0f, -1.0f), RGB_GRAY );
    graphics_set_point_light( 0, VEC3_LEFT * 2.0f + VEC3_UP, RGB_BLUE, true );

    return true;
}
c_linkage b32 application_run(
    maybe_unused EngineContext* ctx, void* opaque
) {
    maybe_unused GameMemory* memory = (GameMemory*)opaque;

    TimeStamp time = engine_time( ctx );
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
        camera_delta.x = ((f32)key_d - (f32)key_a);
        camera_delta.z = -((f32)key_w - (f32)key_s);

        camera_delta =
            v3_mul( camera_delta, time.delta_seconds * move_speed );
        camera_delta =
            q_mul_v3(
                transform_local_rotation( memory->camera.transform ),
                camera_delta );
        transform_translate( memory->camera.transform, camera_delta );

        f32 camera_delta_y = ((f32)key_space - (f32)key_shift) *
            time.delta_seconds * move_speed;
        transform_translate( memory->camera.transform,
            v3( 0.0f, camera_delta_y, 0.0f ) );
    }

    graphics_draw(
        transform_world_matrix( &memory->cube0 ),
        0, 0, 0, 0, 0,
        RGB_WHITE,
        false, true, false, false );
    graphics_draw(
        transform_world_matrix( &memory->cube1 ),
        0, 0, 0, 0, 0,
        RGB_WHITE,
        false, true, false, false );
    graphics_draw(
        memory->floor,
        0, 0, 0, 0, 0,
        RGB_WHITE,
        false, false, true, false );

    graphics_draw(
        transform_world_matrix( &memory->triangle_transform ),
        memory->triangle,
        memory->triangle_diffuse, 0, 0, 0,
        RGB_WHITE,
        false,
        false, false, false );

    transform_rotate( &memory->cube0, q_angle_axis( time.delta_seconds, v3_normalize(VEC3_RIGHT + VEC3_UP) ) );
    transform_rotate( &memory->cube1, q_angle_axis( time.delta_seconds, VEC3_UP ) );

    return true;
}

