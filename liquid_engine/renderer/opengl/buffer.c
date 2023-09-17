// * Description:  OpenGL Buffer Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "renderer/opengl/buffer.h"
#include "renderer/opengl/functions.h"
#include "renderer/opengl/texture.h"
#include "renderer/opengl.h"
#include "core/mem.h"
#include "core/mathf.h"

void gl_light_buffer_create(
    GLBufferID id, struct GLLightBuffer* opt_buffer
) {
    glNamedBufferStorage(
        id, GL_LIGHT_BUFFER_SIZE,
        opt_buffer, GL_DYNAMIC_STORAGE_BIT );
    glBindBufferBase( GL_UNIFORM_BUFFER, GL_LIGHT_BUFFER_BINDING, id );
}
void gl_light_buffer_directional_set_direction(
    GLBufferID id, struct GLLightBuffer* buffer, vec3 direction
) {
    mat4 proj = m4_ortho( -10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f );
    mat4 view = m4_view( v3_neg( direction ), VEC3_ZERO, VEC3_UP );

    buffer->directional.light_space = m4_mul_m4( &proj, &view );
    buffer->directional.direction   = v4_v3( direction );

    usize offset =
        offsetof( struct GLLightBuffer, directional ) +
        offsetof( struct GLDirectionalLight, direction ); 
    usize size = sizeof(vec4) + sizeof(mat4);
    glNamedBufferSubData(
        id, offset, size, &buffer->directional.direction );
}
void gl_light_buffer_directional_set_color(
    GLBufferID id, struct GLLightBuffer* buffer, vec3 color
) {
    buffer->directional.color = rgba_rgb( color );

    usize offset =
        offsetof( struct GLLightBuffer, directional ) +
        offsetof( struct GLDirectionalLight, color );
    usize size = sizeof(vec4);
    glNamedBufferSubData(
        id, offset, size, &buffer->directional.color );
}
void gl_light_buffer_directional_set(
    GLBufferID id, struct GLLightBuffer* buffer,
    vec3 direction, vec3 color
) {
    mat4 proj = m4_ortho( -10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f );
    mat4 view = m4_view( v3_neg( direction ), VEC3_ZERO, VEC3_UP );

    buffer->directional.light_space = m4_mul_m4( &proj, &view );
    buffer->directional.direction   = v4_v3( direction );
    buffer->directional.color = rgba_rgb( color );

    usize offset = offsetof( struct GLLightBuffer, directional );
    usize size   = sizeof( struct GLDirectionalLight );

    glNamedBufferSubData( id, offset, size, &buffer->directional );
}
void gl_light_buffer_point_set_position(
    GLBufferID id, struct GLLightBuffer* buffer, usize index, vec3 position
) {
    assert( index < GL_POINT_LIGHT_COUNT );
    struct GLPointLight* light = buffer->point + index;
    light->position = v4_v3( position );

    light->near_clip = 1.0f;
    light->far_clip  = 25.0f;
    mat4 proj = m4_perspective(
        to_rad32( 90.0f ), 1.0f,
        light->near_clip, light->far_clip
    );

    mat4 views[6];
    views[0] =
        m4_view( position, v3_add( position, VEC3_RIGHT ), VEC3_DOWN );
    views[1] =
        m4_view( position, v3_add( position, VEC3_LEFT ), VEC3_DOWN );
    views[2] =
        m4_view( position, v3_add( position, VEC3_UP ), VEC3_FORWARD );
    views[3] =
        m4_view( position, v3_add( position, VEC3_DOWN ), VEC3_BACK );
    views[4] =
        m4_view( position, v3_add( position, VEC3_FORWARD ), VEC3_DOWN );
    views[5] =
        m4_view( position, v3_add( position, VEC3_BACK ), VEC3_DOWN );

    for( u32 i = 0; i < 6; ++i ) {
        light->light_space[i] = m4_mul_m4( &proj, views + i );
    }

    usize point_offset =
        offsetof( struct GLLightBuffer, point ) +
        ( sizeof( struct GLPointLight ) * index );

    usize offset =
        point_offset +
        offsetof( struct GLPointLight, position );
    usize size = sizeof(vec4) + (sizeof(mat4) * 6);
    glNamedBufferSubData(
        id, offset, size, &light->position );

    if( !light->clipping_planes_set ) {
        offset =
            point_offset +
            offsetof( struct GLPointLight, near_clip );
        size = sizeof(f32) * 2;

        glNamedBufferSubData(
            id, offset, size, &light->near_clip );

        light->clipping_planes_set = true;
    }
}
void gl_light_buffer_point_set_color(
    GLBufferID id, struct GLLightBuffer* buffer, usize index, vec3 color
) {
    assert( index < GL_POINT_LIGHT_COUNT );
    struct GLPointLight* light = buffer->point + index;
    light->color = rgba_rgb( color );

    usize offset =
        offsetof( struct GLLightBuffer, point ) +
        ( sizeof( struct GLPointLight ) * index ) +
        offsetof( struct GLPointLight, color );
    usize size = sizeof(vec4);
    glNamedBufferSubData(
        id, offset, size, &light->color );
}
void gl_light_buffer_point_set_active(
    GLBufferID id, struct GLLightBuffer* buffer, usize index, b32 is_active
) {
    assert( index < GL_POINT_LIGHT_COUNT );
    struct GLPointLight* light = buffer->point + index;
    light->is_active = is_active ? 1.0f : 0.0f;

    usize offset =
        offsetof( struct GLLightBuffer, point ) +
        ( sizeof( struct GLPointLight ) * index ) +
        offsetof( struct GLPointLight, is_active );
    usize size = sizeof(f32);
    glNamedBufferSubData(
        id, offset, size, &light->is_active );
}
void gl_light_buffer_point_set(
    GLBufferID id, struct GLLightBuffer* buffer, usize index,
    vec3 position, vec3 color, b32 is_active
) {
    assert( index < GL_POINT_LIGHT_COUNT );
    struct GLPointLight* light = buffer->point + index;
    light->position = v4_v3( position );
    light->color = rgba_rgb( color );
    light->is_active = is_active ? 1.0f : 0.0f;

    light->near_clip = 1.0f;
    light->far_clip  = 25.0f;
    mat4 proj = m4_perspective(
        to_rad32( 90.0f ), 1.0f,
        light->near_clip, light->far_clip
    );

    mat4 views[6];
    views[0] =
        m4_view( position, v3_add( position, VEC3_RIGHT ), VEC3_DOWN );
    views[1] =
        m4_view( position, v3_add( position, VEC3_LEFT ), VEC3_DOWN );
    views[2] =
        m4_view( position, v3_add( position, VEC3_UP ), VEC3_FORWARD );
    views[3] =
        m4_view( position, v3_add( position, VEC3_DOWN ), VEC3_BACK );
    views[4] =
        m4_view( position, v3_add( position, VEC3_FORWARD ), VEC3_DOWN );
    views[5] =
        m4_view( position, v3_add( position, VEC3_BACK ), VEC3_DOWN );

    for( u32 i = 0; i < 6; ++i ) {
        light->light_space[i] = m4_mul_m4( &proj, views + i );
    }

    usize offset =
        offsetof( struct GLLightBuffer, point ) +
        ( sizeof( struct GLPointLight ) * index );
    usize size = sizeof( struct GLPointLight );
    glNamedBufferSubData( id, offset, size, light );
}
b32 gl_light_buffer_point_is_active(
    struct GLLightBuffer* buffer, usize index
) {
    assert( index < GL_POINT_LIGHT_COUNT );
    struct GLPointLight* light = buffer->point + index;
    return light->is_active > 0.1f;
}

