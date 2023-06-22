/**
 * Description:  Phong Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#version 450 core
#include "matrix.include"

in layout(location = 0) vec2 v_position;
in layout(location = 1) vec3 v_color;

out layout( location = 0 ) struct Vert2Frag {
    vec3 color;
} v2f;

void main() {
    gl_Position = VIEW_PROJECTION * vec4(v_position, 0.0, 1.0);
    v2f.color   = v_color;
}

