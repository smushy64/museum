/**
 * Description:  Text resource packaging.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 22, 2024
*/
#include "shared/defines.h"
#include "shared/liquid_package.h"

#include "core/fs.h"
#include "core/compression.h"

#include "package/manifest.h"
#include "package/logging.h"
#include "package/resource.h"

b32 process_resource_text(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input, FileHandle* output, usize buffer_size, void* buffer
) {

    #define read( buffer_size, buffer ) do {\
        if( !fs_file_read( input, buffer_size, buffer ) ) {\
            log_error( "failed to read text file!" );\
            debug_break();\
            return false;\
        }\
    } while(0)

    #define write( buffer_size, buffer ) do {\
        if( !fs_file_write( output, buffer_size, buffer ) ) {\
            log_error( "failed to write text file!" );\
            debug_break();\
            return false;\
        }\
    } while(0)

    // TODO(alicia): recognize different encodings/convert encodings!
    unused(item);
    usize original_size;
    usize remaining = original_size = fs_file_query_size( input );

    while( remaining ) {
        usize read_size = buffer_size;
        if( read_size > remaining ) {
            read_size = remaining;
        }
        switch( item->compression ) {
            case PACKAGE_COMPRESSION_NONE: {
                read( read_size, buffer );
                write( read_size, buffer );

                remaining -= read_size;
            } break;
            case PACKAGE_COMPRESSION_RLE: {
                if( original_size == remaining ) {
                    u64 original_size_64 = (u64)original_size;
                    write( sizeof(original_size_64), &original_size_64 );
                }

                read( read_size, buffer );

                usize encode_size = 0;
                usize not_written = compression_rle_encode(
                    package_compression_stream, output,
                    read_size, buffer, &encode_size );

                if( not_written ) {
                    log_error( "failed to write RLE compressed text file!" );
                    return false;
                }

                remaining -= read_size;
            } break;
        }
    }

    out_resource->type          = PACKAGE_RESOURCE_TYPE_TEXT;
    out_resource->text.lang     = PACKAGE_TEXT_LANGUAGE_ENGLISH;
    out_resource->text.encoding = PACKAGE_TEXT_ENCODING_UTF8;
    out_resource->compression   = item->compression;

    #undef read
    #undef write
    return true;
}


