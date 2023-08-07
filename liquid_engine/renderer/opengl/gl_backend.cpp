/**
 * Description:  OpenGL Backend Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 23, 2023
*/
#include "gl_backend.h"
#include "gl_functions.h"
#include "gl_buffer.h"
#include "renderer/primitives.h"

#include "platform/platform.h"
#include "core/memory.h"
#include "core/collections.h"
#include "core/time.h"
#include "core/math.h"
#include "core/graphics.h"

#define GL_DEFAULT_CLEAR_COLOR 0.0f, 0.0f, 0.0f, 0.0f

global GLuint NULL_TEXTURE = 0;

void gl_debug_callback (
    GLenum ,       // source
    GLenum ,       // type
    GLuint ,       // id
    GLenum ,       // severity
    GLsizei,       // message length
    const GLchar*, // message
    const void*    // userParam
);
internal void gl_make_texture( Texture* texture );
internal void gl_make_mesh( Mesh* mesh );

#if defined(DEBUG)
void gl_make_debug_text_shader( OpenGLRendererContext* ctx ) {
    ctx->font_mesh.vertices_2d    = (Vertex2D*)FONT_QUAD_2D;
    ctx->font_mesh.vertex_count   = STATIC_ARRAY_COUNT( FONT_QUAD_2D );
    ctx->font_mesh.indices8       = (u8*)FONT_QUAD_2D_INDICES;
    ctx->font_mesh.index_count    = STATIC_ARRAY_COUNT( FONT_QUAD_2D_INDICES );
    ctx->font_mesh.vertex_type    = VERTEX_TYPE_2D;
    ctx->font_mesh.index_type     = INDEX_TYPE_U8;
    ctx->font_mesh.is_static_mesh = true;

    gl_make_mesh( &ctx->font_mesh );

    PlatformFileHandle vert_file = {}, frag_file = {};
    ASSERT( platform_file_open(
        "./resources/shaders/debug_text.vert.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ     |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &vert_file
    ) );
    ASSERT( platform_file_open(
        "./resources/shaders/debug_text.frag.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ     |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &frag_file
    ) );

    u32 vert_binary_size =
        platform_file_query_size( &vert_file );
    void* vert_binary =
        mem_alloc( vert_binary_size, MEMTYPE_RENDERER );
    ASSERT(vert_binary);
    u32 frag_binary_size =
        platform_file_query_size( &frag_file );
    void* frag_binary =
        mem_alloc( frag_binary_size, MEMTYPE_RENDERER );
    ASSERT( frag_binary );

    ASSERT( platform_file_read(
        &vert_file,
        vert_binary_size,
        vert_binary_size,
        vert_binary
    ) );
    ASSERT( platform_file_read(
        &frag_file,
        frag_binary_size,
        frag_binary_size,
        frag_binary
    ) );
    platform_file_close( &vert_file );
    platform_file_close( &frag_file );

    Shader shaders[2] = {};

    ASSERT( gl_shader_compile(
        vert_binary_size,
        vert_binary,
        GL_VERTEX_SHADER,
        "main",
        0,
        0,
        0,
        &shaders[0]
    ) );
    ASSERT( gl_shader_compile(
        frag_binary_size,
        frag_binary,
        GL_FRAGMENT_SHADER,
        "main",
        0,
        0,
        0,
        &shaders[1]
    ) );
    ASSERT( gl_shader_program_link(
        2, shaders,
        &ctx->font
    ) );
    gl_shader_delete( shaders[0] );
    gl_shader_delete( shaders[1] );

    mem_free( vert_binary );
    mem_free( frag_binary );
}

