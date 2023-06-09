#if !defined(RENDERER_BACKEND_HPP)
#define RENDERER_BACKEND_HPP
/**
 * Description:  Renderer Backend
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "defines.h"
#include "renderer_defines.h"

b32 renderer_backend_init(
    RendererBackendType type,
    struct Platform*    platform,
    RendererBackend*    out_backend
);
void renderer_backend_shutdown(
    RendererBackend* backend
);

#endif // header guard