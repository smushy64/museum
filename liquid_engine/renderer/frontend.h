#if !defined(RENDERER_FRONTEND_HPP)
#define RENDERER_FRONTEND_HPP
/**
 * Description:  Renderer Frontend
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 07, 2023
*/
#include "defines.h"
#include "renderer_defines.h"

b32 renderer_init(
    const char*           app_name,
    RendererBackendType   backend_type,
    struct PlatformState* state
);
void renderer_shutdown();
void renderer_on_resize( i32 width, i32 height );

b32 renderer_draw_frame( RenderOrder* order );

#endif // header guard
