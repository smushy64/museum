#if !defined(LD_MEDIA_AUDIO_H)
#define LD_MEDIA_AUDIO_H
/**
 * Description:  Audio Output.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 02, 2024
*/
#include "shared/defines.h"

/// Format that audio system expects.
typedef struct MediaAudioBufferFormat {
    u8 channel_count;
    u8 bits_per_sample;
    u32 samples_per_second;
    u32 buffer_sample_count;
    u32 buffer_size;
} MediaAudioBufferFormat;

/// Buffer to write into when playing a sound.
typedef struct MediaAudioBuffer {
    u32 sample_count;
    u32 buffer_size;
    void* buffer;
} MediaAudioBuffer;

/// Size of opaque (platform-specific data) in audio context.
#define MEDIA_AUDIO_CONTEXT_OPAQUE_DATA_SIZE (128)

/// Audio context.
typedef struct MediaAudioContext {
    u8 opaque[MEDIA_AUDIO_CONTEXT_OPAQUE_DATA_SIZE];
} MediaAudioContext;

/// Initialize audio context.
MEDIA_API b32 media_audio_initialize( u64 buffer_length_ms, MediaAudioContext* out_ctx );
/// Shutdown audio context.
MEDIA_API void media_audio_shutdown( MediaAudioContext* ctx );
/// Check if audio context has been initialized.
MEDIA_API b32 media_audio_is_context_valid( MediaAudioContext* ctx );
/// Query the format that the output buffer expects.
MEDIA_API MediaAudioBufferFormat media_audio_query_buffer_format( MediaAudioContext* ctx );
/// Lock the output buffer.
MEDIA_API b32 media_audio_buffer_lock( MediaAudioContext* ctx, MediaAudioBuffer* out_buffer );
/// Unlock the output buffer.
MEDIA_API void media_audio_buffer_unlock( MediaAudioContext* ctx, MediaAudioBuffer* buffer );
/// Start playing the output buffer to the user's audio device.
MEDIA_API void media_audio_start( MediaAudioContext* ctx );
/// Stop playing the output buffer.
MEDIA_API void media_audio_stop( MediaAudioContext* ctx );

#endif /* header guard */
