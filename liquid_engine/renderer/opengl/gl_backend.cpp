/**
 * Description:  OpenGL Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "gl_backend.h"
#include "gl_defines.h"
#include <glad/glad.h>

#define GL_DEFAULT_CLEAR_COLOR 1.0f, 0.0f, 1.0f, 1.0f

global OpenGLContext CONTEXT = {};

void gl_debug_callback (
    GLenum ,       // source
    GLenum ,       // type
    GLuint ,       // id
    GLenum ,       // severity
    GLsizei,       // message length
    const GLchar*, // message
    const void*    // userParam
);

b32 gl_init(
    struct RendererBackend* backend,
    const char* app_name
) {
    platform_gl_init(
        backend->platform,
        &CONTEXT    
    );

    if( !CONTEXT.context ) {
        GL_LOG_FATAL( "OpenGL Context is null!" );
        return false;
    }


    glClearColor( GL_DEFAULT_CLEAR_COLOR );

#if defined( LD_LOGGING )
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(
        gl_debug_callback,
        nullptr
    );

    const char* vendor       = (const char*)glGetString( GL_VENDOR );
    const char* renderer     = (const char*)glGetString( GL_RENDERER );
    const char* glsl_version = (const char*)glGetString( GL_SHADING_LANGUAGE_VERSION );

    GL_LOG_NOTE( "Vendor:       %s", vendor );
    GL_LOG_NOTE( "Renderer:     %s", renderer );
    GL_LOG_NOTE( "GLSL Version: %s", glsl_version );
    
    i32 ext_count = 0;
    glGetIntegerv( GL_NUM_EXTENSIONS, &ext_count );
    GL_LOG_NOTE( "Supported extensions count: %i", ext_count );

#endif

    SM_UNUSED(app_name);
GL_LOG_INFO("OpenGL initialized successfully.");
    return true;
}

void gl_shutdown(
    struct RendererBackend* backend
) {
    SM_UNUSED(backend);
}

void gl_on_resize(
    struct RendererBackend* backend,
    i32 width, i32 height
) {
    SM_UNUSED(backend);
    SM_UNUSED(width);
    SM_UNUSED(height);
}

b32 gl_begin_frame(
    struct RendererBackend* backend,
    f32 delta_time
) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    SM_UNUSED(backend);
    SM_UNUSED(delta_time);
    return true;
}

b32 gl_end_frame(
    struct RendererBackend* backend,
    f32 delta_time
) {
    CONTEXT.swap_buffers( backend->platform );
    SM_UNUSED(delta_time);
    return true;
}

const char* to_string_source( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}
const char* to_string_type( GLenum type ) {
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
