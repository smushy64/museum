#if !defined(CORE_ECS_HPP)
#define CORE_ECS_HPP
// * Description:  Entity-Component System
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 03, 2023
#include "defines.h"
#include "math/types.h"

enum EntityType : u32 {
    ENTITY_TYPE_NULL,
    ENTITY_TYPE_SHIP,
    ENTITY_TYPE_ASTEROID,

    ENTITY_TYPE_COUNT,
};

typedef u32 EntityFlags;

#define ENTITY_FLAG_HAS_TRANSFORM_ASTEROIDS (1 << 0)
#define ENTITY_FLAG_HAS_PHYSICS_ASTEROIDS   (1 << 1)

struct AsteroidsTransform {
    vec2 position;
    f32  rotation;
};

struct AsteroidsPhysicsComponent {
    vec2 velocity;
    f32  angular_velocity;
};

#define MAX_ENTITIES 20

struct Entity {
    EntityType  type;
    EntityFlags flags;
    union EntityData {
        struct ShipData {
            AsteroidsTransform        transform;
            AsteroidsPhysicsComponent physics;
        } ship;
        struct AsteroidData {
            AsteroidsTransform        transform;
            AsteroidsPhysicsComponent physics;
        } asteroid;
    } data;
};

struct EntityStorageQueryResult {
    u32 index_count;
    u32 indices[MAX_ENTITIES];
};

struct EntityStorage {
    Entity entities[MAX_ENTITIES];
};

/// Create a new entity in the entity storage.
/// Searches through entities until it finds a null entity
/// and copies the entity data provided to that destination.
/// Returns -1 if no null entity was found.
/// Returns entity id if successful.
LD_API i32 entity_storage_create_entity(
    EntityStorage* storage,
    Entity* entity
);

/// Mark an entity as being null.
LD_API void entity_storage_mark_null( EntityStorage* storage, u32 index );

/// Query for entities whose flags match given flags.
LD_API EntityStorageQueryResult entity_storage_query(
    EntityStorage* storage,
    EntityFlags flags
);

#endif // header guard
