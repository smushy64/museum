#if !defined(LD_CORE_AUDIO_H)
#define LD_CORE_AUDIO_H
/**
 * Description:  Audio API for Liquid Engine
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 27, 2023
*/
#include "defines.h"

b32 audio_subsystem_initialize(void);

void audio_subsystem_fill_buffer(void);

void audio_subsystem_shutdown(void);

#endif /* header guard */
