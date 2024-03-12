/**
 * Description:  Logging subsystem for Packager Utility
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 08, 2024
*/
#include "shared/defines.h"
#include "package/logging.h"

#include "core/print.h"
#include "core/sync.h"

global b32 global_is_verbose = false;
global b32 global_is_silent  = false;

global Mutex global_mutex = {};

b32 logging_initialize( b32 verbose, b32 silent ) {
    global_is_verbose = verbose;
    global_is_silent  = silent;

    if( !mutex_create( &global_mutex ) ) {
        return false;
    }

    return true;
}

void logging_print( b32 verbose, b32 error, usize format_len, char* format, ... ) {
    if( !global_is_verbose && verbose ) {
        return;
    }

    if( global_is_silent ) {
        if( !error ) {
            return;
        }
    }

    mutex_lock( &global_mutex );
    read_write_fence();

    va_list va;
    va_start( va, format );

    if( error ) {
        ___internal_print_err_va( format_len, format, va );
    } else {
        ___internal_print_va( format_len, format, va );
    }

    va_end( va );

    read_write_fence();
    mutex_unlock( &global_mutex );
}


