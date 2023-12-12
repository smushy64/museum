#if !defined(LD_CORE_GRAPHICS_INTERNAL_H)
#define LD_CORE_GRAPHICS_INTERNAL_H
// * Description:  Graphics Internal
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 24, 2023
#include "defines.h"

#if defined(LD_API_INTERNAL)
#include "core/math.h"
#include "core/collections.h"
#include "engine/graphics/types.h"
#include "engine/internal/platform.h"

extern struct RendererSubsystem* global_renderer;
extern struct RenderData*        global_render_data;
#define POINT_LIGHT_COUNT (4)

struct RenderCommand;
struct CommandPointLight;
struct CommandDirectionalLight;
struct CommandDraw3D;
struct CommandGenerateMesh;
struct CommandGenerateTexture;
struct CommandRetireResources;

typedef enum : u32 {
    RENDER_COMMAND_DRAW_3D,
    RENDER_COMMAND_GENERATE_MESH,
    RENDER_COMMAND_GENERATE_TEXTURE,
    RENDER_COMMAND_RETIRE_MESHES,
    RENDER_COMMAND_RETIRE_TEXTURES,
    RENDER_COMMAND_POINT_LIGHT,
    RENDER_COMMAND_DIRECTIONAL_LIGHT,

    RENDER_COMMAND_COUNT
} RenderCommandType;
header_only const char* render_command_type_to_cstr( RenderCommandType type ) {
    assert( type < RENDER_COMMAND_COUNT );
    const char* strings[RENDER_COMMAND_COUNT] = {
        "Command Draw 3D",
        "Command Generate Mesh",
        "Command Generate Texture",
        "Command Retire Meshes",
        "Command Retire Textures",
        "Command Set Point Light",
        "Command Set Directional Light",
    };
    return strings[type];
}

typedef u32 Draw3DFlags;
#define DRAW_3D_TRANSPARENT     (1 << 0)
#define DRAW_3D_SHADOW_CASTER   (1 << 1)
#define DRAW_3D_SHADOW_RECEIVER (1 << 2)
#define DRAW_3D_WIREFRAME       (1 << 3)

typedef struct RenderData {
    struct Camera* camera;

    List list_commands;
} RenderData;

struct CommandPointLight {
    vec3 position;
    vec3 color;
    u32  index;
    b32  is_active;
};
struct CommandDirectionalLight {
    vec3 direction;
    b32  is_active;
    vec3 color;
};
struct CommandDraw3D {
    mat4        transform;
    rgb         tint;
    Draw3DFlags flags;
    RenderID    mesh;
    RenderID    texture_diffuse;
    RenderID    texture_normal;
    RenderID    texture_roughness;
    RenderID    texture_metallic;
};
struct CommandGenerateMesh {
    RenderID         id;
    usize            vertex_count;
    struct Vertex3D* vertices;
    usize            index_count;
    u32*             indices;
};
struct CommandGenerateTexture {
    void*                   buffer;
    RenderID                id;
    u32                     width;
    u32                     height;
    u32                     depth;
    GraphicsTextureType     type;
    GraphicsTextureFormat   format;
    GraphicsTextureBaseType base_type;
    GraphicsTextureWrap     wrap_x;
    GraphicsTextureWrap     wrap_y;
    GraphicsTextureWrap     wrap_z;
    GraphicsTextureFilter   min_filter;
    GraphicsTextureFilter   mag_filter;
};
struct CommandRetireResources {
    RenderID* ids;
    usize     count;
};

struct RenderCommand {
    RenderCommandType type;
    union {
        struct CommandPointLight       point_light;
        struct CommandDirectionalLight directional_light;
        struct CommandDraw3D           draw_3d;
        struct CommandGenerateMesh     generate_mesh;
        struct CommandGenerateTexture  generate_texture;
        struct CommandRetireResources  retire_meshes;
        struct CommandRetireResources  retire_textures;
    };
};

typedef void RenderOnResizeFN(void);
typedef b32  RenderBeginFrameFN(void);
typedef b32  RenderEndFrameFN(void);
typedef void RenderShutdownFN(void);

struct RendererSubsystem {
    RendererBackend backend;

    RenderOnResizeFN*   on_resize;
    RenderBeginFrameFN* begin_frame;
    RenderEndFrameFN*   end_frame;
    RenderShutdownFN*   shutdown;

    PlatformSurface* surface;

    ivec2 surface_dimensions;
    ivec2 framebuffer_dimensions;

    mat4 projection;
    b32  projection_has_updated;
    f32  field_of_view;
    f32  near_clip, far_clip;

    b32 point_light_active[POINT_LIGHT_COUNT];
};

usize renderer_subsystem_query_size( RendererBackend backend );
b32 renderer_subsystem_init(
    PlatformSurface* surface,
    RendererBackend backend,
    ivec2 framebuffer_dimensions,
    RenderData* render_data,
    void* buffer );
void renderer_subsystem_on_resize(
    ivec2 surface_dimensions, ivec2 framebuffer_dimensions );
b32 renderer_subsystem_draw(void);
void renderer_subsystem_shutdown(void);

#endif // API internal
#endif // header guard
