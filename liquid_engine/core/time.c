/**
 * Description:  Time implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 23, 2023
*/
#include "defines.h"
#include "core/time.h"
#include "core/math.h"
#include "core/internal.h"

struct FrameTimestamp {
    f64 delta_time;
    f64 time_scale;
    f64 elapsed_time;
    u64 frame_count;
};
global f64 UPDATE_PLATFORM_TIME_TIMER = 0.0f;
global struct FrameTimestamp FRAME_TIMESTAMP = {};
global PlatformTime PLATFORM_TIME = {};

void time_subsystem_initialize(void) {
    FRAME_TIMESTAMP.time_scale = 1.0;

    PLATFORM_TIME = platform->time.query_system_time();
}
void time_subsystem_update(void) {
    f64 elapsed_seconds = platform->time.elapsed_seconds();
    
    FRAME_TIMESTAMP.delta_time   = elapsed_seconds - FRAME_TIMESTAMP.elapsed_time;
    FRAME_TIMESTAMP.elapsed_time = elapsed_seconds;

    FRAME_TIMESTAMP.frame_count++;

    UPDATE_PLATFORM_TIME_TIMER += FRAME_TIMESTAMP.delta_time;
    if( UPDATE_PLATFORM_TIME_TIMER >= 1.0f ) {
        PLATFORM_TIME = platform->time.query_system_time();
    }
}

LD_API f64 time_delta(void) {
    return FRAME_TIMESTAMP.delta_time * FRAME_TIMESTAMP.time_scale;
}
LD_API f64 time_unscaled_delta(void) {
    return FRAME_TIMESTAMP.delta_time;
}
LD_API f64 time_elapsed(void) {
    return FRAME_TIMESTAMP.elapsed_time;
}
LD_API f64 time_query_scale(void) {
    return FRAME_TIMESTAMP.time_scale;
}
LD_API void time_set_scale( f64 time_scale ) {
    FRAME_TIMESTAMP.time_scale = absolute( time_scale );
}
LD_API u64 time_frame_count(void) {
    return FRAME_TIMESTAMP.frame_count;
}

LD_API u32 time_query_year(void) {
    return PLATFORM_TIME.year;
}
LD_API u32 time_query_month(void) {
    return PLATFORM_TIME.month;
}
LD_API u32 time_query_day(void) {
    return PLATFORM_TIME.day;
}
LD_API u32 time_query_hour(void) {
    return PLATFORM_TIME.hour;
}
LD_API void time_query_hour_12hr( u32* out_hour, b32* out_is_am ) {
    u32 hr24   = time_query_hour();
    u32 hr12   = hr24 % 12;
    hr12 = hr12 ? hr12 : 12;
    *out_hour  = hr12;
    *out_is_am = hr24 < 12;
}
LD_API u32 time_query_minute(void) {
    return PLATFORM_TIME.minute;
}
LD_API u32 time_query_second(void) {
    return PLATFORM_TIME.second;
}

