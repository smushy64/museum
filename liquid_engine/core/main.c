// * Description:  Liquid Engine Core Main
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 23, 2023
#include "defines.h"
#include <platform.h>
#include "core/graphics.h"
#include "core/audio.h"
#include "core/string.h"
#include "core/logging.h"
#include "core/memory.h"
#include "core/thread.h"
#include "core/input.h"
#include "core/math.h"
#include "core/time.h"
#include "core/engine.h"
#include "core/collections.h"
#include "core/graphics/internal.h"

#define DEFAULT_RESOLUTION_WIDTH    (800)
#define DEFAULT_RESOLUTION_HEIGHT   (600)
#define DEFAULT_RESOLUTION_SCALE    (1.0f)
#define DEFAULT_AUDIO_VOLUME_MASTER (0.5f)
#define DEFAULT_AUDIO_VOLUME_MUSIC  (1.0f)
#define DEFAULT_AUDIO_VOLUME_SFX    (1.0f)

// TODO(alicia): defaults for other platforms :)
#define DEFAULT_RENDERER_BACKEND (RENDERER_BACKEND_OPENGL)

struct SettingsParse {
    i32 resolution_width;
    i32 resolution_height;
    f32 resolution_scale;
    f32 audio_volume_master;
    f32 audio_volume_music;
    f32 audio_volume_sfx;
    enum RendererBackend backend;
};
internal force_inline
struct SettingsParse ___settings_parse_default(void) {
    struct SettingsParse result = {};
    result.resolution_width  = DEFAULT_RESOLUTION_WIDTH;
    result.resolution_height = DEFAULT_RESOLUTION_HEIGHT;
    result.resolution_scale  = DEFAULT_RESOLUTION_SCALE;
    result.audio_volume_master = DEFAULT_AUDIO_VOLUME_MASTER;
    result.audio_volume_music  = DEFAULT_AUDIO_VOLUME_MUSIC;
    result.audio_volume_sfx    = DEFAULT_AUDIO_VOLUME_SFX;

    result.backend = DEFAULT_RENDERER_BACKEND;
    return result;
}

global b32 global_application_is_running = true;
global f32 global_resolution_scale       = 1.0f;

#define LOGGING_SUBSYSTEM_SIZE (kilobytes(1))

