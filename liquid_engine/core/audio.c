/**
 * Description:  Audio API Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 27, 2023
*/
#include "defines.h"
#include "core/audio.h"
#include "core/time.h"
#include "core/logging.h"
#include "core/memory.h"
#include "core/thread.h"
#include "core/math.h"
#include "core/internal.h"

// TODO(alicia): start debug code!!!!

struct AudioBuffer {
    u8    number_of_channels;
    u8    bytes_per_sample;
    u32   samples_per_second;
    usize sample_count;
    usize buffer_size;
    void* buffer;

    usize running_sample_index;
};

#define MAKE_RIFF( c0, c1, c2, c3 ) ( (c3) << 24u | (c2) << 16u | (c1) << 8u | (c0) )
#define WAVE_FILE_AUDIO_CHUNK_ID      MAKE_RIFF( 'R', 'I', 'F', 'F' )
#define WAVE_FILE_AUDIO_FORMAT        MAKE_RIFF( 'W', 'A', 'V', 'E' )
#define WAVE_FILE_AUDIO_SUBCHUNK_1_ID MAKE_RIFF( 'f', 'm', 't', ' ' )
#define WAVE_FILE_AUDIO_SUBCHUNK_2_ID MAKE_RIFF( 'd', 'a', 't', 'a' )
#define WAVE_FILE_AUDIO_PCM (1)
struct no_padding WaveFileHeader {
    struct RiffHeader {
        union {
            u32  chunk_id;
            char chunk_id_ascii[4];
        };
        u32 chunk_size;
        union {
            u32  format;
            char format_ascii[4];
        };
    } riff;

    struct FMTHeader {
        union {
            u32  subchunk_1_id;
            char subchunk_1_id_ascii[4];
        };
        u32 subchunk_1_size;
        u16 audio_format;
        u16 number_of_channels;
        u32 sample_rate;
        u32 byte_rate;
        u16 block_alignment;
        u16 bits_per_sample;
    } fmt;

    struct DataHeader {
        union {
            u32  subchunk_2_id;
            char subchunk_2_id_ascii[4];
        };
        u32 subchunk_2_size;
    } data;
};

internal
b32 ___debug_load_audio( struct AudioBuffer* out_buffer ) {

    const char* audio_test_path = "./resources/audio_test.wav";

    PlatformFile* file = platform->io.file_open(
        audio_test_path,
        PLATFORM_FILE_READ |
        PLATFORM_FILE_SHARE_READ |
        PLATFORM_FILE_ONLY_EXISTING );
    if( !file ) {
        goto load_audio_fail;
    }

    struct AudioBuffer    audio_buffer = {};
    struct WaveFileHeader wave_header  = {};

    if( !platform->io.file_read( file, sizeof(wave_header), &wave_header ) ) {
        error_log( "Failed to read file at path '{cc}'!", audio_test_path );
        goto load_audio_fail;
    }

    if( wave_header.riff.chunk_id != WAVE_FILE_AUDIO_CHUNK_ID ) {
        error_log( "Audio file is not a wave file or is malformed!" );
        goto load_audio_fail;
    }

    if( wave_header.riff.format != WAVE_FILE_AUDIO_FORMAT ) {
        error_log( "Audio file is not a wave file or is malformed!" );
        goto load_audio_fail;
    }

    if( wave_header.fmt.subchunk_1_id != WAVE_FILE_AUDIO_SUBCHUNK_1_ID ) {
        error_log( "Audio file is not a wave file or is malformed!" );
        goto load_audio_fail;
    }

    if( wave_header.data.subchunk_2_id != WAVE_FILE_AUDIO_SUBCHUNK_2_ID ) {
        error_log( "Audio file is not a wave file or is malformed!" );
        goto load_audio_fail;
    }

    audio_buffer.number_of_channels = wave_header.fmt.number_of_channels;
    audio_buffer.bytes_per_sample =
        wave_header.fmt.number_of_channels *
        (wave_header.fmt.bits_per_sample / 8);
    audio_buffer.samples_per_second = wave_header.fmt.sample_rate;

    audio_buffer.buffer_size  = wave_header.data.subchunk_2_size;
    audio_buffer.sample_count =
        audio_buffer.buffer_size / audio_buffer.bytes_per_sample;

    if( audio_buffer.buffer_size % audio_buffer.bytes_per_sample != 0 ) {
        error_log(
            "Audio file header is corrupted! buffer size does not match actual size!" );
        goto load_audio_fail;
    }

    audio_buffer.buffer = system_alloc( audio_buffer.buffer_size );
    if( !audio_buffer.buffer ) {
        error_log(
            "Failed to allocate {f,b} for audio buffer!",
            (f32)audio_buffer.buffer_size );
        goto load_audio_fail;
    }

    if( !platform->io.file_read( file, audio_buffer.buffer_size, audio_buffer.buffer ) ) {
        error_log( "Failed to read audio bytes!" );
        goto load_audio_fail;
    }

    *out_buffer = audio_buffer;
    platform->io.file_close( file );
    return true;

load_audio_fail:
    system_free( audio_buffer.buffer, audio_buffer.buffer_size );
    platform->io.file_close( file );
    return false;
}

// TODO(alicia): end debug code!!!!

// TODO(alicia): this creates a massive
// buffer for mixing sound, maybe make this 
// buffer smaller or the mixing buffer smaller?
#define AUDIO_BUFFER_LENGTH_MS (500)

global PlatformAudioContext* global_audio_ctx;
global b32 global_audio_is_playing = false;

struct AudioMixer {
    f32 master_volume;
    f32 sfx_volume;
    f32 music_volume;

    void* buffer;
    usize buffer_size;
    PlatformAudioBufferFormat format;
};

