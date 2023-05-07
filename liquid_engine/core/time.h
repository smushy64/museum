#if !defined(TIME_HPP)
#define TIME_HPP
/**
 * Description:  Time related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 06, 2023
*/
#include "defines.h"

struct Time {
    f64 elapsed_time;
    f64 delta_time;
    u64 counter;
};

SM_API void time_update( Time* time );

#endif