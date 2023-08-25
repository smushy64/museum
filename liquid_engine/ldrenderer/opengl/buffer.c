// * Description:  OpenGL Buffer Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "ldrenderer/opengl/buffer.h"
#include "ldrenderer/opengl/functions.h"

void gl_camera_buffer_create(
    GLBufferID buffer_id, struct GLCameraBuffer* camera
) {
    glNamedBufferStorage(
        buffer_id, GL_CAMERA_BUFFER_SIZE,
        camera, GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase(
        GL_UNIFORM_BUFFER,
        GL_CAMERA_BUFFER_BINDING,
        buffer_id
    );
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
        sizeof(mat4), matrix_3d
    );
}
void gl_camera_buffer_update_matrix_ui(
    GLBufferID buffer_id, mat4* matrix_ui
) {
    glNamedBufferSubData(
        buffer_id,
        GL_CAMERA_BUFFER_OFFSET_MATRIX_UI,
        sizeof(mat4), matrix_ui
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

