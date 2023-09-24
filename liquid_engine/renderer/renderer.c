// * Description:  Renderer Frontend Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "renderer.h"
#include "renderer/context.h"
#include "core/graphics.h"
#include "core/graphics/types.h"
#include "core/log.h"
#include "core/mathf.h"
#include "core/mem.h"
#include "core/internal.h"

// TODO(alicia): should these be labeled extern?
// it's all the same translation unit after all . . .

// NOTE(alicia): found in ldrenderer/opengl/ldopengl.c
extern usize GL_RENDERER_BACKEND_SIZE;
extern b32 gl_renderer_backend_init( RendererContext* ctx );

extern b32 vk_renderer_backend_init( RendererContext* ctx );

extern b32 dx11_renderer_backend_init( RendererContext* ctx );

extern b32 dx12_renderer_backend_init( RendererContext* ctx );

usize renderer_subsystem_query_size( enum RendererBackend backend ) {
    const usize backend_sizes[] = {
        GL_RENDERER_BACKEND_SIZE, // OpenGL
        0, // Vulkan
        0, // DirectX11
        0, // DirectX12
    };
    assert( backend < RENDERER_BACKEND_COUNT );
    return backend_sizes[backend];
}
b32 renderer_subsystem_init(
    PlatformSurface* surface,
    enum RendererBackend backend,
    void* context_buffer
) {
    InternalRendererContext* ctx = context_buffer;
    ctx->surface = surface;
    ctx->backend = backend;
    ivec2 dimensions = {};
    platform->surface.query_dimensions(
        ctx->surface, &dimensions.width, &dimensions.height );

    ctx->mesh_map    = map_u32_u32_create(
        MESH_COUNT_MAX * 2, ctx->map_buffer );

    ctx->texture_map = map_u32_u32_create(
        TEXTURE_COUNT_MAX * 2,
        ctx->map_buffer + (MESH_COUNT_MAX * 2) );

    ctx->surface_dimensions     = dimensions;
    ctx->framebuffer_dimensions = dimensions;

    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            if( !gl_renderer_backend_init( ctx ) ) {
                return false;
            }
        } break;
        default: {
            LOG_ASSERT(
                false,
                "Backend \"{cc}\" is not currently supported!",
                renderer_backend_to_string( backend )
            );
        } break;
    }

    renderer_subsystem_on_resize( context_buffer, dimensions, dimensions );

    return true;
}
void renderer_subsystem_on_resize(
    RendererContext* opaque,
    union ivec2 surface_dimensions,
    union ivec2 framebuffer_dimensions
) {
    InternalRendererContext* ctx = opaque;

    ctx->surface_dimensions     = surface_dimensions;
    ctx->framebuffer_dimensions = framebuffer_dimensions;

    ctx->projection3d_dirty = true;

    mat4 view_ui = m4_view_2d( VEC2_ZERO, VEC2_UP );
    mat4 proj_ui = m4_ortho(
        0.0f, (f32)framebuffer_dimensions.width,
        0.0f, (f32)framebuffer_dimensions.height,
        -10.0f, 10.0f
    );

    ctx->projection_ui = m4_mul_m4( &view_ui, &proj_ui );

    ctx->on_resize( ctx );
}

internal no_inline
b32 renderer_sort_command_lt( void* lhs, void* rhs, void* params ) {
    RenderCommand* a = lhs;
    RenderCommand* b = rhs;
    vec3* camera_position = params;

    if(
        a->type == RENDER_COMMAND_TYPE_DRAW &&
        b->type == RENDER_COMMAND_TYPE_DRAW
    ) {
        vec3 a_pos = m4_transform_position( &a->draw.transform );
        vec3 b_pos = m4_transform_position( &b->draw.transform );

        f32 a_dist = v3_sqrmag( v3_sub( a_pos, *camera_position ) );
        f32 b_dist = v3_sqrmag( v3_sub( b_pos, *camera_position ) );

        return a_dist < b_dist;
    } else {
        return
            a->type == RENDER_COMMAND_TYPE_DRAW &&
            b->type != RENDER_COMMAND_TYPE_DRAW;
    }

}
internal no_inline
void renderer_sort_command_swap( void* lhs, void* rhs ) {
    RenderCommand* a = lhs;
    RenderCommand* b = rhs;
    RenderCommand temp = *a;
    *a = *b;
    *b = temp;
}

internal b32 renderer_begin_frame(
    RendererContext* opaque, struct RenderData* render_data
) {
    InternalRendererContext* ctx = opaque;

    vec3 camera_position = VEC3_ZERO;
    if( render_data->camera ) {
        assert( render_data->camera->transform );
        camera_position = transform_world_position(
            render_data->camera->transform );
    }
    sorting_quicksort(
        0, render_data->command_count - 1,
        sizeof(RenderCommand), render_data->command_buffer,
        renderer_sort_command_lt, &camera_position,
        renderer_sort_command_swap );
    usize i = render_data->command_count;
    do {
        --i;
        if(
            render_data->command_buffer[i].type ==
            RENDER_COMMAND_TYPE_DRAW
        ) {
            render_data->non_draw_command_start =
                min( i + 1, render_data->command_count - 1);
            break;
        }
    } while( i );

    // TODO(alicia): frustum culling

    return ctx->begin_frame( ctx, render_data );
}
internal b32 renderer_end_frame(
    RendererContext* opaque, struct RenderData* render_data
) {
    InternalRendererContext* ctx = opaque;

    render_data->non_draw_command_present = false;
    return ctx->end_frame( ctx, render_data );
}
b32 renderer_subsystem_on_draw(
    RendererContext* opaque, struct RenderData* render_data
) {
    if( renderer_begin_frame( opaque, render_data ) ) {
        if( !renderer_end_frame( opaque, render_data ) ) {
            LOG_FATAL( "Renderer failure!" );
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}
enum RendererBackend renderer_subsystem_query_backend(
    RendererContext* opaque
) {
    InternalRendererContext* ctx = opaque;
    if( !ctx ) {
        return 0;
    } else {
        return ctx->backend;
    }
}

