#if !defined(LD_RENDERER_HPP)
#define LD_RENDERER_HPP
// * Description:  Renderer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#if defined(LD_API_INTERNAL)

#include "core/graphics/types.h"
#include "core/mathf/types.h"
#include "core/strings.h"

enum RendererBackend : u32;
struct Platform;
union ivec2;

/// Opaque RendererContext handle
typedef void RendererContext;

struct SetPointLightCommand {
    vec3 position;
    vec3 color;
    u32  index;
    b32  is_active;
};

struct SetDirectionalLightCommand {
    vec3 direction;
    vec3 color;
};

typedef u32 DrawFlags;
#define DRAW_FLAG_TRANSPARENT     (1 << 0)
#define DRAW_FLAG_SHADOW_CASTER   (1 << 1)
#define DRAW_FLAG_SHADOW_RECEIVER (1 << 2)
#define DRAW_FLAG_IS_WIREFRAME    (1 << 3)

struct DrawCommand {
    mat4      transform;
    RenderID  mesh;
    RenderID  texture_diffuse;
    RenderID  texture_normal;
    RenderID  texture_roughness;
    RenderID  texture_metallic;
    rgb       tint;
    DrawFlags flags;
};

struct GenerateMeshCommand {
    RenderID         id;
    usize            vertex_count;
    struct Vertex3D* vertices;
    usize            index_count;
    u32*             indices;
};

struct GenerateTextureCommand {
    RenderID                id;
    GraphicsTextureType     type;
    GraphicsTextureFormat   format;
    GraphicsTextureBaseType base_type;
    GraphicsTextureWrap     wrap_x;
    GraphicsTextureWrap     wrap_y;
    GraphicsTextureWrap     wrap_z;
    GraphicsTextureFilter   minification_filter;
    GraphicsTextureFilter   magnification_filter;
    u32 width, height, depth;
    void* buffer;
};

struct RetireResourcesCommand {
    RenderID* ids;
    usize     count;
};

typedef enum : u32 {
    RENDER_COMMAND_TYPE_DRAW,
    RENDER_COMMAND_TYPE_GENERATE_MESH,
    RENDER_COMMAND_TYPE_GENERATE_TEXTURE,
    RENDER_COMMAND_TYPE_RETIRE_MESHES,
    RENDER_COMMAND_TYPE_RETIRE_TEXTURES,

    RENDER_COMMAND_TYPE_SET_DIRECTIONAL_LIGHT,
    RENDER_COMMAND_TYPE_SET_POINT_LIGHT,

    RENDER_COMMAND_TYPE_COUNT
} RenderCommandType;
header_only const char* render_command_type_to_cstr(
    RenderCommandType type
) {
    assert( type < RENDER_COMMAND_TYPE_COUNT );
    const char* strings[RENDER_COMMAND_TYPE_COUNT] = {
        "Command Draw",
        "Command Generate Mesh",
        "Command Generate Texture",
        "Command Retire Meshes",
        "Command Retire Textures",
        "Command Set Directional Light",
        "Command Set Point Light"
    };
    return strings[type];
}
header_only b32 render_command_type_is_generate( RenderCommandType type ) {
    switch( type ) {
        case RENDER_COMMAND_TYPE_GENERATE_MESH:
        case RENDER_COMMAND_TYPE_GENERATE_TEXTURE:
            return true;
        default: return false;
    }
}
header_only b32 render_command_type_is_retire( RenderCommandType type ) {
    switch( type ) {
        case RENDER_COMMAND_TYPE_RETIRE_MESHES:
        case RENDER_COMMAND_TYPE_RETIRE_TEXTURES:
            return true;
        default: return false;
    }
}

typedef struct RenderCommand {
    RenderCommandType type;
    union {
        struct DrawCommand                draw;
        struct GenerateMeshCommand        generate_mesh;
        struct GenerateTextureCommand     generate_texture;
        struct RetireResourcesCommand     retire_resources;
        struct SetDirectionalLightCommand directional_light;
        struct SetPointLightCommand       point_light;
    };
} RenderCommand;

#define RENDER_DATA_RENDER_COMMAND_MAX (1024)
struct Camera;
/// Render Data
typedef struct RenderData {
    struct Camera* camera;
    f32 elapsed_time;
    f32 delta_time;
    u32 frame_count;

    b32   non_draw_command_present;
    usize non_draw_command_start;

    RenderCommand* command_buffer;
    usize          command_count;
    usize          command_capacity;
} RenderData;

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

