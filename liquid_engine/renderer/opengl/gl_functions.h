#if !defined(RENDERER_OPENGL_GL_FUNCTIONS_HPP)
#define RENDERER_OPENGL_GL_FUNCTIONS_HPP
/**
 * Description:  OpenGL Function Declarations
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "gl_types.h"

namespace impl {

#define DEFINE_GL_FUNCTION( return_value, name, ... )\
    typedef return_value (*name##FN)( __VA_ARGS__ );\
    inline name##FN in_##name = nullptr;

// Textures ------------------------------------------------------------
DEFINE_GL_FUNCTION( void, glCreateTextures, GLenum, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glBindTextureUnit, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glDeleteTextures, GLsizei, const GLuint* );
DEFINE_GL_FUNCTION( void, glTextureParameterf, GLuint, GLenum, GLfloat );
DEFINE_GL_FUNCTION( void, glTextureParameteri, GLuint, GLenum, GLint );
DEFINE_GL_FUNCTION( void, glTextureParameterfv, GLuint, GLenum, const GLfloat* );
DEFINE_GL_FUNCTION( void, glTextureParameteriv, GLuint, GLenum, const GLint* );
DEFINE_GL_FUNCTION( void, glTextureParameterIiv, GLuint, GLenum, const GLint* );
DEFINE_GL_FUNCTION( void, glTextureParameterIuiv, GLuint, GLenum, const GLuint* );
DEFINE_GL_FUNCTION( void, glTextureStorage2D, GLuint, GLsizei, GLenum, GLsizei, GLsizei );
DEFINE_GL_FUNCTION( void, glTextureStorage3D, GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei );
DEFINE_GL_FUNCTION( void, glTextureSubImage2D, GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void* );
DEFINE_GL_FUNCTION( void, glTextureSubImage3D, GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void* );
// Rendering -----------------------------------------------------------
DEFINE_GL_FUNCTION( void, glClear, GLbitfield );
DEFINE_GL_FUNCTION( void, glClearColor, GLfloat, GLfloat, GLfloat, GLfloat );
DEFINE_GL_FUNCTION( void, glClearDepth, GLdouble );
DEFINE_GL_FUNCTION( void, glClearDepthf, GLfloat );
DEFINE_GL_FUNCTION( void, glClearStencil, GLint );
DEFINE_GL_FUNCTION( void, glClearNamedFramebufferiv, GLuint, GLenum, GLint, const GLint* );
DEFINE_GL_FUNCTION( void, glClearNamedFramebufferuiv, GLuint, GLenum, GLint, const GLuint* );
DEFINE_GL_FUNCTION( void, glClearNamedFramebufferfv, GLuint, GLenum, GLint, const GLfloat* );
DEFINE_GL_FUNCTION( void, glClearNamedFramebufferfi, GLuint, GLenum, GLint, const GLfloat, GLint );
// Frame Buffers -------------------------------------------------------
DEFINE_GL_FUNCTION( void, glCreateFramebuffers, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glCreateRenderbuffers, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glDeleteFramebuffers, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glDeleteRenderbuffers, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glBindFramebuffer, GLenum, GLuint );
DEFINE_GL_FUNCTION( void, glBindRenderbuffer, GLenum, GLuint );
DEFINE_GL_FUNCTION( void, glGenerateTextureMipmap, GLuint );
// Shaders -------------------------------------------------------------
DEFINE_GL_FUNCTION( GLuint, glCreateShaderProgramv, GLenum, GLsizei, const char** );
DEFINE_GL_FUNCTION( void, glCompileShader, GLuint );
DEFINE_GL_FUNCTION( void, glGetShaderiv, GLuint, GLenum, GLint* );
DEFINE_GL_FUNCTION( void, glGetProgramiv, GLuint, GLenum, GLint* );
DEFINE_GL_FUNCTION( void, glDeleteShader, GLuint );
DEFINE_GL_FUNCTION( void, glDeleteProgram, GLuint );
DEFINE_GL_FUNCTION( void, glAttachShader, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glDetachShader, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei*, GLchar* );
DEFINE_GL_FUNCTION( void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei*, GLchar* );
DEFINE_GL_FUNCTION( void, glGetActiveUniform, GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum, GLchar* );
DEFINE_GL_FUNCTION( GLint, glGetUniformLocation, GLuint, const GLchar* );
DEFINE_GL_FUNCTION( void, glProgramUniform1f, GLuint, GLint, GLfloat );
DEFINE_GL_FUNCTION( void, glProgramUniform2f, GLuint, GLint, GLfloat, GLfloat );
DEFINE_GL_FUNCTION( void, glProgramUniform3f, GLuint, GLint, GLfloat, GLfloat, GLfloat );
DEFINE_GL_FUNCTION( void, glProgramUniform4f, GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat );
DEFINE_GL_FUNCTION( void, glProgramUniform1i, GLuint, GLint, GLint );
DEFINE_GL_FUNCTION( void, glProgramUniform2i, GLuint, GLint, GLint, GLint );
DEFINE_GL_FUNCTION( void, glProgramUniform3i, GLuint, GLint, GLint, GLint, GLint );
DEFINE_GL_FUNCTION( void, glProgramUniform4i, GLuint, GLint, GLint, GLint, GLint, GLint );
DEFINE_GL_FUNCTION( void, glProgramUniform1ui, GLuint, GLint, GLuint );
DEFINE_GL_FUNCTION( void, glProgramUniform2ui, GLuint, GLint, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glProgramUniform3ui, GLuint, GLint, GLuint, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glProgramUniform4ui, GLuint, GLint, GLuint, GLuint, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glProgramUniform1fv, GLuint, GLint, GLsizei, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniform2fv, GLuint, GLint, GLsizei, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniform3fv, GLuint, GLint, GLsizei, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniform4fv, GLuint, GLint, GLsizei, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniform1iv, GLuint, GLint, GLsizei, const GLint* );
DEFINE_GL_FUNCTION( void, glProgramUniform2iv, GLuint, GLint, GLsizei, const GLint* );
DEFINE_GL_FUNCTION( void, glProgramUniform3iv, GLuint, GLint, GLsizei, const GLint* );
DEFINE_GL_FUNCTION( void, glProgramUniform4iv, GLuint, GLint, GLsizei, const GLint* );
DEFINE_GL_FUNCTION( void, glProgramUniform1uiv, GLuint, GLint, GLsizei, const GLuint* );
DEFINE_GL_FUNCTION( void, glProgramUniform2uiv, GLuint, GLint, GLsizei, const GLuint* );
DEFINE_GL_FUNCTION( void, glProgramUniform3uiv, GLuint, GLint, GLsizei, const GLuint* );
DEFINE_GL_FUNCTION( void, glProgramUniform4uiv, GLuint, GLint, GLsizei, const GLuint* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix2fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix3fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix4fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix2x3fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix3x2fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix2x4fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix4x2fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix3x4fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DEFINE_GL_FUNCTION( void, glProgramUniformMatrix4x3fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
// Buffer Objects ------------------------------------------------------
DEFINE_GL_FUNCTION( void, glCreateBuffers, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glVertexArrayVertexBuffer, GLuint, GLuint, GLuint, GLintptr, GLsizei );
DEFINE_GL_FUNCTION( void, glVertexArrayElementBuffer, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glDeleteBuffers, GLsizei, const GLuint* );
DEFINE_GL_FUNCTION( void, glDrawArrays, GLenum, GLint, GLsizei );
DEFINE_GL_FUNCTION( void, glDrawElements, GLenum, GLsizei, GLenum, const GLvoid* );
DEFINE_GL_FUNCTION( void, glNamedBufferData, GLuint, GLsizeiptr, const void*, GLenum );
DEFINE_GL_FUNCTION( void, glNamedBufferSubData, GLuint, GLintptr, GLsizei, const void* );
DEFINE_GL_FUNCTION( void*, glMapNamedBuffer, GLuint, GLenum );
DEFINE_GL_FUNCTION( void*, glMapNamedBufferRange, GLuint, GLintptr, GLsizei, GLbitfield );
DEFINE_GL_FUNCTION( GLboolean, glUnmapNamedBuffer, GLuint );
DEFINE_GL_FUNCTION( void, glNamedBufferStorage, GLuint, GLsizeiptr, const void*, GLbitfield );
DEFINE_GL_FUNCTION( void, glCreateVertexArrays, GLsizei, GLuint* );
DEFINE_GL_FUNCTION( void, glEnableVertexArrayAttrib, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glDisableVertexArrayAttrib, GLuint, GLuint );
DEFINE_GL_FUNCTION( void, glVertexArrayAttribFormat, GLuint, GLuint, GLint, GLenum, GLboolean, GLuint );
DEFINE_GL_FUNCTION( void, glVertexArrayAttribIFormat, GLuint, GLuint, GLint, GLenum, GLuint );
DEFINE_GL_FUNCTION( void, glVertexArrayAttribLFormat, GLuint, GLuint, GLint, GLenum, GLuint );
DEFINE_GL_FUNCTION( void, glVertexArrayAttribBinding, GLuint, GLuint, GLuint );
// State Management ----------------------------------------------------
DEFINE_GL_FUNCTION( void, glBlendColor, GLfloat, GLfloat, GLfloat, GLfloat );
DEFINE_GL_FUNCTION( void, glBlendEquation, GLenum );
DEFINE_GL_FUNCTION( void, glBlendEquationi, GLuint, GLenum );
DEFINE_GL_FUNCTION( void, glBlendEquationSeparate, GLenum, GLenum );
DEFINE_GL_FUNCTION( void, glBlendEquationSeparatei, GLuint, GLenum, GLenum );
DEFINE_GL_FUNCTION( void, glBlendFunc, GLenum, GLenum );
DEFINE_GL_FUNCTION( void, glBlendFunci, GLuint, GLenum, GLenum );
DEFINE_GL_FUNCTION( void, glBlendFuncSeparate, GLenum, GLenum, GLenum, GLenum );
DEFINE_GL_FUNCTION( void, glBlendFuncSeparatei, GLuint, GLenum, GLenum, GLenum, GLenum );
DEFINE_GL_FUNCTION( void, glCullFace, GLenum );
DEFINE_GL_FUNCTION( void, glEnable, GLenum );
DEFINE_GL_FUNCTION( void, glDisable, GLenum );
DEFINE_GL_FUNCTION( void, glEnablei, GLenum, GLuint );
DEFINE_GL_FUNCTION( void, glDisablei, GLenum, GLuint );
DEFINE_GL_FUNCTION( void, glGetBooleanv, GLenum, GLboolean* );
DEFINE_GL_FUNCTION( void, glGetDoublev, GLenum, GLdouble* );
DEFINE_GL_FUNCTION( void, glGetFloatv, GLenum, GLfloat* );
DEFINE_GL_FUNCTION( void, glGetIntegerv, GLenum, GLint* );
DEFINE_GL_FUNCTION( void, glGetInteger64v, GLenum, GLint64* );
DEFINE_GL_FUNCTION( void, glGetBooleani_v, GLenum, GLuint, GLboolean* );
DEFINE_GL_FUNCTION( void, glGetDoublei_v, GLenum, GLuint, GLdouble* );
DEFINE_GL_FUNCTION( void, glGetFloati_v, GLenum, GLuint, GLfloat* );
DEFINE_GL_FUNCTION( void, glGetIntegeri_v, GLenum, GLuint, GLint* );
DEFINE_GL_FUNCTION( void, glGetInteger64i_v, GLenum, GLuint, GLint64* );
DEFINE_GL_FUNCTION( void, glPixelStoref, GLenum, GLfloat );
DEFINE_GL_FUNCTION( void, glPixelStorei, GLenum, GLint );
DEFINE_GL_FUNCTION( void, glViewport, GLint, GLint, GLsizei, GLsizei );
DEFINE_GL_FUNCTION( void, glScissor, GLint, GLint, GLsizei, GLsizei );
// Transform Feedback --------------------------------------------------
// Utility -------------------------------------------------------------
DEFINE_GL_FUNCTION( const GLubyte*, glGetString, GLenum );
DEFINE_GL_FUNCTION( const GLubyte*, glGetStringi, GLenum, GLuint );
// Queries -------------------------------------------------------------
// Syncing -------------------------------------------------------------
// Vertex Array Objects ------------------------------------------------
DEFINE_GL_FUNCTION( void, glBindVertexArray, GLuint );
DEFINE_GL_FUNCTION( void, glDeleteVertexArrays, GLsizei, const GLuint* );
// Samplers ------------------------------------------------------------
// Program Pipelines ---------------------------------------------------
// Debug ---------------------------------------------------------------
DEFINE_GL_FUNCTION( void, glDebugMessageCallback, DEBUGPROC, void* );

#define IMPL_GL_FUNCTION( return_value, name, ... )\
    [[maybe_unused]]\
    SM_ALWAYS_INLINE return_value\
    name( __VA_ARGS__ )

} // namespace impl

// Textures ------------------------------------------------------------
IMPL_GL_FUNCTION( void, glCreateTextures, GLenum target, GLsizei n, GLuint* textures ) {
    return impl::in_glCreateTextures( target, n, textures );
}
IMPL_GL_FUNCTION( void, glBindTextureUnit, GLuint unit, GLuint texture ) {
    return impl::in_glBindTextureUnit( unit, texture );
}
IMPL_GL_FUNCTION( void, glDeleteTextures, GLsizei n, const GLuint* textures ) {
    return impl::in_glDeleteTextures( n, textures );
}
IMPL_GL_FUNCTION( void, glTextureParameterf, GLuint texture, GLenum pname, GLfloat param ) {
    return impl::in_glTextureParameterf( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameteri, GLuint texture, GLenum pname, GLint param ) {
    return impl::in_glTextureParameteri( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameterfv, GLuint texture, GLenum pname, const GLfloat* param ) {
    return impl::in_glTextureParameterfv( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameteriv, GLuint texture, GLenum pname, const GLint* param ) {
    return impl::in_glTextureParameteriv( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameterIiv, GLuint texture, GLenum pname, const GLint* params ) {
    return impl::in_glTextureParameterIiv( texture, pname, params );
}
IMPL_GL_FUNCTION( void, glTextureParameterIuiv, GLuint texture, GLenum pname, const GLuint* params ) {
    return impl::in_glTextureParameterIuiv( texture, pname, params );
}
IMPL_GL_FUNCTION( void, glTextureStorage2D, GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height ) {
    return impl::in_glTextureStorage2D(
        texture, levels,
        internalFormat,
        width, height
    );
}
IMPL_GL_FUNCTION( void, glTextureStorage3D, GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth ) {
    return impl::in_glTextureStorage3D(
        texture, levels,
        internalFormat,
        width, height, depth
    );
}
IMPL_GL_FUNCTION( void, glTextureSubImage2D, GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels ) {
    return impl::in_glTextureSubImage2D(
        texture, level,
        xoffset, yoffset,
        width, height,
        format, type,
        pixels
    );
}
IMPL_GL_FUNCTION( void, glTextureSubImage3D, GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels ) {
    return impl::in_glTextureSubImage3D(
        texture, level,
        xoffset, yoffset, zoffset,
        width, height, depth,
        format, type,
        pixels
    );
}
// Rendering -----------------------------------------------------------
IMPL_GL_FUNCTION( void, glClear, GLbitfield mask ) {
    return impl::in_glClear( mask );
}
IMPL_GL_FUNCTION( void, glClearColor,
    GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha
) {
    return impl::in_glClearColor( red, green, blue, alpha );
}
IMPL_GL_FUNCTION( void, glClearDepth, GLdouble depth ) {
    return impl::in_glClearDepth( depth );
}
IMPL_GL_FUNCTION( void, glClearDepthf, GLfloat depth ) {
    return impl::in_glClearDepthf( depth );
}
IMPL_GL_FUNCTION( void, glClearStencil, GLint s ) {
    return impl::in_glClearStencil( s );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferiv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value ) {
    return impl::in_glClearNamedFramebufferiv( framebuffer, buffer, drawbuffer, value );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferuiv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value ) {
    return impl::in_glClearNamedFramebufferuiv( framebuffer, buffer, drawbuffer, value );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferfv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value ) {
    return impl::in_glClearNamedFramebufferfv( framebuffer, buffer, drawbuffer, value );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferfi, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat depth, GLint stencil ) {
    return impl::in_glClearNamedFramebufferfi( framebuffer, buffer, drawbuffer, depth, stencil );
}
// Frame Buffers -------------------------------------------------------
IMPL_GL_FUNCTION( void, glCreateFramebuffers, GLsizei n, GLuint* ids ) {
    return impl::in_glCreateFramebuffers( n, ids );
}
IMPL_GL_FUNCTION( void, glCreateRenderbuffers, GLsizei n, GLuint* ids ) {
    return impl::in_glCreateRenderbuffers( n, ids );
}
IMPL_GL_FUNCTION( void, glDeleteFramebuffers, GLsizei n, GLuint* framebuffers ) {
    return impl::in_glDeleteFramebuffers( n, framebuffers );
}
IMPL_GL_FUNCTION( void, glDeleteRenderbuffers, GLsizei n, GLuint* renderbuffers ) {
    return impl::in_glDeleteRenderbuffers( n, renderbuffers );
}
IMPL_GL_FUNCTION( void, glBindFramebuffer, GLenum target, GLuint framebuffer ) {
    return impl::in_glBindFramebuffer( target, framebuffer );
}
IMPL_GL_FUNCTION( void, glBindRenderbuffer, GLenum target, GLuint renderbuffer ) {
    return impl::in_glBindRenderbuffer( target, renderbuffer );
}
IMPL_GL_FUNCTION( void, glGenerateTextureMipmap, GLuint texture ) {
    return impl::in_glGenerateTextureMipmap( texture );
}
// Shaders -------------------------------------------------------------
IMPL_GL_FUNCTION( GLuint, glCreateShaderProgramv, GLenum type, GLsizei count, const char** strings ) {
    return impl::in_glCreateShaderProgramv( type, count, strings );
}
IMPL_GL_FUNCTION( void, glCompileShader, GLuint shader ) {
    return impl::in_glCompileShader( shader );
}
IMPL_GL_FUNCTION( void, glGetShaderiv, GLuint shader, GLenum pname, GLint* params ) {
    return impl::in_glGetShaderiv( shader, pname, params );
}
IMPL_GL_FUNCTION( void, glGetProgramiv, GLuint program, GLenum pname, GLint* params ) {
    return impl::in_glGetProgramiv( program, pname, params );
}
IMPL_GL_FUNCTION( void, glDeleteShader, GLuint shader ) {
    return impl::in_glDeleteShader( shader );
}
IMPL_GL_FUNCTION( void, glDeleteProgram, GLuint program ) {
    return impl::in_glDeleteProgram( program );
}
IMPL_GL_FUNCTION( void, glAttachShader, GLuint program, GLuint shader ) {
    return impl::in_glAttachShader( program, shader );
}
IMPL_GL_FUNCTION( void, glDetachShader, GLuint program, GLuint shader ) {
    return impl::in_glDetachShader( program, shader );
}
IMPL_GL_FUNCTION( void, glGetShaderInfoLog, GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog ) {
    return impl::in_glGetShaderInfoLog( shader, maxLength, length, infoLog );
}
IMPL_GL_FUNCTION( void, glGetProgramInfoLog, GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog ) {
    return impl::in_glGetProgramInfoLog( program, maxLength, length, infoLog );
}
// Buffer Objects ------------------------------------------------------
IMPL_GL_FUNCTION( void, glCreateBuffers, GLsizei n, GLuint* buffers ) {
    return impl::in_glCreateBuffers( n, buffers );
}
IMPL_GL_FUNCTION( void, glDeleteBuffers, GLsizei n, const GLuint* buffers ) {
    return impl::in_glDeleteBuffers( n, buffers );
}
IMPL_GL_FUNCTION( void, glVertexArrayVertexBuffer, GLuint vao, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride ) {
    return impl::in_glVertexArrayVertexBuffer( vao, bindingIndex, buffer, offset, stride );
}
IMPL_GL_FUNCTION( void, glVertexArrayElementBuffer, GLuint vao, GLuint buffer ) {
    return impl::in_glVertexArrayElementBuffer( vao, buffer );
}
IMPL_GL_FUNCTION( void, glNamedBufferData, GLuint buffer, GLsizeiptr size, const void* data, GLenum usage ) {
    return impl::in_glNamedBufferData( buffer, size, data, usage );
}
IMPL_GL_FUNCTION( void, glNamedBufferSubData, GLuint buffer, GLintptr offset, GLsizei size, const void* data ) {
    return impl::in_glNamedBufferSubData( buffer, offset, size, data );
}
IMPL_GL_FUNCTION( void*, glMapNamedBuffer, GLuint buffer, GLenum access ) {
    return impl::in_glMapNamedBuffer( buffer, access );
}
IMPL_GL_FUNCTION( void*, glMapNamedBufferRange, GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access ) {
    return impl::in_glMapNamedBufferRange( buffer, offset, length, access );
}
IMPL_GL_FUNCTION( GLboolean, glUnmapNamedBuffer, GLuint buffer ) {
    return impl::in_glUnmapNamedBuffer( buffer );
}
IMPL_GL_FUNCTION( void, glNamedBufferStorage, GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags ) {
    return impl::in_glNamedBufferStorage( buffer, size, data, flags );
}
IMPL_GL_FUNCTION( void, glCreateVertexArrays, GLsizei n, GLuint* arrays ) {
    return impl::in_glCreateVertexArrays( n, arrays );
}
IMPL_GL_FUNCTION( void, glDrawArrays, GLenum mode, GLint first, GLsizei count ) {
    return impl::in_glDrawArrays( mode, first, count );
}
IMPL_GL_FUNCTION( void, glDrawElements, GLenum mode, GLsizei count, GLenum type, const GLvoid* indices ) {
    return impl::in_glDrawElements( mode, count, type, indices );
}
IMPL_GL_FUNCTION( void, glEnableVertexArrayAttrib, GLuint vao, GLuint index ) {
    return impl::in_glEnableVertexArrayAttrib( vao, index );
}
IMPL_GL_FUNCTION( void, glDisableVertexArrayAttrib, GLuint vao, GLuint index ) {
    return impl::in_glDisableVertexArrayAttrib( vao, index );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset ) {
    return impl::in_glVertexArrayAttribFormat( vao, attribIndex, size, type, normalized, relativeOffset );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribIFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset ) {
    return impl::in_glVertexArrayAttribIFormat( vao, attribIndex, size, type, relativeOffset );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribLFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset ) {
    return impl::in_glVertexArrayAttribLFormat( vao, attribIndex, size, type, relativeOffset );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribBinding, GLuint vao, GLuint attribIndex, GLuint bindingIndex ) {
    return impl::in_glVertexArrayAttribBinding( vao, attribIndex, bindingIndex );
}
IMPL_GL_FUNCTION( void, glGetActiveUniform, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum type, GLchar* name ) {
    return impl::in_glGetActiveUniform( program, index, bufSize, length, size, type, name );
}
IMPL_GL_FUNCTION( GLint, glGetUniformLocation, GLuint program, const GLchar* name ) {
    return impl::in_glGetUniformLocation( program, name );
}
IMPL_GL_FUNCTION( void, glProgramUniform1f, GLuint program, GLint location, GLfloat v0 ) {
    return impl::in_glProgramUniform1f( program, location, v0 );
}
IMPL_GL_FUNCTION( void, glProgramUniform2f, GLuint program, GLint location, GLfloat v0, GLfloat v1 ) {
    return impl::in_glProgramUniform2f( program, location, v0, v1 );
}
IMPL_GL_FUNCTION( void, glProgramUniform3f, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 ) {
    return impl::in_glProgramUniform3f( program, location, v0, v1, v2 );
}
IMPL_GL_FUNCTION( void, glProgramUniform4f, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 ) {
    return impl::in_glProgramUniform4f( program, location, v0, v1, v2, v3 );
}
IMPL_GL_FUNCTION( void, glProgramUniform1i, GLuint program, GLint location, GLint v0 ) {
    return impl::in_glProgramUniform1i( program, location, v0 );
}
IMPL_GL_FUNCTION( void, glProgramUniform2i, GLuint program, GLint location, GLint v0, GLint v1 ) {
    return impl::in_glProgramUniform2i( program, location, v0, v1 );
}
IMPL_GL_FUNCTION( void, glProgramUniform3i, GLuint program, GLint location, GLint v0, GLint v1, GLint v2 ) {
    return impl::in_glProgramUniform3i( program, location, v0, v1, v2 );
}
IMPL_GL_FUNCTION( void, glProgramUniform4i, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 ) {
    return impl::in_glProgramUniform4i( program, location, v0, v1, v2, v3 );
}
IMPL_GL_FUNCTION( void, glProgramUniform1ui, GLuint program, GLint location, GLuint v0 ) {
    return impl::in_glProgramUniform1ui( program, location, v0 );
}
IMPL_GL_FUNCTION( void, glProgramUniform2ui, GLuint program, GLint location, GLuint v0, GLuint v1 ) {
    return impl::in_glProgramUniform2ui( program, location, v0, v1 );
}
IMPL_GL_FUNCTION( void, glProgramUniform3ui, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2 ) {
    return impl::in_glProgramUniform3ui( program, location, v0, v1, v2 );
}
IMPL_GL_FUNCTION( void, glProgramUniform4ui, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 ) {
    return impl::in_glProgramUniform4ui( program, location, v0, v1, v2, v3 );
}
IMPL_GL_FUNCTION( void, glProgramUniform1fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    return impl::in_glProgramUniform1fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform2fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    return impl::in_glProgramUniform2fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform3fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    return impl::in_glProgramUniform3fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform4fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    return impl::in_glProgramUniform4fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform1iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    return impl::in_glProgramUniform1iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform2iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    return impl::in_glProgramUniform2iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform3iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    return impl::in_glProgramUniform3iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform4iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    return impl::in_glProgramUniform4iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform1uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    return impl::in_glProgramUniform1uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform2uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    return impl::in_glProgramUniform2uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform3uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    return impl::in_glProgramUniform3uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform4uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    return impl::in_glProgramUniform4uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix2x3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    return impl::in_glProgramUniformMatrix2x3fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix3x2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    return impl::in_glProgramUniformMatrix3x2fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix2x4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    return impl::in_glProgramUniformMatrix2x4fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix4x2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    return impl::in_glProgramUniformMatrix4x2fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix3x4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    return impl::in_glProgramUniformMatrix3x4fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix4x3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    return impl::in_glProgramUniformMatrix4x3fv( program, location, count, transpose, value );
}
// State Management ----------------------------------------------------
IMPL_GL_FUNCTION( void, glBlendColor, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
    return impl::in_glBlendColor( red, green, blue, alpha );
}
IMPL_GL_FUNCTION( void, glBlendEquation, GLenum mode ) {
    return impl::in_glBlendEquation( mode );
}
IMPL_GL_FUNCTION( void, glBlendEquationi, GLuint buf, GLenum mode ) {
    return impl::in_glBlendEquationi( buf, mode );
}
IMPL_GL_FUNCTION( void, glBlendEquationSeparate, GLenum modeRGB, GLenum modeAlpha ) {
    return impl::in_glBlendEquationSeparate( modeRGB, modeAlpha );
}
IMPL_GL_FUNCTION( void, glBlendEquationSeparatei, GLuint buf, GLenum modeRGB, GLenum modeAlpha ) {
    return impl::in_glBlendEquationSeparatei( buf, modeRGB, modeAlpha );
}
IMPL_GL_FUNCTION( void, glBlendFunc, GLenum sfactor, GLenum dfactor ) {
    return impl::in_glBlendFunc( sfactor, dfactor );
}
IMPL_GL_FUNCTION( void, glBlendFunci, GLuint buf, GLenum sfactor, GLenum dfactor ) {
    return impl::in_glBlendFunci( buf, sfactor, dfactor );
}
IMPL_GL_FUNCTION( void, glBlendFuncSeparate, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha ) {
    return impl::in_glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}
IMPL_GL_FUNCTION( void, glBlendFuncSeparatei, GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha ) {
    return impl::in_glBlendFuncSeparatei( buf, srcRGB, dstRGB, srcAlpha, dstAlpha );
}
IMPL_GL_FUNCTION( void, glCullFace, GLenum mode ) {
    return impl::in_glCullFace( mode );
}
IMPL_GL_FUNCTION( void, glEnable, GLenum cap ) {
    return impl::in_glEnable( cap );
}
IMPL_GL_FUNCTION( void, glDisable, GLenum cap ) {
    return impl::in_glDisable( cap );
}
IMPL_GL_FUNCTION( void, glEnablei, GLenum cap, GLuint index ) {
    return impl::in_glEnablei( cap, index );
}
IMPL_GL_FUNCTION( void, glDisablei, GLenum cap, GLuint index ) {
    return impl::in_glDisablei( cap, index );
}
IMPL_GL_FUNCTION( void, glGetBooleanv, GLenum pname, GLboolean* data ) {
    return impl::in_glGetBooleanv( pname, data );
}
IMPL_GL_FUNCTION( void, glGetDoublev, GLenum pname, GLdouble* data ) {
    return impl::in_glGetDoublev( pname, data );
}
IMPL_GL_FUNCTION( void, glGetFloatv, GLenum pname, GLfloat* data ) {
    return impl::in_glGetFloatv( pname, data );
}
IMPL_GL_FUNCTION( void, glGetIntegerv, GLenum pname, GLint* data ) {
    return impl::in_glGetIntegerv( pname, data );
}
IMPL_GL_FUNCTION( void, glGetInteger64v, GLenum pname, GLint64* data ) {
    return impl::in_glGetInteger64v( pname, data );
}
IMPL_GL_FUNCTION( void, glGetBooleani_v, GLenum target, GLuint index, GLboolean* data ) {
    return impl::in_glGetBooleani_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetDoublei_v, GLenum target, GLuint index, GLdouble* data ) {
    return impl::in_glGetDoublei_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetFloati_v, GLenum target, GLuint index, GLfloat* data ) {
    return impl::in_glGetFloati_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetIntegeri_v, GLenum target, GLuint index, GLint* data ) {
    return impl::in_glGetIntegeri_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetInteger64i_v, GLenum target, GLuint index, GLint64* data ) {
    return impl::in_glGetInteger64i_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glPixelStoref, GLenum pname, GLfloat param ) {
    return impl::in_glPixelStoref( pname, param );
}
IMPL_GL_FUNCTION( void, glPixelStorei, GLenum pname, GLint param ) {
    return impl::in_glPixelStorei( pname, param );
}
IMPL_GL_FUNCTION( void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height ) {
    return impl::in_glViewport( x, y, width, height );
}
IMPL_GL_FUNCTION( void, glScissor, GLint x, GLint y, GLsizei width, GLsizei height ) {
    return impl::in_glScissor( x, y, width, height );
}
// Transform Feedback --------------------------------------------------
// Utility -------------------------------------------------------------
IMPL_GL_FUNCTION( const GLubyte*, glGetString, GLenum name ) {
    return impl::in_glGetString( name );
}
IMPL_GL_FUNCTION( const GLubyte*, glGetStringi, GLenum name, GLuint index ) {
    return impl::in_glGetStringi( name, index );
}
// Queries -------------------------------------------------------------
// Syncing -------------------------------------------------------------
// Vertex Array Objects ------------------------------------------------
IMPL_GL_FUNCTION( void, glBindVertexArray, GLuint array ) {
    return impl::in_glBindVertexArray( array );
}
IMPL_GL_FUNCTION( void, glDeleteVertexArrays, GLsizei n, const GLuint* arrays ) {
    return impl::in_glDeleteVertexArrays( n, arrays );
}
// Samplers ------------------------------------------------------------
// Program Pipelines ---------------------------------------------------
// Debug ---------------------------------------------------------------
IMPL_GL_FUNCTION( void, glDebugMessageCallback,
    DEBUGPROC callback, void* userParam
) {
    return impl::in_glDebugMessageCallback( callback, userParam );
}

#endif // header guard