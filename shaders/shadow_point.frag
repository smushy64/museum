/**
 * Description:  Shadow Point Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 15, 2023
*/
#version 450 core
#define NO_SHADOW 1
#include "lights.include"
in layout(location = 0) vec4 FRAG_POS;

void main() {
    float light_distance = length( FRAG_POS.xyz - POINT[0].POSITION.xyz );
    light_distance       = light_distance / POINT[0].POINT_DATA.z;
    gl_FragDepth         = light_distance;
}

