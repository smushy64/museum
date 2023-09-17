/**
 * Description:  Shadow Point Geometry Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 15, 2023
*/
#version 450 core
#include "lights.include"
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform layout(location = 1) int u_point_index = 0;

out layout(location = 0) vec4 g2f_frag_pos;
out layout(location = 1) flat int g2f_point_index;
void main() {
    g2f_point_index = u_point_index;
    for( int face = 0; face < 6; ++face ) {
        gl_Layer = face;
        for( int i = 0; i < 3; ++i ) {
            g2f_frag_pos = gl_in[i].gl_Position;
            gl_Position = POINT[u_point_index].MATRIX[face] * g2f_frag_pos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
