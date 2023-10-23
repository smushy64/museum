#if !defined(LD_CORE_TIME_H)
#define LD_CORE_TIME_H
/**
 * Description:  Time
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 23, 2023
*/
#include "defines.h"

/// Get time delta between last and current frame in seconds.
/// This value is scaled by time scale.
LD_API f64 time_delta(void);
/// Get time delta between last and current frame in seconds.
LD_API f64 time_unscaled_delta(void);
/// Get the amount of seconds that have passed since the start
/// of the program in seconds.
LD_API f64 time_elapsed(void);
/// Get the current time scale.
LD_API f64 time_query_scale(void);
/// Set the time scale.
LD_API void time_set_scale( f64 time_scale );
/// Get how many frames have been rendered since start of the program.
LD_API u64 time_frame_count(void);

/// Get the current year.
LD_API u32 time_query_year(void);
/// Get the current month.
/// Value returned is between 1-12.
LD_API u32 time_query_month(void);
/// Get the current day of the month.
/// Value returned is between 1-31.
LD_API u32 time_query_day(void);
/// Get the current hour.
/// Value returned is between 0-23.
LD_API u32 time_query_hour(void);
/// Get the current hour in 12hr format.
/// out_hour is between 1-12.
/// out_is_am tells you if it's AM or PM.
LD_API void time_query_hour_12hr( u32* out_hour, b32* out_is_am );
/// Get the current minute.
/// Value returned is between 0-59.
LD_API u32 time_query_minute(void);
/// Get the current seconds.
/// Value returned is between 0-59.
/// Do not use this value for profiling, use time_delta or time_elapsed instead.
LD_API u32 time_query_second(void);

#if defined(LD_API_INTERNAL)

/// Initialize time subsystem.
void time_subsystem_initialize(void);
/// Update time subsystem.
void time_subsystem_update(void);

#endif /* API internal */

#endif /* header guard */
