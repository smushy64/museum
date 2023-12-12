/**
 * Description:  Audio API Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 27, 2023
*/
#include "defines.h"
#include "core/sync.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/fs.h"
#include "core/thread.h"

#include "engine/audio.h"
#include "engine/logging.h"
#include "engine/internal/platform.h"

// TODO(alicia): start debug code!!!!

#include "liquid_package.h"
// #include "resource.h"

struct AudioBuffer {
    u8    number_of_channels;
    u8    bytes_per_sample;
    u32   samples_per_second;
    usize sample_count;
    usize buffer_size;
    usize right_channel_offset;
    void* buffer;
};

typedef enum AudioVoiceType : u8 {
    AUDIO_VOICE_TYPE_NONE,
    AUDIO_VOICE_TYPE_MUSIC,
    AUDIO_VOICE_TYPE_SFX
} AudioVoiceType;

struct AudioVoice {
    AudioVoiceType type;
    b8    is_playing;
    b8    loops;

    f32   volume;
    f32   pan;
    usize running_sample_index;
    struct AudioBuffer* buffer;
};

global struct AudioVoice  global_voices[2] = {};
global struct AudioBuffer global_tmp_music_buffer = {};
global struct AudioBuffer global_tmp_sfx_buffer   = {};

#define make_char( foo ) (#foo)[0]
#define make_riff( str4 ) ((str4)[3] << 24u | (str4)[2] << 16u | (str4)[1] << 8u | (str4)[0])

#define WAVE_FILE_AUDIO_CHUNK_ID      make_riff( "RIFF" )
#define WAVE_FILE_AUDIO_FORMAT        make_riff( "WAVE" )
#define WAVE_FILE_AUDIO_SUBCHUNK_1_ID make_riff( "fmt " )
#define WAVE_FILE_AUDIO_SUBCHUNK_2_ID make_riff( "data" )
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
b32 ___debug_load_audio(
    const char* audio_test_path, struct AudioBuffer* out_buffer
) {
    FSFile* file = fs_file_open(
        audio_test_path,
        FS_FILE_READ |
        FS_FILE_SHARE_READ |
        FS_FILE_ONLY_EXISTING );
    if( !file ) {
        return false;
    }
    struct AudioBuffer buffer = {};

    struct LiquidPackageHeader header = {};
    assert( fs_file_read( file, sizeof(header), &header ) );

    usize offset =
        liquid_package_calculate_resource_file_offset( 0 );
    struct LiquidPackageResource resource = {};
    fs_file_set_offset( file, offset );
    assert( fs_file_read( file, sizeof(resource), &resource ) );
    assert( resource.type == LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO );

    offset = (header.resource_count * sizeof(resource)) + sizeof(header);
    offset += resource.buffer_offset;

    fs_file_set_offset( file, offset );

    buffer.buffer_size = liquid_package_resource_audio_buffer_size( &resource.audio );
    assert( buffer.right_channel_offset < buffer.buffer_size );
    buffer.buffer = system_alloc( buffer.buffer_size );
    assert( buffer.buffer );

    buffer.number_of_channels = 2;
    buffer.bytes_per_sample   = LIQUID_PACKAGE_RESOURCE_AUDIO_BYTES_PER_CHANNEL_SAMPLE;
    buffer.samples_per_second = LIQUID_PACKAGE_RESOURCE_AUDIO_SAMPLES_PER_SECOND;
    buffer.sample_count       = resource.audio.sample_count;
    buffer.right_channel_offset = resource.audio.right_channel_buffer_offset;

    assert( fs_file_read( file, buffer.buffer_size, buffer.buffer ) );

    *out_buffer = buffer;

    fs_file_close( file );
    return true;
#if 0
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
            "Failed to allocate {f,m} for audio buffer!",
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
#endif /* if 0 */
}

// TODO(alicia): end debug code!!!!

#define AUDIO_BUFFER_LENGTH_MS (250)

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

