/**
 * Description:  Debug Draw Image Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: August 08, 2023
*/
#include "defines.include"
in layout(location = 0) struct Vert2Frag {
    vec2 uv;
} v2f;

uniform layout(binding  = 0) sampler2D u_texture;
uniform layout(location = 1) vec4      u_tint;

void main() {
    vec4 tex_sample = texture( u_texture, v2f.uv );
    vec4 color = tex_sample * u_tint;
    FRAG_COLOR = color;
}

