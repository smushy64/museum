#if !defined(OPENGL_HPP)
#define OPENGL_HPP
/**
 * Description:  OpenGL
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 05, 2023
*/
#include "defines.h"
#include "opengl/gltypes.h"

namespace internal {

#define DEFINE_GL_FUNCTION( name, return_val, ... )\
    typedef return_val (*name##_fn)( __VA_ARGS__ );\
    SM_GLOBAL inline name##_fn in_##name = nullptr;

DEFINE_GL_FUNCTION( glGetString, const GLubyte*, GLenum name );
DEFINE_GL_FUNCTION( glEnable, void, GLenum cap );
DEFINE_GL_FUNCTION( glDisable, void, GLenum cap );
DEFINE_GL_FUNCTION( glClear, void, GLbitfield mask );
DEFINE_GL_FUNCTION( glClearColor, void,
    GLfloat r, GLfloat g, GLfloat b, GLfloat a
);
DEFINE_GL_FUNCTION( glClearStencil, void, GLint s );
DEFINE_GL_FUNCTION( glClearDepth, void, GLdouble depth );

} // internal

#define glGetString    ::internal::in_glGetString
#define glClear        ::internal::in_glClear
#define glEnable       ::internal::in_glEnable
#define glDisable      ::internal::in_glDisable
#define glClearColor   ::internal::in_glClearColor
#define glClearStencil ::internal::in_glClearStencil
#define glClearDepth   ::internal::in_glClearDepth

b32 gl_load( GLLoadProc load_proc );

#endif