// * Description:  Graphics Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 18, 2023
#include "core/graphics.h"
#include "renderer.h"

RenderData* RENDER_DATA = NULL;

void graphics_draw(
    mat4*    transform,
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
    assert(
        RENDER_DATA->draw_command_count !=
        RENDER_DATA->draw_command_max_count );

    DrawCommand command = {};
    command.transform         = transform;
    command.mesh              = mesh;
    command.texture_diffuse   = texture_diffuse;
    command.texture_normal    = texture_normal;
    command.texture_roughness = texture_roughness;
    command.texture_metallic  = texture_metallic;
    command.tint              = tint;

    if( is_transparent ) {
        command.flags |= DRAW_FLAG_TRANSPARENT;
    }
    if( is_shadow_caster ) {
        command.flags |= DRAW_FLAG_SHADOW_CASTER;
    }
    if( is_shadow_receiver ) {
        command.flags |= DRAW_FLAG_SHADOW_RECEIVER;
    }
    if( is_wireframe ) {
        command.flags |= DRAW_FLAG_IS_WIREFRAME;
    }

    // TODO(alicia): thread safety!
    RENDER_DATA->draw_commands[RENDER_DATA->draw_command_count++] = command;
}

RenderID graphics_generate_mesh(
    usize vertex_count, struct Vertex3D* vertices,
    usize index_count, u32* indices
) {
    unused(vertex_count);
    unused(vertices);
    unused(index_count);
    unused(indices);
    return 0;
}
void graphics_retire_meshes( usize count, RenderID* meshes ) {
    unused(count);
    unused(meshes);
}

RenderID graphics_generate_texture(
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
    unused(type);
    unused(format);
    unused(base_type);
    unused(wrap_mode_x);
    unused(wrap_mode_y);
    unused(wrap_mode_z);
    unused(minification_filter);
    unused(magnification_filter);
    unused(width);
    unused(height);
    unused(depth);
    unused(buffer_size);
    unused(buffer);
    return 0;
}
void graphics_retire_textures( usize count, RenderID* textures ) {
    unused(count);
    unused(textures);
}

void graphics_subsystem_init( struct RenderData* render_data ) {
    RENDER_DATA = render_data;
}

