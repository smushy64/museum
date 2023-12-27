#if !defined(LP_LOGGING_H)
#define LP_LOGGING_H
/**
 * Description:  Liquid Package Logging
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 06, 2023
*/
#include "shared/defines.h"
#include "core/print.h"

typedef enum LogType {
    LP_LOG_TYPE_NORMAL,
    LP_LOG_TYPE_VERBOSE,
    LP_LOG_TYPE_ERROR,
    LP_LOG_TYPE_WARN,
} LogType;

b32 log_init( b32 is_silent, b32 is_verbose );
void ___log( LogType type, usize format_len, const char* format, ... );

#define lp_print( format, ... )\
    ___log( LP_LOG_TYPE_NORMAL, sizeof(format "\n"), format "\n", ##__VA_ARGS__ )
#define lp_note( format, ... )\
    ___log( LP_LOG_TYPE_VERBOSE, sizeof(format "\n"), format "\n", ##__VA_ARGS__ )
#define lp_warn( format, ... )\
    ___log( LP_LOG_TYPE_WARN, sizeof(format "\n"), format "\n", ##__VA_ARGS__ )
#define lp_error( format, ... )\
    ___log( LP_LOG_TYPE_ERROR, sizeof(format "\n"), format "\n", ##__VA_ARGS__ )

#endif /* header guard */

