/**
 * Description:  Application Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "application.h"
#include "platform/platform.h"
#include "renderer/frontend.h"
#include "events.h"
#include "logging.h"
#include "input.h"
#include "time.h"
#include "memory.h"
#include "string.h"

// TODO(alicia): custom string formatting!
#include <stdio.h>

struct AppContext {
    Platform   platform;
    SystemInfo sysinfo;
    b32 is_running;

    struct {
        f32 delta_time;
        f32 elapsed_time;
        u64 frame_count;
    } time;

    AppRunFn application_run;
    void*    application_params;
};

global AppContext CONTEXT = {};

EventReturnCode on_app_exit( Event*, void* ) {
    CONTEXT.is_running = false;
    return EVENT_CONSUMED;
}

EventReturnCode on_main_surface_destroy( Event*, void* ) {
    CONTEXT.is_running = false;
    return EVENT_CONSUMED;
}

EventReturnCode on_main_surface_active( Event* event, void* ) {
    b32 is_active = event->data.surface_active.is_active;
    if( is_active ) {
        LOG_NOTE("Surface activated.");
    } else {
        LOG_NOTE("Surface deactivated.");
    }
    return EVENT_CONSUMED;
}

EventReturnCode on_main_surface_resize( Event* event, void* ) {
    // TODO(alicia): 
    SM_UNUSED(event);
    return EVENT_NOT_CONSUMED;
}

EventReturnCode on_f4( Event* event, void* ) {
    if( event->data.keyboard.code == KEY_F4 ) {
        if(
            input_is_key_down( KEY_ALT_LEFT ) ||
            input_is_key_down( KEY_ALT_RIGHT )
        ) {
            Event event = {};
            event.code = EVENT_CODE_APP_EXIT;
            event_fire( event );
        }
    }
    return EVENT_NOT_CONSUMED;
}

b32 app_init( AppConfig* config ) {

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

    LOG_INFO("Liquid Engine Version: %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    #define MAX_SURFACE_NAME 255

    char surface_name_buffer[MAX_SURFACE_NAME];
    snprintf(
        surface_name_buffer,
        MAX_SURFACE_NAME,
        "%s | %s",
        config->main_surface.name,
        to_string( config->renderer_backend )
    );

    if( !platform_init(
        surface_name_buffer,
        config->main_surface.dimensions,
        0,
        &CONTEXT.platform
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services."
        );
        return false;
    }

    CONTEXT.sysinfo = query_system_info();

    LOG_NOTE("CPU: %s", CONTEXT.sysinfo.cpu_name_buffer);
    LOG_NOTE("  Threads: %llu", CONTEXT.sysinfo.thread_count);

#if defined(SM_ARCH_X86)
    b32 sse = ARE_SSE_INSTRUCTIONS_AVAILABLE( CONTEXT.sysinfo.features );
    if( SM_SIMD_WIDTH == 4 && !sse ) {
        local const usize ERROR_MESSAGE_SIZE = 0xFF;
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

    LOG_NOTE("Memory: %6.3f GB",
        MB_TO_GB(
            KB_TO_MB(
                BYTES_TO_KB(
                    CONTEXT.sysinfo.total_memory
                )
            )
        )
    );
#endif

    if( !event_init() ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if( !input_init( &CONTEXT.platform ) ) {
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
    if(!event_subscribe(
        EVENT_CODE_INPUT_KEY,
        on_f4,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_APP_EXIT,
        on_app_exit,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }

    if( !renderer_init(
        config->main_surface.name,
        config->renderer_backend,
        &CONTEXT.platform
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize rendering subsystem."
        );
        return false;
    }

    CONTEXT.is_running = true;

#if defined(LD_LOGGING) && defined(LD_PROFILING)
    LOG_NOTE("Initial Memory Usage:");
    for( u64 i = 0; i < MEMTYPE_COUNT; ++i ) {
        MemoryType type = (MemoryType)i;
        usize memory_usage = query_memory_usage( type );
        char usage_buffer[32];
        format_bytes(
            memory_usage,
            usage_buffer,
            32
        );
        LOG_NOTE("    %-30s %s", to_string(type), usage_buffer);
    }
    usize heap_usage = query_heap_usage();
    usize page_usage = query_page_usage();
    usize total_memory_usage = heap_usage + page_usage;
    char usage_buffer[32];
    format_bytes(
        total_memory_usage,
        usage_buffer,
        32
    );
    LOG_NOTE("    %-30s %s", "Total Memory Usage", usage_buffer);
#endif

    return true;
}
b32 app_run() {

    while( CONTEXT.is_running ) {
        input_swap();
        platform_poll_gamepad( &CONTEXT.platform );
        platform_pump_events( &CONTEXT.platform );

        if( !CONTEXT.platform.is_active ) {
            continue;
        }

        f64 seconds_elapsed = platform_read_seconds_elapsed(
            &CONTEXT.platform
        );
        CONTEXT.time.delta_time   = seconds_elapsed - CONTEXT.time.elapsed_time;
        CONTEXT.time.elapsed_time = seconds_elapsed;

        if(!CONTEXT.application_run(
            CONTEXT.application_params,
            CONTEXT.time.delta_time
        )) {
            return false;
        }

        RenderOrder draw_order = {};
        draw_order.delta_time = CONTEXT.time.delta_time;

        if( !renderer_draw_frame( &draw_order ) ) {
            MESSAGE_BOX_FATAL(
                "Renderer Failure",
                "Unknown Error!\n"
                "Please contact me at smushybusiness@gmail.com if you see this!"
            );
            return false;
        }

        CONTEXT.time.frame_count++;
    }

    return true;
}
void app_shutdown() {
    event_unsubscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_main_surface_destroy,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_main_surface_active,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_main_surface_resize,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_INPUT_KEY,
        on_f4,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_APP_EXIT,
        on_app_exit,
        nullptr
    );

    CONTEXT.is_running = false;
    renderer_shutdown();
    event_shutdown();
    input_shutdown();

    platform_shutdown( &CONTEXT.platform );
    log_shutdown();
}
