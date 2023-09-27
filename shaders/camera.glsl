#if !defined(GLSL_UBO_CAMERA)
#define GLSL_UBO_CAMERA
/**
 * Description:  Matrix shader includes
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 20, 2023
*/
#include "constants.glsl"

layout(std140, binding = 0) uniform Camera {
    mat4 VIEW_PROJECTION_UI;
    mat4 VIEW_PROJECTION_3D;
    vec4 CAMERA_WORLD_POSITION;
    // x - near clip
    // y - far clip
    // z - aspect ratio
    // w - field of view (radians)
    vec4 CAMERA_DATA;
};

mat4 camera_view_projection() {
    return VIEW_PROJECTION_3D;
}
mat4 ui_view_projection() {
    return VIEW_PROJECTION_UI;
}

float camera_near_clip() {
    return CAMERA_DATA.x;
}
float camera_far_clip() {
    return CAMERA_DATA.y;
}
vec2 camera_clipping_planes() {
    return vec2( CAMERA_DATA.x, CAMERA_DATA.y );
}

float camera_aspect_ratio() {
    return CAMERA_DATA.z;
}
float camera_field_of_view_radians() {
    return CAMERA_DATA.w;
}
float camera_field_of_view_degrees() {
    return camera_field_of_view_radians() * TO_DEG;
}

vec3 camera_world_position() {
    return CAMERA_WORLD_POSITION.xyz;
}

#endif
