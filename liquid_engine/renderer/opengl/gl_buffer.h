#if !defined(RENDERER_OPENGL_GL_BUFFER_HPP)
#define RENDERER_OPENGL_GL_BUFFER_HPP
/**
 * Description:  OpenGL Buffers
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 16, 2023
*/
#include "gl_types.h"

void gl_create_standard_mesh(
    void* vertices,
    u32*  indices,
    u32   vertex_count,
    u32   index_count,
    GLuint vertex_array_handle,
    GLuint vertex_buffer_handle,
    GLuint element_buffer_handle,
    GLenum usage
);

#endif // header guard
