// * Description:  OpenGL Buffer Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "ldrenderer/opengl/buffer.h"
#include "ldrenderer/opengl/functions.h"

void gl_matrices_buffer_create( GLBufferID id, mat4* matrices ) {
    glNamedBufferStorage(
        id,
        GL_MATRICES_BUFFER_SIZE,
        matrices,
        GL_DYNAMIC_STORAGE_BIT
    );
    glBindBufferBase( GL_UNIFORM_BUFFER, GL_MATRICES_BUFFER_BINDING, id );
}
void gl_matrices_buffer_update_ui( GLBufferID id, mat4* view_projection ) {
    glNamedBufferSubData(
        id,
        GL_MATRICES_BUFFER_OFFSET_VIEW_PROJECTION_UI,
        sizeof(mat4),
        view_projection
    );
}
void gl_matrices_buffer_update_3d( GLBufferID id, mat4* view_projection ) {
    glNamedBufferSubData(
        id,
        GL_MATRICES_BUFFER_OFFSET_VIEW_PROJECTION_3D,
        sizeof(mat4),
        view_projection
    );
}
void gl_matrices_buffer_update_2d( GLBufferID id, mat4* view_projection ) {
    glNamedBufferSubData(
        id,
        GL_MATRICES_BUFFER_OFFSET_VIEW_PROJECTION_2D,
        sizeof(mat4),
        view_projection
    );
}

