#if !defined(APPLICATION_HPP)
#define APPLICATION_HPP
/**
 * Description:  Application code
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "defines.h"
#include "math/types.h"
#include "platform/flags.h"
#include "renderer/renderer.h"

typedef b32 (*AppRunFn)( void* params, f32 delta_time );

struct AppConfig {
    struct SurfaceConfig {
        const char* name;
        ivec2 dimensions;
    } surface;

    u32 log_level;
    RendererBackend renderer_backend;
    PlatformFlags   platform_flags;

    AppRunFn application_run;
    void*    application_params;
};

SM_API b32  app_init( AppConfig* config );
SM_API b32  app_run();
SM_API void app_shutdown();

#endif