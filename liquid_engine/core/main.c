// * Description:  Liquid Engine Core Main
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 23, 2023
#include "defines.h"
#include <platform.h>
#include "core/graphics.h"
#include "core/strings.h"
#include "core/log.h"
#include "core/allocator.h"
#include "core/mem.h"
#include "core/thread.h"
#include "core/input.h"
#include "core/events.h"
#include "core/mathf.h"
#include "core/timer.h"
#include "renderer.h"

#define LOGGING_SUBSYSTEM_SIZE (kilobytes(1))

typedef usize ApplicationQueryMemoryRequirementFN(void);
typedef b32 ApplicationInitializeFN( void* memory );
typedef b32 ApplicationRunFN( TimeStamp time, void* memory );

struct PlatformAPI* platform = NULL;

#if defined(LD_PLATFORM_WINDOWS)
    #define GAME_LIBRARY_PATH_DEFAULT "GAME.DLL"
#else
    #define GAME_LIBRARY_PATH_DEFAULT "game.so"
#endif

#define DEFAULT_RESOLUTION_WIDTH  (800)
#define DEFAULT_RESOLUTION_HEIGHT (600)

#define CORE_SUCCESS                              (0)
#define CORE_ERROR_PARSE                          (128)
#define CORE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED (129)
#define CORE_ERROR_OPEN_GAME_LIBRARY              (130)
#define CORE_ERROR_LOAD_GAME_MEMORY_REQUIREMENT   (131)
#define CORE_ERROR_LOAD_GAME_INITIALIZE           (132)
#define CORE_ERROR_LOAD_GAME_RUN                  (133)
#define CORE_ERROR_ENGINE_MEMORY_ALLOCATION       (134)
#define CORE_ERROR_LOGGING_SUBSYSTEM_INITIALIZE   (135)
#define CORE_ERROR_THREAD_SUBSYSTEM_INITIALIZE    (136)
#define CORE_ERROR_CREATE_SURFACE                 (137)

internal no_inline void on_resolution_change(
    PlatformSurface* surface,
    i32 old_width, i32 old_height,
    i32 new_width, i32 new_height,
    void* user_params
) {
    unused( surface );
    unused( old_width + old_height );

    // TODO(alicia): resolution scaling
    RendererContext** renderer_context = user_params;
    if( *renderer_context ) {
        ivec2 surface_dimensions     = { new_width, new_height };
        ivec2 framebuffer_dimensions = surface_dimensions;
        renderer_subsystem_on_resize(
            *renderer_context,
            surface_dimensions,
            framebuffer_dimensions );
    }
}
internal no_inline void on_close(
    PlatformSurface* surface, void* user_params
) {
    unused(surface);
    b32* application_close = user_params;
    *application_close     = true;
    LOG_NOTE( "Application is shutting down." );
}
internal no_inline void on_activate(
    PlatformSurface* surface, b32 is_active, void* user_params
) {
    unused(surface);
    b32* surface_is_active = user_params;
    *surface_is_active     = is_active;
    LOG_NOTE( "Surface {cc}", is_active ? "is active." : "is inactive." );
}
internal no_inline void on_key(
    PlatformSurface* surface, b32 is_down,
    PlatformKeyboardCode code, void* user_params
) {
    unused(surface);
    unused(user_params);
    input_set_key( code, is_down );
}
internal no_inline void on_mouse_button(
    PlatformSurface* surface, b32 is_down,
    PlatformMouseCode code, void* user_params
) {
    unused(surface);
    unused(user_params);
    input_set_mouse_button( code, is_down );
}
internal no_inline void on_mouse_move(
    PlatformSurface* surface,
    i32 x, i32 y, void* user_params
) {
    unused(surface);
    unused(user_params);
    input_set_mouse_position( iv2( x, y ) );
}
internal no_inline void on_mouse_wheel(
    PlatformSurface* surface,
    b32 is_horizontal, i32 value, void* user_params
) {
    unused(surface);
    unused(user_params);
    if( is_horizontal ) {
        input_set_horizontal_mouse_wheel( value );
    } else {
        input_set_mouse_wheel( value );
    }
}

