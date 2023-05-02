/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include <core/logging.h>
#include <core/string.h>
#include <core/memory.h>
#include <core/collections.h>
#include <core/application.h>
#include <stdio.h>

b32 app_run(void*, f32) {
    return true;
}

int main( int, char** ) {

    AppConfig config = {};
    #define NAME_BUFFER_SIZE 255
    char name_buffer[NAME_BUFFER_SIZE] = {};
    snprintf(
        name_buffer,
        NAME_BUFFER_SIZE,
        "Test Bed %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    config.main_surface.name       = name_buffer;
    config.main_surface.dimensions = { 800, 600 };
    config.main_surface.flags      = SURFACE_CREATE_VISIBLE | SURFACE_CREATE_CENTERED;

    config.log_level      = LOG_LEVEL_ALL_VERBOSE;
    config.platform_flags = PLATFORM_DPI_AWARE;

    config.application_run = app_run;

    if( !application_startup( &config ) ) {
        return -1;
    }
    if( !application_run() ) {
        return -1;
    }
    if( !application_shutdown() ) {
        return -1;
    }
    return 0;
}
