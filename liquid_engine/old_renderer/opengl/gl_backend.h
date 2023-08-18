#if !defined(RENDERER_GL_BACKEND_HPP)
#define RENDERER_GL_BACKEND_HPP
/**
 * Description:  OpenGL Backend
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "defines.h"
#include "renderer/renderer.h"
#include "gl_types.h"
#include "gl_loader.h"
#include "gl_shader.h"
#include "gl_buffer.h"

b32 gl_renderer_backend_initialize( RendererContext* generic_ctx );
void gl_renderer_backend_shutdown( RendererContext* ctx );
void gl_renderer_backend_on_resize(
    RendererContext* ctx,
    i32 width, i32 height
);
b32 gl_renderer_backend_begin_frame(
    RendererContext* ctx,
    struct RenderOrder* order
);
b32 gl_renderer_backend_end_frame(
    RendererContext* ctx,
    struct RenderOrder* order
);

#if defined(DEBUG)
#define MAX_DEBUG_POINTS 32
#endif

#define PHONG_U_TRANSFORM (0)
#define PHONG_TEXTURE     (0)

#define SPRITE_U_TRANSFORM        (0)
#define SPRITE_U_ATLAS_COORDINATE (1)
#define SPRITE_U_FLIP             (2)
#define SPRITE_U_ATLAS_CELL_SIZE  (3)
#define SPRITE_U_Z_INDEX          (4)
#define SPRITE_U_TINT             (5)
#define SPRITE_TEXTURE            (0)

#define DEBUG_TEXT_U_TRANSFORM   (0)
#define DEBUG_TEXT_U_COLOR       (1)
#define DEBUG_TEXT_U_COORDINATES (2)

#define DEBUG_UI_IMAGE_U_TRANSFORM (0)
#define DEBUG_UI_IMAGE_U_TINT      (1)

#define DEBUG_U_COLOR (0)
typedef struct OpenGLRendererContext {
    RendererContext ctx;

    GLuint u_matrices;

    ShaderProgram phong;
    ShaderProgram sprite;
    ShaderProgram font;
    ShaderProgram ui_image;

    vec2 viewport;

#if defined(DEBUG)
    ShaderProgram debug;
    GLuint        debug_vao, debug_vbo;
#endif

    const char* device_vendor;
    const char* device_name;
    const char* device_version;
    const char* device_glsl_version;

    i32 extension_count;

    void* glrc;
} OpenGLRendererContext;

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
