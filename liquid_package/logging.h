#if !defined(LP_LOGGING_H)
#define LP_LOGGING_H
/**
 * Description:  Liquid Packager Logging
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 08, 2023
*/
#include "defines.h"
#include "engine/logging.h"

#define lp_error( format, ... )\
    logging_output_fmt_locked( LOGGING_TYPE_ERROR, NULL, false, true, true, false, format, ##__VA_ARGS__ )

#define lp_warn( format, ... )\
    logging_output_fmt_locked( LOGGING_TYPE_WARN, NULL, false, false, true, false, format, ##__VA_ARGS__ )

#define lp_note( format, ... )\
    logging_output_fmt_locked( LOGGING_TYPE_NOTE, NULL, true, false, true, false, format, ##__VA_ARGS__ )

#define lp_print( format, ... )\
    logging_output_fmt_locked( LOGGING_TYPE_NOTE, NULL, false, true, true, false, format, ##__VA_ARGS__ )

#endif /* header guard */
