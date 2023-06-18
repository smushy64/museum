#if !defined(CORE_ENGINE_HPP)
#define CORE_ENGINE_HPP
// * Description:  Engine
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "defines.h"

#if !defined(SM_API_INTERNAL)
    typedef u32 RendererBackend;
    typedef u32 CursorStyle;
#else
    #include "renderer/renderer.h"
    #include "platform/platform.h"
#endif
typedef u32 LogLevel;
typedef u32 PlatformFlags;

/// Engine Configuration
struct EngineConfig {
    const char*     application_name;
    struct {
        i32 width;
        i32 height;
    } surface_dimensions;
    LogLevel        log_level;
    RendererBackend renderer_backend;
    PlatformFlags   platform_flags;
    const char*     opt_application_icon_path;
};

/// Application function
typedef b32 (*ApplicationRunFN)(
    struct EngineContext*   engine_ctx,
    struct ThreadWorkQueue* thread_work_queue,
    struct RenderOrder* render_order,
    struct Time* time,
    void* user_params
);

/// Run Engine
SM_API b32 engine_run(
    int argc, char** argv,
    ApplicationRunFN application_run,
    void* application_run_user_params,
    EngineConfig* config
);

/// Set cursor style.
SM_API void engine_set_cursor_style( struct EngineContext* ctx, u32 style );
/// Set cursor visibility.
SM_API void engine_set_cursor_visibility( struct EngineContext* ctx, b32 visible );
/// Center cursor.
SM_API void engine_center_cursor( struct EngineContext* ctx );
/// Set cursor lock mode.
/// Centers cursor every frame and disables visibility.
SM_API void engine_lock_cursor( struct EngineContext* ctx, b32 locked );
/// Query current cursor style.
SM_API u32 engine_query_cursor_style( struct EngineContext* ctx );
/// Query cursor visibility mode.
SM_API b32 engine_query_cursor_visibility( struct EngineContext* ctx );
/// Query cursor lock mode.
SM_API b32 engine_query_cursor_locked( struct EngineContext* ctx );
/// Set application name.
SM_API void engine_set_application_name( struct EngineContext* ctx, const char* name );
/// Query application name.
SM_API const char* engine_query_application_name( struct EngineContext* ctx );
/// Query logical processor count.
SM_API usize engine_query_logical_processor_count( struct EngineContext* ctx );
/// Query total system memory.
SM_API usize engine_query_total_system_memory( struct EngineContext* ctx );
/// Query processor name.
SM_API const char* engine_query_processor_name( struct EngineContext* ctx );
/// Query if SSE instructions are available.
SM_API b32 engine_query_is_sse_available( struct EngineContext* ctx );
/// Query if AVX instructions are available.
SM_API b32 engine_query_is_avx_available( struct EngineContext* ctx );
/// Query if AVX2 instructions are available.
SM_API b32 engine_query_is_avx2_available( struct EngineContext* ctx );
/// Query if AVX-512 instructions are available.
SM_API b32 engine_query_is_avx512_available( struct EngineContext* ctx );

/// Handle to opaque library object.
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

#endif // header guard
