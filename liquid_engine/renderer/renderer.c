/**
 * Description:  Renderer Frontend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 11, 2023
*/
#include "renderer.h"
#include "opengl/gl_backend.h"
#include "core/time.h"
#include "core/collections.h"
#include "core/math/type_functions.h"

u32 query_renderer_subsystem_size( RendererBackend backend ) {
    local const u32 sizes[] = {
        sizeof(OpenGLRendererContext),
        0, // Vulkan
        0, // DX11
        0, // DX12
    };
    if( backend >= RENDERER_BACKEND_COUNT ) {
        return 0;
    }
    return sizes[backend];
}

b32 renderer_init(
    StringView       app_name,
    RendererBackend  backend,
    struct Platform* platform,
    u32 storage_size,
    RendererContext* storage
) {
    ASSERT( platform );
    ASSERT( storage );

    storage->platform = platform;
    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            if( storage_size != sizeof(OpenGLRendererContext) ) {
                LOG_FATAL( "Renderer Context buffer is not the right size!" );
                return false;
            }
            if( !gl_renderer_backend_initialize( storage ) ) {
                return false;
            }
        } break;
        default: {
            unused(app_name);
            LOG_FATAL(
                "Backend \"{cc}\" is not currently supported!",
                to_string(backend)
            );
            PANIC();
        } return false;
    }


    return true;
}
void renderer_shutdown( RendererContext* ctx ) {
    ctx->backend_shutdown( ctx );
}
void renderer_on_resize(
    RendererContext* ctx,
    i32 width, i32 height
) {
    ctx->backend_on_resize( ctx, width, height );
}
b32 renderer_begin_frame( RendererContext* ctx, RenderOrder* order ) {
    return ctx->backend_begin_frame( ctx, order );
}
b32 renderer_end_frame( RendererContext* ctx, RenderOrder* order ) {
    b32 result = ctx->backend_end_frame( ctx, order );
    return result;
}
b32 renderer_draw_frame(
    RendererContext* ctx,
    RenderOrder* order
) {
    if( renderer_begin_frame(
        ctx, order
    ) ) {
        if( !renderer_end_frame(
            ctx, order
        ) ) {
            LOG_FATAL( "Renderer end frame failed!" );
            return false;
        }
    }
    return true;
}

const char* to_string( RendererBackend backend ) {
    local const char* strings[RENDERER_BACKEND_COUNT] = {
        "OpenGL "
            VALUE_TO_STRING(GL_VERSION_MAJOR)
            "."
            VALUE_TO_STRING(GL_VERSION_MINOR),
        "Vulkan "
            VALUE_TO_STRING(VULKAN_VERSION_MAJOR)
            "."
            VALUE_TO_STRING(VULKAN_VERSION_MINOR),
        "DirectX 11",
        "DirectX 12"
    };
    ASSERT( backend < RENDERER_BACKEND_COUNT );
    return strings[backend];
}

b32 renderer_backend_is_supported( RendererBackend backend ) {
#if !defined(LD_PLATFORM_WINDOWS)
    if(
        backend == RENDERER_BACKEND_DX11 ||
        backend == RENDERER_BACKEND_DX12
    ) {
        return false;
    }
#endif
    unused(backend);
    return true;
}
#if defined(DEBUG)
#define list_reserve_no_trace( type, capacity )\
    (type*) ::impl::_list_create( capacity, sizeof(type) )

#define list_push_no_trace( list, value ) do {\
    __typeof(value) temp = value;\
    list = (__typeof(list)) ::impl::_list_push(\
        list, &temp\
    );\
} while(0)

LD_API void debug_draw_line(
    RenderOrder* render_order,
    vec2 from, vec2 to,
    rgba color
) {
    DebugPoints points = {};
    points.list_points = list_reserve_no_trace( vec2, 2 );
    list_push_no_trace( points.list_points, from );
    list_push_no_trace( points.list_points, to );
    points.color = color;
    list_push_no_trace( render_order->list_debug_points, points );
}
LD_API void debug_draw_rect(
    RenderOrder* render_order,
    Rect2D rect, rgba color
) {
    DebugPoints points = {};
    points.list_points = list_reserve_no_trace( vec2, 4 );
    list_push_no_trace( points.list_points, rect2d_top_left( rect ) );
    list_push_no_trace( points.list_points, rect2d_top_right( rect ) );
    list_push_no_trace( points.list_points, rect2d_bottom_right( rect ) );
    list_push_no_trace( points.list_points, rect2d_bottom_left( rect ) );
    points.color = color;
    list_push_no_trace( render_order->list_debug_points, points );
}
LD_API void debug_draw_circle(
    RenderOrder* render_order,
    Circle2D circle, rgba color
) {
    DebugPoints points = {};
    const u32 point_count = 12;
    points.list_points = list_reserve_no_trace( vec2, point_count );

    vec2 ray   = VEC2::UP * circle.radius;
    list_push_no_trace( points.list_points, circle.position + ray );

    f32 theta = F32::TAU / (f32)point_count;
    for( u32 i = 0; i < (point_count - 1); ++i ) {
        ray = rotate( ray, theta );
        list_push_no_trace( points.list_points, circle.position + ray );
    }

    points.color = color;
    list_push_no_trace( render_order->list_debug_points, points );
}
#else
LD_API void debug_draw_line(
    RenderOrder* ,
    vec2 , vec2 ,
    rgba 
) { }
LD_API void debug_draw_rect(
    RenderOrder* ,
    Rect2D , rgba 
) { }
LD_API void debug_draw_circle(
    RenderOrder* ,
    Circle2D , rgba 
) { }
#endif

