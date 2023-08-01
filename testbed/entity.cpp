// * Description:  Entities Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 31, 2023
#include "entity.h"
#include <core/math.h>
#include <core/ecs.h>
#include <core/logging.h>

Entity ship_create( struct Texture* texture_atlas ) {
    Entity entity    = {};
    entity.type      = ENTITY_TYPE_SHIP;
    entity.state_flags = ENTITY_STATE_FLAG_IS_ACTIVE |
        ENTITY_STATE_FLAG_IS_2D |
        ENTITY_STATE_FLAG_IS_VISIBLE;
    entity.component_flags     =
        ENTITY_COMPONENT_FLAG_PHYSICS   |
        ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER |
        ENTITY_COMPONENT_FLAG_COLLIDER_2D;

    Ship* ship = (Ship*)entity.bytes;

    ship->transform.scale = { SHIP_SCALE, SHIP_SCALE };
    ship->physics.drag         = SHIP_NORMAL_DRAG;
    ship->physics.angular_drag = ship->physics.drag;
    ship->sprite_renderer      = sprite_renderer_new( texture_atlas, 1 );
    ship->collider             = collider2d_new_rect(
        SHIP_SCALE * 2.0f,
        SHIP_SCALE * 2.0f
    );

    return entity;

}

Entity torpedo_create( struct Texture* texture_atlas ) {
    Entity entity    = {};
    entity.type      = ENTITY_TYPE_TORPEDO;
    entity.state_flags =
        ENTITY_STATE_FLAG_IS_2D |
        ENTITY_STATE_FLAG_IS_VISIBLE;
    entity.component_flags     =
        ENTITY_COMPONENT_FLAG_PHYSICS   |
        ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER |
        ENTITY_COMPONENT_FLAG_COLLIDER_2D;

    Torpedo* torpedo = (Torpedo*)entity.bytes;

    torpedo->transform.scale = v2( TORPEDO_SCALE );
    torpedo->sprite_renderer = sprite_renderer_new( texture_atlas, 1 );
    torpedo->sprite_renderer.z_index = -1;
    torpedo->collider        = collider2d_new_rect(
        TORPEDO_SCALE * 2.0f,
        TORPEDO_SCALE * 2.0f
    );
    torpedo->life_timer = 0.0f;

    return entity;
}

void torpedo_enable( Entity* entity, Ship* ship, vec2 ship_forward ) {
    Torpedo* current_torpedo = (Torpedo*)entity->bytes;
    current_torpedo->transform.position = ship->transform.position;
    current_torpedo->physics.velocity   = ship_forward * TORPEDO_NORMAL_SPEED;
    current_torpedo->life_timer = 0.0f;
    entity_set_active( entity, true );
}

inline internal void asteroid_set_life( Entity* entity, u32 life, RandXOR& rand ) {
    ASSERT( entity->type == ENTITY_TYPE_ASTEROID );
    ASSERT( life <= ASTEROID_MAX_LIFE );

    Asteroid* asteroid = (Asteroid*)entity->bytes;

    asteroid->life = life;

    asteroid->sprite_renderer.atlas_coordinate_position.x =
        rand.next_u32() % 3;
    asteroid->sprite_renderer.atlas_coordinate_position.y =
        ( rand.next_u32() % 2 ) + 1;

    local const f32 ASTEROID_SCALE[] = {
        0, 0.4f, 0.6f, 1.0f
    };
    const vec2 ASTEROID_BASE_SCALE = VEC2::ONE * 0.135f;

    asteroid->transform.scale = ASTEROID_BASE_SCALE * ASTEROID_SCALE[life];
    #define ASTEROID_COLLIDER_SCALE (1.4f)
    asteroid->collider = collider2d_new_rect(
        asteroid->transform.scale.x * ASTEROID_COLLIDER_SCALE,
        asteroid->transform.scale.y * ASTEROID_COLLIDER_SCALE
    );

    vec2 velocity = { rand.next_f32(), rand.next_f32() };
    velocity = normalize( velocity );

    asteroid->physics.velocity         = velocity;
    asteroid->physics.angular_velocity = rand.next_f32();
}

Entity asteroid_create(
    vec2 position, u32 life,
    struct Texture* texture_atlas,
    RandXOR& rand
) {

    Entity    result   = {};
    Asteroid* asteroid = (Asteroid*)result.bytes;

    result.type = ENTITY_TYPE_ASTEROID;
    result.state_flags = ENTITY_STATE_FLAG_IS_ACTIVE |
        ENTITY_STATE_FLAG_IS_VISIBLE |
        ENTITY_STATE_FLAG_IS_2D;
    result.component_flags = ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_COLLIDER_2D |
        ENTITY_COMPONENT_FLAG_PHYSICS |
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER;

    asteroid->transform.position = position;
    asteroid->sprite_renderer    = sprite_renderer_new( texture_atlas, 3u );

    asteroid_set_life( &result, life, rand );

    return result;
}

/// returns true if asteroid is still active
b32 asteroid_damage( Entity* entity, RandXOR& rand ) {
    ASSERT( entity->type == ENTITY_TYPE_ASTEROID );
    Asteroid* asteroid = (Asteroid*)entity->bytes;

    u32 life = asteroid->life;
    ASSERT( life );

    life--;
    ASSERT( life <= ASTEROID_MAX_LIFE );
    if( !life ) {
        entity->type = ENTITY_TYPE_NULL;
        return false;
    }

    asteroid_set_life( entity, life, rand );

    return true;
}