internal void print_help(void);
LD_API int core_init(
    int argc, char** argv,
    struct PlatformAPI* in_platform
) {
    assert( in_platform );
    platform = in_platform;

    i32 width  = DEFAULT_RESOLUTION_WIDTH;
    i32 height = DEFAULT_RESOLUTION_HEIGHT;
    RendererBackend backend = RENDERER_BACKEND_OPENGL;
    STRING( game_library_path, GAME_LIBRARY_PATH_DEFAULT );

    /* parse arguments */ {
        STRING( libload, "--libload=" );
        STRING( help, "--help" );
        STRING( h, "-h" );
#if defined(LD_PLATFORM_WINDOWS)
        STRING( output_debug_string, "--output-debug-string" );
        b32 enable_output_debug_string = false;
        STRING( dx11,   "--directx11" );
        STRING( dx12,   "--directx12" );
#endif
        STRING( opengl, "--opengl" );
        STRING( vulkan, "--vulkan" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
        STRING( metal,  "--metal" );
#endif
#if defined(LD_PLATFORM_WASM)
        STRING( webgl,  "--webgl" );
#endif

        b32 parse_error = false;

        for( int i = 1; i < argc; ++i ) {
            StringSlice current = ss_from_cstr( 0, argv[i] );

            if( ss_find( &current, &libload, NULL ) ) {
                if( current.len - libload.len < 1 ) {
                    StringSlice path = ss_clone( &current );
                    path.buffer += libload.len;
                    path.len    -= libload.len;
                    println_err(
                        LOG_COLOR_RED
                        "invalid game library path: {s}"
                        LOG_COLOR_RESET,
                        path
                    );
                    parse_error = true;
                    break;
                }
                game_library_path.buffer = current.buffer + libload.len;
                game_library_path.len    = current.len - libload.len;
                continue;
            }

            if( ss_cmp( &current, &opengl ) ) {
                backend = RENDERER_BACKEND_OPENGL;
                continue;
            }

            if( ss_cmp( &current, &vulkan ) ) {
                backend = RENDERER_BACKEND_VULKAN;
                continue;
            }

#if defined(LD_PLATFORM_WINDOWS)
            if( ss_cmp( &current, &output_debug_string ) ) {
                enable_output_debug_string = true;
                continue;
            }

            if( ss_cmp( &current, &dx11 ) ) {
                backend = RENDERER_BACKEND_DX11;
                continue;
            }
            if( ss_cmp( &current, &dx12 ) ) {
                backend = RENDERER_BACKEND_DX12;
                continue;
            }
#endif
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
            if( ss_cmp( &current, &metal ) ) {
                backend = RENDERER_BACKEND_METAL;
                continue;
            }
#endif
#if defined(LD_PLATFORM_WASM)
            if( ss_cmp( &current, &webgl ) ) {
                backend = RENDERER_BACKEND_WEBGL;
                continue;
            }
#endif
            if( ss_cmp( &current, &help ) || ss_cmp( &current, &h ) ) {
                print_help();
                return 0;
            }

            println_err(
                LOG_COLOR_RED
                "unrecognized argument: {s}"
                LOG_COLOR_RESET,
                current );
            parse_error = true;
            break;
        }
        
        if( parse_error ) {
            print_help();
            return CORE_ERROR_PARSE;
        }

#if defined(LD_PLATFORM_WINDOWS)
        if( enable_output_debug_string ) {
            log_subsystem_win32_enable_output_debug_string();
        }
#endif
    }

    if( !renderer_backend_is_supported( backend ) ) {
        println_err(
            LOG_COLOR_RED
            "renderer backend '{cc}' is not supported on current platform!"
            LOG_COLOR_RESET,
            renderer_backend_to_string( backend )
        );
        return CORE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED;
    }

    println( "Engine Configuration:" );
    println( "Version:           {i}.{i}", LIQUID_ENGINE_VERSION_MAJOR, LIQUID_ENGINE_VERSION_MINOR );
#if defined(LD_PLATFORM_WINDOWS)
    STRING(os, "win32");
#endif
#if defined(LD_PLATFORM_MACOS)
    STRING(os, "macos");
#endif
#if defined(LD_PLATFORM_IOS)
    STRING(os, "ios");
#endif
#if defined(LD_PLATFORM_ANDROID)
    STRING(os, "android");
#endif
#if defined(LD_PLATFORM_LINUX)
    STRING(os, "linux");
#endif
#if defined(LD_PLATFORM_WASM)
    STRING(os, "wasm");
#endif
#if defined(LD_ARCH_32_BIT)
    #if defined(LD_ARCH_X86)
        STRING(arch, "x86");
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            STRING(arch, "arm little-endian 32-bit");
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            STRING(arch, "arm big-endian 32-bit");
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        STRING(arch, "wasm 32-bit");
    #endif
#endif
#if defined(LD_ARCH_64_BIT)
    #if defined(LD_ARCH_X86)
        STRING(arch, "x86_64");
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            STRING(arch, "arm little-endian 64-bit");
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            STRING(arch, "arm big-endian 64-bit");
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        STRING(arch, "wasm 64-bit");
    #endif
#endif
    println( "Platform:          {s}, {s}", os, arch );
    println( "Game Library Path: {s}", game_library_path );
    println( "Renderer Backend:  {cc}",
        renderer_backend_to_string( backend ) );
    println( "Resolution:        {i}x{i}", width, height );

    char fatal_error_title_buffer[255];
    char fatal_error_message_buffer[255];
    StringSlice fatal_error_title =
        ss( 255, fatal_error_title_buffer );
    StringSlice fatal_error_message =
        ss( 255, fatal_error_message_buffer );

    PlatformLibrary* game =
        platform->library.open( game_library_path.buffer );
    if( !game ) {
        ss_mut_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_OPEN_GAME_LIBRARY, 0 );
        ss_mut_fmt(
            &fatal_error_message,
            "Failed to load game library! Game library path: {s}{c}",
            game_library_path, 0 );
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_OPEN_GAME_LIBRARY;
    }

    ApplicationQueryMemoryRequirementFN*
        application_query_memory_requirement =
        (ApplicationQueryMemoryRequirementFN*)platform->library.load_function(
            game, "application_query_memory_requirement" );
    ApplicationInitializeFN* application_initialize =
        (ApplicationInitializeFN*)platform->library.load_function(
            game, "application_initialize" );
    ApplicationRunFN* application_run =
        (ApplicationRunFN*)platform->library.load_function(
            game, "application_run" );

    if( !application_query_memory_requirement ) {
        ss_mut_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_LOAD_GAME_MEMORY_REQUIREMENT, 0 );
        ss_mut_fmt(
            &fatal_error_message,
            "Failed to load game memory requirement!{c}",
            0 );
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_LOAD_GAME_MEMORY_REQUIREMENT;
    }
    if( !application_initialize ) {
        ss_mut_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_LOAD_GAME_INITIALIZE, 0 );
        ss_mut_fmt(
            &fatal_error_message,
            "Failed to load game initialize function!{c}",
            0 );
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_LOAD_GAME_INITIALIZE;
    }
    if( !application_run ) {
        ss_mut_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_LOAD_GAME_RUN, 0 );
        ss_mut_fmt(
            &fatal_error_message,
            "Failed to load game run function!{c}",
            0 );
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_LOAD_GAME_RUN;
    }

    usize stack_size                       = 0;
    void* stack_buffer                     = NULL;
    usize renderer_subsystem_size          = 0;
    usize renderer_command_buffer_capacity = 0;
    usize renderer_command_buffer_size     = 0;
    usize application_memory_requirement   = 0;
    /* allocate stack */ {
#if defined(LD_LOGGING)
        stack_size += LOGGING_SUBSYSTEM_SIZE;
#endif
        application_memory_requirement =
            application_query_memory_requirement();

        stack_size += application_memory_requirement;

        stack_size += THREAD_SUBSYSTEM_SIZE;
        stack_size += EVENT_SUBSYSTEM_SIZE;
        stack_size += INPUT_SUBSYSTEM_SIZE;

        renderer_subsystem_size = renderer_subsystem_query_size( backend );
        stack_size += renderer_subsystem_size;

        renderer_command_buffer_capacity = RENDER_DATA_RENDER_COMMAND_MAX;
        renderer_command_buffer_size     =
            renderer_command_buffer_capacity * sizeof(RenderCommand);

        stack_size += renderer_command_buffer_size;

        usize stack_page_count = calculate_page_count( stack_size );
        stack_buffer = ldpage_alloc( stack_page_count, MEMORY_TYPE_ENGINE );
        stack_size   = stack_page_count * MEMORY_PAGE_SIZE;
        if( !stack_buffer ) {
            ss_mut_fmt(
                &fatal_error_title,
                "Fatal Error ({u8}){c}",
                CORE_ERROR_ENGINE_MEMORY_ALLOCATION, 0 );
            ss_mut_fmt(
                &fatal_error_message,
                "Out of Memory!{c}",
                0 );
            platform->fatal_message_box(
                fatal_error_title_buffer, fatal_error_message_buffer );
            return CORE_ERROR_ENGINE_MEMORY_ALLOCATION;
        }
    }

    StackAllocator stack;
    stack.size        = stack_size;
    stack.buffer      = stack_buffer;
    stack.memory_type = MEMORY_TYPE_ENGINE;
    stack.current     = 0;

