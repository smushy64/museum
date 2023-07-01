/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include "entry.h"
#include <core/string.h>
#include <core/engine.h>

#include <core/math.h>

global char TESTBED_LOGGING_BUFFER[KILOBYTES(1)];

int main( int argc, const char** argv ) {

    StringView logging_buffer = {};
    logging_buffer.buffer = TESTBED_LOGGING_BUFFER;
    logging_buffer.len    = KILOBYTES(1);
    LD_ASSERT(log_init( LOG_LEVEL_ALL_VERBOSE, logging_buffer ));

    RendererBackend backend = RENDERER_BACKEND_OPENGL;

    for( int i = 1; i < argc; ++i ) {
        if( string_cmp( "--gl", argv[i] ) ) {
            backend = RENDERER_BACKEND_OPENGL;
        } else if( string_cmp( "--vk", argv[i] ) ) {
            backend = RENDERER_BACKEND_VULKAN;
        } else if( string_cmp( "--dx11", argv[i] ) ) {
            backend = RENDERER_BACKEND_DX11;
        }  else if( string_cmp( "--dx12", argv[i] ) ) {
            backend = RENDERER_BACKEND_DX12;
        } 
        if( !renderer_backend_is_supported( backend ) ) {
            println(
                "Renderer Backend {cc} is not supported on this platform!",
                to_string( backend )
            );
            return -1;
        }
    }

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
    config.platform_flags   = (1 << 0);
    config.renderer_backend = backend;

    b32 result = engine_run(
        argc, (char**)argv,
        entry, nullptr,
        &config
    );

    return result ? 0 : -1;
}

