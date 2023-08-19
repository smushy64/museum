#if !defined(LD_RENDERER_OPENGL_HPP)
#define LD_RENDERER_OPENGL_HPP
// * Description:  OpenGL backend
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "ldrenderer.h"
#include "ldrenderer/context.h"
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

/// OpenGL Renderer Context
typedef struct OpenGLRendererContext {
    InternalRendererContext ctx;

    OpenGLDeviceInfo           device_info;   
    OpenGLRenderContextHandle* render_context;

    ivec2 viewport;
} OpenGLRendererContext;

b32 gl_renderer_backend_init( RendererContext* ctx );

#if defined(LD_LOGGING)
    #define GL_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[GL NOTE] " format,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[GL INFO] " format,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[GL DEBUG] " format,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[GL WARN] " format,\
            ##__VA_ARGS__\
        )
    #define GL_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[GL ERROR] " format,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[GL NOTE | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[GL INFO | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[GL DEBUG | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define GL_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[GL WARN | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[GL ERROR | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define GL_LOG_FATAL( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
            "[GL FATAL | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
#else
    #define GL_LOG_NOTE( format, ... )
    #define GL_LOG_INFO( format, ... )
    #define GL_LOG_DEBUG( format, ... )
    #define GL_LOG_WARN( format, ... )
    #define GL_LOG_ERROR( format, ... )
    #define GL_LOG_NOTE_TRACE( format, ... )
    #define GL_LOG_INFO_TRACE( format, ... )
    #define GL_LOG_DEBUG_TRACE( format, ... )
    #define GL_LOG_WARN_TRACE( format, ... )
    #define GL_LOG_ERROR_TRACE( format, ... )
    #define GL_LOG_FATAL( format, ... )
#endif


#endif // header guard
