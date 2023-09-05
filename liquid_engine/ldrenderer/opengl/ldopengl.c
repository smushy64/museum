// * Description:  OpenGL Backend
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "ldrenderer.h"
#include "ldrenderer/ldopengl.h"
#include "ldrenderer/opengl/types.h"
#include "ldrenderer/opengl/functions.h"
#include "ldrenderer/opengl/buffer.h"
#include "core/ldgraphics.h"
#include "core/ldgraphics/primitives.h"
#include "core/ldgraphics/types.h"
#include "core/ldmath.h"
#include "core/ldmemory.h"
#include "core/ldtime.h"
#include "ldplatform.h"

usize GL_RENDERER_BACKEND_SIZE = sizeof(OpenGLRendererContext);

#define GL_DEFAULT_CLEAR_MASK\
    (GL_COLOR_BUFFER_BIT)

void gl_debug_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei message_length,
    const GLchar* message, const void* userParam
);

void gl_renderer_backend_shutdown( RendererContext* renderer_ctx );
void gl_renderer_backend_on_resize( RendererContext* renderer_ctx );
b32 gl_renderer_backend_begin_frame(
    RendererContext* renderer_ctx, RenderData* render_data );
b32 gl_renderer_backend_end_frame(
    RendererContext* renderer_ctx, RenderData* render_data );

internal void gl_init_buffers( OpenGLRendererContext* ctx );
internal void gl_init_shaders( OpenGLRendererContext* ctx );

b32 gl_renderer_backend_init( RendererContext* renderer_ctx ) {
    OpenGLRendererContext* ctx = renderer_ctx;

    if( !platform_gl_surface_init( ctx->ctx.surface ) ) {
        return false;
    }

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

    gl_init_buffers( ctx );
    gl_init_shaders( ctx );

    ctx->framebuffer_main = gl_framebuffer_create(
        ctx->ctx.framebuffer_dimensions.width,
        ctx->ctx.framebuffer_dimensions.height
    );

    GL_LOG_NOTE( "OpenGL Backend successfully initialized." );
    return true;
}

void gl_renderer_backend_shutdown( RendererContext* renderer_ctx ) {
    OpenGLRendererContext* ctx = renderer_ctx;
    platform_gl_surface_shutdown( ctx->ctx.surface );
    GL_LOG_INFO( "OpenGL Backend shutdown." );
}

internal void gl_draw_framebuffer(
    OpenGLRendererContext* ctx, ivec2 viewport
) {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport(
        0, 0,
        viewport.width,
        viewport.height
    );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    GLShaderProgramID program =
        ctx->programs[GL_SHADER_PROGRAM_INDEX_FRAMEBUFFER];
    GLVertexArrayID vertex_array =
        ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER];
    glDisable( GL_DEPTH_TEST );
    glBindVertexArray( vertex_array );
    glUseProgram( program );

    glBindTextureUnit(
        GL_SHADER_PROGRAM_FRAMEBUFFER_TEXTURE_BINDING,
        ctx->framebuffer_main.color_texture_id );
    glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void gl_renderer_backend_on_resize( RendererContext* renderer_ctx ) {
    OpenGLRendererContext* ctx = renderer_ctx;
    gl_camera_buffer_update_matrix_ui(
        ctx->buffers[GL_BUFFER_INDEX_UBO_CAMERA],
        &ctx->ctx.projection_ui
    );

    /// Redraw the framebuffer in new dimensions
    gl_draw_framebuffer( ctx, ctx->ctx.surface_dimensions );
    platform_gl_surface_swap_buffers( ctx->ctx.surface );
}

