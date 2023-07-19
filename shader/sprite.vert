/**
 * Description:  Sprite Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 15, 2023
*/
#include "defines.include"
#include "matrix.include"

in layout(location = 0) vec2 v_position;
in layout(location = 1) vec2 v_uv;

uniform layout(location = 0) mat4 u_transform = mat4(1.0);
// x,y - cell in atlas
// z,w - scale in cells (1, 1) == just one cell
uniform layout(location = 1) ivec4 u_atlas_coordinate;
uniform layout(location = 2) ivec2 u_flip = ivec2(0, 0);
uniform layout(location = 3) uint  u_atlas_cell_size = 32;
uniform layout(location = 4) int   u_z_index = 0;

out layout(location = 0) struct Vert2Frag {
    vec2 uv;
} v2f;

void main() {
    vec4 position = vec4( v_position, float(u_z_index), 1.0 );
    gl_Position = VIEW_PROJECTION * u_transform * position;

    // vec4 scaled_atlas_coordinate = vec4( u_atlas_coordinate );
    vec4 scaled_atlas_coordinate =
        vec4(u_atlas_coordinate) / float(u_atlas_cell_size);
    vec2 uv = v_uv;
    uv.x = abs( u_flip.x - uv.x );
    uv.y = abs( u_flip.y - uv.y );

    v2f.uv = vec2(
        mix(
            scaled_atlas_coordinate.x,
            scaled_atlas_coordinate.x + scaled_atlas_coordinate.z,
            uv.x
        ),
        mix(
            scaled_atlas_coordinate.y,
            scaled_atlas_coordinate.y + scaled_atlas_coordinate.w,
            uv.y
        )
    );
}
