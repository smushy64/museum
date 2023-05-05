#if !defined(GL_TYPES_HPP)
#define GL_TYPES_HPP
/**
 * Description:  OpenGL types
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 05, 2023
*/
#include "defines.h"

typedef i8  GLbyte;
typedef u8  GLubyte;
typedef i16 GLshort;
typedef u16 GLushort;
typedef i32 GLint;
typedef u32 GLuint;
typedef i64 GLint64;
typedef u64 GLuint64;

typedef f32 GLfloat;
typedef f64 GLdouble;

typedef i32 GLsizei;

typedef u8  GLboolean;
typedef u32 GLbitfield;

typedef void GLvoid;

typedef GLuint GLenum;

typedef void* (*GLLoadProc)( const char* name );

#endif // header guard