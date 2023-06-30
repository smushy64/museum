/**
 * Description:  Renderer Frontend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 11, 2023
*/
#include "renderer.h"
#include "opengl/gl_backend.h"
#include "core/time.h"

u32 query_renderer_subsystem_size( RendererBackend backend ) {
    local const u32 sizes[] = {
        sizeof(OpenGLRendererContext),
        0, // Vulkan
        0, // DX11
        0, // DX12
    };
    if( backend >= RENDERER_BACKEND_COUNT ) {
        return 0;
    }
    return sizes[backend];
}

b32 renderer_init(
    StringView       app_name,
    RendererBackend  backend,
    struct Platform* platform,
    u32 storage_size,
    RendererContext* storage
) {
    LD_ASSERT( platform );
    LD_ASSERT( storage );

    storage->platform = platform;
    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            if( storage_size != sizeof(OpenGLRendererContext) ) {
                LOG_FATAL( "Renderer Context buffer is not the right size!" );
                return false;
            }
            if( !gl_renderer_backend_initialize( storage ) ) {
                return false;
            }
        } break;
        default: {
            unused(app_name);
            LOG_FATAL(
                "Backend \"{cc}\" is not currently supported!",
                to_string(backend)
            );
            LD_PANIC();
        } return false;
    }


    return true;
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

