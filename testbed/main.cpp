/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include <core/string.h>
#include <core/memory.h>
#include <core/collections.h>
#include <core/engine.h>
#include <core/input.h>
#include <core/events.h>
#include <core/math.h>
#include <core/graphics.h>
#include <core/threading.h>
#include <core/time.h>
#include <stdio.h>

b32 run(
    struct EngineContext*   engine_ctx,
    struct ThreadWorkQueue* thread_work_queue,
    struct RenderOrder* render_order,
    struct Time* time,
    void* user_params
) {
    SM_UNUSED(engine_ctx),
    SM_UNUSED(thread_work_queue);
    SM_UNUSED(render_order);
    SM_UNUSED(time);
    SM_UNUSED(user_params);
    return true;
}

int main( int argc, char** argv ) {

    RendererBackend backend = 0;

    for( int i = 1; i < argc; ++i ) {
        if( str_cmp( "--gl", argv[i] ) ) {
            backend = 0;
        } else if( str_cmp( "--vk", argv[i] ) ) {
            backend = 1;
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
        argc, argv,
        run, nullptr,
        &config
    );

    return result ? 0 : -1;
}