global struct AudioMixer global_audio_mixer  = {};

f32 ___audio_01_to_log( f32 in_volume ) {
    // f32 volume01 = clamp01( volume );
    // f32 logarithmic = logarithm10( volume01 ) * 20.0f;
    // global_master_volume = logarithmic;
    f32 volume = clamp01( in_volume );
    return volume;
}
f32 ___audio_log_to_01( f32 in_volume ) {
    return in_volume;
}

LD_API void audio_set_master_volume( f32 volume ) {
    global_audio_mixer.master_volume = ___audio_01_to_log( volume );
}
LD_API f32 audio_query_master_volume(void) {
    return global_audio_mixer.master_volume;
}
LD_API f32 audio_query_master_volume_linear(void) {
    return ___audio_log_to_01( global_audio_mixer.master_volume );
}
LD_API void audio_set_music_volume( f32 volume ) {
    global_audio_mixer.music_volume = ___audio_01_to_log( volume );
}
LD_API f32 audio_query_music_volume(void) {
    return global_audio_mixer.music_volume;
}
LD_API f32 audio_query_music_volume_linear(void) {
    return ___audio_log_to_01( global_audio_mixer.music_volume );
}
LD_API void audio_set_sfx_volume( f32 volume ) {
    global_audio_mixer.sfx_volume = ___audio_01_to_log( volume );
}
LD_API f32 audio_query_sfx_volume(void) {
    return global_audio_mixer.sfx_volume;
}
LD_API f32 audio_query_sfx_volume_linear(void) {
    return ___audio_log_to_01( global_audio_mixer.sfx_volume );
}

f32 ___audio_music_volume(void) {
    return global_audio_mixer.master_volume * global_audio_mixer.music_volume;
}
f32 ___audio_sfx_volume(void) {
    return global_audio_mixer.master_volume * global_audio_mixer.sfx_volume;
}

global struct AudioBuffer global_tmp_buffer = {};
b32 audio_subsystem_initialize(void) {
    global_audio_ctx = platform->audio.initialize( AUDIO_BUFFER_LENGTH_MS );
    if( !global_audio_ctx ) {
        fatal_log( "Failed to initialize audio subsystem!" );
        return false;
    }

    global_audio_mixer.master_volume = 0.1f;
    global_audio_mixer.music_volume  = 1.0f;
    global_audio_mixer.sfx_volume    = 1.0f;

    global_audio_is_playing = true;

    global_audio_mixer.format =
        platform->audio.query_buffer_format( global_audio_ctx );

    if( !___debug_load_audio( &global_tmp_buffer ) ) {
        return false;
    }

    info_log( "Audio subsystem initialized." );
    return true;
}

usize audio_subsystem_query_memory_requirement(void) {
    usize sample_size =
        global_audio_mixer.format.number_of_channels * sizeof(f32);

    usize sample_count = global_audio_mixer.format.buffer_sample_count;
    usize buffer_size  = sample_count * sample_size;

    global_audio_mixer.buffer_size = buffer_size;
    return buffer_size;
}
void audio_subsystem_submit_buffer_memory( void* buffer ) {
    global_audio_mixer.buffer = buffer;
}

internal
void ___audio_fill_buffer( usize samples_to_fill ) {

    f32 music_volume = ___audio_music_volume();

    i16* tmp_samples = (i16*)global_tmp_buffer.buffer;

    f32* samples = (f32*)global_audio_mixer.buffer;
    for( usize i = 0; i < samples_to_fill; ++i ) {

        i16* tmp_current_sample =
            tmp_samples + ( global_tmp_buffer.running_sample_index * 2 );
        i16 left_sample  = *( tmp_current_sample + 0 );
        i16 right_sample = *( tmp_current_sample + 1 );

        usize sample_index = i * 2;

        *(samples + sample_index + 0) =
            normalize_range_i16_f32( left_sample ) * music_volume;
        *(samples + sample_index + 1) =
            normalize_range_i16_f32( right_sample ) * music_volume;

        global_tmp_buffer.running_sample_index =
            (global_tmp_buffer.running_sample_index + 1) %
            global_tmp_buffer.sample_count;
    }
}

// global f64 audio_last_time = 0.0f;
void audio_subsystem_output(void) {

    if( !global_audio_is_playing ) {
        return;
    }

    usize out_sample_count = 0;
    usize out_buffer_size  = 0;
    void* out_buffer       = NULL;

    if( !platform->audio.lock_buffer(
        global_audio_ctx, &out_sample_count,
        &out_buffer_size, &out_buffer
    ) ) {
        return;
    }

    ___audio_fill_buffer( out_sample_count );
    i16* out_sample = (i16*)out_buffer;

    f32* in_sample = (f32*)global_audio_mixer.buffer;
    for( usize i = 0; i < out_sample_count; ++i ) {
        f32* current_sample = in_sample + (i * 2);

        i16 left_sample  = normalize_range32_i16( *(current_sample + 0) );
        i16 right_sample = normalize_range32_i16( *(current_sample + 1) );

        *out_sample++ = left_sample;
        *out_sample++ = right_sample;
    }

    platform->audio.unlock_buffer( global_audio_ctx, out_sample_count );
}

void audio_subsystem_shutdown(void) {
    platform->audio.shutdown( global_audio_ctx );
    info_log( "Audio subsystem shutdown." );
}

void audio_subsystem_start(void) {
    if( global_audio_ctx ) {
        global_audio_is_playing = true;
        read_write_fence();
        platform->audio.start( global_audio_ctx );
    }
}
void audio_subsystem_stop(void) {
    if( global_audio_ctx ) {
        global_audio_is_playing = false;
        read_write_fence();
        platform->audio.stop( global_audio_ctx );
    }
}