b32 gl_renderer_backend_begin_frame(
    RendererContext* renderer_ctx, RenderData* render_data
) {
    OpenGLRendererContext* ctx = renderer_ctx;
    GLFramebufferID   framebuffer  = 0;

    struct Camera* camera = render_data->camera;
#if defined(LD_ASSERTIONS)
    if( camera ) {
        LOG_ASSERT(
            camera->transform,
            "All cameras passed into renderer MUST have a transform!"
        );
    }
#endif

    if( camera && camera->transform->camera_dirty ) {

        vec3 camera_world_position =
            transform_world_position( camera->transform );
            
        vec3 camera_world_forward_basis =
            transform_world_forward_basis( camera->transform );
        vec3 camera_world_up_basis =
            transform_world_up_basis( camera->transform );

        vec3 camera_target = v3_add(
            camera_world_position, camera_world_forward_basis );

        mat4 lookat = m4_lookat(
            camera_world_position,
            camera_target,
            camera_world_up_basis
        );
        mat4 view_projection =
            m4_mul_m4( &lookat, &ctx->ctx.projection_3d );
        gl_camera_buffer_update_world_position(
            ctx->buffers[GL_BUFFER_INDEX_UBO_CAMERA],
            camera_world_position
        );
        gl_camera_buffer_update_near_far_planes(
            ctx->buffers[GL_BUFFER_INDEX_UBO_CAMERA],
            &camera->near_clip
        );
        gl_camera_buffer_update_matrix_3d(
            ctx->buffers[GL_BUFFER_INDEX_UBO_CAMERA],
            &view_projection
        );

        camera->transform->camera_dirty = false;
    }

    ivec2 resolution = ctx->ctx.framebuffer_dimensions;

    // NOTE(alicia): recreate the framebuffer to match
    // render resolution.
    if( !iv2_cmp_eq(
        resolution,
        ctx->framebuffer_main.dimensions
    ) ) {
        /// Rescale the framebuffer
        gl_framebuffer_resize(
            &ctx->framebuffer_main,
            resolution.width,
            resolution.height
        );
    }

    framebuffer = ctx->framebuffer_main.id;
    glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
    glBindTextureUnit( GL_SHADER_PROGRAM_FRAMEBUFFER_TEXTURE_BINDING, 0 );
    glViewport(
        0, 0,
        resolution.width,
        resolution.height
    );
    glEnable( GL_CULL_FACE );
    rgba clear_color = RGBA_GRAY;
    f32 clear_depth  = 1.0f;
    glClearNamedFramebufferfv(
        framebuffer,
        GL_COLOR, 0,
        clear_color.c
    );
    glClearNamedFramebufferfv(
        framebuffer,
        GL_DEPTH, 0,
        &clear_depth
    );

    // NOTE(alicia): UI Rendering
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    for( usize i = 0; i < render_data->object_count; ++i ) {
        RenderObject* object = render_data->objects + i;
        GLShaderProgramID program_id = 0;

        switch( object->material.shader ) {
            case RENDER_SHADER_DEBUG_COLOR: {
                program_id = ctx->programs[GL_SHADER_PROGRAM_INDEX_COLOR];
                glUseProgram( program_id );
                glProgramUniform4fv(
                    program_id,
                    GL_SHADER_PROGRAM_COLOR_LOCATION_COLOR,
                    1, object->material.debug_color.color.c
                );
            } break;
            default: continue;
        }

        switch( object->mesh ) {
            case RENDER_MESH_QUAD_2D_LOWER_LEFT: {
                glBindVertexArray(
                    ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_QUAD_2D] );
            } break;
            default: continue;
        }

        glProgramUniformMatrix4fv(
            program_id,
            GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE,
            object->material.transform.c
        );
        glDrawElements(
            GL_TRIANGLES,
            QUAD_2D_INDEX_COUNT,
            GL_UNSIGNED_BYTE,
            NULL
        );
    }

    return true;
}
b32 gl_renderer_backend_end_frame(
    RendererContext* renderer_ctx, RenderData* render_data
) {
    OpenGLRendererContext* ctx = renderer_ctx;

    ivec2 surface_dimensions = ctx->ctx.surface_dimensions;
    gl_draw_framebuffer( ctx, surface_dimensions );
    platform_gl_surface_swap_buffers( ctx->ctx.surface );

    unused(render_data);
    return true;
}

