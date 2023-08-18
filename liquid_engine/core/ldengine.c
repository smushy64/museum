// * Description:  Engine Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "core/ldengine.h"
#include "ldplatform.h"
#include "ldrenderer.h"
#include "core/ldthread.h"
#include "core/ldevent.h"
#include "core/ldlog.h"
#include "core/ldinput.h"
#include "core/ldtime.h"
#include "core/ldmemory.h"
#include "core/ldallocator.h"
#include "core/ldstring.h"
#include "core/ldmath.h"
#include "core/ldcollections.h"
#include "core/ldlibrary.h"
#include "core/ldgraphics.h"

#define APPLICATION_NAME_BUFFER_SIZE 255
char APPLICATION_NAME_BUFFER[APPLICATION_NAME_BUFFER_SIZE] = {};

typedef struct InternalEngineContext {
    SystemInfo       system_info;       // 88
    StackAllocator   stack;             // 32
    Timer            time;              // 16
    Platform*        platform;          // 8
    
    ApplicationConfigFN application_config; // 8
    ApplicationInitFN   application_init;   // 8
    ApplicationRunFN    application_run;    // 8

    StringView application_name_view;
    StringView application_name_writable_view;

    u32 application_title_buffer_writable_offset; // 4
    
    CursorStyle cursor_style;

    b8 cursor_is_visible;         // 1
    b8 cursor_is_locked;          // 1
    b8 is_running;                // 1
    b8 pause_on_surface_inactive; // 1
} InternalEngineContext;

