/**
 * Description:  OpenGL
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 05, 2023
*/
#include "opengl.h"
#include "core/logging.h"

#define LOAD( function ) do {\
    in_##function = (__typeof(in_##function))load_proc(#function);\
    if( !in_##function ) {\
        LOG_WARN("Failed to load function \"" #function "\"!");\
    }\
} while(0)

#define LOAD_REQUIRED( function ) do {\
    in_##function = (__typeof(in_##function))load_proc(#function);\
    if( !in_##function ) {\
        LOG_ERROR("Failed to load required function \"" #function "\"!");\
        return false;\
    }\
} while(0)

b32 gl_load( GLLoadProc load_proc ) {
    using namespace internal;
    
    LOAD_REQUIRED(glGetString);
    LOAD_REQUIRED(glClear);
    LOAD_REQUIRED(glEnable);
    LOAD_REQUIRED(glDisable);
    LOAD_REQUIRED(glClearColor);
    LOAD_REQUIRED(glClearStencil);
    LOAD_REQUIRED(glClearDepth);

    return true;
}
