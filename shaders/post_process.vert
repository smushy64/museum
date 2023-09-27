/**
 * Description:  Post-Process Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 17, 2023
*/
#include "defines.glsl"
in layout(location = 0) vec2 v_vertex;
in layout(location = 1) vec2 v_uv;

out layout(location = 0) vec2 v2f_uv;
void main() {
    gl_Position = vec4( v_vertex, 0.0, 1.0 );
    v2f_uv      = v_uv;
}