void gl_make_debug_shader( OpenGLRendererContext* ctx ) {
    PlatformFileHandle vert_file = {}, frag_file = {};
    ASSERT( platform_file_open(
        "./resources/shaders/debug_draw.vert.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &vert_file
    ) );
    ASSERT( platform_file_open(
        "./resources/shaders/debug_draw.frag.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &frag_file
    ) );

    u32 vert_binary_size =
        platform_file_query_size( &vert_file );
    void* vert_binary = mem_alloc(
        vert_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        &vert_file,
        vert_binary_size,
        vert_binary_size,
        vert_binary
    );

    u32 frag_binary_size =
        platform_file_query_size( &frag_file );
    void* frag_binary = mem_alloc(
        frag_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        &frag_file,
        frag_binary_size,
        frag_binary_size,
        frag_binary
    );

    platform_file_close( &vert_file );
    platform_file_close( &frag_file );

    Shader shaders[2] = {};

    ASSERT( gl_shader_compile(
        vert_binary_size,
        vert_binary,
        GL_VERTEX_SHADER,
        "main",
        0,
        0,
        0,
        &shaders[0]
    ) );
    ASSERT( gl_shader_compile(
        frag_binary_size,
        frag_binary,
        GL_FRAGMENT_SHADER,
        "main",
        0,
        0,
        0,
        &shaders[1]
    ) );
    ASSERT( gl_shader_program_link(
        2, shaders,
        &ctx->debug
    ) );
    gl_shader_delete( shaders[0] );
    gl_shader_delete( shaders[1] );
    mem_free( vert_binary );
    mem_free( frag_binary );

    ctx->debug_color =
        glGetUniformLocation( ctx->debug.handle, "u_color" );
}
#else
void gl_make_debug_shader( OpenGLRendererContext* ctx ) {
    unused(ctx);
}
#endif

void gl_make_sprite_shader( OpenGLRendererContext* ctx ) {

    ctx->sprite_mesh.vertices_2d    = (Vertex2D*)QUAD_2D;
    ctx->sprite_mesh.vertex_count   = STATIC_ARRAY_COUNT( QUAD_2D );
    ctx->sprite_mesh.indices8       = (u8*)QUAD_2D_INDICES;
    ctx->sprite_mesh.index_count    = STATIC_ARRAY_COUNT( QUAD_2D_INDICES );
    ctx->sprite_mesh.vertex_type    = VERTEX_TYPE_2D;
    ctx->sprite_mesh.index_type     = INDEX_TYPE_U8;
    ctx->sprite_mesh.is_static_mesh = true;

    gl_make_mesh( &ctx->sprite_mesh );
    PlatformFileHandle vert_file = {}, frag_file = {};
    ASSERT( platform_file_open(
        "./resources/shaders/sprite.vert.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &vert_file
    ) );
    ASSERT( platform_file_open(
        "./resources/shaders/sprite.frag.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &frag_file
    ) );

    u32 vert_binary_size =
        platform_file_query_size( &vert_file );
    void* vert_binary = mem_alloc(
        vert_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        &vert_file,
        vert_binary_size,
        vert_binary_size,
        vert_binary
    );

    u32 frag_binary_size =
        platform_file_query_size( &frag_file );
    void* frag_binary = mem_alloc(
        frag_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        &frag_file,
        frag_binary_size,
        frag_binary_size,
        frag_binary
    );

    platform_file_close( &vert_file );
    platform_file_close( &frag_file );

    Shader shaders[2] = {};

    ASSERT( gl_shader_compile(
        vert_binary_size,
        vert_binary,
        GL_VERTEX_SHADER,
        "main",
        0,
        0,
        0,
        &shaders[0]
    ) );
    ASSERT( gl_shader_compile(
        frag_binary_size,
        frag_binary,
        GL_FRAGMENT_SHADER,
        "main",
        0,
        0,
        0,
        &shaders[1]
    ) );
    ASSERT( gl_shader_program_link(
        2, shaders,
        &ctx->sprite
    ) );
    gl_shader_delete( shaders[0] );
    gl_shader_delete( shaders[1] );
    mem_free( vert_binary );
    mem_free( frag_binary );
    ASSERT( gl_shader_program_reflection( &ctx->sprite ) );

    ctx->sprite_transform =
        glGetUniformLocation( ctx->sprite.handle, "u_transform" );
    ctx->sprite_atlas_coordinate =
        glGetUniformLocation( ctx->sprite.handle, "u_atlas_coordinate" );
    ctx->sprite_flip =
        glGetUniformLocation( ctx->sprite.handle, "u_flip" );
    ctx->sprite_atlas_cell_size =
        glGetUniformLocation( ctx->sprite.handle, "u_atlas_cell_size" );
    ctx->sprite_tint =
        glGetUniformLocation( ctx->sprite.handle, "u_tint" );
    ctx->sprite_z_index =
        glGetUniformLocation( ctx->sprite.handle, "u_z_index" );

}

