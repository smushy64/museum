#if !defined(LD_RENDERER_OPENGL_FUNCTIONS_HPP)
#define LD_RENDERER_OPENGL_FUNCTIONS_HPP
// * Description:  OpenGL Functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "ldrenderer/opengl/types.h"

#define DECLARE_GL_FUNCTION( ret, fn, ... )\
    typedef ret ___internal_##fn##FN( __VA_ARGS__ );\
    extern ___internal_##fn##FN* ___internal_##fn

// Textures ------------------------------------------------------------
DECLARE_GL_FUNCTION( void, glCreateTextures, GLenum target, GLsizei n, GLuint* textures );
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
DECLARE_GL_FUNCTION( void, glNamedFramebufferTexture, GLuint, GLenum, GLuint, GLint );
DECLARE_GL_FUNCTION( void, glNamedRenderbufferStorage, GLuint, GLenum, GLsizei, GLsizei );
DECLARE_GL_FUNCTION( void, glNamedFramebufferRenderbuffer, GLuint, GLenum, GLenum, GLuint );
DECLARE_GL_FUNCTION( GLenum, glCheckNamedFramebufferStatus, GLuint, GLenum );
DECLARE_GL_FUNCTION( void, glBlitNamedFramebuffer, GLuint, GLuint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum );
DECLARE_GL_FUNCTION( void, glNamedFramebufferDrawBuffer, GLuint, GLenum);
DECLARE_GL_FUNCTION( void, glNamedFramebufferDrawBuffers, GLuint, GLsizei, const GLenum*);
// Shaders -------------------------------------------------------------
DECLARE_GL_FUNCTION( GLuint, glCreateShader, GLenum );
DECLARE_GL_FUNCTION( GLuint, glCreateProgram, void );
DECLARE_GL_FUNCTION( GLuint, glCreateShaderProgramv, GLenum, GLsizei, const char** );
DECLARE_GL_FUNCTION( void, glShaderSource, GLuint, GLsizei, const GLchar**, const GLint* );
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
DECLARE_GL_FUNCTION( void, glPolygonMode, GLenum, GLenum );
DECLARE_GL_FUNCTION( void, glDepthFunc, GLenum );
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

#define IMPLEMENT_GL_FUNCTION( ret, name, ... )\
    static force_inline maybe_unused ret\
    name( __VA_ARGS__ )

