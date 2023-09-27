/**
 * Description:  Shadow Point Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 15, 2023
*/
#version 450 core
#define NO_SHADOW 1
#include "lights.glsl"

in layout(location = 0) vec4 g2f_frag_pos;
in layout(location = 1) flat int g2f_point_index;

void main() {
    float light_distance =
        length( g2f_frag_pos.xyz - POINT[g2f_point_index].POSITION.xyz );
    light_distance = light_distance / POINT[g2f_point_index].POINT_DATA.z;
    gl_FragDepth   = light_distance;
}

