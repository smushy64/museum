/**
 * Description:  Logging implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "logging.h"
#include "platform/memory.h"

// TODO(alicia): custom printf!
// TODO(alicia): custom mutex!
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

static usize BUFFER_SIZE        = KILOBYTES(1);
static MemoryBlock PRINT_BUFFER = {};

SM_API void log_init( LogLevel level ) {
    #if defined(LD_LOGGING)
        SM_ASSERT( level <= MAX_LOG_LEVEL );
        GLOBAL_LOG_LEVEL = level;

        MemoryBlock print_buffer = heap_alloc( BUFFER_SIZE );
        SM_ASSERT( print_buffer.pointer );

        PRINT_BUFFER = print_buffer;

        #if defined(SM_PLATFORM_WINDOWS)
            CONSOLE_HANDLE = GetStdHandle( STD_OUTPUT_HANDLE );
            SM_ASSERT( CONSOLE_HANDLE );

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

// this is for locking the logging function
// so that multiple threads can't print over each other
static pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;

SM_API void log_formatted_locked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
) {
#if defined(LD_LOGGING)

    #if defined(SM_ASSERTIONS)
        if( !PRINT_BUFFER.pointer ) {
            printf(
                "LOG INIT WAS NOT CALLED BEFORE THIS LOG MESSAGE!\n"
            );
            #if defined(SM_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
                OutputDebugStringA(
                    "LOG INIT WAS NOT CALLED BEFORE THIS LOG MESSAGE!\n"
                );
            #endif
            SM_PANIC();
        }
    #endif

    pthread_mutex_lock( &MUTEX );

    b32 always_print =
        (flags & LOG_FLAG_ALWAYS_PRINT) == LOG_FLAG_ALWAYS_PRINT;
    b32 new_line =
        (flags & LOG_FLAG_NEW_LINE) == LOG_FLAG_NEW_LINE;

    if( !(always_print || is_level_valid( level )) ) {
        pthread_mutex_unlock( &MUTEX );
        return;
    }


    va_list args;
    va_start( args, format );

    int write_size = vsnprintf(
        (char*)PRINT_BUFFER.pointer,
        BUFFER_SIZE,
        format,
        args
    );

    va_end( args );

    if(
        (!write_size || write_size < 0) ||
        (((usize)write_size) >= (BUFFER_SIZE - 1))
    ) {
        pthread_mutex_unlock( &MUTEX );
        return;
    }

    if( new_line ) {
        ((char*)PRINT_BUFFER.pointer)[write_size] = '\n';
        ((char*)PRINT_BUFFER.pointer)[write_size + 1] = 0;
    }

    set_color( color );
    printf( (char*)PRINT_BUFFER.pointer );
    set_color( LOG_COLOR_RESET );

    #if defined(SM_PLATFORM_WINDOWS) && defined(LD_OUTPUT_DEBUG_STRING)
        OutputDebugStringA( (char*)PRINT_BUFFER.pointer );
    #endif

    pthread_mutex_unlock( &MUTEX );
#endif
}
