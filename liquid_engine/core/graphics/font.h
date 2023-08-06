#if !defined(CORE_GRAPHICS_FONT_HPP)
#define CORE_GRAPHICS_FONT_HPP
// * Description:  Font
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 05, 2023
#include "defines.h"
#include "renderer/renderer.h"
#include "core/math/types.h"

struct GlyphMetrics {
    vec2 atlas_coordinate;
    vec2 atlas_scale;
    vec2 scale;
    i32  left_bearing;
    i32  top_bearing;
    i32  advance;
    c8   character;
};

/// Font Data
struct FontData {
    GlyphMetrics* metrics;
    f32           point_size;
    u32           glyph_count;
    Texture       texture;
};

LD_API GlyphMetrics* font_data_metrics( FontData* font_data, c8 character );

LD_API b32 debug_font_create(
    const char* path,
    f32         point_size,
    FontData*   out_font
);

#endif // header guard
