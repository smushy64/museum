/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#include "core/logging.h"
#include "core/time.h"
#include "core/internal.h"
#include "core/thread.h"
#include "core/strings.h"

#define LOGGING_TIMESTAMP_BUFFER_SIZE (32)

#define LOGGING_BUFFER_SIZE (kilobytes(1))
global char LOGGING_BUFFER[LOGGING_BUFFER_SIZE] = {};

global LoggingLevel LOGGING_LEVEL = LOGGING_LEVEL_NONE;

global Mutex* LOGGING_MUTEX = NULL;
internal force_inline
void ___log_lock(void) {
    if( LOGGING_MUTEX ) {
        mutex_lock( LOGGING_MUTEX );
    }
}
internal force_inline
void ___log_unlock(void) {
    if( LOGGING_MUTEX ) {
        mutex_unlock( LOGGING_MUTEX );
    }
}

global PlatformFile* LOGGING_FILE = NULL;
internal force_inline
void ___log_output_file( StringSlice* message ) {
    if( LOGGING_FILE ) {
        platform->io.file_write( LOGGING_FILE, message->len, message->buffer );
    }
}


void logging_subsystem_initialize( PlatformFile* output_file ) {
    LOGGING_FILE  = output_file;
    if( LOGGING_FILE ) {
        usize file_size = platform->io.file_query_size( LOGGING_FILE );
        platform->io.file_set_offset( LOGGING_FILE, file_size - 1 );

        ss_mut( logging_file_header, "\n\n[PROGRAM START] --------\n\n" );
        ___log_output_file( &logging_file_header );
    }

    LOGGING_MUTEX = mutex_create();
    assert( LOGGING_MUTEX );
}
void logging_subsystem_detach_file(void) {
    ___log_lock();
    read_write_fence();

    LOGGING_FILE = NULL;

    read_write_fence();
    ___log_unlock();
}

LD_API void logging_set_level( LoggingLevel level ) {
    LOGGING_LEVEL = level;
}
LD_API LoggingLevel logging_query_level(void) {
    return LOGGING_LEVEL;
}

#if defined(LD_PLATFORM_WINDOWS)

    global b32 LOGGING_OUTPUT_DEBUG_STRING_ENABLED = false;

    void logging_set_output_debug_string_enabled( b32 is_enabled ) {
        LOGGING_OUTPUT_DEBUG_STRING_ENABLED = is_enabled;
    }
    b32 logging_query_output_debug_string_enabled(void) {
        return LOGGING_OUTPUT_DEBUG_STRING_ENABLED;
    }

    void ___log_output_debug_string_fn( StringSlice* message ) {
        if( !LOGGING_OUTPUT_DEBUG_STRING_ENABLED ) {
            return;
        }
        platform->io.output_debug_string( message->buffer );
    }

    #define ___log_output_debug_string( message )\
        ___log_output_debug_string_fn( message )

#else
    #define ___log_output_debug_string( ... )
#endif


internal force_inline
b32 ___is_log_allowed( LoggingType type, b32 trace ) {
    if( type == LOGGING_TYPE_FATAL ) {
        return true;
    }

    if( trace && !bitfield_check( LOGGING_LEVEL, LOGGING_LEVEL_TRACE ) ) {
        return false;
    }

    switch( type ) {
        case LOGGING_TYPE_ERROR:
            return bitfield_check( LOGGING_LEVEL, LOGGING_LEVEL_ERROR );
        case LOGGING_TYPE_WARN:
            return bitfield_check( LOGGING_LEVEL, LOGGING_LEVEL_WARN );
        case LOGGING_TYPE_DEBUG:
            return bitfield_check( LOGGING_LEVEL, LOGGING_LEVEL_DEBUG );
        case LOGGING_TYPE_INFO:
            return bitfield_check( LOGGING_LEVEL, LOGGING_LEVEL_INFO );
        case LOGGING_TYPE_NOTE:
            return bitfield_check( LOGGING_LEVEL, LOGGING_LEVEL_NOTE );
        default:
            return false;
    }
}

internal force_inline
StringSlice ___logging_color( LoggingType type ) {
    StringSlice result;
    result.len      = sizeof( CONSOLE_COLOR_BLACK );
    result.capacity = result.len;

    ConsoleColor* colors[] = {
        CONSOLE_COLOR_MAGENTA,
        CONSOLE_COLOR_RED,
        CONSOLE_COLOR_YELLOW,
        CONSOLE_COLOR_BLUE,
        CONSOLE_COLOR_WHITE,
        CONSOLE_COLOR_RESET
    };

    result.buffer = (char*)(colors[type]);

    return result;
}

