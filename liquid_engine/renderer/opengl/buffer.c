// * Description:  OpenGL Buffer Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "renderer/opengl/buffer.h"
#include "renderer/opengl/functions.h"
#include "renderer/opengl/texture.h"
#include "renderer/opengl.h"
#include "core/mem.h"

void gl_light_buffer_create(
    GLBufferID buffer_id, struct GLLightBuffer* light
) {
    glNamedBufferStorage(
        buffer_id, GL_LIGHT_BUFFER_SIZE,
        light, GL_DYNAMIC_STORAGE_BIT );
    glBindBufferBase(
        GL_UNIFORM_BUFFER, GL_LIGHT_BUFFER_BINDING, buffer_id );
}
void gl_light_buffer_update(
    GLBufferID buffer_id, struct GLLightBuffer* light
) {
    glNamedBufferSubData(
        buffer_id, 0, GL_LIGHT_BUFFER_SIZE, light );
}
void gl_light_buffer_update_directional(
    GLBufferID buffer_id, struct GLDirectionalLight* directional
) {
    glNamedBufferSubData(
        buffer_id, 0, GL_DIRECTIONAL_LIGHT_BUFFER_SIZE, directional );
}
void gl_light_buffer_update_point(
    GLBufferID buffer_id, usize index, struct GLPointLight* point
) {
    glNamedBufferSubData(
        buffer_id, GL_LIGHT_BUFFER_OFFSET_POINT( index ),
        GL_POINT_LIGHT_BUFFER_SIZE, point );
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
        result.color_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTextureParameteri(
        result.color_texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

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

GLFramebuffer gl_shadowbuffer_create( i32 width, i32 height ) {
    GLFramebuffer result;
    result.dimensions = iv2( width, height );

    glCreateFramebuffers( 1, &result.id );

    glCreateTextures( GL_TEXTURE_2D, 1, &result.shadow_texture_id );

    glTextureStorage2D(
        result.shadow_texture_id, 1,
        GL_DEPTH_COMPONENT24,
        width, height
    );

    glTextureParameteri(
        result.shadow_texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTextureParameteri(
        result.shadow_texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTextureParameteri(
        result.shadow_texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTextureParameteri(
        result.shadow_texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    vec4 border_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTextureParameterfv(
        result.shadow_texture_id, GL_TEXTURE_BORDER_COLOR, border_color.c );

    glNamedFramebufferTexture(
        result.id,
        GL_DEPTH_ATTACHMENT,
        result.shadow_texture_id, 0
    );

    glNamedFramebufferDrawBuffer( result.id, GL_NONE );

#if defined(LD_ASSERTIONS)
    GLenum status = glCheckNamedFramebufferStatus( result.id, GL_FRAMEBUFFER );
    if( status != GL_FRAMEBUFFER_COMPLETE ) {
        GL_LOG_ERROR( "Framebuffer incomplete!" );
        panic();
    }
#endif

    return result;
}