typedef usize ApplicationQueryMemoryRequirementFN(void);
typedef b32 ApplicationInitializeFN( void* memory );
typedef b32 ApplicationRunFN( void* memory );

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
#define CORE_ERROR_AUDIO_SUBSYSTEM_INITIALIZE     (138)
#define CORE_ERROR_APPLICATION_INITIALIZE         (139)
#define CORE_ERROR_CREATE_SURFACE                 (140)
#define CORE_ERROR_APPLICATION_RUN                (141)
#define CORE_ERROR_RENDERER_DRAW                  (142)

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
    note_log( "Application is shutting down." );
}
internal no_inline void on_activate(
    PlatformSurface* surface, b32 is_active, void* user_params
) {
    unused(surface);
    b32* surface_is_active = user_params;
    *surface_is_active     = is_active;
    note_log( "Surface {cc}", is_active ? "is active." : "is inactive." );

    if( is_active ) {
        audio_subsystem_start();
    } else {
        audio_subsystem_stop();
    }
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

internal b32 parse_settings( struct SettingsParse* out_parse_result );

global PlatformSurface* ENGINE_SURFACE = NULL;

#define DEFAULT_LOGGING_FILE_PATH "./museum-logging.txt"

internal void print_help(void);
LD_API int core_init(
    int argc, char** argv,
    struct PlatformAPI* in_platform
) {
    assert( in_platform );
    platform = in_platform;

    time_subsystem_initialize();

    char fatal_error_title_buffer[255];
    char fatal_error_message_buffer[255];
    StringSlice fatal_error_title =
        string_slice( 255, fatal_error_title_buffer );
    StringSlice fatal_error_message =
        string_slice( 255, fatal_error_message_buffer );

#if defined(LD_LOGGING)

    PlatformFile* logging_file = platform->io.file_open(
        DEFAULT_LOGGING_FILE_PATH, PLATFORM_FILE_WRITE | PLATFORM_FILE_SHARE_READ );

    if( !logging_file ) {
        println_err( "[FATAL] Failed to open logging file!" );
        return CORE_ERROR_LOGGING_SUBSYSTEM_INITIALIZE;
    }

    logging_subsystem_initialize( logging_file );
    logging_set_level( LOGGING_LEVEL_ALL );

#endif

    struct SettingsParse settings = ___settings_parse_default();
    if( !parse_settings( &settings ) ) {
        return CORE_ERROR_PARSE;
    }

    f32 audio_volume_master = settings.audio_volume_master;
    f32 audio_volume_music  = settings.audio_volume_music;
    f32 audio_volume_sfx    = settings.audio_volume_sfx;
    i32 width  = settings.resolution_width;
    i32 height = settings.resolution_height;
    global_resolution_scale = settings.resolution_scale;
    RendererBackend backend = settings.backend;
    string_slice_const( game_library_path, GAME_LIBRARY_PATH_DEFAULT );

    /* parse arguments */ {
        string_slice_const( set_master_volume, "--master-volume=" );
        string_slice_const( set_music_volume, "--music-volume=" );
        string_slice_const( set_sfx_volume, "--sfx-volume=" );
        string_slice_const( set_width, "--width=" );
        string_slice_const( set_height, "--height=" );
        string_slice_const( set_resolution_scale, "--resolution_scale=" );
        string_slice_const( help, "--help" );
        string_slice_const( h, "-h" );

#if defined(LD_DEVELOPER_MODE)
        string_slice_const( libload, "--libload=" );
        string_slice_const( clear_log, "--clear-log" );
#endif
#if defined(LD_PLATFORM_WINDOWS)

#if defined(LD_DEVELOPER_MODE)
        string_slice_const( output_debug_string, "--output-debug-string" );
        b32 enable_output_debug_string = false;
#endif
        string_slice_const( dx11,   "--directx11" );
        string_slice_const( dx12,   "--directx12" );
#endif
        string_slice_const( opengl, "--opengl" );
        string_slice_const( vulkan, "--vulkan" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
        string_slice_const( metal,  "--metal" );
#endif
#if defined(LD_PLATFORM_WASM)
        string_slice_const( webgl,  "--webgl" );
#endif

        b32 parse_error = false;

        for( int i = 1; i < argc; ++i ) {
            StringSlice current = string_slice_from_cstr( 0, argv[i] );

#if defined(LD_DEVELOPER_MODE)
            if( string_slice_find( &current, &libload, NULL ) ) {
                if( current.len - libload.len < 1 ) {
                    StringSlice path = string_slice_clone( &current );
                    path.buffer += libload.len;
                    path.len    -= libload.len;
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid game library path: {s}"
                        CONSOLE_COLOR_RESET,
                        path
                    );
                    parse_error = true;
                    break;
                }
                game_library_path.buffer = current.buffer + libload.len;
                game_library_path.len    = current.len - libload.len;
                continue;
            }
            if( string_slice_cmp( &current, &clear_log ) ) {
                logging_subsystem_detach_file();
                platform->io.file_close( logging_file );

                if( !platform->io.file_delete_by_path( DEFAULT_LOGGING_FILE_PATH ) ) {
                    warn_log( "Unable to delete logging file!" );
                }

                logging_file = platform->io.file_open(
                    DEFAULT_LOGGING_FILE_PATH,
                    PLATFORM_FILE_WRITE | PLATFORM_FILE_SHARE_READ );

                read_write_fence();
                logging_subsystem_attach_file( logging_file );

                note_log( "Logging file cleared!" );
                continue;
            }
#endif

            if( string_slice_find( &current, &set_width, NULL ) ) {
                if( current.len - set_width.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid width!"
                        CONSOLE_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.buffer += set_width.len;
                current.len    -= set_width.len;
                u64 parse_result = 0;
                if( !string_slice_parse_uint( &current, &parse_result ) ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid width {s}!"
                        CONSOLE_COLOR_RESET,
                        current
                    );
                    parse_error = true;
                    break;
                }
                width = max( parse_result, 1 );
                continue;
            }

            if( string_slice_find( &current, &set_height, NULL ) ) {
                if( current.len - set_height.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid height!"
                        CONSOLE_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.buffer += set_height.len;
                current.len    -= set_height.len;
                u64 parse_result = 0;
                if( !string_slice_parse_uint( &current, &parse_result ) ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid height {s}!"
                        CONSOLE_COLOR_RESET,
                        current
                    );
                    parse_error = true;
                    break;
                }
                height = max( parse_result, 1 );
                continue;
            }

            if( string_slice_find( &current, &set_resolution_scale, NULL ) ) {
                if( current.len - set_resolution_scale.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid resolution scale!"
                        CONSOLE_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.buffer += set_resolution_scale.len;
                current.len    -= set_resolution_scale.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( &current, &parse_result ) ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid resolution scale '{s}'!"
                        CONSOLE_COLOR_RESET,
                        current
                    );
                    parse_error = true;
                    break;
                }
                global_resolution_scale = max( parse_result, 0.1 );
                continue;
            }

            if( string_slice_find( &current, &set_master_volume, NULL ) ) {
                if( current.len - set_master_volume.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid master volume!"
                        CONSOLE_COLOR_RESET );
                    parse_error = true;
                    break;
                }
                current.buffer += set_master_volume.len;
                current.len    -= set_master_volume.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( &current, &parse_result ) ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid master volume '{s}'!"
                        CONSOLE_COLOR_RESET, current );
                    parse_error = true;
                    break;
                }
                audio_volume_master = clamp01( parse_result );
                continue;
            }

            if( string_slice_find( &current, &set_music_volume, NULL ) ) {
                if( current.len - set_music_volume.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid music volume!"
                        CONSOLE_COLOR_RESET );
                    parse_error = true;
                    break;
                }
                current.buffer += set_music_volume.len;
                current.len    -= set_music_volume.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( &current, &parse_result ) ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid music volume '{s}'!"
                        CONSOLE_COLOR_RESET, current );
                    parse_error = true;
                    break;
                }
                audio_volume_music = clamp01( parse_result );
                continue;
            }

            if( string_slice_find( &current, &set_sfx_volume, NULL ) ) {
                if( current.len - set_sfx_volume.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid sfx volume!"
                        CONSOLE_COLOR_RESET );
                    parse_error = true;
                    break;
                }
                current.buffer += set_sfx_volume.len;
                current.len    -= set_sfx_volume.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( &current, &parse_result ) ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid sfx volume '{s}'!"
                        CONSOLE_COLOR_RESET, current );
                    parse_error = true;
                    break;
                }
                audio_volume_sfx = clamp01( parse_result );
                continue;
            }

            if( string_slice_cmp( &current, &opengl ) ) {
                backend = RENDERER_BACKEND_OPENGL;
                continue;
            }

            if( string_slice_cmp( &current, &vulkan ) ) {
                backend = RENDERER_BACKEND_VULKAN;
                continue;
            }

