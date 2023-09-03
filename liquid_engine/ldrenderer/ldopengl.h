#if !defined(LD_RENDERER_OPENGL_HPP)
#define LD_RENDERER_OPENGL_HPP
// * Description:  OpenGL backend
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "ldrenderer.h"
#include "ldrenderer/context.h"
#include "ldrenderer/opengl/types.h"
#include "ldrenderer/opengl/shader.h"
#include "core/ldmath/types.h"
#include "core/ldlog.h"

/// GLRC Handle
typedef void OpenGLRenderContextHandle;

/// Device info from OpenGL driver
typedef struct OpenGLDeviceInfo {
    const char* vendor;
    const char* name;
    const char* version;
    const char* glsl_version;

    i32 extension_count;
} OpenGLDeviceInfo;

#define GL_VERTEX_ARRAY_COUNT   (2)
#define GL_SHADER_PROGRAM_COUNT (2)
#define GL_BUFFER_COUNT         (4)

#define GL_SHADER_PROGRAM_INDEX_FRAMEBUFFER (0)
#define GL_SHADER_PROGRAM_INDEX_COLOR       (1)

#define GL_SHADER_PROGRAM_FRAMEBUFFER_TEXTURE_BINDING (0)

#define GL_SHADER_PROGRAM_COLOR_LOCATION_TRANSFORM (0) 
#define GL_SHADER_PROGRAM_COLOR_LOCATION_COLOR     (1) 

#define GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER (0)
#define GL_VERTEX_ARRAY_INDEX_QUAD_2D     (1)

#define GL_BUFFER_INDEX_UBO_CAMERA      (0)
#define GL_BUFFER_INDEX_VBO_FRAMEBUFFER (1)
#define GL_BUFFER_INDEX_VBO_QUAD_2D     (2)
#define GL_BUFFER_INDEX_EBO_QUAD        (3)

/// OpenGL Renderer Context
typedef struct OpenGLRendererContext {
    InternalRendererContext ctx;

    OpenGLDeviceInfo           device_info;   
    OpenGLRenderContextHandle* render_context;

    GLBufferID buffers[GL_BUFFER_COUNT];

    GLShaderProgramID programs[GL_SHADER_PROGRAM_COUNT];

    GLVertexArrayID vertex_arrays[GL_VERTEX_ARRAY_COUNT];

    GLFramebuffer framebuffer_main;
} OpenGLRendererContext;

#if defined(LD_LOGGING)
    #define GL_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            false, true,\
            "[GL NOTE] " format,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            false, true,\
            LOG_COLOR_WHITE\
            "[GL INFO] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            false, true,\
            LOG_COLOR_BLUE\
            "[GL DEBUG] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[GL WARN] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            false, true,\
            LOG_COLOR_RED\
            "[GL ERROR] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[GL NOTE | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_WHITE\
            "[GL INFO | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_BLUE\
            "[GL DEBUG | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define GL_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[GL WARN | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_RED\
            "[GL ERROR | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_FATAL( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            true, true,\
            LOG_COLOR_RED\
            "[GL FATAL | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
#else
    #define GL_LOG_NOTE( format, ... )        unused(format)
    #define GL_LOG_INFO( format, ... )        unused(format) 
    #define GL_LOG_DEBUG( format, ... )       unused(format)  
    #define GL_LOG_WARN( format, ... )        unused(format)   
    #define GL_LOG_ERROR( format, ... )       unused(format)    
    #define GL_LOG_NOTE_TRACE( format, ... )  unused(format)      
    #define GL_LOG_INFO_TRACE( format, ... )  unused(format)         
    #define GL_LOG_DEBUG_TRACE( format, ... ) unused(format)        
    #define GL_LOG_WARN_TRACE( format, ... )  unused(format)         
    #define GL_LOG_ERROR_TRACE( format, ... ) unused(format)            
    #define GL_LOG_FATAL( format, ... )       unused(format)        
#endif


#endif // header guard
