#if !defined(CORE_ASSET_HPP)
#define CORE_ASSET_HPP
// * Description:  Asset Manager
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 13, 2023
#include "defines.h"
#include "math/types.h"

enum TextureFormat : u8;

struct DebugImage {
    union {
        ivec2 dimensions;
        struct { i32 width, height; };
    };
    TextureFormat format;
    void* buffer;
};

LD_API b32 debug_load_bmp( const char* path, DebugImage* out_image );
LD_API void debug_destroy_bmp( DebugImage* image );

#endif // header guard
