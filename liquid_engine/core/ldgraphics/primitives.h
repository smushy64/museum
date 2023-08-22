#if !defined(LD_CORE_GRAPHICS_PRIMITIVES_HPP)
#define LD_CORE_GRAPHICS_PRIMITIVES_HPP
// * Description:  Graphics primitives
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/ldgraphics/types.h"

/// Number of vertices for 2D quad.
#define QUAD_2D_VERTEX_COUNT (4)
/// 2D Quad with origin in center.
extern struct Vertex2D QUAD_2D_CENTERED[QUAD_2D_VERTEX_COUNT];
/// 2D Quad with origin in lower left corner.
extern struct Vertex2D QUAD_2D_LOWER_LEFT[QUAD_2D_VERTEX_COUNT];

/// Number of indices for 2D quad.
#define QUAD_2D_INDEX_COUNT (6)
/// 2D Quad indices.
extern u8 QUAD_2D_INDICES[QUAD_2D_INDEX_COUNT];

#endif // header guard
