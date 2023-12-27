#if !defined(LD_CORE_TIME_H)
#define LD_CORE_TIME_H
/**
 * Description:  Time
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 05, 2023
*/
#include "shared/defines.h"

/// Time record, used for low-resolution time keeping.
typedef struct TimeRecord {
    /// Year.
    u32 year;
    /// Month, 1-12.
    u32 month;
    /// Day, 1-31.
    u32 day;
    /// Hour, 0-23.
    u32 hour;
    /// Minute, 0-59.
    u32 minute;
    /// Second, 0-59.
    u32 second;
} TimeRecord;

/// Initialize time keeping.
CORE_API void time_initialize(void);
/// Update time keeping.
/// Should be called every frame.
CORE_API void time_update(void);

/// Get time delta between last update and current update
/// in seconds. Value returned is scaled by time scale.
CORE_API f64 time_delta_seconds(void);
/// Get time delta between last update and
/// current update in seconds.
CORE_API f64 time_unscaled_delta_seconds(void);
/// Get seconds elapsed since time was initialized.
CORE_API f64 time_elapsed_seconds(void);
/// Query how many times time keeping has been updated.
/// In engine, this corresponds to number of frames rendered.
CORE_API u64 time_query_update_count(void);

/// Query time scale.
CORE_API f64 time_query_scale(void);
/// Set time scale.
CORE_API void time_set_scale( f64 new_scale );

/// Get a record of the current time.
CORE_API TimeRecord time_record(void);

// NOTE(alicia): Conversions.

/// Convert month to a null-terminated C string.
header_only const char* time_month_to_cstr( u32 month ) {
    const char* strings[13] = {
        "Invalid Month",
        "January", "February", "March",
        "April", "May", "June",
        "July",  "August", "September",
        "October", "November", "December",
    };
    return strings[month];
}
/// Convert 24-hour time to 12-hour.
CORE_API void time_hour_24_to_hour_12( u32 hour_24, u32* out_hour_12, b32* out_is_am );
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

#endif /* header guard */
