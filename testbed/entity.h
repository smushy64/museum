#if !defined(TESTBED_ENTITY_HPP)
#define TESTBED_ENTITY_HPP
// * Description:  Entities
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 31, 2023
#include <defines.h>
#include <core/ecs.h>
#include <core/math/rand.h>

#define ENTITY_TYPE_SHIP           1
#define ENTITY_TYPE_ASTEROID       2
#define ENTITY_TYPE_TORPEDO        3
#define ENTITY_TYPE_SHIP_DESTROYED 4
#define ENTITY_TYPE_LIFE_UI        5

inline const char* entity_type_to_string( EntityType type ) {
    switch( type ) {
        case ENTITY_TYPE_SHIP:     return "Ship";
        case ENTITY_TYPE_ASTEROID: return "Asteroid";
        case ENTITY_TYPE_TORPEDO:  return "Torpedo";
        case ENTITY_TYPE_SHIP_DESTROYED: return "Ship Destroyed";
        case ENTITY_TYPE_LIFE_UI: return "Life UI";
        default: return "null";
    }
}

// NOTE(alicia): ship

#define SHIP_NORMAL_SPEED   (1.5f)
#define SHIP_ROTATION_SPEED (5.5f)
#define SHIP_SCALE          (0.05f)
#define SHIP_NORMAL_DRAG    (1.2f)
#define SHIP_STOP_DRAG      (2.5f)
#define SHIP_INVINCIBILITY_TIME (2.5f)
#define SHIP_BLINK_TIME (SHIP_INVINCIBILITY_TIME / 10.0f)
struct Ship {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    f32 invincibility_timer;
    f32 blink_timer;
    b32 is_invincible;
};
STATIC_ASSERT( sizeof(Ship) <= MAX_ENTITY_SIZE );
Entity ship_create( struct Texture* texture_atlas );

// NOTE(alicia): torpedo

#define TORPEDO_SCALE            (0.015f)
#define TORPEDO_LIFETIME_SECONDS (2.0f)
#define TORPEDO_NORMAL_SPEED     (SHIP_NORMAL_SPEED + 0.25f)
struct Torpedo {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    f32 life_timer;
};
STATIC_ASSERT( sizeof(Torpedo) <= MAX_ENTITY_SIZE );
Entity torpedo_create( struct Texture* texture_atlas );
void torpedo_enable( Entity* entity, Ship* ship, vec2 ship_forward );

// NOTE(alicia): asteroid

#define ASTEROID_MAX_LIFE (3ul)
#define ASTEROID_MIN_SPEED (0.25f)
#define ASTEROID_MAX_SPEED (1.0f)
struct Asteroid {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    u32            life;
    u32            score;
};
STATIC_ASSERT( sizeof(Asteroid) <= MAX_ENTITY_SIZE );
Entity asteroid_create(
    vec2 position, u32 life,
    struct Texture* texture_atlas,
    RandXOR& rand
);
b32 asteroid_damage( Entity* entity, RandXOR& rand );

// NOTE(alicia): ship destroyed

#define SHIP_DESTROYED_SCALE   (0.05f)

#define SHIP_DESTROYED_MIN_SPEED (0.8f)
#define SHIP_DESTROYED_MAX_SPEED (1.4f)

#define SHIP_DESTROYED_MIN_ANGULAR_SPEED (1.5f)
#define SHIP_DESTROYED_MAX_ANGULAR_SPEED (2.0f)

#define SHIP_DESTROYED_SECONDS (1.5f)
#define SHIP_DESTROYED_DRAG    (1.5f)
#define SHIP_DESTROYED_PIECE_COUNT (3ul)
struct ShipDestroyed {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
};
Entity ship_destroyed_create( struct Texture* texture_atlas );
void ship_destroyed_enable( Entity* entity, vec2 position, RandXOR& rand );

// NOTE(alicia): life ui

struct LifeUI {
    Transform2D transform;
    u8 _padding[sizeof(Physics2D)];
    SpriteRenderer sprite_renderer;
};
Entity life_ui_create( vec2 position, struct Texture* texture_atlas );

#endif // header guard