#if defined(LD_PLATFORM_WINDOWS)

#if defined(LD_DEVELOPER_MODE)
            if( string_slice_cmp( &current, &output_debug_string ) ) {
                enable_output_debug_string = true;
                continue;
            }
#endif

            if( string_slice_cmp( &current, &dx11 ) ) {
                backend = RENDERER_BACKEND_DX11;
                continue;
            }
            if( string_slice_cmp( &current, &dx12 ) ) {
                backend = RENDERER_BACKEND_DX12;
                continue;
            }
#endif
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
            if( string_slice_cmp( &current, &metal ) ) {
                backend = RENDERER_BACKEND_METAL;
                continue;
            }
#endif
#if defined(LD_PLATFORM_WASM)
            if( string_slice_cmp( &current, &webgl ) ) {
                backend = RENDERER_BACKEND_WEBGL;
                continue;
            }
#endif
            if( string_slice_cmp( &current, &help ) || string_slice_cmp( &current, &h ) ) {
                print_help();
                return 0;
            }

            println_err(
                CONSOLE_COLOR_RED
                "unrecognized argument: {s}"
                CONSOLE_COLOR_RESET,
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
            logging_set_output_debug_string_enabled( true );
        }
#endif
    }

    if( !renderer_backend_is_supported( backend ) ) {
        string_slice_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED, 0 );
        string_slice_fmt(
            &fatal_error_message,
            "Renderer backend '{cc}' is not supported on current platform!{c}",
            renderer_backend_to_string( backend ), 0 );
        fatal_log( "{s}", fatal_error_message );
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED;
    }

    note_log( "Engine Configuration:" );
    note_log( "Version:           {i}.{i}", LIQUID_ENGINE_VERSION_MAJOR, LIQUID_ENGINE_VERSION_MINOR );
