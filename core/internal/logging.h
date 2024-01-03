#if !defined(LD_CORE_INTERNAL_LOGGING_H)
#define LD_CORE_INTERNAL_LOGGING_H
/**
 * Description:  Core internal logging functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"
#include "core/lib.h" // IWYU pragma: keep

void ___internal_core_log(
     LoggingLevel level, usize format_len, const char* format, ... );

#define core_log_note( format, ... )\
    ___internal_core_log(\
        LOGGING_LEVEL_NOTE,\
        text( "[CORE] " format ), ##__VA_ARGS__ )
#define core_log_info( format, ... )\
    ___internal_core_log(\
        LOGGING_LEVEL_INFO,\
        text( "[CORE] " format ), ##__VA_ARGS__ )
#define core_log_warn( format, ... )\
    ___internal_core_log(\
        LOGGING_LEVEL_WARN,\
        text( "[CORE] " format ), ##__VA_ARGS__ )
#define core_log_error( format, ... )\
    ___internal_core_log(\
        LOGGING_LEVEL_ERROR,\
        text( "[CORE] " format ), ##__VA_ARGS__ )
#define core_log_fatal( format, ... )\
    ___internal_core_log(\
        LOGGING_LEVEL_FATAL,\
        text( "[CORE] " format ), ##__VA_ARGS__ )   

#endif /* header guard */
