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
    void log_subsystem_win32_enable_output_debug_string() {
#if defined(LD_LOGGING)
        OUTPUT_DEBUG_STRING_ENABLED = true;
#endif
    }
#endif

global LogLevel LOG_LEVEL   = LOG_LEVEL_NONE;
global Mutex*   MUTEX       = NULL;
global char*    BUFFER      = NULL;
global usize    BUFFER_SIZE = 0;

b32 log_subsystem_init(
    LogLevel level,
    usize log_buffer_size,
    void* log_buffer
) {
#if defined(LD_LOGGING)
    if( MUTEX ) {
        LOG_WARN( "Attempted to reinitialize logging subsystem!" );
        return true;
    }
    LOG_LEVEL   = level;
    BUFFER_SIZE = log_buffer_size;
    BUFFER      = log_buffer;
    MUTEX = mutex_create();
    if( !MUTEX ) {
        return false;
    }
    LOG_INFO( "Logging subsystem successfully initialized." );
#else
    if( MUTEX ) {
        return true;
    }
    MUTEX = mutex_create();
    if( !MUTEX ) {
        return false;
    }
    unused(level);
    unused(log_buffer_size);
    unused(log_buffer);
#endif
    return true;
}
void log_subsystem_shutdown() {
    mutex_destroy( MUTEX );
#if defined(LD_LOGGING)
    if( CHECK_BITS( LOG_LEVEL, LOG_LEVEL_INFO ) ) {
        println(
            LOG_COLOR_WHITE
            "[INFO] Logging subsystem shutdown."
            LOG_COLOR_RESET
        );
#if defined(LD_PLATFORM_WINDOWS)
        if( OUTPUT_DEBUG_STRING_ENABLED ) {
            platform_win32_output_debug_string(
                "[INFO] Logging subsystem shutdown."
            );
        }
#endif
    }
#endif
}

LD_API void log_set_log_level( LogLevel level ) {
    LOG_LEVEL = level;
}
LD_API LogLevel log_query_log_level() {
    return LOG_LEVEL;
}

HOT_PATH internal void log_formatted(
    LogLevel level,
    b32 always_print, b32 new_line,
    b32 locked_requested, const char* format,
    va_list variadic
) {
    b32 locked = locked_requested && MUTEX;

    b32 is_error = CHECK_BITS( level, LOG_LEVEL_ERROR );

#if defined(LD_LOGGING)
    if( !CHECK_BITS( LOG_LEVEL, level ) ) {
        if( !always_print ) {
            return;
        }
    }

    if( locked ) {
        mutex_lock( MUTEX );
    }
    read_write_fence();

    if( BUFFER ) {
        StringView buffer_view;
        buffer_view.buffer = BUFFER;
        buffer_view.len    = BUFFER_SIZE;

        usize chars_written = sv_format_va( buffer_view, format, variadic );
        if( !chars_written ) {
            read_write_fence();
            if( locked ) {
                mutex_unlock( MUTEX );
            }
            return;
        }

        if( new_line ) {
            if( chars_written + 1 >= BUFFER_SIZE ) {
                buffer_view.buffer[chars_written - 1] = '\n';
                buffer_view.buffer[chars_written]     = 0;
            } else {
                buffer_view.buffer[chars_written]     = '\n';
                buffer_view.buffer[chars_written + 1] = 0;
            }
        } else {
            if( chars_written >= BUFFER_SIZE ) {
                buffer_view.buffer[chars_written - 1] = 0;
            } else {
                buffer_view.buffer[chars_written] = 0;
            }
        }

        if( is_error ) {
            str_output_stderr( buffer_view.str );
        } else {
            str_output_stdout( buffer_view.str );
        }

#if defined(LD_PLATFORM_WINDOWS)
        if( OUTPUT_DEBUG_STRING_ENABLED ) {
            const char* str = buffer_view.str;
            platform_win32_output_debug_string( str );
        }
#endif
    } else {
        if( is_error ) {
            if( new_line ) {
                print_err_va( format, variadic );
            } else {
                println_err_va( format, variadic );
            }
        } else {
            if( new_line ) {
                println_va( format, variadic );
            } else {
                print_va( format, variadic );
            }
        }
    }

    read_write_fence();
    if( locked ) {
        mutex_unlock( MUTEX );
    }
#else
    if( !always_print ) {
        return;
    }
    if( locked ) {
        mutex_lock( MUTEX );
    }
    read_write_fence();

    if( is_error ) {
        print_err_va( format, variadic );
        if( new_line ) {
            char_output_stderr( '\n' );
        }
    } else {
        print_va( format, variadic );
        if( new_line ) {
            char_output_stdout( '\n' );
        }
    }

    read_write_fence();
    if( locked ) {
        mutex_unlock( MUTEX );
    }
#endif
}

LD_API void log_formatted_locked(
    LogLevel    level,
    b32         always_print,
    b32         new_line,
    const char* format,
    ...
) {
    va_list variadic;
    va_start( variadic, format );
    log_formatted( level, always_print, new_line, true, format, variadic );
    va_end( variadic );
}
LD_API void log_formatted_unlocked(
    LogLevel    level,
    b32         always_print,
    b32         new_line,
    const char* format,
    ...
) {
    va_list variadic;
    va_start( variadic, format );
    log_formatted( level, always_print, new_line, false, format, variadic );
    va_end( variadic );
}

