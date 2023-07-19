// * Description:  Asset Manager Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 13, 2023
#include "asset.h"
#include "core/logging.h"
#include "core/memory.h"
#include "graphics.h"
#include "platform/io.h"
#include "math/functions.h"

#define BMP_FILE_TYPE ( 'M' << (u16)(8) | 'B' << (u16)(0) )
#define BI_RGB       0u
#define BI_BITFIELDS 3u
#define BPP_24 24u
#define BPP_32 32u
/// BMP file header
MAKE_PACKED( struct BMPHeader {
    union {
        char type_char[2];
        u16  type;
    };
    u32  bmp_file_size;
    u32  __reserved0;
    u32  data_offset;
    u32  unused_dib_header_size;
    i32  image_width;
    i32  image_height;
    u16  unused_biplanes;
    u16  bits_per_pixel;
    u32  compression;
} );

#define BMP_IS_TOP_DOWN( image_height )\
    ( image_height < 0 )

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

    if( BMP_FILE_TYPE != header.type ) {
        LOG_ERROR("File \"{cc}\" is not a BMP!", path);
        return false;
    }

    if( !(
        header.compression == BI_RGB ||
        header.compression == BI_BITFIELDS
    ) ) {
        LOG_ERROR(
            "BMP File \"{cc}\" has an unsupported compression type!",
            path
        );
        return false;
    }

    if( header.bmp_file_size != file_size ) {
        LOG_ERROR("File \"{cc}\" is a malformed BMP!", path);
        return false;
    }

    switch( header.bits_per_pixel ) {
        case BPP_24:
            out_image->format = TEXTURE_FORMAT_RGB;
            break;
        case BPP_32:
            out_image->format = TEXTURE_FORMAT_RGBA;
            break;
        default:
            LOG_ERROR(
                "BMP file \"{cc}\" has invalid bits-per-pixel: {u16}",
                path,
                header.bits_per_pixel
            );
            return false;
    }

    out_image->width  = header.image_width;
    out_image->height = absolute( header.image_height );

    usize image_size = file_size - header.data_offset;

    u32 bytes_per_pixel = header.bits_per_pixel / 8;
    u32 bytes_per_row   = bytes_per_pixel * out_image->width;
    out_image->buffer = mem_alloc(
        image_size +
        bytes_per_row,
        MEMTYPE_UNKNOWN
    );
    if( !out_image->buffer ) {
        return false;
    }

    platform_file_set_offset( bmp_file_handle, header.data_offset );
    platform_file_read(
        bmp_file_handle,
        image_size,
        image_size,
        out_image->buffer
    );

    if( BMP_IS_TOP_DOWN( header.image_height ) ) {
        u8* byte_buffer = (u8*)out_image->buffer;
        u8* temp_buffer = &byte_buffer[image_size];
        i32 i = 0, n = out_image->height - 1;
        while( i <= n ) {
            u8* src = &byte_buffer[i * bytes_per_row];
            u8* dst = &byte_buffer[n * bytes_per_row];
            mem_copy( temp_buffer, src, bytes_per_row );
            mem_copy( src, dst, bytes_per_row );
            mem_copy( dst, temp_buffer, bytes_per_row );
            n--; i++;
        }
        for( i32 y = 0; y < out_image->height; ++y ) {
            n = (out_image->width * bytes_per_pixel) - 1;
            i = 0;
            i32 stride = y * bytes_per_row;
            while( i <= n ) {
                SWAP( byte_buffer[n + stride], byte_buffer[i + stride] );
                n--; i++;
            }
        }
        
    }

    platform_file_close( bmp_file_handle );
    return true;
}
void debug_destroy_bmp( DebugImage* image ) {
    if( image->buffer ) {
        mem_free( image->buffer );
    }
    *image = {};
}

