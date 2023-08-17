#if !defined(RENDERER_OPENGL_GL_SHADER_HPP)
#define RENDERER_OPENGL_GL_SHADER_HPP
/**
 * Description:  OpenGL Shader
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
*/
#include "gl_types.h"

/// OpenGL Shader Uniform Information
typedef struct UniformInfo {
    const char* name;
    GLint       location;
    GLenum      type;
    GLsizei     location_count;
} UniformInfo;
/// OpenGL Shader Program
typedef struct ShaderProgram {
    GLuint handle;
    char*        uniform_names;
    UniformInfo* uniforms;
    GLint        uniform_name_max_length;
    GLint        uniform_count;
} ShaderProgram;
typedef GLuint Shader;

/// Compile SPIR-V Shader
b32 gl_shader_compile(
    u32           spirv_binary_size,
    void*         spirv_binary_buffer,
    GLenum        shader_type,
    const char*   shader_entry_point,
    GLuint        num_specialization_constants,
    const GLuint* constant_index,
    const GLuint* constant_value,
    Shader*       out_shader
);
/// Link shaders.
b32 gl_shader_program_link(
    u32            shader_count,
    Shader*        shaders,
    ShaderProgram* out_shader_program
);
/// Collect information about shader program uniforms.
b32 gl_shader_program_reflection( ShaderProgram* shader_program );
/// Get information about the specified uniform.
/// Returns null if uniform was not found.
UniformInfo* gl_shader_program_uniform_info(
    ShaderProgram* shader_program,
    const char* uniform_name
);
/// Delete shaders.
void gl_shader_delete( Shader shader );
/// Delete shader program.
void gl_shader_program_delete( ShaderProgram* program );

#endif // header guard
