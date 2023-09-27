/**
 * Description:  Shadow Point Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 15, 2023
*/
#version 450 core
#include "transform.glsl"
in layout(location = 0) vec3 v_position;

void main() {
    gl_Position = transform_matrix() * vec4( v_position, 1.0 );
}
