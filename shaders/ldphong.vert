/**
 * Description:  Phong Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "defines.include"
#include "matrix.include"

in layout(location = 0) vec2 v_position;
in layout(location = 1) vec2 v_uv;

uniform layout(location = 0) mat4 u_transform = mat4(1.0);

out layout( location = 0 ) struct Vert2Frag {
    vec2 uv;
} v2f;

void main() {
    gl_Position =
        VIEW_PROJECTION_3D * u_transform * vec4(v_position, 0.0, 1.0);
    v2f.uv = v_uv;
}

