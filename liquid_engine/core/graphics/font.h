#if !defined(CORE_GRAPHICS_FONT_HPP)
#define CORE_GRAPHICS_FONT_HPP
// * Description:  Font
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 05, 2023
#include "defines.h"
#include "renderer/renderer.h"
#include "core/math/types.h"

/// Font metrics for a specific codepoint
struct GlyphMetrics {
    vec2 atlas_coordinate;
    vec2 atlas_scale;
    union {
        ivec2 pixel_scale;
        struct {
            i32 pixel_width;
            i32 pixel_height;
        };
    };
    i32  pixel_left_bearing;
    i32  pixel_top_bearing;
    i32  pixel_advance;
    c8   codepoint;
};

/// Font Data
struct FontData {
    GlyphMetrics* metrics;
    f32           point_size;
    u32           glyph_count;
    Texture       texture;
};

LD_API GlyphMetrics* font_data_metrics( FontData* font_data, c8 codepoint );

LD_API b32 debug_font_create(
    const char* path,
    f32         point_size,
    FontData*   out_font
);
LD_API void debug_font_destroy( FontData* font_data );

#endif // header guard
