/**
 * Description:  Resource Processing Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 20, 2023
*/
#include "defines.h"
#include "core/collections.h"
#include "core/thread.h"
#include "core/fs.h"
// #include "core/math.h"
#include "liquid_package.h"

#include "process_resource.h"
#include "logging.h"
#include "parse_manifest.h"

#define rp_error( format, ... )\
    lp_error( "[{usize}]: " format, index, ##__VA_ARGS__ )

#define rp_print( format, ... )\
    lp_print( "[{usize}]: " format, index, ##__VA_ARGS__ )

#define rp_note( format, ... )\
    lp_note( "[{usize}]: " format, index, ##__VA_ARGS__ )

volatile u32 global_offset = 0;

typedef enum AudioDataCompression {
    AUDIO_DATA_COMPRESSION_NONE,
} AudioDataCompression;

struct AudioData {
    u16 number_of_channels;
    u32 buffer_offset;
    u32 buffer_size;
    AudioDataCompression compression;
};

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
    ChunkID result;
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

maybe_unused
internal b32 ___read_wave_header(
    FSFile* file, usize file_size,
    struct AudioData* out_data, usize index
) {
    unused( file_size );
    struct AudioData audio_data = {0};

    const ChunkID RIFF = make_chunk_id( "RIFF" );
    const ChunkID WAVE = make_chunk_id( "WAVE" );
    const ChunkID fmt  = make_chunk_id( "fmt " );
    const ChunkID data = make_chunk_id( "data" );
    b32 read_fmt  = false;
    b32 read_data = false;

    #define read( value ) do {\
        if( !fs_file_read( file, sizeof( value ), &value ) ) {\
            rp_error( "failed to read resource file!" );\
            return false;\
        }\
    } while(0)
    #define read_no_advance( value ) do {\
        if( !fs_file_read( file, sizeof( value ), &value ) ) {\
            lp_error( "failed to read resource file!" );\
            return false;\
        }\
        usize current_offset = fs_file_query_offset( file );\
        fs_file_set_offset( file, current_offset - sizeof( value ) );\
    } while(0)
    #define advance( bytes ) do {\
        usize current_offset = fs_file_query_offset( file );\
        fs_file_set_offset( file, current_offset + bytes );\
    } while(0)

    RiffHeader riff = {0};
    read( riff );
    if( riff.id != RIFF.id ) {
        rp_error(
            "resource file has a wave file extension but it "
            "does not have properly formatted riff header!" );
        return false;
    }
    if( riff.type != WAVE.id ) {
        rp_error(
            "resource file has a wave file extension but it "
            "does not have properly formatted header!" );
        return false;
    }

    u32 bits_per_sample = 0;
    u32 sample_rate     = 0;
    while( !( read_fmt && read_data ) ) {
        ChunkID chunk = {0};
        read( chunk );
        if( chunk.id == fmt.id ) {
            struct WaveFMTHeader fmt_header = {0};
            read_no_advance( fmt_header );
            advance( chunk.size );

            audio_data.number_of_channels = fmt_header.number_of_channels;

            bits_per_sample = fmt_header.bits_per_sample;
            sample_rate     = fmt_header.sample_rate;

            read_fmt = true;
            continue;
        } else if( chunk.id == data.id ) {
            audio_data.buffer_offset = fs_file_query_offset( file );
            audio_data.buffer_size   = chunk.size;

            read_data = true;
            continue;
        }

        advance( chunk.size );
    }

    if( audio_data.number_of_channels > 2 ) {
        rp_error( "wave files with more than 2 channels are not supported!" );
        return false;
    }
    if( bits_per_sample != 16 ) {
        rp_error( "wave files that are not 16-bit PCM are not supported!" );
        return false;
    }
    if( sample_rate != LIQUID_PACKAGE_RESOURCE_AUDIO_SAMPLES_PER_SECOND ) {
        rp_error( "wave files that are not 44100Hz are not supported!" );
        return false;
    }

    *out_data = audio_data;
    return true;
}

