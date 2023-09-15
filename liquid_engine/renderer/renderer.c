// * Description:  Renderer Frontend Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "renderer.h"
#include "renderer/context.h"
#include "platform.h"
#include "core/engine.h"
#include "core/graphics.h"
#include "core/graphics/types.h"
#include "core/log.h"
#include "core/mathf.h"
#include "core/mem.h"

// TODO(alicia): should these be labeled extern?
// it's all the same translation unit after all . . .

// NOTE(alicia): found in ldrenderer/opengl/ldopengl.c
extern usize GL_RENDERER_BACKEND_SIZE;
extern b32 gl_renderer_backend_init( RendererContext* ctx );

extern b32 vk_renderer_backend_init( RendererContext* ctx );

extern b32 dx11_renderer_backend_init( RendererContext* ctx );

extern b32 dx12_renderer_backend_init( RendererContext* ctx );

usize renderer_subsystem_query_size( enum RendererBackend backend ) {
    const usize backend_sizes[] = {
        GL_RENDERER_BACKEND_SIZE, // OpenGL
        0, // Vulkan
        0, // DirectX11
        0, // DirectX12
    };
    assert( backend < RENDERER_BACKEND_COUNT );
    return backend_sizes[backend];
}
b32 renderer_subsystem_init(
    PlatformSurface* surface,
    enum RendererBackend backend,
    void* context_buffer
) {
    InternalRendererContext* ctx = context_buffer;
    ctx->surface = surface;
    ctx->backend = backend;
    ivec2 dimensions = platform_surface_query_dimensions( ctx->surface );

    ctx->surface_dimensions     = dimensions;
    ctx->framebuffer_dimensions = dimensions;

    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            if( !gl_renderer_backend_init( ctx ) ) {
                return false;
            }
        } break;
        default: {
            LOG_ASSERT(
                false,
                "Backend \"{cc}\" is not currently supported!",
                renderer_backend_to_string( backend )
            );
        } break;
    }

    renderer_subsystem_on_resize( context_buffer, dimensions, dimensions );

    return true;
}
void renderer_subsystem_shutdown( RendererContext* opaque ) {
    InternalRendererContext* ctx = opaque;
    ctx->shutdown( ctx );
}
void renderer_subsystem_on_resize(
    RendererContext* opaque,
    union ivec2 surface_dimensions,
    union ivec2 framebuffer_dimensions
) {
    InternalRendererContext* ctx = opaque;

    ctx->surface_dimensions     = surface_dimensions;
    ctx->framebuffer_dimensions = framebuffer_dimensions;

    ctx->projection3d_dirty = true;

    mat4 view_ui = m4_view_2d( VEC2_ZERO, VEC2_UP );
    mat4 proj_ui = m4_ortho(
        0.0f, (f32)framebuffer_dimensions.width,
        0.0f, (f32)framebuffer_dimensions.height,
        -10.0f, 10.0f
    );

    ctx->projection_ui = m4_mul_m4( &view_ui, &proj_ui );

    ctx->on_resize( ctx );
}
internal b32 renderer_begin_frame(
    RendererContext* opaque, struct RenderData* render_data
) {
    InternalRendererContext* ctx = opaque;

    return ctx->begin_frame( ctx, render_data );
}
internal b32 renderer_end_frame(
    RendererContext* opaque, struct RenderData* render_data
) {
    InternalRendererContext* ctx = opaque;
    return ctx->end_frame( ctx, render_data );
}
b32 renderer_subsystem_on_draw(
    RendererContext* opaque, struct RenderData* render_data
) {
    if( renderer_begin_frame( opaque, render_data ) ) {
        if( !renderer_end_frame( opaque, render_data ) ) {
            LOG_FATAL( "Renderer failure!" );
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}
enum RendererBackend renderer_subsystem_query_backend(
    RendererContext* opaque
) {
    InternalRendererContext* ctx = opaque;
    if( !ctx ) {
        return 0;
    } else {
        return ctx->backend;
    }
}

