// * Description:  Entity-Component System Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 03, 2023
#include "ecs.h"
#include "memory.h"

LD_API EntityStorageQueryResult entity_storage_query_flags(
    EntityStorage* storage,
    b32 only_active, b32 only_2d,
    EntityFlags flags
) {
    EntityStorageQueryResult result = {};

    for( u32 i = 0; i < MAX_ENTITIES; ++i ) {
        Entity* entity = &storage->entities[i];

        if( entity->type == ENTITY_TYPE_NULL ) {
            continue;
        }
        if( only_active ) {
            if( !entity->is_active ) {
                continue;
            }
        }
        if( only_2d ) {
            if( !entity->is_2d ) {
                continue;
            }
        }
        if( ARE_BITS_SET( entity->flags, flags ) ) {
            result.indices[result.index_count++] = i;
        }
    }

    return result;
}
LD_API EntityStorageQueryResult entity_storage_query_type(
    EntityStorage* storage,
    b32 only_active, b32 only_2d,
    EntityType     type
) {
    EntityStorageQueryResult result = {};

    for( u32 i = 0; i < MAX_ENTITIES; ++i ) {
        Entity* entity = &storage->entities[i];
        if( !entity->type ) {
            continue;
        }
        if( only_active ) {
            if( !entity->is_active ) {
                continue;
            }
        }
        if( only_2d ) {
            if( !entity->is_2d ) {
                continue;
            }
        }
        if( entity->type == type ) {
            result.indices[result.index_count++] = i;
        }
    }

    return result;
}
LD_API EntityStorageQueryResult entity_storage_query(
    EntityStorage* storage,
    b32 only_active, b32 only_2d,
    EntityType     type,
    EntityFlags    flags
) {
    EntityStorageQueryResult result = {};

    for( u32 i = 0; i < MAX_ENTITIES; ++i ) {
        Entity* entity = &storage->entities[i];

        if( entity->type == ENTITY_TYPE_NULL ) {
            continue;
        }
        if( only_active ) {
            if( !entity->is_active ) {
                continue;
            }
        }
        if( only_2d ) {
            if( !entity->is_2d ) {
                continue;
            }
        }
        if(
            entity->type == type &&
            ARE_BITS_SET( entity->flags, flags )
        ) {
            result.indices[result.index_count++] = i;
        }

    }

    return result;
}

LD_API i32 entity_storage_create_entity(
    EntityStorage* storage,
    Entity* entity
) {
    for( u32 i = 0; i < MAX_ENTITIES; ++i ) {
        if( storage->entities[i].type == ENTITY_TYPE_NULL ) {
            mem_copy(
                &storage->entities[i],
                entity,
                sizeof(Entity)
            );
            return i;
        }
    }

    return -1;
}

LD_API void entity_storage_mark_null( EntityStorage* storage, u32 index ) {
    storage->entities[index].type = ENTITY_TYPE_NULL;
}

LD_API EntityStorageQueryResult system_physics_solver2d(
    EntityStorage* storage,
    f32 delta_time
) {
    EntityStorageQueryResult query_result = entity_storage_query_flags(
        storage,
        true, true,
        ENTITY_FLAG_TRANSFORM |
        ENTITY_FLAG_PHYSICS
    );

    for( u32 i = 0; i < query_result.index_count; ++i ) {
        Entity* entity = &storage->entities[i];
        entity->transform2d.position +=
            entity->physics2d.velocity * delta_time;
        entity->transform2d.rotation +=
            entity->physics2d.angular_velocity * delta_time;

        entity->physics2d.velocity *=
            ( 1.0f - delta_time * entity->physics2d.drag );
        entity->physics2d.angular_velocity *=
            ( 1.0f - delta_time * entity->physics2d.angular_drag );
    }

    return query_result;
}