b32 gl_renderer_backend_initialize( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    generic_ctx->backend_shutdown    = gl_renderer_backend_shutdown;
    generic_ctx->backend_on_resize   = gl_renderer_backend_on_resize;
    generic_ctx->backend_begin_frame = gl_renderer_backend_begin_frame;
    generic_ctx->backend_end_frame   = gl_renderer_backend_end_frame;

    void* glrc = platform_gl_init( generic_ctx->platform );
    if( !glrc ) {
        return false;
    }

    ctx->glrc = glrc;

#if defined( LD_LOGGING ) && defined( DEBUG )
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(
        gl_debug_callback,
        nullptr
    );
#endif

    const char* device_vendor  = (const char*)glGetString( GL_VENDOR );
    const char* device_name    = (const char*)glGetString( GL_RENDERER );
    const char* device_version = (const char*)glGetString( GL_VERSION );
    const char* device_glsl_version = (const char*)glGetString(
        GL_SHADING_LANGUAGE_VERSION
    );

    GL_LOG_NOTE( "Device Vendor:         {cc}", device_vendor );
    GL_LOG_NOTE( "Device Name:           {cc}", device_name );
    GL_LOG_NOTE( "Device Driver Version: {cc}", device_version );
    GL_LOG_NOTE( "Device GLSL Version:   {cc}", device_glsl_version );

    ctx->device_vendor       = device_vendor;
    ctx->device_name         = device_name;
    ctx->device_version      = device_version;
    ctx->device_glsl_version = device_glsl_version;
    
    i32 extension_count = 0;
    glGetIntegerv( GL_NUM_EXTENSIONS, &extension_count );
    GL_LOG_NOTE( "Supported extensions count: {i}", extension_count );

    ctx->extension_count = extension_count;

    glClearColor( GL_DEFAULT_CLEAR_COLOR );
    glClear( GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT
    );
    platform_gl_swap_buffers( generic_ctx->platform );

    ivec2 dimensions = {
        generic_ctx->platform->surface.width,
        generic_ctx->platform->surface.height
    };

    glViewport(
        0, 0,
        dimensions.width,
        dimensions.height
    );

    // TODO(alicia): TEST CODE!!!!!
    glCreateTextures( GL_TEXTURE_2D, 1, &NULL_TEXTURE );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTextureParameteri( NULL_TEXTURE, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTextureStorage2D(
        NULL_TEXTURE,
        1, GL_RGBA8,
        1, 1
    );
    u32 null_texture_pixel = RGBA_U32( 255, 255, 255, 255 );
    glTextureSubImage2D(
        NULL_TEXTURE,
        0,
        0, 0,
        1, 1,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        &null_texture_pixel
    );

    glCreateBuffers( 1, &ctx->u_matrices );
    f32 aspect_ratio =
        (f32)dimensions.width /
        (f32)dimensions.height;
    mat4 view_projections[2];
    view_projections[0] = lookat(
        VEC3::FORWARD,
        VEC3::ZERO,
        VEC3::UP
    ) * ortho(
        -aspect_ratio,
        aspect_ratio,
        -1.0f,
        1.0f,
        -100.0f,
        100.0f
    );
    view_projections[1] = lookat(
        VEC3::BACK,
        VEC3::ZERO,
        VEC3::UP
    ) * ortho(
        0.0f,
        (f32)dimensions.width,
        0.0f,
        (f32)dimensions.height
    );
    glNamedBufferStorage(
        ctx->u_matrices,
        sizeof(mat4) * 2,
        view_projections,
        GL_DYNAMIC_STORAGE_BIT
    );
    ctx->viewport = v2(dimensions);
    glBindBufferBase(
        GL_UNIFORM_BUFFER,
        0,
        ctx->u_matrices
    );

    PlatformFileHandle phong_vert_file = {}, phong_frag_file = {};
    if( !platform_file_open(
        "./resources/shaders/phong.vert.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &phong_vert_file
    ) ) {
        return false;
    }
    if( !platform_file_open(
        "./resources/shaders/phong.frag.spv",
        PLATFORM_FILE_OPEN_EXISTING |
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ,
        &phong_frag_file
    ) ) {
        return false;
    }

    u32 phong_vert_binary_size =
        platform_file_query_size( &phong_vert_file );
    void* phong_vert_binary = mem_alloc(
        phong_vert_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        &phong_vert_file,
        phong_vert_binary_size,
        phong_vert_binary_size,
        phong_vert_binary
    );

    u32 phong_frag_binary_size =
        platform_file_query_size( &phong_frag_file );
    void* phong_frag_binary = mem_alloc(
        phong_frag_binary_size,
        MEMTYPE_RENDERER
    );
    platform_file_read(
        &phong_frag_file,
        phong_frag_binary_size,
        phong_frag_binary_size,
        phong_frag_binary
    );

    platform_file_close( &phong_vert_file );
    platform_file_close( &phong_frag_file );

    Shader phong_shaders[2] = {};

    if( !gl_shader_compile(
        phong_vert_binary_size,
        phong_vert_binary,
        GL_VERTEX_SHADER,
        "main",
        0,
        0,
        0,
        &phong_shaders[0]
    ) ) {
        return false;
    }
    if( !gl_shader_compile(
        phong_frag_binary_size,
        phong_frag_binary,
        GL_FRAGMENT_SHADER,
        "main",
        0,
        0,
        0,
        &phong_shaders[1]
    ) ) {
        return false;
    }
    if( !gl_shader_program_link(
        2, phong_shaders,
        &ctx->phong
    ) ) {
        return false;
    }
    gl_shader_delete( phong_shaders[0] );
    gl_shader_delete( phong_shaders[1] );
    mem_free( phong_vert_binary );
    mem_free( phong_frag_binary );
    if( !gl_shader_program_reflection( &ctx->phong ) ) {
        return false;
    }

    gl_make_debug_shader( ctx );
    gl_make_sprite_shader( ctx );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

#if defined(DEBUG)
    glCreateVertexArrays( 1, &ctx->debug_vao );
    glCreateBuffers( 1, &ctx->debug_vbo );

    glNamedBufferData(
        ctx->debug_vbo,
        MAX_DEBUG_POINTS * sizeof(vec2),
        nullptr,
        GL_DYNAMIC_DRAW
    );
    glVertexArrayVertexBuffer(
        ctx->debug_vao, 0,
        ctx->debug_vbo, 0,
        sizeof(vec2)
    );
    glEnableVertexArrayAttrib( ctx->debug_vao, 0 );
    glVertexArrayAttribFormat(
        ctx->debug_vao, 0,
        2, GL_FLOAT,
        GL_FALSE,
        0
    );
    glVertexArrayAttribBinding( ctx->debug_vao, 0, 0 );
#endif

    ASSERT( debug_font_create(
        "./resources/test.ttf",
        64.0f,
        &ctx->font_data
    ) );
    gl_make_debug_text_shader( ctx );
    gl_make_texture( &ctx->font_data.texture );

    GL_LOG_INFO("OpenGL backend initialized successfully.");
    return true;
}
void gl_renderer_backend_shutdown( RendererContext* generic_ctx ) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    // TODO(alicia): TEST CODE ONLY

    glDeleteTextures( 1, &NULL_TEXTURE );
    GLuint font_texture_id = ctx->font_data.texture.id.id();
    glDeleteTextures( 1, &font_texture_id );

    glDeleteBuffers( 1, &ctx->u_matrices );
    gl_shader_program_delete( &ctx->sprite );
    gl_shader_program_delete( &ctx->phong );

    debug_font_destroy( &ctx->font_data );

    // TODO(alicia): END TEST CODE ONLY

    platform_gl_shutdown(
        ctx->ctx.platform,
        ctx->glrc
    );

    GL_LOG_INFO("OpenGL backend shutdown.");
}
void gl_renderer_backend_on_resize(
    RendererContext* generic_ctx,
    i32 width, i32 height
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    ctx->viewport = v2( (f32)width, (f32)height );

    glViewport( 0, 0, width, height );
    f32 aspect_ratio = (f32)width / (f32)height;
    mat4 view_projections[2];
    view_projections[0] = lookat(
        VEC3::FORWARD,
        VEC3::ZERO,
        VEC3::UP
    ) * ortho(
        -aspect_ratio,
        aspect_ratio,
        -1.0f,
        1.0f,
        -100.0f,
        100.0f
    );
    view_projections[1] = lookat(
        VEC3::BACK,
        VEC3::ZERO,
        VEC3::UP
    ) * ortho(
        0.0f,
        (f32)width,
        0.0f,
        (f32)height
    );
    glNamedBufferSubData(
        ctx->u_matrices,
        0, sizeof(mat4) * 2,
        view_projections
    );
}

