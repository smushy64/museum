// * Description:  OpenGL Shader Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 24, 2023
#include "defines.h"
#include "core/log.h"
#include "core/strings.h"
#include "core/graphics/internal/opengl.h"
#include "core/graphics/internal/opengl/types.h"
#include "core/graphics/internal/opengl/functions.h"
#include "core/graphics/internal/opengl/shader.h"

internal const char* gl_shader_stage_to_cstr( GLShaderStage stage );
internal void gl_shader_report_error(
    GLuint id, b32 is_program, usize* out_len, const char** out_message );

internal b32 gl_create_shader( GLShaderStage stage, GLShaderID* out_shader ) {
    GLShaderID shader = glCreateShader( stage );
    if( !shader ) {
        GL_LOG_ERROR( "Failed to create shader!" );
        GL_LOG_ERROR(
            "Shader stage is invalid: {u32,x}", shader );
        return false;
    }

    *out_shader = shader;
    return true;
}

b32 gl_shader_compile_source(
    GLint         source_len,
    const char*   source,
    GLShaderStage stage,
    GLShaderID*   out_shader
) {
    GLShaderID shader = 0;
    if( !gl_create_shader( stage, &shader ) ) {
        return false;
    }

    glShaderSource( shader, 1, &source, &source_len );
    glCompileShader( shader );

    GLint compile_status = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_status );

    if( compile_status ) {
        GL_LOG_NOTE(
            "Shader [{u32}][{cc}] compiled successfully.",
            shader, gl_shader_stage_to_cstr( stage ) );
        *out_shader = shader;
        return true;
    }

    usize       error_message_len = 0;
    const char* error_message     = NULL;
    gl_shader_report_error( shader, false, &error_message_len, &error_message );
    StringSlice error = ss_from_cstr( error_message_len, error_message );

    GL_LOG_ERROR(
        "Failed to compile shader stage {cc}!", gl_shader_stage_to_cstr( stage ) );
    if( !error.len ) {
        GL_LOG_ERROR( "Unable to retrieve error log!" );
    } else {
        GL_LOG_ERROR( "{s}", error );
    }

    return false;
}
b32 gl_shader_compile_spirv(
    usize         buffer_size,
    void*         buffer,
    GLShaderStage stage,
    const char*   entry_point,
    GLuint        specialization_constant_count,
    const GLuint* constant_index,
    const GLuint* constant_value,
    GLShaderID*   out_shader
) {
    GLShaderID shader = 0;
    if( !gl_create_shader( stage, &shader ) ) {
        return false;
    }

    glShaderBinary( 1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, buffer, buffer_size );
    glSpecializeShader(
        shader, entry_point, specialization_constant_count,
        constant_index, constant_value );

    GLint compile_status = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_status );

    if( compile_status ) {
        GL_LOG_NOTE(
            "Shader [{u32}][{cc}] compiled successfully.",
            shader, gl_shader_stage_to_cstr( stage ) );
        *out_shader = shader;
        return true;
    }

    usize       error_message_len = 0;
    const char* error_message     = NULL;
    gl_shader_report_error( shader, false, &error_message_len, &error_message );
    StringSlice error = ss_from_cstr( error_message_len, error_message );

    GL_LOG_ERROR(
        "Failed to compile shader stage {cc}!", gl_shader_stage_to_cstr( stage ) );
    if( !error.len ) {
        GL_LOG_ERROR( "Unable to retrieve error log!" );
    } else {
        GL_LOG_ERROR( "{s}", error );
    }

    return false;
}
b32 gl_shader_program_link(
    usize count, GLShaderID* shaders, GLShaderProgramID* out_shader_program
) {
    GLShaderProgramID program = glCreateProgram();
    if( !program ) {
        GL_LOG_FATAL( "An unknown error occurred when creating a Shader Program!" );
        return false;
    }

    for( usize i = 0; i < count; ++i ) {
        glAttachShader( program, shaders[i] );
    }

    glLinkProgram( program );

    for( usize i = 0; i < count; ++i ) {
        glDetachShader( program, shaders[i] );
    }

    GLint link_status = 0;
    glGetProgramiv( program, GL_LINK_STATUS, &link_status );

    if( link_status ) {
        GL_LOG_NOTE( "Program [{u32}] compiled successfully.", program );
        *out_shader_program = program;
        return true;
    }

    usize       error_message_len = 0;
    const char* error_message     = NULL;
    gl_shader_report_error( program, true, &error_message_len, &error_message );
    StringSlice error = ss_from_cstr( error_message_len, error_message );

    GL_LOG_ERROR( "Failed to link shaders!" );
    if( !error.len ) {
        GL_LOG_ERROR( "Unable to retrieve error log!" );
    } else {
        GL_LOG_ERROR( "{s}", error );
    }

    return false;
}
void gl_shader_delete( usize count, GLShaderID* shaders ) {
    for( usize i = 0; i < count; ++i ) {
        glDeleteShader( shaders[i] );
    }
}
void gl_shader_program_delete( usize count, GLShaderProgramID* programs ) {
    for( usize i = 0; i < count; ++i ) {
        glDeleteProgram( programs[i] );
    }
}
#define GL_INFO_LOG_LEN (512)
global char GL_INFO_LOG[GL_INFO_LOG_LEN] = {};
internal void gl_shader_report_error(
    GLuint id, b32 is_program, usize* out_len, const char** out_message
) {
    GLint written_log_len = 0;
    if( is_program ) {
        glGetProgramInfoLog(
            id, GL_INFO_LOG_LEN,
            &written_log_len,
            GL_INFO_LOG
        );
    } else {
        glGetShaderInfoLog(
            id, GL_INFO_LOG_LEN,
            &written_log_len,
            GL_INFO_LOG
        );
    }

    if( written_log_len ) {
        *out_len     = (usize)written_log_len;
        *out_message = GL_INFO_LOG;
    } else {
        *out_len = 0;
    }
}

maybe_unused
internal const char* gl_shader_stage_to_cstr( GLShaderStage stage ) {
    switch( stage ) {
        case GL_VERTEX_SHADER:          return "Vertex";
        case GL_FRAGMENT_SHADER:        return "Fragment";
        case GL_TESS_CONTROL_SHADER:    return "Tesselation Control";
        case GL_TESS_EVALUATION_SHADER: return "Tesselation Evaluation";
        case GL_GEOMETRY_SHADER:        return "Geometry";
        case GL_COMPUTE_SHADER:         return "Compute";
        default: return "Unknown";
    }
}

