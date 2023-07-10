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
#include "core/graphics.h"

#define GL_DEFAULT_CLEAR_COLOR 0.0f, 0.0f, 0.0f, 1.0f

global GLuint NULL_TEXTURE = 0;

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
    glCreateTextures( GL_TEXTURE_2D, 1, &NULL_TEXTURE );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTextureStorage2D(
        NULL_TEXTURE,
        1, GL_RGBA8,
        1, 1
    );
    u32 null_texture_pixel = U32::MAX;
    glTextureSubImage2D(
        NULL_TEXTURE,
        0,
        0, 0,
        1, 1,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        &null_texture_pixel
    );

    glCreateBuffers( 1, &ctx->u_matrices );
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



    GL_LOG_INFO("OpenGL backend initialized successfully.");
    return true;
}
void gl_renderer_backend_shutdown( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    // TODO(alicia): TEST CODE ONLY

    glDeleteTextures( 1, &NULL_TEXTURE );

    glDeleteBuffers( 1, &ctx->u_matrices );
    gl_shader_program_delete( &ctx->phong );

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

internal void gl_make_mesh( Mesh* mesh ) {
    if( mesh->id.is_valid() ) {
        return;
    }
    GLuint vao = 0;
    glCreateVertexArrays( 1, &vao );
    mesh->id = vao;
    GLuint buffers[2];
    glCreateBuffers( 2, buffers );
    GLuint vbo = buffers[0];
    GLuint ebo = buffers[1];

    u32 vertex_size = sizeof(Vertex);
    if( mesh->vertex_type == VERTEX_TYPE_2D ) {
        vertex_size = sizeof(Vertex2D);
    }

    GLenum usage = GL_DYNAMIC_DRAW;
    if( mesh->is_static_mesh ) {
        usage = GL_STATIC_DRAW;
    }

    glNamedBufferData(
        vbo,
        mesh->vertex_count * vertex_size,
        mesh->vertices,
        usage
    );
    glVertexArrayVertexBuffer(
        vao, 0,
        vbo, 0,
        vertex_size
    );
    switch( mesh->vertex_type ) {
        case VERTEX_TYPE_2D: {
            glEnableVertexArrayAttrib( vao, 0 );
            glEnableVertexArrayAttrib( vao, 1 );
            glVertexArrayAttribFormat(
                vao, 0,
                2, GL_FLOAT,
                GL_FALSE,
                0
            );
            glVertexArrayAttribFormat(
                vao, 1,
                2, GL_FLOAT,
                GL_FALSE,
                sizeof(vec2)
            );
            glVertexArrayAttribBinding( vao, 0, 0 );
            glVertexArrayAttribBinding( vao, 1, 0 );
        } break;
        case VERTEX_TYPE_3D: {
            glEnableVertexArrayAttrib( vao, 0 );
            glEnableVertexArrayAttrib( vao, 1 );
            glEnableVertexArrayAttrib( vao, 2 );
            glEnableVertexArrayAttrib( vao, 3 );
            glVertexArrayAttribFormat(
                vao, 0,
                4, GL_FLOAT,
                GL_FALSE,
                0
            );
            glVertexArrayAttribFormat(
                vao, 1,
                3, GL_FLOAT,
                GL_FALSE,
                sizeof(vec4)
            );
            glVertexArrayAttribFormat(
                vao, 2,
                2, GL_FLOAT,
                GL_FALSE,
                sizeof(vec4) + sizeof(vec3)
            );
            glVertexArrayAttribFormat(
                vao, 3,
                3, GL_FLOAT,
                GL_FALSE,
                sizeof(vec4) + sizeof(vec3) + sizeof(vec2)
            );
            glVertexArrayAttribBinding( vao, 0, 0 );
            glVertexArrayAttribBinding( vao, 1, 0 );
            glVertexArrayAttribBinding( vao, 2, 0 );
            glVertexArrayAttribBinding( vao, 3, 0 );
        } break;
        default: PANIC();
    }
    u32 index_size = 0;
    switch( mesh->index_type ) {
        case INDEX_TYPE_U32: {
            index_size = sizeof(u32);
        } break;
        case INDEX_TYPE_U16: {
            index_size = sizeof(u16);
        } break;
        case INDEX_TYPE_U8: {
            index_size = sizeof(u8);
        } break;
        default: PANIC();
    }
    glNamedBufferData(
        ebo,
        index_size * mesh->index_count,
        mesh->indices,
        usage
    );
    glVertexArrayElementBuffer( vao, ebo );
}

b32 gl_renderer_backend_begin_frame(
    RendererContext* generic_ctx,
    RenderOrder* order
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;
    for( u32 i = 0; i < order->mesh_count; ++i ) {
        gl_make_mesh( &order->meshes[i] );
    }

    // TODO(alicia): 
    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT
    );
    glBindTextureUnit( 0, NULL_TEXTURE );

    glUseProgram( ctx->phong.handle );
    GLint transform_location = glGetUniformLocation( ctx->phong.handle, "u_transform" );
    ASSERT( transform_location >= 0 );
    for( u32 i = 0; i < order->draw_binding_count; ++i ) {
        DrawBinding* current_binding = &order->draw_bindings[i];
        Mesh* mesh = &order->meshes[current_binding->mesh_index];

        if( !mesh->id.is_valid() ) {
            continue;
        }

        glProgramUniformMatrix4fv(
            ctx->phong.handle,
            transform_location,
            1, GL_FALSE,
            value_pointer( current_binding->transform )
        );
        glBindVertexArray( mesh->id.id() );
        GLenum index_type = GL_UNSIGNED_INT;
        switch( mesh->index_type ) {
            case INDEX_TYPE_U16:
                index_type = GL_UNSIGNED_SHORT;
                break;
            case INDEX_TYPE_U8:
                index_type = GL_UNSIGNED_BYTE;
                break;
            default: break;
        }
        glDrawElements(
            GL_TRIANGLES,
            mesh->index_count,
            index_type,
            nullptr
        );
    }

    return true;
}
b32 gl_renderer_backend_end_frame(
    RendererContext* ctx,
    RenderOrder* order
) {
    // TODO(alicia): 
    
    platform_gl_swap_buffers( ctx->platform );
    unused(order);
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
