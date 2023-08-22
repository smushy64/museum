// * Description:  Graphics primitives implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "core/ldgraphics/primitives.h"

struct Vertex2D QUAD_2D_CENTERED[] = {
    { { -0.5f,  0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f }, { 1.0f, 0.0f } }
};

struct Vertex2D QUAD_2D_LOWER_LEFT[] = {
    { {  0.0f,  1.0f }, { 0.0f, 1.0f } },
    { {  1.0f,  1.0f }, { 1.0f, 1.0f } },
    { {  0.0f,  0.0f }, { 0.0f, 0.0f } },
    { {  1.0f,  0.0f }, { 1.0f, 0.0f } }
};

u8 QUAD_2D_INDICES[QUAD_2D_INDEX_COUNT] = {
    0, 1, 2,
    2, 3, 0
};

