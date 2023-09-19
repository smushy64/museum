// * Description:  OpenGL Backend
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "renderer.h"
#include "renderer/opengl.h"
#include "renderer/opengl/types.h"
#include "renderer/opengl/functions.h"
#include "renderer/opengl/buffer.h"
#include "renderer/opengl/texture.h"
#include "core/graphics.h"
#include "core/graphics/primitives.h"
#include "core/graphics/types.h"
#include "core/mathf.h"
#include "core/mem.h"
#include "core/timer.h"
#include "platform.h"
#include "core/input.h"

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
internal void gl_init_textures( OpenGLRendererContext* ctx );

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
    gl_init_textures( ctx );

    GLFramebuffer* main_fbo =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_MAIN_FRAMEBUFFER;

    *main_fbo = gl_framebuffer_create(
            ctx->ctx.framebuffer_dimensions.width,
            ctx->ctx.framebuffer_dimensions.height );

    GLFramebuffer* shadow_directional_fbo =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL;

    *shadow_directional_fbo = gl_shadowbuffer_create(
        1024, 1024, GL_SHADOWBUFFER_DIRECTIONAL );

    for( u32 i = 0; i < 4; ++i ) {
        GLFramebuffer* shadow_point_fbo =
            ctx->framebuffers + GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0 + i;
        *shadow_point_fbo =
            gl_shadowbuffer_create( 1024, 1024, GL_SHADOWBUFFER_POINT );
    }

    glSwapInterval( ctx->ctx.surface, 1 );

    ctx->ctx.mesh_null              =
        ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_CUBE_3D];
    ctx->ctx.texture_diffuse_null   =
        ctx->textures_2d[GL_TEXTURE_INDEX_NULL_DIFFUSE].id;
    ctx->ctx.texture_normal_null    =
        ctx->textures_2d[GL_TEXTURE_INDEX_NULL_NORMAL].id;
    ctx->ctx.texture_roughness_null =
        ctx->textures_2d[GL_TEXTURE_INDEX_NULL_ROUGHNESS].id;
    ctx->ctx.texture_metallic_null  =
        ctx->textures_2d[GL_TEXTURE_INDEX_NULL_ROUGHNESS].id;

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
        ctx->programs[GL_SHADER_PROGRAM_INDEX_POST_PROCESS];
    GLVertexArrayID vertex_array =
        ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER];
    glBindVertexArray( vertex_array );
    glUseProgram( program );

    GLFramebuffer* main_fbo =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_MAIN_FRAMEBUFFER;
    glBindTextureUnit(
        GL_SHADER_PROGRAM_POST_PROCESS_RENDER_TEXTURE_BINDING,
        main_fbo->color_texture_id );

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

    gl_data_buffer_set_time(
        ctx->buffers[GL_BUFFER_INDEX_UBO_DATA],
        render_data->elapsed_time,
        render_data->delta_time,
        render_data->frame_count );

    GLFramebuffer* fbo_main =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_MAIN_FRAMEBUFFER;
    GLFramebuffer* fbo_shadow_directional =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL;
    GLFramebuffer* fbo_shadow_point =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0;

    struct Camera* camera = render_data->camera;
#if defined(LD_ASSERTIONS)
    if( camera ) {
        LOG_ASSERT(
            camera->transform,
            "All cameras passed into renderer MUST have a transform!"
        );
    }