[[maybe_unused]]
internal void gl_make_texture( Texture* texture ) {
    if( texture->id.is_valid() ) {
        return;
    }
    if( texture->format == TEXTURE_FORMAT_RED ) {
        glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    }
    GLuint handle = 0;
    glCreateTextures( GL_TEXTURE_2D, 1, &handle );

    GLenum wrap_s, wrap_t;
    switch( texture->wrap_x ) {
        case TEXTURE_WRAP_CLAMP:
            wrap_s = GL_CLAMP_TO_EDGE;
            break;
        case TEXTURE_WRAP_REPEAT:
            wrap_s = GL_REPEAT;
            break;
    }
    switch( texture->wrap_y ) {
        case TEXTURE_WRAP_CLAMP:
            wrap_t = GL_CLAMP_TO_EDGE;
            break;
        case TEXTURE_WRAP_REPEAT:
            wrap_t = GL_REPEAT;
            break;
    }

    GLenum min_filter, mag_filter;
    switch( texture->filter ) {
        case TEXTURE_FILTER_NEAREST:
            min_filter = GL_NEAREST;
            mag_filter = GL_NEAREST;
            break;
        case TEXTURE_FILTER_BILINEAR:
            min_filter = GL_LINEAR;
            mag_filter = GL_LINEAR;
            break;
    }

    glTextureParameteri( handle, GL_TEXTURE_WRAP_S, wrap_s );
    glTextureParameteri( handle, GL_TEXTURE_WRAP_T, wrap_t );
    glTextureParameteri( handle, GL_TEXTURE_MIN_FILTER, min_filter );
    glTextureParameteri( handle, GL_TEXTURE_MAG_FILTER, mag_filter );
    

    GLenum internal_format = {};
    switch( texture->format ) {
        case TEXTURE_FORMAT_RGB:
            internal_format = GL_RGB8;
            break;
        case TEXTURE_FORMAT_RGBA:
            internal_format = GL_RGBA8;
            break;
        case TEXTURE_FORMAT_RED:
            internal_format = GL_R8;
            break;
    }

    glTextureStorage2D(
        handle,
        1, internal_format,
        texture->width, texture->height
    );

    GLenum format = {};
    switch( texture->format ) {
        case TEXTURE_FORMAT_RGB:
            format = GL_RGB;
            break;
        case TEXTURE_FORMAT_RGBA:
            format = GL_RGBA;
            break;
        case TEXTURE_FORMAT_RED:
            format = GL_RED;
            break;
    }

    glTextureSubImage2D(
        handle,
        0,
        0, 0,
        texture->width, texture->height,
        format,
        GL_UNSIGNED_BYTE,
        texture->buffer
    );

    texture->id = handle;

    if( texture->format == TEXTURE_FORMAT_RED ) {
        glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );
    }

}

