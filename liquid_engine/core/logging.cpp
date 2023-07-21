/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "logging.h"
#include "string.h"
#include "memory.h"
#include "threading.h"

#if defined(LD_PLATFORM_WINDOWS)
    #include "platform/platform.h"
    global b32 OUTPUT_DEBUG_STRING_ENABLED = false;
    void log_enable_output_debug_string( b32 enable ) {
        OUTPUT_DEBUG_STRING_ENABLED = enable;
    }
#endif

global LogLevel GLOBAL_LOG_LEVEL = LOG_LEVEL_NONE;
global b32         MUTEX_CREATED;
global MutexHandle MUTEX = {};

#define MAX_LOG_LEVEL (\
    LOG_LEVEL_NONE    |\
    LOG_LEVEL_ERROR   |\
    LOG_LEVEL_WARN    |\
    LOG_LEVEL_DEBUG   |\
    LOG_LEVEL_INFO    |\
    LOG_LEVEL_TRACE   |\
    LOG_LEVEL_VERBOSE \
)

global u32   LOGGING_BUFFER_SIZE = 0ULL;
global char* LOGGING_BUFFER      = nullptr;

global const char* LOG_COLOR_CODES[LOG_COLOR_COUNT] = {
    "\033[1;30m",
    "\033[1;31m",
    "\033[1;32m",
    "\033[1;33m",
    "\033[1;35m",
    "\033[1;36m",
    "\033[1;37m",
    "\033[1;34m",
    "\033[1;0m" ,
    "\033[1;39m"
};

internal void set_color( LogColor color ) {
    #if defined(LD_LOGGING)
        print("{cc}", LOG_COLOR_CODES[color]);
    #endif
}

LD_API b32 is_log_initialized() {
    return LOGGING_BUFFER != nullptr;
}

b32 log_init( LogLevel level, StringView logging_buffer ) {
#if defined(LD_LOGGING)

    ASSERT(!is_log_initialized());

    ASSERT( level <= MAX_LOG_LEVEL );
    GLOBAL_LOG_LEVEL = level;

    ASSERT( logging_buffer.buffer );
    LOGGING_BUFFER_SIZE = logging_buffer.len;
    LOGGING_BUFFER      = logging_buffer.buffer;

    if( !MUTEX_CREATED ) {
        ASSERT( mutex_create( &MUTEX ) );
        MUTEX_CREATED = true;
    }

    LOG_INFO("Logging subsystem successfully initialized.");

#endif // if logging enabled

    return true;
}
void log_shutdown() {
#if defined(LD_LOGGING)
    set_color( LOG_COLOR_WHITE );

    println( "[INFO  ] Logging subsystem shutdown." );

    set_color( LOG_COLOR_RESET );

#if defined(LD_PLATFORM_WINDOWS)
    if( OUTPUT_DEBUG_STRING_ENABLED ) {
        platform_win32_output_debug_string(
            "[INFO ] Logging subsystem shutdown.\n"
        );
    }
#endif

    mutex_destroy( &MUTEX );

#endif
}

inline b32 is_level_valid( LogLevel level ) {
    #if defined(LD_LOGGING)
        return (level & GLOBAL_LOG_LEVEL) == level;
    #endif
    return false;
}

HOT_PATH
internal inline void log_formatted_internal(
    LogLevel level, LogColor color, LogFlags flags,
    b32 lock, const char* format,
    va_list args
) {

#if defined(LD_LOGGING)
    b32 is_error = ARE_BITS_SET( level, LOG_LEVEL_ERROR );
    if( !is_log_initialized() ) {
        ASSERT( mutex_create( &MUTEX ) );
        MUTEX_CREATED = true;
        if( lock ) {
            mutex_lock( &MUTEX );
        }
        set_color( color );
        if( is_error ) {
            printlnerr_va( format, args );
        } else {
            println_va( format, args );
        }
        set_color( LOG_COLOR_RESET );
        if( lock ) {
            mutex_unlock( &MUTEX );
        }
        return;
    }

    if( lock ) {
        mutex_lock( &MUTEX );
    }

    b32 always_print =
        (flags & LOG_FLAG_ALWAYS_PRINT) == LOG_FLAG_ALWAYS_PRINT;
    b32 new_line =
        (flags & LOG_FLAG_NEW_LINE) == LOG_FLAG_NEW_LINE;

    if( !(always_print || is_level_valid( level )) ) {
        if( lock ) {
            mutex_unlock( &MUTEX );
        }
        return;
    }

    StringView logging_buffer_view = {};
    logging_buffer_view.buffer = LOGGING_BUFFER;
    logging_buffer_view.len    = LOGGING_BUFFER_SIZE;

    u32 write_size = string_format_va(
        logging_buffer_view,
        format,
        args
    );

    if(
        (!write_size || write_size < 0) ||
        (((usize)write_size) >= (LOGGING_BUFFER_SIZE - 1))
    ) {
        if( lock ) {
            mutex_unlock( &MUTEX );
        }
        return;
    }

    if( new_line ) {
        LOGGING_BUFFER[write_size] = '\n';
        LOGGING_BUFFER[write_size + 1] = 0;
    } else {
        LOGGING_BUFFER[write_size] = 0;
    }

    set_color( color );

    if( is_error ) {
        output_string_stderr( LOGGING_BUFFER );
    } else {
        output_string_stdout( LOGGING_BUFFER );
    }

    set_color( LOG_COLOR_RESET );

#if defined(LD_PLATFORM_WINDOWS)
    if( OUTPUT_DEBUG_STRING_ENABLED ) {
        platform_win32_output_debug_string( LOGGING_BUFFER );
    }
#endif // if platform windows

    if( lock ) {
        mutex_unlock( &MUTEX );
    }

#endif // if logging enabled
}

LD_API void log_formatted_unlocked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
) {
    va_list args;
    va_start( args, format );
    log_formatted_internal(
        level, color, flags,
        false, format, args
    );
    va_end( args );
}

LD_API void log_formatted_locked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
) {
    va_list args;
    va_start( args, format );
    log_formatted_internal(
        level, color, flags,
        true, format, args
    );
    va_end( args );
}

LD_API LogLevel query_log_level() {
    return GLOBAL_LOG_LEVEL;
}
