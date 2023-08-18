/**
 * Description:  OpenGL Buffer Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 16, 2023
*/
#include "gl_buffer.h"
#include "gl_backend.h"
#include "gl_functions.h"
#include "core/ldmath.h"

#include "core/ldmemory.h"

// TODO(alicia): custom std args ?
#include <stdarg.h>

#define STANDARD_MESH_VERTEX_SIZE\
    sizeof(vec3) +\
    sizeof(vec2) +\
    sizeof(vec4)

void gl_create_standard_mesh(
    void* vertices,
    u32*  indices,
    u32   vertex_count,
    u32   index_count,
    GLuint vertex_array_handle,
    GLuint vertex_buffer_handle,
    GLuint element_buffer_handle,
    GLenum usage
) {
    unused(indices);
    unused(index_count);
    unused(element_buffer_handle);
    glNamedBufferData(
        vertex_buffer_handle,
        vertex_count * STANDARD_MESH_VERTEX_SIZE,
        vertices,
        usage
    );
    glVertexArrayVertexBuffer(
        vertex_array_handle,
        0,
        vertex_buffer_handle,
        0, STANDARD_MESH_VERTEX_SIZE
    );
}

