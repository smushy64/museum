/**
 * Description:  Renderer Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "backend.h"
#include "core/logging.h"
#include "vulkan/vk_backend.h"

b32 renderer_backend_init(
    RendererBackendType   type,
    struct PlatformState* state,
    RendererBackend*      out_backend
) {
    out_backend->platform = state;
    LOG_NOTE(
        "Initializing %s renderer backend . . .",
        to_string(type)
    );
    switch( type ) {
        case BACKEND_VULKAN: {
            out_backend->init      = vk_init;
            out_backend->shutdown  = vk_shutdown;
            out_backend->begin     = vk_begin_frame;
            out_backend->end       = vk_end_frame;
            out_backend->on_resize = vk_on_resize;
        } return true;
        default: DEBUG_UNIMPLEMENTED();
    }
}
void renderer_backend_shutdown(
    RendererBackend* backend
) {
    *backend = {};
}
