/**
 * Description:  OpenGL Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 24, 2023
*/
#include "defines.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/string.h"
#include "core/collections.h"
#include "core/fs.h"
#include "core/time.h"

#include "engine/logging.h"
#include "engine/graphics/primitives.h"
#include "engine/graphics/internal.h"
#include "engine/graphics/internal/opengl.h"
#include "engine/graphics/internal/opengl/types.h"
#include "engine/graphics/internal/opengl/functions.h"
#include "engine/graphics/internal/opengl/shader.h"

global struct OpenGLSubsystem* global_gl = NULL;

#define GL_VERTEX_3D_ATTRIBUTE_COUNT (5)
global struct GLVertexBufferLayout global_vertex3d_buffer_layout;
global GLint global_vertex3d_attribute_component_counts[GL_VERTEX_3D_ATTRIBUTE_COUNT] =
    { 3, 3, 3, 3, 2 };
global GLenum global_vertex3d_attribute_types[GL_VERTEX_3D_ATTRIBUTE_COUNT] =
    { GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT };

internal void gl_draw_framebuffer( ivec2 viewport );
internal GLenum gl_texture_type( GraphicsTextureType type );
internal GLenum gl_texture_format( GraphicsTextureFormat format );
internal GLenum gl_texture_internal_format(
    GraphicsTextureBaseType base_type, GraphicsTextureFormat format );
internal GLenum gl_texture_base_type( GraphicsTextureBaseType type );
internal GLenum gl_texture_wrap( GraphicsTextureWrap wrap );
internal GLenum gl_texture_minification_filter( GraphicsTextureFilter filter );
internal GLenum gl_texture_magnification_filter( GraphicsTextureFilter filter );

