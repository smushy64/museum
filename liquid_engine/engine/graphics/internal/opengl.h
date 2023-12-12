#if !defined(LD_CORE_GRAPHICS_INTERNAL_OPENGL_H)
#define LD_CORE_GRAPHICS_INTERNAL_OPENGL_H
// * Description:  OpenGL
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 24, 2023
#include "defines.h"

#if defined(LD_API_INTERNAL)
#include "engine/internal/platform.h"
#include "engine/graphics/types.h"
#include "engine/graphics/internal.h"
#include "engine/graphics/internal/opengl/types.h"

/// Initialize OpenGL subsystem.
b32 gl_subsystem_init(void);
/// Loader
b32 gl_load_functions( PlatformGLLoadProcFN* proc );

/// OpenGL Subsystem.
struct OpenGLSubsystem;
/// GL Render Context
typedef void GLRC;
/// Information about the current driver.
struct OpenGLDriverInfo {
    const char* vendor;
    const char* name;
    const char* version;
    const char* glsl_version;
};

#define GL_UNIFORM_BUFFER_COUNT (4)
#define GL_SHADER_PROGRAM_COUNT (4)
#define GL_FRAMEBUFFER_COUNT    (6)
#define GL_VERTEX_ARRAY_COUNT   (255)
#define GL_TEXTURE_COUNT        (255)

#define GL_UNIFORM_BUFFER_INDEX_CAMERA (0)
#define GL_UNIFORM_BUFFER_INDEX_LIGHTS (1)
#define GL_UNIFORM_BUFFER_INDEX_DATA   (2)

#define GL_UNIFORM_BUFFER_BINDING_CAMERA    (0)
#define GL_UNIFORM_BUFFER_BINDING_LIGHTS    (1)
#define GL_UNIFORM_BUFFER_BINDING_DATA      (2)
#define GL_UNIFORM_BUFFER_BINDING_TRANSFORM (3)

#define GL_FRAMEBUFFER_INDEX_POST_PROCESS       (0)
#define GL_FRAMEBUFFER_INDEX_SHADOW_DIRECTIONAL (1)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_0     (2)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_1     (3)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_2     (4)
#define GL_FRAMEBUFFER_INDEX_SHADOW_POINT_3     (5)
#define GL_FRAMEBUFFER_SHADOW_POINT_COUNT       (4)

#define GL_SHADER_PROGRAM_INDEX_POST_PROCESS       (0)
#define GL_SHADER_PROGRAM_INDEX_SHADOW_DIRECTIONAL (1)
#define GL_SHADER_PROGRAM_INDEX_SHADOW_POINT       (2)
#define GL_SHADER_PROGRAM_INDEX_PHONG_BRDF         (3)

#define GL_SHADER_PROGRAM_BINDING_DIFFUSE                (0)
#define GL_SHADER_PROGRAM_BINDING_NORMAL                 (1)
#define GL_SHADER_PROGRAM_BINDING_ROUGHNESS              (2)
#define GL_SHADER_PROGRAM_BINDING_METALLIC               (3)
#define GL_SHADER_PROGRAM_BINDING_DIRECTIONAL_SHADOW_MAP (10)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_0     (11)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_1     (12)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_2     (13)
#define GL_SHADER_PROGRAM_BINDING_POINT_SHADOW_MAP_3     (14)

#define GL_SHADER_PROGRAM_POST_PROCESS_BINDING_FRAMEBUFFER_COLOR (0)

#define GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_TINT               (0)
#define GL_SHADER_PROGRAM_PHONG_BRDF_LOCATION_IS_SHADOW_RECEIVER (1)

#define GL_SHADER_PROGRAM_SHADOW_POINT_LOCATION_INDEX (0)

#define GL_VERTEX_ARRAY_INDEX_CUBE        (0)
#define GL_VERTEX_ARRAY_INDEX_FRAMEBUFFER (1)

#define GL_TEXTURE_INDEX_DIFFUSE_NULL   (0)
#define GL_TEXTURE_INDEX_NORMAL_NULL    (1)
#define GL_TEXTURE_INDEX_ROUGHNESS_NULL (2)
#define GL_TEXTURE_INDEX_METALLIC_NULL  (3)

