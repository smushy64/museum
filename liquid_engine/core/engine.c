// * Description:  Engine Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "engine.h"
#include "platform/platform.h"
#include "graphics/font.h"
#include "renderer/renderer.h"
#include "threading.h"
#include "event.h"
#include "logging.h"
#include "input.h"
#include "time.h"
#include "memory.h"
#include "string.h"
#include "time.h"
#include "math.h"
#include "audio.h"
#include "ecs.h"
#include "collections.h"
#include "library.h"
#include "asset.h"

#define APPLICATION_NAME_BUFFER_SIZE 255
char APPLICATION_NAME_BUFFER[APPLICATION_NAME_BUFFER_SIZE] = {};

struct EngineContext {
    SystemInfo       system_info;       // 88 
    RenderOrder      render_order;      // 40
    Timer            time;              // 16
    StackArena       arena;             // 16
    Platform*        platform;          // 8
    RendererContext* renderer_context;  // 8
    EntityStorage*   entity_storage;    // 8
    
    ApplicationConfigFN application_config; // 8
    ApplicationInitFN   application_init;   // 8
    ApplicationRunFN    application_run;    // 8

    StringView application_name_view;
    StringView application_name_writable_view;

    RendererBackend renderer_backend; // 4
    
    u32 application_title_buffer_writable_offset; // 4
    
    CursorStyle cursor_style;

    b8 cursor_is_visible;         // 1
    b8 cursor_is_locked;          // 1
    b8 is_running;                // 1
    b8 pause_on_surface_inactive; // 1
};

