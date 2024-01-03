#if !defined(LD_CORE_LOGGING_H)
#define LD_CORE_LOGGING_H
/**
 * Description:  Logging functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
 * Notes:        define LD_LOGGING to enable logging
*/
#include "shared/defines.h"
#include "core/print.h"

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
LD_API void ___internal_logging_output_fmt(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, ... );
#define logging_output_fmt( type, opt_color_override, trace, always_log, new_line, timestamped, format, ... )\
    ___internal_logging_output_fmt( type, opt_color_override, trace, always_log, new_line, timestamped, sizeof(format), format, ##__VA_ARGS__ )
/// Output formatted logging message.
/// Uses a mutex to make sure there is no cross-talk between threads.
LD_API void ___internal_logging_output_fmt_locked(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, ... );
#define logging_output_fmt_locked( type, opt_color_override, trace, always_log, new_line, timestamped, format, ... )\
    ___internal_logging_output_fmt_locked( type, opt_color_override, trace, always_log, new_line, timestamped, sizeof(format), format, ##__VA_ARGS__ )
/// Output formatted logging message using variadic list.
/// Is not thread-safe, use logging_output_fmt_locked_va instead.
LD_API void ___internal_logging_output_fmt_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, va_list va );
#define logging_output_fmt_va( type, opt_color_override, trace, always_log, new_line, timestamped, format, va )\
    ___internal_logging_output_fmt_va( type, opt_color_override, trace, always_log, new_line, timestamped, sizeof(format), format, va )
/// Output formatted logging message using variadic list.
/// Uses a mutex to make sure there is no cross-talk between threads.
LD_API void ___internal_logging_output_fmt_locked_va(
    LoggingType type, ConsoleColor* opt_color_override,
    b32 trace, b32 always_log, b32 new_line, b32 timestamped,
    usize format_len, const char* format, va_list va );
#define logging_output_fmt_locked_va( type, opt_color_override, trace, always_log, new_line, timestamped, format, va )\
    ___internal_logging_output_fmt_locked_va( type, opt_color_override, trace, always_log, new_line, timestamped, sizeof(format), format, va )

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

