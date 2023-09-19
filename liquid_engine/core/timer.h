#if !defined(CORE_TIME_HPP)
#define CORE_TIME_HPP
/**
 * Description:  Time
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
*/
#include "defines.h"

/// Time
typedef struct TimeStamp {
    f32 delta_seconds;
    f32 elapsed_seconds;
    u64 frame_count;
} TimeStamp;

#endif // header guard