#if defined(LD_PLATFORM_WINDOWS)
    string_slice_const(os, "win32");
#endif
#if defined(LD_PLATFORM_MACOS)
    string_slice_const(os, "macos");
#endif
#if defined(LD_PLATFORM_IOS)
    string_slice_const(os, "ios");
#endif
#if defined(LD_PLATFORM_ANDROID)
    string_slice_const(os, "android");
#endif
#if defined(LD_PLATFORM_LINUX)
    string_slice_const(os, "linux");
#endif
#if defined(LD_PLATFORM_WASM)
    string_slice_const(os, "wasm");
#endif
#if defined(LD_ARCH_32_BIT)
    #if defined(LD_ARCH_X86)
        string_slice_const(arch, "x86");
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            string_slice_const(arch, "arm little-endian 32-bit");
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            string_slice_const(arch, "arm big-endian 32-bit");
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        string_slice_const(arch, "wasm 32-bit");
    #endif
#endif
#if defined(LD_ARCH_64_BIT)
    #if defined(LD_ARCH_X86)
        string_slice_const(arch, "x86_64");
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            string_slice_const(arch, "arm little-endian 64-bit");
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            string_slice_const(arch, "arm big-endian 64-bit");
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        string_slice_const(arch, "wasm 64-bit");
    #endif
