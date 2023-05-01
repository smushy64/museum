#if !defined(LOGGING_HPP)
#define LOGGING_HPP
/**
 * Description:  Logging functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
 * Notes:        define LD_LOGGING to enable logging
 *               define LD_OUTPUT_DEBUG_STRING to
 *                 use OutputDebugStringA on Win32
 *               always call "log_init" at the
 *                 very start of the program
*/
#include "defines.h"

enum LogColor : u32 {
    LOG_COLOR_BLACK,
    LOG_COLOR_RED,
    LOG_COLOR_GREEN,
    LOG_COLOR_YELLOW,
    LOG_COLOR_MAGENTA,
    LOG_COLOR_CYAN,
    LOG_COLOR_WHITE,
    LOG_COLOR_BLUE,
    LOG_COLOR_RESET,
    LOG_COLOR_DEFAULT,

    LOG_COLOR_COUNT
};

#define LOG_LEVEL_NONE    0
#define LOG_LEVEL_ERROR   (1 << 0)
#define LOG_LEVEL_WARN    (1 << 1)
#define LOG_LEVEL_DEBUG   (1 << 2)
#define LOG_LEVEL_INFO    (1 << 3)
#define LOG_LEVEL_TRACE   (1 << 4)
#define LOG_LEVEL_VERBOSE (1 << 5)

#define LOG_LEVEL_ALL (\
    LOG_LEVEL_ERROR |\
    LOG_LEVEL_WARN  |\
    LOG_LEVEL_DEBUG |\
    LOG_LEVEL_INFO   \
)

#define LOG_LEVEL_ALL_TRACE (\
    LOG_LEVEL_ERROR |\
    LOG_LEVEL_WARN  |\
    LOG_LEVEL_DEBUG |\
    LOG_LEVEL_INFO  |\
    LOG_LEVEL_TRACE  \
)

#define LOG_LEVEL_ALL_VERBOSE (\
    LOG_LEVEL_ERROR   |\
    LOG_LEVEL_WARN    |\
    LOG_LEVEL_DEBUG   |\
    LOG_LEVEL_INFO    |\
    LOG_LEVEL_TRACE   |\
    LOG_LEVEL_VERBOSE  \
)

typedef u32 LogLevel;

#define LOG_FLAG_ALWAYS_PRINT (1 << 0)
#define LOG_FLAG_NEW_LINE     (1 << 1)

typedef u32 LogFlags;

/// initialize logging library
SM_API b32 log_init( LogLevel level );
/// log a formatted message, uses a mutex
/// to prevent crosstalk between threads.
SM_API void log_formatted_locked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
);

#if defined(LD_LOGGING)
    #define LOG_NOTE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE  ] " __VA_ARGS__\
        )
    #define LOG_INFO( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO  ] " __VA_ARGS__\
        )
    #define LOG_DEBUG( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG ] " __VA_ARGS__\
        )
    #define LOG_WARN( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN  ] " __VA_ARGS__\
        )
    #define LOG_ERROR( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR ] " __VA_ARGS__\
        )


    #define LOG_NOTE_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET, 0,\
            "[NOTE  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define LOG_INFO_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE, 0,\
            "[INFO  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define LOG_DEBUG_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE, 0,\
            "[DEBUG | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define LOG_WARN_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW, 0,\
            "[WARN  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define LOG_ERROR_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED, 0,\
            "[ERROR | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define LOG_FATAL( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_ALWAYS_PRINT,\
            "[FATAL | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_ALWAYS_PRINT | LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
#else
    #define LOG_NOTE( ... )
    #define LOG_INFO( ... )
    #define LOG_DEBUG( ... )
    #define LOG_WARN( ... )
    #define LOG_ERROR( ... )

    #define LOG_NOTE_TRACE( ... )
    #define LOG_INFO_TRACE( ... )
    #define LOG_DEBUG_TRACE( ... )
    #define LOG_WARN_TRACE( ... )
    #define LOG_ERROR_TRACE( ... )

    #define LOG_FATAL( ... )
#endif

#if defined(SM_ASSERTIONS)

    #if defined(LD_LOGGING)

        #define LOG_ASSERT( condition, ... ) \
            do {\
                if(!(condition)) {\
                    log_formatted_locked(\
                        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
                        LOG_COLOR_RED,\
                        LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
                        "[ASSERTION FAILED | %s | %s:%i] %s",\
                        __FUNCTION__,\
                        __FILE__,\
                        __LINE__,\
                        #condition\
                    );\
                    log_formatted_locked(\
                        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
                        LOG_COLOR_RED,\
                        LOG_FLAG_ALWAYS_PRINT | LOG_FLAG_NEW_LINE,\
                        __VA_ARGS__\
                    );\
                    SM_PANIC();\
                }\
            } while(0)

        #define DEBUG_UNIMPLEMENTED() \
            do {\
                log_formatted_locked(\
                    LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
                    LOG_COLOR_RED,\
                    LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
                    "[UNIMPLEMENTED | %s | %s:%i] ",\
                    __FUNCTION__,\
                    __FILE__,\
                    __LINE__\
                );\
                SM_PANIC();\
            } while(0)
    #else
        #define LOG_ASSERT( condition, ... ) SM_ASSERT( condition )
        #define DEBUG_UNIMPLEMENTED() SM_PANIC()
    #endif

#else
    #define LOG_ASSERT( condition, ... )
    #define DEBUG_UNIMPLEMENTED()
#endif

#endif