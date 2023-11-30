/**
 * Description:  Formatted string print implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 29, 2023
*/
#include "defines.h"
#include "core/print.h"
#include "core/fmt.h"
#include "core/internal.h"

LD_API void print_char_stdout( char c ) {
    platform->io.console_write( platform->io.stdout_handle(), 1, &c );
}
LD_API void print_char_stderr( char c ) {
    platform->io.console_write( platform->io.stderr_handle(), 1, &c );
}

LD_API void print_string_stdout( usize len, const char* buffer ) {
    platform->io.console_write( platform->io.stdout_handle(), len, buffer );
}
LD_API void print_string_stderr( usize len, const char* buffer ) {
    platform->io.console_write( platform->io.stderr_handle(), len, buffer );
}

internal usize ___internal_output_string(
    void* target, usize count, char* characters
) {
    platform->io.console_write( target, count, characters );
    return 0;
}

LD_API void ___internal_print( usize format_len, const char* format, ... ) {
    va_list va;
    va_start( va, format );

    ___internal_fmt_write_va(
        ___internal_output_string,
        platform->io.stdout_handle(), format_len, format, va );

    va_end( va );
}
LD_API void ___internal_print_va( usize format_len, const char* format, va_list va ) {
    ___internal_fmt_write_va(
        ___internal_output_string,
        platform->io.stdout_handle(), format_len, format, va );
}
LD_API void ___internal_print_err( usize format_len, const char* format, ... ) {
    va_list va;
    va_start( va, format );

    ___internal_fmt_write_va(
        ___internal_output_string,
        platform->io.stderr_handle(), format_len, format, va );

    va_end( va );
}
LD_API void ___internal_print_err_va(
    usize format_len, const char* format, va_list va
) {
    ___internal_fmt_write_va(
        ___internal_output_string,
        platform->io.stderr_handle(), format_len, format, va );
}