internal no_inline void gl_on_resize(void) {
    gl_draw_framebuffer( global_renderer->surface_dimensions );
    glSwapBuffers( global_renderer->surface );
}
internal no_inline b32 gl_begin_frame(void) {
    /* resize framebuffer */ {
        ivec2 fbo_main_dimensions =
            global_gl->fbo_dimensions[GL_FRAMEBUFFER_INDEX_POST_PROCESS];
        if( !iv2_cmp(
            global_renderer->framebuffer_dimensions,
            fbo_main_dimensions
        ) ) {
            gl_framebuffers_resize(
                GL_FRAMEBUFFER_INDEX_POST_PROCESS, 1,
                &global_renderer->framebuffer_dimensions );
        }
    }
    ivec2 resolution = global_gl->fbo_dimensions[GL_FRAMEBUFFER_INDEX_POST_PROCESS];

    Camera* camera = global_render_data->camera;
    /* update camera */
    if( camera ) {
        assert( camera->transform );

        if(
            global_renderer->projection_has_updated ||
            camera->transform->camera_dirty
        ) {
            struct OpenGLUniformBufferCamera buffer_camera = {};
            
            buffer_camera.world_position =
                transform_world_position( camera->transform );
            buffer_camera.near_clip     = camera->near_clip;
            buffer_camera.far_clip      = camera->far_clip;
            buffer_camera.field_of_view = camera->fov_radians;
            buffer_camera.aspect_ratio  =
                (f32)global_renderer->surface_dimensions.width /
                (f32)global_renderer->surface_dimensions.height;

            quat camera_rotation = transform_world_rotation( camera->transform );
            vec3 camera_forward  = q_mul_v3( camera_rotation, VEC3_FORWARD );
            vec3 camera_up       = q_mul_v3( camera_rotation, VEC3_UP );

            mat4 view =
                m4_view(
                    buffer_camera.world_position,
                    v3_sub( buffer_camera.world_position, camera_forward ),
                    camera_up );

            buffer_camera.view_projection_3d = m4_mul_m4(
                &global_renderer->projection, &view );

            glNamedBufferSubData(
                global_gl->ubo_camera,
                offsetof( struct OpenGLUniformBufferCamera, view_projection_3d ),
                sizeof( struct OpenGLUniformBufferCamera ) -
                    sizeof( buffer_camera.view_projection_ui ),
                &buffer_camera.view_projection_3d
            );

            camera->transform->camera_dirty         = false;
            global_renderer->projection_has_updated = false;
        }
    }

    /* update misc data */ {
        struct OpenGLUniformBufferData buffer_data = {};
        buffer_data.delta_seconds   = time_delta_seconds();
        buffer_data.elapsed_seconds = time_elapsed_seconds();
        buffer_data.frame_count     = time_query_update_count() % U32_MAX;

        buffer_data.surface_resolution = v2_iv2( global_renderer->surface_dimensions );
        buffer_data.aspect_ratio       =
            (f32)global_renderer->surface_dimensions.width /
            (f32)global_renderer->surface_dimensions.height;

        glNamedBufferSubData(
            global_gl->ubo_data, 0, sizeof( buffer_data ), &buffer_data );
    }

    /* process non-draw commands */ {
        struct RenderCommand* command = list_peek( &global_render_data->list_commands );
        while( command && command->type != RENDER_COMMAND_DRAW_3D ) {
            assert( list_pop( &global_render_data->list_commands ) );

            switch( command->type ) {
                case RENDER_COMMAND_DIRECTIONAL_LIGHT: {
                    struct OpenGLUniformBufferDirectionalLight buffer_light = {};
                    buffer_light.direction = command->directional_light.direction;
                    buffer_light.color     = command->directional_light.color;
                    buffer_light.is_active = command->directional_light.is_active;

#if defined(LD_ASSERTIONS)
                    f32 m = v3_mag( buffer_light.direction );
                    assert( m > 0.999f && m < 1.001f );
#endif

                    vec3 light_position = v3_neg( buffer_light.direction );

                    mat4 view = m4_view( light_position, VEC3_ZERO, VEC3_UP );
                    mat4 proj = m4_ortho(
                        -10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f );

                    buffer_light.matrix = m4_mul_m4( &proj, &view );

                    if( command->directional_light.is_active ) {
                        note_log_gl( "Directional light has been activated." );
                        note_log_gl(
                            "Directional Light "
                            "direction: {v3,.2} color: {v3,.2}",
                            buffer_light.direction, buffer_light.color );
                    } else {
                        note_log_gl( "Directional light has been deactivated." );
                    }

                    glNamedBufferSubData(
                        global_gl->ubo_lights, 0,
                        sizeof( struct OpenGLUniformBufferDirectionalLight ),
                        &buffer_light );
                } break;
                case RENDER_COMMAND_POINT_LIGHT: {
                    struct OpenGLUniformBufferPointLight buffer_light = {};
                    buffer_light.position  = command->point_light.position;
                    buffer_light.color     = command->point_light.color;
                    buffer_light.near_clip = 0.2f;
                    buffer_light.far_clip  = 25.0f;
                    buffer_light.is_active = command->point_light.is_active;
                    global_renderer->point_light_active[command->point_light.index] =
                        command->point_light.is_active;

                    mat4 proj = m4_perspective(
                        to_radians( 90.0f ), 1.0f,
                        buffer_light.near_clip, buffer_light.far_clip );

                    mat4 views[GL_POINT_LIGHT_MATRIX_COUNT];
                    views[0] = m4_view(
                        buffer_light.position,
                        v3_add( buffer_light.position, VEC3_RIGHT ), VEC3_DOWN );
                    views[1] = m4_view(
                        buffer_light.position,
                        v3_add( buffer_light.position, VEC3_LEFT ), VEC3_DOWN );
                    views[2] = m4_view(
                        buffer_light.position,
                        v3_add( buffer_light.position, VEC3_UP ), VEC3_FORWARD );
                    views[3] = m4_view(
                        buffer_light.position,
                        v3_add( buffer_light.position, VEC3_DOWN ), VEC3_BACK );
                    views[4] = m4_view(
                        buffer_light.position,
                        v3_add( buffer_light.position, VEC3_FORWARD ), VEC3_DOWN );
                    views[5] = m4_view(
                        buffer_light.position,
                        v3_add( buffer_light.position, VEC3_BACK ), VEC3_DOWN );

                    for( u32 i = 0; i < GL_POINT_LIGHT_MATRIX_COUNT; ++i ) {
                        buffer_light.matrices[i] = m4_mul_m4( &proj, views + i );
                    }

                    if( command->point_light.is_active ) {
                        note_log_gl( "Light [{u}] has been activated.",
                            command->point_light.index );
                        note_log_gl( "Light position: {v3,.2} color: {v3,.2}",
                            command->point_light.position,
                            command->point_light.color );
                    } else {
                        note_log_gl( "Light [{u}] has been deactivated.",
                            command->point_light.index );
                    }
                    usize offset = offsetof( struct OpenGLUniformBufferLights, point );
                    offset += sizeof( struct OpenGLUniformBufferPointLight ) *
                        command->point_light.index;
                    glNamedBufferSubData(
                        global_gl->ubo_lights, offset,
                        sizeof( struct OpenGLUniformBufferPointLight ),
                        &buffer_light );
                } break;
                case RENDER_COMMAND_GENERATE_MESH: {
                    GLIndexType index_types[] = { GL_UNSIGNED_INT };
                    GLDrawMode  draw_modes[]  = { GL_TRIANGLES };
                    GLVertexArrayType array_types[] = { GL_VERTEX_ARRAY_TYPE_INDEXED };
                    u32 index_counts[]     = { command->generate_mesh.index_count };
                    void* index_buffers[]  = { command->generate_mesh.indices };
                    u32 vertex_counts[]    = { command->generate_mesh.vertex_count };
                    void* vertex_buffers[] = { command->generate_mesh.vertices };

                    GLUsageHint vb_usages[] = { GL_STATIC_DRAW };
                    GLUsageHint ib_usages[] = { GL_STATIC_DRAW };

                    struct GLVertexBufferLayout layouts[] = {
                        global_vertex3d_buffer_layout };

                    usize from = command->generate_mesh.id;
                    usize to   = from + 1;

                    gl_vertex_arrays_create(
                        from, to,
                        index_types,
                        draw_modes,
                        array_types,
                        index_counts,
                        index_buffers,
                        vertex_counts,
                        layouts,
                        vertex_buffers,
                        ib_usages,
                        vb_usages );
                } break;
                case RENDER_COMMAND_GENERATE_TEXTURE: {
                    GLenum  target = gl_texture_type( command->generate_texture.type );
                    usize   index  = command->generate_texture.id;
                    glCreateTextures( target, 1, global_gl->textures + index );

                    GLuint id = global_gl->textures[index];

                    global_gl->tx_types[index] = command->generate_texture.type;
                    global_gl->tx_dimensions[index] = iv3(
                        command->generate_texture.width,
                        command->generate_texture.height,
                        command->generate_texture.depth );

                    glTextureParameteri(
                        id, GL_TEXTURE_WRAP_S,
                        gl_texture_wrap( command->generate_texture.wrap_x ) );
                    glTextureParameteri(
                        id, GL_TEXTURE_WRAP_T,
                        gl_texture_wrap( command->generate_texture.wrap_y ) );
                    if( command->generate_texture.type == GRAPHICS_TEXTURE_TYPE_3D ) {
                        glTextureParameteri(
                            id, GL_TEXTURE_WRAP_R,
                            gl_texture_wrap( command->generate_texture.wrap_z ) );
                    }
                    glTextureParameteri(
                        id, GL_TEXTURE_MAG_FILTER,
                        gl_texture_magnification_filter(
                            command->generate_texture.mag_filter ) );
                    glTextureParameteri(
                        id, GL_TEXTURE_MIN_FILTER,
                        gl_texture_minification_filter(
                            command->generate_texture.min_filter ) );

                    glGenerateTextureMipmap( id );

                    switch( command->generate_texture.type ) {
                        case GRAPHICS_TEXTURE_TYPE_2D: {
                            glTextureStorage2D(
                                id, 1,
                                gl_texture_internal_format(
                                    command->generate_texture.base_type,
                                    command->generate_texture.format ),
                                command->generate_texture.width,
                                command->generate_texture.height );
                            glTextureSubImage2D(
                                id, 0, 0, 0,
                                command->generate_texture.width,
                                command->generate_texture.height,
                                gl_texture_format( command->generate_texture.format ),
                                gl_texture_base_type(
                                    command->generate_texture.base_type ),
                                command->generate_texture.buffer );
                        } break;
                        // TODO(alicia): 3D textures!
                        case GRAPHICS_TEXTURE_TYPE_3D:
                        default: panic();
                    }

                } break;
                case RENDER_COMMAND_RETIRE_MESHES: {
                    gl_vertex_arrays_delete(
                        command->retire_meshes.count,
                        command->retire_meshes.ids );
                } break;
                case RENDER_COMMAND_RETIRE_TEXTURES: {
                    for( usize i = 0; i < command->retire_textures.count; ++i ) {
                        RenderID index = command->retire_textures.ids[i];
                        global_gl->tx_dimensions[index] = IVEC3_ZERO;
                        global_gl->tx_types[index]      = 0;

                        glDeleteTextures( 1, global_gl->textures + index );
                        global_gl->textures[index] = 0;
                    }
                } break;
                default: panic();
            }

            command = list_peek( &global_render_data->list_commands );
        }
    }

    rgba post_process_clear_color = RGBA_BLACK;
    f32  clear_depth              = 1.0f;
    /* clear framebuffers */ {
        glClearNamedFramebufferfv(
            global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_POST_PROCESS],
            GL_COLOR, 0, post_process_clear_color.c );
        glClearNamedFramebufferfv(
            global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_POST_PROCESS],
            GL_DEPTH, 0, &clear_depth );
        glClearNamedFramebufferfv(
            global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL],
            GL_DEPTH, 0, &clear_depth );
    }

    glEnable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );

    for( usize i = 0; i < POINT_LIGHT_COUNT; ++i ) {
        if( global_renderer->point_light_active[i] ) {
            glClearNamedFramebufferfv(
                global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0 + i],
                GL_DEPTH, 0, &clear_depth );
        }
    }

    usize draw_command_count = global_render_data->list_commands.count;
    for( usize i = 0; i < draw_command_count; ++i ) {
        struct RenderCommand* command =
            list_index( &global_render_data->list_commands, i );
        assert( command->type == RENDER_COMMAND_DRAW_3D );

        RenderID mesh = command->draw_3d.mesh;
        if(
            !mesh ||
            mesh >= GL_VERTEX_ARRAY_COUNT ||
            !global_gl->vertex_arrays[mesh]
        ) {
            mesh = GL_VERTEX_ARRAY_INDEX_CUBE;
            command->draw_3d.mesh = mesh;
        }
        RenderID diffuse = command->draw_3d.texture_diffuse;
        if(
            !diffuse ||
            diffuse >= GL_TEXTURE_COUNT ||
            !global_gl->textures[diffuse]
        ) {
            diffuse = GL_TEXTURE_INDEX_DIFFUSE_NULL;
            command->draw_3d.texture_diffuse = diffuse;
        }
        RenderID normal = command->draw_3d.texture_normal;
        if(
            !normal ||
            normal >= GL_TEXTURE_COUNT ||
            !global_gl->textures[normal]
        ) {
            normal = GL_TEXTURE_INDEX_NORMAL_NULL;
            command->draw_3d.texture_normal = normal;
        }
        RenderID roughness = command->draw_3d.texture_roughness;
        if(
            !roughness ||
            roughness >= GL_TEXTURE_COUNT ||
            !global_gl->textures[roughness]
        ) {
            roughness = GL_TEXTURE_INDEX_ROUGHNESS_NULL;
            command->draw_3d.texture_roughness = roughness;
        }
        RenderID metallic = command->draw_3d.texture_metallic;
        if(
            !metallic ||
            metallic >= GL_TEXTURE_COUNT ||
            !global_gl->textures[metallic]
        ) {
            metallic = GL_TEXTURE_INDEX_METALLIC_NULL;
            command->draw_3d.texture_metallic = metallic;
        }

        if(
            !bitfield_check( command->draw_3d.flags, DRAW_3D_SHADOW_CASTER ) ||
            bitfield_check( command->draw_3d.flags, DRAW_3D_WIREFRAME )
        ) {
            continue;
        }

        // TODO(alicia): maybe precalculate entire MVP matrix?
        glNamedBufferSubData(
            global_gl->ubo_transform, 0, sizeof(mat4), &command->draw_3d.transform );

        glBindFramebuffer(
            GL_FRAMEBUFFER,
            global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL] );
        ivec2 shadow_directional_dimensions =
            global_gl->fbo_dimensions[GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL];
        glViewport(
            0, 0,
            shadow_directional_dimensions.width,
            shadow_directional_dimensions.height );

        glBindVertexArray( global_gl->vertex_arrays[mesh] );
        glUseProgram( global_gl->sh_shadow_directional );

        gl_vertex_array_draw( mesh, NULL );

        glUseProgram( global_gl->sh_shadow_point );
        for( usize i = 0; i < POINT_LIGHT_COUNT; ++i ) {
            if( !global_renderer->point_light_active[i] ) {
                continue;
            }

            GLFramebufferID fbo =
                global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0 + i];
            ivec2 shadow_point_dimensions =
                global_gl->fbo_dimensions[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0 + i];

            glBindFramebuffer( GL_FRAMEBUFFER, fbo );
            glViewport(
                0, 0, shadow_point_dimensions.width, shadow_point_dimensions.height );

            glProgramUniform1i(
                global_gl->sh_shadow_point,
                GL_SHADER_PROGRAM_SHADOW_POINT_LOCATION_INDEX, i );

            // TODO(alicia): maybe only draw mesh if light can see it?
            gl_vertex_array_draw( mesh, NULL );
        }

    }

    glEnable( GL_CULL_FACE );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_DIRECTIONAL_SHADOW_MAP,
        global_gl->fbo_texture_0[GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL] );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_0,
        global_gl->fbo_texture_0[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0] );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_1,
        global_gl->fbo_texture_0[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_1] );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_2,
        global_gl->fbo_texture_0[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_2] );
    glBindTextureUnit(
        GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_3,
        global_gl->fbo_texture_0[GL_FRAMEBUFFER_INDEX_SHADOW_POINT_3] );

    glBindFramebuffer(
        GL_FRAMEBUFFER, global_gl->fbo_id[GL_FRAMEBUFFER_INDEX_POST_PROCESS] );
    glViewport( 0, 0, resolution.width, resolution.height );
    glUseProgram( global_gl->sh_phong_brdf );

    for( usize i = 0; i < draw_command_count; ++i ) {
        struct RenderCommand* command =
            list_index( &global_render_data->list_commands, i );

        RenderID mesh      = command->draw_3d.mesh;
        RenderID diffuse   = command->draw_3d.texture_diffuse;
        RenderID normal    = command->draw_3d.texture_normal;
        RenderID roughness = command->draw_3d.texture_roughness;
        RenderID metallic  = command->draw_3d.texture_metallic;

        /* calculate transform */ {
            struct { mat4 transform, normal; } transform = {};
            transform.transform = command->draw_3d.transform;
            mat3 transform_normal;
            if( !m4_normal_matrix(
                &command->draw_3d.transform,
                &transform_normal
            ) ) {
                transform_normal = MAT3_IDENTITY;
            }
            transform.normal = m4_m3( &transform_normal );
            glNamedBufferSubData(
                global_gl->ubo_transform, 0, sizeof(transform), &transform );
        }

        glBindVertexArray( global_gl->vertex_arrays[mesh] );

        glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_DIFFUSE,   global_gl->textures[diffuse] );
        glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_NORMAL,    global_gl->textures[normal] );
        glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_ROUGHNESS, global_gl->textures[roughness] );
        glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_METALLIC,  global_gl->textures[metallic] );

        glProgramUniform3fv(
            global_gl->sh_phong_brdf,
            GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_TINT,
            1, command->draw_3d.tint.c );
        glProgramUniform1i(
            global_gl->sh_phong_brdf,
            GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_IS_SHADOW_RECEIVER,
            bitfield_check( command->draw_3d.flags, DRAW_3D_SHADOW_RECEIVER ) );

        if( bitfield_check( command->draw_3d.flags, DRAW_3D_WIREFRAME ) ) {
            GLDrawMode draw_mode = GL_LINES;
            gl_vertex_array_draw( mesh, &draw_mode );
        } else {
            gl_vertex_array_draw( mesh, NULL );
        }

    }

    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_DIFFUSE,   0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_NORMAL,    0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_ROUGHNESS, 0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_METALLIC,  0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_DIRECTIONAL_SHADOW_MAP, 0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_0, 0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_1, 0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_2, 0 );
    glBindTextureUnit( GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_3, 0 );

    return true;
}
internal no_inline b32 gl_end_frame(void) {
    gl_draw_framebuffer( global_renderer->surface_dimensions );
    glSwapBuffers( global_renderer->surface );
    return true;
}
internal no_inline void gl_shutdown(void) {
    gl_shader_program_delete( GL_SHADER_PROGRAM_COUNT, global_gl->programs );
    glDeleteBuffers( GL_UNIFORM_BUFFER_COUNT, global_gl->uniform_buffers );

    glDeleteFramebuffers( GL_FRAMEBUFFER_COUNT, global_gl->fbo_id );
    glDeleteTextures( GL_FRAMEBUFFER_COUNT, global_gl->fbo_texture_0 );
    glDeleteTextures( GL_FRAMEBUFFER_COUNT, global_gl->fbo_texture_1 );

    memory_zero( global_gl, sizeof( struct OpenGLSubsystem ) );
    global_gl = NULL;
}
internal void gl_draw_framebuffer( ivec2 viewport ) {
    assert( viewport.x > 0 && viewport.y > 0 );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    rgba main_clear_color = RGBA_BLACK;
    glClearNamedFramebufferfv( 0, GL_COLOR, 0, main_clear_color.c );

    glViewport( 0, 0, viewport.width, viewport.height );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    glUseProgram( global_gl->sh_post_process );
    GLTextureID fbo_main_color_attachment =
        global_gl->fbo_texture_0[GL_FRAMEBUFFER_INDEX_POST_PROCESS];
    glBindTextureUnit(
        GL_SHADER_PROGRAM_POST_PROCESS_BINDING_FRAMEBUFFER_COLOR,
        fbo_main_color_attachment );

    glBindVertexArray(
        global_gl->vertex_arrays[GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER] );
    gl_vertex_array_draw( GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER, NULL );

    glBindTextureUnit( GL_SHADER_PROGRAM_POST_PROCESS_BINDING_FRAMEBUFFER_COLOR, 0 );
}