#define AUDIO_MINIMUM_VOLUME (0.0001f)
#define AUDIO_MAXIMUM_VOLUME (1.0f)
f32 ___audio_01_to_db( f32 in_volume ) {
    f32 volume = clamp( in_volume, AUDIO_MINIMUM_VOLUME, AUDIO_MAXIMUM_VOLUME );
    // f32 logarithmic = logarithm10( volume ) * 20.0f;
    return volume;
}
f32 ___audio_db_to_01( f32 in_volume ) {
    return clamp(
        power( 10.0f, in_volume / 20.0f ),
        AUDIO_MINIMUM_VOLUME, AUDIO_MAXIMUM_VOLUME );
}

LD_API void audio_set_master_volume( f32 volume ) {
    global_audio_mixer.master_volume = ___audio_01_to_db( volume );
}
LD_API f32 audio_query_master_volume(void) {
    return global_audio_mixer.master_volume;
}
LD_API f32 audio_query_master_volume_linear(void) {
    return ___audio_db_to_01( global_audio_mixer.master_volume );
}
LD_API void audio_set_music_volume( f32 volume ) {
    global_audio_mixer.music_volume = ___audio_01_to_db( volume );
}
LD_API f32 audio_query_music_volume(void) {
    return global_audio_mixer.music_volume;
}
LD_API f32 audio_query_music_volume_linear(void) {
    return ___audio_db_to_01( global_audio_mixer.music_volume );
}
LD_API void audio_set_sfx_volume( f32 volume ) {
    global_audio_mixer.sfx_volume = ___audio_01_to_db( volume );
}
LD_API f32 audio_query_sfx_volume(void) {
    return global_audio_mixer.sfx_volume;
}
LD_API f32 audio_query_sfx_volume_linear(void) {
    return ___audio_db_to_01( global_audio_mixer.sfx_volume );
}

LD_API void audio_debug_play_sound(void) {
    note_log( "playing sfx . . ." );
    global_voices[1].is_playing           = true;
    global_voices[1].running_sample_index = 0;
}

f32 ___audio_music_volume(void) {
    return global_audio_mixer.master_volume * global_audio_mixer.music_volume;
}
f32 ___audio_sfx_volume(void) {
    return global_audio_mixer.master_volume * global_audio_mixer.sfx_volume;
}

internal
void ___audio_fill_buffer( usize samples_to_fill ) {

    struct AudioVoice* voices = global_voices;

    f32 master_volume = global_audio_mixer.master_volume;
    f32 music_volume  = ___audio_music_volume();
    f32 sfx_volume    = ___audio_sfx_volume();

    f32* samples = (f32*)global_audio_mixer.buffer;
    for( usize i = 0; i < samples_to_fill; ++i ) {

        f32 left_sample  = 0;
        f32 right_sample = 0;

        for( usize j = 0; j < 1; ++j ) {
            struct AudioVoice* voice = voices + j;
            if( !voice->is_playing ) {
                continue;
            }

            f32 volume = master_volume;
            switch( voice->type ) {
                case AUDIO_VOICE_TYPE_MUSIC: {
                    volume = music_volume;
                } break;
                case AUDIO_VOICE_TYPE_SFX: {
                    volume = sfx_volume;
                } break;
                default: break;
            }

            volume *= voice->volume;

            u8* right_channel =
                (u8*)voice->buffer->buffer + voice->buffer->right_channel_offset;

            i16* left_sample_i16 =
                (i16*)voice->buffer->buffer +
                voice->running_sample_index;
            i16* right_sample_i16 =
                (i16*)right_channel +
                voice->running_sample_index;

            f32 voice_left_sample  = normalize_range_i16_f32( *left_sample_i16 );
            f32 voice_right_sample = normalize_range_i16_f32( *right_sample_i16 );

            left_sample  += voice_left_sample  * volume;
            right_sample += voice_right_sample * volume;

            voice->running_sample_index++;
            if( voice->loops ) {
                voice->running_sample_index %= voice->buffer->sample_count;
            } else if( voice->running_sample_index >= voice->buffer->sample_count ) {
                voice->is_playing = false;
            }
        }

        usize sample_index = i * 2;

        *(samples + sample_index + 0) = left_sample;
        *(samples + sample_index + 1) = right_sample;

        // global_tmp_music_buffer.running_sample_index =
        //     (global_tmp_music_buffer.running_sample_index + 1) %
        //     global_tmp_music_buffer.sample_count;
    }
}

