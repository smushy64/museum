/**
 * Description:  Phong BRDF Fragment Shader
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

vec3 directional_light(
    vec3 diffuse, vec3 normal,
    float roughness,
    vec3 camera_direction
);

float directional_shadow( vec4 light_space_position );

vec3 point_light(
    vec3 point_position,
    vec3 point_color,
    float is_active,
    vec3 diffuse,
    vec3 normal,
    float roughness,
    vec3 camera_direction
);

void main() {
    vec3 final_color = vec3(0.0);

    vec3 diffuse = u_tint * texture( u_diffuse, v2f.uv ).rgb * v2f.color.rgb;
    diffuse = pow( diffuse, vec3(2.2) );
    vec3 ambient = (diffuse * 0.1) + AMBIENT;

    vec3 normal    = normalize( v2f.world_normal );
    vec3 tangent   = normalize( v2f.world_tangent );
    vec3 bitangent = normalize( v2f.world_bitangent );

    vec3 normal_sample = (texture( u_normal, v2f.uv ).rgb * 2.0) - vec3(1.0);
    mat3 normal_mat = mat3( tangent, bitangent, normal );
    normal = normalize( normal_mat * normal_sample );
    
    float roughness = texture( u_roughness, v2f.uv ).r;

    // NOTE(alicia): negating camera world position to
    // match fragment world space. 
    vec3 camera_direction = -CAMERA_WORLD_POSITION - v2f.world_position;
    float camera_distance = length( camera_direction );
    camera_direction      = normalize( camera_direction );

    vec3 directional =
        directional_light( diffuse, normal, roughness, camera_direction );
    directional *= directional_shadow( v2f.light_space_position );

    vec3 point_accumulation = vec3(0.0);
    for( int i = 0; i < POINT_LIGHT_COUNT; ++i ) {
        vec3 point = point_light(
            vec3(POINT[i].POSITION),
            vec3(POINT[i].COLOR),
            POINT[i].IS_ACTIVE.x,
            diffuse,
            normal,
            roughness,
            camera_direction );
        point_accumulation += point;
    }

    final_color = directional + point_accumulation + ambient;
    final_color = pow( final_color, vec3(1.0 / 2.2) );
    FRAG_COLOR  = vec4( final_color, 1.0 );
}

vec3 directional_light(
    vec3 diffuse, vec3 normal,
    float roughness,
    vec3 camera_direction
) {
    vec3 light_direction = normalize( vec3( -DIRECTIONAL.DIRECTION ) );
    vec3 light_color     = vec3( DIRECTIONAL.COLOR );

    float light_contribution = dot( normal, light_direction );
    float specular_mask      = step( 0.0, light_contribution );
    light_contribution       = max( light_contribution, 0.0 );

    vec3 reflection_direction = reflect( -light_direction, normal );

    float specular_power = pow(
        max( dot( camera_direction, reflection_direction ), 0.0 ),
        SPECULAR_POWER / 4.0 );

    vec3 specular = vec3(
        ( 1.0 - roughness ) * specular_power * specular_mask );

    return ((diffuse * light_color) * light_contribution) + specular;
}

vec3 point_light(
    vec3 point_position,
    vec3 point_color,
    float is_active,
    vec3 diffuse,
    vec3 normal,
    float roughness,
    vec3 camera_direction
) {
    vec3 light_direction = point_position - v2f.world_position;
    float light_distance = length( light_direction );
    light_direction      = normalize( light_direction );
    float d  = light_distance;
    float d2 = light_distance * light_distance;
    float attenuation = 1.0 / ( 1.0 + ( 0.3 * d ) + ( 0.1 * d2 ) );
    attenuation = clamp( attenuation, 0.0, 1.0 );

    float light_contribution = dot( normal, light_direction );
    float specular_mask      = step( 0.0, light_contribution );
    light_contribution       = max( light_contribution, 0.0 );

    vec3 reflection_direction = reflect( -light_direction, normal );

    float specular_power = pow(
        max( dot( camera_direction, reflection_direction ), 0.0 ),
        SPECULAR_POWER );

    float specular_contribution =
        ( 1.0 - roughness ) * specular_power * specular_mask;

    vec3 diffuse_color       = diffuse * point_color;
    vec3 diffuse_contibution = diffuse_color * light_contribution * attenuation;
    vec3 specular = vec3(specular_contribution) * attenuation;
    vec3 ambient  = attenuation * attenuation * point_color;

    return is_active * ( ambient + specular + diffuse_contibution);
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
    
    return (1.0 - (shadow_mask / 9.0));
}

