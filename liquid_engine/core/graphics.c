// * Description:  Graphics Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 18, 2023
#include "defines.h"
#include "core/log.h"
#include "core/mathf.h"
#include "core/mem.h"
#include "core/graphics.h"
#include "core/graphics/internal.h"
#include "core/graphics/internal/opengl.h"

struct RendererSubsystem* global_renderer = NULL;
RenderData* global_render_data            = NULL;
global RenderID global_running_mesh_id    = 10;
global RenderID global_running_texture_id = 10;

usize renderer_subsystem_query_size( RendererBackend backend ) {
    assert( backend < RENDERER_BACKEND_COUNT );
    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            return sizeof(struct OpenGLSubsystem) + sizeof(struct RendererSubsystem);
        } break;
        default: panic();
    }
}
b32 renderer_subsystem_init(
    PlatformSurface* surface,
    RendererBackend backend,
    ivec2 framebuffer_dimensions,
    RenderData* render_data,
    void* buffer
) {
    usize subsystem_size = renderer_subsystem_query_size( backend );
    mem_zero( buffer, subsystem_size );

    global_renderer    = buffer;
    global_render_data = render_data;

    global_renderer->surface = surface;
    global_renderer->backend = backend;

    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            if( !gl_subsystem_init() ) {
                return false;
            }
        } break;
        default: UNIMPLEMENTED();
    }

    ivec2 surface_dimensions;
    platform->surface.query_dimensions(
        surface, &surface_dimensions.x, &surface_dimensions.y );

    renderer_subsystem_on_resize(
        surface_dimensions, framebuffer_dimensions );
    return true;
}
void renderer_subsystem_on_resize(
    ivec2 surface_dimensions, ivec2 framebuffer_dimensions
) {
    if( !global_renderer ) {
        return;
    }
    global_renderer->surface_dimensions     = surface_dimensions;
    global_renderer->framebuffer_dimensions = framebuffer_dimensions;

    f32 aspect_ratio = iv2_aspect_ratio( surface_dimensions );
    global_renderer->projection = m4_perspective(
        global_renderer->field_of_view,
        aspect_ratio, global_renderer->near_clip, global_renderer->far_clip );

    global_renderer->projection_has_updated = true;

    global_renderer->on_resize();
}

internal no_inline
b32 render_command_sort_lt( void* lhs, void* rhs, void* params ) {
    struct RenderCommand* a = lhs;
    struct RenderCommand* b = rhs;
    vec3 camera_position = *(vec3*)params;

    // if a and b are RENDER_COMMAND_DRAW_3D
    // RENDER_COMMAND_DRAW_3D is 0 so adding the two types should equal 0
    if( !(a->type + b->type) ) {
        // compare distance to camera
        vec3 a_pos = m4_transform_position( &a->draw_3d.transform );
        vec3 b_pos = m4_transform_position( &b->draw_3d.transform );

        f32 a_dist = v3_sqrmag( v3_sub( a_pos, camera_position ) );
        f32 b_dist = v3_sqrmag( v3_sub( b_pos, camera_position ) );

        // objects farther from the camera are rendered first.
        // more overdraw than i'd like but blending should work properly like this.
        // NOTE(alicia): it may be better to only sort this way if
        // object to be drawn is transparent.
        // else sort closer to the camera to hopefully avoid overdraw.
        return a_dist > b_dist;
    }

    // else return comparison of type variants
    // the variants are ordered in such a way that
    // they can be automatically sorted.
    return a->type < b->type;

}
internal no_inline
void render_command_sort_swap( void* lhs, void* rhs ) {
    struct RenderCommand* a = lhs;
    struct RenderCommand* b = rhs;
    struct RenderCommand  temp = *a;
    *a = *b;
    *b = temp;
}

