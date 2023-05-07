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

SM_GLOBAL RendererBackend* BACKEND = nullptr;

b32 renderer_init(
    const char*           app_name,
    RendererBackendType   backend_type,
    struct PlatformState* state
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
        state,
        BACKEND
    ) ) {
        return false;
    }

    if( !BACKEND->init( BACKEND, app_name, state ) ) {
        return false;
    }

    return true;
}
void renderer_shutdown() {
    BACKEND->shutdown( BACKEND );
    mem_free( BACKEND );
}
void renderer_on_resize( i32 width, i32 height ) {
    
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
    if( renderer_begin_frame( order->time.delta_time ) ) {
        if( !renderer_end_frame( order->time.delta_time ) ) {
            LOG_FATAL( "Renderer end frame failed!" );
            return false;
        }
    }
    return true;
}
