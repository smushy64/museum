// * Description:  Testbed Entry Point Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 21, 2023
#include "entry.h"
#include <core/engine.h>
#include <core/collections.h>
#include <core/event.h>
#include <core/input.h>
#include <core/time.h>
#include <core/ecs.h>
#include <core/math/types.h>
#include <core/math/type_functions.h>
#include <renderer/renderer.h>

const char* entity_type_to_string( EntityType type ) {
    switch( type ) {
        case ENTITY_TYPE_SHIP: return "Ship";
        case ENTITY_TYPE_ASTEROID: return "Asteroid";
        default: return "null";
    }
}

struct EntityPhysics {
    struct Transform {
        vec2 position;
        f32  rotation;
    } transform;
    struct Physics {
        vec2 velocity;
        f32  angular_velocity;
    } physics;
};
void system_physics_solver(
    EntityStorage* storage, EntityStorageQueryResult* query_result,
    f32 delta_time, ivec2 surface_dimensions
);

// TODO(alicia): TEST CODE
global DrawBinding* list_draw_bindings = {};

b32 entry(
    struct EngineContext* engine_ctx,
    void*  user_params
) {
    unused(user_params);

    if( !list_draw_bindings ) {
        list_draw_bindings = list_reserve( DrawBinding, 2 );
        DrawBinding null_binding = {};
        list_push( list_draw_bindings, null_binding );
    }

    EntityStorage* storage = engine_get_entity_storage( engine_ctx );
    Time* time = engine_get_time( engine_ctx );

    if( input_is_key_down( KEY_ESCAPE ) ) {
        Event event = {};
        event.code  = EVENT_CODE_EXIT;
        event_fire( event );
    }

    local b32 created_ship = false;
    local b32 created_asteroid = false;
    local i32 ship_id = -1;
    if( !created_ship ) {
        Entity ship = {};
        ship.type  = ENTITY_TYPE_SHIP;
        ship.flags = ENTITY_FLAG_HAS_PHYSICS_ASTEROIDS |
            ENTITY_FLAG_HAS_TRANSFORM_ASTEROIDS;
        ship_id = entity_storage_create_entity( storage, &ship );
        created_ship = true;
    }
    if( !created_asteroid ) {
        Entity asteroid = {};
        asteroid.type  = ENTITY_TYPE_ASTEROID;
        asteroid.flags = ENTITY_FLAG_HAS_PHYSICS_ASTEROIDS |
            ENTITY_FLAG_HAS_TRANSFORM_ASTEROIDS;
        asteroid.data.asteroid.physics.velocity = VEC2::RIGHT + VEC2::UP;
        asteroid.data.asteroid.physics.angular_velocity = 0.1f;
        entity_storage_create_entity( storage, &asteroid );
        created_asteroid = true;
    }

    Entity::EntityData::ShipData* ship =
        &storage->entities[ship_id].data.ship;
    if( ship_id >= 0 ) {
        vec2 input_direction = {};
        input_direction.x = (f32)input_is_key_down( KEY_ARROW_RIGHT ) -
            (f32)input_is_key_down( KEY_ARROW_LEFT );
        input_direction.y = (f32)input_is_key_down( KEY_ARROW_UP ) -
            (f32)input_is_key_down( KEY_ARROW_DOWN );


        vec2 forward_direction = rotate(
            VEC2::UP, ship->transform.rotation
        );

        ship->physics.velocity +=
            forward_direction * input_direction.y * time->delta_seconds;

        ship->physics.angular_velocity +=
            input_direction.x * time->delta_seconds;

    }

    EntityStorageQueryResult physics_objects = entity_storage_query(
        storage,
        ENTITY_FLAG_HAS_TRANSFORM_ASTEROIDS |
        ENTITY_FLAG_HAS_PHYSICS_ASTEROIDS
    );
    system_physics_solver(
        storage,
        &physics_objects,
        time->delta_seconds,
        engine_query_surface_size( engine_ctx )
    );

    list_clear( list_draw_bindings );

    mat4 ship_transform =
        translate( ship->transform.position ) *
        rotate( ship->transform.rotation ) *
        scale( 0.1f, 0.1f );
    RenderOrder* render_order = engine_get_render_order( engine_ctx );

    DrawBinding ship_binding = {};

    ship_binding.transform   = ship_transform;
    ship_binding.mesh_index  = 0;

    list_push( list_draw_bindings, ship_binding );

    render_order->draw_bindings      = list_draw_bindings;
    render_order->draw_binding_count = list_count( list_draw_bindings );

    return true;
}

void system_physics_solver(
    EntityStorage* storage, EntityStorageQueryResult* query_result,
    f32 delta_time, ivec2 dimensions
) {
    f32 aspect_ratio = (f32)dimensions.x / (f32)dimensions.y;
    f32 wrap_padding = 0.1f;

    for( u32 i = 0; i < query_result->index_count; ++i ) {
        Entity* entity = &storage->entities[query_result->indices[i]];
        EntityPhysics* e = (EntityPhysics*)&entity->data;
        e->transform.position += e->physics.velocity * delta_time;
        e->transform.rotation += e->physics.angular_velocity * delta_time;

        f32 abs_pos_x = absolute(e->transform.position.x);
        f32 abs_pos_y = absolute(e->transform.position.y);

        // screen wrapping
        if( abs_pos_x >= aspect_ratio + wrap_padding ) {
            e->transform.position.x *= -1.0f;
        }
        if( abs_pos_y >= 1.0f + wrap_padding ) {
            e->transform.position.y *= -1.0f;
        }

        e->physics.velocity *= 0.999f;
        e->physics.angular_velocity *= 0.999f;
    }
}

