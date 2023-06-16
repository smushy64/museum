#if !defined(RENDERER_HPP)
#define RENDERER_HPP
/**
 * Description:  Renderer Frontend
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 11, 2023
*/
#include "defines.h"
#include "core/logging.h"
#include "core/graphics.h"

#if !defined(RENDERER_BACKEND_DEFINED)
#define RENDERER_BACKEND_DEFINED
enum RendererBackend : u32 {
    BACKEND_VULKAN,
    BACKEND_OPENGL,
    BACKEND_DX11,
    BACKEND_DX12,

    BACKEND_COUNT
};
const char* to_string( RendererBackend backend );
#endif

typedef void (*RendererBackendShutdownFn)(
    struct RendererContext* ctx
);

typedef void (*RendererBackendOnResizeFn)(
    struct RendererContext* ctx,
    i32 width, i32 height
);

typedef b32 (*RendererBackendBeginFrameFn)(
    struct RendererContext* ctx,
    struct Time* time
);

typedef b32 (*RendererBackendEndFrameFn)(
    struct RendererContext* ctx,
    struct Time* time
);

struct RendererContext {
    struct Platform* platform;

    RendererBackendShutdownFn   backend_shutdown;
    RendererBackendOnResizeFn   backend_on_resize;
    RendererBackendBeginFrameFn backend_begin_frame;
    RendererBackendEndFrameFn   backend_end_frame;

    void* backend;
};

struct RenderOrder {
    Mesh* meshes;
    u32   mesh_count;

    struct Time* time;
};

RendererContext* renderer_init(
    const char*      app_name,
    RendererBackend  backend,
    struct Platform* platform
);
void renderer_shutdown( RendererContext* ctx );
void renderer_on_resize(
    RendererContext* ctx,
    i32 width, i32 height
);
b32 renderer_draw_frame(
    RendererContext* ctx,
    RenderOrder* order
);

#endif // header guard