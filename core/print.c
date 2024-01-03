/**
 * Description:  Formatted string print implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 29, 2023
*/
#include "shared/defines.h"
#include "core/print.h"
#include "core/fmt.h"
#include "core/internal/platform.h"

CORE_API void print_char_stdout( char c ) {
    platform_file_write( platform_get_stdout(), 1, &c );
}
CORE_API void print_char_stderr( char c ) {
    platform_file_write( platform_get_stderr(), 1, &c );
}

CORE_API void print_string_stdout( usize len, const char* buffer ) {
    platform_file_write( platform_get_stdout(), len, (void*)buffer );
}
CORE_API void print_string_stderr( usize len, const char* buffer ) {
    platform_file_write( platform_get_stderr(), len, (void*)buffer );
}

internal usize ___internal_output_string(
    void* target, usize count, char* characters
) {
    platform_file_write( target, count, characters );
    return 0;
}

CORE_API void ___internal_print( usize format_len, const char* format, ... ) {
    va_list va;
    va_start( va, format );

    ___internal_fmt_write_va(
        ___internal_output_string,
        platform_get_stdout(), format_len, format, va );

    va_end( va );
}
CORE_API void ___internal_print_va( usize format_len, const char* format, va_list va ) {
    ___internal_fmt_write_va(
        ___internal_output_string,
        platform_get_stdout(), format_len, format, va );
}
CORE_API void ___internal_print_err( usize format_len, const char* format, ... ) {
    va_list va;
    va_start( va, format );

    ___internal_fmt_write_va(
        ___internal_output_string,
        platform_get_stderr(), format_len, format, va );

    va_end( va );
}
CORE_API void ___internal_print_err_va(
    usize format_len, const char* format, va_list va
) {
    ___internal_fmt_write_va(
        ___internal_output_string,
        platform_get_stderr(), format_len, format, va );
}

#if defined(LD_PLATFORM_WINDOWS)

CORE_API void output_debug_string( const char* cstr ) {
    platform_win32_output_debug_string( cstr );
}

#endif

