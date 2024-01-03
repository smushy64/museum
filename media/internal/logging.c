/**
 * Description:  Media logging implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"
#include "media/lib.h"
#include "media/internal/logging.h"

#include "core/string.h"

#define MEDIA_LOGGING_BUFFER_SIZE (512)
global LoggingCallbackFN* global_logging_callback = NULL;
global void*              global_logging_params   = NULL;

void ___internal_media_log(
    LoggingLevel level, usize format_len, const char* format, ...
) {
    if( !global_logging_callback ) {
        return;
    }

    char buffer[MEDIA_LOGGING_BUFFER_SIZE] = {};
    StringSlice slice = {};
    slice.buffer   = buffer;
    slice.len      = 0;
    slice.capacity = MEDIA_LOGGING_BUFFER_SIZE;

    va_list va;
    va_start( va, format );
    ___internal_string_slice_fmt_va( &slice, format_len, format, va );
    va_end( va );

    global_logging_callback( level, slice.len, slice.buffer, global_logging_params );
}

MEDIA_API void media_logging_callback_set( LoggingCallbackFN* callback, void* params ) {
    global_logging_callback = callback;
    global_logging_params   = params;
}
MEDIA_API void media_logging_callback_clear(void) {
    global_logging_callback = NULL;
    global_logging_params   = NULL;
}


// global MediaLoggingLevel global_logging_level = MEDIA_LOGGING_LEVEL_NONE;
// global MediaLoggingCallbackFN* global_logging_callback = NULL;
// global void* global_logging_params = NULL;
//
// #define MEDIA_LOGGING_CAPACITY (512)
// global char global_logging_buffer[MEDIA_LOGGING_CAPACITY] = {};
// global Mutex global_logging_mutex = {};
//
// void ___internal_media_log(
//     MediaLoggingLevel level, usize format_len, const char* format, ...
// ) {
//     if( !global_logging_callback ) {
//         return;
//     }
//     if( !bitfield_check( level, global_logging_level ) ) {
//         return;
//     }
//
//     StringSlice buffer = {};
//     buffer.buffer   = global_logging_buffer;
//     buffer.capacity = MEDIA_LOGGING_CAPACITY;
//
//     mutex_lock( &global_logging_mutex );
//     read_write_fence();
//
//     va_list va;
//     va_start( va, format );
//
//     ___internal_string_slice_fmt_va( &buffer, format_len, format, va );
//
//     va_end( va );
//
//     global_logging_callback( buffer.len, buffer.buffer, level, global_logging_params );
//
//     read_write_fence();
//     mutex_unlock( &global_logging_mutex );
// }
//
// MEDIA_API void media_set_logging_callback(
//     MediaLoggingCallbackFN* callback, void* params
// ) {
//     global_logging_callback = callback;
//     global_logging_params   = params;
// }
// MEDIA_API void media_clear_logging_callback(void) {
//     global_logging_callback = NULL;
//     global_logging_params   = NULL;
// }
//
// MEDIA_API void media_set_logging_level( MediaLoggingLevel level ) {
//     if( !global_logging_mutex.handle ) {
//         assert( mutex_create( &global_logging_mutex ) );
//     }
//     global_logging_level = level;
// }
// MEDIA_API MediaLoggingLevel media_query_logging_level(void) {
//     return global_logging_level;
// }
//
