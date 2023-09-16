// * Description:  OpenGL Textures Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 29, 2023
#include "renderer/opengl/texture.h"
#include "renderer/opengl/functions.h"

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
) {
    GLTexture2D result = {};

    glCreateTextures( GL_TEXTURE_2D, 1, &result.id );

    glTextureParameteri( result.id, GL_TEXTURE_WRAP_S, wrap_x );
    glTextureParameteri( result.id, GL_TEXTURE_WRAP_T, wrap_y );
    glTextureParameteri( result.id, GL_TEXTURE_MAG_FILTER, mag_filter );
    glTextureParameteri( result.id, GL_TEXTURE_MIN_FILTER, min_filter );

    glGenerateTextureMipmap( result.id );

    glTextureStorage2D(
        result.id, 1,
        internal_format,
        width, height
    );
    glTextureSubImage2D(
        result.id, mipmap_level,
        0, 0,
        width, height,
        format, type,
        buffer
    );

    result.width           = width;
    result.height          = height;
    result.mipmap_level    = mipmap_level;
    result.type            = type;
    result.internal_format = internal_format;
    result.format          = format;
    result.wrap_x          = wrap_x;
    result.wrap_y          = wrap_y;
    result.mag_filter      = mag_filter;
    result.min_filter      = min_filter;

    return result;
}

void gl_texture_2d_destroy( usize count, GLTexture2D* textures ) {
    for( usize i = 0; i < count; ++i ) {
        glDeleteTextures( 1, &textures[i].id );
    }
}

