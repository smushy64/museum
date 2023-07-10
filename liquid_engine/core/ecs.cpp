// * Description:  Entity-Component System Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 03, 2023
#include "ecs.h"
#include "memory.h"

LD_API EntityStorageQueryResult entity_storage_query(
    EntityStorage* storage,
    EntityFlags flags
) {
    EntityStorageQueryResult result = {};

    for( u32 i = 0; i < MAX_ENTITIES; ++i ) {
        if( storage->entities[i].type == ENTITY_TYPE_NULL ) {
            continue;
        }
        if( ARE_BITS_SET( storage->entities[i].flags, flags ) ) {
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

