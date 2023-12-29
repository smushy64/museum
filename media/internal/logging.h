#if !defined(LD_MEDIA_INTERNAL_LOGGING_H)
#define LD_MEDIA_INTERNAL_LOGGING_H
/**
 * Description:  Media logging.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"
#include "media/media.h"

void ___internal_media_log(
    MediaLoggingLevel level, usize format_len, const char* format, ... );

#define media_log_info( format, ... )\
    ___internal_media_log(\
        MEDIA_LOGGING_LEVEL_INFO, sizeof( "[MEDIA] " format ),\
        "[MEDIA] " format, ##__VA_ARGS__ )
#define media_log_warn( format, ... )\
    ___internal_media_log(\
        MEDIA_LOGGING_LEVEL_WARN, sizeof( "[MEDIA] " format ),\
        "[MEDIA] " format, ##__VA_ARGS__ )
#define media_log_error( format, ... )\
    ___internal_media_log(\
        MEDIA_LOGGING_LEVEL_ERROR, sizeof( "[MEDIA] " format ),\
        "[MEDIA] " format, ##__VA_ARGS__ )

#endif /* header guard */
