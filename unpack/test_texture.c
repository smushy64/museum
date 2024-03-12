/**
 * Description:  Texture testing.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 05, 2024
*/
#include "unpack/error.h"
#include "shared/liquid_package.h"
#include "core/fs.h"
#include "core/print.h"
#include "core/memory.h"
#include "core/compression.h"
#include "core/math.h"

#include "media/lib.h"
#include "media/surface.h"

#include "unpack/gl_functions.h"

internal UnpackError create_surface( MediaSurface* surface, u32 width, u32 height );
internal void destroy_surface( MediaSurface* surface );

global b32 global_texture_surface_is_running = true;
global GLuint global_texture_shader = 0;
void on_texture_surface_event(
    MediaSurface* surface, MediaSurfaceCallbackData* data, void* params
) {
    switch( data->type ) {
        case MEDIA_SURFACE_CALLBACK_TYPE_CLOSE: {
            global_texture_surface_is_running = false;
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_RESOLUTION_CHANGE: {
            if( global_texture_shader ) {

            }
            glViewport(
                0, 0,
                data->resolution_change.new_width,
                data->resolution_change.new_height );
        } break;
        default: break;
    }
    unused(surface, params);
}

UnpackError test_texture(
    FileHandle* package, struct PackageHeader pkg_header,
    struct PackageResource pkg_resource, u32 resource_id
) {
    MediaSurface surface = {};

    fs_file_set_offset(
        package, sizeof(pkg_header) +
        (pkg_header.resource_count * sizeof(pkg_resource)) +
        pkg_resource.offset, false );

    usize texture_size  = pkg_resource.size;
    usize allocate_size = texture_size;
    usize compressed_size = 0;

    if( pkg_resource.compression ) {
        u64 size64 = 0;
        if( !fs_file_read( package, sizeof(size64), &size64 ) ) {
            error( "failed to read package texture!" );
            return UNPACK_ERROR_FILE_READ;
        }

        texture_size  = size64;
        allocate_size += size64;
        compressed_size = pkg_resource.size;
    }
    void* texture_buffer = system_alloc( allocate_size );
    if( !texture_buffer ) {
        error( "failed to allocate {f,m,.2} for texture!", (f64)allocate_size );
        return UNPACK_ERROR_OUT_OF_MEMORY;
    }

    switch( pkg_resource.compression ) {
        case PACKAGE_COMPRESSION_NONE: {
            if( !fs_file_read( package, texture_size, texture_buffer ) ) {
                error( "failed to read package texture!" );
                return UNPACK_ERROR_FILE_READ;
            }
        } break;
        case PACKAGE_COMPRESSION_RLE: {
            void* compressed_buffer = (u8*)texture_buffer + texture_size;
            if( !fs_file_read( package, compressed_size, compressed_buffer ) ) {
                error( "failed to read package texture!" );
                return UNPACK_ERROR_FILE_READ;
            }
            ByteSlice dst = {};
            dst.buffer   = texture_buffer;
            dst.capacity = texture_size;

            compression_rle_decode(
                compression_byte_slice_stream, &dst,
                compressed_size, compressed_buffer, 0 );
        } break;
    }

    GLenum gl_texture_type = 0;
    switch( pkg_resource.texture.type.dimensions ) {
        case PACKAGE_TEXTURE_DIMENSIONS_1:
        case PACKAGE_TEXTURE_DIMENSIONS_2: {
            gl_texture_type = GL_TEXTURE_2D;
        } break;
        case PACKAGE_TEXTURE_DIMENSIONS_3: {
            gl_texture_type = GL_TEXTURE_3D;
        } break;
    }

    i32 width  = pkg_resource.texture.width;
    i32 height = max( pkg_resource.texture.height, 1 );
    i32 depth  = max( pkg_resource.texture.depth, 1 );
    unused(depth);

    u32 surface_width  = 800;
    u32 surface_height = 600;

    println( "creating window to display texture . . ." );
    UnpackError err = create_surface( &surface, surface_width, surface_height );
    if( err ) {
        system_free( texture_buffer, allocate_size );
        return err;
    }

    GLuint texture = 0;
    glCreateTextures( gl_texture_type, 1, &texture );

    PackageTextureFlags flags = pkg_resource.texture.flags;
    if( bitfield_check( flags, PACKAGE_TEXTURE_FLAG_BILINEAR_FILTER ) ) {
        glTextureParameteri( texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTextureParameteri( texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    } else {
        glTextureParameteri( texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTextureParameteri( texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    }

    if( bitfield_check( flags, PACKAGE_TEXTURE_FLAG_WRAP_X_REPEAT ) ) {
        glTextureParameteri( texture, GL_TEXTURE_WRAP_S, GL_REPEAT );
    } else {
        glTextureParameteri( texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    }

    if( pkg_resource.texture.type.dimensions >= PACKAGE_TEXTURE_DIMENSIONS_2 ) {
        if( bitfield_check( flags, PACKAGE_TEXTURE_FLAG_WRAP_Y_REPEAT ) ) {
            glTextureParameteri( texture, GL_TEXTURE_WRAP_T, GL_REPEAT );
        } else {
            glTextureParameteri( texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        }
    }
    if( pkg_resource.texture.type.dimensions >= PACKAGE_TEXTURE_DIMENSIONS_3 ) {
        if( bitfield_check( flags, PACKAGE_TEXTURE_FLAG_WRAP_Z_REPEAT ) ) {
            glTextureParameteri( texture, GL_TEXTURE_WRAP_R, GL_REPEAT );
        } else {
            glTextureParameteri( texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
        }
    }

    GLenum internal_format = 0;
    GLenum format          = 0;
    GLenum base_type       = 0;

    switch( pkg_resource.texture.type.channels ) {
        case PACKAGE_TEXTURE_CHANNEL_R: {
            format = GL_RED;
        } break;
        case PACKAGE_TEXTURE_CHANNEL_RG: {
            format = GL_RG;
        } break;
        case PACKAGE_TEXTURE_CHANNEL_RGB: {
            format = GL_RGB;
        } break;
        case PACKAGE_TEXTURE_CHANNEL_RGBA: {
            format = GL_RGBA;
        } break;
    }

    switch( pkg_resource.texture.type.base_type ) {
        case PACKAGE_TEXTURE_BASE_TYPE_8BIT: {
            base_type = GL_UNSIGNED_BYTE;
            switch( pkg_resource.texture.type.channels ) {
                case PACKAGE_TEXTURE_CHANNEL_R: {
                    internal_format = GL_R8;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RG: {
                    internal_format = GL_RG8;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGB: {
                    internal_format = GL_RGB8;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGBA: {
                    internal_format = GL_RGBA8;
                } break;
            }
        } break;
        case PACKAGE_TEXTURE_BASE_TYPE_16BIT: {
            base_type = GL_UNSIGNED_SHORT;
            switch( pkg_resource.texture.type.channels ) {
                case PACKAGE_TEXTURE_CHANNEL_R: {
                    internal_format = GL_R16;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RG: {
                    internal_format = GL_RG16;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGB: {
                    internal_format = GL_RGB16;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGBA: {
                    internal_format = GL_RGBA16;
                } break;
            }
        } break;
        case PACKAGE_TEXTURE_BASE_TYPE_32BIT: {
            base_type = GL_UNSIGNED_INT;
            switch( pkg_resource.texture.type.channels ) {
                case PACKAGE_TEXTURE_CHANNEL_R: {
                    internal_format = GL_R32UI;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RG: {
                    internal_format = GL_RG32UI;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGB: {
                    internal_format = GL_RGB32UI;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGBA: {
                    internal_format = GL_RGBA32UI;
                } break;
            }
        } break;
        case PACKAGE_TEXTURE_BASE_TYPE_FLOAT: {
            base_type = GL_FLOAT;
            switch( pkg_resource.texture.type.channels ) {
                case PACKAGE_TEXTURE_CHANNEL_R: {
                    internal_format = GL_R32F;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RG: {
                    internal_format = GL_RG32F;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGB: {
                    internal_format = GL_RGB32F;
                } break;
                case PACKAGE_TEXTURE_CHANNEL_RGBA: {
                    internal_format = GL_RGBA32F;
                } break;
            }
        } break;
    }

    switch( pkg_resource.texture.type.dimensions ) {
        case PACKAGE_TEXTURE_DIMENSIONS_1:
            height = 1;
        case PACKAGE_TEXTURE_DIMENSIONS_2: {
            glTextureStorage2D( texture, 1, internal_format, width, height );
            glTextureSubImage2D(
                texture, 0, 0, 0, width, height, format, base_type, texture_buffer );
        } break;
        case PACKAGE_TEXTURE_DIMENSIONS_3: {
            error( "unhandled case for 3d textures!" );
            panic();
        } break;
    }

    glGenerateTextureMipmap( texture );

    GLuint stages[2];
    assert( gl_compile_shader(
        global_texture_shader_vert_len,
        global_texture_shader_vert, GL_VERTEX_SHADER, stages + 0 ) );
    assert( gl_compile_shader(
        global_texture_shader_frag_len,
        global_texture_shader_frag, GL_FRAGMENT_SHADER, stages + 1 ) );

    GLuint shader = 0;
    assert( gl_link_shader( 2, stages, &shader ) );

    glUseProgram( shader );
    glBindTextureUnit( 0, texture );

    GLuint array = 0;
    glCreateVertexArrays( 1, &array );
    GLuint buffers[2] = {};
    GLuint* vbo = buffers;
    GLuint* ebo = buffers + 1;
    glCreateBuffers( 2, buffers );

    glNamedBufferData(
        *vbo, global_quad_vert_count * sizeof(f32),
        global_quad_verts, GL_STATIC_DRAW );
    glVertexArrayVertexBuffer(
        array, 0, *vbo, 0, 4 * sizeof(f32) );

    glNamedBufferData(
        *ebo, global_quad_index_count * sizeof(u32),
        global_quad_indices, GL_STATIC_DRAW );
    glVertexArrayElementBuffer( array, *ebo );

    glEnableVertexArrayAttrib( array, 0 );
    glVertexArrayAttribFormat( array, 0, 4, GL_FLOAT, GL_FALSE, 0 );
    glVertexArrayAttribBinding( array, 0, 0 );

    glBindVertexArray( array );

    glClearColor( 1.0, 0.0, 1.0, 1.0 );
    glViewport( 0, 0, surface_width, surface_height );
    unused( package, pkg_header, pkg_resource, resource_id );
    while( global_texture_surface_is_running ) {
        media_surface_pump_events( &surface );
        
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glDrawElements(
            GL_TRIANGLES, global_quad_index_count, GL_UNSIGNED_INT, NULL );

        media_surface_gl_swap_buffers( &surface );
    }

    glDeleteTextures( 1, &texture );
    glDeleteProgram( shader );
    destroy_surface( &surface );
    return UNPACK_SUCCESS;
}

internal UnpackError create_surface( MediaSurface* surface, u32 width, u32 height ) {
    if( !media_initialize() ) {
        error( "failed to initialize media library!" );
        return UNPACK_ERROR_CREATE_SURFACE;
    }

    if( !media_surface_create(
        width, height, text( "unpack: texture" ),
        0, on_texture_surface_event, NULL,
        MEDIA_SURFACE_GRAPHICS_BACKEND_OPENGL, surface
    ) ) {
        error( "failed to create surface!" );
        return UNPACK_ERROR_CREATE_SURFACE;
    }

    if( !media_surface_gl_init( surface ) ) {
        media_surface_destroy( surface );
        media_shutdown();
        error( "failed to initialize opengl!" );
        return UNPACK_ERROR_CREATE_SURFACE;
    }

    if( !gl_load_functions() ) {
        media_surface_destroy( surface );
        media_shutdown();
        error( "failed to load opengl functions!" );
        return UNPACK_ERROR_CREATE_SURFACE;
    }

    return UNPACK_SUCCESS;
}

internal void destroy_surface( MediaSurface* surface ) {
    media_surface_destroy( surface );
    media_shutdown();
}

