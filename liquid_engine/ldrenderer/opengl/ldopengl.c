// * Description:  OpenGL Backend
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "ldrenderer.h"
#include "ldrenderer/ldopengl.h"
#include "ldrenderer/opengl/types.h"
#include "ldrenderer/opengl/functions.h"
#include "ldrenderer/opengl/buffer.h"
#include "core/ldgraphics.h"
#include "core/ldgraphics/types.h"
#include "core/ldmath.h"
#include "ldplatform.h"

#define GL_DEFAULT_CLEAR_MASK (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

void gl_debug_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei message_length,
    const GLchar* message, const void* userParam
);

void gl_renderer_backend_shutdown( RendererContext* renderer_ctx );
void gl_renderer_backend_on_resize(
    RendererContext* renderer_ctx, ivec2 surface_dimensions );
b32 gl_renderer_backend_begin_frame(
    RendererContext* renderer_ctx, RenderData* render_data );
b32 gl_renderer_backend_end_frame(
    RendererContext* renderer_ctx, RenderData* render_data );

internal void gl_init_buffers( OpenGLRendererContext* ctx );

b32 gl_renderer_backend_init( RendererContext* renderer_ctx ) {
    OpenGLRendererContext* ctx = renderer_ctx;

    OpenGLRenderContextHandle* render_context = platform_gl_init();
    if( !render_context ) {
        return false;
    }
    ctx->render_context = render_context;

#if defined(LD_LOGGING) && defined(DEBUG)
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( gl_debug_callback, NULL );
#endif

    glGetIntegerv( GL_NUM_EXTENSIONS, &ctx->device_info.extension_count );
    ctx->device_info.vendor  = (const char*)glGetString( GL_VENDOR );
    ctx->device_info.name    = (const char*)glGetString( GL_RENDERER );
    ctx->device_info.version = (const char*)glGetString( GL_VERSION );
    ctx->device_info.glsl_version =
        (const char*)glGetString( GL_SHADING_LANGUAGE_VERSION );

    GL_LOG_NOTE( "Device Vendor:          {cc}", ctx->device_info.vendor );
    GL_LOG_NOTE( "Device Name:            {cc}", ctx->device_info.name );
    GL_LOG_NOTE( "Device Driver Version:  {cc}", ctx->device_info.version );
    GL_LOG_NOTE( "Device GLSL Version:    {cc}", ctx->device_info.glsl_version );
    GL_LOG_NOTE( "Device Extension Count: {i}", ctx->device_info.extension_count );

    ctx->ctx.backend     = RENDERER_BACKEND_OPENGL;
    ctx->ctx.shutdown    = gl_renderer_backend_shutdown;
    ctx->ctx.on_resize   = gl_renderer_backend_on_resize;
    ctx->ctx.begin_frame = gl_renderer_backend_begin_frame;
    ctx->ctx.end_frame   = gl_renderer_backend_end_frame;

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    gl_init_buffers( ctx );

    GL_LOG_NOTE( "OpenGL Backend successfully initialized." );
    return true;
}

