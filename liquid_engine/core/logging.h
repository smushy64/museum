#if !defined(LD_CORE_LOGGING_H)
#define LD_CORE_LOGGING_H
/**
 * Description:  Logging functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
 * Notes:        define LD_LOGGING to enable logging
*/
#include "defines.h"

/// Logging levels.
/// Used to define what log messages should be permitted.
typedef u32 LoggingLevel;
#define LOGGING_LEVEL_NONE  0
#define LOGGING_LEVEL_ERROR (1 << 0)
#define LOGGING_LEVEL_WARN  (1 << 1)
#define LOGGING_LEVEL_DEBUG (1 << 2)
#define LOGGING_LEVEL_INFO  (1 << 3)
#define LOGGING_LEVEL_NOTE  (1 << 4)
#define LOGGING_LEVEL_TRACE (1 << 5)

#define LOGGING_LEVEL_ALL (\
    LOGGING_LEVEL_ERROR |\
    LOGGING_LEVEL_WARN  |\
    LOGGING_LEVEL_DEBUG |\
    LOGGING_LEVEL_INFO  |\
    LOGGING_LEVEL_NOTE  |\
    LOGGING_LEVEL_TRACE\
)

/// Log types.
/// Used to define what type of log a message is.
typedef enum LoggingType : u32 {
    LOGGING_TYPE_FATAL,
    LOGGING_TYPE_ERROR,
    LOGGING_TYPE_WARN,
    LOGGING_TYPE_DEBUG,
    LOGGING_TYPE_INFO,
    LOGGING_TYPE_NOTE,
} LoggingType;

/// Color codes for colored console messages.
typedef const char ConsoleColor;
#define CONSOLE_COLOR_BLACK   "\033[1;30m"
#define CONSOLE_COLOR_RED     "\033[1;31m"
#define CONSOLE_COLOR_GREEN   "\033[1;32m"
#define CONSOLE_COLOR_YELLOW  "\033[1;33m"
#define CONSOLE_COLOR_MAGENTA "\033[1;35m"
#define CONSOLE_COLOR_CYAN    "\033[1;36m"
#define CONSOLE_COLOR_WHITE   "\033[1;37m"
#define CONSOLE_COLOR_BLUE    "\033[1;34m"
#define CONSOLE_COLOR_RESET   "\033[1;00m"

struct StringSlice;

#if defined(LD_API_INTERNAL)

/// Initialize logging subsystem.
/// Optionally takes a handle to file to output messages to.
void logging_subsystem_initialize( void* opt_output_file );
/// Detach output file handle.
void logging_subsystem_detach_file(void);
/// Attach output file handle.
void logging_subsystem_attach_file( void* output_file );

#if defined(LD_PLATFORM_WINDOWS)

/// Enable Win32 OutputDebugStringA.
void logging_set_output_debug_string_enabled( b32 is_enabled );
/// Query if outputting to OutputDebugStringA is enabled.
b32 logging_query_output_debug_string_enabled(void);

#endif // Platform Windows

#endif // API Internal

/// Set logging level.
LD_API void logging_set_level( LoggingLevel level );
/// Query what the current logging level is.
LD_API LoggingLevel logging_query_level(void);

/// Output logging message.
/// Is not thread-safe, use logging_output_locked instead.
LD_API void logging_output(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    struct StringSlice* message );
/// Output logging message.
/// Uses a mutex to make sure there is no cross-talk between threads.
LD_API void logging_output_locked(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    struct StringSlice* message );
/// Output formatted logging message.
/// Is not thread-safe, use logging_output_fmt_locked instead.
LD_API void logging_output_fmt(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, ... );
/// Output formatted logging message.
/// Uses a mutex to make sure there is no cross-talk between threads.
LD_API void logging_output_fmt_locked(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, ... );
/// Output formatted logging message using variadic list.
/// Is not thread-safe, use logging_output_fmt_locked_va instead.
LD_API void logging_output_fmt_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, va_list va );
/// Output formatted logging message using variadic list.
/// Uses a mutex to make sure there is no cross-talk between threads.
LD_API void logging_output_fmt_locked_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    const char* format, va_list va );

#if defined(LD_LOGGING)
    #define fatal_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_FATAL, NULL,\
			false, true, true, true,\
            "[FATAL | " __FILE__ ":{u} > {cc}()] " format,\
            __LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define error_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_ERROR, NULL,\
			false, false, true, true, "[ERROR] " format, ##__VA_ARGS__ )
    #define warn_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_WARN, NULL,\
			false, false, true, true, "[WARN] " format, ##__VA_ARGS__ )
    #define debug_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_DEBUG, NULL,\
			false, false, true, true, "[DEBUG] " format, ##__VA_ARGS__ )
    #define info_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_INFO, NULL,\
			false, false, true, true, "[INFO] " format, ##__VA_ARGS__ )
    #define note_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_NOTE, NULL,\
			false, false, true, true, "[NOTE] " format, ##__VA_ARGS__ )

    #define trace_error_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_ERROR, NULL,\
			true, false, true, true,\
            "[ERROR | " __FILE__ ":{u} > {cc}()] " format,\
            __LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_warn_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_WARN, NULL,\
			true, false, true, true,\
            "[WARN | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_debug_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_DEBUG, NULL,\
			true, false, true, true,\
            "[DEBUG | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_info_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_INFO, NULL,\
			true, false, true, true,\
            "[INFO | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
    #define trace_note_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_NOTE, NULL,\
			true, false, true, true,\
            "[NOTE | " __FILE__ ":{u} > {cc}()] " format,\
			__LINE__, __FUNCTION__, ##__VA_ARGS__ )
#else
    #define fatal_log( ... )
    #define error_log( ... )
    #define warn_log( ... )
    #define debug_log( ... )
    #define info_log( ... )
    #define note_log( ... )

    #define trace_error_log( ... )
    #define trace_warn_log( ... )
    #define trace_debug_log( ... )
    #define trace_info_log( ... )
    #define trace_note_log( ... )
#endif // LD_LOGGING

#if defined(LD_ASSERTIONS) && defined(LD_LOGGING)
    #define assert_log( condition, format, ... ) do {\
            if( !(condition) ) {\
                logging_output_fmt_locked(\
                    LOGGING_TYPE_FATAL, NULL,\
                    true, true, true, true, format, ##__VA_ARGS__ );\
                panic();\
            }\
        } while(0)
    #define panic_log( format, ... )\
        logging_output_fmt_locked(\
            LOGGING_TYPE_FATAL, NULL,\
            true, true, true, true, format, ##__VA_ARGS__ );\
        panic()
    #define unimplemented()\
        logging_output_fmt_locked(\
            LOGGING_TYPE_FATAL, NULL,\
            true, true, true, true,\
            "[UNIMPLEMENTED] in file "\
            __FILE__ " at line {u} in function {cc}()", __LINE__, __FUNCTION__ );\
        panic()
#else
    #define assert_log( ... )
    #define panic_log( ... )
    #define unimplemented()
#endif // LD_ASSERTIONS

#endif /* header guard */