typedef enum : u32 {
    RESOURCE_AUDIO_FILE_TYPE_UNRECOGNIZED,
    RESOURCE_AUDIO_FILE_TYPE_WAVE,
} ResourceAudioFileType;
ResourceAudioFileType ___determine_audio_file_type( StringSlice* extension ) {
    string_slice_const( extension_wave, ".wav" );
    if( !extension->len ) {
        return RESOURCE_AUDIO_FILE_TYPE_UNRECOGNIZED;
    }

    if( string_slice_cmp( extension, &extension_wave ) ) {
        return RESOURCE_AUDIO_FILE_TYPE_WAVE;
    }

    return RESOURCE_AUDIO_FILE_TYPE_UNRECOGNIZED;
}

#define CHANNEL_SIZE (sizeof(i16))
#define STEREO_CHANNEL_COUNT 2
void ___process_audio(
    struct PackagerResourceProcessParams* params,
    const char* path_resource,
    FSFile* file_output,
    ResourceAudioFileType file_type,
    usize package_buffer_start_offset,
    usize package_resource_offset
) {
    unused( params );
    unused( path_resource );
    unused( file_output );
    unused( file_type );
    unused( package_buffer_start_offset );
    unused( package_resource_offset );
#if 0
    unused( file_type );
    usize index = params->index;

    FSFile* file_resource = fs_file_open(
        path_resource, FS_FILE_READ |
        FS_FILE_SHARE_READ |
        FS_FILE_ONLY_EXISTING );
    if( !file_resource ) {
        rp_error( "failed to open resource at path '{cc}'!", path_resource );
        return;
    }

    usize file_size = fs_file_query_size( file_resource );
    struct AudioData audio_data = {0};
    // TODO(alicia): non wave file types!
    if( !___read_wave_header( file_resource, file_size, &audio_data, index ) ) {
        rp_error( "failed to read wave header!" );
        fs_file_close( file_resource );
        return;
    }

    struct LiquidPackageResource resource = {0};
    resource.type = LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO;
    // TODO(alicia): compression!
    resource.audio.compression  = LIQUID_PACKAGE_RESOURCE_AUDIO_COMPRESSION_NONE;
    resource.audio.sample_count =
        audio_data.buffer_size / ( CHANNEL_SIZE * audio_data.number_of_channels );
    resource.audio.right_channel_buffer_offset =
        audio_data.number_of_channels > 1 ? audio_data.buffer_size / 2 : 0;

    fs_file_set_offset( file_resource, audio_data.buffer_offset );

    usize package_buffer_offset =
        interlocked_add( &global_offset, audio_data.buffer_size );
    read_write_fence();

    resource.buffer_offset = package_buffer_offset;
    package_buffer_offset += package_buffer_start_offset;

    void* stream_buffer        = params->stream_buffer;
    usize stream_buffer_size   = params->stream_buffer_size;
    unused( stream_buffer_size );

    i16* stream_buffer_samples = stream_buffer;

    usize stream_buffer_sample_count = params->stream_buffer_size / sizeof(i16);
    assert( stream_buffer_sample_count && stream_buffer_sample_count / 4 );

    // TODO(alicia): compression!
    if( resource.audio.right_channel_buffer_offset ) {
        // TODO(alicia): process all audio this way so it
        // can be compressed, interleaved or whatever
        // other processing needs to be done simultaneously
        /**
         * stream buffer
         * [ | | | | | | | | | | | | | | | | | | | | | | | | | | | ]
         *
         * stream unprocessed (interleaved channels) | stream processed (left then right channel)
         * [L|R|L|R|L|R|L|R|L|R|L|R|L|R]               [L|L|L|L|L|L|L|R|R|R|R|R|R|R]
         *
         * write out processed stream only
        */
        usize stream_buffer_half_sample_count =
            stream_buffer_sample_count / 2;
        usize stream_buffer_processed_half_count =
            stream_buffer_half_sample_count / 2;

        usize stream_buffer_half_size =
            stream_buffer_half_sample_count * CHANNEL_SIZE;

        i16* unprocessed_buffer = stream_buffer_samples;
        i16* processed_buffer   =
            unprocessed_buffer + stream_buffer_half_sample_count;

        i16* left_channel_buffer  = processed_buffer;
        i16* right_channel_buffer =
            processed_buffer + stream_buffer_processed_half_count;

        usize file_buffer_left_offset  = package_buffer_offset;
        usize file_buffer_right_offset =
            file_buffer_left_offset + resource.audio.right_channel_buffer_offset;

        usize remaining_bytes = audio_data.buffer_size;
        while( remaining_bytes ) {
            usize max_read_bytes =
                remaining_bytes % stream_buffer_half_size;
            if( !max_read_bytes ) {
                max_read_bytes = min( stream_buffer_half_size, remaining_bytes );
            }

            fs_file_read( file_resource, max_read_bytes, unprocessed_buffer );
            usize channel_size = max_read_bytes / audio_data.number_of_channels;
            usize sample_count = max_read_bytes / file_resource_sample_size;

            for( usize i = 0; i < sample_count; ++i ) {
                usize unprocessed_index = i * 2;

                i16* src = unprocessed_buffer + unprocessed_index;

                i16* dst_left  = left_channel_buffer  + i;
                i16* dst_right = right_channel_buffer + i;

                *dst_left  = *( src + 0 );
                *dst_right = *( src + 1 );
            }

            usize offset_addend = sample_count * file_resource_sample_size;

            fs_file_set_offset( file_output, file_buffer_left_offset );
            fs_file_write( file_output, channel_size, left_channel_buffer );

            fs_file_set_offset( file_output, file_buffer_right_offset );
            fs_file_write( file_output, channel_size, right_channel_buffer );

            file_buffer_left_offset  += offset_addend;
            file_buffer_right_offset += offset_addend;

            remaining_bytes -= max_read_bytes;
        }

    } else {
        fs_file_set_offset( file_output, package_buffer_offset );

        usize remaining_samples = resource.audio.sample_count;
        while( remaining_samples ) {
            usize max_samples_to_copy =
                remaining_samples % stream_buffer_sample_count;
            if( !max_samples_to_copy ) {
                max_samples_to_copy =
                    min( stream_buffer_sample_count, remaining_samples );
            }

            usize read_size = max_samples_to_copy * file_resource_sample_size;
            fs_file_read( file_resource, read_size, stream_buffer_samples );
            fs_file_write( file_output, read_size, stream_buffer_samples );

            remaining_samples -= max_samples_to_copy;
        }
    }

    // write out liquid resource
    fs_file_set_offset( file_output, package_resource_offset );
    fs_file_write( file_output, sizeof( resource ), &resource );

    rp_note( "packaged '{cc}'!", path_resource );
    fs_file_close( file_resource );

#endif
}

