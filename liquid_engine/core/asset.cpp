// * Description:  Asset Manager Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 13, 2023
#include "asset.h"
#include "core/logging.h"
#include "core/memory.h"
#include "graphics.h"
#include "platform/io.h"
#include "math/functions.h"

/// BMP file header
MAKE_PACKED( struct BMPHeader {
    char identifier[2];
    u32  bmp_file_size;
    u32  __reserved0;
    u32  data_offset;
    u32  unused_dib_header_size;
    i32  image_width;
    i32  image_height;
} );

#define BMP_IS_UPSIDE_DOWN( image_height )\
    ARE_BITS_SET( image_height, I32::SIGN_MASK )

b32 debug_load_bmp( const char* path, DebugImage* out_image ) {
    FileHandle bmp_file_handle = {};
    if( !platform_file_open(
        path,
        PLATFORM_FILE_OPEN_READ |
        PLATFORM_FILE_OPEN_SHARE_READ |
        PLATFORM_FILE_OPEN_EXISTING,
        &bmp_file_handle
    ) ) {
        return false;
    }

    usize file_size = platform_file_query_size( bmp_file_handle );
    if( file_size < sizeof( BMPHeader ) ) {
        LOG_ERROR( "File \"{cc}\" is not a bmp!", path );
        return false;
    }

    BMPHeader header = {};
    platform_file_set_offset( bmp_file_handle, 0 );
    if( !platform_file_read(
        bmp_file_handle,
        sizeof( BMPHeader ),
        sizeof( BMPHeader ),
        &header
    ) ) {
        return false;
    }

    if(!(
        header.identifier[0] == 'B' &&
        header.identifier[1] == 'M'
    )) {
        LOG_ERROR("File \"{cc}\" is not a BMP!", path);
        return false;
    }

    if( header.bmp_file_size != file_size ) {
        LOG_ERROR("File \"{cc}\" is a malformed BMP!", path);
        return false;
    }

    if( BMP_IS_UPSIDE_DOWN( header.image_height ) ) {
    }

    out_image->width  = header.image_width;
    out_image->height = absolute( header.image_height );

    usize image_size = file_size - header.data_offset;

    out_image->buffer = mem_alloc( image_size, MEMTYPE_UNKNOWN );
    if( !out_image->buffer ) {
        return false;
    }

    out_image->format = TEXTURE_FORMAT_RGBA;

    platform_file_set_offset( bmp_file_handle, header.data_offset );
    platform_file_read(
        bmp_file_handle,
        image_size,
        image_size,
        out_image->buffer
    );

    platform_file_close( bmp_file_handle );
    return true;
}
void debug_destroy_bmp( DebugImage* image ) {
    if( image->buffer ) {
        mem_free( image->buffer );
    }
    *image = {};
}