EventCallbackReturn on_app_exit( Event*, void* void_ctx ) {
    EngineContext* ctx = (EngineContext*)void_ctx;
    ctx->is_running    = false;
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackReturn on_active( Event* event, void* ) {
    b32 is_active = event->data.bool32[0];
    if( is_active ) {
        LOG_NOTE("Surface activated.");
    } else {
        LOG_NOTE("Surface deactivated.");
    }
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackReturn on_resize( Event* event, void* void_ctx ) {
    EngineContext* ctx = (EngineContext*)void_ctx;
    i32 width  = event->data.int32[0];
    i32 height = event->data.int32[1];
    renderer_on_resize( ctx->renderer_context, width, height );
    return EVENT_CALLBACK_NOT_CONSUMED;
}

struct ArgParseResult {
    b32 success;
    RendererBackend backend;
    char library_path_buffer[32];
    StringView library_path;
    b32 quit_instant;
};

internal void printhelp() {
    println( "Usage: {cc} [options]", LIQUID_ENGINE_EXECUTABLE );
    println("  --output-debug-string  "
        "enable output debug string (windows only)"
    );
    println("  --gl                   "
        "use OpenGL renderer (default)"
    );
    println("  --vk                   "
        "use Vulkan renderer"
    );
    println("  --dx11                 "
        "use Direct3D11 renderer (windows only)"
    );
    println("  --dx12                 "
        "use Direct3D12 renderer (windows only)"
    );
    println("  --libload=[path]       "
        "define path to game dll/so (default = "
        DEFAULT_LIBRARY_PATH ")"
    );
    println("  --help or -h           "
        "print help"
    );

}

ArgParseResult parse_args( int argc, char** argv ) {
    ArgParseResult result = {};

    
    const char* default_library_path = DEFAULT_LIBRARY_PATH;
    result.library_path.buffer = result.library_path_buffer;
    result.library_path.len    = str_length( default_library_path );
    mem_copy(
        result.library_path.buffer,
        default_library_path,
        result.library_path.len + 1
    );

    result.backend = RENDERER_BACKEND_OPENGL;
    result.success = true;

    for( i32 i = 1; i < argc; ++i ) {
        StringView current_arg = argv[i];

        if(
            string_cmp( current_arg, "--help" ) ||
            string_cmp( current_arg, "-h" )
        ) {
            printhelp();
            result.quit_instant = true;
            return result;
        }

#if defined(LD_PLATFORM_WINDOWS)
        if( string_cmp( current_arg, "--output-debug-string" ) ) {
            log_enable_output_debug_string( true );
            continue;
        }
#endif
        if( string_cmp( current_arg, "--gl" ) ) {
            result.backend = RENDERER_BACKEND_OPENGL;
        } else if( string_cmp( current_arg, "--vk" ) ) {
            result.backend = RENDERER_BACKEND_VULKAN;
        } else if( string_cmp( current_arg, "--dx11" ) ) {
#if defined(LD_PLATFORM_WINDOWS)
            result.backend = RENDERER_BACKEND_DX11;
#else
            printlnerr( "DirectX11 is not available on non-windows platforms!" );
            printhelp();
            result.quit_instant = true;
            return result;
#endif
        } else if( string_cmp( current_arg, "--dx12" ) ) {
#if defined(LD_PLATFORM_WINDOWS)
            result.backend = RENDERER_BACKEND_DX12;
#else
            printlnerr( "DirectX12 is not available on non-windows platforms!" );
            printhelp();
            result.quit_instant = true;
            return result;
#endif
        } else if( string_contains( current_arg, "--libload=" ) ) {
            u32 load_string_len = str_length( "--libload=" );
            result.library_path.buffer = &current_arg.buffer[load_string_len];
            result.library_path.len    = str_length(
                result.library_path.buffer
            );
        } else {
            printlnerr( "Unrecognized argument: {sv}", current_arg );
            printhelp();
            result.success      = false;
            result.quit_instant = true;
        }
    }

    return result;
}

b32 engine_entry( int argc, char** argv ) {

    EngineContext ctx = {};

    ArgParseResult arg_parse = parse_args( argc, argv );
    if( !arg_parse.success ) {
        return false;
    }

    if( arg_parse.quit_instant ) {
        return true;
    }

    LibraryHandle application_lib = {};

    if( !library_load( arg_parse.library_path.buffer, &application_lib ) ) {
        return false;
    }
    ctx.application_config = (ApplicationConfigFN)library_load_function(
        &application_lib,
        APPLICATION_CONFIG_NAME
    );
    if( !ctx.application_config ) {
        return false;
    }
    ctx.application_init = (ApplicationInitFN)library_load_function(
        &application_lib,
        APPLICATION_INIT_NAME
    );
    if( !ctx.application_init ) {
        return false;
    }
    ctx.application_run = (ApplicationRunFN)library_load_function(
        &application_lib,
        APPLICATION_RUN_NAME
    );
    if( !ctx.application_run ) {
        return false;
    }

    EngineConfig config = {};
    config.application_name.buffer = APPLICATION_NAME_BUFFER;
    config.application_name.len    = APPLICATION_NAME_BUFFER_SIZE;
    ctx.application_config( &config );

    ctx.renderer_backend = arg_parse.backend;

    ctx.system_info   = query_system_info();

    u32 threading_subsystem_size = query_threading_subsystem_size();
    u32 event_subsystem_size     = query_event_subsystem_size();
    u32 input_subsystem_size     = query_input_subsystem_size();
    u32 platform_subsystem_size  = query_platform_subsystem_size();
    u32 renderer_subsystem_size  = query_renderer_subsystem_size( ctx.renderer_backend );
    
    u32 logging_subsystem_size = DEFAULT_LOGGING_BUFFER_SIZE;

    u32 application_memory_size = config.memory_size;

    #define STACK_ARENA_SAFETY_BYTES 16
    // calculate required stack arena size
    u32 required_stack_arena_size =
        threading_subsystem_size +
        event_subsystem_size +
        input_subsystem_size +
        platform_subsystem_size +
        renderer_subsystem_size +
        logging_subsystem_size +
        sizeof( EntityStorage ) +
        STACK_ARENA_SAFETY_BYTES +
        application_memory_size;

    if( !stack_arena_create(
        required_stack_arena_size,
        MEMTYPE_ENGINE,
        &ctx.arena
    ) ) {
        printlnerr(
            "Subsystem Failure",
            "Failed to create stack arena! Requested size: {u}",
            required_stack_arena_size
        );
        return false;
    }

    ctx.entity_storage = stack_arena_push( ctx.arena, EntityStorage );

#if defined(LD_LOGGING)

    if( !is_log_initialized() ) {
        StringView logging_buffer = {};
        logging_buffer.len = logging_subsystem_size;
        logging_buffer.buffer =
            (char*)stack_arena_push_item( &ctx.arena, logging_subsystem_size );
        if( !log_init( config.log_level, logging_buffer ) ) {
            MESSAGE_BOX_FATAL(
                "Subsystem Failure",
                "Failed to initialize logging subsystem!\n "
                LD_CONTACT_MESSAGE
            );
            return false;
        }
    }

#endif

    LOG_INFO("Liquid Engine Version: {i}.{i}",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    ctx.application_name_view.len    = APPLICATION_NAME_BUFFER_SIZE;
    ctx.application_name_view.buffer = APPLICATION_NAME_BUFFER;

    void* event_subsystem_data = stack_arena_push_item(
        &ctx.arena,
        event_subsystem_size
    );

    if( !event_init( event_subsystem_data ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    ctx.platform = (Platform*)stack_arena_push_item(
        &ctx.arena,
        platform_subsystem_size
    );

    void* input_subsystem_buffer = stack_arena_push_item(
        &ctx.arena,
        input_subsystem_size
    );
    if( !input_init( ctx.platform, input_subsystem_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize input subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    if( !platform_init(
        { config.surface_dimensions.width, config.surface_dimensions.height },
        config.platform_flags,
        ctx.platform
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    engine_set_application_name( &ctx, config.application_name );

    ctx.pause_on_surface_inactive = CHECK_BITS(
        config.platform_flags,
        PLATFORM_PAUSE_ON_SURFACE_INACTIVE
    );
    RendererContext* renderer_ctx_buffer = (RendererContext*)stack_arena_push_item(
        &ctx.arena, renderer_subsystem_size
    );
    LOG_ASSERT(
        renderer_ctx_buffer,
        "Stack Arena of size {u} is not enough to initialize engine!",
        ctx.arena.arena_size
    );

    ctx.renderer_context = renderer_ctx_buffer;

    if( !renderer_init(
        config.application_name,
        ctx.renderer_backend,
        ctx.platform,
        renderer_subsystem_size,
        ctx.renderer_context
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize rendering subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* threading_buffer = stack_arena_push_item(
        &ctx.arena, threading_subsystem_size
    );
    LOG_ASSERT(
        threading_buffer,
        "Stack Arena of size {u} is not enough to initialize engine!",
        ctx.arena.arena_size
    );

    u32 thread_count  = ctx.system_info.logical_processor_count;
    thread_count = (thread_count == 1 ? thread_count : thread_count - 1);

    if( !threading_init( thread_count, threading_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize threading subsystem!\n"
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    LOG_NOTE("CPU: {cc}", ctx.system_info.cpu_name_buffer);
    LOG_NOTE("  Logical Processors: {u64}",
        ctx.system_info.logical_processor_count
    );

#if defined(LD_ARCH_X86)
    b32 sse = CHECK_BITS(
        ctx.system_info.features,
        SSE_MASK | SSE2_MASK | SSE3_MASK |
        SSSE3_MASK | SSE4_1_MASK | SSE4_2_MASK
    );
    b32 avx    = CHECK_BITS( ctx.system_info.features, AVX_MASK );
    b32 avx2   = CHECK_BITS( ctx.system_info.features, AVX2_MASK );
    b32 avx512 = CHECK_BITS( ctx.system_info.features, AVX512_MASK );
    ProcessorFeatures features = ctx.system_info.features;
    if( LD_SIMD_WIDTH == 4 && !sse ) {
        #define ERROR_MESSAGE_SIZE 256
        char error_message_buffer[ERROR_MESSAGE_SIZE];
        StringView error_message_buffer_view = {};
        error_message_buffer_view.buffer = error_message_buffer;
        error_message_buffer_view.len    = ERROR_MESSAGE_SIZE;
        str_buffer_fill( ERROR_MESSAGE_SIZE, error_message_buffer, ' ' );
        string_format(
            error_message_buffer_view,
            "Your CPU does not support SSE instructions!\n"
            "Missing instructions: {cc}{cc}{cc}{cc}{cc}{cc}",
            CHECK_BITS(features, SSE_MASK)    ? "" : "SSE, ",
            CHECK_BITS(features, SSE2_MASK)   ? "" : "SSE2, ",
            CHECK_BITS(features, SSE3_MASK)   ? "" : "SSE3, ",
            CHECK_BITS(features, SSSE3_MASK)  ? "" : "SSSE3, ",
            CHECK_BITS(features, SSE4_1_MASK) ? "" : "SSE4.1, ",
            CHECK_BITS(features, SSE4_2_MASK) ? "" : "SSE4.2"
        );
        MESSAGE_BOX_FATAL( "Missing instructions.", error_message_buffer );
        return false;
    }

    if( LD_SIMD_WIDTH == 8 && !(avx && avx2) ) {
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            "Your CPU does not support AVX/AVX2 instructions! "
            "This program requires them!"
        );
        return false;
    }


    LOG_NOTE(
        "  Features: {cc}{cc}{cc}{cc}",
        sse    ? "SSE1-4 " : "",
        avx    ? "AVX " : "",
        avx2   ? "AVX2 " : "",
        avx512 ? "AVX-512 " : ""
    );

#endif

    if(!event_subscribe(
        EVENT_CODE_EXIT,
        on_app_exit,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_ACTIVE,
        on_active,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_RESIZE,
        on_resize,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

#if defined(LD_LOGGING) && defined(LD_PROFILING)
    LOG_NOTE("System Memory: {f,b,02.3}", (f64)ctx.system_info.total_memory );
    LOG_NOTE("Initial Memory Usage:");

    f64 total_memory_usage = 0.0;
    for( u64 i = 0; i < MEMTYPE_COUNT; ++i ) {
        MemoryType type = (MemoryType)i;
        f64 memory_usage_f64 = (f64)query_memory_usage( type );
        LOG_NOTE( "    {cc,-25} {f,b,4.2}", to_string(type), memory_usage_f64 );
        total_memory_usage += memory_usage_f64;
    }
    LOG_NOTE("    {cc,-25} {f,b,4.2}", "Total Memory Usage", total_memory_usage);
    LOG_NOTE("Engine stack arena pointer: {u}", ctx.arena.stack_pointer);
#endif

    ctx.cursor_style      = CURSOR_ARROW;
    ctx.cursor_is_visible = true;

    #define UPDATE_FRAME_RATE_COUNTER_RATE 100

    // if( !audio_init( ctx.platform ) ) {
    //     return false;
    // }

    void* application_memory = stack_arena_push_item(
        &ctx.arena, config.memory_size
    );
    if( !ctx.application_init( &ctx, application_memory ) ) {
        return false;
    }

    ctx.is_running = true;
    while( ctx.is_running ) {
        input_swap();
        platform_poll_gamepad( ctx.platform );
        platform_pump_events( ctx.platform );

        if(
            !ctx.platform->is_active &&
            ctx.pause_on_surface_inactive
        ) {
            continue;
        }

        if( input_is_key_down( KEY_ALT_LEFT ) ||
            input_is_key_down( KEY_ALT_RIGHT )
        ) {
            if( input_is_key_down( KEY_F4 ) ) {
                Event event = {};
                event.code  = EVENT_CODE_EXIT;
                event_fire( event );
            }
        }

        if( ctx.cursor_is_locked ) {
            platform_cursor_center( ctx.platform );
        }

        ctx.render_order = {};
        ctx.render_order.time = &ctx.time;
#if defined(DEBUG)
        ctx.render_order.list_debug_points =
            (DebugPoints*)::impl::_list_create(
                LIST_DEFAULT_CAPACITY, sizeof(DebugPoints)
            );
#endif
        if( !ctx.application_run( &ctx, application_memory ) ) {
            return false;
        }
        
        if( !renderer_draw_frame(
            ctx.renderer_context,
            &ctx.render_order
        ) ) {
            MESSAGE_BOX_FATAL(
                "Renderer Failure",
                "Unknown Error!\n"
                LD_CONTACT_MESSAGE
            );
            return false;
        }

#if defined(DEBUG)
        ::impl::_list_free( ctx.render_order.list_debug_points );
#endif

        // audio_test( ctx.platform );

        ctx.time.frame_count++;

        f64 seconds_elapsed = platform_s_elapsed();
        ctx.time.delta_seconds =
            seconds_elapsed - ctx.time.elapsed_seconds;
        ctx.time.elapsed_seconds = seconds_elapsed;

    }

    ctx.is_running = false; 

    library_free( &application_lib );

    // audio_shutdown( ctx.platform );

    event_shutdown();
    input_shutdown();
    renderer_shutdown( ctx.renderer_context );
    platform_shutdown( ctx.platform );
    threading_shutdown();
    log_shutdown();

    return true;
}
void engine_set_cursor_style( struct EngineContext* ctx, u32 style ) {
    ctx->cursor_style = (CursorStyle)style;
    platform_cursor_set_style(
        ctx->platform,
        (CursorStyle)style
    );
}
void engine_set_cursor_visibility( struct EngineContext* ctx, b32 visible ) {
    ctx->cursor_is_visible = visible;
    platform_cursor_set_visible(
        ctx->platform,
        visible
    );
}
void engine_center_cursor( struct EngineContext* ctx ) {
    platform_cursor_center( ctx->platform );
}
void engine_lock_cursor( struct EngineContext* ctx, b32 locked ) {
    ctx->cursor_is_locked = locked;
    if( locked ) {
        ctx->cursor_is_visible = false;
    }
}
u32 engine_query_cursor_style( struct EngineContext* ctx ) {
    return (u32)ctx->cursor_style;
}
b32 engine_query_cursor_visibility( struct EngineContext* ctx ) {
    return ctx->cursor_is_visible;
}
b32 engine_query_cursor_locked( struct EngineContext* ctx ) {
    return ctx->cursor_is_locked;
}
void engine_set_application_name( struct EngineContext* ctx, StringView name ) {
    StringView renderer_backend_name = to_string( ctx->renderer_backend );

    string_format(
        ctx->application_name_view,
        "{sv} | {sv}",
        name, renderer_backend_name
    );

    platform_surface_set_name(
        ctx->platform,
        ctx->application_name_view
    );
}
StringView engine_query_application_name( struct EngineContext* ctx ) {
    return ctx->application_name_view;
}
usize engine_query_logical_processor_count( struct EngineContext* ctx ) {
    return ctx->system_info.logical_processor_count;
}
usize engine_query_total_system_memory( struct EngineContext* ctx ) {
    return ctx->system_info.total_memory;
}
const char* engine_query_processor_name( struct EngineContext* ctx ) {
    return ctx->system_info.cpu_name_buffer;
}
ivec2 engine_query_surface_size( struct EngineContext* ctx ) {
    return ctx->platform->surface.dimensions;
}

LD_API struct EntityStorage* engine_get_entity_storage(
    struct EngineContext* engine_ctx
) {
    return engine_ctx->entity_storage;
}

LD_API struct Timer* engine_get_time( struct EngineContext* engine_ctx ) {
    return &engine_ctx->time;
}

LD_API struct RenderOrder* engine_get_render_order(
    struct EngineContext* engine_ctx
) {
    return &engine_ctx->render_order;
}