#endif
    ivec2 resolution = ctx->ctx.framebuffer_dimensions;

    if(
        camera &&
        (camera->transform->camera_dirty || ctx->ctx.projection3d_dirty)
    ) {
        vec3 camera_world_position =
            transform_world_position( camera->transform );
        quat camera_world_rotation =
            transform_world_rotation( camera->transform );
            
        vec3 camera_world_forward =
            q_mul_v3( camera_world_rotation, VEC3_FORWARD );
        vec3 camera_world_up =
            q_mul_v3( camera_world_rotation, VEC3_UP );

        mat4 projection = m4_perspective(
            camera->fov_radians,
            (f32)resolution.x / (f32)resolution.y,
            camera->near_clip,
            camera->far_clip );

        mat4 view = m4_view(
            camera_world_position,
            v3_sub( camera_world_position, camera_world_forward ),
            camera_world_up );

        mat4 view_projection = m4_mul_m4( &projection, &view );

        GLBufferID ubo = ctx->buffers[GL_BUFFER_INDEX_UBO_CAMERA];
        gl_camera_buffer_update_matrix_3d(
            ubo, &view_projection );
        gl_camera_buffer_update_world_position(
            ubo, camera_world_position );
        gl_camera_buffer_update_near_far_planes(
            ubo, camera->clipping_planes );

        camera->transform->camera_dirty = false;
        ctx->ctx.projection3d_dirty     = false;
    }


    // NOTE(alicia): recreate the framebuffer to match
    // render resolution.
    if( !iv2_cmp(
        resolution,
        fbo_main->dimensions
    ) ) {
        /// Rescale the framebuffer
        gl_framebuffer_resize(
            fbo_main,
            resolution.width,
            resolution.height
        );
    }

    glBindTextureUnit(
        GL_SHADER_PROGRAM_POST_PROCESS_RENDER_TEXTURE_BINDING, 0 );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_DIRECTIONAL_SHADOW_MAP, 0 );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_0, 0 );
    glBindTextureUnit( 
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_1, 0 );
    glBindTextureUnit( 
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_2, 0 );
    glBindTextureUnit( 
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_3, 0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    rgba clear_color = RGBA_BLACK;
    f32  clear_depth = 1.0f;

    GLShaderProgramID sh_phong =
        ctx->programs[GL_SHADER_PROGRAM_INDEX_PHONG_BRDF];
    GLShaderProgramID sh_shadow_directional =
        ctx->programs[GL_SHADER_PROGRAM_INDEX_SHADOW_DIRECTIONAL];
    GLShaderProgramID sh_shadow_point =
        ctx->programs[GL_SHADER_PROGRAM_INDEX_SHADOW_POINT];

    glClearNamedFramebufferfv(
        fbo_main->id, GL_COLOR, 0, clear_color.c );
    glClearNamedFramebufferfv(
        fbo_main->id, GL_DEPTH, 0, &clear_depth );
    glClearNamedFramebufferfv(
        fbo_shadow_directional->id, GL_DEPTH, 0, &clear_depth );
    glClearNamedFramebufferfv(
        (fbo_shadow_point + 0)->id, GL_DEPTH, 0, &clear_depth );
    glClearNamedFramebufferfv(
        (fbo_shadow_point + 1)->id, GL_DEPTH, 0, &clear_depth );
    glClearNamedFramebufferfv(
        (fbo_shadow_point + 2)->id, GL_DEPTH, 0, &clear_depth );
    glClearNamedFramebufferfv(
        (fbo_shadow_point + 3)->id, GL_DEPTH, 0, &clear_depth );

    // draw shadows
    for( usize i = 0; i < render_data->draw_command_count; ++i ) {
        DrawCommand* current = render_data->draw_commands + i;
        if(
            !bitfield_check( current->flags, DRAW_FLAG_SHADOW_CASTER ) ||
            bitfield_check( current->flags, DRAW_FLAG_IS_WIREFRAME )
        ) {
            continue;
        }
        glBindFramebuffer( GL_FRAMEBUFFER, fbo_shadow_directional->id );
        glViewport( 0, 0,
            fbo_shadow_directional->width, fbo_shadow_directional->height );

        glBindVertexArray( current->mesh );

        glProgramUniformMatrix4fv(
            sh_shadow_directional,
            GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE,
            current->transform->c );
        glProgramUniformMatrix4fv(
            sh_shadow_point,
            GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE,
            current->transform->c );

        glUseProgram( sh_shadow_directional );

        // TODO(alicia): index count!!
        glDrawElements(
            GL_TRIANGLES,
            CUBE_3D_INDEX_COUNT,
            GL_UNSIGNED_BYTE,
            NULL );

        glUseProgram( sh_shadow_point );
        for( u32 i = 0; i < 4; ++i ) {
            if( !gl_light_buffer_point_is_active( &ctx->lights, i ) ) {
                continue;
            }
            GLFramebuffer* fbo_current = fbo_shadow_point + i;
            glBindFramebuffer( GL_FRAMEBUFFER, fbo_current->id );
            glViewport( 0, 0,
                fbo_current->width, fbo_current->height );

            glProgramUniform1i( sh_shadow_point,
                GL_SHADER_PROGRAM_SHADOW_POINT_LOCATION_POINT_INDEX, i );

            // TODO(alicia): index count!!
            glDrawElements(
                GL_TRIANGLES,
                CUBE_3D_INDEX_COUNT,
                GL_UNSIGNED_BYTE,
                NULL );
        }

    }
    glBindFramebuffer( GL_FRAMEBUFFER, fbo_main->id );
    glViewport( 0, 0, fbo_main->width, fbo_main->height );
    glUseProgram( sh_phong );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_DIRECTIONAL_SHADOW_MAP,
        fbo_shadow_directional->shadow_texture_id );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_0,
        (fbo_shadow_point + 0)->shadow_texture_id );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_1,
        (fbo_shadow_point + 1)->shadow_texture_id );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_2,
        (fbo_shadow_point + 2)->shadow_texture_id );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_3,
        (fbo_shadow_point + 3)->shadow_texture_id );

    // draw proper
    for( usize i = 0; i < render_data->draw_command_count; ++i ) {
        // TODO(alicia): transparency

        DrawCommand* current = render_data->draw_commands + i;
        glBindVertexArray( current->mesh );

        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_DIFFUSE_TEXTURE,
            current->texture_diffuse );
        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_NORMAL_TEXTURE,
            current->texture_normal );
        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_ROUGHNESS_TEXTURE,
            current->texture_roughness );
        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_METALLIC_TEXTURE,
            current->texture_metallic );
 
        glProgramUniform3fv(
            sh_phong, GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_TINT,
            1, current->tint.c );
        glProgramUniform1i(
            sh_phong,
            GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_SHADOW_RECEIVER,
            bitfield_check( current->flags, DRAW_FLAG_SHADOW_RECEIVER ) );
        glProgramUniformMatrix4fv(
            sh_phong, GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE, current->transform->c );
        mat3 normal_mat = m4_normal_matrix_unchecked( current->transform );
        glProgramUniformMatrix3fv(
            sh_phong, GL_SHADER_PROGRAM_LOCATION_NORMAL_TRANSFORM,
            1, GL_FALSE, normal_mat.c );
        // TODO(alicia): index count!!

        GLenum mode = GL_TRIANGLES;
        if( bitfield_check( current->flags, DRAW_FLAG_IS_WIREFRAME ) ) {
            mode = GL_LINES;
        }
        glDrawElements(
            mode,
            CUBE_3D_INDEX_COUNT,
            GL_UNSIGNED_BYTE,
            NULL
        );
    }

    gl_light_buffer_point_set(
        ctx->buffers[GL_BUFFER_INDEX_UBO_LIGHTS], &ctx->lights, 0,
        v3( -2.0f, 1.0f, 0.0f ),
        RGB_WHITE, true );

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

        mat4 lookat = m4_view( VEC3_BACK, VEC3_ZERO, VEC3_UP );
        mat4 projection = m4_perspective(
            to_rad32(60.0f), aspect_ratio,
            buffer.camera_near,
            buffer.camera_far
        );

        buffer.matrix_3d = m4_mul_m4( &lookat, &projection );
        mat4 view_ui = m4_view_2d( VEC2_ZERO, VEC2_UP );
        mat4 proj_ui = m4_ortho(
            0.0f, (f32)framebuffer_dimensions.width,
            0.0f, (f32)framebuffer_dimensions.height,
            -1.0f, 1.0f
        );

        buffer.matrix_ui = m4_mul_m4( &view_ui, &proj_ui );

        gl_camera_buffer_create( ubo, &buffer );
    }
    /* create lights buffer */ {
        GLBufferID ubo = ctx->buffers[GL_BUFFER_INDEX_UBO_LIGHTS];
        struct GLLightBuffer* buffer = &ctx->lights;

        mem_zero( buffer, sizeof( struct GLLightBuffer ) );
        gl_light_buffer_create( ubo, buffer );
        gl_light_buffer_directional_set(
            ubo, buffer,
            v3( -1.0f, -1.0f, -1.0f ),
            RGB_GRAY );
    }
    /* create data buffer */ {
        GLBufferID ubo = ctx->buffers[GL_BUFFER_INDEX_UBO_DATA];
        gl_data_buffer_create( ubo, NULL );
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

        usize vbo_size = static_array_size( FRAMEBUFFER_VERTICES );
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
    /* create cube 3d mesh */ {
        GLuint vao = ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_CUBE_3D];
        GLuint vbo = ctx->buffers[GL_BUFFER_INDEX_VBO_CUBE_3D];
        GLuint ebo = ctx->buffers[GL_BUFFER_INDEX_EBO_CUBE_3D];
        
        glNamedBufferStorage(
            vbo, CUBE_3D_VERTEX_BUFFER_SIZE,
            CUBE_3D,
            GL_DYNAMIC_STORAGE_BIT
        );
        glNamedBufferStorage(
            ebo, CUBE_3D_INDEX_BUFFER_SIZE,
            CUBE_3D_INDICES,
            GL_DYNAMIC_STORAGE_BIT
        );

        glVertexArrayVertexBuffer(
            vao, 0,
            vbo, 0,
            sizeof(struct Vertex3D)
        );
        glVertexArrayElementBuffer( vao, ebo );

        glEnableVertexArrayAttrib( vao, VERTEX_3D_LOCATION_POSITION );
        glEnableVertexArrayAttrib( vao, VERTEX_3D_LOCATION_UV );
        glEnableVertexArrayAttrib( vao, VERTEX_3D_LOCATION_NORMAL );
        glEnableVertexArrayAttrib( vao, VERTEX_3D_LOCATION_COLOR );
        glEnableVertexArrayAttrib( vao, VERTEX_3D_LOCATION_TANGENT );

        GLuint offset = 0;
        glVertexArrayAttribFormat(
            vao, VERTEX_3D_LOCATION_POSITION,
            3, GL_FLOAT,
            GL_FALSE,
            offset
        );
        offset += sizeof(vec3);
        glVertexArrayAttribFormat(
            vao, VERTEX_3D_LOCATION_UV,
            2, GL_FLOAT,
            GL_FALSE,
            offset
        );
        offset += sizeof(vec2);
        glVertexArrayAttribFormat(
            vao, VERTEX_3D_LOCATION_NORMAL,
            3, GL_FLOAT,
            GL_FALSE,
            offset
        );
        offset += sizeof(vec3);
        glVertexArrayAttribFormat(
            vao, VERTEX_3D_LOCATION_COLOR,
            4, GL_FLOAT,
            GL_FALSE,
            offset
        );
        offset += sizeof(vec4);
        glVertexArrayAttribFormat(
            vao, VERTEX_3D_LOCATION_TANGENT,
            3, GL_FLOAT,
            GL_FALSE,
            offset
        );

        glVertexArrayAttribBinding( vao, VERTEX_3D_LOCATION_POSITION, 0 );
        glVertexArrayAttribBinding( vao, VERTEX_3D_LOCATION_UV, 0 );
        glVertexArrayAttribBinding( vao, VERTEX_3D_LOCATION_NORMAL, 0 );
        glVertexArrayAttribBinding( vao, VERTEX_3D_LOCATION_COLOR, 0 );
        glVertexArrayAttribBinding( vao, VERTEX_3D_LOCATION_TANGENT, 0 );
    }

}

