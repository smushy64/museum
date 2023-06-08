#if !defined(GL_BACKEND_HPP)
#define GL_BACKEND_HPP
/**
 * Description:  OpenGL Backend
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "defines.h"
#include "renderer/renderer_defines.h"

b32 gl_init(
    struct RendererBackend* backend,
    const char* app_name
);

void gl_shutdown(
    struct RendererBackend* backend
);

void gl_on_resize(
    struct RendererBackend* backend,
    i32 width, i32 height
);

b32 gl_begin_frame(
    struct RendererBackend* backend,
    f32 delta_time
);

b32 gl_end_frame(
    struct RendererBackend* backend,
    f32 delta_time
);

#endif // header guard