// * Description:  Engine Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "core/engine.h"
#include "platform.h"
#include "renderer.h"
#include "core/thread.h"
#include "core/events.h"
#include "core/log.h"
#include "core/input.h"
#include "core/timer.h"
#include "core/mem.h"
#include "core/allocator.h"
#include "core/strings.h"
#include "core/mathf.h"
#include "core/collections.h"
#include "core/library.h"
#include "core/graphics.h"
#include "core/graphics/types.h"
#include "core/graphics/ui.h"
#include "core/cstr.h"

#define MAX_APPLICATION_NAME (255)
#define DEFAULT_APPLICATION_NAME "Liquid Engine"
typedef struct InternalEngineContext {
    SystemInfo     system_info; // 88
    StackAllocator stack;       // 32
    Timer          time;        // 16
    PlatformSurface* main_surface;
    RendererContext* main_surface_renderer_context;

    RenderData render_data;

    ivec2 render_resolution;
    f32   render_resolution_scale;

    b8 is_running;                // 1
    b8 pause_on_surface_inactive; // 1
    
    char application_name[MAX_APPLICATION_NAME];
} InternalEngineContext;

internal ivec2 calculate_scaled_resolution(
    ivec2 resolution, f32 resolution_scale
) {
    ivec2 result = (ivec2){
        truncate_i32((f32)resolution.width * resolution_scale),
        truncate_i32((f32)resolution.height * resolution_scale)
    };

    result.x = max( result.x, 1 );
    result.y = max( result.y, 1 );

    return result;
}

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

void on_close_listener( PlatformSurface* surface, void* params ) {
    unused(params);
    unused(surface);
    Event event = {};
    event.code = EVENT_CODE_EXIT;
    event_fire( event );
}
void on_resize_listener(
    PlatformSurface* surface,
    ivec2 old_dimensions, ivec2 new_dimensions,
    void* params
) {
    unused(surface);
    unused(old_dimensions);
    InternalEngineContext* ctx = params;

    ctx->render_resolution = new_dimensions;

    ivec2 scaled_resolution = calculate_scaled_resolution(
        ctx->render_resolution,
        ctx->render_resolution_scale
    );
    renderer_subsystem_on_resize(
        ctx->main_surface_renderer_context,
        new_dimensions,
        scaled_resolution
    );

    Event event = {};
    event.code  = EVENT_CODE_SURFACE_RESIZE;
    event.data.resize = (struct EventResize) {
        new_dimensions,
        scaled_resolution,
        ctx->render_resolution_scale
    };
}
void on_activate_listener(
    PlatformSurface* surface,
    b32 is_active,
    void* params
) {
    unused(surface);
    unused(params);
    Event event = {};
    event.code  = EVENT_CODE_APP_ACTIVE;
    event.data.app_active.active = is_active;
    event_fire( event );
}

typedef struct ArgParseResult {
    b32 success;
    RendererBackend backend;
    StringSlice library_path;
    b32 quit_instant;
} ArgParseResult;

internal void print_help(void) {
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

    result.library_path.buffer = DEFAULT_LIBRARY_PATH;
    result.library_path.len    = sizeof DEFAULT_LIBRARY_PATH;

    result.backend = RENDERER_BACKEND_OPENGL;
    result.success = true;

    STRING( libload, "--libload=" );
    STRING( help, "--help" );
    STRING( h, "-h" );
    STRING( output_debug_string, "--output-debug-string" );
    STRING( gl, "--gl" );
    STRING( vk, "--vk" );
    STRING( dx11, "--dx11" );
    STRING( dx12, "--dx12" );

    for( i32 i = 1; i < argc; ++i ) {
        StringSlice current_arg;
        current_arg.buffer = argv[i];
        current_arg.len    = cstr_len( current_arg.buffer );

        if(
            ss_cmp( &current_arg, &help ) ||
            ss_cmp( &current_arg, &h )
        ) {
            print_help();
            result.quit_instant = true;
            return result;
        }

#if defined(LD_PLATFORM_WINDOWS)
        if( ss_cmp( &current_arg, &output_debug_string ) ) {
            log_subsystem_win32_enable_output_debug_string();
            continue;
        }
#endif
        if( ss_cmp( &current_arg, &gl ) ) {
            result.backend = RENDERER_BACKEND_OPENGL;
        } else if( ss_cmp( &current_arg, &vk ) ) {
            result.backend = RENDERER_BACKEND_VULKAN;
        } else if( ss_cmp( &current_arg, &dx11 ) ) {
#if defined(LD_PLATFORM_WINDOWS)
            result.backend = RENDERER_BACKEND_DX11;
#else
            println_err( "DirectX11 is not available on non-windows platforms!" );
            print_help();
            result.quit_instant = true;
            return result;
#endif
        } else if( ss_cmp( &current_arg, &dx12 ) ) {
#if defined(LD_PLATFORM_WINDOWS)
            result.backend = RENDERER_BACKEND_DX12;
#else
            println_err( "DirectX12 is not available on non-windows platforms!" );
            print_help();
            result.quit_instant = true;
            return result;
#endif
        } else if( ss_find( &current_arg, &libload, NULL ) ) {
            result.library_path.buffer = &current_arg.buffer[libload.len];
            result.library_path.len    = current_arg.len - libload.len;
        } else {
            println_err( "Unrecognized argument: {s}", current_arg );
            print_help();
            result.success      = false;
            result.quit_instant = true;
        }
    }

    return result;
}

