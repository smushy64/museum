/**
 * Description:  Application Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "application.h"
#include "platform/platform.h"
#include "logging.h"

struct AppContext {
    PlatformState platform;
    Surface       main_surface;
    SystemInfo    sysinfo;

    b32 is_running;

    AppRunFn application_run;
    void*    application_params;
};

SM_GLOBAL AppContext context = {};

b32 application_startup( AppConfig* config ) {
    if( !log_init( config->log_level ) ) {
        return false;
    }

    SM_ASSERT(config->application_run);
    context.application_run    = config->application_run;
    context.application_params = config->application_params;

    LOG_NOTE("Liquid Engine Version: %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    if( !platform_init(
        config->platform_flags,
        &context.platform
    ) ) {
        return false;
    }
    
    context.sysinfo = query_system_info();
    LOG_NOTE("CPU: %s", context.sysinfo.cpu_name_buffer);
    LOG_NOTE("  Threads: %llu", context.sysinfo.thread_count);

#if defined(SM_ARCH_X86)
    b32 sse  = ARE_SSE_INSTRUCTIONS_AVAILABLE(
        context.sysinfo.features
    );
    b32 avx    = IS_AVX_AVAILABLE(
        context.sysinfo.features
    );
    b32 avx2   = IS_AVX2_AVAILABLE(
        context.sysinfo.features
    );
    b32 avx512 = IS_AVX512_AVAILABLE(
        context.sysinfo.features
    );

    LOG_NOTE(
        "  Features: %s%s%s%s",
        sse ? "SSE1-4 " : "",
        avx ? "AVX " : "",
        avx2 ? "AVX2 " : "",
        avx512 ? "AVX-512 " : ""
    );
#endif

    LOG_NOTE("Memory: %6.3 GB",
        MB_TO_GB(
            KB_TO_MB(
                BYTES_TO_KB(
                    context.sysinfo.total_memory
                )
            )
        )
    );

    if(!surface_create(
        config->main_surface.name,
        config->main_surface.position,
        config->main_surface.dimensions,
        config->main_surface.flags,
        &context.platform,
        nullptr,
        &context.main_surface
    )) {
        return false;
    }

    context.is_running = true;
    return true;
}
b32 application_run() {

    while( context.is_running ) {
        surface_pump_events(
            &context.main_surface
        );
        Event event = {};
        while( platform_next_event(
            &context.platform,
            &event
        ) ) {
            if( event.type == EVENT_TYPE_SURFACE_DESTROY ) {
                context.is_running = false;
            }
        }
        if(!context.application_run(
            context.application_params,
            0.0f
        )) {
            return false;
        }
    }

    return true;
}
b32 application_shutdown() {
    context.is_running = false;
    surface_destroy( &context.main_surface );
    platform_shutdown( &context.platform );
    return true;
}
