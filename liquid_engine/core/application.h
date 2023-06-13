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

    const char* opt_surface_icon_path;
};

SM_API b32  app_init( AppConfig* config );
SM_API b32  app_run();
SM_API void app_shutdown();

#if !defined(CURSOR_STYLES_DEFINED)
#define CURSOR_STYLES_DEFINED

/// Supported cursor styles
enum CursorStyle : u32 {
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
};

#endif // cursor styles

/// Set cursor style.
SM_API void cursor_set_style( CursorStyle style );
/// Set cursor visibility
SM_API void cursor_set_visibility( b32 visible );
/// Lock cursor.
/// Centers cursor every frame and disables visibility.
SM_API void cursor_set_locked( b32 locked );
/// Center cursor to screen.
SM_API void cursor_center();
/// Query current cursor style.
SM_API CursorStyle cursor_query_style();
/// Query cursor visibility.
SM_API b32 cursor_query_visibility();
/// Query if cursor is locked.
SM_API b32 cursor_query_locked();
/// Set surface name.
SM_API void surface_set_name( const char* name );

/// Handle to library object.
typedef void* LibraryHandle;

namespace impl {
    /// Load local library or from environment path.
    SM_API b32 _library_load(
        const char* library_name, 
        LibraryHandle* out_library
    );
    /// Load local library or from environment path.
    SM_API b32 _library_load_trace(
        const char* library_name, 
        LibraryHandle* out_library,
        const char* function,
        const char* file,
        i32 line
    );
    /// Free a library handle.
    SM_API void _library_free( LibraryHandle library );
    /// Free a library handle.
    SM_API void _library_free_trace( 
        LibraryHandle library,
        const char* function,
        const char* file,
        i32 line
    );
    /// Load a function from a library.
    SM_API void* _library_load_function(
        LibraryHandle library,
        const char* function_name
    );
    /// Load a function from a library.
    SM_API void* _library_load_function_trace(
        LibraryHandle library,
        const char* function_name,
        const char* function,
        const char* file,
        i32 line
    );
} // implementation

#if defined(LD_LOGGING)
    #define library_load( library_name, out_library )\
        ::impl::_library_load_trace(\
            library_name,\
            out_library,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define library_free( library )\
        ::impl::_library_free_trace(\
            library,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define library_load_function( library, function_name )\
        ::impl::_library_load_function_trace(\
            library,\
            function_name,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define library_load( library_name, out_library )\
        ::impl::_library_load( library_name, out_library )
    #define library_free( library )\
        ::impl::_library_free( library )
    #define library_load_function( library, function_name )\
        ::impl::_library_load_function( library, function_name )
#endif

typedef u16 ProcessorFeatures;
#define SSE_MASK    (1 << 0)
#define SSE2_MASK   (1 << 1)
#define SSE3_MASK   (1 << 2)
#define SSSE3_MASK  (1 << 3)
#define SSE4_1_MASK (1 << 4)
#define SSE4_2_MASK (1 << 5)
#define AVX_MASK    (1 << 6)
#define AVX2_MASK   (1 << 7)
#define AVX512_MASK (1 << 8)

#if defined(SM_ARCH_X86)
    #define CPU_NAME_BUFFER_LEN 68
#else
    #error "Architecture is not yet supported!"
#endif

/// System information
struct SystemInfo {
    usize logical_processor_count;
    usize total_memory;
    char  cpu_name_buffer[CPU_NAME_BUFFER_LEN];
    ProcessorFeatures features;
};
/// Query CPU and memory information.
SM_API SystemInfo query_system_info();
/// Check if SSE instructions are available.
SM_API b32 system_is_sse_available( ProcessorFeatures features );
/// Check if AVX instructions are available.
SM_API b32 system_is_avx_available( ProcessorFeatures features );
/// Check if AVX2 instructions are available.
SM_API b32 system_is_avx2_available( ProcessorFeatures features );
/// Check if AVX-512 instructions are available.
SM_API b32 system_is_avx512_available( ProcessorFeatures features );

#endif