internal void gl_init_shaders( OpenGLRendererContext* ctx ) {

    /* create post process shader */ {
        #define POST_PROCESS_SHADER_STAGE_COUNT (2)
        GLShaderID post_process_shaders[POST_PROCESS_SHADER_STAGE_COUNT] = {};
        GLShaderProgramID* program =
            ctx->programs + GL_SHADER_PROGRAM_INDEX_POST_PROCESS;

        PlatformFile* post_process_vert_file = platform_file_open(
            "./resources/shaders/post_process.vert.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( post_process_vert_file );
        PlatformFile* post_process_frag_file = platform_file_open(
            "./resources/shaders/post_process.frag.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( post_process_frag_file );

        usize post_process_vert_file_size =
            platform_file_query_size( post_process_vert_file );
        usize post_process_frag_file_size =
            platform_file_query_size( post_process_frag_file );
        
        usize shader_buffer_size =
            post_process_vert_file_size + post_process_frag_file_size;
        u8* shader_buffer = ldalloc( shader_buffer_size, MEMORY_TYPE_RENDERER );
        assert( shader_buffer );

        b32 result = platform_file_read(
            post_process_vert_file,
            post_process_vert_file_size,
            post_process_vert_file_size,
            shader_buffer
        );
        assert( result );

        result = platform_file_read(
            post_process_frag_file,
            post_process_frag_file_size,
            post_process_frag_file_size,
            shader_buffer + post_process_vert_file_size
        );
        assert( result );


        result = gl_shader_compile_spirv(
            post_process_vert_file_size,
            shader_buffer,
            GL_VERTEX_SHADER,
            "main",
            0, 0, 0,
            &post_process_shaders[0]
        );
        assert( result );

        result = gl_shader_compile_spirv(
            post_process_frag_file_size,
            shader_buffer + post_process_vert_file_size,
            GL_FRAGMENT_SHADER,
            "main",
            0, 0, 0,
            &post_process_shaders[1]
        );
        assert( result );

        result = gl_shader_program_link(
            POST_PROCESS_SHADER_STAGE_COUNT, post_process_shaders,
            program
        );
        assert( result );

        gl_shader_delete(
            POST_PROCESS_SHADER_STAGE_COUNT, post_process_shaders );
        ldfree( shader_buffer, shader_buffer_size, MEMORY_TYPE_RENDERER );
        platform_file_close( post_process_vert_file );
        platform_file_close( post_process_frag_file );

        GL_LOG_NOTE(
            "Successfully compiled + "
            "linked post process shader program: {u32}",
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
        assert( color_vert_file );
        PlatformFile* color_frag_file = platform_file_open(
            "./resources/shaders/ldcolor.frag.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( color_frag_file );

        usize color_vert_file_size =
            platform_file_query_size( color_vert_file );
        usize color_frag_file_size =
            platform_file_query_size( color_frag_file );
        
        usize shader_buffer_size =
            color_vert_file_size + color_frag_file_size;
        u8* shader_buffer = ldalloc( shader_buffer_size, MEMORY_TYPE_RENDERER );
        assert( shader_buffer );

        b32 result = platform_file_read(
            color_vert_file,
            color_vert_file_size,
            color_vert_file_size,
            shader_buffer
        );
        assert( result );

        result = platform_file_read(
            color_frag_file,
            color_frag_file_size,
            color_frag_file_size,
            shader_buffer + color_vert_file_size
        );
        assert( result );


        result = gl_shader_compile_spirv(
            color_vert_file_size,
            shader_buffer,
            GL_VERTEX_SHADER,
            "main",
            0, 0, 0,
            &color_shaders[0]
        );
        assert( result );

        result = gl_shader_compile_spirv(
            color_frag_file_size,
            shader_buffer + color_vert_file_size,
            GL_FRAGMENT_SHADER,
            "main",
            0, 0, 0,
            &color_shaders[1]
        );
        assert( result );

        result = gl_shader_program_link(
            COLOR_SHADER_STAGE_COUNT, color_shaders,
            program
        );
        assert( result );

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

    /* create phong brdf shader */ {
        #define PHONG_BRDF_SHADER_STAGE_COUNT (2)
        GLShaderID phong_shaders[PHONG_BRDF_SHADER_STAGE_COUNT] = {};
        GLShaderProgramID* program =
            ctx->programs + GL_SHADER_PROGRAM_INDEX_PHONG_BRDF;

        PlatformFile* phong_vert_file = platform_file_open(
            "./resources/shaders/phong.vert.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( phong_vert_file );
        PlatformFile* phong_frag_file = platform_file_open(
            "./resources/shaders/phong.frag.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( phong_frag_file );

        usize phong_vert_file_size =
            platform_file_query_size( phong_vert_file );
        assert(phong_vert_file_size);
        usize phong_frag_file_size =
            platform_file_query_size( phong_frag_file );
        assert(phong_frag_file_size);
       
        usize shader_buffer_size =
            phong_vert_file_size + phong_frag_file_size;
        u8* shader_buffer = ldalloc( shader_buffer_size, MEMORY_TYPE_RENDERER );
        assert( shader_buffer );

        b32 result = platform_file_read(
            phong_vert_file,
            phong_vert_file_size,
            phong_vert_file_size,
            shader_buffer
        );
        assert( result );

        result = platform_file_read(
            phong_frag_file,
            phong_frag_file_size,
            phong_frag_file_size,
            shader_buffer + phong_vert_file_size
        );
        assert( result );


        result = gl_shader_compile_spirv(
            phong_vert_file_size,
            shader_buffer,
            GL_VERTEX_SHADER,
            "main",
            0, 0, 0,
            &phong_shaders[0]
        );
        assert( result );

        result = gl_shader_compile_spirv(
            phong_frag_file_size,
            shader_buffer + phong_vert_file_size,
            GL_FRAGMENT_SHADER,
            "main",
            0, 0, 0,
            &phong_shaders[1]
        );
        assert( result );

        result = gl_shader_program_link(
            PHONG_BRDF_SHADER_STAGE_COUNT, phong_shaders,
            program
        );
        assert( result );

        gl_shader_delete( PHONG_BRDF_SHADER_STAGE_COUNT, phong_shaders );
        ldfree( shader_buffer, shader_buffer_size, MEMORY_TYPE_RENDERER );
        platform_file_close( phong_vert_file );
        platform_file_close( phong_frag_file );

        GL_LOG_NOTE(
            "Successfully compiled + "
            "linked phong brdf shader program: {u32}",
            *program
        );
    }

    /* create shadow directional shader */ {
        #define SHADOW_DIRECTIONAL_SHADER_STAGE_COUNT (2)
        GLShaderID shadow_shaders[SHADOW_DIRECTIONAL_SHADER_STAGE_COUNT] = {};
        GLShaderProgramID* program =
            ctx->programs + GL_SHADER_PROGRAM_INDEX_SHADOW_DIRECTIONAL;

        PlatformFile* shadow_vert_file = platform_file_open(
            "./resources/shaders/shadow_directional.vert.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( shadow_vert_file );
        PlatformFile* shadow_frag_file = platform_file_open(
            "./resources/shaders/shadow_directional.frag.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( shadow_frag_file );

        usize shadow_vert_file_size =
            platform_file_query_size( shadow_vert_file );
        assert(shadow_vert_file_size);
        usize shadow_frag_file_size =
            platform_file_query_size( shadow_frag_file );
        assert(shadow_frag_file_size);
       
        usize shader_buffer_size =
            shadow_vert_file_size + shadow_frag_file_size;
        u8* shader_buffer =
            ldalloc( shader_buffer_size, MEMORY_TYPE_RENDERER );
        assert( shader_buffer );

        b32 result = platform_file_read(
            shadow_vert_file,
            shadow_vert_file_size,
            shadow_vert_file_size,
            shader_buffer
        );
        assert( result );

        result = platform_file_read(
            shadow_frag_file,
            shadow_frag_file_size,
            shadow_frag_file_size,
            shader_buffer + shadow_vert_file_size
        );
        assert( result );


        result = gl_shader_compile_spirv(
            shadow_vert_file_size,
            shader_buffer,
            GL_VERTEX_SHADER,
            "main",
            0, 0, 0,
            &shadow_shaders[0]
        );
        assert( result );

        result = gl_shader_compile_spirv(
            shadow_frag_file_size,
            shader_buffer + shadow_vert_file_size,
            GL_FRAGMENT_SHADER,
            "main",
            0, 0, 0,
            &shadow_shaders[1]
        );
        assert( result );

        result = gl_shader_program_link(
            SHADOW_DIRECTIONAL_SHADER_STAGE_COUNT, shadow_shaders,
            program
        );
        assert( result );

        gl_shader_delete(
            SHADOW_DIRECTIONAL_SHADER_STAGE_COUNT, shadow_shaders );
        ldfree( shader_buffer, shader_buffer_size, MEMORY_TYPE_RENDERER );
        platform_file_close( shadow_vert_file );
        platform_file_close( shadow_frag_file );

        GL_LOG_NOTE(
            "Successfully compiled + "
            "linked shadow directional shader program: {u32}",
            *program
        );
    }

    /* create shadow point shader */ {
        #define SHADOW_POINT_SHADER_STAGE_COUNT (3)
        GLShaderID shadow_shaders[SHADOW_POINT_SHADER_STAGE_COUNT] = {};
        GLShaderProgramID* program =
            ctx->programs + GL_SHADER_PROGRAM_INDEX_SHADOW_POINT;

        PlatformFile* shadow_vert_file = platform_file_open(
            "./resources/shaders/shadow_point.vert.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( shadow_vert_file );
        PlatformFile* shadow_geom_file = platform_file_open(
            "./resources/shaders/shadow_point.geom.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( shadow_geom_file );
        PlatformFile* shadow_frag_file = platform_file_open(
            "./resources/shaders/shadow_point.frag.spv",
            PLATFORM_FILE_OPEN_READ |
            PLATFORM_FILE_OPEN_SHARE_READ
        );
        assert( shadow_frag_file );

        usize shadow_vert_file_size =
            platform_file_query_size( shadow_vert_file );
        assert(shadow_vert_file_size);
        usize shadow_geom_file_size =
            platform_file_query_size( shadow_geom_file );
        assert(shadow_geom_file_size);
        usize shadow_frag_file_size =
            platform_file_query_size( shadow_frag_file );
        assert(shadow_frag_file_size);
       
        usize shader_buffer_size =
            shadow_vert_file_size +
            shadow_geom_file_size +
            shadow_frag_file_size;
        u8* shader_buffer =
            ldalloc( shader_buffer_size, MEMORY_TYPE_RENDERER );
        assert( shader_buffer );

        b32 result = platform_file_read(
            shadow_vert_file,
            shadow_vert_file_size,
            shadow_vert_file_size,
            shader_buffer
        );
        assert( result );

        result = platform_file_read(
            shadow_geom_file,
            shadow_geom_file_size,
            shadow_geom_file_size,
            shader_buffer + shadow_vert_file_size
        );
        assert( result );

        result = platform_file_read(
            shadow_frag_file,
            shadow_frag_file_size,
            shadow_frag_file_size,
            shader_buffer + shadow_vert_file_size + shadow_geom_file_size
        );
        assert( result );

        result = gl_shader_compile_spirv(
            shadow_vert_file_size,
            shader_buffer,
            GL_VERTEX_SHADER,
            "main",
            0, 0, 0,
            &shadow_shaders[0]
        );
        assert( result );

        result = gl_shader_compile_spirv(
            shadow_geom_file_size,
            shader_buffer + shadow_vert_file_size,
            GL_GEOMETRY_SHADER,
            "main",
            0, 0, 0,
            &shadow_shaders[1]
        );
        assert( result );

        result = gl_shader_compile_spirv(
            shadow_frag_file_size,
            shader_buffer + shadow_vert_file_size + shadow_geom_file_size,
            GL_FRAGMENT_SHADER,
            "main",
            0, 0, 0,
            &shadow_shaders[2]
        );
        assert( result );

        result = gl_shader_program_link(
            SHADOW_POINT_SHADER_STAGE_COUNT, shadow_shaders,
            program
        );
        assert( result );

        gl_shader_delete( SHADOW_POINT_SHADER_STAGE_COUNT, shadow_shaders );
        ldfree( shader_buffer, shader_buffer_size, MEMORY_TYPE_RENDERER );
        platform_file_close( shadow_vert_file );
        platform_file_close( shadow_geom_file );
        platform_file_close( shadow_frag_file );

        GL_LOG_NOTE(
            "Successfully compiled + "
            "linked shadow point shader program: {u32}",
            *program
        );
    }


}

internal void gl_init_textures( OpenGLRendererContext* ctx ) {
    /* null diffuse texture */ {
        GLTexture2D* texture = ctx->textures_2d + GL_TEXTURE_INDEX_NULL_DIFFUSE;

        *texture = gl_texture_2d_create(
            NULL_DIFFUSE_TEXTURE_WIDTH,
            NULL_DIFFUSE_TEXTURE_HEIGHT, 0,
            GL_UNSIGNED_BYTE,
            GL_RGBA8,
            GL_RGBA,
            GL_REPEAT,
            GL_REPEAT,
            GL_NEAREST,
            GL_NEAREST,
            NULL_DIFFUSE_TEXTURE
        );
    }
    /* null normal texture */ {
        GLTexture2D* texture = ctx->textures_2d + GL_TEXTURE_INDEX_NULL_NORMAL;

        *texture = gl_texture_2d_create(
            NULL_NORMAL_TEXTURE_WIDTH,
            NULL_NORMAL_TEXTURE_HEIGHT, 0,
            GL_UNSIGNED_BYTE,
            GL_RGB8,
            GL_RGB,
            GL_REPEAT,
            GL_REPEAT,
            GL_NEAREST,
            GL_NEAREST,
            NULL_NORMAL_TEXTURE
        );
    }
    /* null roughness texture */ {
        GLTexture2D* texture =
            ctx->textures_2d + GL_TEXTURE_INDEX_NULL_ROUGHNESS;

        *texture = gl_texture_2d_create(
            NULL_ROUGHNESS_TEXTURE_WIDTH,
            NULL_ROUGHNESS_TEXTURE_HEIGHT, 0,
            GL_UNSIGNED_BYTE,
            GL_R8,
            GL_RED,
            GL_REPEAT,
            GL_REPEAT,
            GL_NEAREST,
            GL_NEAREST,
            NULL_ROUGHNESS_TEXTURE
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

