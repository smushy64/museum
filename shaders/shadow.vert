/**
 * Description:  Shadow Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 14, 2023
*/
#include "defines.include"

in layout(location = 0) vec3 v_position;
in layout(location = 1) vec2 v_uv;
in layout(location = 2) vec3 v_normal;
in layout(location = 3) vec4 v_color;
in layout(location = 4) vec3 v_tangent;

uniform layout(location = 0) mat4 u_transform = mat4(1.0);
uniform layout(location = 1) mat4 u_vp        = mat4(1.0);

void main() {
    gl_Position = u_vp * u_transform * vec4( v_position, 1.0 );
}

