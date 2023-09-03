// * Description:  OpenGL Shaders Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 19, 2023
#include "ldrenderer/opengl/shader.h"
#include "ldrenderer/opengl/functions.h"
#include "ldrenderer/ldopengl.h"
#include "core/ldmemory.h"

#define MAX_INFO_LOG_BUFFER_LEN (256)
char INFO_LOG_BUFFER[MAX_INFO_LOG_BUFFER_LEN];

internal const char* shader_stage_to_string( GLShaderStage stage ) {
    switch( stage ) {
        case GL_VERTEX_SHADER: return "Vertex";
        case GL_FRAGMENT_SHADER: return "Fragment";
        case GL_TESS_CONTROL_SHADER: return "Tesselation Control";
        case GL_TESS_EVALUATION_SHADER: return "Tesselation Evaluation";
        case GL_GEOMETRY_SHADER: return "Geometry";
        case GL_COMPUTE_SHADER: return "Compute";
        default: return "Unknown";
    }
}

internal void gl_log_compilation_error(
    GLShaderStage stage, GLShaderID shader
) {
    GLint info_log_len = 0;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &info_log_len );

    GLint written_info_log_len = 0;
    glGetShaderInfoLog(
        shader,
        MAX_INFO_LOG_BUFFER_LEN,
        &written_info_log_len,
        INFO_LOG_BUFFER
    );

    if( info_log_len != written_info_log_len + 1 ) {
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

    GL_LOG_ERROR(
        "Shader Compilation Error! "
        "Shader Stage: \"{cc}\"",
        shader_stage_to_string( stage )
    );
    GL_LOG_ERROR( "{cc}", INFO_LOG_BUFFER );
}

internal void gl_log_linking_error( GLShaderProgramID shader_program ) {
    GLint info_log_len = 0;
    glGetProgramiv( shader_program, GL_INFO_LOG_LENGTH, &info_log_len );

    GLint written_info_log_len = 0;
    glGetProgramInfoLog(
        shader_program,
        MAX_INFO_LOG_BUFFER_LEN,
        &written_info_log_len,
        INFO_LOG_BUFFER
    );

    if( info_log_len != written_info_log_len + 1 ) {
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
    GLShaderID*     out_shader
) {
    GLShaderID shader = glCreateShader( shader_stage );
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

    gl_log_compilation_error( shader_stage, shader );

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
    GLShaderID*     out_shader
) {
    GLShaderID shader = glCreateShader( shader_stage );
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

    gl_log_compilation_error( shader_stage, shader );

    return false;
}
b32 gl_shader_program_link(
    usize              shader_count,
    GLShaderID*        shaders,
    GLShaderProgramID* out_shader_program
) {
    GLShaderProgramID shader_program = glCreateProgram();
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
void gl_shader_delete( usize shader_count, GLShaderID* shaders ) {
    for( usize i = 0; i < shader_count; ++i ) {
        glDeleteShader( shaders[i] );
    }
}
void gl_shader_program_delete(
    usize shader_program_count, GLShaderProgramID* shader_programs
) {
    for( usize i = 0; i < shader_program_count; ++i ) {
        glDeleteProgram( shader_programs[i] );
    }
}

const char* GL_FRAMEBUFFER_SHADER_VERT_SOURCE =
    "#version 460 core\n"
    "in layout(location = 0) vec2 v_vertex;\n"
    "in layout(location = 1) vec2 v_uv;\n"
    "out layout(location = 0) vec2 v2f_uv;\n"
    "void main() {\n"
    "   gl_Position = vec4( v_vertex, 0.0, 1.0 );\n"
    "   v2f_uv = v_uv;\n"
    "}\n";
usize GL_FRAMEBUFFER_SHADER_VERT_SOURCE_LENGTH = 216;

const char* GL_FRAMEBUFFER_SHADER_FRAG_SOURCE =
    "#version 460 core\n"
    "in layout(location = 0) vec2 v2f_uv;\n"
    "uniform layout(binding = 0) sampler2D u_render_texture;\n"
    "out layout(location = 0) vec4 FRAG_COLOR;\n"
    "void main() {\n"
    "   vec4 texture_color = texture( u_render_texture, v2f_uv );\n"
    "   FRAG_COLOR = texture_color;\n"
    "}\n";
usize GL_FRAMEBUFFER_SHADER_FRAG_SOURCE_LENGTH = 262;

