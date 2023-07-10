/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include "entry.h"
#include <core/string.h>
#include <core/engine.h>
#include <core/time.h>
#include <core/math.h>
#include <core/memory.h>

int init( int argc, const char** argv );
int main( int argc, const char** argv ) {
    return init( argc, argv );
}

int init( int argc, const char** argv ) {

    RendererBackend backend = RENDERER_BACKEND_OPENGL;
    EngineConfig config = {};

    #define NAME_BUFFER_SIZE 32
    char name_buffer[NAME_BUFFER_SIZE] = {};
    StringView name_buffer_view = {};
    name_buffer_view.buffer = name_buffer;
    name_buffer_view.len    = NAME_BUFFER_SIZE;
    string_format(
        name_buffer_view,
        "Test Bed {i}.{i}{c}",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR,
        0
    );

    config.opt_application_icon_path = "./resources/images/ui/testbed_icon_256x256.ico";
    config.application_name          = name_buffer;
    config.surface_dimensions        = { 800, 600 };
    config.log_level        = LOG_LEVEL_ALL_VERBOSE;
    // TODO(alicia): expose these flags somewhere
    config.platform_flags   = (1 << 0) | (1 << 1);
    config.renderer_backend = backend;

    b32 result = engine_run(
        argc, (char**)argv,
        entry, nullptr,
        &config
    );

    return result ? 0 : -1;
}

