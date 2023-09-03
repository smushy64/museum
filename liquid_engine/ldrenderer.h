#if !defined(LD_RENDERER_HPP)
#define LD_RENDERER_HPP
// * Description:  Renderer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#if defined(LD_API_INTERNAL)

#include "core/ldmath/types.h"

enum RendererBackend : u32;
struct Platform;
union ivec2;

/// Opaque RendererContext handle
typedef void RendererContext;

struct Timer;
struct Camera;
/// Render Data
typedef struct RenderData {
    struct Camera* camera;
    f32 elapsed_time;
    f32 delta_time;
} RenderData;

/// Renderer backend shutdown function prototype.
typedef void (*RendererBackendShutdownFN)( RendererContext* ctx );
/// Renderer backend surface resize function prototype.
typedef void (*RendererBackendOnResizeFN)( RendererContext* ctx );
/// Renderer backend begin frame function prototype.
typedef b32 (*RendererBackendBeginFrameFN)(
    RendererContext* ctx, struct RenderData* render_data );
/// Renderer backend end frame function prototype.
typedef b32 (*RendererBackendEndFrameFN)(
    RendererContext* ctx, struct RenderData* render_data );

/// Query required buffer size for renderer subsystem.
usize renderer_subsystem_query_size( enum RendererBackend backend );
/// Initialize renderer subsystem.
b32 renderer_subsystem_init(
    void* surface,
    enum RendererBackend backend,
    void* context_buffer
);
/// Shutdown renderer subsystem.
void renderer_subsystem_shutdown( RendererContext* ctx );
/// On resize.
void renderer_subsystem_on_resize(
    RendererContext* ctx,
    ivec2 surface_dimensions, ivec2 framebuffer_dimensions );
/// On draw.
b32 renderer_subsystem_on_draw(
    RendererContext* ctx, struct RenderData* render_data );
/// Get the current renderer backend.
enum RendererBackend renderer_subsystem_query_backend( RendererContext* ctx );

#endif // api internal
#endif // header guard

