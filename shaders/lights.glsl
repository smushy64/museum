#if !defined(GLSL_UBO_LIGHTS)
#define GLSL_UBO_LIGHTS
/**
 * Description:  Lights
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 14, 2023
*/

// size: 96
struct Directional {
    // offset: 0 size: 16
    // x, y, z - color
    // w - is_active
    vec4 COLOR;
    // offset: 16 size: 16
    vec4 DIRECTION;
    // offset: 32 size: 64
    mat4 MATRIX;
};

#define POINT_LIGHT_MATRIX_COUNT (6)
// size: 432
struct Point {
    // offset: 0 size: 16
    vec4 COLOR;
    // offset: 16  size: 16
    vec4 POSITION;
    // offset: 32 size: 384
    mat4 MATRIX[POINT_LIGHT_MATRIX_COUNT];
    // offset: 416 size: 16
    // x - is active
    // y - near clip
    // z - far clip
    // w - padding
    vec4 POINT_DATA;
};

#define POINT_LIGHT_COUNT (4)
// size: 1824
layout(std140, binding = 1) uniform Lights {
    // offset: 0 size: 96
    Directional DIRECTIONAL;
    // offset: 96 size: 1728
    Point POINT[POINT_LIGHT_COUNT];
};

#if defined(GLSL_FRAGMENT) && !defined(NO_SHADOW)
    uniform layout(binding = 10) sampler2D        u_shadow_map;
    uniform layout(binding = 11) samplerCubeArray u_shadow_map_point[4];
#endif

const int POINT_SHADOW_SAMPLE_OFFSET_DIRECTION_COUNT = 20;
const vec3 POINT_SHADOW_SAMPLE_OFFSET_DIRECTIONS[] = vec3[](
    vec3(  1.0,  1.0,  1.0 ), vec3(  1.0, -1.0,  1.0 ), vec3( -1.0, -1.0,  1.0 ), vec3( -1.0,  1.0,  1.0 ),
    vec3(  1.0,  1.0, -1.0 ), vec3(  1.0, -1.0, -1.0 ), vec3( -1.0, -1.0, -1.0 ), vec3( -1.0,  1.0, -1.0 ),
    vec3(  1.0,  1.0,  0.0 ), vec3(  1.0, -1.0,  0.0 ), vec3( -1.0, -1.0,  0.0 ), vec3( -1.0,  1.0,  0.0 ),
    vec3(  1.0,  0.0,  1.0 ), vec3( -1.0,  0.0,  1.0 ), vec3(  1.0,  0.0, -1.0 ), vec3( -1.0,  0.0, -1.0 ),
    vec3(  0.0,  1.0,  1.0 ), vec3(  0.0, -1.0,  1.0 ), vec3(  0.0, -1.0, -1.0 ), vec3(  0.0,  1.0, -1.0 )
);

#endif