void gl_camera_buffer_create(
    GLBufferID buffer_id, struct GLCameraBuffer* camera
) {
    glNamedBufferStorage(
        buffer_id, GL_CAMERA_BUFFER_SIZE,
        camera, GL_DYNAMIC_STORAGE_BIT );
    glBindBufferBase(
        GL_UNIFORM_BUFFER, GL_CAMERA_BUFFER_BINDING, buffer_id );
}
void gl_camera_buffer_update(
    GLBufferID buffer_id, struct GLCameraBuffer* camera
) {
    glNamedBufferSubData(
        buffer_id, 0,
        GL_CAMERA_BUFFER_SIZE,
        camera
    );
}
void gl_camera_buffer_update_matrix_3d(
    GLBufferID buffer_id, mat4* matrix_3d
) {
    glNamedBufferSubData(
        buffer_id,
        GL_CAMERA_BUFFER_OFFSET_MATRIX_3D,
        sizeof(mat4), matrix_3d->c
    );
}
void gl_camera_buffer_update_matrix_ui(
    GLBufferID buffer_id, mat4* matrix_ui
) {
    glNamedBufferSubData(
        buffer_id,
        GL_CAMERA_BUFFER_OFFSET_MATRIX_UI,
        sizeof(mat4), matrix_ui->c
    );
}
void gl_camera_buffer_update_matrices(
    GLBufferID buffer_id, mat4 matrices[2]
) {
    glNamedBufferSubData(
        buffer_id,
        GL_CAMERA_BUFFER_OFFSET_MATRICES,
        sizeof(mat4) * 2, matrices
    );
}
void gl_camera_buffer_update_world_position(
    GLBufferID buffer_id, vec3 world_position
) {
    glNamedBufferSubData(
        buffer_id,
        GL_CAMERA_BUFFER_OFFSET_CAMERA_WORLD_POSITION,
        sizeof(vec3), &world_position
    );
}
void gl_camera_buffer_update_near_far_planes(
    GLBufferID buffer_id, f32 planes[2]
) {
    glNamedBufferSubData(
        buffer_id,
        GL_CAMERA_BUFFER_OFFSET_CAMERA_PLANES,
        sizeof(f32) * 2, planes
    );
}

