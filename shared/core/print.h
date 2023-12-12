#if !defined(LD_SHARED_CORE_PRINT_H)
#define LD_SHARED_CORE_PRINT_H
/**
 * Description:  Formatted string print
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 29, 2023
*/
#include "defines.h"

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

/// Output character to stdout.
CORE_API void print_char_stdout( char c );
/// Output character to stderr.
CORE_API void print_char_stderr( char c );

/// Output string to stdout.
CORE_API void print_string_stdout( usize len, const char* buffer );
/// Output string to stderr.
CORE_API void print_string_stderr( usize len, const char* buffer );

/// Print formatted string to stdout.
CORE_API void ___internal_print( usize format_len, const char* format, ... );
/// Print formatted string to stdout using variadic list.
CORE_API void ___internal_print_va( usize format_len, const char* format, va_list va );
/// Print formatted string to stderr.
CORE_API void ___internal_print_err( usize format_len, const char* format, ... );
/// Print formatted string to stderr using variadic list.
CORE_API void ___internal_print_err_va( usize format_len, const char* format, va_list va );

#define print( format, ... )\
    ___internal_print( sizeof(format), format, ##__VA_ARGS__ )
#define print_va( format, va )\
    ___internal_print_va( sizeof(format), format, va )
#define print_err( format, ... )\
    ___internal_print_err( sizeof(format), format, ##__VA_ARGS__ )
#define print_err_va( format, va )\
    ___internal_print_err_va( sizeof(format), format, va )

#define println( format, ... )\
    print( format "\n", ##__VA_ARGS__ )
#define println_err( format, ... )\
    print_err( format "\n", ##__VA_ARGS__ )

#if defined(LD_PLATFORM_WINDOWS)
    CORE_API void output_debug_string( const char* cstr );
#else
    #define output_debug_string( ... ) 
#endif

#endif /* header guard */
