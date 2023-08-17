#if !defined(RENDERER_OPENGL_GL_FUNCTIONS_HPP)
#define RENDERER_OPENGL_GL_FUNCTIONS_HPP
/**
 * Description:  OpenGL Function Declarations
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "gl_types.h"

#define DECLARE_GL_FUNCTION( return_value, name, ... )\
    typedef return_value (*name##FN)( __VA_ARGS__ );\
    extern name##FN in_##name

// Textures ------------------------------------------------------------
DECLARE_GL_FUNCTION( void, glCreateTextures, GLenum, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glBindTextureUnit, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glDeleteTextures, GLsizei, const GLuint* );
DECLARE_GL_FUNCTION( void, glTextureParameterf, GLuint, GLenum, GLfloat );
DECLARE_GL_FUNCTION( void, glTextureParameteri, GLuint, GLenum, GLint );
DECLARE_GL_FUNCTION( void, glTextureParameterfv, GLuint, GLenum, const GLfloat* );
DECLARE_GL_FUNCTION( void, glTextureParameteriv, GLuint, GLenum, const GLint* );
DECLARE_GL_FUNCTION( void, glTextureParameterIiv, GLuint, GLenum, const GLint* );
DECLARE_GL_FUNCTION( void, glTextureParameterIuiv, GLuint, GLenum, const GLuint* );
DECLARE_GL_FUNCTION( void, glTextureStorage2D, GLuint, GLsizei, GLenum, GLsizei, GLsizei );
DECLARE_GL_FUNCTION( void, glTextureStorage3D, GLuint, GLsizei, GLenum, GLsizei, GLsizei, GLsizei );
DECLARE_GL_FUNCTION( void, glTextureSubImage2D, GLuint, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void* );
DECLARE_GL_FUNCTION( void, glTextureSubImage3D, GLuint, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void* );
// Rendering -----------------------------------------------------------
DECLARE_GL_FUNCTION( void, glClear, GLbitfield );
DECLARE_GL_FUNCTION( void, glClearColor, GLfloat, GLfloat, GLfloat, GLfloat );
DECLARE_GL_FUNCTION( void, glClearDepth, GLdouble );
DECLARE_GL_FUNCTION( void, glClearDepthf, GLfloat );
DECLARE_GL_FUNCTION( void, glClearStencil, GLint );
DECLARE_GL_FUNCTION( void, glClearNamedFramebufferiv, GLuint, GLenum, GLint, const GLint* );
DECLARE_GL_FUNCTION( void, glClearNamedFramebufferuiv, GLuint, GLenum, GLint, const GLuint* );
DECLARE_GL_FUNCTION( void, glClearNamedFramebufferfv, GLuint, GLenum, GLint, const GLfloat* );
DECLARE_GL_FUNCTION( void, glClearNamedFramebufferfi, GLuint, GLenum, GLint, const GLfloat, GLint );
// Frame Buffers -------------------------------------------------------
DECLARE_GL_FUNCTION( void, glCreateFramebuffers, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glCreateRenderbuffers, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glDeleteFramebuffers, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glDeleteRenderbuffers, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glBindFramebuffer, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glBindRenderbuffer, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glGenerateTextureMipmap, GLuint );
// Shaders -------------------------------------------------------------
DECLARE_GL_FUNCTION( GLuint, glCreateShader, GLenum );
DECLARE_GL_FUNCTION( GLuint, glCreateProgram );
DECLARE_GL_FUNCTION( GLuint, glCreateShaderProgramv, GLenum, GLsizei, const char** );
DECLARE_GL_FUNCTION( void, glUseProgram, GLuint );
DECLARE_GL_FUNCTION( void, glShaderBinary, GLsizei, const GLuint*, GLenum, const void*, GLsizei );
DECLARE_GL_FUNCTION( void, glSpecializeShader, GLuint, const GLchar*, GLuint, const GLuint*, const GLuint* );
DECLARE_GL_FUNCTION( void, glCompileShader, GLuint );
DECLARE_GL_FUNCTION( void, glLinkProgram, GLuint );
DECLARE_GL_FUNCTION( void, glGetShaderiv, GLuint, GLenum, GLint* );
DECLARE_GL_FUNCTION( void, glGetProgramiv, GLuint, GLenum, GLint* );
DECLARE_GL_FUNCTION( void, glDeleteShader, GLuint );
DECLARE_GL_FUNCTION( void, glDeleteProgram, GLuint );
DECLARE_GL_FUNCTION( void, glAttachShader, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glDetachShader, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei*, GLchar* );
DECLARE_GL_FUNCTION( void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei*, GLchar* );
DECLARE_GL_FUNCTION( void, glGetActiveUniform, GLuint, GLuint, GLsizei, GLsizei*, GLint*, GLenum*, GLchar* );
DECLARE_GL_FUNCTION( GLint, glGetUniformLocation, GLuint, const GLchar* );
DECLARE_GL_FUNCTION( void, glProgramUniform1f, GLuint, GLint, GLfloat );
DECLARE_GL_FUNCTION( void, glProgramUniform2f, GLuint, GLint, GLfloat, GLfloat );
DECLARE_GL_FUNCTION( void, glProgramUniform3f, GLuint, GLint, GLfloat, GLfloat, GLfloat );
DECLARE_GL_FUNCTION( void, glProgramUniform4f, GLuint, GLint, GLfloat, GLfloat, GLfloat, GLfloat );
DECLARE_GL_FUNCTION( void, glProgramUniform1i, GLuint, GLint, GLint );
DECLARE_GL_FUNCTION( void, glProgramUniform2i, GLuint, GLint, GLint, GLint );
DECLARE_GL_FUNCTION( void, glProgramUniform3i, GLuint, GLint, GLint, GLint, GLint );
DECLARE_GL_FUNCTION( void, glProgramUniform4i, GLuint, GLint, GLint, GLint, GLint, GLint );
DECLARE_GL_FUNCTION( void, glProgramUniform1ui, GLuint, GLint, GLuint );
DECLARE_GL_FUNCTION( void, glProgramUniform2ui, GLuint, GLint, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glProgramUniform3ui, GLuint, GLint, GLuint, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glProgramUniform4ui, GLuint, GLint, GLuint, GLuint, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glProgramUniform1fv, GLuint, GLint, GLsizei, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniform2fv, GLuint, GLint, GLsizei, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniform3fv, GLuint, GLint, GLsizei, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniform4fv, GLuint, GLint, GLsizei, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniform1iv, GLuint, GLint, GLsizei, const GLint* );
DECLARE_GL_FUNCTION( void, glProgramUniform2iv, GLuint, GLint, GLsizei, const GLint* );
DECLARE_GL_FUNCTION( void, glProgramUniform3iv, GLuint, GLint, GLsizei, const GLint* );
DECLARE_GL_FUNCTION( void, glProgramUniform4iv, GLuint, GLint, GLsizei, const GLint* );
DECLARE_GL_FUNCTION( void, glProgramUniform1uiv, GLuint, GLint, GLsizei, const GLuint* );
DECLARE_GL_FUNCTION( void, glProgramUniform2uiv, GLuint, GLint, GLsizei, const GLuint* );
DECLARE_GL_FUNCTION( void, glProgramUniform3uiv, GLuint, GLint, GLsizei, const GLuint* );
DECLARE_GL_FUNCTION( void, glProgramUniform4uiv, GLuint, GLint, GLsizei, const GLuint* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix2fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix3fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix4fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix2x3fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix3x2fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix2x4fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix4x2fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix3x4fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
DECLARE_GL_FUNCTION( void, glProgramUniformMatrix4x3fv, GLuint, GLint, GLsizei, GLboolean, const GLfloat* );
// Buffer Objects ------------------------------------------------------
DECLARE_GL_FUNCTION( void, glCreateBuffers, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glVertexArrayVertexBuffer, GLuint, GLuint, GLuint, GLintptr, GLsizei );
DECLARE_GL_FUNCTION( void, glVertexArrayElementBuffer, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glDeleteBuffers, GLsizei, const GLuint* );
DECLARE_GL_FUNCTION( void, glDrawArrays, GLenum, GLint, GLsizei );
DECLARE_GL_FUNCTION( void, glDrawElements, GLenum, GLsizei, GLenum, const GLvoid* );
DECLARE_GL_FUNCTION( void, glNamedBufferData, GLuint, GLsizeiptr, const void*, GLenum );
DECLARE_GL_FUNCTION( void, glNamedBufferSubData, GLuint, GLintptr, GLsizei, const void* );
DECLARE_GL_FUNCTION( void, glBindVertexBuffer, GLuint, GLuint, GLintptr, GLintptr );
DECLARE_GL_FUNCTION( void, glBindBuffer, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glBindBufferBase, GLenum, GLuint, GLuint );
DECLARE_GL_FUNCTION( void*, glMapNamedBuffer, GLuint, GLenum );
DECLARE_GL_FUNCTION( void*, glMapNamedBufferRange, GLuint, GLintptr, GLsizei, GLbitfield );
DECLARE_GL_FUNCTION( GLboolean, glUnmapNamedBuffer, GLuint );
DECLARE_GL_FUNCTION( void, glNamedBufferStorage, GLuint, GLsizeiptr, const void*, GLbitfield );
DECLARE_GL_FUNCTION( void, glCreateVertexArrays, GLsizei, GLuint* );
DECLARE_GL_FUNCTION( void, glEnableVertexArrayAttrib, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glDisableVertexArrayAttrib, GLuint, GLuint );
DECLARE_GL_FUNCTION( void, glVertexArrayAttribFormat, GLuint, GLuint, GLint, GLenum, GLboolean, GLuint );
DECLARE_GL_FUNCTION( void, glVertexArrayAttribIFormat, GLuint, GLuint, GLint, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glVertexArrayAttribLFormat, GLuint, GLuint, GLint, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glVertexArrayAttribBinding, GLuint, GLuint, GLuint );
// State Management ----------------------------------------------------
DECLARE_GL_FUNCTION( void, glBlendColor, GLfloat, GLfloat, GLfloat, GLfloat );
DECLARE_GL_FUNCTION( void, glBlendEquation, GLenum );
DECLARE_GL_FUNCTION( void, glBlendEquationi, GLuint, GLenum );
DECLARE_GL_FUNCTION( void, glBlendEquationSeparate, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glBlendEquationSeparatei, GLuint, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glBlendFunc, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glBlendFunci, GLuint, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glBlendFuncSeparate, GLenum, GLenum, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glBlendFuncSeparatei, GLuint, GLenum, GLenum, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glCullFace, GLenum );
DECLARE_GL_FUNCTION( void, glEnable, GLenum );
DECLARE_GL_FUNCTION( void, glDisable, GLenum );
DECLARE_GL_FUNCTION( void, glEnablei, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glDisablei, GLenum, GLuint );
DECLARE_GL_FUNCTION( void, glGetBooleanv, GLenum, GLboolean* );
DECLARE_GL_FUNCTION( void, glGetDoublev, GLenum, GLdouble* );
DECLARE_GL_FUNCTION( void, glGetFloatv, GLenum, GLfloat* );
DECLARE_GL_FUNCTION( void, glGetIntegerv, GLenum, GLint* );
DECLARE_GL_FUNCTION( void, glGetInteger64v, GLenum, GLint64* );
DECLARE_GL_FUNCTION( void, glGetBooleani_v, GLenum, GLuint, GLboolean* );
DECLARE_GL_FUNCTION( void, glGetDoublei_v, GLenum, GLuint, GLdouble* );
DECLARE_GL_FUNCTION( void, glGetFloati_v, GLenum, GLuint, GLfloat* );
DECLARE_GL_FUNCTION( void, glGetIntegeri_v, GLenum, GLuint, GLint* );
DECLARE_GL_FUNCTION( void, glGetInteger64i_v, GLenum, GLuint, GLint64* );
DECLARE_GL_FUNCTION( void, glPixelStoref, GLenum, GLfloat );
DECLARE_GL_FUNCTION( void, glPixelStorei, GLenum, GLint );
DECLARE_GL_FUNCTION( void, glViewport, GLint, GLint, GLsizei, GLsizei );
DECLARE_GL_FUNCTION( void, glScissor, GLint, GLint, GLsizei, GLsizei );
// Transform Feedback --------------------------------------------------
// Utility -------------------------------------------------------------
DECLARE_GL_FUNCTION( const GLubyte*, glGetString, GLenum );
DECLARE_GL_FUNCTION( const GLubyte*, glGetStringi, GLenum, GLuint );
// Queries -------------------------------------------------------------
// Syncing -------------------------------------------------------------
// Vertex Array Objects ------------------------------------------------
DECLARE_GL_FUNCTION( void, glBindVertexArray, GLuint );
DECLARE_GL_FUNCTION( void, glDeleteVertexArrays, GLsizei, const GLuint* );
// Samplers ------------------------------------------------------------
// Program Pipelines ---------------------------------------------------
// Debug ---------------------------------------------------------------
DECLARE_GL_FUNCTION( void, glDebugMessageCallback, DEBUGPROC, void* );

#define IMPL_GL_FUNCTION( return_value, name, ... )\
    FORCE_INLINE return_value\
    name( __VA_ARGS__ )

// Textures ------------------------------------------------------------
IMPL_GL_FUNCTION( void, glCreateTextures, GLenum target, GLsizei n, GLuint* textures ) {
    in_glCreateTextures( target, n, textures );
}
IMPL_GL_FUNCTION( void, glBindTextureUnit, GLuint unit, GLuint texture ) {
    in_glBindTextureUnit( unit, texture );
}
IMPL_GL_FUNCTION( void, glDeleteTextures, GLsizei n, const GLuint* textures ) {
    in_glDeleteTextures( n, textures );
}
IMPL_GL_FUNCTION( void, glTextureParameterf, GLuint texture, GLenum pname, GLfloat param ) {
    in_glTextureParameterf( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameteri, GLuint texture, GLenum pname, GLint param ) {
    in_glTextureParameteri( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameterfv, GLuint texture, GLenum pname, const GLfloat* param ) {
    in_glTextureParameterfv( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameteriv, GLuint texture, GLenum pname, const GLint* param ) {
    in_glTextureParameteriv( texture, pname, param );
}
IMPL_GL_FUNCTION( void, glTextureParameterIiv, GLuint texture, GLenum pname, const GLint* params ) {
    in_glTextureParameterIiv( texture, pname, params );
}
IMPL_GL_FUNCTION( void, glTextureParameterIuiv, GLuint texture, GLenum pname, const GLuint* params ) {
    in_glTextureParameterIuiv( texture, pname, params );
}
IMPL_GL_FUNCTION( void, glTextureStorage2D, GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height ) {
    in_glTextureStorage2D(
        texture, levels,
        internalFormat,
        width, height
    );
}
IMPL_GL_FUNCTION( void, glTextureStorage3D, GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth ) {
    in_glTextureStorage3D(
        texture, levels,
        internalFormat,
        width, height, depth
    );
}
IMPL_GL_FUNCTION( void, glTextureSubImage2D, GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels ) {
    in_glTextureSubImage2D(
        texture, level,
        xoffset, yoffset,
        width, height,
        format, type,
        pixels
    );
}
IMPL_GL_FUNCTION( void, glTextureSubImage3D, GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels ) {
    in_glTextureSubImage3D(
        texture, level,
        xoffset, yoffset, zoffset,
        width, height, depth,
        format, type,
        pixels
    );
}
// Rendering -----------------------------------------------------------
IMPL_GL_FUNCTION( void, glClear, GLbitfield mask ) {
    in_glClear( mask );
}
IMPL_GL_FUNCTION( void, glClearColor,
    GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha
) {
    in_glClearColor( red, green, blue, alpha );
}
IMPL_GL_FUNCTION( void, glClearDepth, GLdouble depth ) {
    in_glClearDepth( depth );
}
IMPL_GL_FUNCTION( void, glClearDepthf, GLfloat depth ) {
    in_glClearDepthf( depth );
}
IMPL_GL_FUNCTION( void, glClearStencil, GLint s ) {
    in_glClearStencil( s );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferiv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value ) {
    in_glClearNamedFramebufferiv( framebuffer, buffer, drawbuffer, value );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferuiv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value ) {
    in_glClearNamedFramebufferuiv( framebuffer, buffer, drawbuffer, value );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferfv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value ) {
    in_glClearNamedFramebufferfv( framebuffer, buffer, drawbuffer, value );
}
IMPL_GL_FUNCTION( void, glClearNamedFramebufferfi, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat depth, GLint stencil ) {
    in_glClearNamedFramebufferfi( framebuffer, buffer, drawbuffer, depth, stencil );
}
// Frame Buffers -------------------------------------------------------
IMPL_GL_FUNCTION( void, glCreateFramebuffers, GLsizei n, GLuint* ids ) {
    in_glCreateFramebuffers( n, ids );
}
IMPL_GL_FUNCTION( void, glCreateRenderbuffers, GLsizei n, GLuint* ids ) {
    in_glCreateRenderbuffers( n, ids );
}
IMPL_GL_FUNCTION( void, glDeleteFramebuffers, GLsizei n, GLuint* framebuffers ) {
    in_glDeleteFramebuffers( n, framebuffers );
}
IMPL_GL_FUNCTION( void, glDeleteRenderbuffers, GLsizei n, GLuint* renderbuffers ) {
    in_glDeleteRenderbuffers( n, renderbuffers );
}
IMPL_GL_FUNCTION( void, glBindFramebuffer, GLenum target, GLuint framebuffer ) {
    in_glBindFramebuffer( target, framebuffer );
}
IMPL_GL_FUNCTION( void, glBindRenderbuffer, GLenum target, GLuint renderbuffer ) {
    in_glBindRenderbuffer( target, renderbuffer );
}
IMPL_GL_FUNCTION( void, glGenerateTextureMipmap, GLuint texture ) {
    in_glGenerateTextureMipmap( texture );
}
// Shaders -------------------------------------------------------------
IMPL_GL_FUNCTION( GLuint, glCreateShader, GLenum shaderType ) {
    return in_glCreateShader( shaderType );
}
IMPL_GL_FUNCTION( GLuint, glCreateProgram ) {
    return in_glCreateProgram();
}
IMPL_GL_FUNCTION( GLuint, glCreateShaderProgramv, GLenum type, GLsizei count, const char** strings ) {
    return in_glCreateShaderProgramv( type, count, strings );
}
IMPL_GL_FUNCTION( void, glUseProgram, GLuint program ) {
    in_glUseProgram( program );
}
IMPL_GL_FUNCTION( void, glShaderBinary, GLsizei count, const GLuint* shaders, GLenum binaryFormat, const void* binary, GLsizei length ) {
    in_glShaderBinary( count, shaders, binaryFormat, binary, length );
}
IMPL_GL_FUNCTION( void, glSpecializeShader, GLuint shader, const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue ) {
    in_glSpecializeShader( shader, pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue );
}
IMPL_GL_FUNCTION( void, glCompileShader, GLuint shader ) {
    in_glCompileShader( shader );
}
IMPL_GL_FUNCTION( void, glLinkProgram, GLuint program ) {
    in_glLinkProgram( program );
}
IMPL_GL_FUNCTION( void, glGetShaderiv, GLuint shader, GLenum pname, GLint* params ) {
    in_glGetShaderiv( shader, pname, params );
}
IMPL_GL_FUNCTION( void, glGetProgramiv, GLuint program, GLenum pname, GLint* params ) {
    in_glGetProgramiv( program, pname, params );
}
IMPL_GL_FUNCTION( void, glDeleteShader, GLuint shader ) {
    in_glDeleteShader( shader );
}
IMPL_GL_FUNCTION( void, glDeleteProgram, GLuint program ) {
    in_glDeleteProgram( program );
}
IMPL_GL_FUNCTION( void, glAttachShader, GLuint program, GLuint shader ) {
    in_glAttachShader( program, shader );
}
IMPL_GL_FUNCTION( void, glDetachShader, GLuint program, GLuint shader ) {
    in_glDetachShader( program, shader );
}
IMPL_GL_FUNCTION( void, glGetShaderInfoLog, GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog ) {
    in_glGetShaderInfoLog( shader, maxLength, length, infoLog );
}
IMPL_GL_FUNCTION( void, glGetProgramInfoLog, GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog ) {
    in_glGetProgramInfoLog( program, maxLength, length, infoLog );
}
// Buffer Objects ------------------------------------------------------
IMPL_GL_FUNCTION( void, glCreateBuffers, GLsizei n, GLuint* buffers ) {
    in_glCreateBuffers( n, buffers );
}
IMPL_GL_FUNCTION( void, glDeleteBuffers, GLsizei n, const GLuint* buffers ) {
    in_glDeleteBuffers( n, buffers );
}
IMPL_GL_FUNCTION( void, glVertexArrayVertexBuffer, GLuint vao, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride ) {
    in_glVertexArrayVertexBuffer( vao, bindingIndex, buffer, offset, stride );
}
IMPL_GL_FUNCTION( void, glVertexArrayElementBuffer, GLuint vao, GLuint buffer ) {
    in_glVertexArrayElementBuffer( vao, buffer );
}
IMPL_GL_FUNCTION( void, glNamedBufferData, GLuint buffer, GLsizeiptr size, const void* data, GLenum usage ) {
    in_glNamedBufferData( buffer, size, data, usage );
}
IMPL_GL_FUNCTION( void, glNamedBufferSubData, GLuint buffer, GLintptr offset, GLsizei size, const void* data ) {
    in_glNamedBufferSubData( buffer, offset, size, data );
}
IMPL_GL_FUNCTION( void, glBindVertexBuffer, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLintptr stride ) {
    in_glBindVertexBuffer( bindingIndex, buffer, offset, stride );
}
IMPL_GL_FUNCTION( void, glBindBuffer, GLenum target, GLuint buffer ) {
    in_glBindBuffer( target, buffer );
}
IMPL_GL_FUNCTION( void, glBindBufferBase, GLenum target, GLuint index, GLuint buffer ) {
    in_glBindBufferBase( target, index, buffer );
}
IMPL_GL_FUNCTION( void*, glMapNamedBuffer, GLuint buffer, GLenum access ) {
    return in_glMapNamedBuffer( buffer, access );
}
IMPL_GL_FUNCTION( void*, glMapNamedBufferRange, GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access ) {
    return in_glMapNamedBufferRange( buffer, offset, length, access );
}
IMPL_GL_FUNCTION( GLboolean, glUnmapNamedBuffer, GLuint buffer ) {
    return in_glUnmapNamedBuffer( buffer );
}
IMPL_GL_FUNCTION( void, glNamedBufferStorage, GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags ) {
    in_glNamedBufferStorage( buffer, size, data, flags );
}
IMPL_GL_FUNCTION( void, glCreateVertexArrays, GLsizei n, GLuint* arrays ) {
    in_glCreateVertexArrays( n, arrays );
}
IMPL_GL_FUNCTION( void, glDrawArrays, GLenum mode, GLint first, GLsizei count ) {
    in_glDrawArrays( mode, first, count );
}
IMPL_GL_FUNCTION( void, glDrawElements, GLenum mode, GLsizei count, GLenum type, const GLvoid* indices ) {
    in_glDrawElements( mode, count, type, indices );
}
IMPL_GL_FUNCTION( void, glEnableVertexArrayAttrib, GLuint vao, GLuint index ) {
    in_glEnableVertexArrayAttrib( vao, index );
}
IMPL_GL_FUNCTION( void, glDisableVertexArrayAttrib, GLuint vao, GLuint index ) {
    in_glDisableVertexArrayAttrib( vao, index );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset ) {
    in_glVertexArrayAttribFormat( vao, attribIndex, size, type, normalized, relativeOffset );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribIFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset ) {
    in_glVertexArrayAttribIFormat( vao, attribIndex, size, type, relativeOffset );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribLFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset ) {
    in_glVertexArrayAttribLFormat( vao, attribIndex, size, type, relativeOffset );
}
IMPL_GL_FUNCTION( void, glVertexArrayAttribBinding, GLuint vao, GLuint attribIndex, GLuint bindingIndex ) {
    in_glVertexArrayAttribBinding( vao, attribIndex, bindingIndex );
}
IMPL_GL_FUNCTION( void, glGetActiveUniform, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name ) {
    in_glGetActiveUniform( program, index, bufSize, length, size, type, name );
}
IMPL_GL_FUNCTION( GLint, glGetUniformLocation, GLuint program, const GLchar* name ) {
    return in_glGetUniformLocation( program, name );
}
IMPL_GL_FUNCTION( void, glProgramUniform1f, GLuint program, GLint location, GLfloat v0 ) {
    in_glProgramUniform1f( program, location, v0 );
}
IMPL_GL_FUNCTION( void, glProgramUniform2f, GLuint program, GLint location, GLfloat v0, GLfloat v1 ) {
    in_glProgramUniform2f( program, location, v0, v1 );
}
IMPL_GL_FUNCTION( void, glProgramUniform3f, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 ) {
    in_glProgramUniform3f( program, location, v0, v1, v2 );
}
IMPL_GL_FUNCTION( void, glProgramUniform4f, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 ) {
    in_glProgramUniform4f( program, location, v0, v1, v2, v3 );
}
IMPL_GL_FUNCTION( void, glProgramUniform1i, GLuint program, GLint location, GLint v0 ) {
    in_glProgramUniform1i( program, location, v0 );
}
IMPL_GL_FUNCTION( void, glProgramUniform2i, GLuint program, GLint location, GLint v0, GLint v1 ) {
    in_glProgramUniform2i( program, location, v0, v1 );
}
IMPL_GL_FUNCTION( void, glProgramUniform3i, GLuint program, GLint location, GLint v0, GLint v1, GLint v2 ) {
    in_glProgramUniform3i( program, location, v0, v1, v2 );
}
IMPL_GL_FUNCTION( void, glProgramUniform4i, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 ) {
    in_glProgramUniform4i( program, location, v0, v1, v2, v3 );
}
IMPL_GL_FUNCTION( void, glProgramUniform1ui, GLuint program, GLint location, GLuint v0 ) {
    in_glProgramUniform1ui( program, location, v0 );
}
IMPL_GL_FUNCTION( void, glProgramUniform2ui, GLuint program, GLint location, GLuint v0, GLuint v1 ) {
    in_glProgramUniform2ui( program, location, v0, v1 );
}
IMPL_GL_FUNCTION( void, glProgramUniform3ui, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2 ) {
    in_glProgramUniform3ui( program, location, v0, v1, v2 );
}
IMPL_GL_FUNCTION( void, glProgramUniform4ui, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 ) {
    in_glProgramUniform4ui( program, location, v0, v1, v2, v3 );
}
IMPL_GL_FUNCTION( void, glProgramUniform1fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    in_glProgramUniform1fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform2fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    in_glProgramUniform2fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform3fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    in_glProgramUniform3fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform4fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    in_glProgramUniform4fv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform1iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    in_glProgramUniform1iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform2iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    in_glProgramUniform2iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform3iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    in_glProgramUniform3iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform4iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    in_glProgramUniform4iv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform1uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    in_glProgramUniform1uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform2uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    in_glProgramUniform2uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform3uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    in_glProgramUniform3uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniform4uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    in_glProgramUniform4uiv( program, location, count, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix2fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix3fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix4fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix2x3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix2x3fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix3x2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix3x2fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix2x4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix2x4fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix4x2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix4x2fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix3x4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix3x4fv( program, location, count, transpose, value );
}
IMPL_GL_FUNCTION( void, glProgramUniformMatrix4x3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    in_glProgramUniformMatrix4x3fv( program, location, count, transpose, value );
}
// State Management ----------------------------------------------------
IMPL_GL_FUNCTION( void, glBlendColor, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
    in_glBlendColor( red, green, blue, alpha );
}
IMPL_GL_FUNCTION( void, glBlendEquation, GLenum mode ) {
    in_glBlendEquation( mode );
}
IMPL_GL_FUNCTION( void, glBlendEquationi, GLuint buf, GLenum mode ) {
    in_glBlendEquationi( buf, mode );
}
IMPL_GL_FUNCTION( void, glBlendEquationSeparate, GLenum modeRGB, GLenum modeAlpha ) {
    in_glBlendEquationSeparate( modeRGB, modeAlpha );
}
IMPL_GL_FUNCTION( void, glBlendEquationSeparatei, GLuint buf, GLenum modeRGB, GLenum modeAlpha ) {
    in_glBlendEquationSeparatei( buf, modeRGB, modeAlpha );
}
IMPL_GL_FUNCTION( void, glBlendFunc, GLenum sfactor, GLenum dfactor ) {
    in_glBlendFunc( sfactor, dfactor );
}
IMPL_GL_FUNCTION( void, glBlendFunci, GLuint buf, GLenum sfactor, GLenum dfactor ) {
    in_glBlendFunci( buf, sfactor, dfactor );
}
IMPL_GL_FUNCTION( void, glBlendFuncSeparate, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha ) {
    in_glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}
IMPL_GL_FUNCTION( void, glBlendFuncSeparatei, GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha ) {
    in_glBlendFuncSeparatei( buf, srcRGB, dstRGB, srcAlpha, dstAlpha );
}
IMPL_GL_FUNCTION( void, glCullFace, GLenum mode ) {
    in_glCullFace( mode );
}
IMPL_GL_FUNCTION( void, glEnable, GLenum cap ) {
    in_glEnable( cap );
}
IMPL_GL_FUNCTION( void, glDisable, GLenum cap ) {
    in_glDisable( cap );
}
IMPL_GL_FUNCTION( void, glEnablei, GLenum cap, GLuint index ) {
    in_glEnablei( cap, index );
}
IMPL_GL_FUNCTION( void, glDisablei, GLenum cap, GLuint index ) {
    in_glDisablei( cap, index );
}
IMPL_GL_FUNCTION( void, glGetBooleanv, GLenum pname, GLboolean* data ) {
    in_glGetBooleanv( pname, data );
}
IMPL_GL_FUNCTION( void, glGetDoublev, GLenum pname, GLdouble* data ) {
    in_glGetDoublev( pname, data );
}
IMPL_GL_FUNCTION( void, glGetFloatv, GLenum pname, GLfloat* data ) {
    in_glGetFloatv( pname, data );
}
IMPL_GL_FUNCTION( void, glGetIntegerv, GLenum pname, GLint* data ) {
    in_glGetIntegerv( pname, data );
}
IMPL_GL_FUNCTION( void, glGetInteger64v, GLenum pname, GLint64* data ) {
    in_glGetInteger64v( pname, data );
}
IMPL_GL_FUNCTION( void, glGetBooleani_v, GLenum target, GLuint index, GLboolean* data ) {
    in_glGetBooleani_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetDoublei_v, GLenum target, GLuint index, GLdouble* data ) {
    in_glGetDoublei_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetFloati_v, GLenum target, GLuint index, GLfloat* data ) {
    in_glGetFloati_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetIntegeri_v, GLenum target, GLuint index, GLint* data ) {
    in_glGetIntegeri_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glGetInteger64i_v, GLenum target, GLuint index, GLint64* data ) {
    in_glGetInteger64i_v( target, index, data );
}
IMPL_GL_FUNCTION( void, glPixelStoref, GLenum pname, GLfloat param ) {
    in_glPixelStoref( pname, param );
}
IMPL_GL_FUNCTION( void, glPixelStorei, GLenum pname, GLint param ) {
    in_glPixelStorei( pname, param );
}
IMPL_GL_FUNCTION( void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height ) {
    in_glViewport( x, y, width, height );
}
IMPL_GL_FUNCTION( void, glScissor, GLint x, GLint y, GLsizei width, GLsizei height ) {
    in_glScissor( x, y, width, height );
}
// Transform Feedback --------------------------------------------------
// Utility -------------------------------------------------------------
IMPL_GL_FUNCTION( const GLubyte*, glGetString, GLenum name ) {
    return in_glGetString( name );
}
IMPL_GL_FUNCTION( const GLubyte*, glGetStringi, GLenum name, GLuint index ) {
    return in_glGetStringi( name, index );
}
// Queries -------------------------------------------------------------
// Syncing -------------------------------------------------------------
// Vertex Array Objects ------------------------------------------------
IMPL_GL_FUNCTION( void, glBindVertexArray, GLuint array ) {
    in_glBindVertexArray( array );
}
IMPL_GL_FUNCTION( void, glDeleteVertexArrays, GLsizei n, const GLuint* arrays ) {
    in_glDeleteVertexArrays( n, arrays );
}
// Samplers ------------------------------------------------------------
// Program Pipelines ---------------------------------------------------
// Debug ---------------------------------------------------------------
IMPL_GL_FUNCTION( void, glDebugMessageCallback,
    DEBUGPROC callback, void* userParam
) {
    in_glDebugMessageCallback( callback, userParam );
}

#endif // header guard
