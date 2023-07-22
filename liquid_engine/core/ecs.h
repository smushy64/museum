#if !defined(CORE_ECS_HPP)
#define CORE_ECS_HPP
// * Description:  Entity-Component System
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 03, 2023
#include "defines.h"
#include "core/math/types.h"
#include "core/math/functions.h"
#include "core/graphics.h"
#include "core/threading.h"

typedef u32 EntityType;
#define ENTITY_TYPE_NULL 0

typedef u16 EntityComponentFlags;
#define ENTITY_COMPONENT_FLAG_TRANSFORM       (1 << 0)
#define ENTITY_COMPONENT_FLAG_PHYSICS         (1 << 1)
#define ENTITY_COMPONENT_FLAG_COLLIDER_2D     (1 << 2)
#define ENTITY_COMPONENT_FLAG_SPRITE_RENDERER (1 << 3)

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

struct SpriteRenderer {
    struct Texture* atlas;
    union {
        struct {
            ivec2 atlas_cell_position;
            ivec2 atlas_cell_scale;
        };
        ivec4 atlas_coordinate;
    };
    rgba tint;

    b32 flip_x, flip_y;
    u32 atlas_cell_size;
    i32 z_index;
};
inline SpriteRenderer sprite_renderer_new(
    struct Texture* texture_atlas,
    u32 cell_size
) {
    SpriteRenderer result;
    result.atlas               = texture_atlas;
    result.atlas_cell_size     = cell_size;
    result.atlas_cell_scale    = IVEC2::ONE;
    result.atlas_cell_position = IVEC2::ZERO;
    result.tint                = RGBA::WHITE;
    result.flip_x              = false;
    result.flip_y              = false;
    return result;
}

enum ColliderType2D : u8 {
    COLLIDER_TYPE_2D_RECT,
    COLLIDER_TYPE_2D_CIRCLE,
};
struct Collider2D {
    ColliderType2D type;
    union {
        struct {
            f32 half_width;
            f32 half_height;
        } rect;
        struct {
            f32 radius;
        } circle;
    };
};
inline Collider2D collider2d_new_rect( f32 width, f32 height ) {
    Collider2D result       = {};
    result.type             = COLLIDER_TYPE_2D_RECT;
    result.rect.half_width  = absolute(width)  / 2.0f;
    result.rect.half_height = absolute(height) / 2.0f;

    return result;
}
inline Collider2D collider2d_new_circle( f32 radius ) {
    Collider2D result    = {};
    result.type          = COLLIDER_TYPE_2D_CIRCLE;
    result.circle.radius = radius;

    return result;
}

#define MAX_ENTITIES 20
#define MAX_ENTITY_SIZE 192

typedef u16 EntityStateFlag;
#define ENTITY_STATE_FLAG_IS_ACTIVE  (1 << 0)
#define ENTITY_STATE_FLAG_IS_2D      (1 << 1)
#define ENTITY_STATE_FLAG_IS_VISIBLE (1 << 2)

struct Entity {
    EntityType           type;
    EntityComponentFlags component_flags;
    EntityStateFlag      state_flags;
    mat4 matrix;
    union {
        struct {
            Transform transform;
            Physics   physics;
        };
        struct {
            Transform2D    transform2d;
            Physics2D      physics2d;
            SpriteRenderer sprite_renderer;
            Collider2D     collider2D;
        };
        u8 bytes[MAX_ENTITY_SIZE];
    };
};
FORCE_INLINE void entity_set_active( Entity* entity, b32 active ) {
    if( active ) {
        entity->state_flags |= ENTITY_STATE_FLAG_IS_ACTIVE;
    } else {
        CLEAR_BIT( entity->state_flags, ENTITY_STATE_FLAG_IS_ACTIVE );
    }
}
FORCE_INLINE void entity_set_visible( Entity* entity, b32 visible ) {
    if( visible ) {
        entity->state_flags |= ENTITY_STATE_FLAG_IS_VISIBLE;
    } else {
        CLEAR_BIT( entity->state_flags, ENTITY_STATE_FLAG_IS_VISIBLE );
    }
}
inline vec3 entity_position( Entity* entity ) {
    return {
        entity->matrix.m30,
        entity->matrix.m31,
        entity->matrix.m32
    };
}
inline vec2 entity_position2d( Entity* entity ) {
    return { entity->matrix.m30, entity->matrix.m31 };
}
inline Rect2D entity_collider2d_make_rect( Entity* entity ) {
    vec2 position = entity_position2d( entity );
    Collider2D* collider = &entity->collider2D;
    ASSERT( collider->type == COLLIDER_TYPE_2D_RECT );
    Rect2D result;

    result.left   = position.x - collider->rect.half_width;
    result.right  = position.x + collider->rect.half_width;
    result.top    = position.y + collider->rect.half_height;
    result.bottom = position.y - collider->rect.half_height;
    return result;
}

/// Entity Identifier
typedef i32 EntityID;

struct EntityStorageQueryResult {
    u32 count;
    u32 ids[MAX_ENTITIES];
};

/// Conveniece structure for iterating through
/// query result entities.
struct QueryResultIterator {
private:
    EntityStorageQueryResult* query_result;
    u32 current;
public:
    QueryResultIterator( EntityStorageQueryResult* query_result ):
        query_result(query_result), current(0) {}

    inline b32 next( EntityID* out_next ) {
        if( current >= query_result->count ) {
            return false;
        }
        *out_next = query_result->ids[current++];
        return true;
    }
};

/// Where entities get stored :)
struct EntityStorage {
    Entity entities[MAX_ENTITIES];
};
/// Get entity from storage by id
FORCE_INLINE Entity* entity_storage_get(
    EntityStorage* storage, EntityID id
) {
    return &storage->entities[id];
}

/// Create a new entity in the entity storage.
/// Searches through entities until it finds a null entity
/// and copies the entity data provided to that destination.
/// Returns -1 if no null entity was found.
/// Returns entity id if successful.
LD_API EntityID entity_storage_create_entity(
    EntityStorage* storage,
    Entity* entity
);

/// Mark an entity as being null.
LD_API void entity_storage_mark_null( EntityStorage* storage, EntityID id );

/// Query filter function prototype.
typedef b32 (*EntityFilterFN)( Entity* entity );

/// Query for entities that pass entity filter function.
LD_API EntityStorageQueryResult entity_storage_query(
    EntityStorage* storage,
    EntityFilterFN filter_function
);

/// Apply physics to all active entities with both a
/// transform2d and physics2d component.
/// Returns entities acted upon.
LD_API EntityStorageQueryResult system_physics2d_solver(
    EntityStorage*          storage,
    f32                     delta_time
);

/// Test collider against a list of colliders.
/// Returns a pointer to the first entity hit,
/// or null if no colliders were hit.
LD_API Entity* system_collider2d_solver(
    EntityStorage*            storage,
    EntityID                  collider_id,
    EntityStorageQueryResult* colliders_to_test
);

#endif // header guard
