/**
 * Description:  OpenGL function loader implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 13, 2023
*/
#include "gl_types.h"
#include "gl_functions.h"
#include "gl_loader.h"
#include "gl_backend.h"

#define LOAD_PROC_REQUIRED( name ) do {\
    void* proc = getproc( TO_STRING(name) );\
    if( !proc ) {\
        GL_LOG_FATAL( "Failed to load required function \"" TO_STRING(name) "\"!" );\
        return false;\
    }\
    ::impl::in_##name = (::impl:: name##FN )proc;\
} while(0)

#define LOAD_PROC( name ) do {\
    void* proc = getproc( TO_STRING(name) );\
    if( !proc ) {\
        GL_LOG_WARN( "Failed to load function \"" TO_STRING(name) "\"!" );\
    }\
    ::impl::in_##name = (::impl:: name##FN )proc;\
} while(0)

b32 gl_load( GetProcFN getproc ) {
    LOAD_PROC_REQUIRED( glCreateTextures );
    LOAD_PROC_REQUIRED( glBindTextureUnit );
    LOAD_PROC_REQUIRED( glDeleteTextures );
    LOAD_PROC_REQUIRED( glTextureParameterf );
    LOAD_PROC_REQUIRED( glTextureParameteri );
    LOAD_PROC_REQUIRED( glTextureParameterfv );
    LOAD_PROC_REQUIRED( glTextureParameteriv );
    LOAD_PROC_REQUIRED( glTextureParameterIiv );
    LOAD_PROC_REQUIRED( glTextureParameterIuiv );
    LOAD_PROC_REQUIRED( glTextureStorage2D );
    LOAD_PROC_REQUIRED( glTextureStorage3D );
    LOAD_PROC_REQUIRED( glTextureSubImage2D );
    LOAD_PROC_REQUIRED( glTextureSubImage3D );

    LOAD_PROC_REQUIRED( glClear );
    LOAD_PROC_REQUIRED( glClearColor );
    LOAD_PROC_REQUIRED( glClearDepth );
    LOAD_PROC_REQUIRED( glClearDepthf );
    LOAD_PROC_REQUIRED( glClearStencil );
    LOAD_PROC_REQUIRED( glClearNamedFramebufferiv );
    LOAD_PROC_REQUIRED( glClearNamedFramebufferuiv );
    LOAD_PROC_REQUIRED( glClearNamedFramebufferfv );
    LOAD_PROC_REQUIRED( glClearNamedFramebufferfi );

    LOAD_PROC_REQUIRED( glCreateFramebuffers );
    LOAD_PROC_REQUIRED( glCreateRenderbuffers );
    LOAD_PROC_REQUIRED( glDeleteFramebuffers );
    LOAD_PROC_REQUIRED( glDeleteRenderbuffers );
    LOAD_PROC_REQUIRED( glBindFramebuffer );
    LOAD_PROC_REQUIRED( glBindRenderbuffer );
    LOAD_PROC_REQUIRED( glGenerateTextureMipmap );

    LOAD_PROC_REQUIRED( glCreateShader );
    LOAD_PROC_REQUIRED( glCreateProgram );
    LOAD_PROC_REQUIRED( glCreateShaderProgramv );
    LOAD_PROC_REQUIRED( glUseProgram );
    LOAD_PROC_REQUIRED( glShaderBinary );
    LOAD_PROC_REQUIRED( glSpecializeShader );
    LOAD_PROC_REQUIRED( glCompileShader );
    LOAD_PROC_REQUIRED( glLinkProgram );
    LOAD_PROC_REQUIRED( glGetShaderiv );
    LOAD_PROC_REQUIRED( glGetProgramiv );
    LOAD_PROC_REQUIRED( glDeleteShader );
    LOAD_PROC_REQUIRED( glDeleteProgram );
    LOAD_PROC_REQUIRED( glAttachShader );
    LOAD_PROC_REQUIRED( glDetachShader );
    LOAD_PROC_REQUIRED( glGetShaderInfoLog );
    LOAD_PROC_REQUIRED( glGetProgramInfoLog );
    LOAD_PROC_REQUIRED( glGetActiveUniform );
    LOAD_PROC_REQUIRED( glGetUniformLocation );
    LOAD_PROC_REQUIRED( glProgramUniform1f );
    LOAD_PROC_REQUIRED( glProgramUniform2f );
    LOAD_PROC_REQUIRED( glProgramUniform3f );
    LOAD_PROC_REQUIRED( glProgramUniform4f );
    LOAD_PROC_REQUIRED( glProgramUniform1i );
    LOAD_PROC_REQUIRED( glProgramUniform2i );
    LOAD_PROC_REQUIRED( glProgramUniform3i );
    LOAD_PROC_REQUIRED( glProgramUniform4i );
    LOAD_PROC_REQUIRED( glProgramUniform1ui );
    LOAD_PROC_REQUIRED( glProgramUniform2ui );
    LOAD_PROC_REQUIRED( glProgramUniform3ui );
    LOAD_PROC_REQUIRED( glProgramUniform4ui );
    LOAD_PROC_REQUIRED( glProgramUniform1fv );
    LOAD_PROC_REQUIRED( glProgramUniform2fv );
    LOAD_PROC_REQUIRED( glProgramUniform3fv );
    LOAD_PROC_REQUIRED( glProgramUniform4fv );
    LOAD_PROC_REQUIRED( glProgramUniform1iv );
    LOAD_PROC_REQUIRED( glProgramUniform2iv );
    LOAD_PROC_REQUIRED( glProgramUniform3iv );
    LOAD_PROC_REQUIRED( glProgramUniform4iv );
    LOAD_PROC_REQUIRED( glProgramUniform1uiv );
    LOAD_PROC_REQUIRED( glProgramUniform2uiv );
    LOAD_PROC_REQUIRED( glProgramUniform3uiv );
    LOAD_PROC_REQUIRED( glProgramUniform4uiv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix2fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix3fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix4fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix2x3fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix3x2fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix2x4fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix4x2fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix3x4fv );
    LOAD_PROC_REQUIRED( glProgramUniformMatrix4x3fv );

    LOAD_PROC_REQUIRED( glCreateBuffers );
    LOAD_PROC_REQUIRED( glDeleteBuffers );
    LOAD_PROC_REQUIRED( glVertexArrayVertexBuffer );
    LOAD_PROC_REQUIRED( glVertexArrayElementBuffer );
    LOAD_PROC_REQUIRED( glNamedBufferData );
    LOAD_PROC_REQUIRED( glNamedBufferSubData );
    LOAD_PROC_REQUIRED( glBindVertexBuffer );
    LOAD_PROC_REQUIRED( glBindBuffer );
    LOAD_PROC_REQUIRED( glBindBufferBase );
    LOAD_PROC_REQUIRED( glMapNamedBuffer );
    LOAD_PROC_REQUIRED( glMapNamedBufferRange );
    LOAD_PROC_REQUIRED( glUnmapNamedBuffer );
    LOAD_PROC_REQUIRED( glNamedBufferStorage );
    LOAD_PROC_REQUIRED( glCreateVertexArrays );
    LOAD_PROC_REQUIRED( glDrawArrays );
    LOAD_PROC_REQUIRED( glDrawElements );
    LOAD_PROC_REQUIRED( glEnableVertexArrayAttrib );
    LOAD_PROC_REQUIRED( glDisableVertexArrayAttrib );
    LOAD_PROC_REQUIRED( glVertexArrayAttribFormat );
    LOAD_PROC_REQUIRED( glVertexArrayAttribIFormat );
    LOAD_PROC_REQUIRED( glVertexArrayAttribLFormat );
    LOAD_PROC_REQUIRED( glVertexArrayAttribBinding );

    LOAD_PROC_REQUIRED( glBlendColor );
    LOAD_PROC_REQUIRED( glBlendEquation );
    LOAD_PROC_REQUIRED( glBlendEquationi );
    LOAD_PROC_REQUIRED( glBlendEquationSeparate );
    LOAD_PROC_REQUIRED( glBlendEquationSeparatei );
    LOAD_PROC_REQUIRED( glBlendFunc );
    LOAD_PROC_REQUIRED( glBlendFunci );
    LOAD_PROC_REQUIRED( glBlendFuncSeparate );
    LOAD_PROC_REQUIRED( glBlendFuncSeparatei );
    LOAD_PROC_REQUIRED( glCullFace );
    LOAD_PROC_REQUIRED( glEnable );
    LOAD_PROC_REQUIRED( glDisable );
    LOAD_PROC_REQUIRED( glEnablei );
    LOAD_PROC_REQUIRED( glDisablei );
    LOAD_PROC_REQUIRED( glGetBooleanv );
    LOAD_PROC_REQUIRED( glGetDoublev );
    LOAD_PROC_REQUIRED( glGetFloatv );
    LOAD_PROC_REQUIRED( glGetIntegerv );
    LOAD_PROC_REQUIRED( glGetInteger64v );
    LOAD_PROC_REQUIRED( glGetBooleani_v );
    LOAD_PROC_REQUIRED( glGetDoublei_v );
    LOAD_PROC_REQUIRED( glGetFloati_v );
    LOAD_PROC_REQUIRED( glGetIntegeri_v );
    LOAD_PROC_REQUIRED( glGetInteger64i_v );
    LOAD_PROC_REQUIRED( glPixelStoref );
    LOAD_PROC_REQUIRED( glPixelStorei );
    LOAD_PROC_REQUIRED( glViewport );
    LOAD_PROC_REQUIRED( glScissor );

    LOAD_PROC_REQUIRED( glGetString );
    LOAD_PROC_REQUIRED( glGetStringi );

    LOAD_PROC_REQUIRED( glBindVertexArray );
    LOAD_PROC_REQUIRED( glDeleteVertexArrays );

    LOAD_PROC_REQUIRED( glDebugMessageCallback );
    return true;
}
