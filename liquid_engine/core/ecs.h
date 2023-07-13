#if !defined(CORE_ECS_HPP)
#define CORE_ECS_HPP
// * Description:  Entity-Component System
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 03, 2023
#include "defines.h"
#include "math/types.h"

typedef u32 EntityType;
#define ENTITY_TYPE_NULL 0

typedef u32 EntityFlags;
#define ENTITY_FLAG_TRANSFORM (1 << 0)
#define ENTITY_FLAG_PHYSICS   (1 << 1)

struct Transform {
    vec3 position;
    quat rotation;
    vec3 scale;
};
struct Transform2D {
    vec2 position;
    vec2 scale;
    f32  rotation;
};

struct Physics {
    vec3 velocity;
    vec3 angular_velocity;
    f32  drag;
    f32  angular_drag;
};
struct Physics2D {
    vec2 velocity;
    f32  angular_velocity;
    f32  drag;
    f32  angular_drag;
};

#define MAX_ENTITIES 20
#define MAX_ENTITY_SIZE 128

struct Entity {
    EntityType  type;
    EntityFlags flags;
    b8 is_active;
    b8 is_2d;
    b8 __unused[2];
    union {
        struct {
            Transform transform;
            Physics   physics;
        };
        struct {
            Transform2D transform2d;
            Physics2D   physics2d;
        };
        u8 bytes[MAX_ENTITY_SIZE];
    };
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
LD_API EntityStorageQueryResult entity_storage_query_flags(
    EntityStorage* storage,
    b32 only_active, b32 only_2d,
    EntityFlags flags
);

/// Query for entities whose type match given type.
LD_API EntityStorageQueryResult entity_storage_query_type(
    EntityStorage* storage,
    b32 only_active, b32 only_2d,
    EntityType     type
);

/// Query for entities whose type and flags match.
LD_API EntityStorageQueryResult entity_storage_query(
    EntityStorage* storage,
    b32 only_active, b32 only_2d,
    EntityType     type,
    EntityFlags    flags
);

/// Apply physics to all active entities with both a
/// transform2d and physics2d component.
/// Returns entities acted upon.
LD_API EntityStorageQueryResult system_physics_solver2d(
    EntityStorage* storage,
    f32 delta_time
);

#endif // header guard
