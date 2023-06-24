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
#include "core/engine.h"
#include "core/string.h"

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

u32 renderer_backend_size( RendererBackend backend );
b32 renderer_init(
    StringView       app_name,
    RendererBackend  backend,
    struct Platform* platform,
    u32 storage_size,
    RendererContext* storage
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