GLFramebuffer gl_framebuffer_create( i32 width, i32 height ) {
    GLFramebuffer result = {};
    result.width  = width;
    result.height = height;

    glCreateFramebuffers( 1, &result.id );

    glCreateTextures(
        GL_TEXTURE_2D,
        FRAMEBUFFER_TEXTURE_ID_COUNT,
        result.texture_ids
    );

    glTextureStorage2D(
        result.color_texture_id, 1,
        GL_RGBA8,
        result.width, result.height
    );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTextureStorage2D(
        result.depth_texture_id, 1,
        GL_DEPTH_COMPONENT24,
        result.width, result.height
    );

    glNamedFramebufferTexture(
        result.id,
        GL_COLOR_ATTACHMENT0,
        result.color_texture_id, 0
    );

    glNamedFramebufferTexture(
        result.id,
        GL_DEPTH_ATTACHMENT,
        result.depth_texture_id, 0
    );

    glNamedFramebufferDrawBuffer( result.id, GL_COLOR_ATTACHMENT0 );

#if defined(LD_ASSERTIONS)
    GLenum status = glCheckNamedFramebufferStatus( result.id, GL_FRAMEBUFFER );
    if( status != GL_FRAMEBUFFER_COMPLETE ) {
        GL_LOG_ERROR( "Framebuffer incomplete!" );
        panic();
    }
#endif

    return result;
}
void gl_framebuffer_resize(
    GLFramebuffer* framebuffer, i32 width, i32 height
) {
    gl_framebuffer_destroy( 1, framebuffer );
    *framebuffer = gl_framebuffer_create( width, height );
}
void gl_framebuffer_destroy( usize count, GLFramebuffer* framebuffers ) {
    for( usize i = 0; i < count; ++i ) {
        glDeleteFramebuffers( 1, &framebuffers[i].id );
        glDeleteTextures(
            FRAMEBUFFER_TEXTURE_ID_COUNT,
            framebuffers[i].texture_ids
        );
    }

    mem_zero( framebuffers, sizeof(GLFramebuffer) * count );
}

GLFramebuffer gl_shadowbuffer_create(
    i32 width, i32 height, GLShadowBufferType type
) {
    GLFramebuffer result = {};
    result.dimensions = iv2( width, height );
    glCreateFramebuffers( 1, &result.id );

    switch( type ) {
        case GL_SHADOWBUFFER_DIRECTIONAL: {
            glCreateTextures(
                GL_TEXTURE_2D, 1, &result.shadow_texture_id );

            glTextureStorage2D(
                result.shadow_texture_id, 1,
                GL_DEPTH_COMPONENT24,
                width, height
            );

            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_MIN_FILTER, GL_NEAREST );

            vec4 border_color = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTextureParameterfv(
                result.shadow_texture_id,
                GL_TEXTURE_BORDER_COLOR, border_color.c );
            glNamedFramebufferTexture(
                result.id,
                GL_DEPTH_ATTACHMENT,
                result.shadow_texture_id, 0
            );

            glNamedFramebufferDrawBuffer( result.id, GL_NONE );

        } break;
        case GL_SHADOWBUFFER_POINT: {
            glCreateTextures(
                GL_TEXTURE_CUBE_MAP_ARRAY, 1, &result.shadow_texture_id );

            glTextureStorage3D(
                result.shadow_texture_id, 1,
                GL_DEPTH_COMPONENT32F,
                width, height, 6
            );

            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTextureParameteri(
                result.shadow_texture_id,
                GL_TEXTURE_MIN_FILTER, GL_NEAREST );

            glNamedFramebufferTexture(
                result.id,
                GL_DEPTH_ATTACHMENT,
                result.shadow_texture_id, 0 );

            glNamedFramebufferDrawBuffer( result.id, GL_NONE );
        } break;
    }

#if defined(LD_ASSERTIONS)
    GLenum status = glCheckNamedFramebufferStatus( result.id, GL_FRAMEBUFFER );
    if( status != GL_FRAMEBUFFER_COMPLETE ) {
        GL_LOG_ERROR( "Framebuffer incomplete!" );
        panic();
    }
#endif

    return result;
}

