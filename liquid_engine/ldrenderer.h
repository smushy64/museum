#if !defined(LD_RENDERER_HPP)
#define LD_RENDERER_HPP
// * Description:  Renderer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#if defined(LD_API_INTERNAL)

enum RendererBackend : u32;
struct Platform;
union ivec2;

/// Opaque RendererContext handle
typedef void RendererContext;

/// Render Data
typedef struct RenderData {
    struct Timer* time;
} RenderData;

/// Renderer backend shutdown function prototype.
typedef void (*RendererBackendShutdownFN)( RendererContext* ctx );
/// Renderer backend surface resize function prototype.
typedef void (*RendererBackendOnResizeFN)(
    RendererContext* ctx, union ivec2 surface_dimensions );
/// Renderer backend begin frame function prototype.
typedef b32 (*RendererBackendBeginFrameFN)(
    RendererContext* ctx, struct RenderData* render_data );
/// Renderer backend end frame function prototype.
typedef b32 (*RendererBackendEndFrameFN)(
    RendererContext* ctx, struct RenderData* render_data );

/// Query required buffer size for renderer subsystem.
usize renderer_subsystem_context_size( enum RendererBackend backend );
/// Initialize renderer subsystem.
b32 renderer_subystem_init(
    enum RendererBackend backend,
    struct Platform* platform,
    void* context_buffer
);
/// Shutdown renderer subsystem.
void renderer_subsystem_shutdown();
/// On surface resize callback.
void renderer_subsystem_on_resize( union ivec2 surface_dimensions );
/// On draw callback.
b32 renderer_subsystem_on_draw( struct RenderData* render_data );
/// Get the current renderer backend.
enum RendererBackend renderer_subsystem_query_backend();

#endif // api internal
#endif // header guard
