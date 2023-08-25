#if !defined(LD_RENDERER_OPENGL_SHADER_HPP)
#define LD_RENDERER_OPENGL_SHADER_HPP
// * Description:  OpenGL Shaders
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 19, 2023
#include "defines.h"
#include "ldrenderer/opengl/types.h"

typedef GLuint GLShaderProgram;
typedef GLuint GLShader;
typedef GLenum GLShaderStage;

/// Compile shader from source.
b32 gl_shader_compile_source(
    GLint         source_length,
    const char*   source,
    GLShaderStage shader_stage,
    GLShader*     out_shader
);
/// Compile SPIR-V Shader.
b32 gl_shader_compile_spirv(
    usize         spirv_binary_size,
    void*         spirv_binary_buffer,
    GLShaderStage shader_stage,
    const char*   shader_entry_point,
    GLuint        num_specialization_constants,
    const GLuint* constant_index,
    const GLuint* constant_value,
    GLShader*     out_shader
);
/// Link OpenGL Shaders.
b32 gl_shader_program_link(
    usize            shader_count,
    GLShader*        shaders,
    GLShaderProgram* out_shader_program
);
/// Delete shaders.
void gl_shader_delete( usize shader_count, GLShader* shaders );
/// Delete shader programs.
void gl_shader_program_delete(
    usize shader_program_count, GLShaderProgram* shader_programs );

#endif // header guard
