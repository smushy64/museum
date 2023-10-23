#if !defined(GLSL_CONSTANTS)
#define GLSL_CONSTANTS
/**
 * Description:  Constants to include in shaders
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 13, 2023
*/

const float PI      = (3.141592741);
const float TAU     = (PI * 2.0);
const float HALF_PI = (PI / 2.0);

const float TO_DEG = (180.0 / PI);
const float TO_RAD = (PI / 180.0);

const float AMBIENT_DIRECTIONAL_CONTRIBUTION = 0.01;
const float AMBIENT_POINT_CONTRIBUTION       = 0.1;
const float SPECULAR_CUTOFF                  = 0.2;
const float SPECULAR_POWER                   = 80.0;

#endif