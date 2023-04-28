/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "logging.h"
// TODO(alicia): custom printf!
#include <stdio.h>
#include <pthread.h>

#if defined(SM_PLATFORM_WINDOWS)
    #include <windows.h>
    HANDLE CONSOLE_HANDLE;
#endif

static LogLevel GLOBAL_LOG_LEVEL = LOG_LEVEL_NONE;

#define MAX_LOG_LEVEL (\
    LOG_LEVEL_NONE    |\
    LOG_LEVEL_ERROR   |\
    LOG_LEVEL_WARN    |\
    LOG_LEVEL_DEBUG   |\
    LOG_LEVEL_INFO    |\
    LOG_LEVEL_TRACE   |\
    LOG_LEVEL_VERBOSE \
)

SM_API void logging_init( LogLevel level ) {
    #if defined(LD_LOGGING)
        SM_ASSERT( level <= MAX_LOG_LEVEL );
        GLOBAL_LOG_LEVEL = level;

        #if defined(SM_PLATFORM_WINDOWS)
            CONSOLE_HANDLE = GetStdHandle( STD_OUTPUT_HANDLE );
            DWORD dwMode = 0;
            GetConsoleMode(
                CONSOLE_HANDLE,
                &dwMode
            );
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(
                CONSOLE_HANDLE,
                dwMode
            );
        #endif
    #endif
}

static const char* LOG_COLOR_CODES[LOG_COLOR_COUNT] = {
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

void set_color( LogColor color ) {
    #if defined(LD_LOGGING)
        printf("%s", LOG_COLOR_CODES[color]);
    #endif
}

inline b32 is_level_valid( LogLevel level ) {
    #if defined(LD_LOGGING)
        return (level & GLOBAL_LOG_LEVEL) == level;
    #endif
    return false;
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

SM_API void log_formatted(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
) {
#if defined(LD_LOGGING)

    b32 always_print =
        (flags & LOG_FLAG_ALWAYS_PRINT) == LOG_FLAG_ALWAYS_PRINT;
    b32 new_line =
        (flags & LOG_FLAG_NEW_LINE) == LOG_FLAG_NEW_LINE;

    if( !(always_print || is_level_valid( level )) ) {
        return;
    }

    pthread_mutex_lock( &mutex );

    set_color( color );

    va_list args;
    va_start( args, format );
    vprintf( format, args );
    va_end( args );

    set_color( LOG_COLOR_RESET );
    
    if( new_line ) {
        printf( "\n" );
    }

    pthread_mutex_unlock( &mutex );

#endif
}
