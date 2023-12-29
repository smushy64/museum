#if !defined(LD_MEDIA_MEDIA_H)
#define LD_MEDIA_MEDIA_H
/**
 * Description:  Media Library Configuration.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"

/// Media Library Logging Levels.
typedef u32 MediaLoggingLevel;
#define MEDIA_LOGGING_LEVEL_NONE  (0)
#define MEDIA_LOGGING_LEVEL_INFO  (1 << 0)
#define MEDIA_LOGGING_LEVEL_WARN  (1 << 1)
#define MEDIA_LOGGING_LEVEL_ERROR (1 << 2)

/// Media Library logging callback.
typedef void MediaLoggingCallbackFN(
    usize message_len, const char* message, MediaLoggingLevel level, void* params );

/// Initialize media library.
/// Call this before other library functions.
/// Logging functions can be called before initialization.
MEDIA_API b32 media_initialize(void);

/// Set logging callback.
MEDIA_API void media_set_logging_callback(
    MediaLoggingCallbackFN* callback, void* params );
/// Clear logging callback.
MEDIA_API void media_clear_logging_callback(void);

/// Set media library's logging level.
MEDIA_API void media_set_logging_level( MediaLoggingLevel level );
/// Query media library's logging level.
MEDIA_API MediaLoggingLevel media_query_logging_level(void);

#endif /* header guard */
