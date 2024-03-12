/**
 * Description:  Texture processing.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 24, 2024
*/
#include "shared/defines.h"

#include "core/fs.h"
#include "core/string.h"
#include "core/compression.h"
#include "core/sort.h"
#include "core/rand.h"
#include "core/math.h"

#include "package/manifest.h"
#include "package/logging.h"
#include "package/resource.h"

#include "generated/package_hashes.h"

struct TextureInfo {
    i64 width, height, depth;
    u32 bits_per_pixel;
    PackageTextureDimensions dimensions;
    PackageTextureChannels   channels;

    usize offset;
    usize size;
};

b32 collect_texture_info_bmp(
    usize thread_index, PathSlice path,
    FileHandle* input, struct TextureInfo* out_texture_info );

b32 process_resource_texture(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input, FileHandle* output, usize buffer_size, void* buffer
) {
    FileHandle* temp = NULL;
    #define read( src, size, dst ) do {\
        if( !fs_file_read( src, size, dst ) ) {\
            log_error( "failed to read texture!" );\
            debug_break();\
            if( temp ) {\
                fs_file_close( temp );\
            }\
            return false;\
        }\
    } while(0)
    #define write( src, size, dst ) do {\
        if( !fs_file_write( src, size, dst ) ) {\
            log_error( "failed to write texture!" );\
            debug_break();\
            if( temp ) {\
                fs_file_close( temp );\
            }\
            return false;\
        }\
    } while(0)

    PathSlice ext = {};
    if( !path_slice_get_extension( item->path, &ext ) ) {
        log_error( "path does not have a file extension!" );
        return false;
    }
    u64 ext_hash = string_slice_hash( ext );

    struct TextureInfo info = {};
    switch( ext_hash ) {
        case HASH_EXT_BMP: {
            if( !collect_texture_info_bmp(
                thread_index, item->path, input, &info
            ) ) {
                return false;
            }
        } break;
        default: {
            log_error(
                "path does not have a recognized file extension! ext: {p}", ext );
        } break;
    }

    fs_file_set_offset( input, info.offset, false );

    /* open temp file */ {
        if( item->compression ) {
            path_buffer_empty( temp_path, 256 );
            string_buffer_fmt( &temp_path, "./pkgtemp/{u32}_ctex.tmp{0}", rand_xor_u32() );

            temp = fs_file_open(
                to_slice( &temp_path ),
                FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_READ |
                FILE_OPEN_FLAG_CREATE );

            if( !temp ) {
                log_error( "failed to create texture temp file!" );
                return false;
            }
        }
    }

    u32 width  = absolute( info.width );
    u32 height = absolute( info.height );
    u32 depth  = absolute( info.depth );

    // process image and write to temp/output file,
    // depends on compression
    usize row_size  = info.width * (info.bits_per_pixel / 8);
    usize row_count = info.size / row_size;

    FileHandle* dst = item->compression ? temp : output;

    // TODO(alicia): 
    // what if row_size > buffer_size? (width * Bpp > 2MiB ?)
    // unlikely but that case should probably be handled.
    // buffer size needs to be able to accomodate at least two rows
    // for reversing, at least one row for top-down image.
    // alternatively, we could just write out a negative w/h/d
    // and let the application handle an upside down image.
    // Also, this does not handle 3D textures yet although
    // I struggle to see a need for that?

    // image is upside down
    if( info.height < 0 ) {
        void* swap_buffer      = buffer;
        void* rows_buffer      = (u8*)swap_buffer + row_size;
        usize row_count_buffer = (buffer_size - row_size) / row_size;
        usize remaining_rows = row_count;

        usize running_read_size = 0;
        while( remaining_rows ) {
            usize read_row_count = row_count_buffer;
            if( read_row_count > remaining_rows ) {
                read_row_count = remaining_rows;
            }

            usize read_size   = read_row_count * row_size;
            usize read_offset =
                (info.offset + info.size) -
                (running_read_size + read_size);

            fs_file_set_offset( input, read_offset, false );
            read( input, read_size, rows_buffer );

            sorting_reverse(
                read_row_count, row_size,
                rows_buffer, swap_buffer );
            // TODO(alicia): swap BGR -> RGB?

            write( dst, read_size, rows_buffer );

            running_read_size += read_size;
            remaining_rows    -= read_row_count;
        }
    } else {
        void* rows_buffer      = buffer;
        usize row_count_buffer = buffer_size / row_size;
        usize remaining_rows   = row_count;

        fs_file_set_offset( input, info.offset, false );

        while( remaining_rows ) {
            usize read_row_count = row_count_buffer;
            if( read_row_count > remaining_rows ) {
                read_row_count = remaining_rows;
            }

            usize read_size = read_row_count * row_size;
            read( input, read_size, rows_buffer );

            write( dst, read_size, rows_buffer );
            remaining_rows -= read_row_count;
        }
    }

    switch( item->compression ) {
        case PACKAGE_COMPRESSION_NONE: break;
        case PACKAGE_COMPRESSION_RLE: {
            usize remaining = fs_file_query_size( temp );
            fs_file_set_offset( temp, 0, false );

            u64 original_size = remaining;
            write( output, sizeof(original_size), &original_size );

            while( remaining ) {
                usize read_size = buffer_size;
                if( read_size > remaining ) {
                    read_size = remaining;
                }

                read( temp, read_size, buffer );

                usize not_written = compression_rle_encode(
                    package_compression_stream, output,
                    read_size, buffer, 0 );

                if( not_written ) {
                    log_error( "failed to rle compress texture!" );
                    debug_break();
                    return false;
                }

                remaining -= read_size;
            }
        } break;
    }

    out_resource->type        = PACKAGE_RESOURCE_TYPE_TEXTURE;
    out_resource->compression = item->compression;

    out_resource->texture.width  = width;
    out_resource->texture.height = height;
    out_resource->texture.depth  = depth;

    out_resource->texture.type.dimensions = info.dimensions;
    out_resource->texture.type.channels   = info.channels;
    out_resource->texture.flags           = item->texture.flags;

    // TODO(alicia): properly determine base type
    out_resource->texture.type.base_type  = PACKAGE_TEXTURE_BASE_TYPE_8BIT;

    fs_file_close( temp );
    #undef read
    #undef write
    return true;
}

