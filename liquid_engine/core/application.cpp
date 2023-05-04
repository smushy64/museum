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

struct AppContext {
    PlatformState platform;
    Surface       main_surface;
    SystemInfo    sysinfo;

    b32 is_running;

    AppRunFn application_run;
    void*    application_params;
};

SM_GLOBAL AppContext CONTEXT = {};

b32 on_main_surface_destroy( Event*, void* ) {
    CONTEXT.is_running = false;
    return true;
}

b32 application_startup( AppConfig* config ) {
    if( !log_init( config->log_level ) ) {
        return false;
    }

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
        return false;
    }

    if( !event_init() ) {
        return false;
    }
    if( !input_init() ) {
        return false;
    }

    if(!event_subscribe(
        INTERNAL_EVENT_CODE_SURFACE_DESTROY,
        on_main_surface_destroy,
        nullptr
    )) {
        return false;
    }
    
    CONTEXT.sysinfo = query_system_info();
    LOG_NOTE("CPU: %s", CONTEXT.sysinfo.cpu_name_buffer);
    LOG_NOTE("  Threads: %llu", CONTEXT.sysinfo.thread_count);

#if defined(SM_ARCH_X86)
    b32 sse  = ARE_SSE_INSTRUCTIONS_AVAILABLE(
        CONTEXT.sysinfo.features
    );
    b32 avx    = IS_AVX_AVAILABLE(
        CONTEXT.sysinfo.features
    );
    b32 avx2   = IS_AVX2_AVAILABLE(
        CONTEXT.sysinfo.features
    );
    b32 avx512 = IS_AVX512_AVAILABLE(
        CONTEXT.sysinfo.features
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
        return false;
    }

    CONTEXT.is_running = true;
    return true;
}
b32 application_run() {

    while( CONTEXT.is_running ) {
        input_swap();
        surface_pump_events( &CONTEXT.main_surface );
        
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
        INTERNAL_EVENT_CODE_SURFACE_DESTROY,
        on_main_surface_destroy,
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

    return success;
}
