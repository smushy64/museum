// * Description:  Testbed Entry Point Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 21, 2023
#include "corecpp.inl"
#include <defines.h>
#include <core/logging.h>
#include <core/engine.h>
#include <core/ecs.h>
#include <renderer/renderer.h>
#include <core/graphics.h>
#include <core/collections.h>
#include <core/event.h>
#include <core/input.h>
#include <core/math.h>
#include <core/time.h>
#include <core/asset.h>
#include <core/memory.h>

#define ENTITY_TYPE_SHIP     1
#define ENTITY_TYPE_ASTEROID 2

const char* entity_type_to_string( EntityType type ) {
    switch( type ) {
        case ENTITY_TYPE_SHIP: return "Ship";
        case ENTITY_TYPE_ASTEROID: return "Asteroid";
        default: return "null";
    }
}

#define SHIP_SCALE (0.05f)
struct Ship {
    Transform2D transform;
    Physics2D   physics;
    f32         normal_drag;
    f32         stop_drag;
};
STATIC_ASSERT( sizeof(Ship) <= MAX_ENTITY_SIZE );
Entity ship_create() {
    Entity entity    = {};
    entity.type      = ENTITY_TYPE_SHIP;
    entity.is_2d     = true;
    entity.is_active = true;
    entity.flags     =
        ENTITY_FLAG_PHYSICS |
        ENTITY_FLAG_TRANSFORM;

    Ship* ship = (Ship*)entity.bytes;

    ship->normal_drag          = 1.2f;
    ship->stop_drag            = 2.5f;
    ship->physics.drag         = ship->normal_drag;
    ship->physics.angular_drag = ship->normal_drag;

    return entity;
}

struct Asteroid {
    Transform2D transform;
    Physics2D   physics;
};
STATIC_ASSERT( sizeof(Asteroid) <= MAX_ENTITY_SIZE );
Entity asteroid_create() {
    Entity result    = {};
    result.type      = ENTITY_TYPE_ASTEROID;
    result.is_2d     = true;
    result.is_active = false;
    result.flags     =
        ENTITY_FLAG_PHYSICS |
        ENTITY_FLAG_TRANSFORM;

    return result;
}

struct GameMemory {
    Texture      test_texture;
    DrawBinding* list_draw_bindings;
    i32 ship_id;
    EventListenerID on_exit_listener;
};

EventCallbackReturn on_exit( Event*, void* generic_memory ) {
    GameMemory* memory = (GameMemory*)generic_memory;
    mem_free( memory->test_texture.buffer );

    event_unsubscribe( memory->on_exit_listener );

    return EVENT_CALLBACK_NOT_CONSUMED;
}

extern "C"
void application_config( struct EngineConfig* config ) {

    string_format(
        config->application_name,
        "Test Bed {i}.{i}{c}",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR,
        0
    );

    // TODO(alicia): figure this shit out
    // config->opt_application_icon_path = "./resources/images/ui/testbed_icon_256x256.ico";
    config->surface_dimensions        = { 800, 600 };
    config->log_level        = LOG_LEVEL_ALL_VERBOSE;
    // TODO(alicia): expose these flags somewhere 
    config->platform_flags   = (1 << 0) | (1 << 1);
    config->memory_size = sizeof(GameMemory);
}

extern "C"
b32 application_init( struct EngineContext* ctx, void* generic_memory ) {
    GameMemory* memory = (GameMemory*)generic_memory;
    memory->list_draw_bindings = list_reserve( DrawBinding, 2 );
    if( !memory->list_draw_bindings ) {
        return false;
    }

    DrawBinding null_binding = {};
    list_push( memory->list_draw_bindings, null_binding );

    EntityStorage* storage = engine_get_entity_storage( ctx );

    Entity ship = ship_create();
    memory->ship_id = entity_storage_create_entity( storage, &ship );

    if( memory->ship_id < 0 ) {
        return false;
    }

    Entity asteroid = asteroid_create();
    entity_storage_create_entity( storage, &asteroid );

    DebugImage debug_image = {};
    if( !debug_load_bmp( "./resources/ship.bmp", &debug_image ) ) {
        return false;
    }

    memory->test_texture.dimensions = debug_image.dimensions;
    memory->test_texture.format     = debug_image.format;
    memory->test_texture.buffer     = debug_image.buffer;

    memory->on_exit_listener =
        event_subscribe( EVENT_CODE_EXIT, on_exit, memory );

    return true;
}

extern "C"
b32 application_run( struct EngineContext* ctx, void* generic_memory ) {
    GameMemory* memory = (GameMemory*)generic_memory;
    EntityStorage* storage = engine_get_entity_storage( ctx );
    Time* time = engine_get_time( ctx );

    if( input_is_key_down( KEY_ESCAPE ) ) {
        Event event = {};
        event.code  = EVENT_CODE_EXIT;
        event_fire( event );
    }

    Entity* entity_ship = &storage->entities[memory->ship_id];
    Ship*   ship = (Ship*)entity_ship->bytes;

    vec2 input_direction = {};
    input_direction.x = (f32)input_is_key_down( KEY_ARROW_RIGHT ) -
        (f32)input_is_key_down( KEY_ARROW_LEFT );
    input_direction.y = (f32)input_is_key_down( KEY_ARROW_UP );

    ship->physics.drag = input_is_key_down( KEY_ARROW_DOWN ) ?
        ship->stop_drag : ship->normal_drag;
    ship->physics.angular_drag = ship->physics.drag;

    vec2 forward_direction = rotate(
        VEC2::UP, ship->transform.rotation
    );

    #define MOVEMENT_SPEED 1.5f
    #define ROTATION_SPEED 5.5f

    ship->physics.velocity +=
        forward_direction *
        input_direction.y *
        time->delta_seconds *
        MOVEMENT_SPEED;

    ship->physics.angular_velocity +=
        input_direction.x *
        time->delta_seconds *
        ROTATION_SPEED;

    EntityStorageQueryResult physics_objects = system_physics_solver2d(
        storage, time->delta_seconds
    );

    ivec2 dimensions = engine_query_surface_size( ctx );
    f32 aspect_ratio = (f32)dimensions.x / (f32)dimensions.y;
    f32 wrap_padding = SHIP_SCALE;

    for( u32 i = 0; i < physics_objects.index_count; ++i ) {
        Entity* entity = &storage->entities[i];

        f32 abs_pos_x = absolute(entity->transform2d.position.x);
        f32 abs_pos_y = absolute(entity->transform2d.position.y);

        // screen wrapping
        if( abs_pos_x >= aspect_ratio + wrap_padding ) {
            entity->transform2d.position.x *= -1.0f;
        }
        if( abs_pos_y >= 1.0f + wrap_padding ) {
            entity->transform2d.position.y *= -1.0f;
        }
    }

    list_clear( memory->list_draw_bindings );

    mat4 ship_transform =
        translate( ship->transform.position ) *
        rotate( ship->transform.rotation ) *
        scale( SHIP_SCALE, SHIP_SCALE );
    RenderOrder* render_order = engine_get_render_order( ctx );
    render_order->textures = &memory->test_texture;
    render_order->texture_count = 1;

    DrawBinding ship_binding = {};

    ship_binding.transform     = ship_transform;
    ship_binding.mesh_index    = 0;
    ship_binding.texture_index = 0;

    list_push( memory->list_draw_bindings, ship_binding );

    render_order->draw_bindings      = memory->list_draw_bindings;
    render_order->draw_binding_count = list_count( memory->list_draw_bindings );

    return true;
}

