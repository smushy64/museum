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

    f32 projection_2d_scale;

    f32 projection_3d_fov;
    f32 projection_3d_near;
    f32 projection_3d_far;

    f32 aspect_ratio;

    mat4 projection_3d;
    mat4 projection_2d;
    mat4 projection_ui;

} InternalRendererContext;

#endif // header guard
