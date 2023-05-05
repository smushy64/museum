/**
 * Description:  Application Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "application.h"
#include "platform/platform.h"
#include "events.h"
#include "logging.h"
#include "input.h"

// TODO(alicia): custom string formatting!
#include <stdio.h>

struct AppContext {
    PlatformState platform;
    Surface       main_surface;
    SystemInfo    sysinfo;

    b32 is_active;
    b32 is_running;

    AppRunFn application_run;
    void*    application_params;
};

SM_GLOBAL AppContext CONTEXT = {};

EventConsumption on_main_surface_destroy( Event* event, void* ) {
    if(
        event->data.surface_destroy.surface ==
        &CONTEXT.main_surface
    ) {
        CONTEXT.is_running = false;
        return EVENT_CONSUMED;
    } else {
        return EVENT_NOT_CONSUMED;
    }
}

EventConsumption on_main_surface_active( Event* event, void* ) {
    if(
        event->data.surface_active.surface ==
        &CONTEXT.main_surface
    ) {
        b32 is_active = event->data.surface_active.is_active;
        if( is_active ) {
            LOG_NOTE("Main surface activated.");
        } else {
            LOG_NOTE("Main surface deactivated.");
        }
        CONTEXT.is_active = is_active;
        return EVENT_CONSUMED;
    } else {
        return EVENT_NOT_CONSUMED;
    }
}

EventConsumption on_main_surface_resize( Event* event, void* ) {
    if(
        event->data.surface_resize.surface ==
        &CONTEXT.main_surface
    ) {
        // TODO(alicia): 
        SM_UNUSED(event);
    }
    return EVENT_NOT_CONSUMED;
}

b32 application_startup( AppConfig* config ) {

#if defined(LD_LOGGING)
    if( !log_init( config->log_level ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize logging subsystem"
        );
        return false;
    }
#endif

    SM_ASSERT(config->application_run);
    CONTEXT.application_run    = config->application_run;
    CONTEXT.application_params = config->application_params;

    LOG_NOTE("Liquid Engine Version: %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    if( !platform_init(
        config->platform_flags,
        &CONTEXT.platform
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services."
        );
        return false;
    }

    if( !event_init() ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if( !input_init() ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize input subsystem."
        );
        return false;
    }

    if(!event_subscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_main_surface_destroy,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_main_surface_active,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_main_surface_resize,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    
    CONTEXT.sysinfo = query_system_info();

    LOG_NOTE("CPU: %s", CONTEXT.sysinfo.cpu_name_buffer);
    LOG_NOTE("  Threads: %llu", CONTEXT.sysinfo.thread_count);

#if defined(SM_ARCH_X86)
    b32 sse = ARE_SSE_INSTRUCTIONS_AVAILABLE( CONTEXT.sysinfo.features );
    if( SM_SIMD_WIDTH == 4 && !sse ) {
        SM_LOCAL usize ERROR_MESSAGE_SIZE = 0xFF;
        char error_message_buffer[ERROR_MESSAGE_SIZE];
        snprintf(
            error_message_buffer,
            ERROR_MESSAGE_SIZE,
            "Your CPU does not support SSE instructions!\n"
            "Missing instructions: %s%s%s%s%s%s",
            IS_SSE_AVAILABLE(CONTEXT.sysinfo.features)    ? "" : "SSE, ",
            IS_SSE2_AVAILABLE(CONTEXT.sysinfo.features)   ? "" : "SSE2, ",
            IS_SSE3_AVAILABLE(CONTEXT.sysinfo.features)   ? "" : "SSE3, ",
            IS_SSSE3_AVAILABLE(CONTEXT.sysinfo.features)  ? "" : "SSSE3, ",
            IS_SSE4_1_AVAILABLE(CONTEXT.sysinfo.features) ? "" : "SSE4.1, ",
            IS_SSE4_2_AVAILABLE(CONTEXT.sysinfo.features) ? "" : "SSE4.2"
        );
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            error_message_buffer
        );
        return false;
    }

    b32 avx  = IS_AVX_AVAILABLE( CONTEXT.sysinfo.features );
    b32 avx2 = IS_AVX2_AVAILABLE( CONTEXT.sysinfo.features );

    if( SM_SIMD_WIDTH == 8 && !(avx && avx2) ) {
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            "Your CPU does not support AVX/AVX2 instructions! "
            "This program requires them!"
        );
        return false;
    }

    b32 avx512 = IS_AVX512_AVAILABLE(
        CONTEXT.sysinfo.features
    );

    LOG_NOTE(
        "  Features: %s%s%s%s",
        sse    ? "SSE1-4 " : "",
        avx    ? "AVX " : "",
        avx2   ? "AVX2 " : "",
        avx512 ? "AVX-512 " : ""
    );
#endif

    LOG_NOTE("Memory: %6.3f GB",
        MB_TO_GB(
            KB_TO_MB(
                BYTES_TO_KB(
                    CONTEXT.sysinfo.total_memory
                )
            )
        )
    );

    if(!surface_create(
        config->main_surface.name,
        config->main_surface.position,
        config->main_surface.dimensions,
        config->main_surface.flags,
        &CONTEXT.platform,
        nullptr,
        &CONTEXT.main_surface
    )) {
        MESSAGE_BOX_FATAL(
            "Surface Failure",
            "Failed to create surface."
        );
        return false;
    }

    CONTEXT.is_running = true;
    return true;
}
b32 application_run() {

    while( CONTEXT.is_running ) {
        input_swap();
        platform_poll_gamepad();
        surface_pump_events( &CONTEXT.main_surface );

        if( !CONTEXT.is_active ) {
            continue;
        }

        if(!CONTEXT.application_run(
            CONTEXT.application_params,
            0.0f
        )) {
            return false;
        }
    }

    return true;
}
b32 application_shutdown() {
    b32 success = true;
    if(!event_unsubscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_main_surface_destroy,
        nullptr
    )) {
        success = false;
    }
    if(!event_unsubscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_main_surface_active,
        nullptr
    )) {
        success = false;
    }
    if(!event_unsubscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_main_surface_resize,
        nullptr
    )) {
        success = false;
    }

    if( !event_shutdown() ) {
        success = false;
    }
    if( !input_shutdown() ) {
        success = false;
    }

    CONTEXT.is_running = false;
    surface_destroy( &CONTEXT.main_surface );
    platform_shutdown( &CONTEXT.platform );
    log_shutdown();
    return success;
}
