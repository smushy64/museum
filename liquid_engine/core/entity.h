#if !defined(LD_CORE_ENTITY_HPP)
#define LD_CORE_ENTITY_HPP
// * Description:  Entity
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 22, 2023
#include "defines.h"
#include "core/mathf/types.h"
#include "core/graphics/types.h"

// typedef u32 EntityID;
// #define ENTITY_ID_INVALID (0)
// #define ENTITY_ID_MAX (U32_MAX - 1)
// #define ENTITY_ID_TO_INDEX( id )\
//     ((id) - 1)
// #define ENTITY_INDEX_TO_ID( index )\
//     ((index) + 1)
//
// typedef u32 EntityType;
// #define ENTITY_TYPE_NULL   (0)
// #define ENTITY_TYPE_CAMERA (1)
//
// typedef u32 EntityFlags;
// #define ENTITY_FLAG_ACTIVE  (1 << 0)
// #define ENTITY_FLAG_VISIBLE (1 << 1)
//
// #define ENTITY_MINIMUM_DATA_SIZE (256)
// /// Entity data.
// typedef union EntityData {
//     u8     data[ENTITY_MINIMUM_DATA_SIZE];
//     Camera camera;
// } EntityData;
//
// /// Entity Storage
// typedef struct EntityStorage {
//     Transform*   transforms;
//     EntityType*  types;
//     EntityID*    parents;
//     EntityFlags* flags;
//     EntityData*  data;
//
//     u32 count;
// } EntityStorage;
// /// Create a new entity in storage.
// /// Returns new entity id, if successful then id will not be zero.
// EntityID entity_storage_create_entity( EntityStorage* storage );
// /// Get an entity's transform.
// header_only Transform* entity_storage_get_transform(
//     EntityStorage* storage, EntityID id
// ) {
//     u32 index = ENTITY_ID_TO_INDEX(id);
//     if( index >= storage->count ) {
//         return NULL;
//     }
//
//     return storage->transforms + index;
// }
// /// Get an entity's type.
// header_only EntityType entity_storage_get_type(
//     EntityStorage* storage, EntityID id
// ) {
//     u32 index = ENTITY_ID_TO_INDEX(id);
//     if( index >= storage->count ) {
//         return NULL;
//     }
//
//     return *(storage->types + index);
// }
// /// Get an entity's flags.
// header_only EntityFlags entity_storage_get_flags(
//     EntityStorage* storage, EntityID id
// ) {
//     u32 index = ENTITY_ID_TO_INDEX(id);
//     if( index >= storage->count ) {
//         return NULL;
//     }
//
//     return *(storage->flags + index);
// }
// /// Get an entity's parent's id.
// header_only EntityID entity_storage_get_parent_id(
//     EntityStorage* storage, EntityID id
// ) {
//     u32 index = ENTITY_ID_TO_INDEX(id);
//     if( index >= storage->count ) {
//         return NULL;
//     }
//
//     return *(storage->parents + index);
// }
// /// Get an entity's data.
// header_only EntityData* entity_storage_get_data(
//     EntityStorage* storage, EntityID id
// ) {
//     u32 index = ENTITY_ID_TO_INDEX(id);
//     if( index >= storage->count ) {
//         return NULL;
//     }
//
//     return storage->data + index;
// }

#endif // header guard
