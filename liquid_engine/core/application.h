#if !defined(APPLICATION_HPP)
#define APPLICATION_HPP
/**
 * Description:  Application code
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "defines.h"
#include "smath.h"
#include "platform/flags.h"
#include "renderer/renderer_defines.h"

typedef b32 (*AppRunFn)( void* params, struct Time* time );

struct AppConfig {
    struct SurfaceConfig {
        const char* name;
        ivec2 position;
        ivec2 dimensions;
        SurfaceCreateFlags flags;
    } main_surface;

    u32 log_level;
    RendererBackendType   renderer_backend;
    PlatformInitFlags platform_flags;

    AppRunFn application_run;
    void*    application_params;
};

SM_API b32 app_init( AppConfig* config );
SM_API b32 app_run();
SM_API b32 app_shutdown();

#endif