internal no_inline
void gl_debug_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei message_length,
    const GLchar* message, const void* userParam
);
b32 gl_subsystem_init(void) {
    global_gl = (struct OpenGLSubsystem*)
        ((u8*)global_renderer + sizeof(struct RendererSubsystem));

    if( !platform->surface.gl_init( global_renderer->surface ) ) {
        StringSlice last_error;
        platform->last_error( &last_error.len, (const char**)&last_error.buffer );
        fatal_log_gl( "Failed to initialize OpenGL!" );
        fatal_log_gl( "{s}", last_error );
        return false;
    }

    if( !gl_load_functions( platform->gl_load_proc ) ) {
        fatal_log_gl( "Failed to load OpenGL functions!" );
        return false;
    }

    GLint shader_binary_format_count = 0;
    glGetIntegerv( GL_NUM_SHADER_BINARY_FORMATS, &shader_binary_format_count );
    if( !shader_binary_format_count ) {
        fatal_log_gl( "No binary formats are supported!" );
        fatal_log_gl( "SPIR-V support is required!" );
        return false;
    }

#if defined(LD_LOGGING)
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( gl_debug_callback, NULL );
#endif

    global_gl->driver.vendor       = (const char*)glGetString( GL_VENDOR );
    global_gl->driver.name         = (const char*)glGetString( GL_RENDERER );
    global_gl->driver.version      = (const char*)glGetString( GL_VERSION );
    global_gl->driver.glsl_version = (const char*)glGetString(
        GL_SHADING_LANGUAGE_VERSION );

    global_renderer->on_resize   = gl_on_resize;
    global_renderer->begin_frame = gl_begin_frame;
    global_renderer->end_frame   = gl_end_frame;
    global_renderer->shutdown    = gl_shutdown;

    /* create framebuffers */ {
        ivec2 dimensions[GL_FRAMEBUFFER_COUNT] = {
            iv2_scalar( 1 ),
            iv2_scalar( 1024 ),
            iv2_scalar( 1024 ),
            iv2_scalar( 1024 ),
            iv2_scalar( 1024 ),
            iv2_scalar( 1024 ),
        };
        GLFramebufferType types[GL_FRAMEBUFFER_COUNT] = {
            GL_FRAMEBUFFER_TYPE_POST_PROCESS,
            GL_FRAMEBUFFER_TYPE_SHADOW_DIRECTIONAL,
            GL_FRAMEBUFFER_TYPE_SHADOW_POINT,
            GL_FRAMEBUFFER_TYPE_SHADOW_POINT,
            GL_FRAMEBUFFER_TYPE_SHADOW_POINT,
            GL_FRAMEBUFFER_TYPE_SHADOW_POINT,
        };

        gl_framebuffers_create( 0, GL_FRAMEBUFFER_COUNT, dimensions, types, true );
    }

    /* create global uniform buffers */ {
        glCreateBuffers( GL_UNIFORM_BUFFER_COUNT, global_gl->uniform_buffers );

        /* create camera buffer */ {
            struct OpenGLUniformBufferCamera camera = {};
            camera.world_position = VEC3_BACK;
            mat4 view = m4_view( camera.world_position, VEC3_ZERO, VEC3_UP );
            camera.aspect_ratio =
                (f32)global_renderer->surface_dimensions.width /
                (f32)global_renderer->surface_dimensions.height;
            camera.near_clip = 0.001f;
            camera.far_clip  = 1000.0f;
            mat4 proj = m4_perspective(
                to_degrees( 60.0f ), camera.aspect_ratio,
                camera.near_clip, camera.far_clip );

            camera.view_projection_3d = m4_mul_m4( &proj, &view );
            camera.view_projection_ui = MAT4_IDENTITY;

            glNamedBufferStorage(
                global_gl->ubo_camera,
                sizeof(struct OpenGLUniformBufferCamera),
                &camera, GL_DYNAMIC_STORAGE_BIT );
            glBindBufferBase(
                GL_UNIFORM_BUFFER,
                GL_UNIFORM_BUFFER_BINDING_CAMERA,
                global_gl->ubo_camera );
        }

        /* create lights buffer */ {
            struct OpenGLUniformBufferLights lights = {};
            lights.directional.is_active = false;
            for( usize i = 0; i < POINT_LIGHT_COUNT; ++i ) {
                struct OpenGLUniformBufferPointLight* point =
                    lights.point + i;
                point->is_active = false;
            }

            glNamedBufferStorage(
                global_gl->ubo_lights,
                sizeof(struct OpenGLUniformBufferLights),
                &lights, GL_DYNAMIC_STORAGE_BIT );
            glBindBufferBase(
                GL_UNIFORM_BUFFER,
                GL_UNIFORM_BUFFER_BINDING_LIGHTS,
                global_gl->ubo_lights );
        }

        /* create data buffer */ {
            struct OpenGLUniformBufferData data = {};
            data.time = VEC4_ZERO;
            data.surface_resolution = v2_iv2( global_renderer->surface_dimensions );
            data.aspect_ratio       =
                (f32)global_renderer->surface_dimensions.width /
                (f32)global_renderer->surface_dimensions.height;

            glNamedBufferStorage(
                global_gl->ubo_data,
                sizeof( struct OpenGLUniformBufferData ),
                &data, GL_DYNAMIC_STORAGE_BIT );
            glBindBufferBase(
                GL_UNIFORM_BUFFER,
                GL_UNIFORM_BUFFER_BINDING_DATA,
                global_gl->ubo_data );
        }

        /* create transform buffer */ {
            struct { mat4 transform; mat4 normal; } transform = {};
            transform.transform = MAT4_IDENTITY;
            transform.normal    = MAT4_IDENTITY;

            glNamedBufferStorage(
                global_gl->ubo_transform, sizeof( mat4 ) * 2,
                &transform, GL_DYNAMIC_STORAGE_BIT );
            glBindBufferBase(
                GL_UNIFORM_BUFFER,
                GL_UNIFORM_BUFFER_BINDING_TRANSFORM, global_gl->ubo_transform );
        }
    }

    global_vertex3d_buffer_layout.attribute_count = GL_VERTEX_3D_ATTRIBUTE_COUNT;
    global_vertex3d_buffer_layout.attribute_types = global_vertex3d_attribute_types;
    global_vertex3d_buffer_layout.stride          = sizeof(struct Vertex3D);
    global_vertex3d_buffer_layout.attribute_component_counts =
        global_vertex3d_attribute_component_counts;

    /* create default vertex arrays */ {
        GLIndexType index_types[] = { GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE };
        GLDrawMode  draw_modes[]  = { GL_TRIANGLES, GL_TRIANGLES };
        GLVertexArrayType array_types[] =
            { GL_VERTEX_ARRAY_TYPE_INDEXED, GL_VERTEX_ARRAY_TYPE_ARRAY };
        u32 index_counts[]    = { CUBE_3D_INDEX_COUNT, 0 };
        void* index_buffers[] = { CUBE_3D_INDICES, NULL };
        u32 vertex_counts[]   = { CUBE_3D_VERTEX_COUNT, FRAMEBUFFER_QUAD_VERTEX_COUNT };

        struct GLVertexBufferLayout framebuffer_quad_layout = {};
        framebuffer_quad_layout.stride = sizeof(f32) * 4;

        GLint  fbo_quad_attribute_component_counts[] = { 2, 2 };
        GLenum fbo_quad_attribute_types[]            = { GL_FLOAT, GL_FLOAT };

        framebuffer_quad_layout.attribute_component_counts =
            fbo_quad_attribute_component_counts;
        framebuffer_quad_layout.attribute_types = fbo_quad_attribute_types;
        framebuffer_quad_layout.attribute_count = 2;

        struct GLVertexBufferLayout vertex_layouts[] = {
            global_vertex3d_buffer_layout,
            framebuffer_quad_layout
        };

        f32 fbo_quad_vertices[] = {
            -1.0f, -1.0f, /* uvs */ 0.0f, 0.0f,
             1.0f,  1.0f, /* uvs */ 1.0f, 1.0f,
            -1.0f,  1.0f, /* uvs */ 0.0f, 1.0f,

            -1.0f, -1.0f, /* uvs */ 0.0f, 0.0f,
             1.0f, -1.0f, /* uvs */ 1.0f, 0.0f,
             1.0f,  1.0f, /* uvs */ 1.0f, 1.0f,
        };

        void* vertex_buffers[] = { CUBE_3D, fbo_quad_vertices };

        GLUsageHint vertex_buffer_usage_hints[] = { GL_STATIC_DRAW, GL_STATIC_DRAW };
        GLUsageHint index_buffer_usage_hints[]  = { GL_STATIC_DRAW, GL_STATIC_DRAW };

        gl_vertex_arrays_create(
            GL_VERTEX_ARRAY_INDEX_CUBE, GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER + 1,
            index_types, draw_modes, array_types,
            index_counts, index_buffers, vertex_counts,
            vertex_layouts, vertex_buffers,
            vertex_buffer_usage_hints, index_buffer_usage_hints );
    }

    /* create default textures */ {
        glCreateTextures(
            GL_TEXTURE_2D, GL_TEXTURE_INDEX_METALLIC_NULL + 1, global_gl->textures );

        /* null diffuse */ {
            GLTextureID id = global_gl->textures[GL_TEXTURE_INDEX_DIFFUSE_NULL];
            global_gl->tx_types[GL_TEXTURE_INDEX_DIFFUSE_NULL] =
                GRAPHICS_TEXTURE_TYPE_2D;
            global_gl->tx_dimensions[GL_TEXTURE_INDEX_DIFFUSE_NULL] =
                iv3( NULL_DIFFUSE_TEXTURE_WIDTH, NULL_DIFFUSE_TEXTURE_HEIGHT, 0 );

            glTextureParameteri( id, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );

            glGenerateTextureMipmap( id );

            glTextureStorage2D(
                id, 1, GL_RGBA8,
                NULL_DIFFUSE_TEXTURE_WIDTH, NULL_DIFFUSE_TEXTURE_HEIGHT );
            glTextureSubImage2D(
                id, 0,
                0, 0,
                NULL_DIFFUSE_TEXTURE_WIDTH, NULL_DIFFUSE_TEXTURE_HEIGHT,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL_DIFFUSE_TEXTURE );
        }

        /* null normal */ {
            GLTextureID id = global_gl->textures[GL_TEXTURE_INDEX_NORMAL_NULL];
            global_gl->tx_types[GL_TEXTURE_INDEX_NORMAL_NULL] =
                GRAPHICS_TEXTURE_TYPE_2D;
            global_gl->tx_dimensions[GL_TEXTURE_INDEX_NORMAL_NULL] =
                iv3( NULL_NORMAL_TEXTURE_WIDTH, NULL_NORMAL_TEXTURE_HEIGHT, 0 );

            glTextureParameteri( id, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );

            glGenerateTextureMipmap( id );

            glTextureStorage2D(
                id, 1, GL_RGB8,
                NULL_NORMAL_TEXTURE_WIDTH, NULL_NORMAL_TEXTURE_HEIGHT );
            glTextureSubImage2D(
                id, 0,
                0, 0,
                NULL_NORMAL_TEXTURE_WIDTH, NULL_NORMAL_TEXTURE_HEIGHT,
                GL_RGB, GL_UNSIGNED_BYTE, NULL_NORMAL_TEXTURE );
        }

        /* null roughness */ {
            GLTextureID id = global_gl->textures[GL_TEXTURE_INDEX_ROUGHNESS_NULL];
            global_gl->tx_types[GL_TEXTURE_INDEX_ROUGHNESS_NULL] =
                GRAPHICS_TEXTURE_TYPE_2D;
            global_gl->tx_dimensions[GL_TEXTURE_INDEX_ROUGHNESS_NULL] =
                iv3( NULL_ROUGHNESS_TEXTURE_WIDTH, NULL_ROUGHNESS_TEXTURE_HEIGHT, 0 );

            glTextureParameteri( id, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );

            glGenerateTextureMipmap( id );

            glTextureStorage2D(
                id, 1, GL_R8,
                NULL_ROUGHNESS_TEXTURE_WIDTH, NULL_ROUGHNESS_TEXTURE_HEIGHT );
            glTextureSubImage2D(
                id, 0,
                0, 0,
                NULL_ROUGHNESS_TEXTURE_WIDTH, NULL_ROUGHNESS_TEXTURE_HEIGHT,
                GL_RED, GL_UNSIGNED_BYTE, NULL_ROUGHNESS_TEXTURE );
        }

        /* null metallic */ {
            GLTextureID id = global_gl->textures[GL_TEXTURE_INDEX_METALLIC_NULL];
            global_gl->tx_types[GL_TEXTURE_INDEX_METALLIC_NULL] =
                GRAPHICS_TEXTURE_TYPE_2D;
            global_gl->tx_dimensions[GL_TEXTURE_INDEX_METALLIC_NULL] =
                iv3( NULL_METALLIC_TEXTURE_WIDTH, NULL_METALLIC_TEXTURE_HEIGHT, 0 );

            glTextureParameteri( id, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTextureParameteri( id, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTextureParameteri( id, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );

            glGenerateTextureMipmap( id );

            glTextureStorage2D(
                id, 1, GL_R8,
                NULL_METALLIC_TEXTURE_WIDTH, NULL_METALLIC_TEXTURE_HEIGHT );
            glTextureSubImage2D(
                id, 0,
                0, 0,
                NULL_METALLIC_TEXTURE_WIDTH, NULL_METALLIC_TEXTURE_HEIGHT,
                GL_RED, GL_UNSIGNED_BYTE, NULL_METALLIC_TEXTURE );
        }
    }

    /* create shaders */ {
        usize buffer_size = 0;

        #define GL_OPEN_SHADER( path, name )\
            FSFile* file_##name = fs_file_open(\
                path, FS_FILE_READ | FS_FILE_SHARE_READ |\
                FS_FILE_ONLY_EXISTING );\
            assert( file_##name );\
            usize name##_size   = fs_file_query_size( file_##name );\
            usize name##_offset = buffer_size;\
            buffer_size += name##_size

        #define GL_READ_SHADER( name )\
            assert( fs_file_read(\
                file_##name, name##_size,\
                read_buffer + name##_offset ) );\
            fs_file_close( file_##name )
        
        #define GL_COMPILE_SHADER( name, stage, shader )\
            assert( gl_shader_compile_spirv(\
                name##_size, read_buffer + name##_offset,\
                stage, "main", 0, NULL, NULL, shader\
            ) )

        GL_OPEN_SHADER(
            "./resources/shaders/post_process.vert.spv", post_process_vert );
        GL_OPEN_SHADER(
            "./resources/shaders/post_process.frag.spv", post_process_frag );
        GL_OPEN_SHADER(
            "./resources/shaders/shadow_directional.vert.spv", shadow_directional_vert );
        GL_OPEN_SHADER(
            "./resources/shaders/shadow_directional.frag.spv", shadow_directional_frag );
        GL_OPEN_SHADER(
            "./resources/shaders/shadow_point.vert.spv", shadow_point_vert );
        GL_OPEN_SHADER(
            "./resources/shaders/shadow_point.geom.spv", shadow_point_geom );
        GL_OPEN_SHADER(
            "./resources/shaders/shadow_point.frag.spv", shadow_point_frag );
        GL_OPEN_SHADER(
            "./resources/shaders/phong.vert.spv", phong_vert );
        GL_OPEN_SHADER(
            "./resources/shaders/phong.frag.spv", phong_frag );

        u8* read_buffer = system_alloc( buffer_size );
        assert( read_buffer );

        GL_READ_SHADER( post_process_vert );
        GL_READ_SHADER( post_process_frag );
        GL_READ_SHADER( shadow_directional_vert );
        GL_READ_SHADER( shadow_directional_frag );
        GL_READ_SHADER( shadow_point_vert );
        GL_READ_SHADER( shadow_point_geom );
        GL_READ_SHADER( shadow_point_frag );
        GL_READ_SHADER( phong_vert );
        GL_READ_SHADER( phong_frag );

        GLShaderID post_process_shaders[2];
        GL_COMPILE_SHADER(
            post_process_vert, GL_VERTEX_SHADER, post_process_shaders + 0 );
        GL_COMPILE_SHADER(
            post_process_frag, GL_FRAGMENT_SHADER, post_process_shaders + 1 );

        assert( gl_shader_program_link(
            2, post_process_shaders,
            &global_gl->sh_post_process
        ) );

        gl_shader_delete( 2, post_process_shaders );

        GLShaderID phong_shaders[2];
        GL_COMPILE_SHADER(
            phong_vert, GL_VERTEX_SHADER, phong_shaders + 0 );
        GL_COMPILE_SHADER(
            phong_frag, GL_FRAGMENT_SHADER, phong_shaders + 1 );

        assert( gl_shader_program_link(
            2, phong_shaders,
            &global_gl->sh_phong_brdf
        ) );

        gl_shader_delete( 2, phong_shaders );

        GLShaderID shadow_directional_shaders[2];
        GL_COMPILE_SHADER(
            shadow_directional_vert, GL_VERTEX_SHADER, shadow_directional_shaders + 0 );
        GL_COMPILE_SHADER(
            shadow_directional_frag, GL_FRAGMENT_SHADER, shadow_directional_shaders + 1 );

        assert( gl_shader_program_link(
            2, shadow_directional_shaders,
            &global_gl->sh_shadow_directional
        ) );

        gl_shader_delete( 2, shadow_directional_shaders );

        GLShaderID shadow_point_shaders[3];
        GL_COMPILE_SHADER(
            shadow_point_vert, GL_VERTEX_SHADER, shadow_point_shaders + 0 );
        GL_COMPILE_SHADER(
            shadow_point_geom, GL_GEOMETRY_SHADER, shadow_point_shaders + 1 );
        GL_COMPILE_SHADER(
            shadow_point_frag, GL_FRAGMENT_SHADER, shadow_point_shaders + 2 );

        assert( gl_shader_program_link(
            3, shadow_point_shaders,
            &global_gl->sh_shadow_point
        ) );

        gl_shader_delete( 3, shadow_point_shaders );

        system_free( read_buffer, buffer_size );
    }

    return true;
}
maybe_unused
internal const char* gl_debug_source_to_cstr( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API:             return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY:     return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION:     return "Application";
        default: return "Other";
    }
}
maybe_unused
internal const char* gl_debug_type_to_cstr( GLenum type ) {
    switch( type ) {
        case GL_DEBUG_TYPE_ERROR:               return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
        case GL_DEBUG_TYPE_MARKER:              return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
        case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
        default: return "Other";
    }
}
internal no_inline maybe_unused
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
        id, gl_debug_source_to_cstr( source ),\
        gl_debug_type_to_cstr( type ),\
        message

    switch( severity ) {
        case GL_DEBUG_SEVERITY_HIGH: {
            error_log_gl( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM: {
            warn_log_gl( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_LOW: {
            info_log_gl( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
        default: {
            note_log_gl( "{u32} {cc} {cc} | {cc}", GL_DEBUG_MESSAGE_FORMAT );
        } break;
    }
#endif
}

maybe_unused
internal GLenum gl_texture_type( GraphicsTextureType type ) {
    assert( type < GRAPHICS_TEXTURE_TYPE_COUNT );
    GLenum types[GRAPHICS_TEXTURE_TYPE_COUNT] = {
        GL_TEXTURE_2D,
        GL_TEXTURE_3D
    };
    return types[type];
}
maybe_unused
internal GLenum gl_texture_format( GraphicsTextureFormat format ) {
    assert( format < GRAPHICS_TEXTURE_FORMAT_COUNT );
    GLenum formats[GRAPHICS_TEXTURE_FORMAT_COUNT] = {
        GL_RED,
        GL_RGB,
        GL_RGBA,
        GL_SRGB,
    };
    return formats[format];
}
maybe_unused
internal GLenum gl_texture_internal_format(
    GraphicsTextureBaseType base_type, GraphicsTextureFormat format
) {
    switch( format ) {
        case GRAPHICS_TEXTURE_FORMAT_GRAYSCALE: switch( base_type ) {
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT8:
                return GL_R8;
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT16:
                return GL_R16;
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT32:
                return GL_R32UI;
            case GRAPHICS_TEXTURE_BASE_TYPE_FLOAT32:
                return GL_R32F;
            default: panic();
        } break;
        case GRAPHICS_TEXTURE_FORMAT_RGB: switch( base_type ) {
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT8:
                return GL_RGB8;
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT16:
                return GL_RGB16;
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT32:
                return GL_RGB32UI;
            case GRAPHICS_TEXTURE_BASE_TYPE_FLOAT32:
                return GL_RGB32F;
            default: panic();
        } break;
        case GRAPHICS_TEXTURE_FORMAT_RGBA: switch( base_type ) {
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT8:
                return GL_RGBA8;
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT16:
                return GL_RGBA16;
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT32:
                return GL_RGBA32UI;
            case GRAPHICS_TEXTURE_BASE_TYPE_FLOAT32:
                return GL_RGBA32F;
            default: panic();
        } break;
        case GRAPHICS_TEXTURE_FORMAT_SRGB: switch( base_type ) {
            case GRAPHICS_TEXTURE_BASE_TYPE_UINT8:
                return GL_SRGB8;
            default: panic();
        } break;
        default: panic();
    }
}
maybe_unused
internal GLenum gl_texture_base_type( GraphicsTextureBaseType type ) {
    assert( type < GRAPHICS_TEXTURE_BASE_TYPE_COUNT );
    GLenum types[GRAPHICS_TEXTURE_BASE_TYPE_COUNT] = {
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_INT,
        GL_FLOAT
    };
    return types[type];
}
maybe_unused
internal GLenum gl_texture_wrap( GraphicsTextureWrap wrap ) {
    assert( wrap < GRAPHICS_TEXTURE_WRAP_COUNT );
    GLenum wraps[GRAPHICS_TEXTURE_WRAP_COUNT] = {
        GL_CLAMP_TO_EDGE,
        GL_REPEAT
    };
    return wraps[wrap];
}
maybe_unused
internal GLenum gl_texture_minification_filter(
    GraphicsTextureFilter filter
) {
    assert( filter < GRAPHICS_TEXTURE_FILTER_COUNT );
    GLenum filters[GRAPHICS_TEXTURE_FILTER_COUNT] = {
        GL_NEAREST_MIPMAP_NEAREST,
        GL_LINEAR_MIPMAP_LINEAR
    };
    return filter[filters];
}
maybe_unused
internal GLenum gl_texture_magnification_filter(
    GraphicsTextureFilter filter
) {
    assert( filter < GRAPHICS_TEXTURE_FILTER_COUNT );
    GLenum filters[GRAPHICS_TEXTURE_FILTER_COUNT] = {
        GL_NEAREST,
        GL_LINEAR
    };
    return filters[filter];
}

void gl_framebuffers_create(
    usize from_inclusive, usize to_exclusive,
    ivec2* in_dimensions, GLFramebufferType* types,
    b32 log
) {
    usize count = to_exclusive - from_inclusive;
    glCreateFramebuffers( count, global_gl->fbo_id + from_inclusive );
    for( usize i = 0; i < count; ++i ) {
        usize index            = from_inclusive + i;
        ivec2 dimensions       = in_dimensions[i];
        GLFramebufferType type = types[i];
        GLFramebufferID   id   = global_gl->fbo_id[index];

        GLTextureID* texture_0 = global_gl->fbo_texture_0 + index;
        GLTextureID* texture_1 = global_gl->fbo_texture_1 + index;

        global_gl->fbo_dimensions[index] = dimensions;
        global_gl->fbo_type[index]       = type;

        switch( type ) {
            case GL_FRAMEBUFFER_TYPE_POST_PROCESS: {
                glCreateTextures( GL_TEXTURE_2D, 1, texture_0 );
                glCreateTextures( GL_TEXTURE_2D, 1, texture_1 );
                GLTextureID color_attachment = *texture_0;
                GLTextureID depth_attachment = *texture_1;

                glTextureStorage2D(
                    color_attachment, 1,
                    GL_RGBA16, dimensions.width, dimensions.height );

                glTextureParameteri(
                    color_attachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTextureParameteri(
                    color_attachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                glTextureParameteri(
                    color_attachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTextureParameteri(
                    color_attachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

                glTextureStorage2D(
                    depth_attachment, 1,
                    GL_DEPTH_COMPONENT24,
                    dimensions.width, dimensions.height );

                glNamedFramebufferTexture(
                    id, GL_COLOR_ATTACHMENT0, color_attachment, 0 );
                glNamedFramebufferTexture(
                    id, GL_DEPTH_ATTACHMENT, depth_attachment, 0 );

                glNamedFramebufferDrawBuffer( id, GL_COLOR_ATTACHMENT0 );
            } break;
            case GL_FRAMEBUFFER_TYPE_SHADOW_DIRECTIONAL: {
                glCreateTextures( GL_TEXTURE_2D, 1, texture_0 );
                GLTextureID shadow_attachment = *texture_0;

                glTextureStorage2D(
                    shadow_attachment, 1,
                    GL_DEPTH_COMPONENT24,
                    dimensions.width, dimensions.height );

                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST );

                vec4 border_color = RGBA_WHITE;
                glTextureParameterfv(
                    shadow_attachment,
                    GL_TEXTURE_BORDER_COLOR, border_color.c );
                glNamedFramebufferTexture(
                    id, GL_DEPTH_ATTACHMENT, shadow_attachment, 0 );

                glNamedFramebufferDrawBuffer( id, GL_NONE );
            } break;
            case GL_FRAMEBUFFER_TYPE_SHADOW_POINT: {
                glCreateTextures( GL_TEXTURE_CUBE_MAP_ARRAY, 1, texture_0 );
                GLTextureID shadow_attachment = *texture_0;

                glTextureStorage3D(
                    shadow_attachment, 1,
                    GL_DEPTH_COMPONENT32F,
                    dimensions.width, dimensions.height, 6 );

                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                glTextureParameteri(
                    shadow_attachment,
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST );

                glNamedFramebufferTexture(
                    id, GL_DEPTH_ATTACHMENT,
                    shadow_attachment, 0 );

                glNamedFramebufferDrawBuffer( id, GL_NONE );
            } break;
            default: panic();
        }

#if defined(LD_ASSERTIONS)
        GLenum status = glCheckNamedFramebufferStatus( id, GL_FRAMEBUFFER );
        if( status != GL_FRAMEBUFFER_COMPLETE ) {
            error_log_gl( "Framebuffer [{usize}][{cc}] is incomplete!",
                index, gl_framebuffer_type_to_cstr( type ) );
            panic();
        }
#endif

#if defined(LD_LOGGING)
        if( log ) {
            note_log_gl(
                "Framebuffer [{usize}][{cc}] created with resolution {i}x{i}.",
                index, gl_framebuffer_type_to_cstr( type ),
                dimensions.width, dimensions.height );
        }
#endif
    }
    unused(log);
}
void gl_framebuffers_resize(
    usize from_inclusive, usize to_exclusive, ivec2* in_dimensions
) {
    usize count = to_exclusive - from_inclusive;
    glDeleteTextures( count, global_gl->fbo_texture_0 + from_inclusive );
    glDeleteTextures( count, global_gl->fbo_texture_1 + from_inclusive );
    glDeleteFramebuffers( count, global_gl->fbo_id + from_inclusive );

#if defined(LD_LOGGING)
    ivec2 old_dimensions[GL_FRAMEBUFFER_COUNT];
    for( usize i = 0; i < count; ++i ) {
        usize index = from_inclusive + i;
        old_dimensions[i] = global_gl->fbo_dimensions[index];
    }
#endif

    gl_framebuffers_create(
        from_inclusive, to_exclusive,
        in_dimensions, global_gl->fbo_type + from_inclusive, false );

#if defined(LD_LOGGING)
    for( usize i = 0; i < count; ++i ) {
        usize index            = from_inclusive + i;
        ivec2 old              = old_dimensions[i];
        ivec2 new              = global_gl->fbo_dimensions[index];
        GLFramebufferType type = global_gl->fbo_type[index];

        note_log_gl( "Framebuffer [{usize}][{cc}] resized from {i}x{i} to {i}x{i}",
            index, gl_framebuffer_type_to_cstr( type ),
            old.width, old.height, new.width, new.height );
    }
#endif

}

void gl_vertex_arrays_create(
    usize from_inclusive, usize to_exclusive,
    GLIndexType* index_types, GLDrawMode* draw_modes,
    GLVertexArrayType* vertex_array_types,
    u32* index_counts,  void** index_buffers,
    u32* vertex_counts, struct GLVertexBufferLayout* vertex_layouts,
    void** vertex_buffers,
    GLUsageHint* index_buffer_usage_hints, GLUsageHint* vertex_buffer_usage_hints
) {
    assert(
        index_types        && draw_modes        &&
        vertex_array_types && index_counts      &&
        index_buffers      && vertex_counts     &&
        vertex_layouts     && vertex_buffers );

    usize count = to_exclusive - from_inclusive;
    glCreateVertexArrays( count, global_gl->vertex_arrays + from_inclusive );
    glCreateBuffers(
        count * 2, (GLBufferID*)(global_gl->vao_buffers + from_inclusive) );

    for( usize i = 0; i < count; ++i ) {
        usize index = from_inclusive + i;
        GLVertexArrayID vao = global_gl->vertex_arrays[index];
        GLBufferID      vbo = global_gl->vao_buffers[index].vertex_buffer;
        GLBufferID      ebo = global_gl->vao_buffers[index].element_buffer;

        struct GLVertexBufferLayout* layout = vertex_layouts + i;

        global_gl->vao_index_types[index] = index_types[i];
        global_gl->vao_types[index]       = vertex_array_types[i];
        global_gl->vao_draw_modes[index]  = draw_modes[i];
        switch( vertex_array_types[i] ) {
            case GL_VERTEX_ARRAY_TYPE_ARRAY: {
                global_gl->vao_counts[index] = vertex_counts[i];
            } break;
            case GL_VERTEX_ARRAY_TYPE_INDEXED: {
                global_gl->vao_counts[index] = index_counts[i];
            } break;
            default: panic();
        }

        usize vertex_buffer_size = vertex_counts[i] * layout->stride;

        glNamedBufferData(
            vbo, vertex_buffer_size,
            vertex_buffers[i], vertex_buffer_usage_hints[i] );
        glVertexArrayVertexBuffer( vao, 0, vbo, 0, layout->stride );

        switch( vertex_array_types[i] ) {
            case GL_VERTEX_ARRAY_TYPE_ARRAY: {
                glDeleteBuffers( 1, &ebo );
            } break;
            case GL_VERTEX_ARRAY_TYPE_INDEXED: {
                usize index_count       = index_counts[i];
                usize index_buffer_size = 0;
                switch( index_types[i] ) {
                    case GL_UNSIGNED_BYTE: {
                        index_buffer_size = index_count;
                    } break;
                    case GL_UNSIGNED_SHORT: {
                        index_buffer_size = index_count * sizeof(u16);
                    } break;
                    case GL_UNSIGNED_INT: {
                        index_buffer_size = index_count * sizeof(u32);
                    } break;
                    default: panic();
                }
                glNamedBufferData(
                    ebo, index_buffer_size,
                    index_buffers[i], index_buffer_usage_hints[i] );
                glVertexArrayElementBuffer( vao, ebo );
            } break;
            default: panic();
        }

        assert( layout->attribute_count );
        GLuint offset = 0;
        for( usize attribute = 0; attribute < layout->attribute_count; ++attribute ) {
            GLuint attribute_size = 0;
            GLenum attribute_type = layout->attribute_types[attribute];
            GLint attribute_component_count =
                layout->attribute_component_counts[attribute];
            switch( attribute_type ) {
                case GL_UNSIGNED_BYTE:
                case GL_BYTE: {
                    attribute_size = sizeof(u8) * attribute_component_count;
                } break;
                case GL_UNSIGNED_SHORT:
                case GL_SHORT: {
                    attribute_size = sizeof(u16) * attribute_component_count;
                } break;
                case GL_UNSIGNED_INT:
                case GL_INT:
                case GL_FLOAT: {
                    attribute_size = sizeof(u32) * attribute_component_count;
                } break;
                default: panic();
            }

            glEnableVertexArrayAttrib( vao, attribute );
            glVertexArrayAttribFormat(
                vao, attribute, attribute_component_count,
                attribute_type, GL_FALSE, offset );
            glVertexArrayAttribBinding( vao, attribute, 0 );
            offset += attribute_size;
        }

        note_log_gl( "Vertex Array [{u}] created.", vao );
    }
}
void gl_vertex_arrays_delete_range( usize from_inclusive, usize to_exclusive ) {
    usize count = to_exclusive - from_inclusive;

    glDeleteVertexArrays( count, global_gl->vertex_arrays + from_inclusive );
    glDeleteBuffers( count * 2, (GLuint*)(global_gl->vao_buffers + from_inclusive) );
    memory_zero(
        global_gl->vertex_arrays + from_inclusive, count * sizeof(GLVertexArrayID) );
}
void gl_vertex_arrays_delete( usize count, RenderID* indices ) {
    for( usize i = 0; i < count; ++i ) {
        usize index = indices[i];
        glDeleteVertexArrays( 1, global_gl->vertex_arrays + index );
        glDeleteBuffers( 2, (GLuint*)(global_gl->vao_buffers + index) );
        global_gl->vertex_arrays[index] = 0;
    }
}
void gl_vertex_array_draw( usize index, GLDrawMode* draw_mode_override ) {
    GLVertexArrayType type       = global_gl->vao_types[index];
    u32               count      = global_gl->vao_counts[index];
    GLIndexType       index_type = global_gl->vao_index_types[index];
    GLDrawMode        draw_mode  = global_gl->vao_draw_modes[index];

    if( draw_mode_override ) {
        draw_mode = *draw_mode_override;
    }

    switch( type ) {
        case GL_VERTEX_ARRAY_TYPE_ARRAY: {
            glDrawArrays( draw_mode, 0, count );
        } break;
        case GL_VERTEX_ARRAY_TYPE_INDEXED: {
            glDrawElements( draw_mode, count, index_type, NULL );
        } break;
        default: panic();
    }
}

