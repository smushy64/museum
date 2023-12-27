#if !defined(LD_CORE_GRAPHICS_PRIMITIVES_HPP)
#define LD_CORE_GRAPHICS_PRIMITIVES_HPP
// * Description:  Graphics primitives
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "shared/defines.h"
#include "engine/graphics/types.h"

/// Number of vertices for quad 2d.
#define QUAD_2D_VERTEX_COUNT (4)
/// Size of quad 2d vertex buffer.
#define QUAD_2D_VERTEX_BUFFER_SIZE\
    (QUAD_2D_VERTEX_COUNT * sizeof(struct Vertex2D))

/// 2D Quad with origin in center.
extern struct Vertex2D QUAD_2D_CENTERED[QUAD_2D_VERTEX_COUNT];
/// 2D Quad with origin in lower left corner.
extern struct Vertex2D QUAD_2D_LOWER_LEFT[QUAD_2D_VERTEX_COUNT];

/// Number of vertices for cube 3d.
#define CUBE_3D_VERTEX_COUNT (24)
/// Size of cube 3d vertex buffer.
#define CUBE_3D_VERTEX_BUFFER_SIZE\
    (CUBE_3D_VERTEX_COUNT * sizeof(struct Vertex3D))
/// Cube 3d with origin in center.
extern struct Vertex3D CUBE_3D[CUBE_3D_VERTEX_COUNT];

/// Number of indices for cube 3d.
#define CUBE_3D_INDEX_COUNT (36)
/// Size of cube 3d index buffer.
#define CUBE_3D_INDEX_BUFFER_SIZE\
    (CUBE_3D_INDEX_COUNT * sizeof(u8))
/// Cube 3d indices.
extern u8 CUBE_3D_INDICES[CUBE_3D_INDEX_COUNT];

/// Number of indices for quad 2d.
#define QUAD_2D_INDEX_COUNT (6)
/// Size of quad 2d index buffer.
#define QUAD_2D_INDEX_BUFFER_SIZE\
    (QUAD_2D_INDEX_COUNT * sizeof(u8))
/// Quad 2d indices.
extern u8 QUAD_2D_INDICES[QUAD_2D_INDEX_COUNT];

/// Width of null diffuse texture.
#define NULL_DIFFUSE_TEXTURE_WIDTH  (3)
/// Height of null diffuse texture.
#define NULL_DIFFUSE_TEXTURE_HEIGHT (3)
/// Number of pixels in null diffuse texture.
#define NULL_DIFFUSE_TEXTURE_PIXEL_COUNT (NULL_DIFFUSE_TEXTURE_WIDTH * NULL_DIFFUSE_TEXTURE_HEIGHT)
/// Texture for when the desired diffuse texture is not available.
extern u32 NULL_DIFFUSE_TEXTURE[NULL_DIFFUSE_TEXTURE_PIXEL_COUNT];

/// Width of null normal texture.
#define NULL_NORMAL_TEXTURE_WIDTH       (1)
/// Height of null normal texture.
#define NULL_NORMAL_TEXTURE_HEIGHT      (1)
/// Number of pixels in null normal texture.
#define NULL_NORMAL_TEXTURE_PIXEL_COUNT (1)
/// Texture for when the desired normal texture is not available.
extern u8 NULL_NORMAL_TEXTURE[3];

/// Width of null roughness texture.
#define NULL_ROUGHNESS_TEXTURE_WIDTH       (1)
/// Height of null roughness texture.
#define NULL_ROUGHNESS_TEXTURE_HEIGHT      (1)
/// Number of pixels in null roughness texture.
#define NULL_ROUGHNESS_TEXTURE_PIXEL_COUNT (1)
/// Texture for when the desired roughness texture is not available.
extern u8 NULL_ROUGHNESS_TEXTURE[NULL_ROUGHNESS_TEXTURE_PIXEL_COUNT];

#define NULL_METALLIC_TEXTURE_WIDTH       (1)
#define NULL_METALLIC_TEXTURE_HEIGHT      (1)
#define NULL_METALLIC_TEXTURE_PIXEL_COUNT (1)
extern u8 NULL_METALLIC_TEXTURE[NULL_METALLIC_TEXTURE_PIXEL_COUNT];

#define FRAMEBUFFER_QUAD_VERTEX_COUNT (6)

#endif // header guard
