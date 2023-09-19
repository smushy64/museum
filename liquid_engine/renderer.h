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

enum : RenderID {
    RENDER_SHADER_DEBUG_COLOR,
    RENDER_SHADER_TEXT,
};

enum : RenderID {
    RENDER_MESH_QUAD_2D_LOWER_LEFT,
};

typedef struct {
    rgba color;
} ShaderDebugColor;

typedef struct {
    StringSlice text;
    FontID      font;
    rgba        color;
} ShaderText;

typedef struct {
    RenderID shader;
    mat4     transform;
    union {
        ShaderDebugColor debug_color;
        ShaderText       text;
    };
} Material;

typedef struct {
    RenderID mesh;
    Material material;
} RenderObject;

typedef u32 DrawFlags;
#define DRAW_FLAG_TRANSPARENT     (1 << 0)
#define DRAW_FLAG_SHADOW_CASTER   (1 << 1)
#define DRAW_FLAG_SHADOW_RECEIVER (1 << 2)
#define DRAW_FLAG_IS_WIREFRAME    (1 << 3)

typedef struct DrawCommand {
    mat4*     transform;
    RenderID  mesh;
    RenderID  texture_diffuse;
    RenderID  texture_normal;
    RenderID  texture_roughness;
    RenderID  texture_metallic;
    rgb       tint;
    DrawFlags flags;
} DrawCommand;

typedef enum GenerateCommandType {
    GENERATE_COMMAND_MESH,
    GENERATE_COMMAND_TEXTURE,
} GenerateCommandType;

typedef struct GenerateCommand {
    GenerateCommandType type;
    RenderID id;
    union {
        struct GenerateMesh {
            usize vertex_count;
            struct Vertex3D* vertices;
            usize index_count;
            u32* indices;
        } mesh;
        struct GenerateTexture {
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
        } texture;
    };
} GenerateCommand;

struct Camera;
/// Render Data
typedef struct RenderData {
    struct Camera* camera;
    f32 elapsed_time;
    f32 delta_time;
    u32 frame_count;

    usize max_object_count;
    usize object_count;
    RenderObject* objects;

    DrawCommand* draw_commands;
    usize        draw_command_count;
    usize        draw_command_max_count;

    GenerateCommand* generate_commands;
    usize            generate_command_count;
    usize            generate_command_max_count;
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