struct AudioMT {
    Semaphore buffer_fill;
};
global struct AudioMT global_audio_mt = {};

// global f64 audio_last_time = 0.0f;
void audio_subsystem_output(void) {
    semaphore_signal( &global_audio_mt.buffer_fill );
}

internal no_inline int on_buffer_fill( void* user_params ) {
    unused( user_params );

    read_write_fence();

    loop {
        semaphore_wait( &global_audio_mt.buffer_fill );
        read_write_fence();

        if( !global_audio_is_playing ) {
            continue;
        }

        usize out_sample_count = 0;
        usize out_buffer_size  = 0;
        void* out_buffer       = NULL;

        if( !platform->audio.lock_buffer(
            global_audio_ctx, &out_sample_count,
            &out_buffer_size, &out_buffer
        ) ) {
            continue;
        }

        ___audio_fill_buffer( out_sample_count );
        i16* out_sample = (i16*)out_buffer;

        f32* in_sample = (f32*)global_audio_mixer.buffer;
        for( usize i = 0; i < out_sample_count; ++i ) {
            f32* current_sample = in_sample + (i * 2);

            i16 left_sample  = normalize_range_f32_i16( *(current_sample + 0) );
            i16 right_sample = normalize_range_f32_i16( *(current_sample + 1) );

            *out_sample++ = left_sample;
            *out_sample++ = right_sample;
        }

        platform->audio.unlock_buffer( global_audio_ctx, out_sample_count );
    }

    return 0;
}

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

    if( !___debug_load_audio(
        "./resources.lpkg",
        &global_tmp_music_buffer
    ) ) {
        return false;
    }
    // if( !___debug_load_audio(
    //     "./resources/sfx_test.wav",
    //     &global_tmp_sfx_buffer
    // ) ) {
    //     return false;
    // }

    global_voices[0].type       = AUDIO_VOICE_TYPE_MUSIC;
    global_voices[0].is_playing = true;
    global_voices[0].loops      = true;
    global_voices[0].volume     = 0.2f;
    global_voices[0].pan        = 0.0f;
    global_voices[0].buffer     = &global_tmp_music_buffer;

    global_voices[1].type       = AUDIO_VOICE_TYPE_SFX;
    global_voices[1].is_playing = false;
    global_voices[1].loops      = false;
    global_voices[1].volume     = 0.8f;
    global_voices[1].pan        = 0.0f;
    global_voices[1].buffer     = &global_tmp_sfx_buffer;

    if( !semaphore_create( &global_audio_mt.buffer_fill ) ) {
        fatal_log( "Failed to create multithreaded audio data!" );
        return false;
    }

    read_write_fence();

    if( !thread_create( on_buffer_fill, NULL ) ) {
        fatal_log( "Failed to create audio thread!" );
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

void audio_subsystem_shutdown(void) {
    platform->audio.shutdown( global_audio_ctx );
    info_log( "Audio subsystem shutdown." );
}

void audio_subsystem_resume(void) {
    if( global_audio_ctx && !global_audio_is_playing ) {
        global_audio_is_playing = true;
        read_write_fence();
        platform->audio.start( global_audio_ctx );
    }
}
void audio_subsystem_pause(void) {
    if( global_audio_ctx && global_audio_is_playing ) {
        global_audio_is_playing = false;
        read_write_fence();
        platform->audio.stop( global_audio_ctx );
    }
}

