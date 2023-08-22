// * Description:  Renderer Frontend Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "ldrenderer.h"
#include "ldrenderer/context.h"
#include "ldrenderer/ldopengl.h"
#include "core/ldgraphics.h"
#include "core/ldgraphics/types.h"
#include "core/ldlog.h"
#include "core/ldmath.h"

global InternalRendererContext* RENDERER_CONTEXT = NULL;

b32 gl_renderer_backend_init( RendererContext* ctx );
b32 vk_renderer_backend_init( RendererContext* ctx );
b32 dx11_renderer_backend_init( RendererContext* ctx );
b32 dx12_renderer_backend_init( RendererContext* ctx );

usize renderer_subsystem_query_size( enum RendererBackend backend ) {
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
    f32 aspect_ratio =
        (f32)surface_dimensions.width / (f32)surface_dimensions.height;
    RENDERER_CONTEXT->aspect_ratio = aspect_ratio;

    RENDERER_CONTEXT->projection_2d = m4_projection2d(
        RENDERER_CONTEXT->aspect_ratio,
        RENDERER_CONTEXT->projection_2d_scale );
    RENDERER_CONTEXT->projection_3d = m4_perspective(
        RENDERER_CONTEXT->projection_3d_fov,
        RENDERER_CONTEXT->aspect_ratio,
        RENDERER_CONTEXT->projection_3d_near,
        RENDERER_CONTEXT->projection_3d_far
    );
    RENDERER_CONTEXT->projection_ui = m4_ortho(
        0.0f, (f32)surface_dimensions.width,
        0.0f, (f32)surface_dimensions.height,
        0.0f, 1000.0f
    );

    RENDERER_CONTEXT->on_resize( RENDERER_CONTEXT, surface_dimensions );
}
internal b32 renderer_begin_frame( struct RenderData* render_data ) {

    struct Camera* camera = render_data->camera;
    if( camera ) {
        switch( camera->type ) {
            case CAMERA_TYPE_2D: {
                if(
                    RENDERER_CONTEXT->projection_2d_scale !=
                    camera->camera_2d.scale
                ) {
                    RENDERER_CONTEXT->projection_2d_scale =
                        camera->camera_2d.scale;
                    RENDERER_CONTEXT->projection_2d = m4_projection2d(
                        RENDERER_CONTEXT->aspect_ratio,
                        RENDERER_CONTEXT->projection_2d_scale );
                }
            } break;
            case CAMERA_TYPE_3D: {
                if(
                    RENDERER_CONTEXT->projection_3d_fov !=
                    camera->camera_3d.fov ||
                    RENDERER_CONTEXT->projection_3d_far !=
                    camera->camera_3d.far_ ||
                    RENDERER_CONTEXT->projection_3d_near !=
                    camera->camera_3d.near_
                ) {
                    RENDERER_CONTEXT->projection_3d_fov =
                        camera->camera_3d.fov;
                    RENDERER_CONTEXT->projection_3d_near =
                        camera->camera_3d.near_;
                    RENDERER_CONTEXT->projection_3d_far =
                        camera->camera_3d.far_;

                    RENDERER_CONTEXT->projection_3d = m4_perspective(
                        RENDERER_CONTEXT->projection_3d_fov,
                        RENDERER_CONTEXT->aspect_ratio,
                        RENDERER_CONTEXT->projection_3d_near,
                        RENDERER_CONTEXT->projection_3d_far
                    );
                }
            } break;
        }
    }

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

