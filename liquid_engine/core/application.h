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

typedef b32 (*AppRunFn)( void* params, f32 delta_time );

struct AppConfig {
    struct SurfaceConfig {
        const char* name;
        ivec2 position;
        ivec2 dimensions;
        SurfaceCreateFlags flags;
    } main_surface;

    u32 log_level;
    PlatformInitFlags platform_flags;

    AppRunFn application_run;
    void*    application_params;
};

SM_API b32 application_startup( AppConfig* config );
SM_API b32 application_run();
SM_API b32 application_shutdown();

#endif