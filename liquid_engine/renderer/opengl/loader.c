// * Description:  OpenGL Function Loader Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "renderer/opengl.h"
#include "renderer/opengl/types.h"
#include "renderer/opengl/functions.h"
#include "renderer/opengl/loader.h"

#define LOAD_PROC_REQUIRED( fn ) do {\
void* proc = get_proc_address( macro_name_to_string(fn) );\
if( !proc ) {\
    GL_LOG_FATAL( "Failed to load required function \"" macro_name_to_string(fn) "\"!" );\
    return false;\
}\
___internal_##fn = ( ___internal_##fn##FN* )proc;\
} while(0)

#define LOAD_PROC( fn ) do {\
void* proc = get_proc_address( TO_STRING(fn) );\
if( !proc ) {\
    GL_LOG_WARN( "Failed to load function \"" TO_STRING(fn) "\"!" );\
}\
___internal_##fn = ( ___internal_##fn##FN* )proc;\
} while(0)

#define DEFINE_GL_FUNCTION( fn )\
___internal_##fn##FN* ___internal_##fn

// Textures ------------------------------------------------------------
DEFINE_GL_FUNCTION(glCreateTextures       ); 
DEFINE_GL_FUNCTION(glBindTextureUnit      ); 
DEFINE_GL_FUNCTION(glDeleteTextures       ); 
DEFINE_GL_FUNCTION(glTextureParameterf    ); 
DEFINE_GL_FUNCTION(glTextureParameteri    ); 
DEFINE_GL_FUNCTION(glTextureParameterfv   ); 
DEFINE_GL_FUNCTION(glTextureParameteriv   ); 
DEFINE_GL_FUNCTION(glTextureParameterIiv  ); 
DEFINE_GL_FUNCTION(glTextureParameterIuiv );
DEFINE_GL_FUNCTION(glTextureStorage2D     ); 
DEFINE_GL_FUNCTION(glTextureStorage3D     ); 
DEFINE_GL_FUNCTION(glTextureSubImage2D    ); 
DEFINE_GL_FUNCTION(glTextureSubImage3D    ); 
// Rendering -----------------------------------------------------------
DEFINE_GL_FUNCTION(glClear                   ); 
DEFINE_GL_FUNCTION(glClearColor              ); 
DEFINE_GL_FUNCTION(glClearDepth              ); 
DEFINE_GL_FUNCTION(glClearDepthf             ); 
DEFINE_GL_FUNCTION(glClearStencil            ); 
DEFINE_GL_FUNCTION(glClearNamedFramebufferiv ); 
DEFINE_GL_FUNCTION(glClearNamedFramebufferuiv); 
DEFINE_GL_FUNCTION(glClearNamedFramebufferfv ); 
DEFINE_GL_FUNCTION(glClearNamedFramebufferfi ); 
// Frame Buffers -------------------------------------------------------
DEFINE_GL_FUNCTION(glCreateFramebuffers   ); 
DEFINE_GL_FUNCTION(glCreateRenderbuffers  ); 
DEFINE_GL_FUNCTION(glDeleteFramebuffers   ); 
DEFINE_GL_FUNCTION(glDeleteRenderbuffers  ); 
DEFINE_GL_FUNCTION(glBindFramebuffer      ); 
DEFINE_GL_FUNCTION(glBindRenderbuffer     ); 
DEFINE_GL_FUNCTION(glGenerateTextureMipmap); 
DEFINE_GL_FUNCTION(glNamedFramebufferTexture );
DEFINE_GL_FUNCTION(glNamedRenderbufferStorage );
DEFINE_GL_FUNCTION(glNamedFramebufferRenderbuffer);
DEFINE_GL_FUNCTION(glCheckNamedFramebufferStatus);
DEFINE_GL_FUNCTION(glBlitNamedFramebuffer);
DEFINE_GL_FUNCTION(glNamedFramebufferDrawBuffer);
DEFINE_GL_FUNCTION(glNamedFramebufferDrawBuffers);
DEFINE_GL_FUNCTION(glNamedFramebufferTextureLayer);
// Shaders -------------------------------------------------------------
DEFINE_GL_FUNCTION(glCreateShader  );
DEFINE_GL_FUNCTION(glCreateProgram );
DEFINE_GL_FUNCTION(glShaderSource  );
DEFINE_GL_FUNCTION(glCreateShaderProgramv);
DEFINE_GL_FUNCTION(glUseProgram        ); 
DEFINE_GL_FUNCTION(glShaderBinary      ); 
DEFINE_GL_FUNCTION(glSpecializeShader  ); 
DEFINE_GL_FUNCTION(glCompileShader     ); 
DEFINE_GL_FUNCTION(glLinkProgram       ); 
DEFINE_GL_FUNCTION(glGetShaderiv       ); 
DEFINE_GL_FUNCTION(glGetProgramiv      ); 
DEFINE_GL_FUNCTION(glDeleteShader      ); 
DEFINE_GL_FUNCTION(glDeleteProgram     ); 
DEFINE_GL_FUNCTION(glAttachShader      ); 
DEFINE_GL_FUNCTION(glDetachShader      ); 
DEFINE_GL_FUNCTION(glGetShaderInfoLog  ); 
DEFINE_GL_FUNCTION(glGetProgramInfoLog ); 
DEFINE_GL_FUNCTION(glGetActiveUniform  ); 
DEFINE_GL_FUNCTION(glGetUniformLocation); 
DEFINE_GL_FUNCTION(glProgramUniform1f         ); 
DEFINE_GL_FUNCTION(glProgramUniform2f         ); 
DEFINE_GL_FUNCTION(glProgramUniform3f         ); 
DEFINE_GL_FUNCTION(glProgramUniform4f         ); 
DEFINE_GL_FUNCTION(glProgramUniform1i         ); 
DEFINE_GL_FUNCTION(glProgramUniform2i         ); 
DEFINE_GL_FUNCTION(glProgramUniform3i         ); 
DEFINE_GL_FUNCTION(glProgramUniform4i         ); 
DEFINE_GL_FUNCTION(glProgramUniform1ui        ); 
DEFINE_GL_FUNCTION(glProgramUniform2ui        ); 
DEFINE_GL_FUNCTION(glProgramUniform3ui        ); 
DEFINE_GL_FUNCTION(glProgramUniform4ui        ); 
DEFINE_GL_FUNCTION(glProgramUniform1fv        ); 
DEFINE_GL_FUNCTION(glProgramUniform2fv        ); 
DEFINE_GL_FUNCTION(glProgramUniform3fv        ); 
DEFINE_GL_FUNCTION(glProgramUniform4fv        ); 
DEFINE_GL_FUNCTION(glProgramUniform1iv        ); 
DEFINE_GL_FUNCTION(glProgramUniform2iv        ); 
DEFINE_GL_FUNCTION(glProgramUniform3iv        ); 
DEFINE_GL_FUNCTION(glProgramUniform4iv        ); 
DEFINE_GL_FUNCTION(glProgramUniform1uiv       ); 
DEFINE_GL_FUNCTION(glProgramUniform2uiv       ); 
DEFINE_GL_FUNCTION(glProgramUniform3uiv       ); 
DEFINE_GL_FUNCTION(glProgramUniform4uiv       ); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix2fv  ); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix3fv  ); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix4fv  ); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix2x3fv); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix3x2fv); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix2x4fv); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix4x2fv); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix3x4fv); 
DEFINE_GL_FUNCTION(glProgramUniformMatrix4x3fv); 
// Buffer Objects ------------------------------------------------------
DEFINE_GL_FUNCTION(glCreateBuffers           ); 
DEFINE_GL_FUNCTION(glVertexArrayVertexBuffer ); 
DEFINE_GL_FUNCTION(glVertexArrayElementBuffer); 
DEFINE_GL_FUNCTION(glDeleteBuffers           ); 
DEFINE_GL_FUNCTION(glDrawArrays              ); 
DEFINE_GL_FUNCTION(glDrawElements            ); 
DEFINE_GL_FUNCTION(glNamedBufferData         ); 
DEFINE_GL_FUNCTION(glNamedBufferSubData      ); 
DEFINE_GL_FUNCTION(glBindVertexBuffer        ); 
DEFINE_GL_FUNCTION(glBindBuffer              ); 
DEFINE_GL_FUNCTION(glBindBufferBase          ); 
DEFINE_GL_FUNCTION(glMapNamedBuffer          ); 
DEFINE_GL_FUNCTION(glMapNamedBufferRange     ); 
DEFINE_GL_FUNCTION(glUnmapNamedBuffer        );
DEFINE_GL_FUNCTION(glNamedBufferStorage      ); 
DEFINE_GL_FUNCTION(glCreateVertexArrays      ); 
DEFINE_GL_FUNCTION(glEnableVertexArrayAttrib ); 
DEFINE_GL_FUNCTION(glDisableVertexArrayAttrib); 
DEFINE_GL_FUNCTION(glVertexArrayAttribFormat ); 
DEFINE_GL_FUNCTION(glVertexArrayAttribIFormat); 
DEFINE_GL_FUNCTION(glVertexArrayAttribLFormat); 
DEFINE_GL_FUNCTION(glVertexArrayAttribBinding); 
// State Management ----------------------------------------------------
DEFINE_GL_FUNCTION(glBlendColor            ); 
DEFINE_GL_FUNCTION(glBlendEquation         ); 
DEFINE_GL_FUNCTION(glBlendEquationi        ); 
DEFINE_GL_FUNCTION(glBlendEquationSeparate ); 
DEFINE_GL_FUNCTION(glBlendEquationSeparatei); 
DEFINE_GL_FUNCTION(glBlendFunc             ); 
DEFINE_GL_FUNCTION(glBlendFunci            ); 
DEFINE_GL_FUNCTION(glBlendFuncSeparate     ); 
DEFINE_GL_FUNCTION(glBlendFuncSeparatei    ); 
DEFINE_GL_FUNCTION(glCullFace              ); 
DEFINE_GL_FUNCTION(glEnable                ); 
DEFINE_GL_FUNCTION(glDisable               ); 
DEFINE_GL_FUNCTION(glEnablei               ); 
DEFINE_GL_FUNCTION(glDisablei              ); 
DEFINE_GL_FUNCTION(glGetBooleanv           ); 
DEFINE_GL_FUNCTION(glGetDoublev            ); 
DEFINE_GL_FUNCTION(glGetFloatv             ); 
DEFINE_GL_FUNCTION(glGetIntegerv           ); 
DEFINE_GL_FUNCTION(glGetInteger64v         ); 
DEFINE_GL_FUNCTION(glGetBooleani_v         ); 
DEFINE_GL_FUNCTION(glGetDoublei_v          ); 
DEFINE_GL_FUNCTION(glGetFloati_v           ); 
DEFINE_GL_FUNCTION(glGetIntegeri_v         ); 
DEFINE_GL_FUNCTION(glGetInteger64i_v       ); 
DEFINE_GL_FUNCTION(glPixelStoref           ); 
DEFINE_GL_FUNCTION(glPixelStorei           ); 
DEFINE_GL_FUNCTION(glViewport              ); 
DEFINE_GL_FUNCTION(glScissor               ); 
DEFINE_GL_FUNCTION(glPolygonMode           ); 
DEFINE_GL_FUNCTION(glDepthFunc);
// Transform Feedback --------------------------------------------------
// Utility -------------------------------------------------------------
DEFINE_GL_FUNCTION(glGetString );
DEFINE_GL_FUNCTION(glGetStringi);
// Queries -------------------------------------------------------------
// Syncing -------------------------------------------------------------
// Vertex Array Objects ------------------------------------------------
DEFINE_GL_FUNCTION(glBindVertexArray   ); 
DEFINE_GL_FUNCTION(glDeleteVertexArrays); 
// Samplers ------------------------------------------------------------
// Program Pipelines ---------------------------------------------------
// Debug ---------------------------------------------------------------
DEFINE_GL_FUNCTION(glDebugMessageCallback);

