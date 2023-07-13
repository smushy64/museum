#if !defined(CORE_GRAPHICS_HPP)
#define CORE_GRAPHICS_HPP
/**
 * Description:  Graphics
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "defines.h"
#include "math/types.h"

/// Renderer ID.
/// Some C++ shenanigans to get it to
/// be uninitialized if 0.
struct RendererID {
private:
    union {
        [[maybe_unused]]
        u8  ___unused[3];
        b8  initialized;
        u32 id_value;
    };
public:
    RendererID() : id_value(0) {}
    RendererID( u32 x ) { id_value = 0x1 | (x << 1); }

    b32 is_valid() const { return initialized; }
    u32 id() const { return id_value >> 1; }
};

/// Vertex definition
struct Vertex {
    vec4 position;
    vec3 color;
    vec2 uv;
    vec3 normal;
};

/// 2D-Vertex definition
struct Vertex2D {
    vec2 position;
    vec2 uv;
};

enum VertexType : u8 {
    VERTEX_TYPE_3D,
    VERTEX_TYPE_2D
};

enum IndexType : u8 {
    INDEX_TYPE_U32,
    INDEX_TYPE_U16,
    INDEX_TYPE_U8
};

typedef u32 MeshFlags;

/// Mesh definition
struct Mesh {
    union {
        Vertex*   vertices_3d;
        Vertex2D* vertices_2d;
        void*     vertices;
    };
    union {
        u32* indices32;
        u16* indices16;
        u8*  indices8;
        void* indices;
    };

    VertexType vertex_type;
    IndexType  index_type;
    b8         is_static_mesh;

    u32 vertex_count;
    u32 index_count;

    RendererID id;
};

/// Texture formats
enum TextureFormat : u8 {
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RGBA,
};

/// Texture definition
struct Texture {
    union {
        ivec2 dimensions;
        struct { i32 width, height; };
    };
    void* buffer;
    TextureFormat format;
    RendererID id;
};

#endif // header guard
