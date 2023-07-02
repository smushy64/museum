// * Description:  Time implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 02, 2023
#include "time.h"
#include "platform/platform.h"
#include "string.h"
using namespace impl;

LD_API AutoTimer::AutoTimer(
    const char* function,
    const char* file,
    int line
)
: function(function), file(file), line(line) {

    start_tick_count = platform_ticks();
    println(
        "Begin AutoTimer: {cc}() {cc}:{i}",
        function, file, line
    );
}
LD_API AutoTimer::~AutoTimer() {
    u64 end_tick_count = platform_ticks();
    f64 end_time =
        (f64)((end_tick_count - start_tick_count) * 1000.0) /
        (f64)(platform_ticks_per_second());

    println(
        "End   AutoTimer: {cc}() {cc}:{i} | TIME: {f,.3}ms",
        function, file, line, end_time
    );
}

