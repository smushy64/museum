/**
 * Description:  OpenGL Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "gl_backend.h"
#include "gl_functions.h"
#include "gl_buffer.h"

#include "platform/platform.h"
#include "platform/io.h"
#include "core/memory.h"
#include "core/collections.h"
#include "core/time.h"
#include "core/math.h"

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

    // TODO(alicia): TEST CODE!!!!!
    glCreateBuffers( 3, ctx->buffer_handles );
    f32 aspect_ratio =
        (f32)dimensions.width /
        (f32)dimensions.height;
    mat4 view_projection = lookat(
        VEC3::FORWARD,
        VEC3::ZERO,
        VEC3::UP
    ) * ortho(
        -aspect_ratio,
        aspect_ratio,
        -1.0f,
        1.0f
    );
    glNamedBufferStorage(
        ctx->u_matrices,
        sizeof(mat4),
        value_pointer( view_projection ),
        GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(
        GL_UNIFORM_BUFFER,
        0,
        ctx->u_matrices
    );

    FileHandle phong_vert_file = {}, phong_frag_file = {};
    if( !platform_file_open(
        "./resources/shaders/phong.vert.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &phong_vert_file
    ) ) {
        GL_LOG_FATAL("FUCK");
        return false;
    }
    if( !platform_file_open(
        "./resources/shaders/phong.frag.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &phong_frag_file
    ) ) {
        GL_LOG_FATAL("FUCK");
        return false;
    }

    u32 phong_vert_binary_size =
        platform_file_query_size( phong_vert_file );
    void* phong_vert_binary = mem_alloc(
        phong_vert_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        phong_vert_file,
        phong_vert_binary_size,
        phong_vert_binary_size,
        phong_vert_binary
    );

    u32 phong_frag_binary_size =
        platform_file_query_size( phong_frag_file );
    void* phong_frag_binary = mem_alloc(
        phong_frag_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        phong_frag_file,
        phong_frag_binary_size,
        phong_frag_binary_size,
        phong_frag_binary
    );

    platform_file_close( phong_vert_file );
    platform_file_close( phong_frag_file );

    Shader phong_shaders[2] = {};

    if( !gl_shader_compile(
        phong_vert_binary_size,
        phong_vert_binary,
        GL_VERTEX_SHADER,
        "main",
        0,
        0,
        0,
        &phong_shaders[0]
    ) ) {
        return false;
    }
    if( !gl_shader_compile(
        phong_frag_binary_size,
        phong_frag_binary,
        GL_FRAGMENT_SHADER,
        "main",
        0,
        0,
        0,
        &phong_shaders[1]
    ) ) {
        return false;
    }
    if( !gl_shader_program_link(
        2, phong_shaders,
        &ctx->phong
    ) ) {
        return false;
    }
    gl_shader_delete( phong_shaders[0] );
    gl_shader_delete( phong_shaders[1] );
    mem_free( phong_vert_binary );
    mem_free( phong_frag_binary );
    if( !gl_shader_program_reflection( &ctx->phong ) ) {
        return false;
    }

    glCreateVertexArrays( 1, &ctx->vao_triangle );
    local f32 TRIANGLE_VERTICES[] = {
        -1.0f, -1.0f, /* color */  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, /* color */  0.0f,  1.0f,  0.0f,
        0.0f,1.0f, /* color */ 0.0f, 0.0f, 1.0f,
    };
    local u32 TRIANGLE_INDICES[] = {
        0, 1, 2
    };
    glNamedBufferData(
        ctx->vbo_triangle,
        STATIC_ARRAY_SIZE( TRIANGLE_VERTICES ),
        TRIANGLE_VERTICES,
        GL_STATIC_DRAW
    );
    glVertexArrayVertexBuffer(
        ctx->vao_triangle,
        0,
        ctx->vbo_triangle,
        0, sizeof(f32) * 5
    );
    glEnableVertexArrayAttrib( ctx->vao_triangle, 0 );
    glEnableVertexArrayAttrib( ctx->vao_triangle, 1 );
    glVertexArrayAttribFormat(
        ctx->vao_triangle,
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        0
    );
    glVertexArrayAttribFormat(
        ctx->vao_triangle,
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(f32) * 2
    );
    glVertexArrayAttribBinding(
        ctx->vao_triangle,
        0,
        0
    );
    glVertexArrayAttribBinding(
        ctx->vao_triangle,
        1,
        0
    );

    glNamedBufferData(
        ctx->ebo_triangle,
        STATIC_ARRAY_SIZE( TRIANGLE_INDICES ),
        TRIANGLE_INDICES,
        GL_STATIC_DRAW
    );
    glVertexArrayElementBuffer(
        ctx->vao_triangle,
        ctx->ebo_triangle
    );

    GL_LOG_INFO("OpenGL backend initialized successfully.");
    return true;
}
void gl_renderer_backend_shutdown( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    // TODO(alicia): TEST CODE ONLY

    glDeleteBuffers( 3, ctx->buffer_handles );
    gl_shader_program_delete( &ctx->phong );
    glDeleteVertexArrays( 1, &ctx->vao_triangle );

    // TODO(alicia): END TEST CODE ONLY

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

    glViewport( 0, 0, width, height );
    f32 aspect_ratio = (f32)width / (f32)height;
    mat4 view_projection = lookat(
        VEC3::FORWARD,
        VEC3::ZERO,
        VEC3::UP
    ) * ortho(
        -aspect_ratio,
        aspect_ratio,
        -1.0f,
        1.0f
    );
    glNamedBufferSubData(
        ctx->u_matrices,
        0, sizeof(mat4),
        value_pointer( view_projection )
    );
}
b32 gl_renderer_backend_begin_frame( RendererContext* generic_ctx, Time* time ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;
    // TODO(alicia): 
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glUseProgram( ctx->phong.handle );
    glBindVertexArray( ctx->vao_triangle );
    glDrawElements(
        GL_TRIANGLES,
        3,
        GL_UNSIGNED_INT,
        nullptr
    );

    unused(time);
    return true;
}
b32 gl_renderer_backend_end_frame( RendererContext* ctx, Time* time ) {
    // TODO(alicia): 
    unused(time);
    
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
        "{u} | Source [{cc}] Type [{cc}] {cc}", id,\
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
