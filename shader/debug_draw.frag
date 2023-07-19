/**
 * Description:  Debug Draw Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 16, 2023
*/
#include "defines.include"
uniform layout(location = 0) vec4 u_color = vec4(1.0);

void main() {
    FRAG_COLOR = u_color;
}

