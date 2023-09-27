#if !defined(GLSL_TRANSFORM)
#define GLSL_TRANSFORM
/*
 * Description:  Transform Matrix
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 26, 2023
*/

layout(std140, binding = 3) uniform Transform {
    mat4 TRANSFORM;
    mat4 _NORMAL_;
};

mat4 transform_matrix() {
    return TRANSFORM;
}
mat3 normal_matrix() {
    return mat3( _NORMAL_ );
}

#endif

