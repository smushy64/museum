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

    start_ms = platform_ms_elapsed();
    println(
        "Begin AutoTimer: {cc}() {cc}:{i}",
        function, file, line
    );
}
LD_API AutoTimer::~AutoTimer() {
    f64 end_ms = platform_ms_elapsed();

    println(
        "End   AutoTimer: {cc}() {cc}:{i} | TIME: {f,.3}ms",
        function, file, line, end_ms
    );
}

