// * Description:  Liquid Engine Core Main
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 23, 2023
#include "defines.h"
#include <platform.h>
#include "core/graphics.h"
#include "core/strings.h"
#include "core/log.h"
#include "core/memoryf.h"
#include "core/thread.h"
#include "core/input.h"
#include "core/mathf.h"
#include "core/timer.h"
#include "core/engine.h"
#include "core/graphics/internal.h"

global b32 global_application_is_running = true;
global f32 global_resolution_scale       = 1.0f;
global TimeStamp* global_time_stamp      = NULL;

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
#define CORE_ERROR_RENDERER_SUBSYSTEM_INITIALIZE  (137)
#define CORE_ERROR_APPLICATION_INITIALIZE         (138)
#define CORE_ERROR_CREATE_SURFACE                 (139)
#define CORE_ERROR_APPLICATION_RUN                (140)
#define CORE_ERROR_RENDERER_DRAW                  (141)

internal no_inline void on_resolution_change(
    PlatformSurface* surface,
    i32 old_width, i32 old_height,
    i32 new_width, i32 new_height,
    void* user_params
) {
    unused( surface );
    unused( old_width + old_height );
    unused( user_params );

    ivec2 surface_dimensions  = { new_width, new_height };
    vec2  surface_dimensionsf = v2_iv2( surface_dimensions );
    ivec2 framebuffer_dimensions = iv2_v2( v2_mul(
        surface_dimensionsf, global_resolution_scale ) );
    renderer_subsystem_on_resize( surface_dimensions, framebuffer_dimensions );
}
internal no_inline void on_close(
    PlatformSurface* surface, void* user_params
) {
    unused(surface && user_params);
    global_application_is_running = false;
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
    input_subsystem_set_key( code, is_down );
}
internal no_inline void on_mouse_button(
    PlatformSurface* surface, b32 is_down,
    PlatformMouseCode code, void* user_params
) {
    unused(surface);
    unused(user_params);
    input_subsystem_set_mouse_button( code, is_down );
}
internal no_inline void on_mouse_move(
    PlatformSurface* surface,
    i32 x, i32 y, void* user_params
) {
    unused(user_params);

    i32 width, height;
    platform->surface.query_dimensions( surface, &width, &height );

    f32 x01 = (f32)x / (f32)width;
    f32 y01 = (f32)y / (f32)height;
    input_subsystem_set_mouse_position( x, y, x01, y01 );
}
internal no_inline void on_mouse_move_relative(
    PlatformSurface* surface,
    i32 x_rel, i32 y_rel, void* user_params
) {
    unused(surface);
    unused(user_params);

    input_subsystem_set_mouse_relative( x_rel, y_rel );
}
internal no_inline void on_mouse_wheel(
    PlatformSurface* surface,
    b32 is_horizontal, i32 value, void* user_params
) {
    unused(surface);
    unused(user_params);
    if( is_horizontal ) {
        input_subsystem_set_mouse_wheel_horizontal( value );
    } else {
        input_subsystem_set_mouse_wheel( value );
    }
}

#define DEFAULT_RESOLUTION_WIDTH  (800)
#define DEFAULT_RESOLUTION_HEIGHT (600)
#define DEFAULT_RESOLUTION_SCALE  (1.0f)
struct SettingsParse {
    i32 resolution_width;
    i32 resolution_height;
    f32 resolution_scale;
    enum RendererBackend backend;
};
internal struct SettingsParse parse_settings(void);

