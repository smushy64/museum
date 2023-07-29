// * Description:  Entity-Component System Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 03, 2023
#include "ecs.h"
#include "core/memory.h"
#include "core/logging.h"
#include "core/math/type_functions.h"

LD_API EntityStorageQueryResult entity_storage_query(
    EntityStorage* storage,
    EntityFilterFN filter_function
) {
    EntityStorageQueryResult result = {};

    for( EntityID id = 0; id < MAX_ENTITIES; ++id ) {
        Entity* entity = entity_storage_get( storage, id );
        if(
            entity->type == ENTITY_TYPE_NULL ||
            !filter_function( entity )
        ) {
            continue;
        }
        result.ids[result.count++] = id;
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

LD_API void entity_storage_mark_null(
    EntityStorage* storage,
    EntityID id
) {
    Entity* entity = entity_storage_get( storage, id );
    entity->type   = ENTITY_TYPE_NULL;
}

internal b32 filter_physics2d( Entity* entity ) {
    b32 is_active_visible_2d = CHECK_BITS(
        entity->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE  |
        ENTITY_STATE_FLAG_IS_VISIBLE |
        ENTITY_STATE_FLAG_IS_2D
    );
    if( !is_active_visible_2d ) {
        return false;
    }

    b32 has_transform_physics = CHECK_BITS(
        entity->component_flags,
        ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_PHYSICS
    );
    return has_transform_physics;
}

LD_API EntityStorageQueryResult system_physics2d_solver(
    EntityStorage*          storage,
    f32                     delta_time
) {
    EntityStorageQueryResult query_result = entity_storage_query(
        storage, filter_physics2d
    );

    QueryResultIterator iterator = &query_result;
    EntityID id;
    while( iterator.next( &id ) ) {
        Entity* entity = entity_storage_get( storage, id );

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

internal Entity* system_collider2d_solver_circle(
    EntityStorage*            storage,
    EntityID                  collider_id,
    Circle2D                  collider_circle,
    EntityStorageQueryResult* colliders_to_test
) {

    QueryResultIterator iterator = colliders_to_test;
    EntityID id;
    while( iterator.next( &id ) ) {
        if( id == collider_id ) {
            continue;
        }
        Entity* current = entity_storage_get( storage, id );
        vec2 current_position = entity_position2d( current );

        ASSERT( CHECK_BITS(
            current->component_flags,
            ENTITY_COMPONENT_FLAG_COLLIDER_2D
        ) );

        switch( current->collider2D.type ) {
            case COLLIDER_TYPE_2D_CIRCLE: {
                Circle2D current_collider_circle = {
                    current_position,
                    current->collider2D.circle.radius
                };

                if( circle2d_overlap_circle2d(
                    collider_circle,
                    current_collider_circle
                ) ) {
                    return current;
                }
            } break;
            case COLLIDER_TYPE_2D_RECT:
            default: UNIMPLEMENTED();
        }
    }

    return nullptr;
}

internal Entity* system_collider2d_solver_rect(
    EntityStorage*            storage,
    EntityID                  collider_id,
    Rect2D                    collider_rect,
    EntityStorageQueryResult* colliders_to_test
) {

    QueryResultIterator iterator = colliders_to_test;
    EntityID id;
    while( iterator.next( &id ) ) {
        if( id == collider_id ) {
            continue;
        }
        Entity* current = entity_storage_get( storage, id );
        vec2 current_position = entity_position2d( current );

        ASSERT( CHECK_BITS(
            current->component_flags,
            ENTITY_COMPONENT_FLAG_COLLIDER_2D
        ) );

        switch( current->collider2D.type ) {
            case COLLIDER_TYPE_2D_RECT: {
                Rect2D current_collider_rect = {
                    current_position.x - current->collider2D.rect.half_width,
                    current_position.x + current->collider2D.rect.half_width,
                    current_position.y + current->collider2D.rect.half_height,
                    current_position.y - current->collider2D.rect.half_height
                };

                if( rect2d_overlap_rect2d(
                    collider_rect,
                    current_collider_rect
                ) ) {
                    return current;
                }
            } break;
            case COLLIDER_TYPE_2D_CIRCLE:
            default: UNIMPLEMENTED();
        }
    }

    return nullptr;
}

LD_API Entity* system_collider2d_solver(
    EntityStorage*            storage,
    EntityID                  collider_id,
    EntityStorageQueryResult* colliders_to_test
) {

    Entity* collider = &storage->entities[collider_id];

    ASSERT( CHECK_BITS(
        collider->component_flags,
        ENTITY_COMPONENT_FLAG_COLLIDER_2D
    ) );
    vec2 position = entity_position2d( collider );
    switch( collider->collider2D.type ) {
        case COLLIDER_TYPE_2D_CIRCLE: {
            Circle2D collider_circle = {
                position,
                collider->collider2D.circle.radius
            };
            return system_collider2d_solver_circle(
                storage,
                collider_id,
                collider_circle,
                colliders_to_test
            );
        } break;
        case COLLIDER_TYPE_2D_RECT: {
            Rect2D collider_rect = {
                position.x - collider->collider2D.rect.half_width,
                position.x + collider->collider2D.rect.half_width,
                position.y + collider->collider2D.rect.half_height,
                position.y - collider->collider2D.rect.half_height
            };
            return system_collider2d_solver_rect(
                storage,
                collider_id,
                collider_rect,
                colliders_to_test
            );
        } break;
        default: UNIMPLEMENTED();
    }
    return nullptr;
}

