/**
 * Description:  Renderer Frontend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 11, 2023
*/
#include "renderer.h"
#include "opengl/gl_backend.h"
#include "core/time.h"

RendererContext* renderer_init(
    StringView       app_name,
    RendererBackend  backend,
    struct Platform* platform
) {
    LD_ASSERT( platform );

    RendererContext* result = nullptr;
    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            result = gl_renderer_backend_initialize( platform );
            if( !result ) {
                return nullptr;
            }
        } break;
        default: {
            unused(app_name);
            LOG_FATAL(
                "Backend \"%s\" is not currently supported!",
                to_string(backend)
            );
            LD_PANIC();
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
b32 renderer_begin_frame( RendererContext* ctx, Time* time ) {
    return ctx->backend_begin_frame( ctx, time );
}
b32 renderer_end_frame( RendererContext* ctx, Time* time ) {
    b32 result = ctx->backend_end_frame( ctx, time );
    return result;
}
b32 renderer_draw_frame(
    RendererContext* ctx,
    RenderOrder* order
) {
    if( renderer_begin_frame(
        ctx, order->time
    ) ) {
        if( !renderer_end_frame(
            ctx, order->time
        ) ) {
            LOG_FATAL( "Renderer end frame failed!" );
            return false;
        }
    }
    return true;
}

const char* to_string( RendererBackend backend ) {
    local const char* strings[RENDERER_BACKEND_COUNT] = {
        "OpenGL "
            VALUE_TO_STRING(GL_VERSION_MAJOR)
            "."
            VALUE_TO_STRING(GL_VERSION_MINOR),
        "Vulkan "
            VALUE_TO_STRING(VULKAN_VERSION_MAJOR)
            "."
            VALUE_TO_STRING(VULKAN_VERSION_MINOR),
        "DirectX 11",
        "DirectX 12"
    };
    LD_ASSERT( backend < RENDERER_BACKEND_COUNT );
    return strings[backend];
}

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