#endif
    note_log( "Platform:          {s}, {s}", os, arch );
    note_log( "Page Size:         {usize}", platform->query_info()->page_size );
    note_log( "Game Library Path: {s}", game_library_path );
    note_log( "Renderer Backend:  {cc}",
        renderer_backend_to_string( backend ) );
    note_log( "Resolution:        {i}x{i}", width, height );
    note_log( "Resolution Scale:  {f,.2}x", global_resolution_scale );

    PlatformLibrary* game =
        platform->library.open( game_library_path.buffer );
    if( !game ) {
        string_slice_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_OPEN_GAME_LIBRARY, 0 );
        string_slice_fmt(
            &fatal_error_message,
            "Failed to load game library! Game library path: {s}{c}",
            game_library_path, 0 );
        fatal_log("{s}", fatal_error_message);
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
        string_slice_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_LOAD_GAME_MEMORY_REQUIREMENT, 0 );
        string_slice_fmt(
            &fatal_error_message,
            "Failed to load game memory requirement!{c}",
            0 );
        fatal_log("{s}", fatal_error_message);
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_LOAD_GAME_MEMORY_REQUIREMENT;
    }
    if( !application_initialize ) {
        string_slice_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_LOAD_GAME_INITIALIZE, 0 );
        string_slice_fmt(
            &fatal_error_message,
            "Failed to load game initialize function!{c}",
            0 );
        fatal_log("{s}", fatal_error_message);
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_LOAD_GAME_INITIALIZE;
    }
    if( !application_run ) {
        string_slice_fmt(
            &fatal_error_title,
            "Fatal Error ({u8}){c}",
            CORE_ERROR_LOAD_GAME_RUN, 0 );
        string_slice_fmt(
            &fatal_error_message,
            "Failed to load game run function!{c}",
            0 );
        fatal_log("{s}", fatal_error_message);
        platform->fatal_message_box(
            fatal_error_title_buffer, fatal_error_message_buffer );
        return CORE_ERROR_LOAD_GAME_RUN;
    }

    if( !audio_subsystem_initialize() ) {
        fatal_log( "Failed to initialize audio subsystem!" );
        platform->fatal_message_box(
            "Fatal Error "
            macro_value_to_string( CORE_ERROR_AUDIO_SUBSYSTEM_INITIALIZE ),
            "Failed to initialize audio subsystem!" );
        return CORE_ERROR_AUDIO_SUBSYSTEM_INITIALIZE;
    }

    usize audio_subsystem_memory_requirement =
        audio_subsystem_query_memory_requirement();

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
        stack_size += audio_subsystem_memory_requirement;

        renderer_subsystem_size = renderer_subsystem_query_size( backend );
        stack_size += renderer_subsystem_size;

        renderer_command_buffer_capacity = 1024;
        renderer_command_buffer_size = list_calculate_memory_requirement(
            renderer_command_buffer_capacity, sizeof(struct RenderCommand) );

        stack_size += renderer_command_buffer_size;

        usize stack_page_count = memory_size_to_page_count( stack_size );
        stack_buffer = system_page_alloc( stack_page_count );
        stack_size   = page_count_to_memory_size( stack_page_count );

        info_log(
            "Stack Size: {usize}({f,.2,b}) Stack Pages: {usize}",
            stack_size, (f64)stack_size, stack_page_count );
        if( !stack_buffer ) {
            string_slice_fmt(
                &fatal_error_title,
                "Fatal Error ({u8}){c}",
                CORE_ERROR_ENGINE_MEMORY_ALLOCATION, 0 );
            string_slice_fmt(
                &fatal_error_message,
                "Out of Memory!{c}",
                0 );
            fatal_log("{s}", fatal_error_message);
            platform->fatal_message_box(
                fatal_error_title_buffer, fatal_error_message_buffer );
            return CORE_ERROR_ENGINE_MEMORY_ALLOCATION;
        }

    }

    StackAllocator stack = {};
    stack.buffer      = stack_buffer;
    stack.buffer_size = stack_size;

    void* audio_subsystem_buffer =
        stack_allocator_push( &stack, audio_subsystem_memory_requirement );
    audio_subsystem_submit_buffer_memory( audio_subsystem_buffer );
    audio_set_master_volume( audio_volume_master );
    audio_set_music_volume( audio_volume_music );
    audio_set_sfx_volume( audio_volume_sfx );

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
            fatal_log( "Failed to initialize thread subsystem!" );
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
            fatal_log( "{s}", last_error );
            fatal_log( "Failed to create main surface!" );
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

    ENGINE_SURFACE = surface;

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
            fatal_log( "Failed to initialize renderer subsystem!" );
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
        fatal_log( "Failed to initialize application!" );
        platform->fatal_message_box(
            "Fatal Error "
            macro_value_to_string( CORE_ERROR_APPLICATION_INITIALIZE ),
            "Failed to initialize application!" );
        return CORE_ERROR_APPLICATION_INITIALIZE;
    }

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
            engine_toggle_fullscreen();
        }

        if( !application_run( application_memory ) ) {
            fatal_log( "Failed to run application!" );
            platform->fatal_message_box(
                "Fatal Error "
                macro_value_to_string( CORE_ERROR_APPLICATION_RUN ),
                "Failed to run application!" );
            return CORE_ERROR_APPLICATION_RUN;
        }

        audio_subsystem_output();

        if( !renderer_subsystem_draw() ) {
            fatal_log( "Renderer failed!" );
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

        time_subsystem_update();
    }

    audio_subsystem_shutdown();

    platform->surface.clear_callbacks( surface );

    renderer_subsystem_shutdown();
    platform->surface.destroy( surface );