internal force_inline
void ___log_output_console( LoggingType type, struct StringSlice* message ) {
    switch( type ) {
        case LOGGING_TYPE_FATAL:
        case LOGGING_TYPE_ERROR: {
            ss_output_stderr( message );
        } break;
        default: {
            ss_output_stdout( message );
        } break;
    }
}

internal force_inline
void ___log_console_color_reset( LoggingType type ) {
    StringSlice console_color;
    console_color.buffer = CONSOLE_COLOR_RESET;
    console_color.len    = sizeof( CONSOLE_COLOR_RESET );

    ___log_output_console( type, &console_color );
}

internal force_inline
void ___log_generate_timestamp( StringSlice* slice ) {

    u32 month = time_query_month();
    u32 day   = time_query_day();
    u32 year  = time_query_year();

    b32 is_am;
    u32 hour;
    time_query_hour_12hr( &hour, &is_am );

    u32 minute = time_query_minute();
    u32 second = time_query_second();

    ss_mut_fmt(
        slice, "[{u,02}/{u,02}/{u,04} {u,02}:{u,02}:{u,02} {cc}] ",
        month, day, year,
        hour, minute, second,
        is_am ? "AM" : "PM" );
}

LD_API void logging_output(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    struct StringSlice* message
) {
    if( !always_log ) {
        if( !___is_log_allowed( type, trace ) ) {
            return;
        }
    }

    StringSlice console_color = {};
    if( opt_color_override ) {
        console_color = ss_from_cstr( 0, opt_color_override );
    } else {
        console_color = ___logging_color( type );
    }

    ___log_output_console( type, &console_color );

    if( timestamped ) {
        char timestamp_buffer[LOGGING_TIMESTAMP_BUFFER_SIZE] = {};
        StringSlice timestamp;
        timestamp.buffer   = timestamp_buffer;
        timestamp.len      = 0;
        timestamp.capacity = LOGGING_TIMESTAMP_BUFFER_SIZE;

        ___log_generate_timestamp( &timestamp );

        ___log_output_file( &timestamp );
        ___log_output_debug_string( &timestamp );
    }

    ___log_output_console( type, message );
    ___log_output_file( message );
    ___log_output_debug_string( message );

    if( new_line ) {
        StringSlice nl;
        nl.buffer   = "\n";
        nl.len      = 1;
        nl.capacity = 2;

        ___log_output_console( type, &nl );
        ___log_output_file( &nl );
        ___log_output_debug_string( &nl );
    }

    ___log_console_color_reset( type );
}
LD_API void logging_output_locked(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    struct StringSlice* message
) {
    ___log_lock();
    read_write_fence();

    logging_output(
        type, opt_color_override, trace,
        always_log, new_line, timestamped, message );

    read_write_fence();
    ___log_unlock();
}

LD_API void logging_output_fmt_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, va_list va
) {
    if( !always_log ) {
        if( !___is_log_allowed( type, trace ) ) {
            return;
        }
    }

    StringSlice format_buffer;
    format_buffer.buffer   = LOGGING_BUFFER;
    format_buffer.len      = 0;
    format_buffer.capacity = LOGGING_BUFFER_SIZE;

    ss_mut_fmt_va( &format_buffer, format, va );
    if( !ss_mut_push( &format_buffer, 0 ) ) {
        format_buffer.buffer[format_buffer.len - 1] = 0;
    }

    logging_output(
        type, opt_color_override, trace,
        always_log, new_line, timestamped, &format_buffer );
}
LD_API void logging_output_fmt_locked_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, va_list va
) {
    ___log_lock();
    read_write_fence();

    logging_output_fmt_va(
        type, opt_color_override, trace,
        always_log, new_line, timestamped, format, va );

    read_write_fence();
    ___log_unlock();
}

LD_API void logging_output_fmt(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, ...
) {
    va_list va;
    va_start( va, format );

    logging_output_fmt_va(
        type, opt_color_override, trace,
        always_log, new_line, timestamped, format, va );

    va_end( va );
}
LD_API void logging_output_fmt_locked(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, ...
) {
    va_list va;
    va_start( va, format );

    logging_output_fmt_locked_va(
        type, opt_color_override, trace,
        always_log, new_line, timestamped, format, va );

    va_end( va );
}


