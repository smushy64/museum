#if !defined(LD_CORE_TIME_H)
#define LD_CORE_TIME_H
/**
 * Description:  Time
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: October 23, 2023
*/
#include "defines.h"

// NOTE(alicia): High resolution time keeping.

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

// NOTE(alicia): Low resolution time keeping.

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

// NOTE(alicia): Conversions.

/// Convert whole nanoseconds to fractional milliseconds.
header_only f64 time_whole_ns_to_fract_ms( u64 ns ) {
    return (f64)ns / 1000000.0;
}
/// Convert whole nanoseconds to fractional seconds.
header_only f64 time_whole_ns_to_fract_seconds( u64 ns ) {
    f64 ms = time_whole_ns_to_fract_ms( ns );
    return ms / 1000.0;
}

/// Convert whole milliseconds to whole nanoseconds.
header_only u64 time_whole_ms_to_whole_ns( u64 ms ) {
    return ms * 1000000;
}
/// Convert fractional milliseconds to whole nanoseconds.
/// Be aware that remainder gets truncated.
header_only u64 time_fract_ms_to_whole_ns( f64 ms ) {
    return (u64)(ms * 1000000.0);
}
/// Convert whole milliseconds to fractional seconds.
header_only f64 time_whole_ms_to_fract_seconds( u64 ms ) {
    return (f64)ms / 1000.0;
}
/// Convert fractional milliseconds to fractional seconds.
header_only f64 time_fract_ms_to_fract_seconds( f64 ms ) {
    return ms / 1000.0;
}

/// Convert whole seconds to whole nanoseconds.
header_only u64 time_whole_seconds_to_whole_ns( u64 seconds ) {
    return seconds * 1000 * 1000000;
}
/// Convert fractional seconds to whole nanoseconds.
/// Be aware that remainder gets truncated and
/// this conversion is not particularly accurate.
header_only u64 time_fract_seconds_to_whole_ns( f64 seconds ) {
    return (u64)(seconds * 1000.0 * 1000000.0);
}
/// Convert whole seconds to whole milliseconds.
header_only u64 time_whole_seconds_to_whole_ms( u64 seconds ) {
    return seconds * 1000;
}
/// Convert fractional seconds to whole milliseconds.
/// Be aware that remainder gets truncated.
header_only u64 time_fract_seconds_to_whole_ms( f64 seconds ) {
    return (u64)(seconds * 1000.0);
}
/// Convert fractional seconds to fractional milliseconds.
header_only f64 time_fract_seconds_to_fract_ms( f64 seconds ) {
    return seconds * 1000.0;
}

typedef struct TimeInterval {
    u64 seconds;
    u64 milliseconds;
    u64 nanoseconds;
} TimeInterval;

/// Create a time interval.
header_only TimeInterval time_create_interval( u64 seconds, u64 ms, u64 ns ) {
    TimeInterval result;

    result.seconds      = seconds;
    result.milliseconds = ms;
    result.nanoseconds  = ns;

    return result;
}
/// Create a time interval from whole seconds.
header_only TimeInterval time_create_interval_whole_seconds( u64 seconds ) {
    TimeInterval result = {};
    result.seconds = seconds;
    return result;
}
/// Create a time interval from fractional seconds.
header_only TimeInterval time_create_interval_fract_seconds( f64 seconds ) {
    TimeInterval result = {};
    result.seconds = (u64)seconds;

    result.milliseconds = time_fract_seconds_to_whole_ms(
        seconds - (f64)result.seconds);

    result.nanoseconds = time_fract_seconds_to_whole_ns(
        (seconds - (f64)result.seconds) - (f64)result.milliseconds );

    return result;
}

#if defined(LD_API_INTERNAL)

/// Initialize time subsystem.
void time_subsystem_initialize(void);
/// Update time subsystem.
void time_subsystem_update(void);

#endif /* API internal */

#endif /* header guard */
