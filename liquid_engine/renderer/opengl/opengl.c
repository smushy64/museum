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
#include "core/collections.h"
#include "core/mem.h"

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

internal GLenum gl_texture_type( GraphicsTextureType type );
internal GLenum gl_texture_format( GraphicsTextureFormat format );
internal GLenum gl_texture_internal_format(
    GraphicsTextureBaseType base_type, GraphicsTextureFormat format );
internal GLenum gl_texture_base_type( GraphicsTextureBaseType type );
internal GLenum gl_texture_wrap( GraphicsTextureWrap wrap );
internal GLenum gl_texture_minification_filter(
    GraphicsTextureFilter filter );
internal GLenum gl_texture_magnification_filter(
    GraphicsTextureFilter filter );

b32 gl_renderer_backend_init( RendererContext* renderer_ctx ) {
    OpenGLRendererContext* ctx = renderer_ctx;

    ctx->buffer_count       = GL_MINIMUM_RESOURCE_COUNT;
    ctx->vertex_array_count = GL_MINIMUM_RESOURCE_COUNT;
    ctx->texture_2d_count   = GL_MINIMUM_RESOURCE_COUNT;

    ctx->buffers = ldalloc(
        ctx->buffer_count * sizeof(GLBufferID), MEMORY_TYPE_RENDERER );
    ctx->vertex_arrays = ldalloc(
        ctx->vertex_array_count * sizeof(GLVertexArray), MEMORY_TYPE_RENDERER );
    ctx->textures_2d = ldalloc(
        ctx->texture_2d_count * sizeof(GLTexture2D), MEMORY_TYPE_RENDERER );

    if(
        !ctx->buffers       ||
        !ctx->vertex_arrays ||
        !ctx->textures_2d
    ) {
        return false;
    }

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
    GLVertexArray* array =
        ctx->vertex_arrays + GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER;
    glBindVertexArray( array->id );
    glUseProgram( program );

    GLFramebuffer* main_fbo =
        ctx->framebuffers + GL_FRAMEBUFFER_INDEX_MAIN_FRAMEBUFFER;
    glBindTextureUnit(
        GL_SHADER_PROGRAM_POST_PROCESS_RENDER_TEXTURE_BINDING,
        main_fbo->color_texture_id );

    glDrawArrays( array->default_mode, 0, 6 );
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

internal void gl_generate(
    OpenGLRendererContext* ctx, RenderCommand* command
) {
    switch( command->type ) {
        case RENDER_COMMAND_TYPE_GENERATE_MESH: {
            struct GenerateMeshCommand* mesh = &command->generate_mesh;
            assert( mesh->vertex_count && mesh->index_count );
            assert( !map_u32_u32_key_exists( &ctx->ctx.mesh_map, mesh->id ) );

            map_u32_u32_push(
                &ctx->ctx.mesh_map,
                mesh->id, ctx->vertex_array_count );
            GLVertexArray* array =
                ctx->vertex_arrays + (ctx->vertex_array_count++);
            glCreateVertexArrays( 1, &array->id );
            glCreateBuffers( 2, ctx->buffers + ctx->buffer_count );
            GLVertexArrayID vao = array->id;
            GLBufferID vbo = ctx->buffers[ctx->buffer_count];
            GLBufferID ebo = ctx->buffers[ctx->buffer_count + 1];
            ctx->buffer_count += 2;

            array->index_type   = GL_UNSIGNED_INT;
            array->default_mode = GL_TRIANGLES;
            array->index_count  = mesh->index_count;

            glNamedBufferStorage(
                vbo, mesh->vertex_count * sizeof(struct Vertex3D),
                mesh->vertices, GL_DYNAMIC_STORAGE_BIT );
            glNamedBufferStorage(
                ebo, mesh->index_count * sizeof(u32),
                mesh->indices, GL_DYNAMIC_STORAGE_BIT );

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

            GL_LOG_NOTE( "Mesh [{u}] created. Vertex count: {usize}, Index count: {usize}", mesh->id, mesh->vertex_count, mesh->index_count );
        } break;
        case RENDER_COMMAND_TYPE_GENERATE_TEXTURE: {
            struct GenerateTextureCommand* texture = &command->generate_texture;
            // TODO(alicia): 3D textures
            assert( texture->type == GRAPHICS_TEXTURE_TYPE_2D );
            GLTexture2D* tx2d = ctx->textures_2d + ctx->texture_2d_count;
            Map_u32_u32* map  = &ctx->ctx.texture_map;
            assert( !map_u32_u32_key_exists( map, texture->id ) );

            *tx2d = gl_texture_2d_create(
                texture->width, texture->height, 0,
                gl_texture_base_type( texture->base_type ),
                gl_texture_internal_format( texture->base_type, texture->format ),
                gl_texture_format( texture->format ),
                gl_texture_wrap( texture->wrap_x ),
                gl_texture_wrap( texture->wrap_y ),
                gl_texture_magnification_filter( texture->magnification_filter ),
                gl_texture_minification_filter( texture->minification_filter ),
                texture->buffer );

            map_u32_u32_push( map, texture->id, tx2d->id );

            GL_LOG_NOTE( "Texture [{u}] created.", texture->id );
        } break;
        default: panic();
    }
}

internal void gl_retire(
    OpenGLRendererContext* ctx, RenderCommand* command
) {
    unused(ctx);
    unused(command);
}

b32 gl_renderer_backend_begin_frame(
    RendererContext* renderer_ctx, RenderData* render_data
) {
    OpenGLRendererContext* ctx = renderer_ctx;

    // execute generate commands
    usize command_count_max = render_data->command_count;
    if( render_data->non_draw_command_present ) {
        for(
            usize i = render_data->non_draw_command_start;
            i < command_count_max;
            ++i
        ) {
            RenderCommand* current = render_data->command_buffer + i;
            switch( current->type ) {
                case RENDER_COMMAND_TYPE_GENERATE_MESH:
                case RENDER_COMMAND_TYPE_GENERATE_TEXTURE: {
                    gl_generate( ctx, current );
                } break;
                case RENDER_COMMAND_TYPE_RETIRE_MESHES:
                case RENDER_COMMAND_TYPE_RETIRE_TEXTURES: {
                    gl_retire( ctx, current );
                } break;
                case RENDER_COMMAND_TYPE_SET_DIRECTIONAL_LIGHT: {
                    gl_light_buffer_directional_set(
                        ctx->buffers[GL_BUFFER_INDEX_UBO_LIGHTS],
                        &ctx->lights,
                        current->directional_light.direction,
                        current->directional_light.color );
                } break;
                case RENDER_COMMAND_TYPE_SET_POINT_LIGHT: {
                    // TODO(alicia): better way to do this!
                    if( current->point_light.is_active ) {
                        gl_light_buffer_point_set(
                            ctx->buffers[GL_BUFFER_INDEX_UBO_LIGHTS],
                            &ctx->lights,
                            current->point_light.index,
                            current->point_light.position,
                            current->point_light.color,
                            current->point_light.is_active );
                    } else {
                        gl_light_buffer_point_set_active(
                            ctx->buffers[GL_BUFFER_INDEX_UBO_LIGHTS],
                            &ctx->lights,
                            current->point_light.index,
                            current->point_light.is_active );
                    }
                } break;
                default: UNIMPLEMENTED();
            }
            render_data->command_count--;
        }
    }

    // remap render ids
    Map_u32_u32* mesh_map    = &ctx->ctx.mesh_map;
    Map_u32_u32* texture_map = &ctx->ctx.texture_map;
    for( usize i = 0; i < render_data->command_count; ++i ) {
        RenderCommand* current = render_data->command_buffer + i;
        RenderID id = 0;
        if( !map_u32_u32_get( mesh_map, current->draw.mesh, &id ) ) {
            id = GL_VERTEX_ARRAY_INDEX_CUBE_3D;
        }
        current->draw.mesh = id;

        if( !map_u32_u32_get(
            texture_map, current->draw.texture_diffuse, &id
        ) ) {
            id = ctx->textures_2d[GL_TEXTURE_INDEX_NULL_DIFFUSE].id;
        }
        current->draw.texture_diffuse = id;

        if( !map_u32_u32_get(
            texture_map, current->draw.texture_normal, &id
        ) ) {
            id = ctx->textures_2d[GL_TEXTURE_INDEX_NULL_NORMAL].id;
        }
        current->draw.texture_normal = id;

        if( !map_u32_u32_get(
            texture_map, current->draw.texture_roughness, &id
        ) ) {
            id = ctx->textures_2d[GL_TEXTURE_INDEX_NULL_ROUGHNESS].id;
        }
        current->draw.texture_roughness = id;

        if( !map_u32_u32_get(
            texture_map, current->draw.texture_metallic, &id
        ) ) {
            id = ctx->textures_2d[GL_TEXTURE_INDEX_NULL_ROUGHNESS].id;
        }
        current->draw.texture_metallic = id;
    }

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
    for( usize i = 0; i < render_data->command_count; ++i ) {
        RenderCommand* current = render_data->command_buffer + i;
        if(
            !bitfield_check( current->draw.flags, DRAW_FLAG_SHADOW_CASTER ) ||
            bitfield_check( current->draw.flags, DRAW_FLAG_IS_WIREFRAME )
        ) {
            continue;
        }
        glBindFramebuffer( GL_FRAMEBUFFER, fbo_shadow_directional->id );
        glViewport( 0, 0,
            fbo_shadow_directional->width, fbo_shadow_directional->height );

        GLVertexArray* array = ctx->vertex_arrays + current->draw.mesh;
        glBindVertexArray( array->id );

        glProgramUniformMatrix4fv(
            sh_shadow_directional,
            GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE,
            current->draw.transform.c );
        glProgramUniformMatrix4fv(
            sh_shadow_point,
            GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE,
            current->draw.transform.c );

        glUseProgram( sh_shadow_directional );

        glDrawElements(
            array->default_mode,
            array->index_count,
            array->index_type,
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

            glDrawElements(
                array->default_mode,
                array->index_count,
                array->index_type,
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
    for( usize i = 0; i < render_data->command_count; ++i ) {
        // TODO(alicia): transparency

        RenderCommand* current = render_data->command_buffer + i;
        GLVertexArray* array   = ctx->vertex_arrays + current->draw.mesh;
        glBindVertexArray( array->id );

        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_DIFFUSE_TEXTURE,
            current->draw.texture_diffuse );
        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_NORMAL_TEXTURE,
            current->draw.texture_normal );
        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_ROUGHNESS_TEXTURE,
            current->draw.texture_roughness );
        glBindTextureUnit(
            GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_METALLIC_TEXTURE,
            current->draw.texture_metallic );
 
        glProgramUniform3fv(
            sh_phong, GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_TINT,
            1, current->draw.tint.c );
        glProgramUniform1i(
            sh_phong,
            GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_SHADOW_RECEIVER,
            bitfield_check( current->draw.flags, DRAW_FLAG_SHADOW_RECEIVER ) );
        glProgramUniformMatrix4fv(
            sh_phong, GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
            1, GL_FALSE, current->draw.transform.c );
        mat3 normal_mat = m4_normal_matrix_unchecked( &current->draw.transform );
        glProgramUniformMatrix3fv(
            sh_phong, GL_SHADER_PROGRAM_LOCATION_NORMAL_TRANSFORM,
            1, GL_FALSE, normal_mat.c );
        // TODO(alicia): index count!!

        GLenum mode = array->default_mode;
        if( bitfield_check( current->draw.flags, DRAW_FLAG_IS_WIREFRAME ) ) {
            mode = GL_LINES;
        }
        glDrawElements(
            mode,
            array->index_count,
            array->index_type,
            NULL
        );
    }

    // NOTE(alicia): UI Rendering
    // glDisable( GL_DEPTH_TEST );
    // glEnable( GL_BLEND );
    // glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    //
    // for( usize i = 0; i < render_data->object_count; ++i ) {
    //     RenderObject* object = render_data->objects + i;
    //     GLShaderProgramID program_id = 0;
    //
    //     switch( object->material.shader ) {
    //         case RENDER_SHADER_DEBUG_COLOR: {
    //             program_id = ctx->programs[GL_SHADER_PROGRAM_INDEX_COLOR];
    //             glUseProgram( program_id );
    //             glProgramUniform4fv(
    //                 program_id,
    //                 GL_SHADER_PROGRAM_COLOR_LOCATION_COLOR,
    //                 1, object->material.debug_color.color.c
    //             );
    //         } break;
    //         default: continue;
    //     }
    //
    //     switch( object->mesh ) {
    //         case RENDER_MESH_QUAD_2D_LOWER_LEFT: {
    //             glBindVertexArray(
    //                 ctx->vertex_arrays[GL_VERTEX_ARRAY_INDEX_QUAD_2D] );
    //         } break;
    //         default: continue;
    //     }
    //
    //     glProgramUniformMatrix4fv(
    //         program_id,
    //         GL_SHADER_PROGRAM_LOCATION_TRANSFORM,
    //         1, GL_FALSE,
    //         object->material.transform.c
    //     );
    //     glDrawElements(
    //         GL_TRIANGLES,
    //         QUAD_2D_INDEX_COUNT,
    //         GL_UNSIGNED_BYTE,
    //         NULL
    //     );
    // }

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
    glCreateBuffers( GL_DEFAULT_BUFFER_COUNT, ctx->buffers );
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

    /* create quad 2d mesh */ {
        GLVertexArray* current =
            ctx->vertex_arrays + GL_VERTEX_ARRAY_INDEX_QUAD_2D;
        glCreateVertexArrays( 1, &current->id );
        GLuint vao = current->id;
        GLuint vbo = ctx->buffers[GL_BUFFER_INDEX_VBO_QUAD_2D];
        GLuint ebo = ctx->buffers[GL_BUFFER_INDEX_EBO_QUAD];

        current->index_type   = GL_UNSIGNED_BYTE;
        current->index_count  = QUAD_2D_INDEX_COUNT;
        current->default_mode = GL_TRIANGLES;
        
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
        GLVertexArray* current =
            ctx->vertex_arrays + GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER;
        glCreateVertexArrays( 1, &current->id );
        GLuint vao = current->id;
        GLuint vbo = ctx->buffers[GL_BUFFER_INDEX_VBO_FRAMEBUFFER];

        current->default_mode = GL_TRIANGLES;
        current->index_type   = GL_UNSIGNED_BYTE;
        current->index_count  = QUAD_2D_INDEX_COUNT;
     
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
            0 );
        glVertexArrayAttribFormat(
            vao, 1,
            2, GL_FLOAT,
            GL_FALSE,
            sizeof(f32) * 2 );

        glVertexArrayAttribBinding( vao, 0, 0 );
        glVertexArrayAttribBinding( vao, 1, 0 );
    }
    /* create cube 3d mesh */ {
        GLVertexArray* current =
            ctx->vertex_arrays +GL_VERTEX_ARRAY_INDEX_CUBE_3D;
        glCreateVertexArrays( 1, &current->id );
        GLuint vao = current->id;
        GLuint vbo = ctx->buffers[GL_BUFFER_INDEX_VBO_CUBE_3D];
        GLuint ebo = ctx->buffers[GL_BUFFER_INDEX_EBO_CUBE_3D];
        
        current->index_type   = GL_UNSIGNED_BYTE;
        current->index_count  = CUBE_3D_INDEX_COUNT;
        current->default_mode = GL_TRIANGLES;

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

maybe_unused
internal GLenum gl_texture_type( GraphicsTextureType type ) {
    assert( type < GRAPHICS_TEXTURE_TYPE_COUNT );
    GLenum types[GRAPHICS_TEXTURE_TYPE_COUNT] = {
        GL_TEXTURE_2D,
        GL_TEXTURE_3D
    };
    return types[type];
}
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
internal GLenum gl_texture_wrap( GraphicsTextureWrap wrap ) {
    assert( wrap < GRAPHICS_TEXTURE_WRAP_COUNT );
    GLenum wraps[GRAPHICS_TEXTURE_WRAP_COUNT] = {
        GL_CLAMP_TO_EDGE,
        GL_REPEAT
    };
    return wraps[wrap];
}
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


