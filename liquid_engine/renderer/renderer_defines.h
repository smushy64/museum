#if !defined(RENDERER_DEFINES_HPP)
#define RENDERER_DEFINES_HPP
/**
 * Description:  Renderer defines
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 06, 2023
*/
#include "defines.h"
#include "core/time.h"

enum RendererBackendType : u32 {
    BACKEND_VULKAN,
    BACKEND_OPENGL,
    BACKEND_DX11,
    BACKEND_DX12,

    BACKEND_COUNT
};
const char* to_string( RendererBackendType backend );

typedef b32 (*RendererBackendInitializeFn)(
    struct RendererBackend* backend,
    const char* app_name
);

typedef void (*RendererBackendShutdownFn)(
    struct RendererBackend* backend
);

typedef void (*RendererBackendOnResizeFn)(
    struct RendererBackend* backend,
    i32 width, i32 height
);

typedef b32 (*RendererBackendBeginFrameFn)(
    struct RendererBackend* backend,
    f32 delta_time
);

typedef b32 (*RendererBackendEndFrameFn)(
    struct RendererBackend* backend,
    f32 delta_time
);

struct RendererBackend {
    struct PlatformState*       platform;

    RendererBackendInitializeFn init;
    RendererBackendShutdownFn   shutdown;
    RendererBackendOnResizeFn   on_resize;
    RendererBackendBeginFrameFn begin;
    RendererBackendEndFrameFn   end;

    u64 frame_count;
};

struct RenderOrder {
    Time time;
};

#endif