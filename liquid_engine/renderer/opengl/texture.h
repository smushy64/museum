#if !defined(LD_RENDERER_OPENGL_TEXTURE_H)
#define LD_RENDERER_OPENGL_TEXTURE_H
// * Description:  OpenGL Textures
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 29, 2023
#include "defines.h"
#include "renderer/opengl/types.h"

/// Create a 2D texture.
GLTexture gl_texture_create(
    i32 width, i32 height, i32 mipmap_level,
    GLType type,
    GLInternalFormat internal_format,
    GLFormat         format,
    GLWrapMode  wrap_x, GLWrapMode wrap_y,
    GLMagFilter mag_filter,
    GLMinFilter min_filter,
    void* buffer
);
/// Destroy 2D textures.
void gl_texture_destroy( usize count, GLTexture* textures );

#endif // header guard
