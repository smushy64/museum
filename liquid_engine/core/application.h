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

enum GraphicsBackend : u32 {
    BACKEND_VULKAN,
    BACKEND_OPENGL,
    BACKEND_DX11,
    BACKEND_DX12,

    BACKEND_COUNT
};
inline const char* to_string( GraphicsBackend backend ) {
    SM_LOCAL const char* strings[BACKEND_COUNT] = {
        "Vulkan 1.2",
        "OpenGL 4.5",
        "DirectX 11",
        "DirectX 12"
    };
    SM_ASSERT( backend < BACKEND_COUNT );
    return strings[backend];
}

struct AppConfig {
    struct SurfaceConfig {
        const char* name;
        ivec2 position;
        ivec2 dimensions;
        SurfaceCreateFlags flags;
    } main_surface;

    u32 log_level;
    GraphicsBackend graphics_backend;
    PlatformInitFlags platform_flags;

    AppRunFn application_run;
    void*    application_params;
};

SM_API b32 application_startup( AppConfig* config );
SM_API b32 application_run();
SM_API b32 application_shutdown();

#endif