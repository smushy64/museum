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
};

struct AudioBuffer {
    void* buffer;
    usize buffer_size;

    PlatformAudioBufferFormat format;
};

global struct AudioBuffer global_audio_buffer = {};
global struct AudioMixer  global_audio_mixer  = {};

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

    global_audio_buffer.format =
        platform->audio.query_buffer_format( global_audio_ctx );

    info_log( "Audio subsystem initialized." );
    return true;
}

usize audio_subsystem_query_memory_requirement(void) {
    usize sample_size =
        global_audio_buffer.format.number_of_channels * sizeof(f32);

    usize sample_count = global_audio_buffer.format.buffer_sample_count;
    usize buffer_size  = sample_count * sample_size;

    global_audio_buffer.buffer_size = buffer_size;
    return buffer_size;
}
void audio_subsystem_submit_buffer_memory( void* buffer ) {
    global_audio_buffer.buffer = buffer;
}

global f32 t_sine = 0.0f;
internal
void ___audio_fill_buffer( usize samples_to_fill ) {
    i32 wave_period  = global_audio_buffer.format.samples_per_second / 256;
    f32* samples = (f32*)global_audio_buffer.buffer;
    for( usize i = 0; i < samples_to_fill; ++i ) {
        f32 sine_value   = sine( t_sine ) * 0.25f;
        f32 sample_value = sine_value * global_audio_mixer.master_volume;

        usize sample_index = i * 2;

        *(samples + sample_index + 0) = sample_value;
        *(samples + sample_index + 1) = sample_value;

        t_sine += 2.0f * F32_TAU / (f32)wave_period;
    }
}

// global f64 audio_last_time = 0.0f;
void audio_subsystem_output(void) {
    unused(t_sine);

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

    f32* in_sample = (f32*)global_audio_buffer.buffer;
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

