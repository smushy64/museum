// * Description:  Audio Subsystem Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "audio.h"
#include "platform/platform.h"
#include "core/logging.h"

global b32 sound_is_playing = true;

b32 audio_init( struct Platform* platform ) {
    if( !platform_init_audio( platform ) ) {
        return false;
    }
    LOG_INFO( "Audio subsystem successfully initialized." );
    return true;
}

void audio_test( struct Platform* platform ) {
    if( sound_is_playing ) {
        platform_audio_test( platform, 500 );
    }
}

void audio_shutdown( struct Platform* platform ) {
    platform_shutdown_audio( platform );
    LOG_INFO( "Audio subsystem shutdown." );
}

