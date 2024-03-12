/**
 * Description:  Process audio files.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 11, 2024
*/
#include "shared/defines.h"
#include "shared/liquid_package.h"

#include "core/rand.h"
#include "core/path.h"
#include "core/fs.h"
#include "core/compression.h"
#include "core/memory.h"

#include "package/manifest.h"
#include "package/logging.h"
#include "package/resource.h"

#include "generated/package_hashes.h"

typedef enum AudioFormat : u32 {
    AUDIO_FORMAT_PCM_INTERLACED,
} AudioFormat;

struct AudioInfo {
    u32 channel_count;
    u32 bytes_per_sample;
    u32 samples_per_second;
    u32 buffer_offset;
    u32 buffer_size;
    AudioFormat format;
};

b32 parse_audio_info_wave(
    usize thread_index, FileHandle* file, struct AudioInfo* out_info );

b32 process_resource_audio(
    usize thread_index, ManifestItem* item, struct PackageResource* out_resource,
    FileHandle* input, FileHandle* output, usize buffer_size, void* buffer
) {
    #define read( src, size, dst ) do {\
        if( !fs_file_read( src, size, dst ) ) {\
            log_error( "failed to read audio file!" );\
            debug_break();\
            return false;\
        }\
    } while(0)
    #define write( dst, size, src ) do {\
        if( !fs_file_write( dst, size, src ) ) {\
            debug_break();\
            log_error( "failed to write to destination file!" );\
            return false;\
        }\
    } while(0)

    PathSlice ext = {};
    if( !path_slice_get_extension( item->path, &ext ) ) {
        log_error( "path does not have a file extension!" );
        return false;
    }
    u64 ext_hash = string_slice_hash( ext );

    struct AudioInfo audio_info = {};

    switch( ext_hash ) {
        case HASH_EXT_WAVE: {
            if( !parse_audio_info_wave( thread_index, input, &audio_info ) ) {
                return false;
            }
        } break;
        default: {
            log_error(
                "path does not have a recognized file extension! ext: {p}", ext );
        } return false;
    }

    // NOTE(alicia): read in audio data and write to temp file
    // audio data may be interlaced so write out deinterlaced data
    // to temp file

    path_buffer_empty( deinterlaced_path, 256 );
    if( string_buffer_fmt( &deinterlaced_path,
        "./pkgtemp/{u32}_deinterlaced.tmp{0}", rand_xor_u32()
    ) ) {
        PathSlice end = path_slice( ".tmp" );
        end.len++;
        deinterlaced_path.len -= end.len;
        string_buffer_append( &deinterlaced_path, end );
    }

    FileOpenFlags deinterlaced_open_flags = FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_READ;
    FileHandle* deinterlaced = fs_file_open(
        to_slice( &deinterlaced_path ),
        FILE_OPEN_FLAG_CREATE | deinterlaced_open_flags );
    if( !deinterlaced ) {
        deinterlaced = fs_file_open(
            to_slice( &deinterlaced_path ),
            FILE_OPEN_FLAG_TRUNCATE | deinterlaced_open_flags );
        if( !deinterlaced ) {
            log_error( "failed to create temp deinterlace file!" );
            return false;
        }
    }

    // TODO(alicia): if bytes per channel > 2 (i16)
    // then convert to i16 samples?
    switch( audio_info.format ) {
        case AUDIO_FORMAT_PCM_INTERLACED: {
            usize interlaced_sample_size =
                audio_info.channel_count * audio_info.bytes_per_sample;
            usize remaining_samples = audio_info.buffer_size / interlaced_sample_size;

            fs_file_set_offset( input, audio_info.buffer_offset, false );

            if( audio_info.channel_count == 1 ) {
                // NOTE(alicia): no need to deinterlace if
                // there's only one channel
                
                fs_file_to_file_copy(
                    deinterlaced, input,
                    buffer_size, buffer,
                    audio_info.buffer_size );
                break;
            }

            usize interlaced_buffer_size = buffer_size / 2;

            u8* interlaced_sample_buffer   = buffer;
            u8* deinterlaced_sample_buffer =
                interlaced_sample_buffer + interlaced_buffer_size;

            usize buffer_interlaced_sample_count =
                interlaced_buffer_size / interlaced_sample_size;

            usize deinterlaced_file_channel_offset = 0;
            usize deinterlaced_file_channel_size   = remaining_samples * audio_info.bytes_per_sample;
            while( remaining_samples ) {
                usize sample_count = buffer_interlaced_sample_count;
                if( sample_count > remaining_samples ) {
                    sample_count = remaining_samples;
                }

                read(
                    input, sample_count * interlaced_sample_size,
                    interlaced_sample_buffer );

                // deinterlace read result into
                // deinterlace buffer
                for(
                    usize sample_index = 0;
                    sample_index < sample_count;
                    ++sample_index
                ) {
                    u8* interlaced_sample =
                        interlaced_sample_buffer +
                        ( sample_index * interlaced_sample_size );

                    usize deinterlaced_sample_offset =
                        sample_index * audio_info.bytes_per_sample;
                    usize deinterlaced_channel_size =
                        sample_count * audio_info.bytes_per_sample;

                    for(
                        usize channel_index = 0;
                        channel_index < audio_info.channel_count;
                        ++channel_index
                    ) {
                        u8* channel_data =
                            interlaced_sample + ( channel_index * audio_info.bytes_per_sample );

                        u8* dst_sample =
                            deinterlaced_sample_buffer +
                            deinterlaced_sample_offset +
                            ( channel_index * deinterlaced_channel_size );

                        memory_copy( dst_sample, channel_data, audio_info.bytes_per_sample );
                    }
                }

                usize deinterlaced_buffer_channel_size =
                    sample_count * audio_info.bytes_per_sample;

                // now copy deinterlaced result to
                // deinterlaced temp file.
                for(
                    usize channel_index = 0;
                    channel_index < audio_info.channel_count;
                    ++channel_index
                ) {
                    usize dst_offset =
                        ( deinterlaced_file_channel_size * channel_index ) +
                        deinterlaced_file_channel_offset;

                    u8* channel_data =
                        deinterlaced_sample_buffer +
                        (deinterlaced_buffer_channel_size * channel_index);

                    fs_file_set_offset( deinterlaced, dst_offset, false );

                    write( deinterlaced, deinterlaced_buffer_channel_size, channel_data );
                }

                deinterlaced_file_channel_offset += deinterlaced_buffer_channel_size;
                remaining_samples -= sample_count;
            }
        } break;
    }

    // NOTE(alicia): copy contents from temp file to
    // output file while applying compression if needed.

    b32 result = true;
    fs_file_set_offset( deinterlaced, 0, false );
    switch( item->compression ) {
        case PACKAGE_COMPRESSION_RLE: {
            usize remaining   = fs_file_query_size( deinterlaced );
            u64 original_size = (u64)remaining;
            
            write( output, sizeof(original_size), &original_size );
            while( remaining ) {
                usize read_size = buffer_size;
                if( read_size > remaining ) {
                    read_size = remaining;
                }

                read( deinterlaced, read_size, buffer );

                usize encode_size = 0;
                usize not_written = compression_rle_encode(
                    package_compression_stream, output, read_size,
                    buffer, &encode_size );
                if( not_written ) {
                    log_error( "failed to write RLE compressed audio file!" );
                    result = false;
                    goto process_resource_audio_end;
                }

                remaining  -= read_size;
            }
        } break;
        case PACKAGE_COMPRESSION_NONE: {
            usize remaining = fs_file_query_size( deinterlaced );

            while( remaining ) {
                usize read_size = buffer_size;
                if( read_size > remaining ) {
                    read_size = remaining;
                }

                read( deinterlaced, read_size, buffer );
                write( output, read_size, buffer );

                remaining -= read_size;
            }
        } break;
    }

    out_resource->type                     = item->type;
    out_resource->compression              = item->compression;
    out_resource->audio.bytes_per_sample   = audio_info.bytes_per_sample;
    out_resource->audio.channel_count      = audio_info.channel_count;
    out_resource->audio.samples_per_second = audio_info.samples_per_second;