#if defined(LD_LOGGING)
    platform->io.file_close( logging_file );
#endif

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
    println( "--clear-log                clear museum-logging.txt (developer mode only)" );
#endif /* developer mode */
    println( "--width=[integer]          overwrite screen width (default=settings.ini)" );
    println( "--height=[integer]         overwrite screen height (default=settings.ini)" );
    println( "--resolution_scale=[float] overwrite resolution scale (default=settings.ini)" );
    println( "--master-volume=[float]    overwrite master volume (default=settings.ini)" );
    println( "--music-volume=[float]     overwrite music volume (default=settings.ini)" );
    println( "--sfx-volume=[float]       overwrite sfx volume (default=settings.ini)" );
    println( "--opengl                   use OpenGL renderer backend (default)" );
    println( "--vulkan                   use Vulkan renderer backend" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
    println( "--metal                    use Metal renderer backend (macos/ios only)" );
#endif /* macos or ios */
#if defined(LD_PLATFORM_WASM)
    println( "--webgl                    use WebGL renderer backend (wasm only)" );
#endif /* wasm */
#if defined(LD_PLATFORM_WINDOWS)

    #if defined(LD_DEVELOPER_MODE)
        println( "--output-debug-string      enable output debug string (developer mode only, win32 only, default=false)" );
    #endif /* win32 developer mode */

    println( "--directx11                use DirectX11 renderer backend (win32 only)" );
    println( "--directx12                use DirectX12 renderer backend (win32 only)" );
#endif /* win32 */
    println( "--help,-h                  print this message" );
}

internal b32 ___settings_parse_uint(
    StringSlice* line, StringSlice* token, u64* out_result
) {
    StringSlice number = {};
    number.buffer = line->buffer + token->len;
    number.len    = line->len    - token->len;

    for( usize i = 0; i < number.len; ++i ) {
        char current = number.buffer[i];

        if( char_is_digit( current ) ) {
            number.buffer += i;
            number.len    -= i;

            u64 int_parse = 0;
            if( !string_slice_parse_uint( &number, &int_parse ) ) {
                return false;
            }

            *out_result = int_parse;
            break;
        }
    }

    return true;
}
internal b32 ___settings_parse_float(
    StringSlice* line, StringSlice* token, f64* out_result
) {
    StringSlice number = {};
    number.buffer = line->buffer + token->len;
    number.len    = line->len    - token->len;

    for( usize i = 0; i < number.len; ++i ) {
        char current = number.buffer[i];

        if( char_is_digit( current ) ) {
            number.buffer += i;
            number.len    -= i;

            while( number.len && !char_is_digit( number.buffer[number.len - 1] ) ) {
                string_slice_pop( &number, NULL );
            }

            f64 float_parse = 0.0;
            if( !string_slice_parse_float( &number, &float_parse ) ) {
                return false;
            }

            *out_result = float_parse;
            break;
        }
    }

    return true;
}
enum Section : u32 {
    SECTION_UNKNOWN,
    SECTION_GRAPHICS,
    SECTION_AUDIO
};

