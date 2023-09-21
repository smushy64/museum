#if !defined(LD_RENDERER_CONTEXT_HPP)
#define LD_RENDERER_CONTEXT_HPP
// * Description:  Renderer Context
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "renderer.h"
#include "core/mathf/types.h"
#include "core/collections.h"

enum RendererBackend : u32;

#define MESH_COUNT_MAX    (256)
#define TEXTURE_COUNT_MAX (256)

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

    b32 projection3d_dirty;

    Map_u32_u32 mesh_map;
    Map_u32_u32 texture_map;

    void* surface;

    u32 map_buffer[(MESH_COUNT_MAX * 2) + (TEXTURE_COUNT_MAX * 2)];
} InternalRendererContext;

#endif // header guard