#define BMP_FILE_ID ( 'M' << (u16)(8) | 'B' << (u16)(0) )
typedef enum BitmapCompression : u32 {
    BI_RGB       = 0,
    BI_BITFIELDS = 3,
} BitmapCompression;

struct no_padding BitmapHeader {
    union {
        char id_c[2];
        u16  id;
    };
    u32 file_size;
    u16 reserved[2];
    u32 pixel_offset;
};

typedef enum BitmapInfoHeaderSize : u32 {
    BITMAP_INFO_HEADER_V1 = 40,
    BITMAP_INFO_HEADER_V4 = 108,
    BITMAP_INFO_HEADER_V5 = 124
} BitmapInfoHeaderSize;

struct no_padding BitmapInfo {
    i32 width, height;
    u16 planes;
    u16 bit_count;
    BitmapCompression compression;
    u32 size_image;
    i32 x_pixels_per_meter;
    i32 y_pixels_per_meter;
    u32 clr_used;
    u32 clr_important;
};

typedef u32 FixedPoint230;

struct no_padding Ciexyz {
    FixedPoint230 x;
    FixedPoint230 y;
    FixedPoint230 z;
};

struct no_padding CiexyzTriple {
    struct Ciexyz red;
    struct Ciexyz green;
    struct Ciexyz blue;
};

struct no_padding BitmapInfoV4 {
    u32 red_mask;
    u32 green_mask;
    u32 blue_mask;
    u32 alpha_mask;
    u32 cs_type;
    struct CiexyzTriple endpoints;
    u32 gamma_red;
    u32 gamma_green;
    u32 gamma_blue;
};

struct no_padding BitmapInfoV5 {
    u32 intent;
    u32 profile_data;
    u32 profile_size;
    u32 reserved;
};

struct no_padding BitmapInfoHeader {
    BitmapInfoHeaderSize size;
    struct BitmapInfo    info;
    struct BitmapInfoV4  v4;
    struct BitmapInfoV5  v5;
};

b32 collect_texture_info_bmp(
    usize thread_index, PathSlice path,
    FileHandle* input, struct TextureInfo* out_texture_info
) {
    #define read( src, size, dst ) do {\
        if( !fs_file_read( src, size, dst ) ) {\
            log_error( "failed to read audio file!" );\
            debug_break();\
            return false;\
        }\
    } while(0)
    
    struct BitmapHeader header = {};
    read( input, sizeof(header), &header );

    if( header.id != BMP_FILE_ID ) {
        log_error( "file '{p}' is not a bmp!", path );
        return false;
    }

    struct BitmapInfoHeader info_header = {};
    read( input, sizeof(info_header.size), &info_header.size );

    switch( info_header.size ) {
        case BITMAP_INFO_HEADER_V1: {
            read( input, sizeof(info_header.info), &info_header.info );
        } break;
        case BITMAP_INFO_HEADER_V4: {
            read( input,
                 sizeof( info_header.info ) + sizeof( info_header.v4 ),
                 &info_header.info );
        } break;
        case BITMAP_INFO_HEADER_V5: {
            read( input,
                 sizeof( info_header.info ) +
                 sizeof( info_header.v4 )   + sizeof( info_header.v5 ),
                 &info_header.info );
        } break;
        default: {
            log_error( "bitmap has unsupported info header!" );
            debug_break();
        } return false;
    }

    switch( info_header.info.compression ) {
        case BI_RGB:
        case BI_BITFIELDS: break;

        default: {
            log_error(
                "bitmap has unsupported compression! compression: {u32,X}",
                info_header.info.compression );
            debug_break();
        } return false;
    }

    out_texture_info->width  = info_header.info.width;
    out_texture_info->height = info_header.info.height;
    if( out_texture_info->height ) {
        out_texture_info->dimensions = PACKAGE_TEXTURE_DIMENSIONS_2;
    } else {
        out_texture_info->dimensions = PACKAGE_TEXTURE_DIMENSIONS_1;
    }
    out_texture_info->bits_per_pixel = info_header.info.bit_count;
    out_texture_info->offset = header.pixel_offset;
    out_texture_info->size   =
        fs_file_query_size( input ) - out_texture_info->offset;

    if( info_header.info.bit_count == 24 ) {
        out_texture_info->channels = PACKAGE_TEXTURE_CHANNEL_RGB;
    } else {
        out_texture_info->channels = PACKAGE_TEXTURE_CHANNEL_RGBA;
    }

    #undef read
    return true;
}

#undef BMP_FILE_TYPE
#undef BI_RGB
#undef BI_BITFIELDS

