/**
 * Description:  Time implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 05, 2023
*/
#include "shared/defines.h"
#include "core/time.h"
#include "core/internal/platform.h"

global f64 global_time_scale     = 1.0;
global f64 global_time_elapsed   = 0.0;
global f64 global_time_delta     = 0.0;
global u64 global_update_counter = 0;

CORE_API void time_initialize(void) {
    platform_time_initialize();
}
CORE_API void time_update(void) {
    f64 elapsed = platform_time_query_elapsed_seconds();

    global_time_delta   = elapsed - global_time_elapsed;
    global_time_elapsed = elapsed;
    global_update_counter++;
}

CORE_API f64 time_delta_seconds(void) {
    return global_time_delta * global_time_scale;
}
CORE_API f64 time_unscaled_delta_seconds(void) {
    return global_time_delta;
}
CORE_API f64 time_elapsed_seconds(void) {
    return global_time_elapsed;
}
CORE_API u64 time_query_update_count(void) {
    return global_update_counter;
}
CORE_API f64 time_query_scale(void) {
    return global_time_scale;
}
CORE_API void time_set_scale( f64 new_scale ) {
    global_time_scale = new_scale;
}

CORE_API TimeRecord time_record(void) {
    TimeRecord result = {};
    platform_time_record( &result );
    return result;
}

CORE_API void time_hour_24_to_hour_12(
    u32 hour_24, u32* out_hour_12, b32* out_is_am
) {
    u32 hour_12 = hour_24 % 12;
    hour_12 = hour_12 ? hour_12 : 12;

    *out_hour_12 = hour_12;
    if( out_is_am ) {
        *out_is_am = hour_24 < 12;
    }
}

