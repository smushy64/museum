#if !defined(LD_CORE_GRAPHICS_TYPES_HPP)
#define LD_CORE_GRAPHICS_TYPES_HPP
// * Description:  Graphics types
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/mathf/types.h"

typedef u32 RenderID;
#define RENDER_ID_NULL (0)

typedef enum GraphicsTextureType : u32 {
    GRAPHICS_TEXTURE_TYPE_2D,
    GRAPHICS_TEXTURE_TYPE_3D,

    GRAPHICS_TEXTURE_TYPE_COUNT
} GraphicsTextureType;
header_only const char*
graphics_texture_type_to_cstr( GraphicsTextureType type ) {
    assert( type < GRAPHICS_TEXTURE_TYPE_COUNT );
    const char* strings[GRAPHICS_TEXTURE_TYPE_COUNT] = {
        "Texture 2D",
        "Texture 3D"
    };
    return strings[type];
}
header_only usize
graphics_texture_type_dimension_count( GraphicsTextureType type ) {
    assert( type < GRAPHICS_TEXTURE_TYPE_COUNT );
    usize dimensions[GRAPHICS_TEXTURE_TYPE_COUNT] = {
        2, 3
    };
    return dimensions[type];
}

typedef enum GraphicsTextureFormat : u32 {
    GRAPHICS_TEXTURE_FORMAT_GRAYSCALE,
    GRAPHICS_TEXTURE_FORMAT_RGB,
    GRAPHICS_TEXTURE_FORMAT_RGBA,
    GRAPHICS_TEXTURE_FORMAT_SRGB,
    GRAPHICS_TEXTURE_FORMAT_SRGBA,

    GRAPHICS_TEXTURE_FORMAT_COUNT
} GraphicsTextureFormat;
header_only const char*
graphics_texture_format_to_cstr( GraphicsTextureFormat format ) {
    assert( format < GRAPHICS_TEXTURE_FORMAT_COUNT );
    const char* strings[GRAPHICS_TEXTURE_FORMAT_COUNT] = {
        "Format Grayscale",
        "Format RGB",
        "Format RGBA",
        "Format sRGB",
        "Format sRGBA"
    };
    return strings[format];
}
header_only usize
graphics_texture_format_channel_count( GraphicsTextureFormat format ) {
    assert( format < GRAPHICS_TEXTURE_FORMAT_COUNT );
    usize channels[GRAPHICS_TEXTURE_FORMAT_COUNT] = {
        1, 3, 4, 3, 4
    };
    return channels[format];
}

typedef enum GraphicsTextureBaseType : u32 {
    GRAPHICS_TEXTURE_BASE_TYPE_UINT8,
    GRAPHICS_TEXTURE_BASE_TYPE_UINT16,
    GRAPHICS_TEXTURE_BASE_TYPE_UINT32,
    GRAPHICS_TEXTURE_BASE_TYPE_FLOAT32,

    GRAPHICS_TEXTURE_BASE_TYPE_COUNT
} GraphicsTextureBaseType;
header_only const char*
graphics_texture_base_type_to_cstr( GraphicsTextureBaseType type ) {
    assert( type < GRAPHICS_TEXTURE_BASE_TYPE_COUNT );
    const char* strings[GRAPHICS_TEXTURE_BASE_TYPE_COUNT] = {
        "Base Type u8",
        "Base Type u16",
        "Base Type u32",
        "Base Type f32"
    };
    return strings[type];
}
header_only usize
graphics_texture_base_type_size( GraphicsTextureBaseType type ) {
    assert( type < GRAPHICS_TEXTURE_BASE_TYPE_COUNT );
    usize sizes[GRAPHICS_TEXTURE_BASE_TYPE_COUNT] = {
        1, 2, 4, 4
    };
    return sizes[type];
}

