/**
 * Description:  OpenGL shader implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
*/
#include "gl_backend.h"
#include "gl_shader.h"
#include "gl_functions.h"
#include "core/memory.h"
#include "core/string.h"

b32 gl_shader_compile(
    u32           spirv_binary_size,
    void*         spirv_binary_buffer,
    GLenum        shader_type,
    const char*   shader_entry_point,
    GLuint        num_specialization_constants,
    const GLuint* constant_index,
    const GLuint* constant_value,
    Shader*       out_shader
) {
    GLuint shader_handle = glCreateShader( shader_type );
    glShaderBinary(
        1,
        &shader_handle,
        GL_SHADER_BINARY_FORMAT_SPIR_V,
        spirv_binary_buffer,
        spirv_binary_size
    );

    glSpecializeShader(
        shader_handle,
        shader_entry_point,
        num_specialization_constants,
        constant_index,
        constant_value
    );

    GLint compile_status = 0;
    glGetShaderiv(
        shader_handle,
        GL_COMPILE_STATUS,
        &compile_status
    );

    if( compile_status ) {
        *out_shader = shader_handle;
        return true;
    } else {
        // TODO(alicia): don't heap allocate!
        GLint info_log_length = 0;
        glGetShaderiv(
            shader_handle,
            GL_INFO_LOG_LENGTH,
            &info_log_length
        );
        char* info_log_buffer = (char*)mem_alloc(
            info_log_length,
            MEMTYPE_RENDERER
        );
        glGetShaderInfoLog(
            shader_handle,
            info_log_length,
            nullptr,
            info_log_buffer
        );
        GL_LOG_ERROR( "Compilation Error:\n%s", info_log_buffer );
        mem_free( info_log_buffer );
        return false;
    }
}
b32 gl_shader_program_link(
    u32            shader_count,
    Shader*        shaders,
    ShaderProgram* out_shader_program
) {
    GLuint program_handle = glCreateProgram();

    for( u32 i = 0; i < shader_count; ++i ) {
        glAttachShader( program_handle, shaders[i] );
    }

    glLinkProgram( program_handle );

    for( u32 i = 0; i < shader_count; ++i ) {
        glDetachShader( program_handle, shaders[i] );
    }

    GLint link_status = 0;
    glGetProgramiv(
        program_handle,
        GL_LINK_STATUS,
        &link_status
    );

    if( !link_status ) {
        // TODO(alicia): don't heap allocate!
        GLint info_log_length = 0;
        glGetProgramiv(
            program_handle,
            GL_INFO_LOG_LENGTH,
            &info_log_length
        );
        char* info_log_buffer = (char*)mem_alloc(
            info_log_length,
            MEMTYPE_RENDERER
        );
        glGetProgramInfoLog(
            program_handle,
            info_log_length,
            nullptr,
            info_log_buffer
        );
        GL_LOG_ERROR( "Linking Error:\n%s", info_log_buffer );
        mem_free( info_log_buffer );
        return false;
    }

    glGetProgramiv(
        program_handle,
        GL_ACTIVE_UNIFORMS,
        &out_shader_program->uniform_count
    );
    out_shader_program->handle = program_handle;

    return true;
}
b32 gl_shader_program_reflection( ShaderProgram* shader_program ) {
    glGetProgramiv(
        shader_program->handle,
        GL_ACTIVE_UNIFORM_MAX_LENGTH,
        &shader_program->uniform_name_max_length
    );

    UniformInfo* uniforms_buffer = (UniformInfo*)mem_alloc(
        sizeof(UniformInfo) * shader_program->uniform_count,
        MEMTYPE_RENDERER
    );
    char* uniform_names_buffer = (char*)mem_alloc(
        shader_program->uniform_name_max_length *
            shader_program->uniform_count,
        MEMTYPE_RENDERER
    );

    if( !uniforms_buffer || !uniform_names_buffer ) {
        GL_LOG_ERROR( "Failed to allocate uniform info buffer!" );
        return false;
    }

    shader_program->uniforms      = uniforms_buffer;
    shader_program->uniform_names = uniform_names_buffer;

    for( i32 i = 0; i < shader_program->uniform_count; ++i ) {
        UniformInfo* current_info = &shader_program->uniforms[i];
        current_info->name = &shader_program->uniform_names[
            i * shader_program->uniform_name_max_length
        ];

        glGetActiveUniform(
            shader_program->handle,
            i,
            shader_program->uniform_name_max_length,
            nullptr,
            &current_info->location_count,
            &current_info->type,
            (GLchar*)current_info->name
        );

        current_info->location = glGetUniformLocation(
            shader_program->handle,
            current_info->name
        );
    }

    return true;
}
UniformInfo* gl_shader_program_uniform_info(
    ShaderProgram* shader_program,
    const char* uniform_name
) {
    for( GLint i = 0; i < shader_program->uniform_count; ++i ) {
        UniformInfo* current_uniform = &shader_program->uniforms[i];
        if( str_cmp( uniform_name, current_uniform->name ) ) {
            return current_uniform;
        }
    }
    return nullptr;
}
void gl_shader_delete( Shader shader ) {
    glDeleteShader( shader );
}
void gl_shader_program_delete( ShaderProgram* program ) {
    glDeleteProgram( program->handle );
    if( program->uniform_names ) {
        mem_free( program->uniform_names );
    }
    if( program->uniforms ) {
        mem_free( program->uniforms );
    }
    *program = {};
}
