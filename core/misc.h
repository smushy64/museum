#if !defined(LD_CORE_MISC_H)
#define LD_CORE_MISC_H
/**
 * Description:  Miscellaneous functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"

/// Types of logging messages.
typedef enum CoreLoggingType : u32 {
    CORE_LOGGING_TYPE_NOTE,
    CORE_LOGGING_TYPE_INFO,
    CORE_LOGGING_TYPE_WARN,
    CORE_LOGGING_TYPE_ERROR,
    CORE_LOGGING_TYPE_FATAL,

    CORE_LOGGING_TYPE_MEMORY_SUCCESS,
    CORE_LOGGING_TYPE_MEMORY_ERROR,
} CoreLoggingType;

/// Logging callback function prototype.
typedef void CoreLoggingCallbackFN(
    usize message_len, const char* message, CoreLoggingType type );

/// Set the logging callback function.
CORE_API void core_logging_callback_set( CoreLoggingCallbackFN* callback );
/// Clear the logging callback function.
CORE_API void core_logging_callback_clear(void);

/// Query how many processors the system has.
CORE_API usize core_query_processor_count(void);

#endif /* header guard */
