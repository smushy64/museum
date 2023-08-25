// * Description:  OpenGL Shaders Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 19, 2023
#include "ldrenderer/opengl/shader.h"
#include "ldrenderer/opengl/functions.h"
#include "ldrenderer/ldopengl.h"
#include "core/ldmemory.h"

#define MAX_INFO_LOG_BUFFER_LEN (256)
char INFO_LOG_BUFFER[MAX_INFO_LOG_BUFFER_LEN];

internal void gl_log_compilation_error( GLShader shader ) {
    GLint info_log_len = 0;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &info_log_len );

    GLint written_info_log_len = 0;
    glGetShaderInfoLog(
        shader,
        MAX_INFO_LOG_BUFFER_LEN,
        &written_info_log_len,
        INFO_LOG_BUFFER
    );

    if( info_log_len != written_info_log_len ) {
        GL_LOG_WARN(
            "INFO_LOG_BUFFER is not large enough to contain "
            "info log!"
        );
        GL_LOG_WARN(
            "Info log length: {i32} | "
            "Max info log len: {i32} | "
            "Written log len: {i32}",
            info_log_len, MAX_INFO_LOG_BUFFER_LEN,
            written_info_log_len
        );
    }

    GL_LOG_ERROR( "Shader Compilation Error!" );
    GL_LOG_ERROR( "{cc}", INFO_LOG_BUFFER );
}

internal void gl_log_linking_error( GLShaderProgram shader_program ) {
    GLint info_log_len = 0;
    glGetProgramiv( shader_program, GL_INFO_LOG_LENGTH, &info_log_len );

    GLint written_info_log_len = 0;
    glGetProgramInfoLog(
        shader_program,
        MAX_INFO_LOG_BUFFER_LEN,
        &written_info_log_len,
        INFO_LOG_BUFFER
    );

    if( info_log_len != written_info_log_len ) {
        GL_LOG_WARN(
            "INFO_LOG_BUFFER is not large enough to contain "
            "info log!"
        );
        GL_LOG_WARN(
            "Info log length: {i32} | "
            "Max info log len: {i32} | "
            "Written log len: {i32}",
            info_log_len, MAX_INFO_LOG_BUFFER_LEN,
            written_info_log_len
        );
    }

    GL_LOG_ERROR( "Shader Program Linking Error!" );
    GL_LOG_ERROR( "{cc}", INFO_LOG_BUFFER );
}

b32 gl_shader_compile_source(
    GLint         source_length,
    const char*   source,
    GLShaderStage shader_stage,
    GLShader*     out_shader
) {
    GLShader shader = glCreateShader( shader_stage );
    if( !shader ) {
        GL_LOG_ERROR(
            "Failed to create shader! "
            "Shader stage is likely invalid: {u32}",
            shader_stage
        );
        return false;
    }

    glShaderSource( shader, 1, &source, &source_length );
    glCompileShader( shader );

    GLint compile_status = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_status );

    if( compile_status ) {
        *out_shader = shader;
        return true;
    }

    gl_log_compilation_error( shader );

    return false;
}

b32 gl_shader_compile_spirv(
    usize         spirv_binary_size,
    void*         spirv_binary_buffer,
    GLShaderStage shader_stage,
    const char*   shader_entry_point,
    GLuint        num_specialization_constants,
    const GLuint* constant_index,
    const GLuint* constant_value,
    GLShader*     out_shader
) {
    GLShader shader = glCreateShader( shader_stage );
    if( !shader ) {
        GL_LOG_ERROR(
            "Failed to create shader! "
            "Shader stage is likely invalid: {u32}",
            shader_stage
        );
        return false;
    }

    glShaderBinary(
        1, &shader,
        GL_SHADER_BINARY_FORMAT_SPIR_V,
        spirv_binary_buffer,
        spirv_binary_size
    );

    glSpecializeShader(
        shader,
        shader_entry_point,
        num_specialization_constants,
        constant_index,
        constant_value
    );

    GLint compile_status = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_status );

    if( compile_status ) {
        *out_shader = shader;
        return true;
    }

    gl_log_compilation_error( shader );

    return false;
}
b32 gl_shader_program_link(
    usize            shader_count,
    GLShader*        shaders,
    GLShaderProgram* out_shader_program
) {
    GLShaderProgram shader_program = glCreateProgram();
    if( !shader_program ) {
        GL_LOG_ERROR(
            "An unknown error occurred when creating shader program!"
        );
        return false;
    }

    for( usize i = 0; i < shader_count; ++i ) {
        glAttachShader( shader_program, shaders[i] );
    }

    glLinkProgram( shader_program );

    for( usize i = 0; i < shader_count; ++i ) {
        glDetachShader( shader_program, shaders[i] );
    }

    GLint link_status = 0;
    glGetProgramiv( shader_program, GL_LINK_STATUS, &link_status );

    if( link_status ) {
        *out_shader_program = shader_program;
        return true;
    }

    gl_log_linking_error( shader_program );

    return false;
}
void gl_shader_delete( usize shader_count, GLShader* shaders ) {
    for( usize i = 0; i < shader_count; ++i ) {
        glDeleteShader( shaders[i] );
    }
}
void gl_shader_program_delete(
    usize shader_program_count, GLShaderProgram* shader_programs
) {
    for( usize i = 0; i < shader_program_count; ++i ) {
        glDeleteProgram( shader_programs[i] );
    }
}