internal void print_help(void);
LD_API int core_init(
    int argc, char** argv,
    struct PlatformAPI* in_platform
) {
    assert( in_platform );
    platform = in_platform;

    char fatal_error_title_buffer[255];
    char fatal_error_message_buffer[255];
    StringSlice fatal_error_title =
        ss( 255, fatal_error_title_buffer );
    StringSlice fatal_error_message =
        ss( 255, fatal_error_message_buffer );

#if defined(LD_LOGGING)
    /* initialize logging subsystem */ {
        void* logging_subsystem_buffer = platform->memory.heap_alloc( kilobytes(1) );
        if( !logging_subsystem_buffer || !log_subsystem_init(
            LOG_LEVEL_ALL_VERBOSE,
            kilobytes(1),
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

    struct SettingsParse settings = parse_settings();

    i32 width  = settings.resolution_width;
    i32 height = settings.resolution_height;
    // TODO(alicia): make this modifiable through command line
    global_resolution_scale = settings.resolution_scale;
    RendererBackend backend = settings.backend;
    ss_const( game_library_path, GAME_LIBRARY_PATH_DEFAULT );

    /* parse arguments */ {
#if defined(LD_DEVELOPER_MODE)
        ss_const( libload, "--libload=" );
#endif
        ss_const( set_width, "--width=" );
        ss_const( set_height, "--height=" );
        ss_const( set_resolution_scale, "--resolution_scale=" );
        ss_const( help, "--help" );
        ss_const( h, "-h" );
#if defined(LD_PLATFORM_WINDOWS)

#if defined(LD_DEVELOPER_MODE)
        ss_const( output_debug_string, "--output-debug-string" );
        b32 enable_output_debug_string = false;
#endif
        ss_const( dx11,   "--directx11" );
        ss_const( dx12,   "--directx12" );
#endif
        ss_const( opengl, "--opengl" );
        ss_const( vulkan, "--vulkan" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
        ss_const( metal,  "--metal" );
#endif
#if defined(LD_PLATFORM_WASM)
        ss_const( webgl,  "--webgl" );
#endif

        b32 parse_error = false;

        for( int i = 1; i < argc; ++i ) {
            StringSlice current = ss_from_cstr( 0, argv[i] );

#if defined(LD_DEVELOPER_MODE)
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
#endif

            if( ss_find( &current, &set_width, NULL ) ) {
                if( current.len - set_width.len < 1 ) {
                    println_err(
                        LOG_COLOR_RED
                        "invalid width!"
                        LOG_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.buffer += set_width.len;
                current.len    -= set_width.len;
                u64 parse_result = 0;
                if( !ss_parse_uint( &current, &parse_result ) ) {
                    println_err(
                        LOG_COLOR_RED
                        "invalid width {s}!"
                        LOG_COLOR_RESET,
                        current
                    );
                    parse_error = true;
                    break;
                }
                width = max( parse_result, 1 );
                continue;
            }

            if( ss_find( &current, &set_height, NULL ) ) {
                if( current.len - set_height.len < 1 ) {
                    println_err(
                        LOG_COLOR_RED
                        "invalid height!"
                        LOG_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.buffer += set_height.len;
                current.len    -= set_height.len;
                u64 parse_result = 0;
                if( !ss_parse_uint( &current, &parse_result ) ) {
                    println_err(
                        LOG_COLOR_RED
                        "invalid height {s}!"
                        LOG_COLOR_RESET,
                        current
                    );
                    parse_error = true;
                    break;
                }
                height = max( parse_result, 1 );
                continue;
            }

            if( ss_find( &current, &set_resolution_scale, NULL ) ) {
                if( current.len - set_resolution_scale.len < 1 ) {
                    println_err(
                        LOG_COLOR_RED
                        "invalid resolution scale!"
                        LOG_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.buffer += set_resolution_scale.len;
                current.len    -= set_resolution_scale.len;
                f64 parse_result = 0.0;
                if( !ss_parse_float( &current, &parse_result ) ) {
                    println_err(
                        LOG_COLOR_RED
                        "invalid resolution scale {s}!"
                        LOG_COLOR_RESET,
                        current
                    );
                    parse_error = true;
                    break;
                }
                global_resolution_scale = max( parse_result, 0.1 );
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

#if defined(LD_DEVELOPER_MODE)
            if( ss_cmp( &current, &output_debug_string ) ) {
                enable_output_debug_string = true;
                continue;
            }
#endif

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

#if defined(LD_PLATFORM_WINDOWS) && defined(LD_DEVELOPER_MODE)
        if( enable_output_debug_string ) {
            log_subsystem_win32_enable_output_debug_string();
        }
#endif
    }

    if( !renderer_backend_is_supported( backend ) ) {
        ss_mut_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED, 0 );
        ss_mut_fmt(
            &fatal_error_message,
            "Renderer backend '{cc}' is not supported on current platform!{c}",
            renderer_backend_to_string( backend ), 0 );
        LOG_FATAL( "{s}", fatal_error_message );
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED;
    }

    LOG_NOTE( "Engine Configuration:" );
    LOG_NOTE( "Version:           {i}.{i}", LIQUID_ENGINE_VERSION_MAJOR, LIQUID_ENGINE_VERSION_MINOR );
#if defined(LD_PLATFORM_WINDOWS)
    ss_const(os, "win32");
#endif
#if defined(LD_PLATFORM_MACOS)
    ss_const(os, "macos");
#endif
#if defined(LD_PLATFORM_IOS)
    ss_const(os, "ios");
#endif
#if defined(LD_PLATFORM_ANDROID)
    ss_const(os, "android");
#endif
#if defined(LD_PLATFORM_LINUX)
    ss_const(os, "linux");
#endif
#if defined(LD_PLATFORM_WASM)
    ss_const(os, "wasm");
#endif
#if defined(LD_ARCH_32_BIT)
    #if defined(LD_ARCH_X86)
        ss_const(arch, "x86");
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            ss_const(arch, "arm little-endian 32-bit");
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            ss_const(arch, "arm big-endian 32-bit");
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        ss_const(arch, "wasm 32-bit");
    #endif
#endif
#if defined(LD_ARCH_64_BIT)
    #if defined(LD_ARCH_X86)
        ss_const(arch, "x86_64");
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            ss_const(arch, "arm little-endian 64-bit");
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            ss_const(arch, "arm big-endian 64-bit");
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        ss_const(arch, "wasm 64-bit");
    #endif
#endif
    LOG_NOTE( "Platform:          {s}, {s}", os, arch );
    LOG_NOTE( "Page Size:         {usize}", platform->query_info()->page_size );
    LOG_NOTE( "Game Library Path: {s}", game_library_path );
    LOG_NOTE( "Renderer Backend:  {cc}",
        renderer_backend_to_string( backend ) );
    LOG_NOTE( "Resolution:        {i}x{i}", width, height );
    LOG_NOTE( "Resolution Scale:  {f,.2}x", global_resolution_scale );

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
        LOG_FATAL("{s}", fatal_error_message);
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
        LOG_FATAL("{s}", fatal_error_message);
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
        LOG_FATAL("{s}", fatal_error_message);
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
        LOG_FATAL("{s}", fatal_error_message);
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
        application_memory_requirement =
            application_query_memory_requirement();

        stack_size += application_memory_requirement;

        stack_size += THREAD_SUBSYSTEM_SIZE;
        stack_size += input_subsystem_query_size();

        renderer_subsystem_size = renderer_subsystem_query_size( backend );
        stack_size += renderer_subsystem_size;

        renderer_command_buffer_capacity = 1024;
        renderer_command_buffer_size = list_calculate_memory_requirement(
            renderer_command_buffer_capacity, sizeof(struct RenderCommand) );

        stack_size += renderer_command_buffer_size;

        usize stack_page_count = memory_size_to_page_count( stack_size );
        stack_buffer = system_page_alloc( stack_page_count );
        stack_size   = page_count_to_memory_size( stack_page_count );

        LOG_INFO(
            "Stack Size: {usize} Stack Pages: {usize}",
            stack_size, stack_page_count );
        if( !stack_buffer ) {
            ss_mut_fmt(
                &fatal_error_title,
                "Fatal Error ({u8}){c}",
                CORE_ERROR_ENGINE_MEMORY_ALLOCATION, 0 );
            ss_mut_fmt(
                &fatal_error_message,
                "Out of Memory!{c}",
                0 );
            LOG_FATAL("{s}", fatal_error_message);
            platform->fatal_message_box(
                fatal_error_title_buffer, fatal_error_message_buffer );
            return CORE_ERROR_ENGINE_MEMORY_ALLOCATION;
        }

    }

    StackAllocator stack = {};
    stack.buffer      = stack_buffer;
    stack.buffer_size = stack_size;

    /* initialize input subsystem */ {
        void* input_subsytem_buffer =
            stack_allocator_push( &stack, input_subsystem_query_size() );
        input_subsystem_initialize( input_subsytem_buffer );
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

    b32 surface_is_active = true;
    /* initialize surface */ {
        #define SURFACE_CREATE_HIDDEN true
        #define SURFACE_RESIZEABLE    true
        surface = platform->surface.create(
            width, height,
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
        callbacks.on_key                      = on_key;
        callbacks.on_mouse_button             = on_mouse_button;
        callbacks.on_mouse_move               = on_mouse_move;
        callbacks.on_mouse_move_relative      = on_mouse_move_relative;
        callbacks.on_mouse_wheel              = on_mouse_wheel;
        callbacks.on_resolution_change        = on_resolution_change;

        platform->surface.set_callbacks( surface, &callbacks );
        platform->surface.set_visible( surface, true );
    }

    RenderData render_data = {};
    /* initialize renderer */ {
        void* renderer_subsystem_buffer =
            stack_allocator_push( &stack, renderer_subsystem_size );
        void* renderer_command_buffer =
            stack_allocator_push( &stack, renderer_command_buffer_size );

        render_data.list_commands = list_create(
            renderer_command_buffer_capacity,
            sizeof(struct RenderCommand),
            renderer_command_buffer );

        if( !renderer_subsystem_init(
            surface, backend,
            iv2_v2( v2_mul( v2( (f32)width, (f32)height ), global_resolution_scale ) ),
            &render_data,
            renderer_subsystem_buffer
        ) ) {
            LOG_FATAL( "Failed to initialize renderer subsystem!" );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string( CORE_ERROR_RENDERER_SUBSYSTEM_INITIALIZE ),
                "Failed to initialize renderer subsystem!" );
            return CORE_ERROR_RENDERER_SUBSYSTEM_INITIALIZE;
        }
    }

    void* application_memory =
        stack_allocator_push( &stack, application_memory_requirement );
    if( !application_initialize( application_memory ) ) {
        LOG_FATAL( "Failed to initialize application!" );
        platform->fatal_message_box(
            "Fatal Error "
            macro_value_to_string( CORE_ERROR_APPLICATION_INITIALIZE ),
            "Failed to initialize application!" );
        return CORE_ERROR_APPLICATION_INITIALIZE;
    }

    TimeStamp time  = {};
    time.time_scale = 1.0f;
    global_time_stamp = &time;

    while( global_application_is_running ) {
        input_subsystem_swap_state();
        input_subsystem_update_gamepads();
        platform->surface.pump_events();

        if( !surface_is_active ) {
            continue;
        }

        if( input_key( KEY_ALT_LEFT ) || input_key( KEY_ALT_RIGHT ) ) {
            if( input_key( KEY_F4 ) ) {
                break;
            }
        }

        if( input_key_down( KEY_F11 ) ) {
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
            LOG_FATAL( "Failed to run application!" );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string( CORE_ERROR_APPLICATION_RUN ),
                "Failed to run application!" );
            return CORE_ERROR_APPLICATION_RUN;
        }

        render_data.time = time;

        if( !renderer_subsystem_draw() ) {
            LOG_FATAL( "Renderer failed!" );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string( CORE_ERROR_RENDERER_DRAW ),
                "Renderer failed!" );
            return CORE_ERROR_RENDERER_DRAW;
        }

        list_clear( render_data.list_commands );

        if( input_is_mouse_locked() ) {
            platform->surface.center_cursor( surface );
        }

        time.frame_count++;
        f64 elapsed_seconds  = platform->time.elapsed_seconds();
        time.delta_seconds   =
            (time.unscaled_delta_seconds = elapsed_seconds - time.elapsed_seconds) *
            time.time_scale;
        time.elapsed_seconds = elapsed_seconds;
    }

    platform->surface.clear_callbacks( surface );

    renderer_subsystem_shutdown();
    platform->surface.destroy( surface );
    log_subsystem_shutdown();

    return CORE_SUCCESS;
}

internal void print_help(void) {

#if defined(LD_DEVELOPER_MODE)
        const char* version = " Debug";
#else
        const char* version = "";
#endif

    println(
        "OVERVIEW: Liquid Engine Core {i}.{i}{cc}\n",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR, version
    );
    println( "USAGE: {cc} [options]\n", LIQUID_ENGINE_EXECUTABLE );
    println( "OPTIONS:" );
#if defined(LD_DEVELOPER_MODE)
    println( "--libload=[string]         use a different game dll from default (developer mode only, default='" GAME_LIBRARY_PATH_DEFAULT "')"  );
    println( "--output-debug-string      enable output debug string (developer mode only, win32 only, default=false)" );
#endif
    println( "--width=[integer]          overwrite screen width (default=settings.ini)" );
    println( "--height=[integer]         overwrite screen height (default=settings.ini)" );
    println( "--resolution_scale=[float] overwrite resolution scale (default=settings.ini)" );
    println( "--opengl                   use OpenGL renderer backend (default)" );
    println( "--vulkan                   use Vulkan renderer backend" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
    println( "--metal                    use Metal renderer backend (macos/ios only)" );
#endif
#if defined(LD_PLATFORM_WASM)
    println( "--webgl                    use WebGL renderer backend (wasm only)" );
#endif
#if defined(LD_PLATFORM_WINDOWS)
    println( "--directx11                use DirectX11 renderer backend (win32 only)" );
    println( "--directx12                use DirectX12 renderer backend (win32 only)" );
#endif
    println( "--help,-h                  print this message" );
}

#define ENGINE_SETTINGS_BUFFER_CAPACITY (kilobytes(1))
global char ENGINE_SETTINGS_BUFFER[ENGINE_SETTINGS_BUFFER_CAPACITY] = {};

internal struct SettingsParse parse_settings(void) {
    struct SettingsParse parse_result = {};

    // TODO(alicia): load from user settings.ini
    PlatformFileFlags flags =
        PLATFORM_FILE_READ |
        PLATFORM_FILE_READ |
        PLATFORM_FILE_ONLY_EXISTING;
    #define SETTINGS_PATH "./settings.ini"

    PlatformFile* settings_file = platform->io.file_open(
        SETTINGS_PATH, flags | PLATFORM_FILE_ONLY_EXISTING );

    if( !settings_file ) {
        settings_file = platform->io.file_open(
            SETTINGS_PATH, PLATFORM_FILE_WRITE );
        // TODO(alicia): logging!
        assert( settings_file );

        StringSlice default_settings;
        default_settings.buffer   = ENGINE_SETTINGS_BUFFER;
        default_settings.capacity = ENGINE_SETTINGS_BUFFER_CAPACITY;
        default_settings.len      = 0;

        ss_mut_fmt( &default_settings, "[graphics] \n" );
        ss_mut_fmt( &default_settings, "width            = {i} \n", DEFAULT_RESOLUTION_WIDTH );
        ss_mut_fmt( &default_settings, "height           = {i} \n", DEFAULT_RESOLUTION_HEIGHT );
        ss_mut_fmt( &default_settings, "resolution_scale = {f,.1} \n", DEFAULT_RESOLUTION_SCALE );
        ss_mut_fmt( &default_settings, "backend          = opengl \n" );

        b32 write_result = platform->io.file_write(
            settings_file, default_settings.len, default_settings.buffer );

        // TODO(alicia): logging!
        assert( write_result );

        platform->io.file_close( settings_file );

        settings_file = platform->io.file_open( SETTINGS_PATH, flags );

        // TODO(alicia): logging!
        assert( settings_file );
    }

    parse_result.resolution_width  = DEFAULT_RESOLUTION_WIDTH;
    parse_result.resolution_height = DEFAULT_RESOLUTION_HEIGHT;
    parse_result.resolution_scale  = DEFAULT_RESOLUTION_SCALE;
    parse_result.backend           = RENDERER_BACKEND_OPENGL;

    usize settings_file_size = platform->io.file_query_size( settings_file );
    if( !settings_file_size ) {
        platform->io.file_close( settings_file );
        return parse_result;
    }

    char* settings_file_buffer = system_alloc( settings_file_size );
    // TODO(alicia): logging
    assert( settings_file_buffer );

    // TODO(alicia): logging
    assert( platform->io.file_read(
        settings_file,
        settings_file_size, settings_file_size,
        settings_file_buffer ) );

    StringSlice settings = {
        settings_file_buffer, settings_file_size, settings_file_size
    };

    enum Section : u32 {
        SECTION_UNKNOWN,
        SECTION_GRAPHICS
    };
    enum Section section = SECTION_UNKNOWN;
    unused(section);

    ss_const( token_section_resolution, "[graphics]" );
    ss_const( token_width, "width" );
    ss_const( token_height, "height" );
    ss_const( token_resolution_scale, "resolution_scale" );
    ss_const( token_backend, "backend" );

    ss_const( token_opengl, "opengl" );
    ss_const( token_vulkan, "vulkan" );
    ss_const( token_metal, "metal" );
    ss_const( token_webgl, "webgl" );
    ss_const( token_directx11, "directx11" );
    ss_const( token_directx12, "directx12" );

    usize eol = 0;
    StringSlice line = ss_clone( &settings );

    while( ss_find_char( &line, '\n', &eol ) ) {
        StringSlice temp = ss_clone( &line );
        temp.len = eol + 1;

        switch( temp.buffer[0] ) {
            case '[': {
                if( ss_find( &temp, &token_section_resolution, NULL ) ) {
                    section = SECTION_GRAPHICS;
                }
            } break;
            case ';': {
                section = SECTION_UNKNOWN;
            } break;
        }

        if( section == SECTION_UNKNOWN ) {
            goto skip;
        }

        switch( section ) {
            case SECTION_GRAPHICS: {
                if( ss_find( &temp, &token_width, NULL ) ) {
                    StringSlice number;
                    number.buffer = temp.buffer + token_width.len;
                    number.len    = temp.len    - token_width.len;

                    for( usize i = 0; i < number.len; ++i ) {
                        char current = number.buffer[i];

                        if( char_is_digit( current ) ) {
                            number.buffer += i;
                            number.len    -= i;

                            u64 int_parse = 0;
                            if( ss_parse_uint( &number, &int_parse ) ) {
                                parse_result.resolution_width =
                                    max( int_parse, 1 );
                            }

                            break;
                        }
                    }
                } else if( ss_find( &temp, &token_height, NULL ) ) {
                    StringSlice number;
                    number.buffer = temp.buffer + token_height.len;
                    number.len    = temp.len    - token_height.len;

                    for( usize i = 0; i < number.len; ++i ) {
                        char current = number.buffer[i];

                        if( char_is_digit( current ) ) {
                            number.buffer += i;
                            number.len    -= i;

                            u64 int_parse = 0;
                            if( ss_parse_uint( &number, &int_parse ) ) {
                                parse_result.resolution_height =
                                    max( int_parse, 1 );
                            }

                            break;
                        }
                    }
                } else if( ss_find( &temp, &token_resolution_scale, NULL ) ) {
                    StringSlice number;
                    number.buffer = temp.buffer + token_resolution_scale.len;
                    number.len    = temp.len    - token_resolution_scale.len;

                    for( usize i = 0; i < number.len; ++i ) {
                        char current = number.buffer[i];

                        if( char_is_digit( current ) ) {
                            number.buffer += i;
                            number.len    -= i;

                            while(
                                number.len &&
                                !char_is_digit(number.buffer[number.len - 1])
                            ) {
                                ss_mut_pop( &number, NULL );
                            }

                            f64 float_parse = 0.0;
                            if( ss_parse_float( &number, &float_parse ) ) {
                                parse_result.resolution_scale =
                                    max( float_parse, 0.1 );
                            }

                            break;
                        }
                    }
                } else if( ss_find( &temp, &token_backend, NULL ) ) {
                    StringSlice backend;
                    backend.buffer = temp.buffer + token_backend.len;
                    backend.len    = temp.len    - token_backend.len;

                    if( ss_find( &backend, &token_opengl, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_OPENGL;
                    } else if( ss_find( &backend, &token_vulkan, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_VULKAN;
                    } else if( ss_find( &backend, &token_metal, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_METAL;
                    } else if( ss_find( &backend, &token_webgl, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_WEBGL;
                    } else if( ss_find( &backend, &token_directx11, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_DX11;
                    } else if( ss_find( &backend, &token_directx12, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_DX12;
                    }
                }
            } break;
            default: break;
        }

    skip:
        line.buffer += temp.len;
        line.len    -= temp.len;
    }

    if( !renderer_backend_is_supported( parse_result.backend ) ) {
        // TODO(alicia): replace with default backend for platform.
        parse_result.backend = RENDERER_BACKEND_OPENGL;
    }

    system_free( settings_file_buffer, settings_file_size );
    platform->io.file_close( settings_file );
    return parse_result;
}

LD_API void engine_exit(void) {
    LOG_NOTE( "Application requested program to exit." );
    global_application_is_running = false;
}
LD_API f32 engine_query_time_scale(void) {
    return global_time_stamp->time_scale;
}
LD_API void engine_set_time_scale( f32 scale ) {
    global_time_stamp->time_scale = max( scale, 0.001f );
}

#include "core_generated_dependencies.inl"
