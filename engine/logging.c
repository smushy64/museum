/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "shared/defines.h"
#include "core/sync.h"
#include "core/print.h"
#include "core/string.h"
#include "core/fs.h"
#include "core/time.h"

#include "engine/logging.h"

#define LOGGING_TIMESTAMP_BUFFER_SIZE (32)

#define LOGGING_BUFFER_SIZE (kilobytes(1))
global char LOGGING_BUFFER[LOGGING_BUFFER_SIZE] = {0};

global LoggingLevel LOGGING_LEVEL = LOGGING_LEVEL_NONE;

global Mutex LOGGING_MUTEX = {};
internal force_inline
void ___log_lock(void) {
    if( LOGGING_MUTEX.handle ) {
        mutex_lock( &LOGGING_MUTEX );
    }
}
internal force_inline
void ___log_unlock(void) {
    if( LOGGING_MUTEX.handle ) {
        mutex_unlock( &LOGGING_MUTEX );
    }
}

global FSFile* LOGGING_FILE = NULL;
internal force_inline
void ___log_output_file( StringSlice* message ) {
    if( LOGGING_FILE ) {
        fs_file_write( LOGGING_FILE, message->len, message->buffer );
    }
}


void logging_subsystem_initialize( FSFile* output_file ) {
    LOGGING_FILE  = output_file;
    if( LOGGING_FILE ) {
        usize file_size = fs_file_query_size( LOGGING_FILE );
        fs_file_set_offset( LOGGING_FILE, file_size - 1 );

        string_slice_mut( logging_file_header, "\n\n[PROGRAM START] --------\n\n" );
        ___log_output_file( &logging_file_header );
    }

    assert( mutex_create( &LOGGING_MUTEX ) );
}
void logging_subsystem_attach_file( FSFile* file ) {
    ___log_lock();
    read_write_fence();

    LOGGING_FILE = file;

    read_write_fence();
    ___log_unlock();
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
        output_debug_string( message->buffer );
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
            string_slice_output_stderr( message );
        } break;
        default: {
            string_slice_output_stdout( message );
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

    TimeRecord record = time_record();

    b32 is_am;
    u32 hour;
    time_hour_24_to_hour_12( record.hour, &hour, &is_am );

    string_slice_fmt(
        slice, "[{u,02}/{u,02}/{u,04} {u,02}:{u,02}:{u,02} {cc}] ",
        record.month, record.day, record.year,
        hour, record.minute, record.second,
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

    StringSlice console_color = {0};
    if( opt_color_override ) {
        console_color = string_slice_from_cstr( 0, opt_color_override );
    } else {
        console_color = ___logging_color( type );
    }

    ___log_output_console( type, &console_color );

    if( timestamped ) {
        char timestamp_buffer[LOGGING_TIMESTAMP_BUFFER_SIZE] = {0};
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

LD_API void ___internal_logging_output_fmt_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, va_list va
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

    ___internal_string_slice_fmt_va( &format_buffer, format_len, format, va );
    if( !string_slice_push( &format_buffer, 0 ) ) {
        format_buffer.buffer[format_buffer.len - 1] = 0;
    }

    logging_output(
        type, opt_color_override, trace,
        always_log, new_line, timestamped, &format_buffer );
}
LD_API void ___internal_logging_output_fmt_locked_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, va_list va
) {
    ___log_lock();
    read_write_fence();

    ___internal_logging_output_fmt_va(
        type, opt_color_override, trace,
        always_log, new_line, timestamped,
        format_len, format, va );

    read_write_fence();
    ___log_unlock();
}

LD_API void ___internal_logging_output_fmt(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );

    ___internal_logging_output_fmt_va(
        type, opt_color_override, trace,
        always_log, new_line, timestamped,
        format_len, format, va );

    va_end( va );
}
LD_API void ___internal_logging_output_fmt_locked(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, ...
) {
    va_list va;
    va_start( va, format );

    ___internal_logging_output_fmt_locked_va(
        type, opt_color_override, trace,
        always_log, new_line, timestamped,
        format_len, format, va );

    va_end( va );
}


