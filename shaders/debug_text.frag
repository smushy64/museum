/**
 * Description:  Debug Draw Text Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: August 06, 2023
*/
#include "defines.include"
in layout(location = 0) struct Vert2Frag {
    vec2 uv;
} v2f;

uniform layout(binding  = 0) sampler2D u_texture;
uniform layout(location = 1) vec4      u_color;
uniform layout(location = 2) vec4      u_coordinates;

void main() {
    vec2 coordinate = vec2(
        mix( u_coordinates.x, u_coordinates.x + u_coordinates.z, v2f.uv.x ),
        mix( u_coordinates.y, u_coordinates.y + u_coordinates.w, v2f.uv.y )
    );
    float alpha = texture( u_texture, coordinate ).r;
    FRAG_COLOR = vec4( u_color.rgb, u_color.a * alpha );
}

