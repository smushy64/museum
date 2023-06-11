/**
 * Description:  Renderer Frontend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 11, 2023
*/
#include "renderer.h"
#include "opengl/gl_backend.h"

RendererContext* renderer_init(
    const char*      app_name,
    RendererBackend  backend,
    struct Platform* platform
) {
    SM_ASSERT( platform );

    RendererContext* result = nullptr;
    switch( backend ) {
        case BACKEND_OPENGL: {
            result = gl_renderer_backend_initialize( platform );
            if( !result ) {
                return nullptr;
            }
        } break;
        default: {
            SM_UNUSED(app_name);
            LOG_FATAL(
                "Backend \"%s\" is not currently supported!",
                to_string(backend)
            );
            SM_PANIC();
        } return nullptr;
    }

    result->platform = platform;

    return result;
}
void renderer_shutdown( RendererContext* ctx ) {
    ctx->backend_shutdown( ctx );
}
void renderer_on_resize(
    RendererContext* ctx,
    i32 width, i32 height
) {
    ctx->backend_on_resize( ctx, width, height );
}
b32 renderer_begin_frame( RendererContext* ctx, f32 delta_time ) {
    return ctx->backend_begin_frame( ctx, delta_time );
}
b32 renderer_end_frame( RendererContext* ctx, f32 delta_time ) {
    b32 result = ctx->backend_end_frame( ctx, delta_time );
    ctx->frame_count++;
    return result;
}
b32 renderer_draw_frame(
    RendererContext* ctx,
    RenderOrder* order
) {
    if( renderer_begin_frame(
        ctx,
        order->delta_time
    ) ) {
        if( !renderer_end_frame(
            ctx,
            order->delta_time
        ) ) {
            LOG_FATAL( "Renderer end frame failed!" );
            return false;
        }
    }
    return true;
}

const char* to_string( RendererBackend backend ) {
    local const char* strings[BACKEND_COUNT] = {
        "Vulkan "
            VALUE_TO_STRING(VULKAN_VERSION_MAJOR)
            "."
            VALUE_TO_STRING(VULKAN_VERSION_MINOR),
        "OpenGL "
            VALUE_TO_STRING(GL_VERSION_MAJOR)
            "."
            VALUE_TO_STRING(GL_VERSION_MINOR),
        "DirectX 11",
        "DirectX 12"
    };
    SM_ASSERT( backend < BACKEND_COUNT );
    return strings[backend];
}
