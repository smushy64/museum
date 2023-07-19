/**
 * Description:  Sprite Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 15, 2023
*/
#include "defines.include"
in layout(location = 0) struct Vert2Frag {
    vec2 uv;
} v2f;

uniform layout(binding  = 0) sampler2D u_atlas;
uniform layout(location = 5) vec4      u_tint = vec4(1.0);

void main() {
    vec4 tex_sample = texture( u_atlas, v2f.uv );
    FRAG_COLOR = tex_sample * u_tint;
}

