#if !defined(GLSL_DEFINES)
#define GLSL_DEFINES
/**
 * Description:  Common things to include in shaders
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: July 16, 2023
*/
#version 450 core
#if GLSL_FRAGMENT
    out layout(location = 0) vec4 FRAG_COLOR;
#endif

vec3 linear_to_srgb( vec3 linear ) {
    return pow( linear, vec3( 1.0 / 2.2 ) );
}

vec3 srgb_to_linear( vec3 srgb ) {
    return pow( srgb, vec3( 2.2 ) );
}

layout(std140, binding = 2) uniform Data {
    // x - elapsed time
    // y - delta time
    // z - frame count
    // w - ???
    vec4 TIME;
    // x - resolution
    // y - resolution
    // z - aspect ratio
    // w - ???
    vec4 SURFACE_RESOLUTION;
};

float elapsed_time() {
    return TIME.x;
}
float delta_time() {
    return TIME.y;
}
uint frame_count() {
    return floatBitsToUint( TIME.z );
}

vec2 surface_resolution() {
    return SURFACE_RESOLUTION.xy;
}
float surface_aspect_ratio() {
    return SURFACE_RESOLUTION.z;
}

float inv_lerp( float a, float b, float v ) {
    return ( v - a ) / ( b - a );
}

#endif
