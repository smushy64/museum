#if !defined(LD_CORE_AUDIO_H)
#define LD_CORE_AUDIO_H
/**
 * Description:  Audio API for Liquid Engine
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 27, 2023
*/
#include "defines.h"

/// Set master volume.
/// Volume must be a value from 0.0-1.0.
/// Volume is a linear value that gets transformed into decibels.
LD_API void audio_set_master_volume( f32 volume );
/// Query the current value of master volume.
/// Value returned is in decibels.
LD_API f32 audio_query_master_volume(void);
/// Query the current value of master volume.
/// The value returned is a linear value from 0.0-1.0.
LD_API f32 audio_query_master_volume_linear(void);
/// Set music volume.
/// Volume must be a value from 0.0-1.0.
/// Volume is a linear value that gets transformed into decibels.
LD_API void audio_set_music_volume( f32 volume );
/// Query the current value of music volume.
/// Value returned is in decibels.
LD_API f32 audio_query_music_volume(void);
/// Query the current value of music volume.
/// The value returned is a linear value from 0.0-1.0.
LD_API f32 audio_query_music_volume_linear(void);
/// Set sfx volume.
/// Volume must be a value from 0.0-1.0.
/// Volume is a linear value that gets transformed decibels.
LD_API void audio_set_sfx_volume( f32 volume );
/// Query the current value of sfx volume.
/// Value returned is in decibels.
LD_API f32 audio_query_sfx_volume(void);
/// Query the current value of sfx volume.
/// The value returned is a linear value from 0.0-1.0.
LD_API f32 audio_query_sfx_volume_linear(void);

LD_API void audio_debug_play_sound(void);

#if defined(LD_API_INTERNAL)

/// Initialize the audio subsystem.
b32 audio_subsystem_initialize(void);
/// Query the memory requirement for the audio subsystem.
/// This function must be called after the subsystem has been initialized.
usize audio_subsystem_query_memory_requirement(void);
/// Submit buffer for audio subsystem.
/// This buffer must be large enough to hold
/// the result of audio_subsystem_query_memory_requirement.
void audio_subsystem_submit_buffer_memory( void* buffer );
/// Mixes all in-game audio and fills output audio buffer.
void audio_subsystem_output(void);
/// Shutdown audio subsystem.
void audio_subsystem_shutdown(void);
/// Resume playing audio buffer.
void audio_subsystem_resume(void);
/// Pause playing audio buffer.
void audio_subsystem_pause(void);

#endif /* API internal */

#endif /* header guard */
