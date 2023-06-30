#if !defined(CORE_AUDIO_HPP)
#define CORE_AUDIO_HPP
// * Description:  Audio Subsystem
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "defines.h"

#define AUDIO_KHZ             48000
#define AUDIO_CHANNEL_COUNT   2
#define AUDIO_BITS_PER_SAMPLE 16
#define AUDIO_BYTES_PER_SAMPLE \
    (sizeof(i16) * AUDIO_CHANNEL_COUNT)
#define AUDIO_BUFFER_SIZE \
    (AUDIO_KHZ * AUDIO_BYTES_PER_SAMPLE)

void audio_test( struct Platform* platform );

#if defined( LD_API_INTERNAL )
    /// Initialize audio subsystem.
    b32 audio_init( struct Platform* platform );
    /// Shutdown audio subsystem.
    void audio_shutdown( struct Platform* platform );
#endif

#endif // header guard
