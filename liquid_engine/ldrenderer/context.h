#if !defined(LD_RENDERER_CONTEXT_HPP)
#define LD_RENDERER_CONTEXT_HPP
// * Description:  Renderer Context
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "ldrenderer.h"
#include "core/ldmath/types.h"

enum RendererBackend : u32;

typedef struct InternalRendererContext {
    enum RendererBackend        backend;

    RendererBackendShutdownFN   shutdown;
    RendererBackendOnResizeFN   on_resize;
    RendererBackendBeginFrameFN begin_frame;
    RendererBackendEndFrameFN   end_frame;

    ivec2 surface_dimensions;
    ivec2 framebuffer_dimensions;

    mat4 projection_3d;
    mat4 projection_ui;
    f32 fov_radians;
    f32 near_clip;
    f32 far_clip;

    f32 aspect_ratio;

    void* surface;
} InternalRendererContext;

#endif // header guard
