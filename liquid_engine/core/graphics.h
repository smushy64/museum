#if !defined(CORE_GRAPHICS_HPP)
#define CORE_GRAPHICS_HPP
// * Description:  Graphics
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "core/mathf/types.h"
#include "core/graphics/types.h"

LD_API void graphics_draw(
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
);

/// Send a generate mesh command to the renderer.
/// It takes one frame to generate a mesh.
/// ID is a unique identifier given to the renderer by the user.
RenderID graphics_generate_mesh(
    usize vertex_count, struct Vertex3D* vertices,
    usize index_count, u32* indices );
/// Retire meshes.
void graphics_retire_meshes( usize count, RenderID* meshes );

/// Send a generate texture command to the renderer.
/// It takes one frame to generate a texture.
/// ID is a unique identifier given to the renderer by the user.
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
);
/// Retire textures.
void graphics_retire_textures( usize count, RenderID* textures );

/// Send a generate texture command to the renderer.
/// It takes one frame to generate a texture.
/// Texture id will be written to out_result when the texture is ready.
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

/// Supported renderer backends
typedef enum RendererBackend : u32 {
    RENDERER_BACKEND_OPENGL,
    RENDERER_BACKEND_VULKAN,
    RENDERER_BACKEND_DX11,
    RENDERER_BACKEND_DX12,

    RENDERER_BACKEND_COUNT
} RendererBackend;
/// Convert renderer backend to const char*
header_only
const char* renderer_backend_to_string( RendererBackend backend ) {
    const char* strings[RENDERER_BACKEND_COUNT] = {
        "OpenGL "
            macro_value_to_string( GL_VERSION_MAJOR ) "."
            macro_value_to_string( GL_VERSION_MINOR ),
        "Vulkan "
            macro_value_to_string( VULKAN_VERSION_MAJOR ) "."
            macro_value_to_string( VULKAN_VERSION_MINOR ),
        "DirectX 11",
        "DirectX 12"
    };
    assert( backend < RENDERER_BACKEND_COUNT );
    return strings[backend];
}
/// Check if renderer backend is supported on the current platform.
header_only
b32 renderer_backend_is_supported( RendererBackend backend ) {
#if !defined(LD_PLATFORM_WINDOWS)
    if(
        backend == RENDERER_BACKEND_DX11 ||
        backend == RENDERER_BACKEND_DX12
    ) {
        return false;
    }
#endif
    unused(backend);
    return true;
}

#if defined(LD_API_INTERNAL)
    struct RenderData;
    void graphics_subsystem_init( struct RenderData* render_data );
#endif

#endif // header guard
