/**
 * Description:  Blinn-Phong BRDF Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 12, 2023
*/
#include "defines.include"
#include "camera.include"
#include "lights.include"
in layout(location = 0) vec3 v_position;
in layout(location = 1) vec2 v_uv;
in layout(location = 2) vec3 v_normal;
in layout(location = 3) vec4 v_color;
in layout(location = 4) vec3 v_tangent;

uniform layout(location = 0) mat4 u_transform  = mat4(1.0);
uniform layout(location = 1) mat3 u_normal_mat = mat3(1.0);

out layout(location = 0) struct Vert2Frag {
    vec3 local_position;
    vec3 world_position;
    vec4 light_space_position;
    vec2 uv;
    vec3 world_normal;
    vec3 world_tangent;
    vec3 world_bitangent;
    vec4 color;
} v2f;

void main() {
    vec4 world_position = u_transform * vec4( v_position, 1.0 );
    v2f.local_position  = v_position;
    v2f.world_position  = world_position.xyz;
    v2f.light_space_position = DIRECTIONAL.MATRIX * world_position;

    v2f.uv              = v_uv;
    v2f.world_normal    = u_normal_mat * v_normal;
    v2f.world_tangent   = u_normal_mat * v_tangent;
    v2f.world_bitangent = cross( v2f.world_tangent, v2f.world_normal );
    v2f.color           = v_color;

    gl_Position = VIEW_PROJECTION_3D * world_position;
}

