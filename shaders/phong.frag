/**
 * Description:  Blinn-Phong BRDF Fragment Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 12, 2023
*/
#include "defines.include"
#include "camera.include"
#include "lights.include"
#include "constants.include"

in layout(location = 0) struct Vert2Frag {
    vec3 local_position;
    vec3 world_position;
    vec4 light_space_position;
    vec2 uv;
    vec3 world_normal;
    vec3 world_tangent;
    vec3 world_bitangent;
    vec4 color;
} v2f;

uniform layout(binding = 0) sampler2D u_diffuse;
uniform layout(binding = 1) sampler2D u_normal;
uniform layout(binding = 2) sampler2D u_roughness;
uniform layout(binding = 3) sampler2D u_metallic;

uniform layout(location = 2) vec3 u_tint = vec3(1.0);

struct LightResult {
    vec3 light;
    vec3 ambient;
};

LightResult directional_light(
    vec4 light_space_position,
    vec3 diffuse,
    vec3 normal,
    float roughness,
    vec3 camera_direction
);
float directional_shadow( vec4 light_space_position );

LightResult point_light(
    vec3  world_position,
    vec3  point_position,
    vec3  point_color,
    float is_active,
    float far_clip,
    int   index,
    vec3  diffuse,
    vec3  normal,
    float roughness,
    vec3  camera_direction
);
float point_shadow(
    vec3 world_position, vec3 point_position, float far_clip, int index );

void main() {
    vec3 final_color = vec3(0.0);

    vec3 tint           = srgb_to_linear( u_tint );
    vec3 vertex_color   = srgb_to_linear( v2f.color.rgb );
    vec3 diffuse_sample = srgb_to_linear( texture( u_diffuse, v2f.uv ).rgb );

    vec3 diffuse = tint * vertex_color * diffuse_sample;
    vec3 ambient = vec3(0.0);

    vec3 normal    = normalize( v2f.world_normal );
    vec3 tangent   = normalize( v2f.world_tangent );
    vec3 bitangent = normalize( v2f.world_bitangent );

    vec3 normal_sample = (texture( u_normal, v2f.uv ).rgb * 2.0) - vec3(1.0);
    mat3 normal_mat = mat3( tangent, bitangent, normal );
    normal = normalize( normal_mat * normal_sample );
    
    float roughness = texture( u_roughness, v2f.uv ).r;

    vec3 camera_direction = CAMERA_WORLD_POSITION.xyz - v2f.world_position;
    float camera_distance = length( camera_direction );
    camera_direction      = normalize( camera_direction );

    LightResult directional = directional_light(
        v2f.light_space_position,
        diffuse, normal, roughness,
        camera_direction );
    ambient += directional.ambient * AMBIENT_DIRECTIONAL_CONTRIBUTION;

    vec3 point_accumulation = vec3(0.0);
    for( int i = 0; i < 4; ++i ) {
        LightResult point = point_light(
            v2f.world_position,
            POINT[i].POSITION.xyz,
            srgb_to_linear( POINT[i].COLOR.rgb ),
            POINT[i].POINT_DATA.x,
            POINT[i].POINT_DATA.z,
            i,
            diffuse,
            normal,
            roughness,
            camera_direction );
        point_accumulation += point.light;
        ambient += point.ambient * AMBIENT_POINT_CONTRIBUTION;
    }

    final_color = directional.light + point_accumulation + ambient;
    final_color = linear_to_srgb( final_color );
    FRAG_COLOR  = vec4( final_color, 1.0 );
}

LightResult directional_light(
    vec4 light_space_position,
    vec3 diffuse_sample,
    vec3 normal,
    float roughness,
    vec3 camera_direction
) {
    LightResult result;

    vec3 light_direction = normalize( vec3( -DIRECTIONAL.DIRECTION ) );
    vec3 light_color     = srgb_to_linear( vec3( DIRECTIONAL.COLOR ) );
    float light_strength = length( light_color );

    float diffuse_contribution = dot( light_direction, normal );
    diffuse_contribution = max( diffuse_contribution, 0.0 );

    vec3 diffuse   = diffuse_sample * light_color;
    result.ambient = diffuse;
    diffuse *= diffuse_contribution;

    vec3 reflect_direction = reflect( -light_direction, normal );
    vec3 halfway_direction = normalize( light_direction + camera_direction );

    float specular_contribution = pow(
        max( dot( normal, halfway_direction ), 0.0 ),
        SPECULAR_POWER / 2.0 );
    vec3 specular = vec3( (1.0 - roughness) * specular_contribution );
    specular *= diffuse_contribution * light_strength;

    float shadow_mask = directional_shadow( light_space_position ) * 0.8;

    result.light = (diffuse + specular) * (1.0 - shadow_mask);
    return result;
}

