#if !defined(LD_PACKAGE_MANIFEST_H)
#define LD_PACKAGE_MANIFEST_H
/**
 * Description:  Package Utility Manifest
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 08, 2024
*/
#include "shared/defines.h"
#include "shared/liquid_package.h"
#include "core/path.h"
#include "core/string.h"
#include "package/error.h"

typedef struct ManifestItem {
    PackageResourceType type;
    PackageCompression  compression;
    PathSlice           path;
    StringSlice         identifier;

    union {
        struct {
            PackageTextureFlags flags;
        } texture;
    };
} ManifestItem;

typedef struct Manifest {
    StringSlice text;
    struct ManifestList {
        ManifestItem* buffer;
        u32 count;
        u32 capacity;
    } items;
    usize longest_identifier_length;
    usize longest_type_name_length;

    volatile u32 item_completion_count;
} Manifest;

PackageError manifest_parse( PathSlice manifest_path, Manifest* out_manifest );
void manifest_destroy( Manifest* manifest );

#endif /* header guard */
