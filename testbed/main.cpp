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
#include <core/input.h>
#include <core/events.h>
#include <stdio.h>

EventConsumption on_input( Event* event, void* ) {

    switch( event->code ) {
        case EVENT_CODE_INPUT_KEY: {
            LOG_DEBUG("%s: %s",
                to_string( event->data.keyboard.code ),
                event->data.keyboard.is_down ? "Down" : "Up"
            );
        } break;
        case EVENT_CODE_INPUT_MOUSE_BUTTON: {
            LOG_DEBUG("%s: %s",
                to_string( event->data.mouse_button.code ),
                event->data.mouse_button.is_down ? "Down" : "Up"
            );
        } break;
        case EVENT_CODE_INPUT_MOUSE_MOVE: {
            vec2 ndc = mouse_position_to_ndc(
                event->data.mouse_move.coord,
                {800, 600}
            );
            LOG_DEBUG(
                "mouse position: { %f, %f }",
                ndc.x, ndc.y
            );
        } break;
        default: break;
    }

    return EVENT_NOT_CONSUMED;
}

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

    // SM_LOCAL const usize event_code_count = 3;
    // EventCode codes[event_code_count] = {
    //     EVENT_CODE_INPUT_KEY,
    //     EVENT_CODE_INPUT_MOUSE_BUTTON,
    //     EVENT_CODE_INPUT_MOUSE_MOVE
    // };

    // event_subscribe_multiple_codes(
    //     event_code_count,
    //     codes,
    //     on_input,
    //     nullptr
    // );

    if( !application_run() ) {
        return -1;
    }

    // event_unsubscribe_multiple_codes(
    //     event_code_count,
    //     codes,
    //     on_input,
    //     nullptr
    // );

    if( !application_shutdown() ) {
        return -1;
    }
    return 0;
}
