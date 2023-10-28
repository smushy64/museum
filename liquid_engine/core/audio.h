#if !defined(LD_CORE_AUDIO_H)
#define LD_CORE_AUDIO_H
/**
 * Description:  Audio API for Liquid Engine
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 27, 2023
*/
#include "defines.h"

LD_API void audio_set_master_volume( f32 volume );
LD_API f32  audio_query_master_volume(void);

b32 audio_subsystem_initialize(void);

void audio_subsystem_fill_buffer(void);

void audio_subsystem_shutdown(void);

#endif /* header guard */