internal void gl_init_buffers( OpenGLRendererContext* ctx ) {
    glCreateBuffers( GL_BUFFER_COUNT, ctx->buffers );
    /* create matrices buffer */ {
        GLBufferID ubo = ctx->buffers[GL_BUFFER_INDEX_UBO_CAMERA];

        struct GLCameraBuffer buffer = {};
        buffer.camera_near = 0.001f;
        buffer.camera_far  = 1000.0f;

        ivec2 framebuffer_dimensions = ctx->ctx.framebuffer_dimensions;

        f32 aspect_ratio =
            (f32)framebuffer_dimensions.width /
            (f32)framebuffer_dimensions.height;

        mat4 lookat = m4_lookat( VEC3_BACK, VEC3_ZERO, VEC3_UP );
        mat4 projection = m4_perspective(
            to_rad(60.0f), aspect_ratio,
            buffer.camera_near,
            buffer.camera_far
        );

        buffer.matrix_3d = m4_mul_m4( &lookat, &projection );
        mat4 view_ui = m4_lookat_2d( VEC2_ZERO, VEC2_UP );
        mat4 proj_ui = m4_ortho(
            0.0f, (f32)framebuffer_dimensions.width,
            0.0f, (f32)framebuffer_dimensions.height,
            -1.0f, 1.0f
        );

        buffer.matrix_ui = m4_mul_m4( &view_ui, &proj_ui );

        gl_camera_buffer_create( ubo, &buffer );
    }

    glCreateVertexArrays( GL_VERTEX_ARRAY_COUNT, ctx->vertex_arrays );
    /* create quad 2d mesh */ {
        GLuint vao = ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_QUAD_2D];
        GLuint vbo = ctx->buffers[GL_BUFFER_INDEX_VBO_QUAD_2D];
        GLuint ebo = ctx->buffers[GL_BUFFER_INDEX_EBO_QUAD];
        
        glNamedBufferStorage(
            vbo, QUAD_2D_VERTEX_BUFFER_SIZE,
            QUAD_2D_LOWER_LEFT,
            GL_DYNAMIC_STORAGE_BIT
        );
        glNamedBufferStorage(
            ebo, QUAD_2D_INDEX_BUFFER_SIZE,
            QUAD_2D_INDICES,
            GL_DYNAMIC_STORAGE_BIT
        );

        glVertexArrayVertexBuffer(
            vao, 0,
            vbo, 0,
            sizeof(struct Vertex2D)
        );
        glVertexArrayElementBuffer( vao, ebo );

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
    }
    /* create framebuffer quad */ {
        GLuint vao = ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER];
        GLuint vbo = ctx->buffers[GL_BUFFER_INDEX_VBO_FRAMEBUFFER];
     
        f32 FRAMEBUFFER_VERTICES[] = {
            -1.0f, -1.0f, /* uvs */ 0.0f, 0.0f,
             1.0f,  1.0f, /* uvs */ 1.0f, 1.0f,
            -1.0f,  1.0f, /* uvs */ 0.0f, 1.0f,

            -1.0f, -1.0f, /* uvs */ 0.0f, 0.0f,
             1.0f, -1.0f, /* uvs */ 1.0f, 0.0f,
             1.0f,  1.0f, /* uvs */ 1.0f, 1.0f,
        };

        usize vbo_size = STATIC_ARRAY_SIZE( FRAMEBUFFER_VERTICES );
        glNamedBufferStorage(
            vbo, vbo_size,
            FRAMEBUFFER_VERTICES,
            GL_DYNAMIC_STORAGE_BIT
        );

        glVertexArrayVertexBuffer(
            vao, 0,
            vbo, 0,
            sizeof(f32) * 4
        );

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
            sizeof(f32) * 2
        );

        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribBinding( vao, 1, 0 );
    }

}

