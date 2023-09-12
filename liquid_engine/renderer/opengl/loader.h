#if !defined(LD_RENDERER_OPENGL_LOADER_HPP)
#define LD_RENDERER_OPENGL_LOADER_HPP
// * Description:  OpenGL Function Loader
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"

typedef void* (*GetProcAddressFN)( const char* function_name );

b32 gl_load_functions( GetProcAddressFN get_proc_address );

#endif // header guard
