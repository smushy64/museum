#if !defined(CORE_ENGINE_HPP)
#define CORE_ENGINE_HPP
// * Description:  Engine
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "defines.h"
#include "core/ldlog.h"
#include "core/ldstring.h"
#include "core/ldmath/types.h"
#include "core/ldevent.h"

typedef void EngineContext;

LD_API struct Timer* engine_get_time( EngineContext* ctx );

/// Get memory requirement from application.
/// Function should look like this:
/// usize application_query_memory_requirement();
typedef usize ApplicationQueryMemoryRequirementFN();
/// Application init. Called once before run loop.
/// Function should look like this:
/// b32 application_init( EngineContext* ctx, void* memory );
typedef b32 ApplicationInitFN( EngineContext* ctx, void* memory );
/// Application run. Called once every frame.
/// Function should look like this:
/// b32 application_run( EngineContext* ctx, void* memory );
typedef b32 ApplicationRunFN( EngineContext* ctx, void* memory );

#define DEFAULT_SURFACE_DIMENSIONS (ivec2){ 800, 600 }

#if defined(LD_API_INTERNAL)
    #if defined(DEBUG)
        #define DEFAULT_LOG_LEVEL LOG_LEVEL_ALL_VERBOSE
    #else
        #define DEFAULT_LOG_LEVEL LOG_LEVEL_NONE
    #endif

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
typedef enum CursorStyle : u32 {
    CURSOR_STYLE_ARROW,
    CURSOR_STYLE_RESIZE_VERTICAL,
    CURSOR_STYLE_RESIZE_HORIZONTAL,
    CURSOR_STYLE_RESIZE_TOP_RIGHT_BOTTOM_LEFT,
    CURSOR_STYLE_RESIZE_TOP_LEFT_BOTTOM_RIGHT,
    CURSOR_STYLE_BEAM,
    CURSOR_STYLE_CLICK,
    CURSOR_STYLE_WAIT,
    CURSOR_STYLE_FORBIDDEN,

    CURSOR_STYLE_COUNT
} CursorStyle;
headerfn const char* to_string( CursorStyle cursor_style ) {
    const char* strings[CURSOR_STYLE_COUNT] = {
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
    if( cursor_style >= CURSOR_STYLE_COUNT ) {
        return "Unknown";
    }
    return strings[cursor_style];
}

/// Set cursor style.
LD_API void engine_cursor_set_style( CursorStyle style );
/// Set cursor visibility.
LD_API void engine_cursor_set_visibility( b32 visible );
/// Center cursor.
LD_API void engine_cursor_center();
/// Query current cursor style.
LD_API CursorStyle engine_cursor_style();
/// Query if cursor is visible.
LD_API b32 engine_cursor_visible();
/// Set application name.
/// Name must be a null-terminated string.
LD_API void engine_application_set_name( EngineContext* ctx, StringView name );
/// Query application name.
LD_API StringView engine_application_name( EngineContext* ctx );
/// Query logical processor count.
LD_API usize engine_query_logical_processor_count( EngineContext* ctx );
/// Query total system memory.
LD_API usize engine_query_total_system_memory( EngineContext* ctx );
/// Query processor name.
LD_API const char* engine_query_processor_name( EngineContext* ctx );
/// Centers surface on screen.
/// Does nothing on platforms that don't use windows.
LD_API void engine_surface_center();
/// Set surface dimensions.
LD_API void engine_surface_set_dimensions( ivec2 new_dimensions );
/// Query surface dimensions.
LD_API ivec2 engine_surface_query_dimensions();
/// Send an exit event to the engine.
headerfn void engine_exit() {
    Event event = {};
    event.code  = EVENT_CODE_EXIT;
    event_fire( event );
}

#endif // header guard