internal b32 renderer_subsystem_begin_frame(void) {
    vec3 camera_position = VEC3_ZERO;
    Camera* camera = global_render_data->camera;
    if( camera ) {
        assert( camera->transform );
        camera_position = transform_world_position( camera->transform );
    }

    sorting_quicksort(
        0, list_count( global_render_data->list_commands ) - 1,
        sizeof(struct RenderCommand),
        global_render_data->list_commands,
        render_command_sort_lt, &camera_position,
        render_command_sort_swap );

    // TODO(alicia): frustum culling

    if( global_render_data->camera ) {
        Camera* camera = global_render_data->camera;
        if(
            camera->fov_radians != global_renderer->field_of_view ||
            camera->near_clip   != global_renderer->near_clip ||
            camera->far_clip    != global_renderer->far_clip
        ) {
            global_renderer->field_of_view = camera->fov_radians;
            global_renderer->near_clip     = camera->near_clip;
            global_renderer->far_clip      = camera->far_clip;
            f32 aspect_ratio =
                iv2_aspect_ratio( global_renderer->surface_dimensions );

            global_renderer->projection = m4_perspective(
                camera->fov_radians,
                aspect_ratio,
                camera->near_clip, camera->far_clip );

            global_renderer->projection_has_updated = true;
        }
    }

    return global_renderer->begin_frame();
}
internal b32 renderer_subsystem_end_frame(void) {
    return global_renderer->end_frame();
}
b32 renderer_subsystem_draw(void) {
    if( renderer_subsystem_begin_frame() ) {
        if( !renderer_subsystem_end_frame() ) {
            LOG_FATAL( "Renderer failed!" );
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void renderer_subsystem_shutdown(void) {
    global_renderer->shutdown();
    global_renderer = NULL;
}

LD_API void graphics_set_camera( struct Camera* camera ) {
    global_render_data->camera = camera;
}
LD_API void graphics_draw(
    mat4     transform,
    RenderID mesh,
    RenderID texture_diffuse,
    RenderID texture_normal,
    RenderID texture_roughness,
    RenderID texture_metallic,
    rgb      tint,
    b32      is_transparent,
    b32      is_shadow_caster,
    b32      is_shadow_receiver,
    b32      is_wireframe
) {
    struct RenderCommand command = {};
    command.type                      = RENDER_COMMAND_DRAW_3D;
    command.draw_3d.transform         = transform;
    command.draw_3d.mesh              = mesh;
    command.draw_3d.texture_diffuse   = texture_diffuse;
    command.draw_3d.texture_normal    = texture_normal;
    command.draw_3d.texture_roughness = texture_roughness;
    command.draw_3d.texture_metallic  = texture_metallic;
    command.draw_3d.tint              = tint;

    if( is_transparent ) {
        command.draw_3d.flags |= DRAW_3D_TRANSPARENT;
    }
    if( is_shadow_caster && !is_wireframe ) {
        command.draw_3d.flags |= DRAW_3D_SHADOW_CASTER;
    }
    if( is_shadow_receiver && !is_wireframe ) {
        command.draw_3d.flags |= DRAW_3D_SHADOW_RECEIVER;
    }
    if( is_wireframe ) {
        command.draw_3d.flags |= DRAW_3D_WIREFRAME;
    }

    assert( list_push( global_render_data->list_commands, &command ) );
}
LD_API RenderID graphics_generate_mesh(
    usize vertex_count, struct Vertex3D* vertices,
    usize index_count, u32* indices
) {
    struct RenderCommand command = {};
    command.type                       = RENDER_COMMAND_GENERATE_MESH;
    command.generate_mesh.id           = global_running_mesh_id;
    command.generate_mesh.vertex_count = vertex_count;
    command.generate_mesh.vertices     = vertices;
    command.generate_mesh.index_count  = index_count;
    command.generate_mesh.indices      = indices;

    assert( list_push( global_render_data->list_commands, &command ) );

    return global_running_mesh_id++;
}
LD_API void graphics_retire_meshes( usize count, RenderID* meshes ) {
    struct RenderCommand command = {};
    command.type                = RENDER_COMMAND_RETIRE_MESHES;
    command.retire_meshes.count = count;
    command.retire_meshes.ids   = meshes;

    assert( list_push( global_render_data->list_commands, &command ) );
}
LD_API RenderID graphics_generate_texture(
    GraphicsTextureType     type,
    GraphicsTextureFormat   format,
    GraphicsTextureBaseType base_type,
    GraphicsTextureWrap     wrap_mode_x,
    GraphicsTextureWrap     wrap_mode_y,
    GraphicsTextureWrap     wrap_mode_z,
    GraphicsTextureFilter   minification_filter,
    GraphicsTextureFilter   magnification_filter,
    u32 width, u32 height, u32 depth,
    usize buffer_size,
    void* buffer
) {
    struct RenderCommand command = {};
    command.type                        = RENDER_COMMAND_GENERATE_TEXTURE;
    command.generate_texture.type       = type;
    command.generate_texture.format     = format;
    command.generate_texture.base_type  = base_type;
    command.generate_texture.wrap_x     = wrap_mode_x;
    command.generate_texture.wrap_y     = wrap_mode_y;
    command.generate_texture.wrap_z     = wrap_mode_z;
    command.generate_texture.min_filter = minification_filter;
    command.generate_texture.mag_filter = magnification_filter;
    command.generate_texture.width      = width;
    command.generate_texture.height     = height;
    command.generate_texture.depth      = depth;
    command.generate_texture.buffer     = buffer;
    command.generate_texture.id         = global_running_texture_id;

    unused(buffer_size);
    assert( list_push( global_render_data->list_commands, &command ) );

    return global_running_texture_id++;
}
LD_API void graphics_retire_textures( usize count, RenderID* textures ) {
    struct RenderCommand command = {};
    command.type                  = RENDER_COMMAND_RETIRE_TEXTURES;
    command.retire_textures.count = count;
    command.retire_textures.ids   = textures;

    assert( list_push( global_render_data->list_commands, &command ) );
}
LD_API void graphics_set_directional_light(
    vec3 direction, vec3 color, b32 is_active
) {
    struct RenderCommand command = {};
    command.type                        = RENDER_COMMAND_DIRECTIONAL_LIGHT;
    command.directional_light.direction = direction;
    command.directional_light.color     = color;
    command.directional_light.is_active = is_active;

    assert( list_push( global_render_data->list_commands, &command ) );
}

LD_API void graphics_set_point_light(
    u32 index, vec3 position, vec3 color, b32 is_active
) {
    struct RenderCommand command  = {};
    command.type                  = RENDER_COMMAND_POINT_LIGHT;
    command.point_light.position  = position;
    command.point_light.color     = color;
    command.point_light.index     = index;
    command.point_light.is_active = is_active;

    assert( list_push( global_render_data->list_commands, &command ) );
}

