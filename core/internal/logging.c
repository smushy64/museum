/**
 * Description:  Core internal logging implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"
#include "core/lib.h"
#include "core/internal/logging.h"
#include "core/string.h"

#define CORE_LOGGING_BUFFER_SIZE (512)
global LoggingCallbackFN* global_logging_callback = NULL;
global void*              global_logging_params   = NULL;

void ___internal_core_log(
    LoggingLevel level, usize format_len, const char* format, ...
) {
    if( !global_logging_callback ) {
        return;
    }

    char logging_buffer[CORE_LOGGING_BUFFER_SIZE] = {};
    StringBuffer buffer = {};
    buffer.buffer   = logging_buffer;
    buffer.len      = 0;
    buffer.capacity = CORE_LOGGING_BUFFER_SIZE;

    va_list va;
    va_start( va, format );
    ___internal_string_buffer_fmt_va( &buffer, format_len, format, va );
    va_end( va );

    global_logging_callback( level, buffer.len, buffer.buffer, global_logging_params );
}

CORE_API void core_logging_callback_set( LoggingCallbackFN* callback, void* params ) {
    global_logging_callback = callback;
    global_logging_params   = params;
}
CORE_API void core_logging_callback_clear(void) {
    global_logging_callback = NULL;
    global_logging_params   = NULL;
}