// Textures ------------------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glCreateTextures, GLenum target, GLsizei n, GLuint* textures ) {
    ___internal_glCreateTextures( target, n, textures );
}
IMPLEMENT_GL_FUNCTION( void, glBindTextureUnit, GLuint unit, GLuint texture ) {
    ___internal_glBindTextureUnit( unit, texture );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteTextures, GLsizei n, const GLuint* textures ) {
    ___internal_glDeleteTextures( n, textures );
}
IMPLEMENT_GL_FUNCTION( void, glTextureParameterf, GLuint texture, GLenum pname, GLfloat param ) {
    ___internal_glTextureParameterf( texture, pname, param );
}
IMPLEMENT_GL_FUNCTION( void, glTextureParameteri, GLuint texture, GLenum pname, GLint param ) {
    ___internal_glTextureParameteri( texture, pname, param );
}
IMPLEMENT_GL_FUNCTION( void, glTextureParameterfv, GLuint texture, GLenum pname, const GLfloat* param ) {
    ___internal_glTextureParameterfv( texture, pname, param );
}
IMPLEMENT_GL_FUNCTION( void, glTextureParameteriv, GLuint texture, GLenum pname, const GLint* param ) {
    ___internal_glTextureParameteriv( texture, pname, param );
}
IMPLEMENT_GL_FUNCTION( void, glTextureParameterIiv, GLuint texture, GLenum pname, const GLint* params ) {
    ___internal_glTextureParameterIiv( texture, pname, params );
}
IMPLEMENT_GL_FUNCTION( void, glTextureParameterIuiv, GLuint texture, GLenum pname, const GLuint* params ) {
    ___internal_glTextureParameterIuiv( texture, pname, params );
}
IMPLEMENT_GL_FUNCTION( void, glTextureStorage2D, GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height ) {
    ___internal_glTextureStorage2D( texture, levels, internalFormat, width, height );
}
IMPLEMENT_GL_FUNCTION( void, glTextureStorage3D, GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth ) {
    ___internal_glTextureStorage3D( texture, levels, internalFormat, width, height, depth );
}
IMPLEMENT_GL_FUNCTION( void, glTextureSubImage2D, GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels ) {
    ___internal_glTextureSubImage2D( texture, level, xoffset, yoffset, width, height, format, type, pixels );
}
IMPLEMENT_GL_FUNCTION( void, glTextureSubImage3D, GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels ) {
    ___internal_glTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );
}
// Rendering -----------------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glClear, GLbitfield mask ) {
    ___internal_glClear( mask );
}
IMPLEMENT_GL_FUNCTION( void, glClearColor,
    GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha
) {
    ___internal_glClearColor( red, green, blue, alpha );
}
IMPLEMENT_GL_FUNCTION( void, glClearDepth, GLdouble depth ) {
    ___internal_glClearDepth( depth );
}
IMPLEMENT_GL_FUNCTION( void, glClearDepthf, GLfloat depth ) {
    ___internal_glClearDepthf( depth );
}
IMPLEMENT_GL_FUNCTION( void, glClearStencil, GLint s ) {
    ___internal_glClearStencil( s );
}
IMPLEMENT_GL_FUNCTION( void, glClearNamedFramebufferiv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value ) {
    ___internal_glClearNamedFramebufferiv( framebuffer, buffer, drawbuffer, value );
}
IMPLEMENT_GL_FUNCTION( void, glClearNamedFramebufferuiv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value ) {
    ___internal_glClearNamedFramebufferuiv( framebuffer, buffer, drawbuffer, value );
}
IMPLEMENT_GL_FUNCTION( void, glClearNamedFramebufferfv, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value ) {
    ___internal_glClearNamedFramebufferfv( framebuffer, buffer, drawbuffer, value );
}
IMPLEMENT_GL_FUNCTION( void, glClearNamedFramebufferfi, GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat depth, GLint stencil ) {
    ___internal_glClearNamedFramebufferfi( framebuffer, buffer, drawbuffer, depth, stencil );
}
// Frame Buffers -------------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glCreateFramebuffers, GLsizei n, GLuint* ids ) {
    ___internal_glCreateFramebuffers( n, ids );
}
IMPLEMENT_GL_FUNCTION( void, glCreateRenderbuffers, GLsizei n, GLuint* ids ) {
    ___internal_glCreateRenderbuffers( n, ids );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteFramebuffers, GLsizei n, GLuint* framebuffers ) {
    ___internal_glDeleteFramebuffers( n, framebuffers );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteRenderbuffers, GLsizei n, GLuint* renderbuffers ) {
    ___internal_glDeleteRenderbuffers( n, renderbuffers );
}
IMPLEMENT_GL_FUNCTION( void, glBindFramebuffer, GLenum target, GLuint framebuffer ) {
    ___internal_glBindFramebuffer( target, framebuffer );
}
IMPLEMENT_GL_FUNCTION( void, glBindRenderbuffer, GLenum target, GLuint renderbuffer ) {
    ___internal_glBindRenderbuffer( target, renderbuffer );
}
IMPLEMENT_GL_FUNCTION( void, glGenerateTextureMipmap, GLuint texture ) {
    ___internal_glGenerateTextureMipmap( texture );
}
IMPLEMENT_GL_FUNCTION( void, glNamedFramebufferTexture, GLuint framebuffer, GLenum attachment, GLuint texture, GLint level ) {
    ___internal_glNamedFramebufferTexture( framebuffer, attachment, texture, level );
}
IMPLEMENT_GL_FUNCTION( void, glNamedRenderbufferStorage, GLuint renderbuffer, GLenum internalFormat, GLsizei width, GLsizei height ) {
    ___internal_glNamedRenderbufferStorage( renderbuffer, internalFormat, width, height );
}
IMPLEMENT_GL_FUNCTION( void, glNamedFramebufferRenderbuffer, GLuint framebuffer, GLenum attachment, GLenum renderbufferTarget, GLuint renderbuffer ) {
    ___internal_glNamedFramebufferRenderbuffer( framebuffer, attachment, renderbufferTarget, renderbuffer );
}
IMPLEMENT_GL_FUNCTION( GLenum, glCheckNamedFramebufferStatus, GLuint framebuffer, GLenum target ) {
    return ___internal_glCheckNamedFramebufferStatus( framebuffer, target );
}
IMPLEMENT_GL_FUNCTION( void, glBlitNamedFramebuffer, GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter ) {
    ___internal_glBlitNamedFramebuffer( readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter );
}
IMPLEMENT_GL_FUNCTION( void, glNamedFramebufferDrawBuffer, GLuint framebuffer, GLenum buffer ) {
    ___internal_glNamedFramebufferDrawBuffer( framebuffer, buffer );
}
IMPLEMENT_GL_FUNCTION( void, glNamedFramebufferDrawBuffers, GLuint framebuffer, GLsizei n, const GLenum* buffers ) {
    ___internal_glNamedFramebufferDrawBuffers( framebuffer, n, buffers );
}
// Shaders -------------------------------------------------------------
IMPLEMENT_GL_FUNCTION( GLuint, glCreateShader, GLenum shaderType ) {
    return ___internal_glCreateShader( shaderType );
}
IMPLEMENT_GL_FUNCTION( GLuint, glCreateProgram, void ) {
    return ___internal_glCreateProgram();
}
IMPLEMENT_GL_FUNCTION( void, glShaderSource, GLuint shader, GLsizei count, const GLchar** string, const GLint* length ) {
    ___internal_glShaderSource( shader, count, string, length );
}
IMPLEMENT_GL_FUNCTION( GLuint, glCreateShaderProgramv, GLenum type, GLsizei count, const char** strings ) {
    return ___internal_glCreateShaderProgramv( type, count, strings );
}
IMPLEMENT_GL_FUNCTION( void, glUseProgram, GLuint program ) {
    ___internal_glUseProgram( program );
}
IMPLEMENT_GL_FUNCTION( void, glShaderBinary, GLsizei count, const GLuint* shaders, GLenum binaryFormat, const void* binary, GLsizei length ) {
    ___internal_glShaderBinary( count, shaders, binaryFormat, binary, length );
}
IMPLEMENT_GL_FUNCTION( void, glSpecializeShader, GLuint shader, const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue ) {
    ___internal_glSpecializeShader( shader, pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue );
}
IMPLEMENT_GL_FUNCTION( void, glCompileShader, GLuint shader ) {
    ___internal_glCompileShader( shader );
}
IMPLEMENT_GL_FUNCTION( void, glLinkProgram, GLuint program ) {
    ___internal_glLinkProgram( program );
}
IMPLEMENT_GL_FUNCTION( void, glGetShaderiv, GLuint shader, GLenum pname, GLint* params ) {
    ___internal_glGetShaderiv( shader, pname, params );
}
IMPLEMENT_GL_FUNCTION( void, glGetProgramiv, GLuint program, GLenum pname, GLint* params ) {
    ___internal_glGetProgramiv( program, pname, params );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteShader, GLuint shader ) {
    ___internal_glDeleteShader( shader );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteProgram, GLuint program ) {
    ___internal_glDeleteProgram( program );
}
IMPLEMENT_GL_FUNCTION( void, glAttachShader, GLuint program, GLuint shader ) {
    ___internal_glAttachShader( program, shader );
}
IMPLEMENT_GL_FUNCTION( void, glDetachShader, GLuint program, GLuint shader ) {
    ___internal_glDetachShader( program, shader );
}
IMPLEMENT_GL_FUNCTION( void, glGetShaderInfoLog, GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog ) {
    ___internal_glGetShaderInfoLog( shader, maxLength, length, infoLog );
}
IMPLEMENT_GL_FUNCTION( void, glGetProgramInfoLog, GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog ) {
    ___internal_glGetProgramInfoLog( program, maxLength, length, infoLog );
}
// Buffer Objects ------------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glCreateBuffers, GLsizei n, GLuint* buffers ) {
    ___internal_glCreateBuffers( n, buffers );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteBuffers, GLsizei n, const GLuint* buffers ) {
    ___internal_glDeleteBuffers( n, buffers );
}
IMPLEMENT_GL_FUNCTION( void, glVertexArrayVertexBuffer, GLuint vao, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLsizei stride ) {
    ___internal_glVertexArrayVertexBuffer( vao, bindingIndex, buffer, offset, stride );
}
IMPLEMENT_GL_FUNCTION( void, glVertexArrayElementBuffer, GLuint vao, GLuint buffer ) {
    ___internal_glVertexArrayElementBuffer( vao, buffer );
}
IMPLEMENT_GL_FUNCTION( void, glNamedBufferData, GLuint buffer, GLsizeiptr size, const void* data, GLenum usage ) {
    ___internal_glNamedBufferData( buffer, size, data, usage );
}
IMPLEMENT_GL_FUNCTION( void, glNamedBufferSubData, GLuint buffer, GLintptr offset, GLsizei size, const void* data ) {
    ___internal_glNamedBufferSubData( buffer, offset, size, data );
}
IMPLEMENT_GL_FUNCTION( void, glBindVertexBuffer, GLuint bindingIndex, GLuint buffer, GLintptr offset, GLintptr stride ) {
    ___internal_glBindVertexBuffer( bindingIndex, buffer, offset, stride );
}
IMPLEMENT_GL_FUNCTION( void, glBindBuffer, GLenum target, GLuint buffer ) {
    ___internal_glBindBuffer( target, buffer );
}
IMPLEMENT_GL_FUNCTION( void, glBindBufferBase, GLenum target, GLuint index, GLuint buffer ) {
    ___internal_glBindBufferBase( target, index, buffer );
}
IMPLEMENT_GL_FUNCTION( void*, glMapNamedBuffer, GLuint buffer, GLenum access ) {
    return ___internal_glMapNamedBuffer( buffer, access );
}
IMPLEMENT_GL_FUNCTION( void*, glMapNamedBufferRange, GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access ) {
    return ___internal_glMapNamedBufferRange( buffer, offset, length, access );
}
IMPLEMENT_GL_FUNCTION( GLboolean, glUnmapNamedBuffer, GLuint buffer ) {
    return ___internal_glUnmapNamedBuffer( buffer );
}
IMPLEMENT_GL_FUNCTION( void, glNamedBufferStorage, GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags ) {
    ___internal_glNamedBufferStorage( buffer, size, data, flags );
}
IMPLEMENT_GL_FUNCTION( void, glCreateVertexArrays, GLsizei n, GLuint* arrays ) {
    ___internal_glCreateVertexArrays( n, arrays );
}
IMPLEMENT_GL_FUNCTION( void, glDrawArrays, GLenum mode, GLint first, GLsizei count ) {
    ___internal_glDrawArrays( mode, first, count );
}
IMPLEMENT_GL_FUNCTION( void, glDrawElements, GLenum mode, GLsizei count, GLenum type, const GLvoid* indices ) {
    ___internal_glDrawElements( mode, count, type, indices );
}
IMPLEMENT_GL_FUNCTION( void, glEnableVertexArrayAttrib, GLuint vao, GLuint index ) {
    ___internal_glEnableVertexArrayAttrib( vao, index );
}
IMPLEMENT_GL_FUNCTION( void, glDisableVertexArrayAttrib, GLuint vao, GLuint index ) {
    ___internal_glDisableVertexArrayAttrib( vao, index );
}
IMPLEMENT_GL_FUNCTION( void, glVertexArrayAttribFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset ) {
    ___internal_glVertexArrayAttribFormat( vao, attribIndex, size, type, normalized, relativeOffset );
}
IMPLEMENT_GL_FUNCTION( void, glVertexArrayAttribIFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset ) {
    ___internal_glVertexArrayAttribIFormat( vao, attribIndex, size, type, relativeOffset );
}
IMPLEMENT_GL_FUNCTION( void, glVertexArrayAttribLFormat, GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLuint relativeOffset ) {
    ___internal_glVertexArrayAttribLFormat( vao, attribIndex, size, type, relativeOffset );
}
IMPLEMENT_GL_FUNCTION( void, glVertexArrayAttribBinding, GLuint vao, GLuint attribIndex, GLuint bindingIndex ) {
    ___internal_glVertexArrayAttribBinding( vao, attribIndex, bindingIndex );
}
IMPLEMENT_GL_FUNCTION( void, glGetActiveUniform, GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name ) {
    ___internal_glGetActiveUniform( program, index, bufSize, length, size, type, name );
}
IMPLEMENT_GL_FUNCTION( GLint, glGetUniformLocation, GLuint program, const GLchar* name ) {
    return ___internal_glGetUniformLocation( program, name );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform1f, GLuint program, GLint location, GLfloat v0 ) {
    ___internal_glProgramUniform1f( program, location, v0 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform2f, GLuint program, GLint location, GLfloat v0, GLfloat v1 ) {
    ___internal_glProgramUniform2f( program, location, v0, v1 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform3f, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 ) {
    ___internal_glProgramUniform3f( program, location, v0, v1, v2 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform4f, GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 ) {
    ___internal_glProgramUniform4f( program, location, v0, v1, v2, v3 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform1i, GLuint program, GLint location, GLint v0 ) {
    ___internal_glProgramUniform1i( program, location, v0 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform2i, GLuint program, GLint location, GLint v0, GLint v1 ) {
    ___internal_glProgramUniform2i( program, location, v0, v1 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform3i, GLuint program, GLint location, GLint v0, GLint v1, GLint v2 ) {
    ___internal_glProgramUniform3i( program, location, v0, v1, v2 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform4i, GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 ) {
    ___internal_glProgramUniform4i( program, location, v0, v1, v2, v3 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform1ui, GLuint program, GLint location, GLuint v0 ) {
    ___internal_glProgramUniform1ui( program, location, v0 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform2ui, GLuint program, GLint location, GLuint v0, GLuint v1 ) {
    ___internal_glProgramUniform2ui( program, location, v0, v1 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform3ui, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2 ) {
    ___internal_glProgramUniform3ui( program, location, v0, v1, v2 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform4ui, GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 ) {
    ___internal_glProgramUniform4ui( program, location, v0, v1, v2, v3 );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform1fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    ___internal_glProgramUniform1fv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform2fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    ___internal_glProgramUniform2fv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform3fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    ___internal_glProgramUniform3fv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform4fv, GLuint program, GLint location, GLsizei count, const GLfloat* value ) {
    ___internal_glProgramUniform4fv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform1iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    ___internal_glProgramUniform1iv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform2iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    ___internal_glProgramUniform2iv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform3iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    ___internal_glProgramUniform3iv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform4iv, GLuint program, GLint location, GLsizei count, const GLint* value ) {
    ___internal_glProgramUniform4iv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform1uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    ___internal_glProgramUniform1uiv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform2uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    ___internal_glProgramUniform2uiv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform3uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    ___internal_glProgramUniform3uiv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniform4uiv, GLuint program, GLint location, GLsizei count, const GLuint* value ) {
    ___internal_glProgramUniform4uiv( program, location, count, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix2fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix3fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix4fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix2x3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix2x3fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix3x2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix3x2fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix2x4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix2x4fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix4x2fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix4x2fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix3x4fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix3x4fv( program, location, count, transpose, value );
}
IMPLEMENT_GL_FUNCTION( void, glProgramUniformMatrix4x3fv, GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ) {
    ___internal_glProgramUniformMatrix4x3fv( program, location, count, transpose, value );
}
// State Management ----------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glBlendColor, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha ) {
    ___internal_glBlendColor( red, green, blue, alpha );
}
IMPLEMENT_GL_FUNCTION( void, glBlendEquation, GLenum mode ) {
    ___internal_glBlendEquation( mode );
}
IMPLEMENT_GL_FUNCTION( void, glBlendEquationi, GLuint buf, GLenum mode ) {
    ___internal_glBlendEquationi( buf, mode );
}
IMPLEMENT_GL_FUNCTION( void, glBlendEquationSeparate, GLenum modeRGB, GLenum modeAlpha ) {
    ___internal_glBlendEquationSeparate( modeRGB, modeAlpha );
}
IMPLEMENT_GL_FUNCTION( void, glBlendEquationSeparatei, GLuint buf, GLenum modeRGB, GLenum modeAlpha ) {
    ___internal_glBlendEquationSeparatei( buf, modeRGB, modeAlpha );
}
IMPLEMENT_GL_FUNCTION( void, glBlendFunc, GLenum sfactor, GLenum dfactor ) {
    ___internal_glBlendFunc( sfactor, dfactor );
}
IMPLEMENT_GL_FUNCTION( void, glBlendFunci, GLuint buf, GLenum sfactor, GLenum dfactor ) {
    ___internal_glBlendFunci( buf, sfactor, dfactor );
}
IMPLEMENT_GL_FUNCTION( void, glBlendFuncSeparate, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha ) {
    ___internal_glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}
IMPLEMENT_GL_FUNCTION( void, glBlendFuncSeparatei, GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha ) {
    ___internal_glBlendFuncSeparatei( buf, srcRGB, dstRGB, srcAlpha, dstAlpha );
}
IMPLEMENT_GL_FUNCTION( void, glCullFace, GLenum mode ) {
    ___internal_glCullFace( mode );
}
IMPLEMENT_GL_FUNCTION( void, glEnable, GLenum cap ) {
    ___internal_glEnable( cap );
}
IMPLEMENT_GL_FUNCTION( void, glDisable, GLenum cap ) {
    ___internal_glDisable( cap );
}
IMPLEMENT_GL_FUNCTION( void, glEnablei, GLenum cap, GLuint index ) {
    ___internal_glEnablei( cap, index );
}
IMPLEMENT_GL_FUNCTION( void, glDisablei, GLenum cap, GLuint index ) {
    ___internal_glDisablei( cap, index );
}
IMPLEMENT_GL_FUNCTION( void, glGetBooleanv, GLenum pname, GLboolean* data ) {
    ___internal_glGetBooleanv( pname, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetDoublev, GLenum pname, GLdouble* data ) {
    ___internal_glGetDoublev( pname, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetFloatv, GLenum pname, GLfloat* data ) {
    ___internal_glGetFloatv( pname, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetIntegerv, GLenum pname, GLint* data ) {
    ___internal_glGetIntegerv( pname, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetInteger64v, GLenum pname, GLint64* data ) {
    ___internal_glGetInteger64v( pname, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetBooleani_v, GLenum target, GLuint index, GLboolean* data ) {
    ___internal_glGetBooleani_v( target, index, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetDoublei_v, GLenum target, GLuint index, GLdouble* data ) {
    ___internal_glGetDoublei_v( target, index, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetFloati_v, GLenum target, GLuint index, GLfloat* data ) {
    ___internal_glGetFloati_v( target, index, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetIntegeri_v, GLenum target, GLuint index, GLint* data ) {
    ___internal_glGetIntegeri_v( target, index, data );
}
IMPLEMENT_GL_FUNCTION( void, glGetInteger64i_v, GLenum target, GLuint index, GLint64* data ) {
    ___internal_glGetInteger64i_v( target, index, data );
}
IMPLEMENT_GL_FUNCTION( void, glPixelStoref, GLenum pname, GLfloat param ) {
    ___internal_glPixelStoref( pname, param );
}
IMPLEMENT_GL_FUNCTION( void, glPixelStorei, GLenum pname, GLint param ) {
    ___internal_glPixelStorei( pname, param );
}
IMPLEMENT_GL_FUNCTION( void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height ) {
    ___internal_glViewport( x, y, width, height );
}
IMPLEMENT_GL_FUNCTION( void, glScissor, GLint x, GLint y, GLsizei width, GLsizei height ) {
    ___internal_glScissor( x, y, width, height );
}
IMPLEMENT_GL_FUNCTION( void, glPolygonMode, GLenum face, GLenum mode ) {
    ___internal_glPolygonMode( face, mode );
}
IMPLEMENT_GL_FUNCTION( void, glDepthFunc, GLenum func ) {
    ___internal_glDepthFunc( func );
}
// Transform Feedback --------------------------------------------------
// Utility -------------------------------------------------------------
IMPLEMENT_GL_FUNCTION( const GLubyte*, glGetString, GLenum name ) {
    return ___internal_glGetString( name );
}
IMPLEMENT_GL_FUNCTION( const GLubyte*, glGetStringi, GLenum name, GLuint index ) {
    return ___internal_glGetStringi( name, index );
}
// Queries -------------------------------------------------------------
// Syncing -------------------------------------------------------------
// Vertex Array Objects ------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glBindVertexArray, GLuint array ) {
    ___internal_glBindVertexArray( array );
}
IMPLEMENT_GL_FUNCTION( void, glDeleteVertexArrays, GLsizei n, const GLuint* arrays ) {
    ___internal_glDeleteVertexArrays( n, arrays );
}
// Samplers ------------------------------------------------------------
// Program Pipelines ---------------------------------------------------
// Debug ---------------------------------------------------------------
IMPLEMENT_GL_FUNCTION( void, glDebugMessageCallback,
    DEBUGPROC callback, void* userParam
) {
    ___internal_glDebugMessageCallback( callback, userParam );
}

#endif // header guard