typedef enum GraphicsTextureWrap : u32 {
    GRAPHICS_TEXTURE_WRAP_CLAMP,
    GRAPHICS_TEXTURE_WRAP_REPEAT,

    GRAPHICS_TEXTURE_WRAP_COUNT
} GraphicsTextureWrap;
header_only const char*
graphics_texture_wrap_to_cstr( GraphicsTextureWrap wrap ) {
    assert( wrap < GRAPHICS_TEXTURE_WRAP_COUNT );
    const char* strings[GRAPHICS_TEXTURE_WRAP_COUNT] = {
        "Wrap Clamp",
        "Wrap Repeat"
    };
    return strings[wrap];
}

typedef enum GraphicsTextureFilter : u32 {
    GRAPHICS_TEXTURE_FILTER_NEAREST,
    GRAPHICS_TEXTURE_FILTER_BILINEAR,

    GRAPHICS_TEXTURE_FILTER_COUNT
} GraphicsTextureFilter;
header_only const char*
graphics_texture_filter_to_cstr( GraphicsTextureFilter filter ) {
    assert( filter < GRAPHICS_TEXTURE_FILTER_COUNT );
    const char* strings[GRAPHICS_TEXTURE_FILTER_COUNT] = {
        "Filter Nearest-Neighbor",
        "Filter Bilinear"
    };
    return strings[filter];
}

header_only usize graphics_calculate_texture_buffer_size(
    GraphicsTextureType     type,
    GraphicsTextureFormat   format,
    GraphicsTextureBaseType base_type,
    u32 width, u32 height
) {
    usize dimension_count = graphics_texture_type_dimension_count( type );
    usize channel_count   = graphics_texture_format_channel_count( format );
    usize base_type_size  = graphics_texture_base_type_size( base_type );
    return dimension_count * channel_count * base_type_size * width * height;
}

/// Opaque id for font.
typedef u32 FontID;
#define FONT_ID_DEFAULT (0)

/// 3D camera.
typedef struct Camera {
    struct Transform* transform;
    f32  fov_radians;
    union {
        struct {
            f32  near_clip;
            f32  far_clip;
        };
        f32 clipping_planes[2];
    };
} Camera;

#define VERTEX_2D_LOCATION_POSITION (0)
#define VERTEX_2D_LOCATION_UV       (1)
/// 2D Vertex for use in renderer.
struct Vertex2D {
    vec2 position;
    vec2 uv;
};
/// Make vertex 2d.
header_only force_inline maybe_unused
struct Vertex2D vertex2d( vec2 position, vec2 uv ) {
    struct Vertex2D result;
    result.position = position;
    result.uv       = uv;
    return result;
}

#define VERTEX_3D_LOCATION_POSITION (0)
#define VERTEX_3D_LOCATION_UV       (1)
#define VERTEX_3D_LOCATION_NORMAL   (2)
#define VERTEX_3D_LOCATION_COLOR    (3)
#define VERTEX_3D_LOCATION_TANGENT  (4)
/// 3D Vertex for use in renderer.
struct Vertex3D {
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec4 color;
    vec3 tangent; 
};
/// Make vertex 3d.
header_only force_inline maybe_unused
struct Vertex3D vertex3d(
    vec3 position,
    vec2 uv,
    vec3 normal,
    vec4 color,
    vec3 tangent
) {
    struct Vertex3D result;
    result.position = position;
    result.uv       = uv;
    result.color    = color;
    result.normal   = normal;
    result.tangent  = tangent;
    return result;
}

/// Mesh
typedef struct Mesh {
    Transform* transform;
    struct Vertex3D* vertices;
    u32* indices;
    u32 vertex_count;
    u32 index_count;
} Mesh;
/// Make mesh.
header_only force_inline maybe_unused
Mesh mesh(
    Transform* transform,
    struct Vertex3D* vertices, u32 vertex_count,
    u32* indices, u32 index_count
) {
    Mesh result;
    result.transform    = transform;
    result.vertices     = vertices;
    result.vertex_count = vertex_count;
    result.indices      = indices;
    result.index_count  = index_count;
    return result;
}

#endif // header guard