b32 gl_load_functions( GetProcAddressFN get_proc_address ) {
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
    LOAD_PROC_REQUIRED( glNamedFramebufferTexture );
    LOAD_PROC_REQUIRED( glNamedRenderbufferStorage );
    LOAD_PROC_REQUIRED( glNamedFramebufferRenderbuffer );
    LOAD_PROC_REQUIRED( glCheckNamedFramebufferStatus );
    LOAD_PROC_REQUIRED( glBlitNamedFramebuffer );
    LOAD_PROC_REQUIRED( glNamedFramebufferDrawBuffer );
    LOAD_PROC_REQUIRED( glNamedFramebufferDrawBuffers );
    LOAD_PROC_REQUIRED(glNamedFramebufferTextureLayer);

    LOAD_PROC_REQUIRED( glCreateShader );
    LOAD_PROC_REQUIRED( glCreateProgram );
    LOAD_PROC_REQUIRED( glShaderSource );
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
    LOAD_PROC_REQUIRED( glPolygonMode );
    LOAD_PROC_REQUIRED(glDepthFunc);

    LOAD_PROC_REQUIRED( glGetString );
    LOAD_PROC_REQUIRED( glGetStringi );

    LOAD_PROC_REQUIRED( glBindVertexArray );
    LOAD_PROC_REQUIRED( glDeleteVertexArrays );

    LOAD_PROC_REQUIRED( glDebugMessageCallback );
    return true;
}

