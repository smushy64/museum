/**
 * Description:  Core internal logging implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 03, 2023
*/
#include "defines.h"
#include "core/misc.h"
#include "core/internal/logging.h"
#include "core/string.h"

#define CORE_LOGGING_BUFFER_SIZE (512)
global CoreLoggingCallbackFN* global_logging_callback = NULL;

void ___internal_core_log(
    CoreLoggingType type, usize format_len, const char* format, ...
) {
    if( !global_logging_callback ) {
        return;
    }

    char buffer[CORE_LOGGING_BUFFER_SIZE] = {};
    StringSlice slice = {};
    slice.buffer   = buffer;
    slice.len      = 0;
    slice.capacity = CORE_LOGGING_BUFFER_SIZE;

    va_list va;
    va_start( va, format );
    ___internal_string_slice_fmt_va( &slice, format_len, format, va );
    va_end( va );

    global_logging_callback( slice.len, slice.buffer, type );

}

CORE_API void core_logging_callback_set( CoreLoggingCallbackFN* callback ) {
    global_logging_callback = callback;
}
CORE_API void core_logging_callback_clear(void) {
    global_logging_callback = NULL;
}

