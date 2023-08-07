#if !defined(RENDERER_PRIMITIVES_HPP)
#define RENDERER_PRIMITIVES_HPP
// * Description:  Renderer Primitives
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 16, 2023
#include "defines.h"
#include "core/graphics.h"

/// Quad 2D vertices
global Vertex2D FONT_QUAD_2D[] = {
    { {  0.0f,  1.0f }, { 0.0f, 1.0f } },
    { {  1.0f,  1.0f }, { 1.0f, 1.0f } },
    { {  0.0f,  0.0f }, { 0.0f, 0.0f } },
    { {  1.0f,  0.0f }, { 1.0f, 0.0f } }
};
/// Quad 2D indices
global u8 FONT_QUAD_2D_INDICES[] = {
    2, 1, 0,
    1, 2, 3
};

/// Quad 2D vertices
global Vertex2D QUAD_2D[] = {
    { {  1.0f,  1.0f }, { 1.0f, 1.0f } },
    { { -1.0f,  1.0f }, { 0.0f, 1.0f } },
    { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f }, { 1.0f, 0.0f } }
};
/// Quad 2D indices
global u8 QUAD_2D_INDICES[] = {
    0, 1, 2,
    2, 3, 0
};

#endif // header guard
