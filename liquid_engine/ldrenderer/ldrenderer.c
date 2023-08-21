// * Description:  Renderer Frontend Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "ldrenderer.h"
#include "ldrenderer/context.h"
#include "ldrenderer/ldopengl.h"
#include "core/ldgraphics.h"
#include "core/ldlog.h"
#include "core/ldmath/types.h"

global InternalRendererContext* RENDERER_CONTEXT = NULL;

usize renderer_query_subsystem_size( enum RendererBackend backend ) {
    local const usize backend_sizes[] = {
        sizeof(OpenGLRendererContext), // OpenGL
        0, // Vulkan
        0, // DirectX11
        0, // DirectX12
    };
    ASSERT( backend < RENDERER_BACKEND_COUNT );
    return backend_sizes[backend];
}
b32 renderer_subsystem_init(
    enum RendererBackend backend,
    void* context_buffer
) {
    RENDERER_CONTEXT          = context_buffer;
    RENDERER_CONTEXT->backend = backend;

    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            if( !gl_renderer_backend_init( RENDERER_CONTEXT ) ) {
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

    return true;
}
void renderer_subsystem_shutdown() {
    RENDERER_CONTEXT->shutdown( RENDERER_CONTEXT );
}
void renderer_subsystem_on_resize( union ivec2 surface_dimensions ) {
    RENDERER_CONTEXT->on_resize( RENDERER_CONTEXT, surface_dimensions );
}
internal b32 renderer_begin_frame( struct RenderData* render_data ) {
    return RENDERER_CONTEXT->begin_frame( RENDERER_CONTEXT, render_data );
}
internal b32 renderer_end_frame( struct RenderData* render_data ) {
    return RENDERER_CONTEXT->end_frame( RENDERER_CONTEXT, render_data );
}
b32 renderer_subsystem_on_draw( struct RenderData* render_data ) {
    if( renderer_begin_frame( render_data ) ) {
        if( !renderer_end_frame( render_data ) ) {
            LOG_FATAL( "Renderer failure!" );
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

enum RendererBackend renderer_subsystem_query_backend() {
    if( !RENDERER_CONTEXT ) {
        return 0;
    } else {
        return RENDERER_CONTEXT->backend;
    }
}

