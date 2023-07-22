#if !defined(LOGGING_HPP)
#define LOGGING_HPP
/**
 * Description:  Logging functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
 * Notes:        define LD_LOGGING to enable logging
*/
#include "defines.h"
#include "core/string.h"

#define DEFAULT_LOGGING_BUFFER_SIZE KILOBYTES(1)

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

/// Query if logging subsystem is already initialized.
LD_API b32 is_log_initialized();
/// initialize logging subsystem
LD_API b32 log_init( LogLevel level, StringView logging_buffer );

#if defined(LD_API_INTERNAL)
    /// shutdown logging subsystem
    void log_shutdown();
#if defined(LD_PLATFORM_WINDOWS)
    /// enable output debug string [win32 only]
    void log_enable_output_debug_string( b32 enable );
#endif // if platform windows

#endif // if api internal

/// Get the current log level.
LD_API LogLevel query_log_level();

/// log a formatted message, uses a mutex
/// to prevent crosstalk between threads.
LD_API void log_formatted_locked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
);
/// log a formatted message, does not use a mutex.
LD_API void log_formatted_unlocked(
    LogLevel    level,
    LogColor    color,
    LogFlags    flags,
    const char* format,
    ...
);

#define LOG_FATAL( format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        LOG_COLOR_RED,\
        LOG_FLAG_ALWAYS_PRINT | LOG_FLAG_NEW_LINE,\
        "[FATAL | {cc}() | {cc}:{i}] " format,\
        __FUNCTION__,\
        __FILE__,\
        __LINE__,\
        ##__VA_ARGS__\
    )
        

#if defined(LD_LOGGING)
    #define LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE  ] " format,\
            ##__VA_ARGS__\
        )
    #define LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO  ] " format,\
            ##__VA_ARGS__\
        )
    #define LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG ] " format,\
            ##__VA_ARGS__\
        )
    #define LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN  ] " format,\
            ##__VA_ARGS__\
        )
    #define LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR ] " format,\
            ##__VA_ARGS__\
        )

    #define LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE  | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO  | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN  | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
#else
    #define LOG_NOTE( format, ... )
    #define LOG_INFO( format, ... )
    #define LOG_DEBUG( format, ... )
    #define LOG_WARN( format, ... )
    #define LOG_ERROR( format, ... )

    #define LOG_NOTE_TRACE( format, ... )
    #define LOG_INFO_TRACE( format, ... )
    #define LOG_DEBUG_TRACE( format, ... )
    #define LOG_WARN_TRACE( format, ... )
    #define LOG_ERROR_TRACE( format, ... )

#endif

#if defined(LD_ASSERTIONS)
    #define LOG_PANIC( format, ... )\
         log_formatted_unlocked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
            "[PANIC | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        );\
        PANIC()
    #define LOG_ASSERT( condition, format, ... ) \
        do {\
            if(!(condition)) {\
                log_formatted_unlocked(\
                    LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
                    LOG_COLOR_RED,\
                    LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
                    "[ASSERTION FAILED | {cc}() | {cc}:{i}] ({cc}) " format,\
                    __FUNCTION__,\
                    __FILE__,\
                    __LINE__,\
                    #condition,\
                    ##__VA_ARGS__\
                );\
                PANIC();\
            }\
        } while(0)

    #define UNIMPLEMENTED() \
        do {\
            log_formatted_unlocked(\
                LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
                LOG_COLOR_RED,\
                LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
                "[UNIMPLEMENTED | {cc}() | {cc}:{i}] ",\
                __FUNCTION__,\
                __FILE__,\
                __LINE__\
            );\
            PANIC();\
        } while(0)
#else
    #define LOG_PANIC( format, ... ) 
    #define LOG_ASSERT( condition, format, ... ) 
    #define UNIMPLEMENTED()
#endif

#endif