process_resource_audio_end:

    // NOTE(alicia): destroy deinterlaced temp file.
    fs_file_close( deinterlaced );
    fs_delete_file( to_slice( &deinterlaced_path ) );

    #undef read
    #undef write
    return result;
}

#define WAVE_FILE_AUDIO_PCM (1)
struct no_padding RiffHeader {
    union { u32 id; char id_c[4]; };
    u32 chunk_size;
    union { u32 type; char type_c[4]; };
};

struct no_padding ChunkID {
    union { u32 id; char id_c[4]; };
    u32 size;
};

#define wave_chunk_id( str )\
    ( (str)[3] << 24u | (str)[2] << 16u | (str)[1] << 8u | (str)[0] )

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

b32 parse_audio_info_wave(
    usize thread_index, FileHandle* file, struct AudioInfo* out_info
) {
    // TODO(alicia): determine audio format
    struct AudioInfo info = {};

    b32 read_fmt = false, read_data = false;

    #define read( val ) do {\
        if( !fs_file_read( file, sizeof(val), &val ) ) {\
            log_error( "failed to read wave file!" );\
            debug_break();\
            return false;\
        }\
    } while(0)

    #define read_inplace( val )\
        read( val );\
        fs_file_set_offset( file, fs_file_query_offset(file) - sizeof(val), false )

    #define advance( size )\
        fs_file_set_offset( file, fs_file_query_offset( file ) + size, false )

    struct RiffHeader riff = {};
    read( riff );
    if( riff.id != CHUNK_RIFF ) {
        log_error(
            "provided file has wave extension but is not a "
            "valid wave file! missing riff id!" );
        return false;
    }
    if( riff.type != CHUNK_WAVE ) {
        log_error(
            "provided file has wave extension but is not a "
            "valid wave file! missing riff wave type!" );
        return false;
    }

    while( !( read_fmt && read_data ) ) {
        struct ChunkID chunk_id = {};
        read( chunk_id );

        if( chunk_id.id == CHUNK_FMT ) {
            struct WaveFMTHeader fmt_header = {};
            read_inplace( fmt_header );

            info.channel_count      = fmt_header.number_of_channels;
            info.bytes_per_sample   = fmt_header.bits_per_sample / 8;
            info.samples_per_second = fmt_header.sample_rate;

            read_fmt = true;
        } else if( chunk_id.id == CHUNK_DATA ) {
            info.buffer_offset = fs_file_query_offset( file );
            info.buffer_size   = chunk_id.size;

            read_data = true;
        }

        advance( chunk_id.size );
    }

    #undef read
    #undef read_inplace
    #undef advance
    *out_info = info;
    return true;
}

#undef CHUNK_RIFF
#undef CHUNK_WAVE
#undef CHUNK_FMT 
#undef CHUNK_DATA
#undef wave_chunk_id
#undef CHANNEL_SIZE
