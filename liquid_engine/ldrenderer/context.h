#if !defined(LD_RENDERER_CONTEXT_HPP)
#define LD_RENDERER_CONTEXT_HPP
// * Description:  Renderer Context
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "ldrenderer.h"

enum RendererBackend : u32;

typedef struct InternalRendererContext {
    enum RendererBackend        backend;

    RendererBackendShutdownFN   shutdown;
    RendererBackendOnResizeFN   on_resize;
    RendererBackendBeginFrameFN begin_frame;
    RendererBackendEndFrameFN   end_frame;
} InternalRendererContext;

#endif // header guard
