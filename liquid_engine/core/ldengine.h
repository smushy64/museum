#if !defined(CORE_ENGINE_HPP)
#define CORE_ENGINE_HPP
// * Description:  Engine
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "defines.h"
#include "core/ldlog.h"
#include "core/ldstring.h"
#include "core/ldmath/types.h"

typedef u32 PlatformFlags;

typedef void EngineContext;

LD_API struct Timer* engine_get_time( EngineContext* engine_ctx );
LD_API struct RenderOrder* engine_get_render_order( EngineContext* engine_ctx );

/// Supported renderer backends
typedef enum : u32 {
    RENDERER_BACKEND_OPENGL,
    RENDERER_BACKEND_VULKAN,
    RENDERER_BACKEND_DX11,
    RENDERER_BACKEND_DX12,

    RENDERER_BACKEND_COUNT
} RendererBackend;
/// Convert renderer backend to const char*
const char* renderer_backend_to_string( RendererBackend backend );
/// Check if renderer backend is supported on the current platform.
b32 renderer_backend_is_supported( RendererBackend backend );

/// Engine Configuration
typedef struct EngineConfig {
    StringView application_name;
    struct {
        i32 width;
        i32 height;
    } surface_dimensions;
    LogLevel        log_level;
    PlatformFlags   platform_flags;
    u32 memory_size;
} EngineConfig;

/// Function for getting engine configuration from application.
typedef void (*ApplicationConfigFN)( struct EngineConfig* config );
/// Application init. Called once before run loop.
typedef b32 (*ApplicationInitFN)( EngineContext* ctx, void* memory );
/// Application run. Called once every frame
typedef b32 (*ApplicationRunFN)( EngineContext* ctx, void* memory );

#define APPLICATION_CONFIG_NAME "application_config"
#define APPLICATION_INIT_NAME   "application_init"
#define APPLICATION_RUN_NAME    "application_run"

#if defined(LD_API_INTERNAL)

#if defined(LD_PLATFORM_WINDOWS)
    #if defined(DEBUG)
        #define DEFAULT_LIBRARY_PATH "testbed-debug.dll"
    #else
        #define DEFAULT_LIBRARY_PATH "testbed-release.dll"
    #endif
#else
    #if defined(DEBUG)
        #define DEFAULT_LIBRARY_PATH "./testbed-debug.so"
    #else
        #define DEFAULT_LIBRARY_PATH "./testbed-release.so"
    #endif
#endif

    /// Engine entry point
    b32 engine_entry( int argc, char** argv );
#endif

/// Supported cursor styles
typedef enum : u32 {
    CURSOR_ARROW,
    CURSOR_RESIZE_VERTICAL,
    CURSOR_RESIZE_HORIZONTAL,
    CURSOR_RESIZE_TOP_RIGHT_BOTTOM_LEFT,
    CURSOR_RESIZE_TOP_LEFT_BOTTOM_RIGHT,
    CURSOR_BEAM,
    CURSOR_CLICK,
    CURSOR_WAIT,
    CURSOR_FORBIDDEN,

    CURSOR_COUNT
} CursorStyle;
inline const char* to_string( CursorStyle cursor_style ) {
    const char* strings[CURSOR_COUNT] = {
        "Arrow",
        "Resize Vertical",
        "Resize Horizontal",
        "Resize Top Right Bottom Left",
        "Resize Top Left Bottom Right",
        "Beam",
        "Click",
        "Wait",
        "Forbidden",
    };
    if( cursor_style >= CURSOR_COUNT ) {
        return "Unknown";
    }
    return strings[cursor_style];
}

/// Set cursor style.
LD_API void engine_set_cursor_style( EngineContext* ctx, u32 style );
/// Set cursor visibility.
LD_API void engine_set_cursor_visibility( EngineContext* ctx, b32 visible );
/// Center cursor.
LD_API void engine_center_cursor( EngineContext* ctx );
/// Set cursor lock mode.
/// Centers cursor every frame and disables visibility.
LD_API void engine_lock_cursor( EngineContext* ctx, b32 locked );
/// Query current cursor style.
LD_API u32 engine_query_cursor_style( EngineContext* ctx );
/// Query cursor visibility mode.
LD_API b32 engine_query_cursor_visibility( EngineContext* ctx );
/// Query cursor lock mode.
LD_API b32 engine_query_cursor_locked( EngineContext* ctx );
/// Set application name. Application name cannot exceed 255 length.
LD_API void engine_set_application_name( EngineContext* ctx, StringView name );
/// Query application name.
LD_API StringView engine_query_application_name( EngineContext* ctx );
/// Query logical processor count.
LD_API usize engine_query_logical_processor_count( EngineContext* ctx );
/// Query total system memory.
LD_API usize engine_query_total_system_memory( EngineContext* ctx );
/// Query processor name.
LD_API const char* engine_query_processor_name( EngineContext* ctx );
/// Query engine surface size
LD_API ivec2 engine_query_surface_size( EngineContext* ctx );

#endif // header guard
