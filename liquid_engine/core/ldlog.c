/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "core/ldlog.h"
#include "core/ldstring.h"
#include "core/ldmemory.h"
#include "core/ldthread.h"

#if defined(LD_PLATFORM_WINDOWS)
    #include "ldplatform.h"
    global b32 OUTPUT_DEBUG_STRING_ENABLED = false;
    void log_enable_output_debug_string( b32 enable ) {
        OUTPUT_DEBUG_STRING_ENABLED = enable;
    }
#endif

global LogLevel GLOBAL_LOG_LEVEL = LOG_LEVEL_NONE;
global b32      MUTEX_CREATED;
global Mutex*   MUTEX = NULL;

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
global char* LOGGING_BUFFER      = NULL;

global const char* LOG_COLOR_CODES[LOG_COLOR_COUNT] = {
    "\033[1;30m", // BLACK
    "\033[1;31m", // RED
    "\033[1;32m", // GREEN
    "\033[1;33m", // YELLOW
    "\033[1;35m", // MAGENTA
    "\033[1;36m", // CYAN
    "\033[1;37m", // WHITE
    "\033[1;34m", // BLUE
    "\033[1;0m" , // RESET
    "\033[1;39m"  // DEFAULT
};

internal void set_color( LogColor color ) {
    print("{cc}", LOG_COLOR_CODES[color]);
}

LD_API b32 is_log_initialized() {
    return LOGGING_BUFFER != NULL;
}

b32 log_init(
    LogLevel level,
    StringView logging_buffer
) {
#if defined(LD_LOGGING)

    ASSERT(!is_log_initialized());

    ASSERT( level <= MAX_LOG_LEVEL );
    GLOBAL_LOG_LEVEL = level;

    ASSERT( logging_buffer.buffer );
    LOGGING_BUFFER_SIZE = logging_buffer.len;
    LOGGING_BUFFER      = (char*)logging_buffer.buffer;

    if( !MUTEX_CREATED ) {
        MUTEX = mutex_create();
        ASSERT(MUTEX);
        MUTEX_CREATED = true;
    }

    LOG_INFO("Logging subsystem successfully initialized.");

#endif // if logging enabled
    unused(level);
    unused(logging_buffer);
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

    mutex_destroy( MUTEX );
#endif
}

internal b32 is_level_valid( LogLevel level ) {
    #if defined(LD_LOGGING)
        return (level & GLOBAL_LOG_LEVEL) == level;
    #endif
    unused(level);
    return false;
}

HOT_PATH internal void log_formatted_internal(
    LogLevel level, LogColor color, LogFlags flags,
    b32 lock, const char* format,
    va_list args
) {
    b32 always_print = CHECK_BITS( flags, LOG_FLAG_ALWAYS_PRINT );

#if defined(LD_LOGGING)
    b32 is_error = CHECK_BITS( level, LOG_LEVEL_ERROR );
    if( !is_log_initialized() ) {
        MUTEX = mutex_create();
        ASSERT( MUTEX );
        MUTEX_CREATED = true;
        if( lock ) {
            mutex_lock( MUTEX );
        }
        set_color( color );
        if( is_error ) {
            println_err_va( format, args );
        } else {
            println_va( format, args );
        }
        set_color( LOG_COLOR_RESET );
        if( lock ) {
            mutex_unlock( MUTEX );
        }
        return;
    }

    if( lock ) {
        mutex_lock( MUTEX );
    }

    read_write_fence();

    b32 new_line = CHECK_BITS( flags, LOG_FLAG_NEW_LINE );

    if( !(always_print || is_level_valid( level )) ) {
        if( lock ) {
            mutex_unlock( MUTEX );
        }
        return;
    }

    StringView logging_buffer_view = {};
    logging_buffer_view.buffer = LOGGING_BUFFER;
    logging_buffer_view.len    = LOGGING_BUFFER_SIZE;

    u32 write_size = sv_format_va(
        logging_buffer_view,
        format,
        args
    );

    if(
        (!write_size || write_size < 0) ||
        (((usize)write_size) >= (LOGGING_BUFFER_SIZE - 1))
    ) {
        if( lock ) {
            mutex_unlock( MUTEX );
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
        str_output_stderr( LOGGING_BUFFER );
    } else {
        str_output_stdout( LOGGING_BUFFER );
    }

    set_color( LOG_COLOR_RESET );

#if defined(LD_PLATFORM_WINDOWS)
    if( OUTPUT_DEBUG_STRING_ENABLED ) {
        platform_win32_output_debug_string( LOGGING_BUFFER );
    }
#endif // if platform windows

    read_write_fence();
    if( lock ) {
        mutex_unlock( MUTEX );
    }

#else // if logging enabled
    b32 is_error = CHECK_BITS( level, LOG_LEVEL_ERROR );
    if( always_print ) {
        if( lock ) {
            mutex_lock( MUTEX );
        }

        read_write_fence();

        set_color( color );

        if( is_error ) {
            printerr_va( format, args );
        } else {
            print_va( format, args );
        }

        set_color( LOG_COLOR_RESET );

        read_write_fence();

        if( lock ) {
            mutex_unlock( MUTEX );
        }
    }
#endif // if logging disabled
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