#if defined(LD_LOGGING)
    /* initialize logging subsystem */ {
        void* logging_subsystem_buffer =
            stack_allocator_push( &stack, LOGGING_SUBSYSTEM_SIZE );
        if( !log_subsystem_init(
            LOG_LEVEL_ALL_VERBOSE,
            LOGGING_SUBSYSTEM_SIZE,
            logging_subsystem_buffer
        ) ) {
            println_err(
                LOG_COLOR_RED
                "[FATAL] Failed to initialize logging subsystem!"
                LOG_COLOR_RESET );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string(
                    CORE_ERROR_LOGGING_SUBSYSTEM_INITIALIZE ),
                "Failed to initialize logging subsystem!" );
            return CORE_ERROR_LOGGING_SUBSYSTEM_INITIALIZE;
        }
    }
#endif

    /* initialize event subsystem */ {
        void* event_subsystem_buffer =
            stack_allocator_push( &stack, EVENT_SUBSYSTEM_SIZE );
        if( !event_subsystem_init( event_subsystem_buffer ) ) {
            // TODO(alicia): error code + logging!
            return -1;
        }
    }

    /* initialize input subsystem */ {
        void* input_subsytem_buffer =
            stack_allocator_push( &stack, INPUT_SUBSYSTEM_SIZE );
        input_subsystem_init( input_subsytem_buffer );
    }

    /* initialize threading subsystem */ {
        void* threading_subsystem_buffer =
            stack_allocator_push( &stack, THREAD_SUBSYSTEM_SIZE );

        u32 thread_count = platform->query_info()->logical_processor_count;
        thread_count = max( thread_count, 1 );

        if( !thread_subsystem_init(
            thread_count,
            threading_subsystem_buffer
        ) ) {
            LOG_FATAL( "Failed to initialize thread subsystem!" );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string(
                    CORE_ERROR_THREAD_SUBSYSTEM_INITIALIZE ),
                "Failed to initialize thread subsystem!" );
            return CORE_ERROR_THREAD_SUBSYSTEM_INITIALIZE;
        }
    }

    PlatformSurface* surface = NULL;

    b32 application_close = false;
    b32 surface_is_active = true;
    RendererContext* renderer_context = NULL;
    /* initialize surface */ {
        #define SURFACE_CREATE_HIDDEN true
        #define SURFACE_RESIZEABLE    true
        surface = platform->surface.create(
            DEFAULT_RESOLUTION_WIDTH,
            DEFAULT_RESOLUTION_HEIGHT,
            "liquid engine",
            SURFACE_CREATE_HIDDEN,
            SURFACE_RESIZEABLE,
            backend );
        if( !surface ) {
            StringSlice last_error;
            platform->last_error(
                &last_error.len, (const char**)&last_error.buffer );
            LOG_FATAL( "{s}", last_error );
            LOG_FATAL( "Failed to create main surface!" );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string( CORE_ERROR_CREATE_SURFACE ),
                "Failed to create main surface!" );
            return CORE_ERROR_CREATE_SURFACE;
        }

        PlatformSurfaceCallbacks callbacks = {};
        callbacks.on_activate                 = on_activate;
        callbacks.on_activate_params          = &surface_is_active;
        callbacks.on_close                    = on_close;
        callbacks.on_close_params             = &application_close;
        callbacks.on_key                      = on_key;
        callbacks.on_mouse_button             = on_mouse_button;
        callbacks.on_mouse_move               = on_mouse_move;
        callbacks.on_mouse_wheel              = on_mouse_wheel;
        callbacks.on_resolution_change        = on_resolution_change;
        callbacks.on_resolution_change_params = &renderer_context;

        platform->surface.set_callbacks( surface, &callbacks );

        platform->surface.set_visible( surface, true );
    }

    RenderData render_data = {};
    /* initialize renderer */ {
        renderer_context =
            stack_allocator_push( &stack, renderer_subsystem_size );
        void* renderer_command_buffer =
            stack_allocator_push( &stack, renderer_command_buffer_size );

        render_data.command_buffer   = renderer_command_buffer;
        render_data.command_capacity = renderer_command_buffer_capacity;

        if( !renderer_subsystem_init(
            surface, backend,
            renderer_context
        ) ) {
            // TODO(alicia): error code + logging!
            return -1;
        }
    }
    graphics_subsystem_init( &render_data );

    void* application_memory =
        stack_allocator_push( &stack, application_memory_requirement );
    if( !application_initialize( application_memory ) ) {
        // TODO(alicia): error code + logging!
        return -1;
    }

    TimeStamp time = {};

    while( !application_close ) {
        input_swap();
        platform->surface.pump_events();

        if( !surface_is_active ) {
            continue;
        }

        if(
            input_is_key_down( KEY_ALT_LEFT ) ||
            input_is_key_down( KEY_ALT_RIGHT )
        ) {
            if( input_is_key_down( KEY_F4 ) ) {
                break;
            }
        }

        if( input_key_press( KEY_F11 ) ) {
            PlatformSurfaceMode mode =
                platform->surface.query_mode( surface );
            switch( mode ) {
                case PLATFORM_SURFACE_FULLSCREEN: {
                    platform->surface.set_mode(
                        surface, PLATFORM_SURFACE_WINDOWED );
                } break;
                case PLATFORM_SURFACE_WINDOWED: {
                    platform->surface.set_mode(
                        surface, PLATFORM_SURFACE_FULLSCREEN );
                } break;
            }
        }

        if( !application_run( time, application_memory ) ) {
            // TODO(alicia): error code + logging!
            return -1;
        }

        render_data.delta_time   = time.delta_seconds;
        render_data.elapsed_time = time.elapsed_seconds;
        render_data.frame_count  = time.frame_count;

        // if( !renderer_subsystem_on_draw(
        //     renderer_context,
        //     &render_data
        // ) ) {
        //     // TODO(alicia): error code + logging!
        //     return -1;
        // }

        render_data.command_count = 0;

        event_fire_end_of_frame();

        time.frame_count++;
        f64 elapsed_seconds  = platform->time.elapsed_seconds();
        time.delta_seconds   = elapsed_seconds - time.elapsed_seconds;
        time.elapsed_seconds = elapsed_seconds;
    }

    platform->surface.clear_callbacks( surface );

    platform->surface.destroy( surface );
    log_subsystem_shutdown();

    return CORE_SUCCESS;
}

internal void print_help(void) {
    println(
        "OVERVIEW: Liquid Engine Core {i}.{i}\n",
        LIQUID_ENGINE_VERSION_MAJOR, LIQUID_ENGINE_VERSION_MINOR );
    println( "USAGE: {cc} [options]\n", LIQUID_ENGINE_EXECUTABLE );
    println( "OPTIONS:" );
    println( "--libload=[string]     use a different game dll from default (debug only, default: '" GAME_LIBRARY_PATH_DEFAULT "')"  );
    println( "--opengl               use OpenGL renderer backend (default)" );
    println( "--vulkan               use Vulkan renderer backend" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
    println( "--metal                use Metal renderer backend (macos/ios only)" );
#endif
#if defined(LD_PLATFORM_WASM)
    println( "--webgl                use WebGL renderer backend (wasm only)" );
#endif
#if defined(LD_PLATFORM_WINDOWS)
    println( "--directx11            use DirectX11 renderer backend (win32 only)" );
    println( "--directx12            use DirectX12 renderer backend (win32 only)" );
    println( "--output-debug-string  enable output debug string (win32 only)" );
#endif
    println( "--help,-h              print this message" );
}

#include "core_generated_dependencies.inl"
