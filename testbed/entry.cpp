// * Description:  Testbed Entry
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <core/string.h>
#include <core/math.h>
#include <core/graphics.h>
#include <core/input.h>
#include <core/engine.h>
#include <core/memory.h>
#include <core/logging.h>
#include <core/time.h>
#include <core/audio.h>
#include <core/collections.h>

struct GameMemory {
    Transform camera_transform;
    Camera    camera;
    f32 camera_yaw, camera_pitch;

    Transform cube0;
    Transform cube1;
    Transform triangle_transform;

    mat4 floor;

    vec3 cube_rotation;

    RenderID triangle;
    RenderID triangle_diffuse;
};

struct Vertex3D triangle_vertices[] = {
    { { -0.5f, -0.5f, 0.0f }, { VEC3_FORWARD }, { VEC3_RIGHT }, { RGB_RED },   { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, 0.0f }, { VEC3_FORWARD }, { VEC3_RIGHT }, { RGB_GREEN }, { 1.0f, 0.0f } },
    { {  0.0f,  0.5f, 0.0f }, { VEC3_FORWARD }, { VEC3_RIGHT }, { RGB_BLUE },  { 0.5f, 1.0f } },
};

u32 triangle_indices[] = { 0, 1, 2 };
u8  triangle_diffuse[] = { 255, 255, 255 };

c_linkage usize application_query_memory_requirement() {
    return sizeof(GameMemory);
}

c_linkage b32 application_initialize( void* in_memory ) {
    GameMemory* memory = (GameMemory*)in_memory;
    memory->camera_transform =
        transform_create(
            v3_mul( VEC3_FORWARD, 2.0f ),
            QUAT_IDENTITY,
            VEC3_ONE
        );
    memory->camera.transform   = &memory->camera_transform;
    memory->camera.near_clip   = 0.01f;
    memory->camera.far_clip    = 1000.0f;
    memory->camera.fov_radians = to_radians( 90.0f );

    memory->floor = m4_transform(
        VEC3_DOWN, QUAT_IDENTITY, v3( 100.0f, 1.0f, 100.0f ) );
    memory->cube0 = transform_create(
        v3( 0.0f, 1.2f, 0.0f ), QUAT_IDENTITY, VEC3_ONE );
    memory->cube1 = transform_create(
        v3( 0.0f, 0.75f, 0.0f ), QUAT_IDENTITY, v3_mul( VEC3_ONE, 0.5f ) );
    memory->cube1.parent = &memory->cube0;

    graphics_set_camera( &memory->camera );

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
        triangle_diffuse );

    memory->triangle_transform =
        transform_create( VEC3_ZERO, QUAT_IDENTITY, VEC3_ONE );

    graphics_set_directional_light(
        v3_normalize( v3(-1.0f, -1.0f, -1.0f) ), RGB_WHITE, true );
    graphics_set_point_light(
        0, VEC3_LEFT + (2.0f * VEC3_UP), RGB_RED, true );

    return true;
}

c_linkage b32 application_run( void* in_memory ) {
    GameMemory* memory = (GameMemory*)in_memory;
    f64 delta_time = time_delta();

    if( input_key_down( KEY_ESCAPE ) ) {
        engine_exit();
    }

    f32 move_speed   = 1.25f;
    f32 rotate_speed = 0.85f;

    b32 mouse_right = input_mouse_button( MOUSE_BUTTON_RIGHT );
    input_mouse_set_locked( mouse_right );
    if( mouse_right ) {
        f32 pitch_delta = input_mouse_relative_y();
        f32 yaw_delta   = input_mouse_relative_x();


        memory->camera_pitch += pitch_delta * rotate_speed * delta_time;
        memory->camera_yaw   += yaw_delta   * rotate_speed * delta_time;

        #define CAMERA_MAX_PITCH ( to_radians(80.0f) )
        memory->camera_pitch = clamp(
            memory->camera_pitch, -CAMERA_MAX_PITCH, CAMERA_MAX_PITCH );

        quat camera_yaw = q_angle_axis( memory->camera_yaw, VEC3_UP );
        transform_set_rotation( memory->camera.transform, camera_yaw );

        vec3 camera_right = transform_world_right( memory->camera.transform );
        quat camera_pitch = q_angle_axis( memory->camera_pitch, camera_right );

        transform_rotate( memory->camera.transform, camera_pitch );

        b32 key_a = input_key( KEY_A );
        b32 key_d = input_key( KEY_D );
        b32 key_w = input_key( KEY_W );
        b32 key_s = input_key( KEY_S );
        b32 key_shift = input_key( KEY_SHIFT_LEFT );
        b32 key_space = input_key( KEY_SPACE );
        if( key_a || key_d || key_w || key_s || key_shift || key_space ) {
            vec3 camera_delta = {};
            camera_delta.x = ((f32)key_d - (f32)key_a);
            camera_delta.z = -((f32)key_w - (f32)key_s);

            camera_delta =
                v3_mul( camera_delta, delta_time * move_speed );
            camera_delta =
                q_mul_v3(
                    transform_local_rotation( memory->camera.transform ),
                    camera_delta );
            transform_translate( memory->camera.transform, camera_delta );

            f32 camera_delta_y = ((f32)key_space - (f32)key_shift) *
                delta_time * move_speed;
            transform_translate( memory->camera.transform,
                v3( 0.0f, camera_delta_y, 0.0f ) );
        }
    }

    if( input_key_down( KEY_E ) ) {
        audio_debug_play_sound();
    }

    graphics_draw(
        transform_world_matrix( &memory->cube0 ),
        0, 0, 0, 0, 0,
        RGB_WHITE,
        false, true, true, false );
    graphics_draw(
        transform_world_matrix( &memory->cube1 ),
        0, 0, 0, 0, 0,
        RGB_WHITE,
        false, true, true, false );
    graphics_draw(
        transform_world_matrix( &memory->triangle_transform ),
        memory->triangle,
        memory->triangle_diffuse,
        0, 0, 0,
        RGB_WHITE,
        false, true, true, false );
    graphics_draw(
        memory->floor,
        0, 0, 0, 0, 0,
        RGB_WHITE,
        false, false, true, false );

    transform_rotate( &memory->cube0, q_angle_axis( delta_time, v3_normalize(VEC3_RIGHT + VEC3_UP) ) );
    transform_rotate( &memory->cube1, q_angle_axis( delta_time, VEC3_UP ) );

    return true;
}

#include "testbed_generated_dependencies.inl"

