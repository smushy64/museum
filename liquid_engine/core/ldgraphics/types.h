#if !defined(LD_CORE_GRAPHICS_TYPES_HPP)
#define LD_CORE_GRAPHICS_TYPES_HPP
// * Description:  Graphics types
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/ldmath/types.h"
#include "core/ldmath/type_functions.h"

/// 3D camera.
typedef struct Camera {
    struct Transform* transform;
    f32  fov_radians;
    f32  near_clip;
    f32  far_clip;
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
#define VERTEX_3D_LOCATION_TANGENT  (3)
/// 3D Vertex for use in renderer.
struct Vertex3D {
    vec3 position;
    vec2 uv;      
    vec3 normal;  
    vec4 tangent; 
};
/// Make vertex 3d.
header_only force_inline maybe_unused
struct Vertex3D vertex3d(
    vec3 position, vec2 uv,
    vec3 normal, vec4 tangent
) {
    struct Vertex3D result;
    result.position = position;
    result.uv       = uv;
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
