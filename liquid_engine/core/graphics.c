// * Description:  Graphics Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 18, 2023
#include "core/graphics.h"
#include "renderer.h"

RenderData* RENDER_DATA = NULL;

LD_API void graphics_set_camera( struct Camera* camera ) {
    RENDER_DATA->camera = camera;
}

LD_API void graphics_set_directional_light(
    vec3 direction, vec3 color
) {
    // TODO(alicia): thread safety!
    assert(
        RENDER_DATA->command_count !=
        RENDER_DATA->command_capacity );

    RenderCommand command = {};
    command.type = RENDER_COMMAND_TYPE_SET_DIRECTIONAL_LIGHT;
    command.directional_light.direction = direction;
    command.directional_light.color     = color;

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] = command;
}

LD_API void graphics_set_point_light(
    u32 index, vec3 position, vec3 color, b32 is_active
) {
    // TODO(alicia): thread safety!
    assert(
        RENDER_DATA->command_count !=
        RENDER_DATA->command_capacity );

    RenderCommand command = {};
    command.type = RENDER_COMMAND_TYPE_SET_POINT_LIGHT;
    command.point_light.position  = position;
    command.point_light.color     = color;
    command.point_light.index     = index;
    command.point_light.is_active = is_active;

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] = command;

}

LD_API void graphics_draw(
    mat4     transform,
    RenderID mesh,
    RenderID texture_diffuse,
    RenderID texture_normal,
    RenderID texture_roughness,
    RenderID texture_metallic,
    rgb      tint,
    b32      is_transparent,
    b32      is_shadow_caster,
    b32      is_shadow_receiver,
    b32      is_wireframe
) {
    // TODO(alicia): thread safety!
    assert(
        RENDER_DATA->command_count !=
        RENDER_DATA->command_capacity );

    RenderCommand command = {};
    command.type                   = RENDER_COMMAND_TYPE_DRAW;
    command.draw.transform         = transform;
    command.draw.mesh              = mesh;
    command.draw.texture_diffuse   = texture_diffuse;
    command.draw.texture_normal    = texture_normal;
    command.draw.texture_roughness = texture_roughness;
    command.draw.texture_metallic  = texture_metallic;
    command.draw.tint              = tint;

    if( is_transparent ) {
        command.draw.flags |= DRAW_FLAG_TRANSPARENT;
    }
    if( is_shadow_caster ) {
        command.draw.flags |= DRAW_FLAG_SHADOW_CASTER;
    }
    if( is_shadow_receiver ) {
        command.draw.flags |= DRAW_FLAG_SHADOW_RECEIVER;
    }
    if( is_wireframe ) {
        command.draw.flags |= DRAW_FLAG_IS_WIREFRAME;
    }

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] = command;
}

global RenderID GRAPHICS_RUNNING_ID = 1;

LD_API RenderID graphics_generate_mesh(
    usize vertex_count, struct Vertex3D* vertices,
    usize index_count, u32* indices
) {
    // TODO(alicia): thread safety!!
    if(
        RENDER_DATA->command_count ==
        RENDER_DATA->command_capacity
    ) {
        return RENDER_ID_NULL;
    }

    RenderCommand command              = {};
    command.type                       = RENDER_COMMAND_TYPE_GENERATE_MESH;
    command.generate_mesh.id           = GRAPHICS_RUNNING_ID++;
    command.generate_mesh.vertex_count = vertex_count;
    command.generate_mesh.vertices     = vertices;
    command.generate_mesh.index_count  = index_count;
    command.generate_mesh.indices      = indices;

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] =
        command;

    RENDER_DATA->non_draw_command_present = true;
    return command.generate_mesh.id;
}
LD_API b32 graphics_retire_meshes( usize count, RenderID* meshes ) {
    // TODO(alicia): thread safety!!
    if(
        RENDER_DATA->command_count ==
        RENDER_DATA->command_capacity
    ) {
        return false;
    }

    RenderCommand command          = {};
    command.type                   = RENDER_COMMAND_TYPE_RETIRE_MESHES;
    command.retire_resources.count = count;
    command.retire_resources.ids   = meshes;

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] =
        command;

    RENDER_DATA->non_draw_command_present = true;
    return true;
}

LD_API RenderID graphics_generate_texture(
    GraphicsTextureType     type,
    GraphicsTextureFormat   format,
    GraphicsTextureBaseType base_type,
    GraphicsTextureWrap     wrap_mode_x,
    GraphicsTextureWrap     wrap_mode_y,
    GraphicsTextureWrap     wrap_mode_z,
    GraphicsTextureFilter   minification_filter,
    GraphicsTextureFilter   magnification_filter,
    u32 width, u32 height, u32 depth,
    usize buffer_size,
    void* buffer
) {
    // TODO(alicia): thread safety!
    if(
        RENDER_DATA->command_count ==
        RENDER_DATA->command_capacity
    ) {
        return RENDER_ID_NULL;
    }

#if defined(LD_ASSERTIONS)
    usize calculated_texture_buffer_size =
        graphics_calculate_texture_buffer_size(
        type, format, base_type, width, height, 0 );
    assert( buffer_size >= calculated_texture_buffer_size );
#endif

    RenderCommand command = {};

    command.type = RENDER_COMMAND_TYPE_GENERATE_TEXTURE;
    command.generate_texture.id                   = GRAPHICS_RUNNING_ID++;
    command.generate_texture.type                 = type;
    command.generate_texture.format               = format;
    command.generate_texture.base_type            = base_type;
    command.generate_texture.wrap_x               = wrap_mode_x;
    command.generate_texture.wrap_y               = wrap_mode_y;
    command.generate_texture.wrap_z               = wrap_mode_z;
    command.generate_texture.buffer               = buffer;
    command.generate_texture.minification_filter  = minification_filter;
    command.generate_texture.magnification_filter = magnification_filter;
    command.generate_texture.width                = width;
    command.generate_texture.height               = height;
    command.generate_texture.depth                = depth;
    command.generate_texture.buffer               = buffer;

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] =
        command;

    RENDER_DATA->non_draw_command_present = true;
    return command.generate_texture.id;
}
LD_API b32 graphics_retire_textures( usize count, RenderID* textures ) {
    // TODO(alicia): thread safety!!
    if(
        RENDER_DATA->command_count ==
        RENDER_DATA->command_capacity
    ) {
        return false;
    }

    RenderCommand command = {};
    command.type = RENDER_COMMAND_TYPE_RETIRE_TEXTURES;
    command.retire_resources.count = count;
    command.retire_resources.ids   = textures;

    RENDER_DATA->command_buffer[RENDER_DATA->command_count++] =
        command;
    RENDER_DATA->non_draw_command_present = true;
    return true;
}

void graphics_subsystem_init( struct RenderData* render_data ) {
    RENDER_DATA = render_data;
}

