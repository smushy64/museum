#if !defined(LD_CORE_INTERNAL_LOGGING_H)
#define LD_CORE_INTERNAL_LOGGING_H
/**
 * Description:  Core internal logging functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"
#include "core/misc.h"

void ___internal_core_log(
    CoreLoggingType type, usize format_len, const char* format, ... );

#define core_log_note( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_NOTE,\
        sizeof( "[CORE] " format), "[CORE] " format, ##__VA_ARGS__ )
#define core_log_info( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_INFO,\
        sizeof( "[CORE] " format), "[CORE] " format, ##__VA_ARGS__ )
#define core_log_warn( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_WARN,\
        sizeof( "[CORE] " format), "[CORE] " format, ##__VA_ARGS__ )
#define core_log_error( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_ERROR,\
        sizeof( "[CORE] " format), "[CORE] " format, ##__VA_ARGS__ )
#define core_log_fatal( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_FATAL,\
        sizeof( "[CORE] " format), "[CORE] " format, ##__VA_ARGS__ )   

#endif /* header guard */
