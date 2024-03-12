/**
 * Description:  OpenGL functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 07, 2024
*/
#include "shared/defines.h"

#include "unpack/gl_functions.h"
#include "unpack/error.h"

#include "media/surface.h"

#include "core/string.h"
#include "core/memory.h"

const char global_texture_shader_vert[] =
"#version 450 core\n"
"in layout(location = 0) vec4 v_vert;\n"
"out layout(location = 0) struct Vert2Frag {\n"
"   vec2 uv;\n"
"} v2f;\n"
// "uniform layout(location = 0) mat4 u_transform;\n"
"void main() {\n"
"   v2f.uv = v_vert.zw;\n"
"   gl_Position = vec4( v_vert.xy, 0.0, 1.0 );\n"
// "   gl_Position = u_transform * vec4( v_vert.xy, 0.0, 1.0 );\n"
"}\n"
;
usize global_texture_shader_vert_len = sizeof(global_texture_shader_vert) - 1;

const char global_texture_shader_frag[] =
"#version 450 core\n"
"in layout(location = 0) struct Vert2Frag {\n"
"   vec2 uv;\n"
"} v2f;\n"
"uniform layout(binding = 0) sampler2D u_texture;\n"
"out layout(location = 0) vec4 FRAG_COLOR;\n"
"void main() {\n"
// "   FRAG_COLOR = vec4(1.0);\n"
"   FRAG_COLOR = texture( u_texture, v2f.uv );\n"
"}\n"
;
usize global_texture_shader_frag_len = sizeof(global_texture_shader_frag) - 1;

f32 global_quad_verts[] = {
    /* top left */     -0.5f,  0.5f, 0.0f, 1.0f,
    /* bottom left */  -0.5f, -0.5f, 0.0f, 0.0f,
    /* top right */     0.5f,  0.5f, 1.0f, 1.0f,
    /* bottom right */  0.5f, -0.5f, 1.0f, 0.0f,
};
usize global_quad_vert_count = static_array_count( global_quad_verts );

u32 global_quad_indices[] = {
    /* left tri  */ 0, 2, 1,
    /* right tri */ 1, 3, 2,
};
usize global_quad_index_count = static_array_count( global_quad_indices );

#define glfn(fn)\
    fn##FN* ___internal_##fn = NULL

glfn( glCreateTextures );
glfn( glTextureParameteri );
glfn( glGenerateTextureMipmap );
glfn( glTextureStorage2D );
glfn( glTextureStorage3D );
glfn( glTextureSubImage2D );
glfn( glTextureSubImage3D );
glfn( glBindTextureUnit );
glfn( glClear );
glfn( glCreateShader );
glfn( glCreateProgram );
glfn( glShaderSource );
glfn( glCompileShader );
glfn( glGetShaderiv );
glfn( glGetShaderInfoLog );
glfn( glAttachShader );
glfn( glDetachShader );
glfn( glLinkProgram );
glfn( glGetProgramiv );
glfn( glGetProgramInfoLog );
glfn( glProgramUniform1i );
glfn( glProgramUniformMatrix4fv );
glfn( glUseProgram );
glfn( glDeleteProgram );
glfn( glDeleteTextures );
glfn( glCreateVertexArrays );
glfn( glCreateBuffers );
glfn( glDrawElements );
glfn( glNamedBufferData );
glfn( glVertexArrayVertexBuffer );
glfn( glVertexArrayElementBuffer );
glfn( glEnableVertexArrayAttrib );
glfn( glVertexArrayAttribFormat );
glfn( glVertexArrayAttribBinding );
glfn( glViewport );
glfn( glClearColor );
glfn( glBindVertexArray );

b32 gl_load_functions(void) {
    #define load( fn ) do {\
        ___internal_##fn = (fn##FN*)media_gl_load_proc( #fn );\
        if( !___internal_##fn ) {\
            error( "failed to load function '" #fn "'!" );\
            return false;\
        }\
    } while(0)

    load( glCreateTextures );
    load( glTextureParameteri );
    load( glGenerateTextureMipmap );
    load( glTextureStorage2D );
    load( glTextureStorage3D );
    load( glTextureSubImage2D );
    load( glTextureSubImage3D );
    load( glBindTextureUnit );
    load( glClear );
    load( glCreateShader );
    load( glCreateProgram );
    load( glShaderSource );
    load( glCompileShader );
    load( glGetShaderiv );
    load( glGetShaderInfoLog );
    load( glAttachShader );
    load( glDetachShader );
    load( glLinkProgram );
    load( glGetProgramiv );
    load( glGetProgramInfoLog );
    load( glProgramUniform1i );
    load( glProgramUniformMatrix4fv );
    load( glUseProgram );
    load( glDeleteProgram );
    load( glDeleteTextures );
    load( glCreateVertexArrays );
    load( glCreateBuffers );
    load( glDrawElements );
    load( glNamedBufferData );
    load( glVertexArrayVertexBuffer );
    load( glVertexArrayElementBuffer );
    load( glEnableVertexArrayAttrib );
    load( glVertexArrayAttribFormat );
    load( glVertexArrayAttribBinding );
    load( glViewport );
    load( glClearColor );
    load( glBindVertexArray );

    #undef load
    return true;
}

b32 gl_compile_shader(
    GLint source_len, const char* source, GLenum stage, GLuint* out_shader
) {
    GLuint shader = glCreateShader( stage );
    if( !shader ) {
        return false;
    }

    glShaderSource( shader, 1, &source, &source_len );
    glCompileShader( shader );

    GLint status = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &status );

    if( status ) {
        *out_shader = shader;
        return true;
    }

    GLint max_len = kilobytes(1);
    char* msg     = system_alloc( max_len );
    assert( msg );
    GLint log_len = 0;
    glGetShaderInfoLog( shader, max_len, &log_len, msg );

    StringSlice msg_slice = { msg, log_len };
    error( "failed to compile shader!" );
    error( "{s}", msg_slice );

    system_free( msg, max_len );

    return false;
}
b32 gl_link_shader( usize count, GLuint* shaders, GLuint* out_shader_program ) {
    GLuint program = glCreateProgram();
    if( !program ) {
        return false;
    }

    for( usize i = 0; i < count; ++i ) {
        glAttachShader( program, shaders[i] );
    }

    glLinkProgram( program );

    for( usize i = 0; i < count; ++i ) {
        glDetachShader( program, shaders[i] );
    }

    GLint status = 0;
    glGetProgramiv( program, GL_LINK_STATUS, &status );

    if( status ) {
        *out_shader_program = program;
        return true;
    }

    GLint max_len = kilobytes(1);
    char* msg     = system_alloc( max_len );
    assert( msg );
    GLint log_len = 0;
    glGetProgramInfoLog( program, max_len, &log_len, msg );

    StringSlice msg_slice = { msg, log_len };
    error( "failed to compile shader!" );
    error( "{s}", msg_slice );

    system_free( msg, max_len );

    return false;
}



#undef glfn

