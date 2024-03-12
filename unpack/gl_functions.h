#if !defined(UNPACK_GL_FUNCTIONS_H)
#define UNPACK_GL_FUNCTIONS_H
/**
 * Description:  OpenGL functions for unpack utility.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 06, 2024
*/
#include "shared/defines.h"

typedef u32 GLuint;
typedef i32 GLint;

typedef i32 GLsizei;

typedef GLuint GLenum;
typedef GLuint GLbitfield;

typedef char GLchar;
typedef unsigned char GLboolean;
typedef f32 GLfloat;

typedef void GLvoid;

typedef isize GLsizeiptr;
typedef isize GLintptr;

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_NEAREST 0x2600
#define GL_LINEAR  0x2601
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801

#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TEXTURE_WRAP_R 0x2804

#define GL_CLAMP_TO_EDGE 0x812F
#define GL_REPEAT 0x2901

#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82

#define GL_DEPTH_BUFFER_BIT   0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT   0x00004000

#define GL_RG 0x8227
#define GL_RED 0x1903
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_RGBA32F 0x8814
#define GL_RGB32F 0x8815
#define GL_RGBA16F 0x881A
#define GL_RGB16F 0x881B
#define GL_RGBA32UI 0x8D70
#define GL_RGB32UI 0x8D71
#define GL_RGBA16UI 0x8D76
#define GL_RGB16UI 0x8D77
#define GL_RGBA8UI 0x8D7C
#define GL_RGB8UI 0x8D7D
#define GL_RGBA32I 0x8D82
#define GL_RGB32I 0x8D83
#define GL_RGBA16I 0x8D88
#define GL_RGB16I 0x8D89
#define GL_RGBA8I 0x8D8E
#define GL_RGB8I 0x8D8F
#define GL_RG 0x8227
#define GL_RG_INTEGER 0x8228
#define GL_R8 0x8229
#define GL_R16 0x822A
#define GL_RG8 0x822B
#define GL_RG16 0x822C
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_RG16F 0x822F
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB8 0x8051
#define GL_RGB10 0x8052
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGBA2 0x8055
#define GL_RGBA4 0x8056
#define GL_RGB5_A1 0x8057
#define GL_RGBA8 0x8058
#define GL_RGB10_A2 0x8059
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_RGB8UI 0x8D7D
#define GL_RG32F 0x8230
#define GL_R8I 0x8231
#define GL_R8UI 0x8232
#define GL_R16I 0x8233
#define GL_R16UI 0x8234
#define GL_R32I 0x8235
#define GL_R32UI 0x8236
#define GL_RG8I 0x8237
#define GL_RG8UI 0x8238
#define GL_RG16I 0x8239
#define GL_RG16UI 0x823A
#define GL_RG32I 0x823B
#define GL_RG32UI 0x823C
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA

b32 gl_load_functions(void);

#define glfn( ret, fn, ... )\
    typedef ret fn##FN( __VA_ARGS__ );\
    extern fn##FN* ___internal_##fn;\
    header_only force_inline ret fn( __VA_ARGS__ )

