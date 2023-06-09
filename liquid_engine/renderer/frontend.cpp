/**
 * Description:  Renderer Frontend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "frontend.h"
#include "backend.h"
#include "core/logging.h"
#include "core/memory.h"
#include "platform/platform.h"

global RendererBackend* BACKEND = nullptr;

b32 renderer_init(
    const char*         app_name,
    RendererBackendType backend_type,
    Platform*           platform
) {
    BACKEND = (RendererBackend*)mem_alloc(
        sizeof(RendererBackend),
        MEMTYPE_RENDERER
    );
    if( !BACKEND ) {
        LOG_ERROR("Failed to allocate backend memory!");
        return false;
    }
    if( !renderer_backend_init(
        backend_type,
        platform,
        BACKEND
    ) ) {
        return false;
    }

    if( !BACKEND->init( BACKEND, app_name ) ) {
        return false;
    }

    return true;
}
void renderer_shutdown() {
    BACKEND->shutdown( BACKEND );
    mem_free( BACKEND );
}
void renderer_on_resize( i32 width, i32 height ) {
    SM_UNUSED(width);
    SM_UNUSED(height);
}

b32 renderer_begin_frame( f32 delta_time ) {
    return BACKEND->begin( BACKEND, delta_time );
}

b32 renderer_end_frame( f32 delta_time ) {
    b32 result = BACKEND->end( BACKEND, delta_time );
    BACKEND->frame_count++;
    return result;
}

b32 renderer_draw_frame( RenderOrder* order ) {
    if( renderer_begin_frame( order->delta_time ) ) {
        if( !renderer_end_frame( order->delta_time ) ) {
            LOG_FATAL( "Renderer end frame failed!" );
            return false;
        }
    }
    return true;
}

const char* to_string( RendererBackendType backend ) {
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
