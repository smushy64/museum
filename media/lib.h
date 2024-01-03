#if !defined(LD_MEDIA_MEDIA_H)
#define LD_MEDIA_MEDIA_H
/**
 * Description:  Media Library Configuration.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"

/// Initialize media library.
/// Call this before other library functions.
/// Logging functions can be called before initialization.
MEDIA_API b32 media_initialize(void);

/// Set logging callback.
MEDIA_API void media_logging_callback_set(
    LoggingCallbackFN* callback, void* params );
/// Clear logging callback.
MEDIA_API void media_logging_callback_clear(void);

#endif /* header guard */