internal void gl_make_mesh( Mesh* mesh ) {
    if( mesh->id.is_valid() ) {
        return;
    }
    GLuint vao = 0;
    glCreateVertexArrays( 1, &vao );
    mesh->id = vao;
    GLuint buffers[2];
    glCreateBuffers( 2, buffers );
    GLuint vbo = buffers[0];
    GLuint ebo = buffers[1];

    u32 vertex_size = sizeof(Vertex);
    if( mesh->vertex_type == VERTEX_TYPE_2D ) {
        vertex_size = sizeof(Vertex2D);
    }

    GLenum usage = GL_DYNAMIC_DRAW;
    if( mesh->is_static_mesh ) {
        usage = GL_STATIC_DRAW;
    }

    glNamedBufferData(
        vbo,
        mesh->vertex_count * vertex_size,
        mesh->vertices,
        usage
    );
    glVertexArrayVertexBuffer(
        vao, 0,
        vbo, 0,
        vertex_size
    );
    switch( mesh->vertex_type ) {
        case VERTEX_TYPE_2D: {
            glEnableVertexArrayAttrib( vao, 0 );
            glEnableVertexArrayAttrib( vao, 1 );
            glVertexArrayAttribFormat(
                vao, 0,
                2, GL_FLOAT,
                GL_FALSE,
                0
            );
            glVertexArrayAttribFormat(
                vao, 1,
                2, GL_FLOAT,
                GL_FALSE,
                sizeof(vec2)
            );
            glVertexArrayAttribBinding( vao, 0, 0 );
            glVertexArrayAttribBinding( vao, 1, 0 );
        } break;
        case VERTEX_TYPE_3D: {
            glEnableVertexArrayAttrib( vao, 0 );
            glEnableVertexArrayAttrib( vao, 1 );
            glEnableVertexArrayAttrib( vao, 2 );
            glEnableVertexArrayAttrib( vao, 3 );
            glVertexArrayAttribFormat(
                vao, 0,
                4, GL_FLOAT,
                GL_FALSE,
                0
            );
            glVertexArrayAttribFormat(
                vao, 1,
                3, GL_FLOAT,
                GL_FALSE,
                sizeof(vec4)
            );
            glVertexArrayAttribFormat(
                vao, 2,
                2, GL_FLOAT,
                GL_FALSE,
                sizeof(vec4) + sizeof(vec3)
            );
            glVertexArrayAttribFormat(
                vao, 3,
                3, GL_FLOAT,
                GL_FALSE,
                sizeof(vec4) + sizeof(vec3) + sizeof(vec2)
            );
            glVertexArrayAttribBinding( vao, 0, 0 );
            glVertexArrayAttribBinding( vao, 1, 0 );
            glVertexArrayAttribBinding( vao, 2, 0 );
            glVertexArrayAttribBinding( vao, 3, 0 );
        } break;
        default: PANIC();
    }
    u32 index_size = 0;
    switch( mesh->index_type ) {
        case INDEX_TYPE_U32: {
            index_size = sizeof(u32);
        } break;
        case INDEX_TYPE_U16: {
            index_size = sizeof(u16);
        } break;
        case INDEX_TYPE_U8: {
            index_size = sizeof(u8);
        } break;
        default: PANIC();
    }
    glNamedBufferData(
        ebo,
        index_size * mesh->index_count,
        mesh->indices,
        usage
    );
    glVertexArrayElementBuffer( vao, ebo );
}

