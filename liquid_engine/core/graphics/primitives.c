// * Description:  Graphics primitives implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "core/graphics/primitives.h"

struct Vertex2D QUAD_2D_CENTERED[QUAD_2D_VERTEX_COUNT] = {
    { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f }, { 1.0f, 0.0f } }
};

struct Vertex2D QUAD_2D_LOWER_LEFT[QUAD_2D_VERTEX_COUNT] = {
    { { 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { 0.0f, 1.0f }, { 0.0f, 1.0f } },
    { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { { 1.0f, 0.0f }, { 1.0f, 0.0f } }
};

u8 QUAD_2D_INDICES[QUAD_2D_INDEX_COUNT] = {
    0, 2, 1,
    0, 3, 2
};

