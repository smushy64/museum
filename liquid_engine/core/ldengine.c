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
    SystemInfo     system_info; // 88
    StackAllocator stack;       // 32
    Timer          time;        // 16

    b8 is_running;                // 1
    b8 pause_on_surface_inactive; // 1
} InternalEngineContext;

EventCallbackResult on_app_exit( Event* event, void* void_ctx ) {
    unused(event);
    InternalEngineContext* ctx = void_ctx;
    ctx->is_running = false;
    LOG_INFO( "Application requested engine exit." );
    return EVENT_CALLBACK_CONSUMED;
}
EventCallbackResult on_active( Event* event, void* params ) {
    unused(params);
    if( event->data.app_active.active ) {
        LOG_NOTE("Surface activated.");
    } else {
        LOG_NOTE("Surface deactivated.");
    }
    return EVENT_CALLBACK_CONSUMED;
}
EventCallbackResult on_resize( Event* event, void* params ) {
    unused(params);
    renderer_subsystem_on_resize( event->data.resize.new_dimensions );
    return EVENT_CALLBACK_NOT_CONSUMED;
}

typedef struct ArgParseResult {
    b32 success;
    RendererBackend backend;
    StringView library_path;
    b32 quit_instant;
} ArgParseResult;

internal void print_help() {
    println(
        "OVERVIEW: Liquid Engine {i}.{i}\n",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );
    println("USAGE: {cc} [options]\n", LIQUID_ENGINE_EXECUTABLE );
    println("OPTIONS:");
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

    result.library_path = SV( DEFAULT_LIBRARY_PATH );

    result.backend = RENDERER_BACKEND_OPENGL;
    result.success = true;

    StringView libload = SV("--libload=");

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
            log_subsystem_win32_enable_output_debug_string();
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
        } else if( sv_contains( current_arg, libload ) ) {
            result.library_path.str = &current_arg.str[libload.len];
            result.library_path.len = current_arg.len - libload.len;
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

#if defined(LD_LOGGING)

    usize log_subsystem_size = KILOBYTES(1);
    void* log_subsystem_buffer = ldalloc( log_subsystem_size, MEMORY_TYPE_ENGINE );
    if( log_subsystem_buffer ) {
        if( !log_subsystem_init(
            DEFAULT_LOG_LEVEL, log_subsystem_size, log_subsystem_buffer
        ) ) {
            MESSAGE_BOX_FATAL(
                "Subsystem Failure",
                "Failed to initialize logging subsystem!\n "
                LD_CONTACT_MESSAGE
            );
            return false;
        }
    }

#endif

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
    ApplicationQueryMemoryRequirementFN* application_query_memory_requirement =
    (ApplicationQueryMemoryRequirementFN*)library_load_function(
        &application_lib, "application_query_memory_requirement"
    );
    if( !application_query_memory_requirement ) {
        return false;
    }

    ApplicationInitFN* application_init =
        (ApplicationInitFN*)library_load_function(
            &application_lib, "application_init" );
    if( !application_init ) {
        return false;
    }
    ApplicationRunFN* application_run =
        (ApplicationRunFN*)library_load_function(
            &application_lib, "application_run" );
    if( !application_run ) {
        return false;
    }

    usize application_memory_size = application_query_memory_requirement();

    platform_query_system_info( &ctx.system_info );

    u32 thread_count = ctx.system_info.logical_processor_count;
    thread_count = (thread_count == 1 ? thread_count : thread_count - 1);

    usize thread_subsystem_size = thread_subsystem_query_size( thread_count );
    usize event_subsystem_size  = event_subsystem_query_size();
    usize input_subsystem_size  = input_subsystem_query_size();
    usize platform_subsystem_size  = platform_subsystem_query_size();
    usize renderer_subsystem_size  =
        renderer_subsystem_query_size( arg_parse.backend );

    // calculate required stack arena size
    usize required_stack_size =
        thread_subsystem_size   +
        event_subsystem_size    +
        input_subsystem_size    +
        platform_subsystem_size +
        renderer_subsystem_size +
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

    LOG_INFO("Liquid Engine Version: {i}.{i}",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    void* event_subsystem_data = stack_allocator_push(
        &ctx.stack,
        event_subsystem_size
    );

    if( !event_subsystem_init( event_subsystem_data ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* input_subsystem_buffer = stack_allocator_push(
        &ctx.stack,
        input_subsystem_size
    );
    if( !input_subsystem_init( input_subsystem_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize input subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* platform_buffer =
        stack_allocator_push( &ctx.stack, platform_subsystem_size );
    if( !platform_subsystem_init(
        DEFAULT_SURFACE_DIMENSIONS, platform_buffer
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    ctx.pause_on_surface_inactive = true;
    void* renderer_subsystem_buffer =
        stack_allocator_push( &ctx.stack, renderer_subsystem_size );

    if( !renderer_subsystem_init(
        arg_parse.backend,
        renderer_subsystem_buffer
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize renderer subsystem!\n"
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* thread_subsystem_buffer =
        stack_allocator_push( &ctx.stack, thread_subsystem_size );
    LOG_ASSERT(
        thread_subsystem_buffer,
        "Stack Arena of size {u} is not enough to initialize engine!",
        ctx.stack.size
    );

    if( !thread_subsystem_init( thread_count, thread_subsystem_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize threading subsystem!\n"
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    LOG_NOTE("CPU: {cc}", ctx.system_info.cpu_name_buffer);
    LOG_NOTE("  Logical Processors: {u16}",
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
        sse    ? "[SSE1-4] " : "",
        avx    ? "[AVX] " : "",
        avx2   ? "[AVX2] " : "",
        avx512 ? "[AVX-512] " : ""
    );
#endif // x86

    EventListenerID event_exit_id = event_subscribe(
        EVENT_CODE_EXIT, on_app_exit, &ctx );
    if( !event_exit_id ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    EventListenerID event_on_active_id = event_subscribe(
        EVENT_CODE_APP_ACTIVE, on_active, &ctx );
    if( !event_on_active_id ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    EventListenerID event_on_surface_resize_id = event_subscribe(
        EVENT_CODE_SURFACE_RESIZE, on_resize, &ctx );
    if( !event_on_surface_resize_id ) {
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

    ctx.is_running = true;
    void* application_memory =
        stack_allocator_push( &ctx.stack, application_memory_size );
    if( !application_init( &ctx, application_memory ) ) {
        return false;
    }

    while( ctx.is_running ) {
        input_swap();
        platform_poll_gamepad();
        platform_pump_events();

        if( !platform_is_active() && ctx.pause_on_surface_inactive ) {
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

        RenderData render_data = {};
        render_data.time = &ctx.time;

        if( !application_run( &ctx, application_memory ) ) {
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

        event_fire_end_of_frame();

        ++ctx.time.frame_count;

        f64 seconds_elapsed = platform_s_elapsed();
        ctx.time.delta_seconds =
            seconds_elapsed - ctx.time.elapsed_seconds;
        ctx.time.elapsed_seconds = seconds_elapsed;
    }

    event_unsubscribe( event_exit_id );
    event_unsubscribe( event_on_surface_resize_id );
    event_unsubscribe( event_on_active_id );

    renderer_subsystem_shutdown();
    platform_subsystem_shutdown();
    thread_subsystem_shutdown();
    log_subsystem_shutdown();

    return true;
}
LD_API void engine_cursor_set_style( CursorStyle style ) {
    platform_cursor_set_style( style );
}
LD_API void engine_cursor_set_visibility( b32 visible ) {
    platform_cursor_set_visible( visible );
}
LD_API void engine_cursor_center() {
    platform_cursor_center();
}
LD_API CursorStyle engine_cursor_style() {
    return platform_cursor_style();
}
LD_API b32 engine_cursor_visible() {
    return platform_cursor_visible();
}
LD_API void engine_set_application_name( const char* name ) {
    LOG_NOTE( "Application name set: {cc}", name );
    platform_set_application_name( name );
}
LD_API StringView engine_application_name() {
    StringView name = SV( platform_application_name() );
    return name;
}
LD_API usize engine_query_logical_processor_count( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->system_info.logical_processor_count;
}
LD_API usize engine_query_total_system_memory( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->system_info.total_memory;
}
LD_API const char* engine_query_processor_name( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->system_info.cpu_name_buffer;
}
LD_API void engine_surface_set_dimensions( ivec2 new_dimensions ) {
    LOG_NOTE( "Set surface dimensions: {iv2}", new_dimensions );
    platform_surface_set_dimensions( new_dimensions );
}
LD_API ivec2 engine_query_surface_dimensions() {
    return platform_surface_dimensions();
}
LD_API void engine_surface_center() {
    LOG_NOTE( "Surface centered." );
    platform_surface_center();
}

LD_API struct Timer* engine_get_time( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return &ctx->time;
}