typedef enum : u8 {
    GL_FRAMEBUFFER_TYPE_POST_PROCESS,
    GL_FRAMEBUFFER_TYPE_SHADOW_DIRECTIONAL,
    GL_FRAMEBUFFER_TYPE_SHADOW_POINT,

    GL_FRAMEBUFFER_TYPE_COUNT
} GLFramebufferType;
header_only const char* gl_framebuffer_type_to_cstr( GLFramebufferType type ) {
    assert( type < GL_FRAMEBUFFER_TYPE_COUNT );
    const char* types[GL_FRAMEBUFFER_TYPE_COUNT] = {
        "Post Process",
        "Shadow Directional",
        "Shadow Point"
    };
    return types[type];
}
void gl_framebuffers_create(
    usize from_inclusive, usize to_exclusive,
    ivec2* dimensions, GLFramebufferType* types, b32 log );
void gl_framebuffers_resize(
    usize from_inclusive, usize to_exclusive, ivec2* dimensions );

typedef enum : u8 {
    GL_VERTEX_ARRAY_TYPE_ARRAY,
    GL_VERTEX_ARRAY_TYPE_INDEXED,

    GL_VERTEX_ARRAY_TYPE_COUNT
} GLVertexArrayType;
header_only const char* gl_vertex_array_type_to_cstr( GLVertexArrayType type ) {
    assert( type < GL_VERTEX_ARRAY_TYPE_COUNT );
    const char* types[GL_VERTEX_ARRAY_TYPE_COUNT] = {
        "Array",
        "Indexed"
    };
    return types[type];
}

struct GLVertexBuffer {
    GLBufferID vertex_buffer;
    GLBufferID element_buffer;
};

struct GLVertexBufferLayout {
    GLint*  attribute_component_counts;
    GLenum* attribute_types;
    usize   attribute_count;
    usize   stride;
};

void gl_vertex_arrays_create(
    usize from_inclusive, usize to_exclusive,
    GLIndexType* index_types, GLDrawMode* draw_modes,
    GLVertexArrayType* vertex_array_types,
    u32* index_counts,  void** index_buffers,
    u32* vertex_counts, struct GLVertexBufferLayout* vertex_layouts,
    void** vertex_buffers,
    GLUsageHint* index_buffer_usage_hints, GLUsageHint* vertex_buffer_usage_hints
);
void gl_vertex_arrays_delete( usize count, RenderID* indices );
void gl_vertex_arrays_delete_range( usize from_inclusive, usize to_exclusive );
void gl_vertex_array_draw( usize index, GLDrawMode* draw_mode_override );

struct no_padding OpenGLUniformBufferCamera {
    mat4 view_projection_ui;
    mat4 view_projection_3d;
    union {
        struct {
            vec3 world_position;
            u32  ___padding0;
        };
        vec4 world_position_v4;
    };
    union {
        struct {
            f32 near_clip;
            f32 far_clip;
            f32 aspect_ratio;
            f32 field_of_view;
        };
        vec4 data;
    };
};

struct no_padding OpenGLUniformBufferDirectionalLight {
    union {
        struct {
            vec3 color;
            f32  is_active;
        };
        vec4 color_v4;
    };
    union {
        struct {
            vec3 direction;
            u32  ___padding1;
        };
        vec4 direction_v4;
    };
    mat4 matrix;
};

#define GL_POINT_LIGHT_MATRIX_COUNT (6)
struct no_padding OpenGLUniformBufferPointLight {
    union {
        struct {
            vec3 color;
            u32  ___padding0;
        };
        vec4 color_v4;
    };
    union {
        struct {
            vec3 position;
            u32  ___padding1;
        };
        vec4 position_v4;
    };
    mat4 matrices[GL_POINT_LIGHT_MATRIX_COUNT];
    union {
        struct {
            f32 is_active;
            f32 near_clip;
            f32 far_clip;
            u32 ___padding2;
        };
        vec4 data;
    };
};

struct no_padding OpenGLUniformBufferLights {
    struct OpenGLUniformBufferDirectionalLight directional;
    struct OpenGLUniformBufferPointLight       point[POINT_LIGHT_COUNT];
};

