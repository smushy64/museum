#if !defined(LD_PACKAGE_LOGGING_H)
#define LD_PACKAGE_LOGGING_H
/**
 * Description:  Logging subsystem for Packager Utility
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 08, 2024
*/
#include "shared/defines.h"
#include "core/print.h"

b32 logging_initialize( b32 verbose, b32 silent );

void logging_print( b32 verbose, b32 error, usize format_len, char* format, ... );

#define log_error( format, ... )\
    logging_print( false, true,\
        sizeof( CONSOLE_COLOR_RED "[{usize}] " format "\n" CONSOLE_COLOR_RESET),\
        CONSOLE_COLOR_RED "[{usize}] " format "\n" CONSOLE_COLOR_RESET,\
        thread_index, ##__VA_ARGS__ )
#define log_print( format, ... )\
    logging_print( false, false, sizeof(format "\n" ),\
    format "\n", ##__VA_ARGS__ )
#define log_note( format, ... )\
    logging_print( true, false, sizeof("[{usize}] " format "\n"),\
    "[{usize}] " format "\n", thread_index, ##__VA_ARGS__ )

#endif /* header guard */
