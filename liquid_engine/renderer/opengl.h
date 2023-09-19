#if !defined(LD_RENDERER_OPENGL_HPP)
#define LD_RENDERER_OPENGL_HPP
// * Description:  OpenGL backend
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 18, 2023
#include "defines.h"
#include "renderer.h"
#include "renderer/context.h"
#include "renderer/opengl/types.h"
#include "renderer/opengl/shader.h"
#include "renderer/opengl/buffer.h"
#include "core/mathf/types.h"
#include "core/log.h"

/// GLRC Handle
typedef void GLRC;

/// Device info from OpenGL driver
typedef struct OpenGLDeviceInfo {
    const char* vendor;
    const char* name;
    const char* version;
    const char* glsl_version;

    i32 extension_count;
} OpenGLDeviceInfo;
#define GL_SHADER_PROGRAM_LOCATION_TRANSFORM                      (0)
#define GL_SHADER_PROGRAM_LOCATION_NORMAL_TRANSFORM               (1)

#define GL_SHADER_PROGRAM_BINDING_DIRECTIONAL_SHADOW_MAP (10)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_0     (11)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_1     (12)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_2     (13)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_3     (14)

#define GL_VERTEX_ARRAY_COUNT    (3)
#define GL_SHADER_PROGRAM_COUNT  (5)
#define GL_BUFFER_COUNT          (8)
#define GL_TEXTURE_2D_COUNT      (3)
#define GL_FRAMBUFFER_COUNT      (6)

#define GL_SHADER_PROGRAM_INDEX_POST_PROCESS       (0)
#define GL_SHADER_PROGRAM_INDEX_COLOR              (1)
#define GL_SHADER_PROGRAM_INDEX_PHONG_BRDF         (2)
#define GL_SHADER_PROGRAM_INDEX_SHADOW_DIRECTIONAL (3)
#define GL_SHADER_PROGRAM_INDEX_SHADOW_POINT       (4)

#define GL_SHADER_PROGRAM_POST_PROCESS_RENDER_TEXTURE_BINDING  (0)

#define GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_TINT             (2)
#define GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_SHADOW_RECEIVER  (3)
#define GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_DIFFUSE_TEXTURE   (0)
#define GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_NORMAL_TEXTURE    (1)
#define GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_ROUGHNESS_TEXTURE (2)
#define GL_SHADER_PROGRAM_PHONG_BRDF_BINDING_METALLIC_TEXTURE  (3)

#define GL_SHADER_PROGRAM_SHADOW_POINT_LOCATION_POINT_INDEX (1)

#define GL_SHADER_PROGRAM_COLOR_LOCATION_COLOR (1) 

#define GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER (0)
#define GL_VERTEX_ARRAY_INDEX_QUAD_2D     (1)
#define GL_VERTEX_ARRAY_INDEX_CUBE_3D     (2)

enum : u32 {
    GL_BUFFER_INDEX_UBO_CAMERA     ,
    GL_BUFFER_INDEX_UBO_LIGHTS     ,
    GL_BUFFER_INDEX_UBO_DATA       ,
    GL_BUFFER_INDEX_VBO_FRAMEBUFFER,
    GL_BUFFER_INDEX_VBO_QUAD_2D    ,
    GL_BUFFER_INDEX_EBO_QUAD       ,
    GL_BUFFER_INDEX_VBO_CUBE_3D    ,
    GL_BUFFER_INDEX_EBO_CUBE_3D    ,
};

#define GL_TEXTURE_INDEX_NULL_DIFFUSE   (0)
#define GL_TEXTURE_INDEX_NULL_NORMAL    (1)
#define GL_TEXTURE_INDEX_NULL_ROUGHNESS (2)

#define GL_FRAMEBUFFER_INDEX_MAIN_FRAMEBUFFER   (0)
#define GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL (1)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0     (2)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_1     (3)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_2     (4)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_3     (5)

/// OpenGL Renderer Context
typedef struct OpenGLRendererContext {
    InternalRendererContext ctx;

    OpenGLDeviceInfo device_info;   
    GLRC* glrc;

    GLBufferID buffers[GL_BUFFER_COUNT];

    GLShaderProgramID programs[GL_SHADER_PROGRAM_COUNT];

    GLVertexArrayID vertex_arrays[GL_VERTEX_ARRAY_COUNT];

    GLTexture2D textures_2d[GL_TEXTURE_2D_COUNT];

    GLFramebuffer framebuffers[GL_FRAMBUFFER_COUNT];

    struct GLLightBuffer lights;
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
