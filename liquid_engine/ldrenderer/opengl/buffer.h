#if !defined(LD_RENDERER_OPENGL_BUFFER_HPP)
#define LD_RENDERER_OPENGL_BUFFER_HPP
// * Description:  OpenGL Buffer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 21, 2023
#include "defines.h"
#include "core/ldmath/types.h"
#include "ldrenderer/opengl/types.h"

#define GL_MATRICES_BUFFER_OFFSET_VIEW_PROJECTION_3D (0)
#define GL_MATRICES_BUFFER_OFFSET_VIEW_PROJECTION_UI (sizeof(mat4))
#define GL_MATRICES_BUFFER_OFFSET_VIEW_PROJECTION_2D (sizeof(mat4) * 2)
#define GL_MATRICES_BUFFER_SIZE    (sizeof(mat4) * 3)
#define GL_MATRICES_BUFFER_BINDING (0)
#define GL_MATRICES_BUFFER_INDEX   (0)

/// Make matrices buffer.
void gl_matrices_buffer_create( GLBufferID id, mat4* matrices );
/// Update UI projection matrix.
void gl_matrices_buffer_update_ui( GLBufferID id, mat4* view_projection );
/// Update 2D projection matrix.
void gl_matrices_buffer_update_3d( GLBufferID id, mat4* view_projection );
/// Update 3D projection matrix.
void gl_matrices_buffer_update_2d( GLBufferID id, mat4* view_projection );

#endif // header guard
