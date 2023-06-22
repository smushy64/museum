/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include "entry.h"
#include <core/string.h>
#include <core/engine.h>
#include <stdio.h>

int main( int argc, const char** argv ) {

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
            printf(
                "Renderer Backend %s is not supported on this platform!",
                to_string( backend )
            );
            return -1;
        }
    }

    EngineConfig config = {};

    #define NAME_BUFFER_SIZE 32
    char name_buffer[NAME_BUFFER_SIZE] = {};
    snprintf(
        name_buffer,
        NAME_BUFFER_SIZE,
        "Test Bed %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
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

