/**
 * Description:  OpenGL Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "gl_backend.h"
#include "gl_functions.h"
#include "gl_buffer.h"
#include "renderer/primitives.h"

#include "ldplatform.h"
#include "core/ldmemory.h"
#include "core/ldcollections.h"
#include "core/ldtime.h"
#include "core/ldmath.h"

#define GL_DEFAULT_CLEAR_COLOR 0.0f, 0.0f, 0.0f, 0.0f
#define GL_RGBA_TO_COLOR( rgba ) rgba.r, rgba.g, rgba.b, rgba.a

void gl_debug_callback (
    GLenum ,       // source
    GLenum ,       // type
    GLuint ,       // id
    GLenum ,       // severity
    GLsizei,       // message length
    const GLchar*, // message
    const void*    // userParam
);

b32 gl_renderer_backend_initialize( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    generic_ctx->backend_shutdown    = gl_renderer_backend_shutdown;
    generic_ctx->backend_on_resize   = gl_renderer_backend_on_resize;
    generic_ctx->backend_begin_frame = gl_renderer_backend_begin_frame;
    generic_ctx->backend_end_frame   = gl_renderer_backend_end_frame;

    void* glrc = platform_gl_init( generic_ctx->platform );
    if( !glrc ) {
        return false;
    }

    ctx->glrc = glrc;

#if defined( LD_LOGGING ) && defined( DEBUG )
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(
        gl_debug_callback,
        NULL
    );
#endif

    const char* device_vendor  = (const char*)glGetString( GL_VENDOR );
    const char* device_name    = (const char*)glGetString( GL_RENDERER );
    const char* device_version = (const char*)glGetString( GL_VERSION );
    const char* device_glsl_version = (const char*)glGetString(
        GL_SHADING_LANGUAGE_VERSION
    );

    GL_LOG_NOTE( "Device Vendor:         {cc}", device_vendor );
    GL_LOG_NOTE( "Device Name:           {cc}", device_name );
    GL_LOG_NOTE( "Device Driver Version: {cc}", device_version );
    GL_LOG_NOTE( "Device GLSL Version:   {cc}", device_glsl_version );

    ctx->device_vendor       = device_vendor;
    ctx->device_name         = device_name;
    ctx->device_version      = device_version;
    ctx->device_glsl_version = device_glsl_version;
    
    i32 extension_count = 0;
    glGetIntegerv( GL_NUM_EXTENSIONS, &extension_count );
    GL_LOG_NOTE( "Supported extensions count: {i}", extension_count );

    ctx->extension_count = extension_count;

    glClearColor( GL_DEFAULT_CLEAR_COLOR );
    glClear( GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT
    );
    platform_gl_swap_buffers( generic_ctx->platform );

    ivec2 dimensions = {
        generic_ctx->platform->surface.width,
        generic_ctx->platform->surface.height
    };

    glViewport(
        0, 0,
        dimensions.width,
        dimensions.height
    );

    GL_LOG_INFO("OpenGL backend initialized successfully.");
    return true;
}
void gl_renderer_backend_shutdown( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    platform_gl_shutdown(
        ctx->ctx.platform,
        ctx->glrc
    );

    GL_LOG_INFO("OpenGL backend shutdown.");
}
void gl_renderer_backend_on_resize(
    RendererContext* generic_ctx,
    i32 width, i32 height
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;
    unused(ctx);
    unused(width);
    unused(height);
}

b32 gl_renderer_backend_begin_frame(
    RendererContext* generic_ctx,
    RenderOrder* order
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;
    unused(ctx);
    unused(order);
    return true;
}
b32 gl_renderer_backend_end_frame(
    RendererContext* generic_ctx,
    RenderOrder* order
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;
    unused(ctx);
    unused(order);
    return true;
}

extern inline const char* to_string_source( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}
extern inline const char* to_string_type( GLenum type ) {
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

void gl_debug_callback (
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei message_length,
    const GLchar* message,
    const void* user_param
) {
    unused( message_length );
    unused( user_param );
    #define GL_DEBUG_MESSAGE_FORMAT \
        "{u32} {cc} {cc} | {cc}",\
        id,\
        to_string_source(source),\
        to_string_type(type),\
        message

    switch( severity ) {
        case GL_DEBUG_SEVERITY_HIGH: {
            GL_LOG_ERROR( GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM: {
            GL_LOG_WARN( GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_LOW: {
            GL_LOG_INFO( GL_DEBUG_MESSAGE_FORMAT );
        } break;
        default: {
            GL_LOG_NOTE( GL_DEBUG_MESSAGE_FORMAT );
        } break;
    }

}