glfn( void, glCreateTextures, GLenum target, GLsizei n, GLuint* textures ) {
    ___internal_glCreateTextures( target, n, textures );
}
glfn( void, glTextureParameteri, GLuint texture, GLenum pname, GLint param ) {
    ___internal_glTextureParameteri( texture, pname, param );
}
glfn( void, glGenerateTextureMipmap, GLuint texture ) {
    ___internal_glGenerateTextureMipmap( texture );
}
glfn( void, glTextureStorage2D,
    GLuint texture, GLsizei levels,
    GLenum internalFormat, GLsizei width, GLsizei height
) {
    ___internal_glTextureStorage2D( texture, levels, internalFormat, width, height );
}
glfn( void, glTextureStorage3D,
    GLuint texture, GLsizei levels,
    GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth
) {
    ___internal_glTextureStorage3D(
        texture, levels, internalFormat, width, height, depth );
}
glfn( void, glTextureSubImage2D,
    GLuint texture, GLint level, GLint xoffset, GLint yoffset,
    GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels
) {
    ___internal_glTextureSubImage2D(
        texture, level, xoffset, yoffset,
        width, height, format, type, pixels );
}
glfn( void, glTextureSubImage3D,
    GLuint texture, GLint level,
    GLint xoffset, GLint yoffset, GLint zoffset,
    GLsizei width, GLsizei height, GLsizei depth,
    GLenum format, GLenum type, const void* pixels
) {
    ___internal_glTextureSubImage3D(
        texture, level, xoffset, yoffset, zoffset,
        width, height, depth, format, type, pixels );
}
glfn( void, glBindTextureUnit, GLuint unit, GLuint texture ) {
    ___internal_glBindTextureUnit( unit, texture );
}
glfn( void, glClear, GLbitfield mask ) {
    ___internal_glClear( mask );
}
glfn( GLuint, glCreateShader, GLenum shaderType ) {
    return ___internal_glCreateShader( shaderType );
}
glfn( GLuint, glCreateProgram, void ) {
    return ___internal_glCreateProgram();
}
glfn( void, glShaderSource,
    GLuint shader, GLsizei count, const GLchar** string, const GLint* length
) {
    ___internal_glShaderSource( shader, count, string, length );
}
glfn( void, glCompileShader, GLenum shader ) {
    ___internal_glCompileShader( shader );
}
glfn( void, glGetShaderiv, GLuint shader, GLenum pname, GLint* params ) {
    ___internal_glGetShaderiv( shader, pname, params );
}
glfn( void, glGetShaderInfoLog,
    GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog
) {
    ___internal_glGetShaderInfoLog( shader, maxLength, length, infoLog );
}
glfn( void, glAttachShader, GLuint program, GLuint shader ) {
    ___internal_glAttachShader( program, shader );
}
glfn( void, glDetachShader, GLuint program, GLuint shader ) {
    ___internal_glDetachShader( program, shader );
}
glfn( void, glLinkProgram, GLuint program ) {
    ___internal_glLinkProgram( program );
}
glfn( void, glGetProgramiv, GLuint program, GLenum pname, GLint* params ) {
    ___internal_glGetProgramiv( program, pname, params );
}
glfn( void, glGetProgramInfoLog,
    GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog
) {
    ___internal_glGetProgramInfoLog( program, maxLength, length, infoLog );
}
glfn( void, glProgramUniform1i, GLuint program, GLint location, GLint v0 ) {
    ___internal_glProgramUniform1i( program, location, v0 );
}
glfn( void, glProgramUniformMatrix4fv,
    GLuint program, GLint location, GLsizei count,
    GLboolean transpose, const GLfloat* value
) {
    ___internal_glProgramUniformMatrix4fv( program, location, count, transpose, value );
}
glfn( void, glUseProgram, GLuint program ) {
    ___internal_glUseProgram( program );
}
glfn( void, glDeleteProgram, GLuint program ) {
    ___internal_glDeleteProgram( program );
}
glfn( void, glDeleteTextures, GLsizei n, const GLuint* textures ) {
    ___internal_glDeleteTextures( n, textures );
}
glfn( void, glCreateVertexArrays, GLsizei n, GLuint* arrays ) {
    ___internal_glCreateVertexArrays( n, arrays );
}
glfn( void, glDrawElements, GLenum mode, GLsizei count, GLenum type, const GLvoid* indices ) {
    ___internal_glDrawElements( mode, count, type, indices );
}
glfn( void, glCreateBuffers, GLsizei n, GLuint* buffers ) {
    ___internal_glCreateBuffers( n, buffers );
}
glfn( void, glNamedBufferData, GLuint buffer, GLsizeiptr size, const void* data, GLenum usage ) {
    ___internal_glNamedBufferData( buffer, size, data, usage );
}
glfn( void, glVertexArrayVertexBuffer, GLuint vao, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride ) {
    ___internal_glVertexArrayVertexBuffer( vao, bindingIndex, buffer, offset, stride );
}
glfn( void, glVertexArrayElementBuffer, GLuint vao, GLuint buffer ) {
    ___internal_glVertexArrayElementBuffer( vao, buffer );
}
glfn( void, glEnableVertexArrayAttrib, GLuint vao, GLuint index ) {
    ___internal_glEnableVertexArrayAttrib( vao, index );
}
glfn( void, glVertexArrayAttribFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset ) {
    ___internal_glVertexArrayAttribFormat( vao, attribIndex, size, type, normalized, relativeOffset );
}
glfn( void, glVertexArrayAttribBinding, GLuint vao, GLuint attribIndex, GLuint bindingIndex ) {
    ___internal_glVertexArrayAttribBinding( vao, attribIndex, bindingIndex );
}
glfn( void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height ) {
    ___internal_glViewport( x, y, width, height );
}
glfn( void, glClearColor, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
    ___internal_glClearColor( red, green, blue, alpha );
}
glfn( void, glBindVertexArray, GLuint array ) {
    ___internal_glBindVertexArray( array );
}

b32 gl_compile_shader(
    GLint source_len, const char* source, GLenum stage, GLuint* out_shader );
b32 gl_link_shader( usize count, GLuint* shaders, GLuint* out_shader_program );

extern const char global_texture_shader_vert[];
extern usize global_texture_shader_vert_len;
extern const char global_texture_shader_frag[];
extern usize global_texture_shader_frag_len;

extern f32 global_quad_verts[];
extern usize global_quad_vert_count;

extern u32 global_quad_indices[];
extern usize global_quad_index_count;

#undef glfn
#endif /* header guard */
