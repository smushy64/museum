#if !defined(CORE_TIME_HPP)
#define CORE_TIME_HPP
/**
 * Description:  Time
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 15, 2023
*/
#include "defines.h"

/// Time
struct Timer {
    f32 delta_seconds;
    f32 elapsed_seconds;
    u64 frame_count;
};

namespace impl {

/// Timer structure.
struct LD_API_STRUCT AutoTimer {
private:
    f64 start_ms;
    const char* function;
    const char* file;
    int line;
public:
    AutoTimer(
        const char* function,
        const char* file,
        int line
    );
    ~AutoTimer();
};

} // namespace impl

/// macro bullshit to get unique identifier for each auto timer

#define ______EXPANSION_2( a, b )\
    ::impl::AutoTimer a##_ ##b(\
    __FUNCTION__, __FILE__, __LINE__ )

#define ______EXPANSION_1( a, b ) ______EXPANSION_2( a, b )

/// Create a timer for the current scope.
/// Prints to the console how long it took for the
/// current scope to execute ( in milliseconds ).
#define AUTO_TIMER\
    ______EXPANSION_1(_AUTO_TIMER_, __LINE__)

#endif // header guard
