/**
 * Description:  Liquid Package Audio Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 08, 2023
*/
#include <defines.h>
#include <core/string.h>
#include <core/memory.h>
#include <core/internal.h>

#include <liquid_package.h>

#include "shared_buffer.h"
#include "internal/package_audio.h"

#define WAVE_FILE_AUDIO_PCM (1)
typedef struct no_padding {
    union {
        u32  id;
        char id_ascii[4];
    };
    u32 chunk_size;
    union {
        u32  type;
        char type_ascii[4];
    };
} RiffHeader;

typedef struct no_padding {
    union {
        u32  id;
        char id_ascii[4];
    };
    u32  size;
} ChunkID;

ChunkID make_chunk_id( const char* str4 ) {
    ChunkID result = {};
    result.id = ((str4)[3] << 24u | (str4)[2] << 16u | (str4)[1] << 8u | (str4)[0]);
    return result;
}

struct WaveFMTHeader {
    u16 audio_format;
    u16 number_of_channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_alignment;
    u16 bits_per_sample;
};

struct AudioData {
    u16 number_of_channels;
    u32 buffer_offset;
    u32 buffer_size;
};

internal PackageAudioError ___read_wave_header(
    PlatformFile* file, usize file_size, struct AudioData* out_data
) {
    unused( file_size );
    struct AudioData audio_data = {};

    const ChunkID RIFF = make_chunk_id( "RIFF" );
    const ChunkID WAVE = make_chunk_id( "WAVE" );
    const ChunkID fmt  = make_chunk_id( "fmt " );
    const ChunkID data = make_chunk_id( "data" );
    b32 read_fmt  = false;
    b32 read_data = false;

    #define read( value ) do {\
        if( !platform->io.file_read( file, sizeof( value ), &value ) ) {\
            return PACKAGE_AUDIO_ERROR_READ_RESOURCE_FILE;\
        }\
    } while(0)
    #define read_no_advance( value ) do {\
        if( !platform->io.file_read( file, sizeof( value ), &value ) ) {\
            return PACKAGE_AUDIO_ERROR_READ_RESOURCE_FILE;\
        }\
        usize current_offset = platform->io.file_query_offset( file );\
        platform->io.file_set_offset( file, current_offset - sizeof( value ) );\
    } while(0)
    #define advance( bytes ) do {\
        usize current_offset = platform->io.file_query_offset( file );\
        platform->io.file_set_offset( file, current_offset + bytes );\
    } while(0)

    RiffHeader riff = {};
    read( riff );
    if( riff.id != RIFF.id ) {
        return PACKAGE_AUDIO_ERROR_INVALID_FILE;
    }
    if( riff.type != WAVE.id ) {
        return PACKAGE_AUDIO_ERROR_INVALID_FILE;
    }

    u32 bits_per_sample = 0;
    u32 sample_rate     = 0;
    while( !( read_fmt && read_data ) ) {
        ChunkID chunk = {};
        read( chunk );
        if( chunk.id == fmt.id ) {
            struct WaveFMTHeader fmt_header = {};
            read_no_advance( fmt_header );
            advance( chunk.size );

            audio_data.number_of_channels = fmt_header.number_of_channels;

            bits_per_sample = fmt_header.bits_per_sample;
            sample_rate     = fmt_header.sample_rate;

            read_fmt = true;
            continue;
        } else if( chunk.id == data.id ) {
            audio_data.buffer_offset = platform->io.file_query_offset( file );
            audio_data.buffer_size   = chunk.size;

            read_data = true;
            continue;
        }

        advance( chunk.size );
    }

    if( audio_data.number_of_channels > 2 ) {
        return PACKAGE_AUDIO_ERROR_UNSUPPORTED_WAVE_FORMAT;
    }
    if( bits_per_sample != 16 ) {
        return PACKAGE_AUDIO_ERROR_UNSUPPORTED_WAVE_FORMAT;
    }
    if( sample_rate != LIQUID_PACKAGE_RESOURCE_AUDIO_SAMPLES_PER_SECOND ) {
        return PACKAGE_AUDIO_ERROR_UNSUPPORTED_WAVE_FORMAT;
    }

    *out_data = audio_data;
    return PACKAGE_AUDIO_SUCCESS;
}

