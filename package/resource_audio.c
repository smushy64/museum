/**
 * Description:  Package Audio implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 12, 2023
*/
#include "shared/defines.h"
#include "core/fs.h"
#include "package/resource_audio.h"

#include "package/logging.h"
#include "package/manifest.h"
#include "package/resource_package.h"

#include "core/string.h"
#include "core/sync.h"

typedef enum InputAudioCompression : u32 {
    INPUT_AUDIO_COMPRESSION_NONE,
} InputAudioCompression;

struct AudioInfo {
    u16 number_of_channels;
    u32 buffer_offset;
    u32 buffer_size;
    InputAudioCompression compression;
};

global u64 global_hash_ext_wav = 0;
global b32 global_audio_file_type_hashes_generated = false;

internal void ___generate_audio_file_format_hashes(void) {
    if( global_audio_file_type_hashes_generated ) {
        return;
    }
    interlocked_exchange( &global_audio_file_type_hashes_generated, true );

    global_hash_ext_wav = cstr_hash( ".wav" );
}

internal b32 ___get_wave_info( FSFile* input, struct AudioInfo* out_info );

b32 package_audio(
    FSFile* output_file, usize buffer_start_offset,
    usize index, struct ManifestItem* item,
    usize buffer_size, void* buffer,
    struct LiquidPackageResource* out_resource
) {
    unused(index);

    // determine audio file type
    const char* input_file_path = item->path;
    StringSlice ext = string_slice_from_cstr( 0, input_file_path );

    /* clip ext down to just file extension */ {
        usize dot_position = 0;
        for( usize i = ext.len; i-- > 0; ) {
            if( ext.buffer[i] == '.' ) {
                dot_position = i;
                break;
            }
        }

        if( !dot_position ) {
            lp_error( "unrecognized file type: '{cc}'", input_file_path );
            return false;
        }

        ext.buffer += dot_position;
        ext.len    -= dot_position;
    }

    ___generate_audio_file_format_hashes();

    FSFile* input_file =
        fs_file_open( input_file_path, FS_FILE_READ | FS_FILE_SHARE_READ );
    if( !input_file ) {
        lp_error( "failed to open file at path '{cc}'!", input_file_path );
        return false;
    }

    struct AudioInfo audio_info = {};

    u64 ext_hash = string_slice_hash( ext );
    if( ext_hash == global_hash_ext_wav ) {
        if( !___get_wave_info( input_file, &audio_info ) ) {
            return false;
        }
    } else {
        lp_error( "unrecognized file extension: '{s}'", ext );
        return false;
    }

    usize offset = output_file_allocate( audio_info.buffer_size );
    
    out_resource->offset = offset;
    out_resource->size   = audio_info.buffer_size;

    offset += buffer_start_offset;

    fs_file_set_offset( output_file, offset );
    fs_file_set_offset( input_file, audio_info.buffer_offset );

    if( audio_info.number_of_channels > 1 ) {
        out_resource->audio.right_channel_buffer_offset = out_resource->size / 2;
    }

    unused(buffer_size);
    unused(buffer);
    unused(out_resource);

    fs_file_close( input_file );

    return true;
}

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

#define wave_chunk_id( str4 )\
    ( (str4)[3] << 24u | (str4)[2] << 16u | (str4)[1] << 8u | (str4)[0] )

#define CHUNK_RIFF wave_chunk_id( "RIFF" )
#define CHUNK_WAVE wave_chunk_id( "WAVE" )
#define CHUNK_FMT  wave_chunk_id( "fmt " )
#define CHUNK_DATA wave_chunk_id( "data" )

struct WaveFMTHeader {
    u16 audio_format;
    u16 number_of_channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_alignment;
    u16 bits_per_sample;
};

internal b32 ___get_wave_info(
    FSFile* input, struct AudioInfo* out_info
) {
    struct AudioInfo audio_info = {};

    b32 read_fmt  = false;
    b32 read_data = false;

    #define wave_read( value ) do {\
        if( !fs_file_read( input, sizeof( value ), &value ) ) {\
            lp_error( "failed to read wave file!" );\
            return false;\
        }\
    } while(0)
    #define wave_read_no_advance( value ) do {\
        if( !fs_file_read( input, sizeof( value ), &value ) ) {\
            lp_error( "failed to read wave file!" );\
            return false;\
        }\
        usize current_offset = fs_file_query_offset( input );\
        fs_file_set_offset( input, current_offset - sizeof( value ) );\
    } while(0)
    #define wave_advance( bytes ) do {\
        usize current_offset = fs_file_query_offset( input );\
        fs_file_set_offset( input, current_offset + bytes );\
    } while(0)

    RiffHeader riff = {};
    wave_read( riff );
    if( riff.id != CHUNK_RIFF ) {
        lp_error( "wave file is invalid! missing RIFF chunk!" );
        return false;
    }
    if( riff.type != CHUNK_WAVE ) {
        lp_error( "wave file is invalid! missing WAVE chunk!" );
        return false;
    }

    u32 bits_per_sample = 0;
    u32 sample_rate     = 0;
    while( !( read_fmt && read_data ) ) {
        ChunkID chunk = {};
        wave_read( chunk );
        if( chunk.id == CHUNK_FMT ) {
            struct WaveFMTHeader fmt_header = {};
            wave_read_no_advance( fmt_header );
            wave_advance( chunk.size );

            audio_info.number_of_channels = fmt_header.number_of_channels;

            bits_per_sample = fmt_header.bits_per_sample;
            sample_rate     = fmt_header.sample_rate;

            read_fmt = true;
            continue;
        } else if( chunk.id == CHUNK_DATA ) {
            audio_info.buffer_offset = fs_file_query_offset( input );
            audio_info.buffer_size   = chunk.size;

            read_data = true;
            continue;
        }

        wave_advance( chunk.size );
    }

    if( audio_info.number_of_channels > 2 ) {
        lp_error(
            "wave file has unsupported number of channels: {u16}",
            audio_info.number_of_channels );
        return false;
    }
    if( bits_per_sample != 16 ) {
        lp_error(
            "wave file has unsupported bits per sample: {u32}",
            bits_per_sample );
        return false;
    }
    if( sample_rate != LIQUID_PACKAGE_RESOURCE_AUDIO_SAMPLES_PER_SECOND ) {
        lp_error( "wave file has unsupported sample rate: {u32}", sample_rate );
        return false;
    }

    *out_info = audio_info;
    return true;
}

