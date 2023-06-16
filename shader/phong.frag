/**
 * Description:  Phong Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#version 450 core
in layout( location = 0 ) struct Vert2Frag {
    vec3 color;
} v2f;

out layout( location = 0 ) vec4 FRAG_COLOR;
void main() {
    FRAG_COLOR = vec4( v2f.color, 1.0 );
}

// in layout(location = 0) struct Vert2Frag {
//     vec3 color;
//     vec2 uv;
// } v2f;

// out layout(location = 0) vec4 FRAG_COLOR;
// void main() {
//     FRAG_COLOR = vec4(v2f.color, 1.0);
// }
