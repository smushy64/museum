/**
 * Description:  Shadow Point Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 15, 2023
*/
#version 450 core
in layout(location = 0) vec3 v_position;

uniform layout(location = 0) mat4 u_transform = mat4(1.0);

void main() {
    gl_Position = u_transform * vec4( v_position, 1.0 );
}
