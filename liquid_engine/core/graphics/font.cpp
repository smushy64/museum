// * Description:  Font Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 05, 2023
#include "core/graphics/font.h"
#include "core/math/type_functions.h"
#include "core/memory.h"
#include "platform/platform.h"

// TODO(alicia): TEMPORARY
#include "core/string.h"
#define STB_TRUETYPE_IMPLEMENTATION 1
#define STBTT_cos(x)      (cos(x))
#define STBTT_fabs(x)     (absolute(x))
#define STBTT_fmod(x,y)   (mod(x,y))
#define STBTT_ifloor(x)   ((int)floor32(x))
#define STBTT_iceil(x)    ((int)ceil32(x))
#define STBTT_sqrt(x)     (sqrt(x))
// TODO(alicia): check if this is actually necessary
#define STBTT_pow(x,y)    (0)
#define STBTT_acos(x)     (acos(x))
#define STBTT_malloc(x,u) ((void)(u),mem_alloc(x, MEMTYPE_USER))
#define STBTT_free(x,u)   ((void)(u),mem_free(x))
#define STBTT_assert(x) do {\
    if( !(x) ) {\
        PANIC();\
    }\
} while(0)
#define STBTT_strlen(x)   (str_length(x))
#define STBTT_memcpy      mem_copy
#define STBTT_memset      mem_set

#include <stb/stb_truetype.h>

#define FONT_ATLAS_WIDTH  (1024)
#define FONT_ATLAS_HEIGHT (1024)
#define FONT_ATLAS_OVERSAMPLE (2)

LD_API b32 debug_font_create(
    const char* path,
    f32         point_size,
    FontData*   out_font
) {
    PlatformFileHandle font_file_handle = {};
    if( !platform_file_open(
        path,
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ |
        PLATFORM_FILE_OPEN_EXISTING,
        &font_file_handle
    ) ) {
        return false;
    }

    Texture texture = {};
    texture.width   = FONT_ATLAS_WIDTH;
    texture.height  = FONT_ATLAS_HEIGHT;
    texture.format  = TEXTURE_FORMAT_RGBA;

    u32 buffer_size =
        horizontal_mul( texture.dimensions ) *
        texture_format_byte_size( texture.format );
    texture.buffer  = mem_alloc( buffer_size, MEMTYPE_UNKNOWN );
    ASSERT( texture.buffer );
    u32 intermediate_buffer_size = horizontal_mul( texture.dimensions );
    u8* intermediate_buffer = (u8*)mem_alloc(
        intermediate_buffer_size,
        MEMTYPE_UNKNOWN
    );
    ASSERT( intermediate_buffer );

    usize font_file_buffer_size =
        platform_file_query_size( &font_file_handle );
    void* font_file_buffer =
        mem_alloc( font_file_buffer_size, MEMTYPE_UNKNOWN );
    ASSERT( font_file_buffer );

    b32 read_result = platform_file_read(
        &font_file_handle,
        font_file_buffer_size,
        font_file_buffer_size,
        font_file_buffer
    );

    platform_file_close( &font_file_handle );

    if( !read_result ) {
        return false;
    }

    #define TIGHT_PACKING 0
    #define PADDING 1
    stbtt_pack_context pack_context;
    ASSERT( stbtt_PackBegin(
        &pack_context,
        (u8*)intermediate_buffer,
        texture.width,
        texture.height,
        TIGHT_PACKING,
        PADDING,
        nullptr
    ) );

    stbtt_PackSetOversampling(
        &pack_context,
        FONT_ATLAS_OVERSAMPLE,
        FONT_ATLAS_OVERSAMPLE
    );

    c8 from = ' ';
    c8 to   = '~';

    u32 glyph_count = ((u32)to - (u32)from) + 1;
    GlyphMetrics* metrics = (GlyphMetrics*)mem_alloc(
        glyph_count * sizeof(GlyphMetrics),
        MEMTYPE_UNKNOWN
    );
    ASSERT( metrics );

    stbtt_packedchar* packed_chars = (stbtt_packedchar*)mem_alloc(
        sizeof(stbtt_packedchar) * glyph_count,
        MEMTYPE_UNKNOWN
    );
    ASSERT(packed_chars);

    stbtt_pack_range pack_range   = {};
    pack_range.font_size          = point_size;
    pack_range.num_chars          = (i32)glyph_count;
    pack_range.chardata_for_range = packed_chars;
    pack_range.first_unicode_codepoint_in_range = from;

    ASSERT(stbtt_PackFontRanges(
        &pack_context,
        (u8*)font_file_buffer,
        0,
        &pack_range,
        1
    ));
    stbtt_PackEnd( &pack_context );

    stbtt_fontinfo font_info;
    ASSERT( stbtt_InitFont(
        &font_info,
        (u8*)font_file_buffer,
        stbtt_GetFontOffsetForIndex( (u8*)font_file_buffer, 0 )
    ) );

    f32 em_scale = stbtt_ScaleForMappingEmToPixels( &font_info, point_size );

    for( u32 i = 0; i < glyph_count; ++i ) {
        c8 character = from + (c8)i;
        GlyphMetrics current_metrics = {};
        stbtt_packedchar packed_char = packed_chars[i];

        current_metrics.character = character;
        current_metrics.scale.width  = (f32)( packed_char.x1 - packed_char.x0 );
        current_metrics.scale.height = (f32)( packed_char.y1 - packed_char.y0 );
        current_metrics.atlas_coordinate.x =
            (f32)( packed_char.x0 ) / (f32)texture.width;
        current_metrics.atlas_coordinate.y =
            (f32)( packed_char.y0 ) / (f32)texture.height;
        current_metrics.atlas_scale.x =
            current_metrics.scale.width / (f32)texture.width;
        current_metrics.atlas_scale.y =
            current_metrics.scale.height / (f32)texture.height;

        i32 advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(
            &font_info,
            character,
            &advance_width,
            &left_side_bearing
        );

        current_metrics.left_bearing = (i32)((f32)left_side_bearing * em_scale);
        current_metrics.top_bearing  = packed_char.yoff2;
        current_metrics.advance      = packed_char.xadvance;

        metrics[i] = current_metrics;
    }

    u32* texture_pixels = (u32*)texture.buffer;
    for( i32 y = 0; y < texture.height; ++y ) {
        for( i32 x = 0; x < texture.width; ++x ) {
            i32 index = (y * texture.height) + x;
            u8 pixel = intermediate_buffer[index];
            texture_pixels[index] = RGBA_U32( pixel, pixel, pixel, 255 );
        }
    }

    mem_free( intermediate_buffer );
    mem_free( font_file_buffer );
    mem_free( packed_chars );
    out_font->point_size  = point_size;
    out_font->glyph_count = glyph_count;
    out_font->metrics     = metrics;
    out_font->texture     = texture;
    return true;
}

LD_API GlyphMetrics* font_data_metrics(
    FontData* font_data,
    c8 character
) {
    for( u32 i = 0; font_data->glyph_count; ++i ) {
        if( font_data->metrics[i].character == character ) {
            return &font_data->metrics[i];
        }
    }
    return nullptr;
}

