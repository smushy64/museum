#if !defined(LD_CORE_GRAPHICS_PRIMITIVES_HPP)
#define LD_CORE_GRAPHICS_PRIMITIVES_HPP
// * Description:  Graphics primitives
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/graphics/types.h"

/// Number of vertices for 2D quad.
#define QUAD_2D_VERTEX_COUNT (4)
/// Size of quad 2d buffer.
#define QUAD_2D_VERTEX_BUFFER_SIZE\
    (QUAD_2D_VERTEX_COUNT * sizeof(struct Vertex2D))
/// 2D Quad with origin in center.
extern struct Vertex2D QUAD_2D_CENTERED[QUAD_2D_VERTEX_COUNT];
/// 2D Quad with origin in lower left corner.
extern struct Vertex2D QUAD_2D_LOWER_LEFT[QUAD_2D_VERTEX_COUNT];

/// Number of indices for 2D quad.
#define QUAD_2D_INDEX_COUNT (6)
/// Size of quad 2d index buffer.
#define QUAD_2D_INDEX_BUFFER_SIZE\
    (QUAD_2D_INDEX_COUNT * sizeof(u8))
/// 2D Quad indices.
extern u8 QUAD_2D_INDICES[QUAD_2D_INDEX_COUNT];

#endif // header guard
