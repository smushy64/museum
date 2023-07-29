/**
 * Description:  Debug Draw Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 16, 2023
*/
#include "defines.include"
#include "matrix.include"
in layout(location = 0) vec3 v_position;

void main() {
    gl_Position = VIEW_PROJECTION * vec4( v_position, 1.0 );
}

