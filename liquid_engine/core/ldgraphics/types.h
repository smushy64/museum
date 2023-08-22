#if !defined(LD_CORE_GRAPHICS_TYPES_HPP)
#define LD_CORE_GRAPHICS_TYPES_HPP
// * Description:  Graphics types
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/ldmath/types.h"

/// Renderer ID
typedef u32 RendererID;
/// Invalid renderer id.
#define RENDERER_ID_INVALID (U32_MAX)

/// 3D camera.
typedef struct Camera3D {
    vec3 position;
    vec3 target;
    quat rotation;
    f32  fov;
    f32  near_;
    f32  far_;
} Camera3D;

/// 2D camera.
typedef struct Camera2D {
    vec2 position;
    f32  scale;
    f32  rotation_radians;
} Camera2D;

/// Types of camera.
typedef enum CameraType : u32 {
    CAMERA_TYPE_3D,
    CAMERA_TYPE_2D
} CameraType;

/// Combined camera.
typedef struct Camera {
    CameraType type;
    union {
        Camera3D camera_3d;
        Camera2D camera_2d;
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
header_only always_inline
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
header_only always_inline
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

#endif // header guard