EventCallbackResult on_app_exit( Event* event, void* void_ctx ) {
    unused(event);
    InternalEngineContext* ctx = void_ctx;
    ctx->is_running = false;
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackResult on_active( Event* event, void* params ) {
    unused(params);
    b32 is_active = event->data.bool32[0];
    if( is_active ) {
        LOG_NOTE("Surface activated.");
    } else {
        LOG_NOTE("Surface deactivated.");
    }
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackResult on_resize( Event* event, void* params ) {
    unused(params);
    ivec2 dimensions = event->data.iv2[0];
    renderer_subsystem_on_resize( dimensions );
    return EVENT_CALLBACK_NOT_CONSUMED;
}

typedef struct ArgParseResult {
    b32 success;
    RendererBackend backend;
    char library_path_buffer[32];
    StringView library_path;
    b32 quit_instant;
} ArgParseResult;

internal void print_help() {
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
        StringView current_arg = SV( argv[i] );

        if(
            sv_cmp( current_arg, SV( "--help" ) ) ||
            sv_cmp( current_arg, SV( "-h" ) )
        ) {
            print_help();
            result.quit_instant = true;
            return result;
        }

#if defined(LD_PLATFORM_WINDOWS)
        if( sv_cmp( current_arg, SV( "--output-debug-string" ) ) ) {
            log_enable_output_debug_string( true );
            continue;
        }
#endif
        if( sv_cmp( current_arg, SV( "--gl" ) ) ) {
            result.backend = RENDERER_BACKEND_OPENGL;
        } else if( sv_cmp( current_arg, SV( "--vk" ) ) ) {
            result.backend = RENDERER_BACKEND_VULKAN;
        } else if( sv_cmp( current_arg, SV( "--dx11" ) ) ) {
#if defined(LD_PLATFORM_WINDOWS)
            result.backend = RENDERER_BACKEND_DX11;
#else
            println_err( "DirectX11 is not available on non-windows platforms!" );
            print_help();
            result.quit_instant = true;
            return result;
#endif
        } else if( sv_cmp( current_arg, SV( "--dx12" ) ) ) {
#if defined(LD_PLATFORM_WINDOWS)
            result.backend = RENDERER_BACKEND_DX12;
#else
            println_err( "DirectX12 is not available on non-windows platforms!" );
            print_help();
            result.quit_instant = true;
            return result;
#endif
        } else if( sv_contains( current_arg, SV( "--libload=" ) ) ) {
            u32 load_string_len = str_length( "--libload=" );
            result.library_path.buffer = &current_arg.buffer[load_string_len];
            result.library_path.len    = str_length(
                result.library_path.buffer
            );
        } else {
            println_err( "Unrecognized argument: {sv}", current_arg );
            print_help();
            result.success      = false;
            result.quit_instant = true;
        }
    }

    return result;
}

b32 engine_entry( int argc, char** argv ) {

    InternalEngineContext ctx = {};

    ArgParseResult arg_parse = parse_args( argc, argv );
    if( !arg_parse.success ) {
        return false;
    }

    if( arg_parse.quit_instant ) {
        return true;
    }

    DynamicLibrary application_lib = {};

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

    RendererBackend backend = arg_parse.backend;

    ctx.system_info   = query_system_info();

    u32 threading_subsystem_size = query_threading_subsystem_size();
    u32 event_subsystem_size     = query_event_subsystem_size();
    u32 input_subsystem_size     = query_input_subsystem_size();
    u32 platform_subsystem_size  = query_platform_subsystem_size();
    usize renderer_subsystem_size =
        renderer_subsystem_context_size( arg_parse.backend );
    
    u32 logging_subsystem_size = DEFAULT_LOGGING_BUFFER_SIZE;

    u32 application_memory_size = config.memory_size;

    // calculate required stack arena size
    usize required_stack_size =
        threading_subsystem_size +
        event_subsystem_size     +
        input_subsystem_size     +
        platform_subsystem_size  +
        renderer_subsystem_size  +
        logging_subsystem_size   +
        application_memory_size;

    usize stack_allocator_pages  = calculate_page_size( required_stack_size );
    void* stack_allocator_buffer =
        ldpage_alloc( stack_allocator_pages, MEMORY_TYPE_ENGINE );
    if( !stack_allocator_buffer ) {
        println_err(
            "Subsystem Failure"
            "Failed to allocate stack allocator buffer!"
        );
        return false;
    }

    ctx.stack = stack_allocator_from_buffer(
        required_stack_size, stack_allocator_buffer, MEMORY_TYPE_ENGINE
    );

#if defined(LD_LOGGING)

    if( !is_log_initialized() ) {
        StringView logging_buffer = {};
        logging_buffer.len = logging_subsystem_size;
        logging_buffer.buffer =
            (char*)stack_allocator_push( &ctx.stack, logging_subsystem_size );
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

    void* event_subsystem_data = stack_allocator_push(
        &ctx.stack,
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

    ctx.platform = (Platform*)stack_allocator_push(
        &ctx.stack,
        platform_subsystem_size
    );

    void* input_subsystem_buffer = stack_allocator_push(
        &ctx.stack,
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
        iv2( config.surface_dimensions.width, config.surface_dimensions.height ),
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
    RendererContext* renderer_ctx_buffer = stack_allocator_push(
        &ctx.stack, renderer_subsystem_size
    );
    LOG_ASSERT(
        renderer_ctx_buffer,
        "Stack Arena of size {u} is not enough to initialize engine!",
        ctx.stack.size
    );

    if( !renderer_subystem_init(
        arg_parse.backend,
        ctx.platform,
        renderer_ctx_buffer
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize renderer subsystem!\n"
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* threading_buffer = stack_allocator_push(
        &ctx.stack, threading_subsystem_size
    );
    LOG_ASSERT(
        threading_buffer,
        "Stack Arena of size {u} is not enough to initialize engine!",
        ctx.stack.size
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

        StringView error_message_buffer_view;
        error_message_buffer_view.buffer = error_message_buffer;
        error_message_buffer_view.len    = ERROR_MESSAGE_SIZE;

        sv_fill( error_message_buffer_view, ' ' );
        sv_format(
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
    for( u64 i = 0; i < MEMORY_TYPE_COUNT; ++i ) {
        MemoryType type = (MemoryType)i;
        f64 memory_usage_f64 = (f64)query_memory_usage( type );
        LOG_NOTE(
            "    {cc,-25} {f,b,4.2}",
            memory_type_to_string(type),
            memory_usage_f64
        );
        total_memory_usage += memory_usage_f64;
    }
    LOG_NOTE("    {cc,-25} {f,b,4.2}", "Total Memory Usage", total_memory_usage);
    LOG_NOTE("Engine stack pointer: {u64}", (u64)ctx.stack.current);
#endif

    ctx.cursor_style      = CURSOR_ARROW;
    ctx.cursor_is_visible = true;

    // if( !audio_init( ctx.platform ) ) {
    //     return false;
    // }

    void* application_memory = stack_allocator_push(
        &ctx.stack, config.memory_size
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

        RenderData render_data = {};
        render_data.time = &ctx.time;

        if( !ctx.application_run( &ctx, application_memory ) ) {
            return false;
        }

        if( !renderer_subsystem_on_draw( &render_data ) ) {
            MESSAGE_BOX_FATAL(
                "Renderer Failure",
                "Unknown Error!\n"
                LD_CONTACT_MESSAGE
            );
            return false;
        }
        
        // audio_test( ctx.platform );

        event_fire_end_of_frame();

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
    renderer_subsystem_shutdown();
    platform_shutdown( ctx.platform );
    threading_shutdown();
    log_shutdown();

    return true;
}
void engine_set_cursor_style( EngineContext* opaque, u32 style ) {
    InternalEngineContext* ctx = opaque;
    ctx->cursor_style = (CursorStyle)style;
    platform_cursor_set_style(
        ctx->platform,
        (CursorStyle)style
    );
}
void engine_set_cursor_visibility( EngineContext* opaque, b32 visible ) {
    InternalEngineContext* ctx = opaque;
    ctx->cursor_is_visible = visible;
    platform_cursor_set_visible(
        ctx->platform,
        visible
    );
}
void engine_center_cursor( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    platform_cursor_center( ctx->platform );
}
void engine_lock_cursor( EngineContext* opaque, b32 locked ) {
    InternalEngineContext* ctx = opaque;
    ctx->cursor_is_locked = locked;
    if( locked ) {
        ctx->cursor_is_visible = false;
    }
}
u32 engine_query_cursor_style( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return (u32)ctx->cursor_style;
}
b32 engine_query_cursor_visibility( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->cursor_is_visible;
}
b32 engine_query_cursor_locked( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->cursor_is_locked;
}
void engine_set_application_name( EngineContext* opaque, StringView name ) {
    InternalEngineContext* ctx = opaque;

    RendererBackend backend = renderer_subsystem_query_backend();

    StringView renderer_backend_name =
        SV( renderer_backend_to_string( backend ) );

    sv_format(
        ctx->application_name_view,
        "{sv} | {sv}",
        name, renderer_backend_name
    );

    platform_surface_set_name(
        ctx->platform,
        ctx->application_name_view
    );
}
StringView engine_query_application_name( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->application_name_view;
}
usize engine_query_logical_processor_count( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->system_info.logical_processor_count;
}
usize engine_query_total_system_memory( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->system_info.total_memory;
}
const char* engine_query_processor_name( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->system_info.cpu_name_buffer;
}
ivec2 engine_query_surface_size( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->platform->surface.dimensions;
}

LD_API struct Timer* engine_get_time( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return &ctx->time;
}

