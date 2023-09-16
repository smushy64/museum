#if !defined(LD_RENDERER_OPENGL_TEXTURE_H)
#define LD_RENDERER_OPENGL_TEXTURE_H
// * Description:  OpenGL Textures
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 29, 2023
#include "defines.h"
#include "renderer/opengl/types.h"

/// Create a 2D texture.
GLTexture2D gl_texture_2d_create(
    i32              width,
    i32              height,
    i32              mipmap_level,
    GLType           type,
    GLInternalFormat internal_format,
    GLFormat         format,
    GLWrapMode       wrap_x,
    GLWrapMode       wrap_y,
    GLMagFilter      mag_filter,
    GLMinFilter      min_filter,
    void*            buffer
);
/// Destroy 2D textures.
void gl_texture_2d_destroy( usize count, GLTexture2D* textures );



#endif // header guard
