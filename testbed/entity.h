#if !defined(TESTBED_ENTITY_HPP)
#define TESTBED_ENTITY_HPP
// * Description:  Entities
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 31, 2023
#include <defines.h>
#include <core/ecs.h>
#include <core/math/rand.h>

#define ENTITY_TYPE_SHIP     1
#define ENTITY_TYPE_ASTEROID 2
#define ENTITY_TYPE_TORPEDO  3

inline const char* entity_type_to_string( EntityType type ) {
    switch( type ) {
        case ENTITY_TYPE_SHIP:     return "Ship";
        case ENTITY_TYPE_ASTEROID: return "Asteroid";
        case ENTITY_TYPE_TORPEDO:  return "Torpedo";
        default: return "null";
    }
}

// NOTE(alicia): ship

#define SHIP_NORMAL_SPEED   (1.5f)
#define SHIP_ROTATION_SPEED (5.5f)
#define SHIP_SCALE          (0.05f)
#define SHIP_NORMAL_DRAG    (1.2f)
#define SHIP_STOP_DRAG      (2.5f)
struct Ship {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
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
struct Asteroid {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    u32            life;
};
STATIC_ASSERT( sizeof(Asteroid) <= MAX_ENTITY_SIZE );
Entity asteroid_create(
    vec2 position, u32 life,
    struct Texture* texture_atlas,
    RandXOR& rand
);
b32 asteroid_damage( Entity* entity, RandXOR& rand );

#endif // header guard