PackageAudioError package_audio(
    StringSlice* path, struct LiquidPackageResourceAudio* out_resource,
    usize* out_buffer_size, void** out_buffer
) {
    StringSlice ext = {};
    for( usize i = path->len; i-- > 0; ) {
        ext.len++;
        if( path->buffer[i] == '.' ) {
            ext.buffer = path->buffer + i;
            break;
        }
    }

    string_slice_const( ext_wave, ".wav" );
    if( !ext.len || !string_slice_cmp( &ext, &ext_wave ) ) {
        return PACKAGE_AUDIO_ERROR_UNRECOGNIZED_EXTENSION;
    }

    PlatformFile* file = platform->io.file_open(
        path->buffer,
        PLATFORM_FILE_READ |
        PLATFORM_FILE_SHARE_READ |
        PLATFORM_FILE_ONLY_EXISTING );
    if( !file ) {
        return PACKAGE_AUDIO_ERROR_OPEN_RESOURCE_FILE;
    }

    usize file_size = platform->io.file_query_size( file );

    struct AudioData data = {};
    PackageAudioError wave_header_result =
        ___read_wave_header( file, file_size, &data );
    if( wave_header_result != PACKAGE_AUDIO_SUCCESS ) {
        platform->io.file_close( file );
        return wave_header_result;
    }

    struct LiquidPackageResourceAudio resource = {};
    resource.compression  = LIQUID_PACKAGE_RESOURCE_AUDIO_COMPRESSION_NONE;
    resource.sample_count =
        data.buffer_size /
        ( data.number_of_channels *
        LIQUID_PACKAGE_RESOURCE_AUDIO_BYTES_PER_CHANNEL_SAMPLE );

    resource.right_channel_buffer_offset =
        data.number_of_channels > 1 ?
        data.buffer_size / 2 : 0;

    i16* deinterleaved_buffer = package_shared_buffer_allocate( data.buffer_size );
    if( !deinterleaved_buffer ) {
        platform->io.file_close( file );
        return PACKAGE_AUDIO_ERROR_OUT_OF_MEMORY;
    }

    platform->io.file_set_offset( file, data.buffer_offset );

    if( resource.right_channel_buffer_offset ) {
        i16* unprocessed_data = package_shared_buffer_allocate( data.buffer_size );
        if( !unprocessed_data ) {
            package_shared_buffer_free( deinterleaved_buffer, data.buffer_size );
            platform->io.file_close( file );
            return PACKAGE_AUDIO_ERROR_OUT_OF_MEMORY;
        }

        if( !platform->io.file_read(
            file, data.buffer_size,
            unprocessed_data
        ) ) {
            package_shared_buffer_free( unprocessed_data, data.buffer_size );
            package_shared_buffer_free( deinterleaved_buffer, data.buffer_size );
            platform->io.file_close( file );
            return PACKAGE_AUDIO_ERROR_READ_RESOURCE_FILE;
        }

        for( usize i = 0; i < resource.sample_count; ++i ) {
            usize index = i * sizeof(i16);

            i16* src = unprocessed_data + index;
            i16* dst_left  = deinterleaved_buffer + i;
            i16* dst_right =
                (i16*)(
                    (u8*)deinterleaved_buffer +
                    resource.right_channel_buffer_offset +
                    index
                );

            *dst_left  = *( src + 0 );
            *dst_right = *( src + 1 );
        }

        package_shared_buffer_free( unprocessed_data, data.buffer_size );
    } else {
        if( !platform->io.file_read(
            file, data.buffer_size,
            deinterleaved_buffer
        ) ) {
            package_shared_buffer_free( deinterleaved_buffer, data.buffer_size );
            platform->io.file_close( file );
            return PACKAGE_AUDIO_ERROR_READ_RESOURCE_FILE;
        }
    }

    *out_resource    = resource;
    *out_buffer_size = data.buffer_size;
    *out_buffer      = deinterleaved_buffer;

    platform->io.file_close( file );
    return PACKAGE_AUDIO_SUCCESS;
}

