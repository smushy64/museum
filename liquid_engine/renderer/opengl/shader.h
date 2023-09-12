#if !defined(LD_RENDERER_OPENGL_SHADER_HPP)
#define LD_RENDERER_OPENGL_SHADER_HPP
// * Description:  OpenGL Shaders
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 19, 2023
#include "defines.h"
#include "renderer/opengl/types.h"

/// Embedded framebuffer shader vertex source.
extern const char* GL_FRAMEBUFFER_SHADER_VERT_SOURCE;
/// Embedded framebuffer shader vertex source length.
extern usize GL_FRAMEBUFFER_SHADER_VERT_SOURCE_LENGTH;

/// Embedded framebuffer shader fragment source.
extern const char* GL_FRAMEBUFFER_SHADER_FRAG_SOURCE;
/// Embedded framebuffer shader fragment source length.
extern usize GL_FRAMEBUFFER_SHADER_FRAG_SOURCE_LENGTH;

/// Compile shader from source.
b32 gl_shader_compile_source(
    GLint         source_length,
    const char*   source,
    GLShaderStage shader_stage,
    GLShaderID*   out_shader
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
    GLShaderID*     out_shader
);
/// Link OpenGL Shaders.
b32 gl_shader_program_link(
    usize              shader_count,
    GLShaderID*        shaders,
    GLShaderProgramID* out_shader_program
);
/// Delete shaders.
void gl_shader_delete( usize shader_count, GLShaderID* shaders );
/// Delete shader programs.
void gl_shader_program_delete(
    usize shader_program_count, GLShaderProgramID* shader_programs );

#endif // header guard