internal
b32 parse_settings( struct SettingsParse* out_parse_result ) {
    struct SettingsParse parse_result = {};

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
        if( !settings_file ) {
            fatal_log( "Failed to open settings file at all!" );
            return false;
        }

        string_slice_mut_capacity( default_settings, 128 );

        #define settings_output_string( format, ... )\
            default_settings.len = 0;\
            string_slice_fmt( &default_settings, format, ##__VA_ARGS__ );\
            platform->io.file_write(\
                settings_file, default_settings.len, default_settings.buffer )

        settings_output_string( "[graphics] \n" );
        settings_output_string( "width            = {i} \n", DEFAULT_RESOLUTION_WIDTH );
        settings_output_string( "height           = {i} \n", DEFAULT_RESOLUTION_HEIGHT );
        settings_output_string( "resolution_scale = {f,.1} \n", DEFAULT_RESOLUTION_SCALE );
        settings_output_string( "backend          = opengl \n" );

        settings_output_string( "[audio] \n" );
        settings_output_string( "master = {f,.1} \n", DEFAULT_AUDIO_VOLUME_MASTER );
        settings_output_string( "music  = {f,.1} \n", DEFAULT_AUDIO_VOLUME_MUSIC );
        settings_output_string( "sfx    = {f,.1} \n", DEFAULT_AUDIO_VOLUME_SFX );

        platform->io.file_close( settings_file );

        settings_file = platform->io.file_open( SETTINGS_PATH, flags );

        if( !settings_file ) {
            fatal_log( "Failed to reopen settings file for reading!" );
            return false;
        }
    }

    parse_result.resolution_width  = DEFAULT_RESOLUTION_WIDTH;
    parse_result.resolution_height = DEFAULT_RESOLUTION_HEIGHT;
    parse_result.resolution_scale  = DEFAULT_RESOLUTION_SCALE;
    parse_result.backend           = RENDERER_BACKEND_OPENGL;

    usize settings_file_size = platform->io.file_query_size( settings_file );
    if( !settings_file_size ) {
        platform->io.file_close( settings_file );
        *out_parse_result = parse_result;
        warn_log( "Settings file is empty!" );
        return true;
    }

    char* settings_file_buffer = system_alloc( settings_file_size );
    if( !settings_file_buffer ) {
        platform->io.file_close( settings_file );
        fatal_log( "Failed to allocate settings file buffer!" );
        return false;
    }

    b32 read_result = platform->io.file_read(
        settings_file,
        settings_file_size,
        settings_file_buffer );
    if( !read_result ) {
        platform->io.file_close( settings_file );
        system_free( settings_file_buffer, settings_file_size );
        fatal_log( "Failed to read settings file!" );
        return false;
    }

    StringSlice settings;
    settings.buffer   = settings_file_buffer;
    settings.len      = settings_file_size;
    settings.capacity = settings_file_size;

    enum Section section = SECTION_UNKNOWN;

    string_slice_const( token_section_resolution, "[graphics]" );
    string_slice_const( token_width, "width" );
    string_slice_const( token_height, "height" );
    string_slice_const( token_resolution_scale, "resolution_scale" );
    string_slice_const( token_backend, "backend" );

    string_slice_const( token_section_audio, "[audio]" );
    string_slice_const( token_audio_volume_master, "master" );
    string_slice_const( token_audio_volume_music, "music" );
    string_slice_const( token_audio_volume_sfx, "sfx" );

    string_slice_const( token_opengl, "opengl" );
    string_slice_const( token_vulkan, "vulkan" );
    string_slice_const( token_metal, "metal" );
    string_slice_const( token_webgl, "webgl" );
    string_slice_const( token_directx11, "directx11" );
    string_slice_const( token_directx12, "directx12" );

    usize eol = 0;
    StringSlice line = string_slice_clone( &settings );

    while( string_slice_find_char( &line, '\n', &eol ) ) {
        StringSlice temp = string_slice_clone( &line );
        temp.len = eol + 1;

        switch( temp.buffer[0] ) {
            case '[': {
                if( string_slice_find( &temp, &token_section_resolution, NULL ) ) {
                    section = SECTION_GRAPHICS;
                } else if( string_slice_find( &temp, &token_section_audio, NULL ) ) {
                    section = SECTION_AUDIO;
                }
            } break;
            case ' ':
            case ';': {
                section = SECTION_UNKNOWN;
            } break;
        }

        if( section == SECTION_UNKNOWN ) {
            goto skip;
        }

        u64 int_parse   = 0;
        f64 float_parse = 0.0;

        switch( section ) {
            case SECTION_GRAPHICS: {
                if( string_slice_find( &temp, &token_width, NULL ) ) {
                    if( ___settings_parse_uint( &temp, &token_width, &int_parse ) ) {
                        parse_result.resolution_width = max( int_parse, 1 );
                    }
                } else if( string_slice_find( &temp, &token_height, NULL ) ) {
                    if( ___settings_parse_uint( &temp, &token_height, &int_parse ) ) {
                        parse_result.resolution_height = max( int_parse, 1 );
                    }
                } else if( string_slice_find( &temp, &token_resolution_scale, NULL ) ) {
                    if( ___settings_parse_float(
                        &temp, &token_resolution_scale, &float_parse
                    ) ) {
                        parse_result.resolution_scale = max( float_parse, 0.1 );
                    }
                } else if( string_slice_find( &temp, &token_backend, NULL ) ) {
                    StringSlice backend;
                    backend.buffer = temp.buffer + token_backend.len;
                    backend.len    = temp.len    - token_backend.len;

                    if( string_slice_find( &backend, &token_opengl, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_OPENGL;
                    } else if( string_slice_find( &backend, &token_vulkan, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_VULKAN;
                    } else if( string_slice_find( &backend, &token_metal, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_METAL;
                    } else if( string_slice_find( &backend, &token_webgl, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_WEBGL;
                    } else if( string_slice_find( &backend, &token_directx11, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_DX11;
                    } else if( string_slice_find( &backend, &token_directx12, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_DX12;
                    }
                }
            } break;
            case SECTION_AUDIO: {
                if( string_slice_find( &temp, &token_audio_volume_master, NULL ) ) {
                    if( ___settings_parse_float(
                        &temp, &token_audio_volume_master, &float_parse
                    ) ) {
                        parse_result.audio_volume_master = clamp01( float_parse );
                    }
                } else if( string_slice_find( &temp, &token_audio_volume_music, NULL ) ) {
                    if( ___settings_parse_float( &temp, &token_audio_volume_music, &float_parse ) ) {
                        parse_result.audio_volume_music = clamp01( float_parse );
                    }
                } else if( string_slice_find( &temp, &token_audio_volume_sfx, NULL ) ) {
                    if( ___settings_parse_float(
                        &temp, &token_audio_volume_sfx, &float_parse
                    ) ) {
                        parse_result.audio_volume_sfx = clamp01( float_parse );
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
        warn_log(
            "Backend '{cc}' is not supported on current platform!",
            renderer_backend_to_string( parse_result.backend ) );
        parse_result.backend = DEFAULT_RENDERER_BACKEND;
    }

    system_free( settings_file_buffer, settings_file_size );
    platform->io.file_close( settings_file );

    *out_parse_result = parse_result;
    return true;
}

LD_API void engine_exit(void) {
    note_log( "Application requested program to exit." );
    global_application_is_running = false;
}
LD_API void engine_set_fullscreen( b32 is_fullscreen ) {
    assert( ENGINE_SURFACE );
    PlatformSurfaceMode mode =
        is_fullscreen ? PLATFORM_SURFACE_FULLSCREEN : PLATFORM_SURFACE_WINDOWED;
    platform->surface.set_mode( ENGINE_SURFACE, mode );
}
LD_API b32 engine_query_fullscreen(void) {
    assert( ENGINE_SURFACE );
    return
        platform->surface.query_mode( ENGINE_SURFACE ) == PLATFORM_SURFACE_FULLSCREEN;
}
LD_API void engine_toggle_fullscreen(void) {
    engine_set_fullscreen( !engine_query_fullscreen() );
}

#include "core_generated_dependencies.inl"