struct no_padding OpenGLUniformBufferData {
    union {
        struct {
            f32 elapsed_seconds;
            f32 delta_seconds;
            u32 frame_count;
            u32 ___padding0;
        };
        vec4 time;
    };
    union {
        struct {
            vec2 surface_resolution;
            f32  aspect_ratio;
            u32  ___padding1;
        };
        vec4 surface_resolution_4;
    };
};

struct OpenGLSubsystem {
    struct OpenGLDriverInfo driver;
    
    union {
        struct {
            GLShaderProgramID sh_post_process;
            GLShaderProgramID sh_shadow_directional;
            GLShaderProgramID sh_shadow_point;
            GLShaderProgramID sh_phong_brdf;
        };
        GLShaderProgramID programs[GL_SHADER_PROGRAM_COUNT];
    };
    union {
        struct {
            GLBufferID ubo_camera;
            GLBufferID ubo_lights;
            GLBufferID ubo_data;
            GLBufferID ubo_transform;
        };
        GLBufferID uniform_buffers[GL_UNIFORM_BUFFER_COUNT];
    };
    struct {
        GLFramebufferID   fbo_id[GL_FRAMEBUFFER_COUNT];
        GLFramebufferType fbo_type[GL_FRAMEBUFFER_COUNT];
        ivec2             fbo_dimensions[GL_FRAMEBUFFER_COUNT];
        GLTextureID       fbo_texture_0[GL_FRAMEBUFFER_COUNT];
        GLTextureID       fbo_texture_1[GL_FRAMEBUFFER_COUNT];
    };
    struct {
        GLTextureID         textures[GL_TEXTURE_COUNT];
        GraphicsTextureType tx_types[GL_TEXTURE_COUNT];
        ivec3               tx_dimensions[GL_TEXTURE_COUNT];
    };
    struct {
        GLVertexArrayID       vertex_arrays[GL_VERTEX_ARRAY_COUNT];
        GLVertexArrayType     vao_types[GL_VERTEX_ARRAY_COUNT];
        u32                   vao_counts[GL_VERTEX_ARRAY_COUNT];
        GLIndexType           vao_index_types[GL_VERTEX_ARRAY_COUNT];
        GLDrawMode            vao_draw_modes[GL_VERTEX_ARRAY_COUNT];
        struct GLVertexBuffer vao_buffers[GL_VERTEX_ARRAY_COUNT];
    };
};

#if defined(LD_LOGGING)
    #define fatal_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_FATAL, NULL,\
			false, true, true, true,\
            "[GL FATAL | " __FILE__ ":{u} > {cc}()] " format,\
            __LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define error_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_ERROR, NULL,\
			false, false, true, true, "[GL ERROR] " format, ##__VA_ARGS__ )
    #define warn_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_WARN, NULL,\
			false, false, true, true, "[GL WARN] " format, ##__VA_ARGS__ )
    #define debug_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_DEBUG, NULL,\
			false, false, true, true, "[GL DEBUG] " format, ##__VA_ARGS__ )
    #define info_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_INFO, NULL,\
			false, false, true, true, "[GL INFO] " format, ##__VA_ARGS__ )
    #define note_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_NOTE, NULL,\
			false, false, true, true, "[GL NOTE] " format, ##__VA_ARGS__ )

    #define trace_error_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_ERROR, NULL,\
			true, false, true, true,\
            "[GL ERROR | " __FILE__ ":{u} > {cc}()] " format,\
            __LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_warn_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_WARN, NULL,\
			true, false, true, true,\
            "[GL WARN | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_debug_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_DEBUG, NULL,\
			true, false, true, true,\
            "[GL DEBUG | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_info_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_INFO, NULL,\
			true, false, true, true,\
            "[GL INFO | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_note_log_gl( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_NOTE, NULL,\
			true, false, true, true,\
            "[GL NOTE | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
#else
    #define fatal_log_gl( ... )
    #define error_log_gl( ... )
    #define warn_log_gl( ... )
    #define debug_log_gl( ... )
    #define info_log_gl( ... )
    #define note_log_gl( ... )

    #define trace_error_log_gl( ... )
    #define trace_warn_log_gl( ... )
    #define trace_debug_log_gl( ... )
    #define trace_info_log_gl( ... )
    #define trace_note_log_gl( ... )
#endif // LD_LOGGING


#endif // API internal
#endif // header guard
