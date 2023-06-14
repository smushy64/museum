#if !defined(RENDERER_OPENGL_GL_LOADER_HPP)
#define RENDERER_OPENGL_GL_LOADER_HPP
/**
 * Description:  OpenGL function loader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 11, 2023
*/
#include "defines.h"
typedef void* (*GetProcFN)(const char* function_name);

[[maybe_unused]]
b32 gl_load( GetProcFN getproc );

#endif // header guard