void packager_resource_process( void* user_params ) {
    struct PackagerResourceProcessParams* params = user_params;
    assert( params->stream_buffer_size / 4 );

    usize index = params->index;

    FSFile* file_output = fs_file_open(
        params->tmp_output_path, FS_FILE_WRITE | FS_FILE_SHARE_WRITE );
    if( !file_output ) {
        rp_error( "failed to open output file!" );
        return;
    }

    ManifestResource* manifest_resource =
        list_index( params->manifest_resources, params->index );

    usize package_buffer_start_offset =
        sizeof( struct LiquidPackageHeader ) +
        ( sizeof( struct LiquidPackageResource ) * params->manifest_resources->count );
    usize package_resource_offset =
        sizeof( struct LiquidPackageHeader ) +
        ( sizeof( struct LiquidPackageResource ) * params->index );

    StringSlice resource_file_extension = {0};
    for( usize i = manifest_resource->path.len; i-- > 0; ) {
        resource_file_extension.len++;
        if( manifest_resource->path.buffer[i] == '.' ) {
            resource_file_extension.buffer = manifest_resource->path.buffer + i;
            break;
        }
    }

    switch( manifest_resource->type ) {
        case LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO: {
            ResourceAudioFileType file_type =
                ___determine_audio_file_type( &resource_file_extension );
            ___process_audio(
                params, manifest_resource->path.buffer,
                file_output, file_type,
                package_buffer_start_offset,
                package_resource_offset );
        } break;
        default: unimplemented();
    }

    read_write_fence();
    usize pending_work_count = thread_work_query_pending_count();
    // NOTE(alicia): == 1 because this function
    // counts as one piece of work
    if( pending_work_count == 1 ) {
        semaphore_signal( params->finished );
    }

}

