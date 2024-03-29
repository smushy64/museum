// * Description:  Graphics primitives implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "engine/graphics/primitives.h"

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

struct Vertex3D CUBE_3D[CUBE_3D_VERTEX_COUNT] = {
    // back face
    { { -0.5f, -0.5f, -0.5f, }, {  0.0f,  0.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f, }, {  0.0f,  0.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f, }, {  0.0f,  0.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f, }, {  0.0f,  0.0f, -1.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

    // front face
    { { -0.5f, -0.5f,  0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f, }, {  0.0f,  0.0f,  1.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

    // left face
    { { -0.5f, -0.5f, -0.5f, }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f, }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f, }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f, }, { -1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

    // right face
    { {  0.5f, -0.5f, -0.5f, }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f, }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f, }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f, }, {  1.0f,  0.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

    // bottom face
    { { -0.5f, -0.5f, -0.5f, }, {  0.0f, -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f, }, {  0.0f, -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f, }, {  0.0f, -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f, }, {  0.0f, -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },

    // top face
    { { -0.5f,  0.5f, -0.5f, }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f, }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f, }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f, }, {  0.0f,  1.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
};

u8 CUBE_3D_INDICES[CUBE_3D_INDEX_COUNT] = {
    // back face
    0, 1, 2,
    0, 2, 3,

    // front face
    4, 6, 5,
    4, 7, 6,

    // left face
    8,  9, 10,
    8, 10, 11,

    // right face
    12, 14, 13,
    12, 15, 14,

    // bottom face
    16, 18, 17,
    16, 19, 18,

    // top face
    20, 21, 22,
    20, 22, 23,
};

// u32 NULL_DIFFUSE_TEXTURE[NULL_DIFFUSE_TEXTURE_PIXEL_COUNT] = {
//     rgb_u32( 255, 255, 255 ), rgb_u32( 255, 255, 255 ), rgb_u32( 255, 255, 255 ),
//     rgb_u32( 255, 255, 255 ), rgb_u32( 255, 255, 255 ), rgb_u32( 255, 255, 255 ),
//     rgb_u32( 255, 255, 255 ), rgb_u32( 255, 255, 255 ), rgb_u32( 255, 255, 255 ),
// };
u32 NULL_DIFFUSE_TEXTURE[NULL_DIFFUSE_TEXTURE_PIXEL_COUNT] = {
    rgb_u32( 255,   0, 255 ), rgb_u32(   0,   0,   0 ), rgb_u32( 255,   0, 255 ),
    rgb_u32(   0,   0,   0 ), rgb_u32( 255,   0, 255 ), rgb_u32(   0,   0,   0 ),
    rgb_u32( 255,   0, 255 ), rgb_u32(   0,   0,   0 ), rgb_u32( 255,   0, 255 ),
};
u8 NULL_NORMAL_TEXTURE[3] = { 127, 127, 255 };

// u8 NULL_ROUGHNESS_TEXTURE[NULL_ROUGHNESS_TEXTURE_PIXEL_COUNT] = { 200 };
u8 NULL_ROUGHNESS_TEXTURE[NULL_ROUGHNESS_TEXTURE_PIXEL_COUNT] = { 0 };

u8 NULL_METALLIC_TEXTURE[NULL_METALLIC_TEXTURE_PIXEL_COUNT] = { 0 };

