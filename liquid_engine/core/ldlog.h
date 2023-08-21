#if !defined(LOGGING_HPP)
#define LOGGING_HPP
/**
 * Description:  Logging functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
 * Notes:        define LD_LOGGING to enable logging
*/
#include "defines.h"
#include "core/ldstring.h"

/// Log level
typedef u32 LogLevel;

/// Set logging level.
LD_API void log_set_log_level( LogLevel level );
/// Get current logging level.
LD_API LogLevel log_query_log_level();

/// log a formatted message, uses a mutex
/// to prevent crosstalk between threads.
LD_API void log_formatted_locked(
    LogLevel    level,
    b32         always_print,
    b32         new_line,
    const char* format,
    ...
);
/// log a formatted message, does not use a mutex.
LD_API void log_formatted_unlocked(
    LogLevel    level,
    b32         always_print,
    b32         new_line,
    const char* format,
    ...
);

#define LOG_COLOR_BLACK   "\033[1;30m"
#define LOG_COLOR_RED     "\033[1;31m"
#define LOG_COLOR_GREEN   "\033[1;32m"
#define LOG_COLOR_YELLOW  "\033[1;33m"
#define LOG_COLOR_MAGENTA "\033[1;35m"
#define LOG_COLOR_CYAN    "\033[1;36m"
#define LOG_COLOR_WHITE   "\033[1;37m"
#define LOG_COLOR_BLUE    "\033[1;34m"
#define LOG_COLOR_RESET   "\033[1;0m"
#define LOG_COLOR_DEFAULT "\033[1;39m"

/// No logs will be printed
#define LOG_LEVEL_NONE    ((LogLevel)(0))
/// Only error logs will be printed.
#define LOG_LEVEL_ERROR   ((LogLevel)(1 << 0))
/// Only warning logs will be printed.
#define LOG_LEVEL_WARN    ((LogLevel)(1 << 1))
/// Only debug logs will be printed.
#define LOG_LEVEL_DEBUG   ((LogLevel)(1 << 2))
/// Only info logs will be printed.
#define LOG_LEVEL_INFO    ((LogLevel)(1 << 3))
/// Trace logs will be printed.
#define LOG_LEVEL_TRACE   ((LogLevel)(1 << 4))
/// Verbose logs will be printed.
#define LOG_LEVEL_VERBOSE ((LogLevel)(1 << 5))

/// All severity logs will be printed.
#define LOG_LEVEL_ALL (\
    LOG_LEVEL_ERROR |\
    LOG_LEVEL_WARN  |\
    LOG_LEVEL_DEBUG |\
    LOG_LEVEL_INFO   \
)
/// All severity logs and trace logs will be printed.
#define LOG_LEVEL_ALL_TRACE (\
    LOG_LEVEL_ERROR |\
    LOG_LEVEL_WARN  |\
    LOG_LEVEL_DEBUG |\
    LOG_LEVEL_INFO  |\
    LOG_LEVEL_TRACE  \
)
/// All logs will be printed.
#define LOG_LEVEL_ALL_VERBOSE (\
    LOG_LEVEL_ERROR   |\
    LOG_LEVEL_WARN    |\
    LOG_LEVEL_DEBUG   |\
    LOG_LEVEL_INFO    |\
    LOG_LEVEL_TRACE   |\
    LOG_LEVEL_VERBOSE  \
)

#if defined(LD_API_INTERNAL)
    /// initialize logging subsystem
    b32 log_subsystem_init(
        LogLevel level, usize log_buffer_size, void* log_buffer );

    /// shutdown logging subsystem
    void log_subsystem_shutdown();

#if defined(LD_PLATFORM_WINDOWS)
    /// enable output debug string [win32 only]
    void log_subsystem_win32_enable_output_debug_string();
#endif // if platform windows

#endif // if api internal

#define LOG_FATAL( format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        true, true,\
        LOG_COLOR_RED\
        "[FATAL | {cc}() | {cc}:{i}] " format\
        LOG_COLOR_RESET,\
        __FUNCTION__,\
        __FILE__,\
        __LINE__,\
        ##__VA_ARGS__\
    )
        

#if defined(LD_LOGGING)
    #define LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[NOTE] " format,\
            ##__VA_ARGS__\
        )
    #define LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            false, true,\
            LOG_COLOR_WHITE\
            "[INFO] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            false, true,\
            LOG_COLOR_BLUE\
            "[DEBUG] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[WARN] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            false, true,\
            LOG_COLOR_RED\
            "[ERROR] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )

    #define LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[NOTE | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_WHITE\
            "[INFO | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_BLUE\
            "[DEBUG | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[WARN | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_RED\
            "[ERROR | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
#else
    #define LOG_NOTE( format, ... ) unused(format)
    #define LOG_INFO( format, ... ) unused(format)
    #define LOG_DEBUG( format, ... ) unused(format)
    #define LOG_WARN( format, ... ) unused(format)
    #define LOG_ERROR( format, ... ) unused(format)

    #define LOG_NOTE_TRACE( format, ... ) unused(format)
    #define LOG_INFO_TRACE( format, ... ) unused(format)
    #define LOG_DEBUG_TRACE( format, ... ) unused(format)
    #define LOG_WARN_TRACE( format, ... ) unused(format)
    #define LOG_ERROR_TRACE( format, ... ) unused(format)

#endif

#if defined(LD_ASSERTIONS)
    #define LOG_PANIC( format, ... )\
         log_formatted_unlocked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            true, true,\
            LOG_COLOR_RED\
            "[PANIC | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
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
                    true, true,\
                    LOG_COLOR_RED\
                    "[ASSERTION FAILED | {cc}() | {cc}:{i}] ({cc}) " format\
                    LOG_COLOR_RESET,\
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
                true, true,\
                LOG_COLOR_RED\
                "[UNIMPLEMENTED | {cc}() | {cc}:{i}]"\
                LOG_COLOR_RESET,\
                __FUNCTION__,\
                __FILE__,\
                __LINE__\
            );\
            PANIC();\
        } while(0)
#else
    #define LOG_PANIC( format, ... ) unused(format)
    #define LOG_ASSERT( condition, format, ... ) unused(condition)
    #define UNIMPLEMENTED()
#endif

#endif
