/**
 * Description:  Core internal logging implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "shared/defines.h"
#include "core/lib.h"
#include "core/internal/logging.h"
#include "core/string.h"

#define CORE_LOGGING_BUFFER_SIZE (kilobytes(1))
global LoggingCallbackFN* global_logging_callback = NULL;
global void*              global_logging_params   = NULL;

b32 core_log_enabled(void) {
    return global_logging_callback != NULL;
}

void ___internal_core_log(
    LoggingLevel level, usize format_len, const char* format, ...
) {
    if( !global_logging_callback ) {
        return;
    }

    char logging_buffer[CORE_LOGGING_BUFFER_SIZE] = {};
    StringBuffer buffer = {};
    buffer.c   = logging_buffer;
    buffer.len = 0;
    buffer.cap = CORE_LOGGING_BUFFER_SIZE;

    va_list va;
    va_start( va, format );
    string_buffer_fmt_cstr_va( &buffer, format_len, format, va );
    va_end( va );

    global_logging_callback( level, buffer.len, buffer.str, global_logging_params );
}

CORE_API void core_logging_callback_set( LoggingCallbackFN* callback, void* params ) {
    global_logging_callback = callback;
    global_logging_params   = params;
}
CORE_API void core_logging_callback_clear(void) {
    global_logging_callback = NULL;
    global_logging_params   = NULL;
}

