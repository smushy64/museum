/**
 * Description:  Color Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: August 26, 2023
*/
#include "defines.include"
uniform layout(location = 1) vec4 u_color = vec4(1.0);

in layout(location = 0) struct Vert2Frag {
    vec2 uv;
} v2f;

void main() {
    FRAG_COLOR = u_color;
}