b32 gl_renderer_backend_begin_frame(
    RendererContext* generic_ctx,
    RenderOrder* order
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

    // TODO(alicia): 
    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT
    );
    glBindTextureUnit( 0, NULL_TEXTURE );

    if( order->sprites.count ) {
        glBindVertexArray( ctx->sprite_mesh.id.id() );
        glUseProgram( ctx->sprite.handle );
    }

    QueryResultIterator iterator = &order->sprites;
    EntityID id;
    while( iterator.next( &id ) ) {
        Entity*         entity = entity_storage_get( order->storage, id );
        SpriteRenderer* sprite = &entity->sprite_renderer;
        ivec2 flip = { (i32)sprite->flip_x, (i32)sprite->flip_y };
        glProgramUniformMatrix4fv(
            ctx->sprite.handle,
            ctx->sprite_transform,
            1, GL_FALSE,
            value_pointer( entity->matrix )
        );
        glProgramUniform4iv(
            ctx->sprite.handle,
            ctx->sprite_atlas_coordinate, 1,
            value_pointer( sprite->atlas_coordinate )
        );
        glProgramUniform2iv(
            ctx->sprite.handle,
            ctx->sprite_flip, 1,
            value_pointer( flip )
        );
        glProgramUniform1ui(
            ctx->sprite.handle,
            ctx->sprite_atlas_cell_size,
            sprite->atlas_cell_size
        );
        glProgramUniform1i(
            ctx->sprite.handle,
            ctx->sprite_z_index,
            sprite->z_index
        );
        glProgramUniform4fv(
            ctx->sprite.handle,
            ctx->sprite_tint, 1,
            value_pointer( sprite->tint )
        );
        RendererID atlas_id = sprite->atlas->id;
        if( atlas_id.is_valid() ) {
            glBindTextureUnit( 0, atlas_id.id() );
        } else {
            gl_make_texture( sprite->atlas );
            glBindTextureUnit( 0, NULL_TEXTURE );
        }
        GLenum index_type = GL_UNSIGNED_BYTE;
        glDrawElements(
            GL_TRIANGLES,
            ctx->sprite_mesh.index_count,
            index_type,
            nullptr
        );
    }

    glBindVertexArray( ctx->font_mesh.id.id() );
    glUseProgram( ctx->font.handle );
    glBindTextureUnit( 0, ctx->font_data.texture.id.id() );

    for( u32 i = 0; i < order->text_count; ++i ) {
        UIText* text = (UIText*)&order->ui_text[i];
        glProgramUniform4fv(
            ctx->font.handle,
            DEBUG_TEXT_U_COLOR, 1,
            value_pointer( text->color )
        );

        f32 origin_x = text->position.x * ctx->viewport.x;
        f32 origin_y = text->position.y * ctx->viewport.y;

        for( u32 i = 0; i < text->text.len; ++i ) {
            c8 character = text->text.buffer[i];
            GlyphMetrics* metrics = font_data_metrics(
                &ctx->font_data,
                character
            );
            if( !metrics ) {
                continue;
            }

            vec2 char_scale = v2(
                (f32)metrics->pixel_width,
                (f32)metrics->pixel_height
            ) * text->scale;

            f32 left_side_bearing =
                (f32)metrics->pixel_left_bearing * text->scale;
            f32 top_bearing =
                (f32)metrics->pixel_top_bearing * text->scale;

            vec2 char_translate = {
                origin_x + left_side_bearing,
                origin_y - top_bearing
            };

            mat4 transform =
                translate( char_translate ) *
                scale( char_scale );

            glProgramUniformMatrix4fv(
                ctx->font.handle,
                DEBUG_TEXT_U_TRANSFORM,
                1, GL_FALSE,
                value_pointer( transform )
            );

            vec4 font_coordinates = {
                metrics->atlas_coordinate.x,
                metrics->atlas_coordinate.y,
                metrics->atlas_scale.x,
                metrics->atlas_scale.y
            };
            glProgramUniform4fv(
                ctx->font.handle,
                DEBUG_TEXT_U_COORDINATES, 1,
                value_pointer( font_coordinates )
            );

            glDrawElements(
                GL_TRIANGLES,
                ctx->font_mesh.index_count,
                GL_UNSIGNED_BYTE,
                nullptr
            );

            f32 advance =
                (f32)metrics->pixel_advance * text->scale;

            origin_x += advance;

        }

    }

    return true;
}
b32 gl_renderer_backend_end_frame(
    RendererContext* generic_ctx,
    RenderOrder* order
) {
    OpenGLRendererContext* ctx = (OpenGLRendererContext*)generic_ctx;

#if defined(DEBUG)
    glUseProgram( ctx->debug.handle );
    glBindVertexArray( ctx->debug_vao );
    u32 debug_point_count = list_count( order->list_debug_points );
    for( u32 i = 0; i < debug_point_count; ++i ) {
        DebugPoints* current = &order->list_debug_points[i];
        if( !current->list_points ) {
            continue;
        }
        u32 point_count = list_count( current->list_points );
        if( !point_count ) {
            ::impl::_list_free( current->list_points );
            continue;
        }
        glProgramUniform4fv(
            ctx->debug.handle,
            ctx->debug_color, 1,
            value_pointer( current->color )
        );
        glNamedBufferSubData(
            ctx->debug_vbo,
            0, sizeof(vec2) * point_count,
            current->list_points
        );
        glDrawArrays(
            GL_LINE_LOOP,
            0, point_count
        );
        ::impl::_list_free( current->list_points );
    }
#endif

    platform_gl_swap_buffers( ctx->ctx.platform );
    unused(order);
    return true;
}

inline const char* to_string_source( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}
inline const char* to_string_type( GLenum type ) {
    switch( type ) {
        case GL_DEBUG_TYPE_ERROR: return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
        case GL_DEBUG_TYPE_MARKER: return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
        case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
        default: return "Other";
    }
}

void gl_debug_callback (
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei,       // message length
    const GLchar* message,
    const void*    // userParam
) {
    #define GL_DEBUG_MESSAGE_FORMAT \
        "{u32} {cc} {cc} | {cc}",\
        id,\
        to_string_source(source),\
        to_string_type(type),\
        message

    switch( severity ) {
        case GL_DEBUG_SEVERITY_HIGH: {
            GL_LOG_ERROR( GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM: {
            GL_LOG_WARN( GL_DEBUG_MESSAGE_FORMAT );
        } break;
        case GL_DEBUG_SEVERITY_LOW: {
            GL_LOG_INFO( GL_DEBUG_MESSAGE_FORMAT );
        } break;
        default: {
            GL_LOG_NOTE( GL_DEBUG_MESSAGE_FORMAT );
        } break;
    }

}
