// * Description:  Testbed Entry
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <core/ldengine.h>
#include <core/ldlog.h>

extern "C" void application_config( struct EngineConfig* config ) {
    config->application_name.len = sv_format(
        config->application_name,
        "Foo"
    );
    config->surface_dimensions = { 800, 600 };
    config->platform_flags     = (1 << 0) | (1 << 1) | (1 << 2);
    config->memory_size        = 1;
    config->log_level          = LOG_LEVEL_ALL_VERBOSE;
}

extern "C" b32 application_init( EngineContext* ctx, void* memory ) {
    unused(ctx);
    unused(memory);
    return true;
}

extern "C" b32 application_run( EngineContext* ctx, void* memory ) {
    unused(ctx);
    unused(memory);
    return true;
}

