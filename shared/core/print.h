#if !defined(LD_SHARED_CORE_PRINT_H)
#define LD_SHARED_CORE_PRINT_H
/**
 * Description:  Formatted string print
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 29, 2023
*/
#include "defines.h"

/// Output character to stdout.
LD_API void char_output_stdout( char c );
/// Output character to stderr.
LD_API void char_output_stderr( char c );

/// Print formatted string to stdout.
LD_API void ___internal_print( usize format_len, const char* format, ... );
/// Print formatted string to stdout using variadic list.
LD_API void ___internal_print_va( usize format_len, const char* format, va_list va );
/// Print formatted string to stderr.
LD_API void ___internal_print_err( usize format_len, const char* format, ... );
/// Print formatted string to stderr using variadic list.
LD_API void ___internal_print_err_va( usize format_len, const char* format, va_list va );

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

#endif /* header guard */
