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
#include "core/ecs.h"

typedef void (*RendererBackendShutdownFn)(
    struct RendererContext* ctx
);

typedef void (*RendererBackendOnResizeFn)(
    struct RendererContext* ctx,
    i32 width, i32 height
);

typedef b32 (*RendererBackendBeginFrameFn)(
    struct RendererContext* ctx,
    struct RenderOrder* order
);

typedef b32 (*RendererBackendEndFrameFn)(
    struct RendererContext* ctx,
    struct RenderOrder* order
);

struct RendererContext {
    struct Platform* platform;

    RendererBackendShutdownFn   backend_shutdown;
    RendererBackendOnResizeFn   backend_on_resize;
    RendererBackendBeginFrameFn backend_begin_frame;
    RendererBackendEndFrameFn   backend_end_frame;
};

struct DrawBinding {
    mat4 transform;
    u32  mesh_index;
    u32  texture_index;
};

#if defined(DEBUG)
struct DebugPoints {
    vec2* list_points;
    rgba  color;
};
#endif

enum UIAnchorX {
    UI_ANCHOR_X_LEFT,
    UI_ANCHOR_X_CENTER,
    UI_ANCHOR_X_RIGHT
};

struct UIText {
    StringView text;
    vec2       position;
    f32        scale;
    rgba       color;
    UIAnchorX  anchor_x;
};

struct UIImage {
    vec2      position;
    f32       scale;
    rgba      color;
    UIAnchorX anchor_x;
    Texture*  texture;
};

struct RenderOrder {
    Mesh* meshes;
    u32   mesh_count;

    Texture* textures;
    u32      texture_count;

    UIText* ui_text;
    u32     text_count;

    UIImage* ui_image;
    u32      image_count;

    DrawBinding* draw_bindings;
    u32 draw_binding_count;

    struct EntityStorage*    storage;
    EntityStorageQueryResult sprites;

    struct Timer* time;

#if defined(DEBUG)
    DebugPoints* list_debug_points;
#endif
};

LD_API void debug_draw_line( RenderOrder* render_order, vec2 from, vec2 to, rgba color );
LD_API void debug_draw_rect( RenderOrder* render_order, Rect2D rect, rgba color );
LD_API void debug_draw_circle( RenderOrder* render_order, Circle2D circle, rgba color );

u32 query_renderer_subsystem_size( RendererBackend backend );
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
