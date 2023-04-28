/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "testbed_pch.h"
#include <core/logging.h>
#include <platform/os.h>
#include <platform/memory.h>
#include <stdio.h>

int main( int, char** ) {
    logging_init( LOG_LEVEL_ALL_VERBOSE );

    return 0;
}
