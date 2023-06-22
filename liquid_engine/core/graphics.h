#if !defined(CORE_GRAPHICS_HPP)
#define CORE_GRAPHICS_HPP
/**
 * Description:  Graphics
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "defines.h"
#include "math/types.h"

typedef u32 RendererID;

/// Vertex definition
struct Vertex {
    vec4 position;
    vec3 color;
    vec2 uv;
    vec3 normal;
};

/// Mesh definition
struct Mesh {
    Vertex* vertices;
    u32*    indices;

    u32 vertex_count;
    u32 index_count;

    RendererID id;
};
LD_API Mesh mesh_create(
    u32 vertex_count,
    u32 index_count,
    Vertex* vertices,
    u32* indices
);

/// Basic mesh upload
LD_API void upload_mesh_list(
    struct RenderOrder* order,
    Mesh* list,
    u32 mesh_count
);

#endif // header guard