b32 engine_entry( int argc, char** argv ) {

#if defined(LD_LOGGING)

    usize log_subsystem_size = kilobytes(1);
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
    ctx.render_resolution_scale = 1.0f;
    ctx.render_resolution = calculate_scaled_resolution(
        DEFAULT_SURFACE_DIMENSIONS,
        ctx.render_resolution_scale
    );

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

    usize thread_subsystem_size   =
        thread_subsystem_query_size( thread_count );
    usize renderer_subsystem_size =
        renderer_subsystem_query_size( arg_parse.backend );

    usize ui_max_elements = 1000;
    usize ui_subsystem_size = ui_calculate_required_size( ui_max_elements );

    usize max_render_objects = 1000;
    usize render_object_buffer_size = max_render_objects * sizeof(RenderObject);

    // calculate required stack arena size
    usize required_stack_size =
        thread_subsystem_size   +
        EVENT_SUBSYSTEM_SIZE    +
        INPUT_SUBSYSTEM_SIZE    +
        PLATFORM_SUBSYSTEM_SIZE +
        PLATFORM_SURFACE_BUFFER_SIZE +
        renderer_subsystem_size +
        application_memory_size +
        ui_subsystem_size +
        render_object_buffer_size;

    usize stack_allocator_pages  = calculate_page_count( required_stack_size );
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
        stack_allocator_pages * MEMORY_PAGE_SIZE,
        stack_allocator_buffer, MEMORY_TYPE_ENGINE );

    LOG_INFO("Liquid Engine Version: {i}.{i}",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    void* event_subsystem_data = stack_allocator_push(
        &ctx.stack, EVENT_SUBSYSTEM_SIZE );

    if( !event_subsystem_init( event_subsystem_data ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* input_subsystem_buffer = stack_allocator_push(
        &ctx.stack, INPUT_SUBSYSTEM_SIZE );
    if( !input_subsystem_init( input_subsystem_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize input subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* platform_buffer = stack_allocator_push(
        &ctx.stack, PLATFORM_SUBSYSTEM_SIZE );
    if( !platform_subsystem_init( platform_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    PlatformSurface* surface = stack_allocator_push(
        &ctx.stack, PLATFORM_SURFACE_BUFFER_SIZE );

    PlatformSurfaceCreateFlags surface_flags = 0;
    surface_flags |=
        PLATFORM_SURFACE_CREATE_HIDDEN | PLATFORM_SURFACE_CREATE_RESIZEABLE;
    if( !platform_surface_create(
        DEFAULT_SURFACE_DIMENSIONS,
        "Liquid Engine",
        arg_parse.backend,
        surface_flags,
        surface
    ) ) {
        MESSAGE_BOX_FATAL(
            "Platform Layer Failure",
            "Failed to create main surface!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    ctx.main_surface = surface;
    platform_surface_show( surface );
    platform_surface_set_resize_callback(
        surface, on_resize_listener, &ctx );
    platform_surface_set_activate_callback(
        surface, on_activate_listener, &ctx );
    platform_surface_set_close_callback(
        surface, on_close_listener, &ctx );

    ctx.pause_on_surface_inactive = true;
    ctx.main_surface_renderer_context =
        stack_allocator_push( &ctx.stack, renderer_subsystem_size );

    ctx.render_data.max_object_count = max_render_objects;
    ctx.render_data.object_count = 0;
    ctx.render_data.objects =
        stack_allocator_push( &ctx.stack, render_object_buffer_size );

    if( !renderer_subsystem_init(
        ctx.main_surface,
        arg_parse.backend,
        ctx.main_surface_renderer_context
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize renderer subsystem!\n"
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    void* ui_buffer = stack_allocator_push( &ctx.stack, ui_subsystem_size );
    if( !ui_subsystem_init( ui_max_elements, ui_buffer ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize ui subsystem!\n"
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
    b32 sse = bitfield_check(
        ctx.system_info.features,
        SSE_MASK | SSE2_MASK | SSE3_MASK |
        SSSE3_MASK | SSE4_1_MASK | SSE4_2_MASK
    );
    b32 avx    = bitfield_check( ctx.system_info.features, AVX_MASK );
    b32 avx2   = bitfield_check( ctx.system_info.features, AVX2_MASK );
    b32 avx512 = bitfield_check( ctx.system_info.features, AVX512_MASK );
    unused(avx512);
    ProcessorFeatures features = ctx.system_info.features;
    if( LD_SIMD_WIDTH == 4 && !sse ) {
        #define ERROR_MESSAGE_SIZE 256
        char error_message_buffer[ERROR_MESSAGE_SIZE];

        StringSlice error_message =
            ss_from_cstr( ERROR_MESSAGE_SIZE - 1, error_message_buffer );

        ss_mut_fill( &error_message, ' ' );
        ss_mut_fmt(
            &error_message,
            "Your CPU does not support SSE instructions!\n"
            "Missing instructions: {cc}{cc}{cc}{cc}{cc}{cc}",
            bitfield_check(features, SSE_MASK)    ? "" : "SSE, ",
            bitfield_check(features, SSE2_MASK)   ? "" : "SSE2, ",
            bitfield_check(features, SSE3_MASK)   ? "" : "SSE3, ",
            bitfield_check(features, SSSE3_MASK)  ? "" : "SSSE3, ",
            bitfield_check(features, SSE4_1_MASK) ? "" : "SSE4.1, ",
            bitfield_check(features, SSE4_2_MASK) ? "" : "SSE4.2"
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

    void* application_memory =
        stack_allocator_push( &ctx.stack, application_memory_size );

#if defined(LD_LOGGING) && defined(LD_PROFILING)
    LOG_NOTE("System Memory: {f,b,02.3}", (f64)ctx.system_info.total_memory );
    LOG_NOTE("Page Size: {u64}", (u64)MEMORY_PAGE_SIZE);
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
    LOG_NOTE( "Stack usage: {f,b,.2} out of {f,b,.2}", (f64)ctx.stack.current, (f64)ctx.stack.size );
    LOG_NOTE("Engine stack pointer: {u64}", (u64)ctx.stack.current);
#endif

    STRING( default_name, DEFAULT_APPLICATION_NAME );
    engine_application_set_name( &ctx, &default_name );
    ctx.is_running = true;
    if( !application_init( &ctx, application_memory ) ) {
        return false;
    }

    while( ctx.is_running ) {
        input_swap();
        platform_poll_gamepad();
        platform_surface_pump_events( surface );

        b32 is_active_now = platform_surface_query_active( surface );

        if( !is_active_now &&
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

        if( input_key_press( KEY_F11 ) ) {
            b32 is_fullscreen = engine_surface_query_fullscreen( &ctx );
            engine_surface_set_fullscreen( &ctx, !is_fullscreen );
        }

        ctx.render_data.delta_time   = ctx.time.delta_seconds;
        ctx.render_data.elapsed_time = ctx.time.elapsed_seconds;

        if( !application_run( &ctx, application_memory ) ) {
            return false;
        }

        ivec2 surface_dimensions = engine_surface_query_resolution( &ctx );
        ui_subsystem_update_render_data( surface_dimensions, &ctx.render_data );

        if( !renderer_subsystem_on_draw(
            ctx.main_surface_renderer_context,
            &ctx.render_data
        ) ) {
            MESSAGE_BOX_FATAL(
                "Renderer Failure",
                "Unknown Error!\n"
                LD_CONTACT_MESSAGE
            );
            return false;
        }

        event_fire_end_of_frame();

#if defined(LD_PLATFORM_WINDOWS)
        if(
            ctx.time.frame_count %
            WIN32_POLL_FOR_NEW_XINPUT_GAMEPAD_RATE == 0
        ) {
            platform_win32_signal_xinput_polling_thread();
        }
#endif
        ++ctx.time.frame_count;

        f64 seconds_elapsed = platform_s_elapsed();
        ctx.time.delta_seconds =
            seconds_elapsed - ctx.time.elapsed_seconds;
        ctx.time.elapsed_seconds = seconds_elapsed;

        ctx.render_data.object_count = 0;
    }

    event_unsubscribe( event_exit_id );
    event_unsubscribe( event_on_active_id );

    renderer_subsystem_shutdown( ctx.main_surface_renderer_context );
    platform_surface_destroy( surface );
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
LD_API void engine_cursor_center( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    platform_cursor_center( ctx->main_surface );
}
LD_API CursorStyle engine_cursor_style(void) {
    return platform_cursor_style();
}
LD_API b32 engine_cursor_visible(void) {
    return platform_cursor_visible();
}
LD_API void engine_application_set_name(
    EngineContext* opaque, StringSlice* name
) {
    InternalEngineContext* ctx = opaque;
    usize copy_size = name->len;
    enum RendererBackend backend =
        renderer_subsystem_query_backend( ctx->main_surface_renderer_context );
    StringSlice backend_name =
        ss_from_cstr( 0, renderer_backend_to_string( backend ) );

    usize backend_append_size = backend_name.len + 3;

    usize max_copy_size = MAX_APPLICATION_NAME - backend_append_size + 1;
    if( copy_size >= max_copy_size ) {
        copy_size = max_copy_size;
    }
    mem_copy( ctx->application_name, name->buffer, copy_size );

    ctx->application_name[copy_size + 0] = ' ';
    ctx->application_name[copy_size + 1] = '|';
    ctx->application_name[copy_size + 2] = ' ';

    mem_copy(
        ctx->application_name + copy_size + 3,
        backend_name.buffer, backend_name.len );

    usize name_size = copy_size + backend_append_size;
    ctx->application_name[name_size] = 0;
    platform_surface_set_name( ctx->main_surface, ctx->application_name );
}
LD_API StringSlice engine_application_name( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    StringSlice result = ss_from_cstr( 0, ctx->application_name );
    return result;
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
LD_API void engine_surface_center( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    platform_surface_center( ctx->main_surface );
}
LD_API Timer engine_time( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->time;
}
LD_API void engine_surface_set_fullscreen(
    EngineContext* opaque, b32 fullscreen
) {
    InternalEngineContext* ctx = opaque;
    PlatformSurfaceMode mode = fullscreen ?
        PLATFORM_SURFACE_MODE_FULLSCREEN :
        PLATFORM_SURFACE_MODE_FLOATING_WINDOW;
    platform_surface_set_mode( ctx->main_surface, mode );
}
LD_API b32 engine_surface_query_fullscreen( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return platform_surface_query_mode( ctx->main_surface ) ==
        PLATFORM_SURFACE_MODE_FULLSCREEN;
}

LD_API ivec2 engine_surface_query_size( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return platform_surface_query_dimensions( ctx->main_surface );
}
LD_API ivec2 engine_surface_query_resolution( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->render_resolution;
}
LD_API f32 engine_surface_query_resolution_scale( EngineContext* opaque ) {
    InternalEngineContext* ctx = opaque;
    return ctx->render_resolution_scale;
}
LD_API void engine_surface_set_resolution(
    EngineContext* opaque,
    ivec2 surface_size,
    ivec2 render_resolution,
    f32 render_resolution_scale
) {
    InternalEngineContext* ctx = opaque;

    ctx->render_resolution       = render_resolution;
    ctx->render_resolution_scale = render_resolution_scale;

    if( engine_surface_query_fullscreen( ctx ) ) {

        ivec2 scaled_resolution = calculate_scaled_resolution(
            ctx->render_resolution,
            ctx->render_resolution_scale
        );
        renderer_subsystem_on_resize(
            ctx->main_surface_renderer_context,
            surface_size,
            scaled_resolution
        );

        Event event = {};
        event.code  = EVENT_CODE_SURFACE_RESIZE;
        event.data.resize = (struct EventResize) {
            surface_size,
            render_resolution,
            render_resolution_scale
        };

    } else {
        // NOTE(alicia): the callback will take care of the rest.
        platform_surface_set_dimensions( ctx->main_surface, surface_size );
    }

}


