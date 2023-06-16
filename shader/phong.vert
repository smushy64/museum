/**
 * Description:  Phong Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#version 450 core
in layout(location = 0) vec2 v_position;
in layout(location = 1) vec3 v_color;

layout( std140, binding = 0 ) uniform matrices {
    mat4 u_view_projection;
};

out layout( location = 0 ) struct Vert2Frag {
    vec3 color;
} v2f;

void main() {
    gl_Position = u_view_projection * vec4(v_position, 0.0, 1.0);
    v2f.color   = v_color;
}

// in layout(location = 0) vec4 v_position;
// in layout(location = 1) vec3 v_color;
// in layout(location = 2) vec2 v_uv;
// in layout(location = 3) vec3 v_normal;

// layout(std140, binding = 0) uniform matrices {
//     mat4 u_view_projection;
// };

// uniform layout(location = 0) mat4 u_transform;

// out layout(location = 0) struct Vert2Frag {
//     vec3 color;
//     vec2 uv;
// } v2f;

// void main() {
//     gl_Position = u_transform * u_view_projection * v_position;
//     v2f.color   = v_color;
//     v2f.uv      = v_uv;
// }
