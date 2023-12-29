#if !defined(CORE_GRAPHICS_HPP)
#define CORE_GRAPHICS_HPP
// * Description:  Graphics
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "shared/defines.h"
#include "core/math.h"
#include "engine/graphics/types.h"

struct MediaSurface;

/// Set the current camera.
LD_API void graphics_set_camera( struct Camera* camera );

/// Draw a mesh using phong brdf.
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
);

/// Send a generate mesh command to the renderer.
/// Provided buffers must live until the end of the current frame.
LD_API RenderID graphics_generate_mesh(
    usize vertex_count, struct Vertex3D* vertices,
    usize index_count, u32* indices );
/// Retire meshes.
/// Provided buffers must live until the end of the current frame.
LD_API void graphics_retire_meshes( usize count, RenderID* meshes );

/// Send a generate texture command to the renderer.
/// Provided buffers must live until the end of the current frame.
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
);
/// Retire textures.
/// Provided buffers must live until the end of the current frame.
LD_API void graphics_retire_textures( usize count, RenderID* textures );

/// Send a generate texture command to the renderer.
/// Provided buffers must live until the end of the current frame.
header_only
RenderID graphics_generate_texture_2d(
    GraphicsTextureFormat format,
    GraphicsTextureBaseType base_type,
    GraphicsTextureWrap     wrap_x,
    GraphicsTextureWrap     wrap_y,
    GraphicsTextureFilter   minification_filter,
    GraphicsTextureFilter   magnification_filter,
    u32 width, u32 height,
    usize buffer_size,
    void* buffer
) {
    return graphics_generate_texture(
        GRAPHICS_TEXTURE_TYPE_2D,
        format, base_type,
        wrap_x, wrap_y, (GraphicsTextureWrap)0,
        minification_filter, magnification_filter,
        width, height, 0,
        buffer_size, buffer
    );
}

/// Send a command to the renderer to change directional light parameters.
LD_API void graphics_set_directional_light(
    vec3 direction, vec3 color, b32 is_active );

/// Send a command to the renderer to change point light parameters.
LD_API void graphics_set_point_light(
    u32 index, vec3 position, vec3 color, b32 is_active );

#endif // header guard
