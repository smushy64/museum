#if !defined(LP_PARSE_MANIFEST_H)
#define LP_PARSE_MANIFEST_H
/**
 * Description:  Parse manifests.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 19, 2023
*/
#include "defines.h"
#include "core/string.h"
#include "core/collections.h"
#include "core/memory.h"
#include "liquid_package.h"

typedef struct {
    LiquidPackageResourceType type;
    StringSlice path, id;
} ManifestResource;

b32 packager_manifest_parse(
    List* list_resources, struct StackAllocator* stack, const char* path );

#endif /* header guard */
