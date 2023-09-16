/**
 * Description:  Shadow Point Geometry Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 15, 2023
*/
#version 450 core
#include "lights.include"
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out layout(location = 0) vec4 FRAG_POS;
void main() {
    for( int face = 0; face < 6; ++face ) {
        gl_Layer = face;
        for( int i = 0; i < 3; ++i ) {
            FRAG_POS = gl_in[i].gl_Position;
            gl_Position = POINT[0].MATRIX[face] * FRAG_POS;
            EmitVertex();
        }
        EndPrimitive();
    }
}
