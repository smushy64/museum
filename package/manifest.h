#if !defined(LP_MANIFEST_H)
#define LP_MANIFEST_H
/**
 * Description:  Package Manifest
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 07, 2023
*/
#include "shared/defines.h"
#include "core/string.h"
#include "core/path.h"
#include "shared/liquid_package.h"

typedef struct ManifestItem {
    StringSlice               identifier;
    LiquidPackageResourceType type;
    LiquidPackageCompression  compression;
    PathSlice                 path;
} ManifestItem;

typedef struct Manifest {
    StringSlice   directory;
    usize         item_count;
    ManifestItem* items;
    usize         text_buffer_size;
    char*         text_buffer;
} Manifest;

b32 manifest_parse( PathSlice path, Manifest* out_manifest );
void manifest_free( Manifest* manifest );

#endif /* header guard */
