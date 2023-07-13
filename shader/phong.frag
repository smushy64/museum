/**
 * Description:  Phong Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#version 450 core
in layout( location = 0 ) struct Vert2Frag {
    vec2 uv;
} v2f;

uniform layout(binding = 0) sampler2D u_texture;

out layout( location = 0 ) vec4 FRAG_COLOR;
void main() {
    vec4 tex_sample = texture( u_texture, v2f.uv );
    FRAG_COLOR = vec4( tex_sample );
}

