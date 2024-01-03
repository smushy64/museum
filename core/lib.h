#if !defined(LD_CORE_LIB_H)
#define LD_CORE_LIB_H
/**
 * Description:  Core library management functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"

/// Set the logging callback function.
CORE_API void core_logging_callback_set( LoggingCallbackFN* callback, void* params );
/// Clear the logging callback function.
CORE_API void core_logging_callback_clear(void);

#endif /* header guard */