LightResult point_light(
    vec3  world_position,
    vec3  light_position,
    vec3  srgb_light_color,
    float is_active,
    float far_clip,
    int   index,
    vec3  diffuse_sample,
    vec3  normal,
    float roughness,
    vec3  camera_direction
) {
    LightResult result;

    vec3 light_color = srgb_to_linear( srgb_light_color );

    vec3 light_direction = light_position - world_position;
    float light_distance = length( light_direction );
    light_direction      = normalize( light_direction );

    float light_strength = length( light_color );

    float diffuse_contribution = dot( light_direction, normal );
    diffuse_contribution = max( diffuse_contribution, 0.0 );

    vec3 base_diffuse = diffuse_sample * light_color;
    vec3 diffuse      = base_diffuse * diffuse_contribution;

    vec3 reflect_direction = reflect( -light_direction, normal );
    vec3 halfway_direction = normalize( light_direction + camera_direction );

    float specular_contribution = pow(
        max( dot( normal, halfway_direction ), 0.0 ),
        SPECULAR_POWER );

    vec3 specular = vec3( (1.0 - roughness) * specular_contribution );
    specular *= diffuse_contribution * light_strength;

    float d  = light_distance;
    float d2 = light_distance * light_distance;

    float attenuation = 1.0 / ( 1.0 + ( 0.3 * d ) + ( 0.1 * d2 ) );
    attenuation = clamp( attenuation, 0.0, 1.0 );

    float shadow_mask =
        point_shadow( world_position, light_position, far_clip, index );
    shadow_mask *= attenuation;

    result.ambient = is_active * ( base_diffuse * attenuation * 1.4 );
    result.light   =
        is_active * ( ( 1.0 - shadow_mask ) * ( diffuse + specular ) );

    return result;
}

float directional_shadow( vec4 light_space_position ) {
    vec3 projection_coordinates =
        light_space_position.xyz / light_space_position.w;

    projection_coordinates = projection_coordinates * 0.5 + 0.5;

    const float bias = 0.005;

    float shadow_mask = 0.0f;
    float current_depth = projection_coordinates.z;

    vec2 texel_size = 1.0 / textureSize( u_shadow_map, 0 );
    for( int x = -1; x <= 1; ++x ) {
        for( int y = -1; y <= 1; ++y ) {
            float pcf_depth = texture(
                u_shadow_map,
                projection_coordinates.xy + (vec2(x, y) * texel_size) ).r;
            shadow_mask += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }

    if( projection_coordinates.z > 1.0 ) {
        shadow_mask = 0.0;
    }
    
    return ( shadow_mask / 9.0 );
}

float point_shadow(
    vec3 world_position, vec3 point_position, float far_clip, int index
) {
    vec3 to_light = world_position - point_position;
    float current_depth = length( to_light );

    float shadow_mask = 0.0;
    float bias        = 0.15;
    float view_distance = length( CAMERA_WORLD_POSITION.xyz - world_position );
    float disk_radius   = 0.05;
    for( int i = 0; i < POINT_SHADOW_SAMPLE_OFFSET_DIRECTION_COUNT; ++i ) {
        vec3 offset       = POINT_SHADOW_SAMPLE_OFFSET_DIRECTIONS[i];
        vec4 sample_point = vec4(to_light + offset * disk_radius, 0.0);
        float closest_depth =
            texture( u_shadow_map_point[index], sample_point ).r;
        closest_depth *= far_clip;

        shadow_mask += current_depth - bias > closest_depth ? 1.0 : 0.0;
    }

    shadow_mask /= float( POINT_SHADOW_SAMPLE_OFFSET_DIRECTION_COUNT );

    return shadow_mask;
}

