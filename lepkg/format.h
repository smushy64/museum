#if !defined(LEPKG_FILEFORMAT_HPP)
#define LEPKG_FILEFORMAT_HPP
// * Description:  lepkg file format
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 11, 2023
#include "defines.h"

/// Liquid Engine Package Identifier
#define LEPKG_FILE_IDENTIFIER (658458948ul)

typedef struct Vector2D {
    f32 x, y;
} Vector2D;

typedef struct Vector3D {
    f32 x, y, z;
} Vector3D;

typedef struct Vector2DInt {
    i32 x, y;
} Vector2DInt;

typedef struct Vector3DInt {
    i32 x, y, z;
} Vector3DInt;

typedef u32 AssetID;

typedef enum : u8 {
    ASSET_TYPE_UNKNOWN,

    ASSET_TYPE_METADATA,
    ASSET_TYPE_IMAGE,
    ASSET_TYPE_FONT,
    ASSET_TYPE_AUDIO,
    ASSET_TYPE_MODEL_3D,
    ASSET_TYPE_SHADER
} AssetType;

typedef struct AssetMetaData_0_1 {

} AssetMetaData_0_1;

typedef enum : u32 {
    TEXTURE_FORMAT_UNKNOWN,

    TEXTURE_FORMAT_RGBA,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RED
} TextureFormat;

typedef struct PACKED {
    union {
        Vector2DInt dimensions;
        struct { i32 width, height; };
    };
    u64 buffer_offset;
    TextureFormat format;
} AssetTexture_0_1;

typedef struct AssetFontGlyphMetric_0_1 {
    Vector2D atlas_position;
    Vector2D atlas_scale;

    Vector2DInt pixel_scale;

    i32 pixel_left_bearing;
    i32 pixel_top_bearing;
    i32 pixel_advance;

    c32 codepoint;
} AssetFontGlyphMetric_0_1;

typedef struct PACKED {
    f32     point_size;
    AssetID texture;
    u32     metrics_count;
    u64     metrics_offset;
} AssetFont_0_1;

typedef struct PACKED {

} AssetAudio_0_1;

typedef enum : u8 {
    VERTEX_TYPE_2D,
    VERTEX_TYPE_3D
} VertexType;

typedef enum : u8 {
    INDEX_TYPE_U8,
    INDEX_TYPE_U16,
    INDEX_TYPE_U32,
} IndexType;

typedef struct Vertex2D_0_1 {
    Vector2D position;
    Vector2D uv;
} Vertex2D_0_1;

typedef struct Vertex3D_0_1 {
    Vector3D position;
    Vector2D uv;
    Vector3D normal;
} Vertex3D_0_1;

typedef struct PACKED {
    VertexType vertex_type; // 1
    IndexType  index_type;  // 1
    u64 vertex_count;       // 8
    u64 index_count;        // 8
    u64 vertices_offset;    // 8
    u64 indices_offset;     // 8
} AssetModel3D_0_1;

typedef enum : u8 {
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT,
    SHADER_TYPE_GEOMETRY,
    SHADER_TYPE_COMPUTE
} ShaderType;

typedef struct PACKED {
    ShaderType type;
    u64 buffer_offset;
    u64 buffer_size;
} AssetShader_0_1;

typedef struct PACKED {
    AssetType type;
    union {
        AssetMetaData_0_1 metadata;
        AssetTexture_0_1  texture;
        AssetFont_0_1     font;
        AssetAudio_0_1    audio;
        AssetModel3D_0_1  model3d;
        AssetShader_0_1   shader;
    };
} Asset_0_1;

/// Liquid Engine Package Header Version 0.1
typedef struct PACKED {
    u32 identifier;
    union {
        u32 version;
        struct {
            u16 minor;
            u16 major;
        };
    };
    u32 header_size;
    u32 asset_count;
    u64 file_size;
} LEPkgHeader_0_1;

#if defined( LEPKG_INTERNAL )

void print_outline( u16 major, u16 minor );

#endif

#endif // header guard
