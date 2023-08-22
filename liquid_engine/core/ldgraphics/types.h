#if !defined(LD_CORE_GRAPHICS_TYPES_HPP)
#define LD_CORE_GRAPHICS_TYPES_HPP
// * Description:  Graphics types
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/ldmath/types.h"

#define VERTEX_2D_LOCATION_POSITION (0)
#define VERTEX_2D_LOCATION_UV       (1)
struct Vertex2D {
    vec2 position;
    vec2 uv;
};
/// Make vertex 2d.
extern FORCE_INLINE
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
struct Vertex3D {
    vec3 position;
    vec2 uv;      
    vec3 normal;  
    vec4 tangent; 
};
/// Make vertex 3d.
extern FORCE_INLINE
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
