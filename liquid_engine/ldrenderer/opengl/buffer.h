#if !defined(LD_RENDERER_OPENGL_BUFFER_HPP)
#define LD_RENDERER_OPENGL_BUFFER_HPP
// * Description:  OpenGL Buffer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/ldmath/types.h"
#include "ldrenderer/opengl/types.h"

struct packed GLCameraBuffer {
    mat4 matrix_3d;
    mat4 matrix_ui;

    vec3 camera_world_position;
    int  ___padding0;

    f32 camera_near;
    f32 camera_far;
};

#define GL_CAMERA_BUFFER_OFFSET_MATRIX_3D (0)

#define GL_CAMERA_BUFFER_OFFSET_MATRIX_UI\
    (GL_CAMERA_BUFFER_OFFSET_MATRIX_3D + sizeof(mat4))

#define GL_CAMERA_BUFFER_OFFSET_MATRICES\
    (GL_CAMERA_BUFFER_OFFSET_MATRIX_3D)

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_WORLD_POSITION\
    (GL_CAMERA_BUFFER_OFFSET_MATRIX_UI + sizeof(mat4))

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_NEAR\
    (GL_CAMERA_BUFFER_OFFSET_CAMERA_WORLD_POSITION + sizeof(vec4))

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_FAR\
    (GL_CAMERA_BUFFER_OFFSET_CAMERA_NEAR + sizeof(f32))

#define GL_CAMERA_BUFFER_OFFSET_CAMERA_PLANES\
    (GL_CAMERA_BUFFER_OFFSET_CAMERA_NEAR)

#define GL_CAMERA_BUFFER_SIZE    (sizeof(struct GLCameraBuffer))
#define GL_CAMERA_BUFFER_BINDING (0)

#define GL_BUFFER_INDEX_CAMERA (0)

/// Make camera buffer
void gl_camera_buffer_create(
    GLBufferID buffer_id, struct GLCameraBuffer* camera );
/// Update all camera buffer fields.
void gl_camera_buffer_update(
    GLBufferID buffer_id, struct GLCameraBuffer* camera );
/// Update 3d view projection matrix.
void gl_camera_buffer_update_matrix_3d(
    GLBufferID buffer_id, mat4* matrix_3d );
/// Update ui view projection matrix.
void gl_camera_buffer_update_matrix_ui(
    GLBufferID buffer_id, mat4* matrix_ui );
/// Update both view projection matrices.
void gl_camera_buffer_update_matrices(
    GLBufferID buffer_id, mat4 matrices[2] );
/// Update camera world position.
void gl_camera_buffer_update_world_position(
    GLBufferID buffer_id, vec3 world_position );
/// Update camera near and far planes.
void gl_camera_buffer_update_near_far_planes(
    GLBufferID buffer_id, f32 planes[2] );

#endif // header guard
