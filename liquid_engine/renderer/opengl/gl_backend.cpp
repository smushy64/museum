/**
 * Description:  OpenGL Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "gl_backend.h"
#include "platform/platform.h"
#include <glad/glad.h>
#include "core/memory.h"

#define GL_DEFAULT_CLEAR_COLOR 0.0f, 0.0f, 0.0f, 1.0f

void gl_debug_callback (
    GLenum ,       // source
    GLenum ,       // type
    GLuint ,       // id
    GLenum ,       // severity
    GLsizei,       // message length
    const GLchar*, // message
    const void*    // userParam
);

RendererContext* gl_renderer_backend_initialize( Platform* platform ) {
    OpenGLRendererContext* result = (OpenGLRendererContext*)mem_alloc(
        sizeof(OpenGLRendererContext),
        MEMTYPE_RENDERER
    );
    if( !result ) {
        GL_LOG_FATAL("Failed to allocate backend memory!");
        return nullptr;
    }
    result->ctx.platform = platform;

    result->ctx.backend_shutdown    = gl_renderer_backend_shutdown;
    result->ctx.backend_on_resize   = gl_renderer_backend_on_resize;
    result->ctx.backend_begin_frame = gl_renderer_backend_begin_frame;
    result->ctx.backend_end_frame   = gl_renderer_backend_end_frame;

    void* glrc = platform_gl_init( platform );
    if( !glrc ) {
        mem_free( result );
        return nullptr;
    }

    result->glrc = glrc;

#if defined( LD_LOGGING )
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(
        gl_debug_callback,
        nullptr
    );
#endif

    const char* device_vendor  = (const char*)glGetString( GL_VENDOR );
    const char* device_name    = (const char*)glGetString( GL_RENDERER );
    const char* device_version = (const char*)glGetString( GL_VERSION );
    const char* device_glsl_version = (const char*)glGetString(
        GL_SHADING_LANGUAGE_VERSION
    );

    GL_LOG_NOTE( "Device Vendor:         %s", device_vendor );
    GL_LOG_NOTE( "Device Name:           %s", device_name );
    GL_LOG_NOTE( "Device Driver Version: %s", device_version );
    GL_LOG_NOTE( "Device GLSL Version:   %s", device_glsl_version );

    result->device_vendor       = device_vendor;
    result->device_name         = device_name;
    result->device_version      = device_version;
    result->device_glsl_version = device_glsl_version;
    
    i32 extension_count = 0;
    glGetIntegerv( GL_NUM_EXTENSIONS, &extension_count );
    GL_LOG_NOTE( "Supported extensions count: %i", extension_count );

    result->extension_count = extension_count;

    glClearColor( GL_DEFAULT_CLEAR_COLOR );
    glClear(
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
    );
    platform_gl_swap_buffers( result->ctx.platform );

    glViewport(
        0, 0,
        platform->surface.width, platform->surface.height
    );

    GL_LOG_INFO("OpenGL backend initialized successfully.");
    return (RendererContext*)result;
}
void gl_renderer_backend_shutdown( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;
    platform_gl_shutdown(
        ctx->ctx.platform,
        ctx->glrc
    );

    mem_free( ctx );
    GL_LOG_INFO("OpenGL backend shutdown.");
}
void gl_renderer_backend_on_resize(
    RendererContext*,
    i32 width, i32 height
) {
    glViewport( 0, 0, width, height );
}
b32 gl_renderer_backend_begin_frame(
    RendererContext* ctx,
    f32 delta_time
) {
    // TODO(alicia): 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    SM_UNUSED(ctx);
    SM_UNUSED(delta_time);
    return true;
}
b32 gl_renderer_backend_end_frame(
    RendererContext* ctx,
    f32 delta_time
) {
    // TODO(alicia): 
    SM_UNUSED(delta_time);
    platform_gl_swap_buffers( ctx->platform );
    return true;
}

inline const char* to_string_source( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}
inline const char* to_string_type( GLenum type ) {
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
    GLsizei,       // message length
    const GLchar* message,
    const void*    // userParam
) {
    #define GL_DEBUG_MESSAGE_FORMAT \
        "%i | Source [%s] Type [%s] %s", id,\
        to_string_source(source),\
        to_string_type(type), message

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
