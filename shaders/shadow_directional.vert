/**
 * Description:  Shadow Directional Vertex Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 14, 2023
*/
#include "defines.glsl"
#include "lights.glsl"
#include "transform.glsl"
in layout(location = 0) vec3 v_position;

void main() {
    gl_Position = DIRECTIONAL.MATRIX * transform_matrix() * vec4( v_position, 1.0 );
}

