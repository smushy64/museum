// * Description:  Parse Image Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "image.h"
#include "fs.h"
#include <math.h>
#include <stdlib.h>
#include <memory.h>

#define BMP_FILE_TYPE ( 'M' << (u16)(8) | 'B' << (u16)(0) )
#define BI_RGB       0u
#define BI_BITFIELDS 3u
#define BPP_24 24u
#define BPP_32 32u

typedef struct PACKED {
    u32 header_size;
    i32 width;
    i32 height;
    u16 biplanes; // must be 1
    u16 bits_per_pixel;
    u32 compression;
    u32 image_size;
    u32 horizontal_resolution;
    u32 vertical_resolution;
    u32 color_palette_size;
    u32 important_color_count;
} BMPDIBHeader;

typedef struct PACKED {
    union {
        char type_char[2];
        u16  type;
    };
    u32 bmp_file_size;
    u32 __reserved0;
    u32 data_offset;

    BMPDIBHeader dib;
} BMPHeader;

#define BMP_IS_TOP_DOWN( image_height ) \
    (image_height < 0)

b32 parse_bmp_0_1(
    usize* buffer_size,
    void** buffer,
    FILE*  image_file,
    AssetTexture_0_1* out_texture
) {
    usize file_size = get_file_size( image_file );
    if( file_size < sizeof( BMPHeader ) ) {
        return false;
    }

    BMPHeader header = {0};
    fread( &header, 1, sizeof(BMPHeader), image_file );

    if( header.type != BMP_FILE_TYPE ) {
        return false;
    }

    if( !(
        header.dib.compression == BI_RGB ||
        header.dib.compression == BI_BITFIELDS
    ) ) {
        return false;
    }

    if( header.bmp_file_size != file_size ) {
        return false;
    }

    switch( header.dib.bits_per_pixel ) {
        case BPP_24: {
            out_texture->format = TEXTURE_FORMAT_RGB;
        } break;
        case BPP_32: {
            out_texture->format = TEXTURE_FORMAT_RGBA;
        } break;
        default: return false;
    }

    out_texture->width  = header.dib.width;
    out_texture->height = abs( header.dib.height );

    usize image_size = file_size - header.data_offset;

    u32 bytes_per_pixel = header.dib.bits_per_pixel / 8;
    u32 bytes_per_row   = bytes_per_pixel * out_texture->width;

    usize buffer_offset = *buffer_size;
    void* buffer_ptr    = *buffer;
    if( !buffer_ptr ) {
        buffer_ptr   = malloc( image_size );
        *buffer_size = image_size;
        ASSERT( buffer_ptr );
    } else {
        *buffer_size += image_size;
        buffer_ptr = realloc( buffer_ptr, *buffer_size );
        ASSERT( buffer_ptr );
    }

    u8* pixel_ptr = buffer_ptr + buffer_offset;

    fseeko( image_file, header.data_offset, SEEK_SET );
    fread( pixel_ptr, 1, image_size, image_file );

    if( BMP_IS_TOP_DOWN( header.dib.height ) ) {
        u8* byte_buffer = pixel_ptr;
        u8* temp_buffer = &byte_buffer[image_size];
        i32 i = 0;
        i32 n = out_texture->height - 1;
        while( i <= n ) {
            u8* src = &byte_buffer[i * bytes_per_row];
            u8* dst = &byte_buffer[n * bytes_per_row];
            memcpy( temp_buffer, src, bytes_per_row );
            memcpy( src, dst, bytes_per_row );
            memcpy( dst, temp_buffer, bytes_per_row );
            n--; i++;
        }

        for( i32 y = 0; y < out_texture->height; ++y ) {
            n = ( out_texture->width * bytes_per_pixel ) - 1;
            i = 0;
            i32 stride = y * bytes_per_row;
            while( i <= n ) {
                SWAP( byte_buffer[n + stride], byte_buffer[i + stride] );
                n--; i++;
            }
        }
    }

    out_texture->buffer_offset = buffer_offset;

    *buffer = buffer_ptr;
    return true;
}

