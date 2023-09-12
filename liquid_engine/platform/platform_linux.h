#if !defined(LD_PLATFORM_LINUX_H)
#define LD_PLATFORM_LINUX_H
// * Description:  Linux Platform
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 04, 2023
#include "defines.h"

#if defined(LD_PLATFORM_LINUX) && defined(LD_API_INTERNAL)
#include "ldplatform.h"
#include "core/ldlog.h"
#include <SDL2/SDL.h>

typedef struct LinuxSurface {
    SDL_Window* handle;

    ivec2 dimensions;

    PlatformSurfaceOnResizeFN* on_resize;
    void* on_resize_user_params;
    PlatformSurfaceOnActivateFN*  on_activate;
    void* on_activate_user_params;
    PlatformSurfaceOnCloseFN*  on_close;
    void* on_close_user_params;

    PlatformSurfaceCreateFlags creation_flags;
    PlatformSurfaceMode mode;
    b8 is_active;

    enum RendererBackend backend;
    union {
        SDL_GLContext glrc;
    };
} LinuxSurface;

#define LINUX_LIB_COUNT (1)
typedef struct LinuxPlatform {
    CursorStyle cursor_style;
    b32         cursor_visible;

    union {
        struct {
            void* libgl;
        };
        void* libs[LINUX_LIB_COUNT];
    };
} LinuxPlatform;

KeyboardCode x_key_to_keycode( u32 x_key );
KeyboardCode sdl_key_to_keycode( SDL_Keysym key );

#if defined(LD_LOGGING)
    #define LINUX_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[LINUX NOTE] " format,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            false, true,\
            LOG_COLOR_WHITE\
            "[LINUX INFO] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            false, true,\
            LOG_COLOR_BLUE\
            "[LINUX DEBUG] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[LINUX WARN] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            false, true,\
            LOG_COLOR_RED\
            "[LINUX ERROR] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[LINUX NOTE | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_WHITE\
            "[LINUX INFO | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_BLUE\
            "[LINUX DEBUG | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define LINUX_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[LINUX WARN | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_RED\
            "[LINUX ERROR | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
#else
    #define LINUX_LOG_NOTE( format, ... ) unused(format)
    #define LINUX_LOG_INFO( format, ... ) unused(format)
    #define LINUX_LOG_DEBUG( format, ... ) unused(format)
    #define LINUX_LOG_WARN( format, ... ) unused(format)
    #define LINUX_LOG_ERROR( format, ... ) unused(format)

    #define LINUX_LOG_NOTE_TRACE( format, ... ) unused(format)
    #define LINUX_LOG_INFO_TRACE( format, ... ) unused(format)
    #define LINUX_LOG_DEBUG_TRACE( format, ... ) unused(format)
    #define LINUX_LOG_WARN_TRACE( format, ... ) unused(format)
    #define LINUX_LOG_ERROR_TRACE( format, ... ) unused(format)

#endif

#define LINUX_LOG_FATAL( format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        true, true,\
        LOG_COLOR_RED\
        "[LINUX FATAL | {cc}() | {cc}:{i}] " format\
        LOG_COLOR_RESET,\
        __FUNCTION__,\
        __FILE__,\
        __LINE__,\
        ##__VA_ARGS__\
    )



#endif // linux platform
#endif // header guard

