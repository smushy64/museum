#if !defined(LD_CORE_GRAPHICS_INTERNAL_OPENGL_SHADER_H)
#define LD_CORE_GRAPHICS_INTERNAL_OPENGL_SHADER_H
// * Description:  OpenGL Shaders
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 24, 2023
#include "defines.h"

#if defined(LD_API_INTERNAL)
#include "core/graphics/internal/opengl/types.h"

/// Compile GLSL Shaders from source.
b32 gl_shader_compile_source(
    GLint         source_len,
    const char*   source,
    GLShaderStage stage,
    GLShaderID*   out_shader
);
/// Compiler shader from SPIR-V.
b32 gl_shader_compile_spirv(
    usize         buffer_size,
    void*         buffer,
    GLShaderStage stage,
    const char*   entry_point,
    GLuint        specialization_constant_count,
    const GLuint* constant_index,
    const GLuint* constant_value,
    GLShaderID*   out_shader
);
/// Link shaders into shader program.
b32 gl_shader_program_link(
    usize count, GLShaderID* shaders, GLShaderProgramID* out_shader_program );
/// Delete shaders.
void gl_shader_delete( usize count, GLShaderID* shaders );
/// Delete shader programs.
void gl_shader_program_delete( usize count, GLShaderProgramID* programs );

#endif // API internal

#endif // header guard
