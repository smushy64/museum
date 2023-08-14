// * Description:  Liquid Engine Packager Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "package.h"
#include "format.h"
#include "version.h"
#include "term_color.h"
#include "fs.h"
#include "str.h"
#include "image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

internal b32 make_package_0_1( PackagerParameters* params );

#define FATAL() printerrln( "Something went wrong!" ); PANIC()

b32 make_package( PackagerParameters* params ) {

    if( !params->overwrite_existing ) {
        if( does_file_exist( params->output_path ) ) {
            printerrln(
                TC_RED( "Error: " )
                "Package \"%s\" already exists! "
                "Use --overwrite to overwrite.",
                params->output_path
            );
            return false;
        }
    }

    switch( params->major ) {
        case 0: switch( params->minor ) {
            case 1: {
                return make_package_0_1( params );
            } break;
            default: FATAL();
        }
        default: FATAL();
    }
    return false;
}

internal void package_shader_spv_0_1(
    usize*      buffer_size,
    void**      buffer,
    Asset_0_1*  asset,
    const char* shader_path
) {
    ShaderType shader_type;
    if( str_contains( shader_path, ".vert", NULL ) ) {
        shader_type = SHADER_TYPE_VERTEX;
    } else if( str_contains( shader_path, ".frag", NULL ) ) {
        shader_type = SHADER_TYPE_FRAGMENT;
    } else {
        // TODO(alicia): handle geometry, compute!
        PANIC();
    }

    usize offset      = *buffer_size;
    void* buffer_ptr  = *buffer;
    FILE* shader_file = fopen( shader_path, "rb+" );
    ASSERT( shader_file );

    usize file_size = get_file_size( shader_file );
    if( !buffer ) {
        *buffer_size = file_size;
        buffer_ptr   = malloc( file_size );
        ASSERT( buffer_ptr );
    } else {
        *buffer_size += file_size;
        buffer_ptr = realloc( buffer_ptr, *buffer_size );
        ASSERT( buffer_ptr );
    }

    fread( buffer_ptr + offset, 1, file_size, shader_file );

    asset->shader.type          = shader_type;
    asset->shader.buffer_offset = offset;
    asset->shader.buffer_size   = file_size;

    *buffer = buffer_ptr;
    fclose( shader_file );
}


internal void package_image_0_1(
    usize* buffer_size,
    void** buffer,
    ImageFormat format,
    Asset_0_1*  asset,
    const char* image_path
) {

    AssetTexture_0_1 texture = {0};

    FILE* image_file = fopen( image_path, "rb+" );
    ASSERT( image_file );

    switch( format ) {
        case IMAGE_TYPE_BMP: {
            ASSERT( parse_bmp_0_1(
                buffer_size,
                buffer,
                image_file,
                &texture
            ) );
        } break;
        // TODO(alicia): other formats!
        default: PANIC();
    }

    // TODO(alicia): compression!
    texture.compression = TEXTURE_COMPRESSION_NONE;

    asset->texture = texture;

    fclose( image_file );
}

internal b32 make_package_0_1( PackagerParameters* params ) {
    FILE* output_file = fopen( params->output_path, "wb+" );
    if( !output_file ) {
        printerrln(
            TC_RED( "Error: " )
            "Failed to open \"%s\" for writing!",
            params->output_path
        );
        return false;
    }

    println( "Creating package at \"%s\" . . .", params->output_path );

    LEPkgHeader_0_1 header = {0};
    header.identifier  = LEPKG_FILE_IDENTIFIER;
    header.version     = LEPKG_MAKE_VERSION( params->major, params->minor );
    header.header_size = sizeof( LEPkgHeader_0_1 );

    u32        asset_count = 0;
    Asset_0_1* assets      =
        malloc( sizeof(Asset_0_1) * params->input_path_count );
    ASSERT( assets );

    usize buffer_size = 0;
    void* buffer      = NULL;

    for( u32 i = 0; i < params->input_path_count; ++i ) {
        Asset_0_1*  current_asset = &assets[asset_count];
        const char* current_path  = params->input_paths[i];

        FiletypeInfo info = get_file_asset_info( current_path );
        if( !info.is_supported ) {
            continue;
        }

        current_asset->type = info.asset_type;

        switch( info.asset_type ) {
            case ASSET_TYPE_SHADER: {
                switch( info.shader_format ) {
                    case SHADER_FORMAT_SPV: {
                        package_shader_spv_0_1(
                            &buffer_size,
                            &buffer,
                            current_asset,
                            current_path
                        );
                        asset_count++;
                    } break;
                    default: PANIC();
                }
            } break;
            case ASSET_TYPE_IMAGE: {
                package_image_0_1(
                    &buffer_size,
                    &buffer,
                    info.image_format,
                    current_asset,
                    current_path
                );
            } break;
            default: PANIC();
        }
    }

    header.asset_count = asset_count;
    header.file_size   =
        (sizeof(Asset_0_1) * header.asset_count) +
        (buffer_size) +
        (header.header_size);

    fclose( output_file );

    return true;
}

