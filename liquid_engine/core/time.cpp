/**
 * Description:  Time implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 06, 2023
*/
#include "time.h"
#include "platform/platform.h"

void time_update( Time* time ) {
    f64 seconds_elapsed = platform_seconds_elapsed();
    time->delta_time    = seconds_elapsed - time->elapsed_time;
    time->elapsed_time  = seconds_elapsed;
    time->counter++;
}
