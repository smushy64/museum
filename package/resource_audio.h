#if !defined(LP_PACKAGE_AUDIO_H)
#define LP_PACKAGE_AUDIO_H
/**
 * Description:  Package Audio
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 12, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"

struct ManifestItem;
struct LiquidPackageResource;

b32 package_audio(
    FSFile* file, usize buffer_start_offset,
    usize index, struct ManifestItem* item,
    usize buffer_size, void* buffer, struct LiquidPackageResource* out_resource );

#endif /* header guard */