void gl_renderer_backend_shutdown( RendererContext* renderer_ctx ) {
    OpenGLRendererContext* ctx = renderer_ctx;
    platform_gl_shutdown( ctx->render_context );
    GL_LOG_INFO( "OpenGL Backend shutdown." );
}
void gl_renderer_backend_on_resize(
    RendererContext* renderer_ctx, ivec2 surface_dimensions
) {
    OpenGLRendererContext* ctx = renderer_ctx;
    ctx->viewport = surface_dimensions;
    glViewport(
        0, 0,
        ctx->viewport.width,
        ctx->viewport.height
    );
    gl_matrices_buffer_update_ui(
        ctx->buffers[GL_MATRICES_BUFFER_INDEX],
        &ctx->ctx.projection_ui
    );
}
b32 gl_renderer_backend_begin_frame(
    RendererContext* renderer_ctx, RenderData* render_data
) {
    OpenGLRendererContext* ctx = renderer_ctx;
 
    struct Camera* camera = render_data->camera;
    if( camera ) {
        switch( camera->type ) {
            case CAMERA_TYPE_2D: {
                vec2 up = v2_rotate(
                    VEC2_UP, camera->camera_2d.rotation_radians );
                mat4 lookat = m4_lookat_2d( camera->camera_2d.position, up );
                mat4 vp = m4_mul_m4( &lookat, &ctx->ctx.projection_2d );
                gl_matrices_buffer_update_2d(
                    ctx->buffers[GL_MATRICES_BUFFER_INDEX],
                    &vp
                );
            } break;
            case CAMERA_TYPE_3D: {
                vec3 up = q_mul_v3( camera->camera_3d.rotation, VEC3_UP );
                mat4 lookat = m4_lookat(
                    camera->camera_3d.position,
                    camera->camera_3d.target,
                    up
                );
                mat4 vp = m4_mul_m4( &lookat, &ctx->ctx.projection_2d );
                gl_matrices_buffer_update_3d(
                    ctx->buffers[GL_MATRICES_BUFFER_INDEX],
                    &vp
                );
            } break;
        }

    }

    glClear( GL_DEFAULT_CLEAR_MASK );
    unused(ctx);
    unused(render_data);
    return true;
}
b32 gl_renderer_backend_end_frame(
    RendererContext* renderer_ctx, RenderData* render_data
) {
    OpenGLRendererContext* ctx = renderer_ctx;
    platform_gl_swap_buffers();
    unused(ctx);
    unused(render_data);
    return true;
}

internal void gl_init_buffers( OpenGLRendererContext* ctx ) {
    glCreateBuffers( GL_BUFFER_COUNT, ctx->buffers );

    // create matrices buffer
    ivec2 dimensions = DEFAULT_SURFACE_DIMENSIONS;
    f32 aspect_ratio = (f32)dimensions.width / (f32)dimensions.height;
    mat4 matrices[3];
    /* vp 3d */ {
        mat4 lookat_3d     = m4_lookat( VEC3_BACK, VEC3_ZERO, VEC3_UP );
        mat4 projection_3d = m4_perspective(
            to_rad(60.0f), aspect_ratio, 0.001f, 1000.0f );
        matrices[0] = m4_mul_m4( &lookat_3d, &projection_3d );
    }
    /* vp ui */ {
        mat4 projection_ui = m4_ortho(
            0.0f, (f32)dimensions.width,
            0.0f, (f32)dimensions.height,
            0.0f, 1000.0f
        );
        matrices[1] = projection_ui;
    }
    /* vp 2d */ {
        mat4 lookat_2d     = m4_lookat_2d( VEC2_ZERO, VEC2_UP );
        mat4 projection_2d = m4_projection2d( aspect_ratio, 1.0f );
        matrices[2] = m4_mul_m4( &lookat_2d, &projection_2d );
    }
    gl_matrices_buffer_create(
        ctx->buffers[GL_MATRICES_BUFFER_INDEX], matrices );
}

const char* gl_debug_source_to_string( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}
const char* gl_debug_type_to_string( GLenum type ) {
    switch( type ) {
        case GL_DEBUG_TYPE_ERROR: return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
        case GL_DEBUG_TYPE_MARKER: return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
        case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
        default: return "Other";
    }
}
void gl_debug_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei message_length,
    const GLchar* message, const void* userParam
) {
    unused(source);
    unused(type);
    unused(id);
    unused(severity);
    unused(message_length);
    unused(message);
    unused(userParam);
#if defined(LD_LOGGING)
    #define GL_DEBUG_MESSAGE_FORMAT\
        id, gl_debug_source_to_string( source ),\
        gl_debug_type_to_string( type ),\
        message

    switch( severity ) {
        case GL_DEBUG_SEVERITY_HIGH: {
            GL_LOG_ERROR( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM: {
            GL_LOG_WARN( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_LOW: {
            GL_LOG_INFO( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
        default: {
            GL_LOG_NOTE( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
    }

#endif
}

