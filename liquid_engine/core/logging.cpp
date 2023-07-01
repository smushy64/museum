/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "logging.h"
#include "string.h"
#include "memory.h"

// TODO(alicia): custom mutex!
#include <pthread.h>

#if defined(LD_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

global LogLevel GLOBAL_LOG_LEVEL = LOG_LEVEL_NONE;

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

        LD_ASSERT(!is_log_initialized());

        LD_ASSERT( level <= MAX_LOG_LEVEL );
        GLOBAL_LOG_LEVEL = level;

        LD_ASSERT( logging_buffer.buffer );
        LOGGING_BUFFER_SIZE = logging_buffer.len;
        LOGGING_BUFFER      = logging_buffer.buffer;

        #if defined(LD_PLATFORM_WINDOWS)
            HANDLE console_handle = GetStdHandle( STD_OUTPUT_HANDLE );
            if( console_handle == INVALID_HANDLE_VALUE ) {
                return false;
            }

            DWORD dwMode = 0;
            GetConsoleMode(
                console_handle,
                &dwMode
            );
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(
                console_handle,
                dwMode
            );
        #endif
    #endif

    LOG_INFO("Logging subsystem successfully initialized.");

    return true;
}
void log_shutdown() {
#if defined(LD_LOGGING)
    set_color( LOG_COLOR_WHITE );

    println( "[INFO  ] Logging subsystem shutdown.\n" );

    set_color( LOG_COLOR_RESET );

    #if defined(LD_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
        OutputDebugStringA( "[INFO  ] Logging subsystem shutdown.\n" );
    #endif

#endif
}

inline b32 is_level_valid( LogLevel level ) {
    #if defined(LD_LOGGING)
        return (level & GLOBAL_LOG_LEVEL) == level;
    #endif
    return false;
}

// this is for locking the logging function
// so that multiple threads can't print over each other
global pthread_mutex_t MUTEX_0 = PTHREAD_MUTEX_INITIALIZER;

void log_formatted_unlocked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
) {
    if( !is_log_initialized() ) {
        return;
    }
#if defined(LD_LOGGING)

    #if defined(LD_ASSERTIONS)
        if( !LOGGING_BUFFER ) {
            println(
                "LOG INIT WAS NOT CALLED BEFORE THIS LOG MESSAGE!\n"
            );
            #if defined(LD_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
                OutputDebugStringA(
                    "LOG INIT WAS NOT CALLED BEFORE THIS LOG MESSAGE!\n"
                );
            #endif
            LD_PANIC();
        }
    #endif

    b32 always_print =
        (flags & LOG_FLAG_ALWAYS_PRINT) == LOG_FLAG_ALWAYS_PRINT;
    b32 new_line =
        (flags & LOG_FLAG_NEW_LINE) == LOG_FLAG_NEW_LINE;

    if( !(always_print || is_level_valid( level )) ) {
        return;
    }


    va_list args;
    va_start( args, format );

    StringView logging_buffer_view = {};
    logging_buffer_view.buffer = LOGGING_BUFFER;
    logging_buffer_view.len    = LOGGING_BUFFER_SIZE;
    u32 write_size = string_format_va(
        logging_buffer_view,
        format,
        args
    );

    va_end( args );

    if(
        (!write_size || write_size < 0) ||
        (((usize)write_size) >= (LOGGING_BUFFER_SIZE - 1))
    ) {
        return;
    }

    if( new_line ) {
        LOGGING_BUFFER[write_size] = '\n';
        LOGGING_BUFFER[write_size + 1] = 0;
    }

    set_color( color );
    print( "{sv}", logging_buffer_view );
    set_color( LOG_COLOR_RESET );

    #if defined(LD_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
        OutputDebugStringA( LOGGING_BUFFER );
    #endif

#endif
}

void log_formatted_locked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
) {
    if( !is_log_initialized() ) {
        return;
    }
#if defined(LD_LOGGING)

    #if defined(LD_ASSERTIONS)
        if( !LOGGING_BUFFER ) {
            println(
                "LOG INIT WAS NOT CALLED BEFORE THIS LOG MESSAGE!\n"
            );
            #if defined(LD_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
                OutputDebugStringA(
                    "LOG INIT WAS NOT CALLED BEFORE THIS LOG MESSAGE!\n"
                );
            #endif
            LD_PANIC();
        }
    #endif

    pthread_mutex_lock( &MUTEX_0 );

    b32 always_print =
        (flags & LOG_FLAG_ALWAYS_PRINT) == LOG_FLAG_ALWAYS_PRINT;
    b32 new_line =
        (flags & LOG_FLAG_NEW_LINE) == LOG_FLAG_NEW_LINE;

    if( !(always_print || is_level_valid( level )) ) {
        pthread_mutex_unlock( &MUTEX_0 );
        return;
    }

#if 0 
    va_list args;
    va_start( args, format );

    StringView logging_buffer_view = {};
    logging_buffer_view.buffer = LOGGING_BUFFER;
    logging_buffer_view.len    = LOGGING_BUFFER_SIZE;

    u32 write_size = string_format_va(
        logging_buffer_view,
        format,
        args
    );

    va_end( args );

    if(
        (!write_size || write_size < 0) ||
        (((usize)write_size) >= (LOGGING_BUFFER_SIZE - 1))
    ) {
        pthread_mutex_unlock( &MUTEX_0 );
        return;
    }

    if( new_line ) {
        LOGGING_BUFFER[write_size] = '\n';
        LOGGING_BUFFER[write_size + 1] = 0;
    } else {
        LOGGING_BUFFER[write_size] = 0;
    }

    set_color( color );
    print( LOGGING_BUFFER );
    set_color( LOG_COLOR_RESET );

    #if defined(LD_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
        OutputDebugStringA( LOGGING_BUFFER );
    #endif

    pthread_mutex_unlock( &MUTEX_0 );
#endif
    
    va_list args;
    va_start( args, format );
    set_color( color );
    print_va( format, args );
    if( new_line ) {
        print( "\n" );
    }
    set_color( LOG_COLOR_RESET );

    va_end( args );
    pthread_mutex_unlock( &MUTEX_0 );


#endif
}

LogLevel query_log_level() {
    return GLOBAL_LOG_LEVEL;
}