internal void gl_init_shaders( OpenGLRendererContext* ctx ) {

    /* framebuffer shader */ {
        #define FRAMEBUFFER_SHADER_STAGE_COUNT (2)
        GLShaderID framebuffer_shaders[FRAMEBUFFER_SHADER_STAGE_COUNT] = {};
        GLShaderProgramID* program = ctx->programs + GL_SHADER_PROGRAM_INDEX_FRAMEBUFFER;

        b32 result = gl_shader_compile_source(
            GL_FRAMEBUFFER_SHADER_VERT_SOURCE_LENGTH,
            GL_FRAMEBUFFER_SHADER_VERT_SOURCE,
            GL_VERTEX_SHADER,
            &framebuffer_shaders[0]
        );
        ASSERT( result );

        result = gl_shader_compile_source(
            GL_FRAMEBUFFER_SHADER_FRAG_SOURCE_LENGTH,
            GL_FRAMEBUFFER_SHADER_FRAG_SOURCE,
            GL_FRAGMENT_SHADER,
            &framebuffer_shaders[1]
        );
        ASSERT( result );

        result = gl_shader_program_link(
            FRAMEBUFFER_SHADER_STAGE_COUNT, framebuffer_shaders,
            program
        );
        ASSERT( result );

        gl_shader_delete(
            FRAMEBUFFER_SHADER_STAGE_COUNT, framebuffer_shaders );

        GL_LOG_NOTE(
            "Successfully compiled + "
            "linked framebuffer shader program: {u32}",
            *program
        );

    }

    /* create ui color shader */ {
        #define COLOR_SHADER_STAGE_COUNT (2)
        GLShaderID color_shaders[COLOR_SHADER_STAGE_COUNT] = {};
        GLShaderProgramID* program = ctx->programs + GL_SHADER_PROGRAM_INDEX_COLOR;

        PlatformFile* color_vert_file = platform_file_open(
            "./resources/shaders/ldcolor.vert.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        ASSERT( color_vert_file );
        PlatformFile* color_frag_file = platform_file_open(
            "./resources/shaders/ldcolor.frag.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        ASSERT( color_frag_file );

        usize color_vert_file_size =
            platform_file_query_size( color_vert_file );
        usize color_frag_file_size =
            platform_file_query_size( color_frag_file );
        
        usize shader_buffer_size =
            color_vert_file_size + color_frag_file_size;
        u8* shader_buffer = ldalloc( shader_buffer_size, MEMORY_TYPE_RENDERER );
        ASSERT( shader_buffer );

        b32 result = platform_file_read(
            color_vert_file,
            color_vert_file_size,
            color_vert_file_size,
            shader_buffer
        );
        ASSERT( result );

        result = platform_file_read(
            color_frag_file,
            color_frag_file_size,
            color_frag_file_size,
            shader_buffer + color_vert_file_size
        );
        ASSERT( result );


        result = gl_shader_compile_spirv(
            color_vert_file_size,
            shader_buffer,
            GL_VERTEX_SHADER,
            "main",
            0, 0, 0,
            &color_shaders[0]
        );
        ASSERT( result );

        result = gl_shader_compile_spirv(
            color_frag_file_size,
            shader_buffer + color_vert_file_size,
            GL_FRAGMENT_SHADER,
            "main",
            0, 0, 0,
            &color_shaders[1]
        );
        ASSERT( result );

        result = gl_shader_program_link(
            COLOR_SHADER_STAGE_COUNT, color_shaders,
            program
        );
        ASSERT( result );

        gl_shader_delete( COLOR_SHADER_STAGE_COUNT, color_shaders );
        ldfree( shader_buffer, shader_buffer_size, MEMORY_TYPE_RENDERER );
        platform_file_close( color_vert_file );
        platform_file_close( color_frag_file );

        GL_LOG_NOTE(
            "Successfully compiled + "
            "linked debug color shader program: {u32}",
            *program
        );
    }

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

