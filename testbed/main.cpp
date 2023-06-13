/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include <core/string.h>
#include <core/memory.h>
#include <core/collections.h>
#include <core/application.h>
#include <core/input.h>
#include <core/events.h>
#include <core/math.h>
#include <stdio.h>

b32 app_run( void*, f32 ) {
    return true;
}

int main( int argc, char** argv ) {

    RendererBackend backend = BACKEND_OPENGL;

    for( int i = 1; i < argc; ++i ) {
        if( str_cmp( "--gl", argv[i] ) ) {
            backend = BACKEND_OPENGL;
        } else if( str_cmp( "--vk", argv[i] ) ) {
            backend = BACKEND_VULKAN;
        }
    }

    AppConfig config = {};
    #define NAME_BUFFER_SIZE 32
    char name_buffer[NAME_BUFFER_SIZE] = {};
    snprintf(
        name_buffer,
        NAME_BUFFER_SIZE,
        "Test Bed %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    config.opt_surface_icon_path = "./resources/images/ui/testbed_icon_256x256.ico";
    config.surface.name          = name_buffer;
    config.surface.dimensions    = { 800, 600 };

    config.log_level        = LOG_LEVEL_ALL_VERBOSE;
    config.platform_flags   = PLATFORM_DPI_AWARE;
    config.renderer_backend = backend;

    config.application_run = app_run;

    if( !app_init( &config ) ) {
        return -1;
    }
    LOG_INFO(
        "Using renderer backend \"%s\" . . .",
        to_string( backend )
    );

    if( !app_run() ) {
        return -1;
    }

    app_shutdown();
    return 0;
}
