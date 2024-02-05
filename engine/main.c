/**
 * Description:  Liquid Engine Core Main
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 23, 2023
*/
#include "shared/defines.h"
#include "core/print.h"
#include "core/path.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/collections.h"
#include "core/sync.h"
#include "core/fs.h"
#include "core/shared_object.h"
#include "core/time.h"
#include "core/jobs.h"
#include "core/system.h"
#include "core/lib.h"

#include "engine/audio.h"
#include "engine/logging.h"
#include "engine/input.h"
#include "engine/engine.h"
#include "engine/graphics/internal.h"

#include "media/surface.h"
#include "media/lib.h"

global const char* global_executable_name = "";

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

#define ENGINE_SUCCESS                              (0)
#define ENGINE_ERROR_PARSE                          (128)
#define ENGINE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED (129)
#define ENGINE_ERROR_OPEN_GAME_LIBRARY              (130)
#define ENGINE_ERROR_LOAD_GAME_FUNCTIONS            (131)
#define ENGINE_ERROR_ENGINE_MEMORY_ALLOCATION       (134)
#define ENGINE_ERROR_LOGGING_SUBSYSTEM_INITIALIZE   (135)
#define ENGINE_ERROR_THREAD_SUBSYSTEM_INITIALIZE    (136)
#define ENGINE_ERROR_RENDERER_SUBSYSTEM_INITIALIZE  (137)
#define ENGINE_ERROR_AUDIO_SUBSYSTEM_INITIALIZE     (138)
#define ENGINE_ERROR_APPLICATION_INITIALIZE         (139)
#define ENGINE_ERROR_CREATE_SURFACE                 (140)
#define ENGINE_ERROR_APPLICATION_RUN                (141)
#define ENGINE_ERROR_RENDERER_DRAW                  (142)
#define ENGINE_ERROR_MISSING_INSTRUCTIONS           (143)
#define ENGINE_ERROR_UNKNOWN (255)

typedef struct SurfaceCallbackData {
    b32* surface_is_active;
} SurfaceCallbackData;
internal no_inline void surface_callback(
    MediaSurface* surface, MediaSurfaceCallbackData* data, void* params
) {
    SurfaceCallbackData* engine_data = params;

    switch( data->type ) {
        case MEDIA_SURFACE_CALLBACK_TYPE_CLOSE: {
            global_application_is_running = false;
            note_log( "application is shutting down." );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_ACTIVATE: {
            *engine_data->surface_is_active = data->activate.is_active;
            note_log(
                "Surface {cc}", data->activate.is_active ?
                "is active." : "is inactive." );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_RESOLUTION_CHANGE: {
            ivec2 surface_dimensions = {
                data->resolution_change.new_width,
                data->resolution_change.new_height };
            vec2 surface_dimensions_f = v2_iv2( surface_dimensions );

            ivec2 framebuffer_dimensions =
                iv2_v2( v2_mul( surface_dimensions_f, global_resolution_scale ) );
            renderer_subsystem_on_resize( surface_dimensions, framebuffer_dimensions );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_KEYBOARD_KEY: {
            input_subsystem_set_key( data->key.key, data->key.is_down );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_BUTTON: {
            input_subsystem_set_mouse_button(
                data->mouse_button.button,
                data->mouse_button.is_down );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE: {
            i32 w, h;
            media_surface_query_dimensions( surface, &w, &h );

            i32 x = data->mouse_move.x;
            i32 y = data->mouse_move.y;
            f32 x01 = (f32)x / (f32)w;
            f32 y01 = (f32)y / (f32)h;

            input_subsystem_set_mouse_position( x, y, x01, y01 );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE_RELATIVE: {
            input_subsystem_set_mouse_relative(
                data->mouse_move.x, data->mouse_move.y );
        } break;
        case MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_WHEEL: {
            if( data->mouse_wheel.is_horizontal ) {
                input_subsystem_set_mouse_wheel_horizontal( data->mouse_wheel.value );
            } else {
                input_subsystem_set_mouse_wheel( data->mouse_wheel.value );
            }
        } break;
    }
}

internal b32 parse_settings( struct SettingsParse* out_parse_result );
internal b32 check_instructions( SystemInfo* info );

global MediaSurface* ENGINE_SURFACE = NULL;

#define DEFAULT_LOGGING_FILE_PATH "./museum-logging.txt"

internal void lib_logging(
    LoggingLevel level, usize message_len, const char* message, void* params );
internal void print_help(void);

int main( int argc, char** argv ) {
    #define exit( code )\
        media_shutdown();\
        job_system_shutdown();\
        return code

    global_executable_name = argv[0];

    SystemInfo system_info = {};
    system_info_query( &system_info );

    time_initialize();

#if defined(LD_LOGGING)

    PathSlice default_logging_path = path_slice( DEFAULT_LOGGING_FILE_PATH );
    FileHandle* logging_file = fs_file_open(
        default_logging_path, FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_SHARE_ACCESS_READ );

    if( !logging_file ) {
        println_err( "[FATAL] Failed to open logging file!" );
        exit( ENGINE_ERROR_LOGGING_SUBSYSTEM_INITIALIZE );
    }

    logging_subsystem_initialize( logging_file );
    logging_set_level( LOGGING_LEVEL_ALL );

    core_logging_callback_set( lib_logging, NULL );
    media_logging_callback_set( lib_logging, NULL );

#endif
    if( !media_initialize() ) {
        fatal_log( "failed to initialize media!" );
        exit( ENGINE_ERROR_UNKNOWN );
    }

    if( !check_instructions( &system_info ) ) {
        exit( ENGINE_ERROR_MISSING_INSTRUCTIONS );
    }

    struct SettingsParse settings = ___settings_parse_default();
    if( !parse_settings( &settings ) ) {
        fatal_log( "failed to parse settings!" );
        exit( ENGINE_ERROR_PARSE );
    }

    f32 audio_volume_master = settings.audio_volume_master;
    f32 audio_volume_music  = settings.audio_volume_music;
    f32 audio_volume_sfx    = settings.audio_volume_sfx;
    unused( audio_volume_master + audio_volume_sfx + audio_volume_music );
    i32 width  = settings.resolution_width;
    i32 height = settings.resolution_height;
    global_resolution_scale = settings.resolution_scale;
    RendererBackend backend = settings.backend;

    StringSlice game_library_path = string_slice( GAME_LIBRARY_PATH_DEFAULT );

    /* parse arguments */ {
        StringSlice set_master_volume    = string_slice( "--master-volume=" );
        StringSlice set_music_volume     = string_slice( "--music-volume=" );
        StringSlice set_sfx_volume       = string_slice( "--sfx-volume=" );
        StringSlice set_width            = string_slice( "--width=" );
        StringSlice set_height           = string_slice( "--height=" );
        StringSlice set_resolution_scale = string_slice( "--resolution_scale=" );
        StringSlice help                 = string_slice( "--help" );
        StringSlice h                    = string_slice( "-h" );

#if defined(LD_DEVELOPER_MODE)
        StringSlice libload   = string_slice( "--libload=" );
        StringSlice clear_log = string_slice( "--clear-log" );
#endif
#if defined(LD_PLATFORM_WINDOWS)

#if defined(LD_DEVELOPER_MODE)
        StringSlice output_debug_string = string_slice( "--output-debug-string" );
        b32 enable_output_debug_string  = false;
#endif
        StringSlice dx11 = string_slice( "--directx11" );
        StringSlice dx12 = string_slice( "--directx12" );
#endif
        StringSlice opengl = string_slice( "--opengl" );
        StringSlice vulkan = string_slice( "--vulkan" );
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
        StringSlice metal = string_slice( "--metal" );
#endif
#if defined(LD_PLATFORM_WASM)
        StringSlice webgl = string_slice( "--webgl" );
#endif

        b32 parse_error = false;

        for( int i = 1; i < argc; ++i ) {
            StringSlice current = string_slice_from_cstr( 0, argv[i] );

#if defined(LD_DEVELOPER_MODE)
            if( string_slice_find( current, libload, NULL ) ) {
                if( current.len - libload.len < 1 ) {
                    StringSlice path = current;
                    path.str += libload.len;
                    path.len -= libload.len;
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid game library path: {s}"
                        CONSOLE_COLOR_RESET,
                        path );
                    parse_error = true;
                    break;
                }
                game_library_path.str = current.str + libload.len;
                game_library_path.len = current.len - libload.len;
                continue;
            }
            if( string_slice_cmp( current, clear_log ) ) {
                logging_subsystem_detach_file();
                fs_file_close( logging_file );

                if( !fs_delete_file( default_logging_path ) ) {
                    warn_log( "Unable to delete logging file!" );
                }

                logging_file = fs_file_open(
                    default_logging_path,
                    FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_SHARE_ACCESS_READ );

                read_write_fence();
                logging_subsystem_attach_file( logging_file );

                note_log( "Logging file cleared!" );
                continue;
            }
#endif

            if( string_slice_find( current, set_width, NULL ) ) {
                if( current.len - set_width.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid width!"
                        CONSOLE_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.str += set_width.len;
                current.len -= set_width.len;
                u64 parse_result = 0;
                if( !string_slice_parse_uint( current, &parse_result ) ) {
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

            if( string_slice_find( current, set_height, NULL ) ) {
                if( current.len - set_height.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid height!"
                        CONSOLE_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.str += set_height.len;
                current.len -= set_height.len;
                u64 parse_result = 0;
                if( !string_slice_parse_uint( current, &parse_result ) ) {
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

            if( string_slice_find( current, set_resolution_scale, NULL ) ) {
                if( current.len - set_resolution_scale.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid resolution scale!"
                        CONSOLE_COLOR_RESET
                    );
                    parse_error = true;
                    break;
                }
                current.str += set_resolution_scale.len;
                current.len -= set_resolution_scale.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( current, &parse_result ) ) {
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

            if( string_slice_find( current, set_master_volume, NULL ) ) {
                if( current.len - set_master_volume.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid master volume!"
                        CONSOLE_COLOR_RESET );
                    parse_error = true;
                    break;
                }
                current.str += set_master_volume.len;
                current.len -= set_master_volume.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( current, &parse_result ) ) {
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

            if( string_slice_find( current, set_music_volume, NULL ) ) {
                if( current.len - set_music_volume.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid music volume!"
                        CONSOLE_COLOR_RESET );
                    parse_error = true;
                    break;
                }
                current.str += set_music_volume.len;
                current.len -= set_music_volume.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( current, &parse_result ) ) {
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

            if( string_slice_find( current, set_sfx_volume, NULL ) ) {
                if( current.len - set_sfx_volume.len < 1 ) {
                    println_err(
                        CONSOLE_COLOR_RED
                        "invalid sfx volume!"
                        CONSOLE_COLOR_RESET );
                    parse_error = true;
                    break;
                }
                current.str += set_sfx_volume.len;
                current.len -= set_sfx_volume.len;
                f64 parse_result = 0.0;
                if( !string_slice_parse_float( current, &parse_result ) ) {
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

            if( string_slice_cmp( current, opengl ) ) {
                backend = RENDERER_BACKEND_OPENGL;
                continue;
            }

            if( string_slice_cmp( current, vulkan ) ) {
                backend = RENDERER_BACKEND_VULKAN;
                continue;
            }

#if defined(LD_PLATFORM_WINDOWS)

#if defined(LD_DEVELOPER_MODE)
            if( string_slice_cmp( current, output_debug_string ) ) {
                enable_output_debug_string = true;
                continue;
            }
#endif

            if( string_slice_cmp( current, dx11 ) ) {
                backend = RENDERER_BACKEND_DX11;
                continue;
            }
            if( string_slice_cmp( current, dx12 ) ) {
                backend = RENDERER_BACKEND_DX12;
                continue;
            }
#endif
#if defined(LD_PLATFORM_MACOS) || defined(LD_PLATFORM_IOS)
            if( string_slice_cmp( current, metal ) ) {
                backend = RENDERER_BACKEND_METAL;
                continue;
            }
#endif
#if defined(LD_PLATFORM_WASM)
            if( string_slice_cmp( current, webgl ) ) {
                backend = RENDERER_BACKEND_WEBGL;
                continue;
            }
#endif
            if(
                string_slice_cmp( current, help ) ||
                string_slice_cmp( current, h )
            ) {
                print_help();
                exit( ENGINE_SUCCESS );
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
            exit( ENGINE_ERROR_PARSE );
        }

#if defined(LD_PLATFORM_WINDOWS) && defined(LD_DEVELOPER_MODE)
        if( enable_output_debug_string ) {
            logging_set_output_debug_string_enabled( true );
        }
#endif
    }

    if( !renderer_backend_is_supported( backend ) ) {
        string_buffer_empty( error_title, 64 );
        string_buffer_empty( error_message, 255 );

        string_buffer_fmt(
            &error_title,
            "Fatal Error ({u8}){0}",
            ENGINE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED );
        string_buffer_fmt(
            &error_message,
            "Renderer backend '{cc}' is not supported on this platform!{0}",
            renderer_backend_to_string( backend ) );

        fatal_log( "{s}", string_buffer_to_slice( &error_message ) );
        media_fatal_message_box_blocking(
            error_title.str, error_message.str );
        exit( ENGINE_ERROR_RENDERER_BACKEND_NOT_SUPPORTED );
    }

    note_log( "Engine Configuration:" );
    note_log( "Version:           {i}.{i}", LIQUID_ENGINE_VERSION_MAJOR, LIQUID_ENGINE_VERSION_MINOR );
#if defined(LD_PLATFORM_WINDOWS)
    StringSlice os = string_slice( "win32" );
#endif
#if defined(LD_PLATFORM_MACOS)
    StringSlice os = string_slice( "macos" );
#endif
#if defined(LD_PLATFORM_IOS)
    StringSlice os = string_slice( "ios" );
#endif
#if defined(LD_PLATFORM_ANDROID)
    StringSlice os = string_slice( "android" );
#endif
#if defined(LD_PLATFORM_LINUX)
    StringSlice os = string_slice( "linux" );
#endif
#if defined(LD_PLATFORM_WASM)
    StringSlice os = string_slice( "wasm" );
#endif
#if defined(LD_ARCH_32_BIT)
    #if defined(LD_ARCH_X86)
        StringSlice arch = string_slice( "x86" );
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            StringSlice arch = string_slice( "arm little-endian 32-bit" );
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            StringSlice arch = string_slice( "arm big-endian 32-bit" );
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        StringSlice arch = string_slice( "wasm 32-bit" );
    #endif
#endif
#if defined(LD_ARCH_64_BIT)
    #if defined(LD_ARCH_X86)
        StringSlice arch = string_slice( "x86_64" );
    #endif
    #if defined(LD_ARCH_ARM)
        #if defined(LD_ARCH_LITTLE_ENDIAN)
            StringSlice arch = string_slice( "arm little-endian 64-bit" );
        #endif
        #if defined(LD_ARCH_BIG_ENDIAN)
            StringSlice arch = string_slice( "arm big-endian 64-bit" );
        #endif
    #endif
    #if defined(LD_ARCH_WASM)
        StringSlice arch = string_slice( "wasm 64-bit" );
    #endif
#endif
    note_log( "Platform:          {s}, {s}", os, arch );
    note_log( "Page Size:         {usize}", system_info.page_size );
    note_log( "Game Library Path: {s}", game_library_path );
    note_log( "Renderer Backend:  {cc}",
        renderer_backend_to_string( backend ) );
    note_log( "Resolution:        {i}x{i}", width, height );
    note_log( "Resolution Scale:  {f,.2}x", global_resolution_scale );

    SharedObject* game = shared_object_open( game_library_path.str );
    if( !game ) {
        string_buffer_empty( error_title, 64 );
        string_buffer_empty( error_message, 255 );

        string_buffer_fmt(
            &error_title,
            "Fatal Error ({u8}){0}",
            ENGINE_ERROR_OPEN_GAME_LIBRARY );
        string_buffer_fmt(
            &error_message,
            "Failed to open game library! Game library path: {s}{0}",
            game_library_path );

        fatal_log( "{s}", string_buffer_to_slice( &error_message ) );
        media_fatal_message_box_blocking(
            error_title.str, error_message.str );
        exit( ENGINE_ERROR_OPEN_GAME_LIBRARY );
    }

    ApplicationQueryMemoryRequirementFN*
        application_query_memory_requirement =
        (ApplicationQueryMemoryRequirementFN*)shared_object_load(
            game, "application_query_memory_requirement" );
    ApplicationInitializeFN* application_initialize =
        (ApplicationInitializeFN*)shared_object_load(
            game, "application_initialize" );
    ApplicationRunFN* application_run =
        (ApplicationRunFN*)shared_object_load(
            game, "application_run" );

    if( !(
        application_query_memory_requirement &&
        application_initialize &&
        application_run
    ) ) {
        string_buffer_empty( error_title, 64 );
        string_buffer_empty( error_message, 255 );

        string_buffer_fmt(
            &error_title,
            "Fatal Error ({u8}){0}",
            ENGINE_ERROR_LOAD_GAME_FUNCTIONS );
        string_buffer_fmt(
            &error_message,
            "Failed to load game library functions!{0}" );

        fatal_log( "{s}", string_buffer_to_slice( &error_message ) );
        media_fatal_message_box_blocking(
            error_title.str, error_message.str );
        exit( ENGINE_ERROR_LOAD_GAME_FUNCTIONS );
    }

#if 0
    if( !audio_subsystem_initialize() ) {
        fatal_log( "Failed to initialize audio subsystem!" );
        media_fatal_message_box_blocking(
            "Fatal Error "
            macro_value_to_string( ENGINE_ERROR_AUDIO_SUBSYSTEM_INITIALIZE ),
            "Failed to initialize audio subsystem!" );
        exit( ENGINE_ERROR_AUDIO_SUBSYSTEM_INITIALIZE );
    }
#endif

    usize audio_subsystem_memory_requirement =
        audio_subsystem_query_memory_requirement();

    u32 thread_count = system_info.cpu_count;
    thread_count = max( thread_count, 1 );

    usize stack_size                       = 0;
    void* stack_buffer                     = NULL;
    usize renderer_subsystem_size          = 0;
    usize renderer_command_buffer_capacity = 0;
    usize renderer_command_buffer_size     = 0;
    usize application_memory_requirement   = 0;
    usize jobs_system_memory_requirement   =
        job_system_query_memory_requirement( thread_count );
    /* allocate stack */ {
        application_memory_requirement =
            application_query_memory_requirement();

        stack_size += application_memory_requirement;

        stack_size += jobs_system_memory_requirement;
        stack_size += input_subsystem_query_memory_requirement();
        stack_size += audio_subsystem_memory_requirement;

        renderer_subsystem_size = renderer_subsystem_query_size( backend );
        stack_size += renderer_subsystem_size;

        renderer_command_buffer_capacity = 1024;
        renderer_command_buffer_size =
            sizeof(struct RenderCommand) * renderer_command_buffer_capacity;

        stack_size += renderer_command_buffer_size;

        usize stack_page_count = memory_size_to_page_count( stack_size );
        stack_buffer           = system_page_alloc( stack_page_count );
        stack_size             = page_count_to_memory_size( stack_page_count );

        info_log(
            "Stack Size: {usize}({f,.2,m}) Stack Pages: {usize}",
            stack_size, (f64)stack_size, stack_page_count );
        if( !stack_buffer ) {
            string_buffer_empty( error_title, 64 );
            string_buffer_empty( error_message, 255 );

            string_buffer_fmt(
                &error_title,
                "Fatal Error ({u8}){0}",
                ENGINE_ERROR_ENGINE_MEMORY_ALLOCATION );
            string_buffer_fmt(
                &error_message,
                "Out of Memory!{0}" );

            fatal_log( "{s}", string_buffer_to_slice( &error_message ) );
            media_fatal_message_box_blocking(
                error_title.str, error_message.str );
            exit( ENGINE_ERROR_ENGINE_MEMORY_ALLOCATION );
        }

    }

    StackAllocator stack = {};
    stack.buffer      = stack_buffer;
    stack.buffer_size = stack_size;

#if 0
    void* audio_subsystem_buffer =
        stack_allocator_push( &stack, audio_subsystem_memory_requirement );
    audio_subsystem_submit_buffer_memory( audio_subsystem_buffer );
    audio_set_master_volume( audio_volume_master );
    audio_set_music_volume( audio_volume_music );
    audio_set_sfx_volume( audio_volume_sfx );
#endif

    /* initialize input subsystem */ {
        void* input_subsytem_buffer = stack_allocator_push(
            &stack, input_subsystem_query_memory_requirement() );
        input_subsystem_initialize( input_subsytem_buffer );
    }

    /* initialize threading subsystem */ {
        void* jobs_subsystem_buffer =
            stack_allocator_push( &stack, jobs_system_memory_requirement );

        if( !job_system_initialize( thread_count, jobs_subsystem_buffer ) ) {
            fatal_log( "Failed to initialize thread subsystem!" );
            media_fatal_message_box_blocking(
                "Fatal Error "
                macro_value_to_string(
                    ENGINE_ERROR_THREAD_SUBSYSTEM_INITIALIZE ),
                "Failed to initialize thread subsystem!" );
            exit( ENGINE_ERROR_THREAD_SUBSYSTEM_INITIALIZE );
        }
    }

    b32 surface_is_active = true;
    MediaSurface surface = {};
    SurfaceCallbackData surface_callback_data = {};
    surface_callback_data.surface_is_active = &surface_is_active;

    /* initialize surface */ {
        #define SURFACE_CREATE_HIDDEN true
        #define SURFACE_RESIZEABLE    true
        MediaSurfaceFlags flags =
            MEDIA_SURFACE_FLAG_HIDDEN | MEDIA_SURFACE_FLAG_RESIZEABLE;
        if( !media_surface_create(
            width, height, sizeof("liquid engine"), "liquid engine",
            flags, surface_callback, &surface_callback_data, backend, &surface
        ) ) {
            media_fatal_message_box_blocking( "Fatal Error", "Failed to create window!" );
            exit( ENGINE_ERROR_CREATE_SURFACE );
        }
        media_surface_set_hidden( &surface, false );
    }

    ENGINE_SURFACE = &surface;

    RenderData render_data = {};
    /* initialize renderer */ {
        void* renderer_subsystem_buffer =
            stack_allocator_push( &stack, renderer_subsystem_size );
        void* renderer_command_buffer =
            stack_allocator_push( &stack, renderer_command_buffer_size );

        render_data.list_commands = list_create(
            renderer_command_buffer_capacity,
            sizeof(struct RenderCommand), renderer_command_buffer );

        if( !renderer_subsystem_init(
            &surface, backend,
            iv2_v2( v2_mul( v2( (f32)width, (f32)height ), global_resolution_scale ) ),
            &render_data,
            renderer_subsystem_buffer
        ) ) {
            fatal_log( "Failed to initialize renderer subsystem!" );
            media_fatal_message_box_blocking(
                "Fatal Error "
                macro_value_to_string( ENGINE_ERROR_RENDERER_SUBSYSTEM_INITIALIZE ),
                "Failed to initialize renderer subsystem!" );
            exit( ENGINE_ERROR_RENDERER_SUBSYSTEM_INITIALIZE );
        }
    }

    void* application_memory =
        stack_allocator_push( &stack, application_memory_requirement );
    if( !application_initialize( application_memory ) ) {
        fatal_log( "Failed to initialize application!" );
        media_fatal_message_box_blocking(
            "Fatal Error "
            macro_value_to_string( ENGINE_ERROR_APPLICATION_INITIALIZE ),
            "Failed to initialize application!" );
        exit( ENGINE_ERROR_APPLICATION_INITIALIZE );
    }

    while( global_application_is_running ) {
        input_subsystem_swap_state();
        input_subsystem_update_gamepads();
        media_surface_pump_events( &surface );

#if 0
        if( !surface_is_active ) {
            audio_subsystem_pause();
            continue;
        } else {
            audio_subsystem_resume();
        }
#endif

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
            media_fatal_message_box_blocking(
                "Fatal Error "
                macro_value_to_string( ENGINE_ERROR_APPLICATION_RUN ),
                "Failed to run application!" );
            exit( ENGINE_ERROR_APPLICATION_RUN );
        }

#if 0
        audio_subsystem_output();
#endif

        if( !renderer_subsystem_draw() ) {
            fatal_log( "Renderer failed!" );
            media_fatal_message_box_blocking(
                "Fatal Error "
                macro_value_to_string( ENGINE_ERROR_RENDERER_DRAW ),
                "Renderer failed!" );
            exit( ENGINE_ERROR_RENDERER_DRAW );
        }

        list_clear( &render_data.list_commands );

        if( input_is_mouse_locked() ) {
            media_surface_cursor_center( &surface );
        }

        time_update();
    }


    audio_subsystem_shutdown();

    media_surface_clear_callback( &surface );

    renderer_subsystem_shutdown();
    media_surface_destroy( &surface );

#if defined(LD_LOGGING)
    fs_file_close( logging_file );
#endif

    shared_object_close( game );
    exit( ENGINE_SUCCESS );

    #undef exit
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
    println( "USAGE: {cc} [options]\n", global_executable_name );
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
    StringSlice line, StringSlice token, u64* out_result
) {
    StringSlice number = {};
    number.str = line.str + token.len;
    number.len = line.len    - token.len;

    for( usize i = 0; i < number.len; ++i ) {
        char current = number.str[i];

        if( char_is_digit( current ) ) {
            number.str += i;
            number.len -= i;

            u64 int_parse = 0;
            if( !string_slice_parse_uint( number, &int_parse ) ) {
                return false;
            }

            *out_result = int_parse;
            break;
        }
    }

    return true;
}
internal b32 ___settings_parse_float(
    StringSlice line, StringSlice token, f64* out_result
) {
    StringSlice number = {};
    number.str = line.str + token.len;
    number.len = line.len    - token.len;

    for( usize i = 0; i < number.len; ++i ) {
        char current = number.str[i];

        if( char_is_digit( current ) ) {
            number.str += i;
            number.len -= i;

            while( number.len && !char_is_digit( number.str[number.len - 1] ) ) {
                string_slice_pop( number, &number, NULL );
            }

            f64 float_parse = 0.0;
            if( !string_slice_parse_float( number, &float_parse ) ) {
                return false;
            }

            *out_result = float_parse;
            break;
        }
    }

    return true;
}
typedef enum Section : u32 {
    SECTION_UNKNOWN,
    SECTION_GRAPHICS,
    SECTION_AUDIO
} Section;

internal
b32 parse_settings( struct SettingsParse* out_parse_result ) {
    struct SettingsParse parse_result = {};

    FileOpenFlags flags = FILE_OPEN_FLAG_READ;
    PathSlice settings_path = path_slice( "./settings.ini" );

    FileHandle* settings_file = fs_file_open(
        settings_path, flags );

    if( !settings_file ) {
        settings_file = fs_file_open(
            settings_path, FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_CREATE );
        if( !settings_file ) {
            fatal_log( "failed to open settings file at all!" );
            return false;
        }

        #define settings_output_string( format, ... )\
            fs_file_write_fmt( settings_file, format, ##__VA_ARGS__ )

        settings_output_string( "[graphics] \n" );
        settings_output_string( "width            = {i} \n", DEFAULT_RESOLUTION_WIDTH );
        settings_output_string( "height           = {i} \n", DEFAULT_RESOLUTION_HEIGHT );
        settings_output_string( "resolution_scale = {f,.1} \n", DEFAULT_RESOLUTION_SCALE );
        settings_output_string( "backend          = opengl \n" );

        settings_output_string( "[audio] \n" );
        settings_output_string( "master = {f,.1} \n", DEFAULT_AUDIO_VOLUME_MASTER );
        settings_output_string( "music  = {f,.1} \n", DEFAULT_AUDIO_VOLUME_MUSIC );
        settings_output_string( "sfx    = {f,.1} \n", DEFAULT_AUDIO_VOLUME_SFX );

        fs_file_close( settings_file );

        settings_file = fs_file_open( settings_path, flags );

        if( !settings_file ) {
            fatal_log( "failed to reopen settings file for reading!" );
            return false;
        }
    }

    parse_result.resolution_width  = DEFAULT_RESOLUTION_WIDTH;
    parse_result.resolution_height = DEFAULT_RESOLUTION_HEIGHT;
    parse_result.resolution_scale  = DEFAULT_RESOLUTION_SCALE;
    parse_result.backend           = RENDERER_BACKEND_OPENGL;

    usize settings_file_size = fs_file_query_size( settings_file );
    if( !settings_file_size ) {
        fs_file_close( settings_file );
        *out_parse_result = parse_result;
        warn_log( "settings file is empty!" );
        return true;
    }

    char* settings_file_buffer = system_alloc( settings_file_size );
    if( !settings_file_buffer ) {
        fs_file_close( settings_file );
        fatal_log( "failed to allocate settings file buffer!" );
        return false;
    }

    b32 read_result = fs_file_read(
        settings_file,
        settings_file_size,
        settings_file_buffer );
    if( !read_result ) {
        fs_file_close( settings_file );
        system_free( settings_file_buffer, settings_file_size );
        fatal_log( "failed to read settings file!" );
        return false;
    }

    StringSlice settings;
    settings.str = settings_file_buffer;
    settings.len = settings_file_size;

    enum Section section = SECTION_UNKNOWN;

    StringSlice token_section_graphics          = string_slice( "[graphics]" );
    StringSlice token_graphics_width            = string_slice( "width" );
    StringSlice token_graphics_height           = string_slice( "height" );
    StringSlice token_graphics_resolution_scale = string_slice( "resolution_scale" );
    StringSlice token_graphics_backend          = string_slice( "backend" );
    StringSlice token_graphics_opengl           = string_slice( "opengl" );
    StringSlice token_graphics_vulkan           = string_slice( "vulkan" );
    StringSlice token_graphics_metal            = string_slice( "metal" );
    StringSlice token_graphics_webgl            = string_slice( "webgl" );
    StringSlice token_graphics_directx11        = string_slice( "directx11" );
    StringSlice token_graphics_directx12        = string_slice( "directx12" );

    StringSlice token_section_audio       = string_slice( "[audio]" );
    StringSlice token_audio_volume_master = string_slice( "master" );
    StringSlice token_audio_volume_music  = string_slice( "music" );
    StringSlice token_audio_volume_sfx    = string_slice( "sfx" );


    usize eol = 0;
    StringSlice line = settings;

    while( string_slice_find_char( line, '\n', &eol ) ) {
        StringSlice temp = line;
        temp.len = eol + 1;

        switch( temp.str[0] ) {
            case '[': {
                if( string_slice_find( temp, token_section_graphics, NULL ) ) {
                    section = SECTION_GRAPHICS;
                } else if( string_slice_find( temp, token_section_audio, NULL ) ) {
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
                if( string_slice_find( temp, token_graphics_width, NULL ) ) {
                    if( ___settings_parse_uint( temp, token_graphics_width, &int_parse ) ) {
                        parse_result.resolution_width = max( int_parse, 1 );
                    }
                } else if( string_slice_find( temp, token_graphics_height, NULL ) ) {
                    if( ___settings_parse_uint( temp, token_graphics_height, &int_parse ) ) {
                        parse_result.resolution_height = max( int_parse, 1 );
                    }
                } else if( string_slice_find( temp, token_graphics_resolution_scale, NULL ) ) {
                    if( ___settings_parse_float(
                        temp, token_graphics_resolution_scale, &float_parse
                    ) ) {
                        parse_result.resolution_scale = max( float_parse, 0.1 );
                    }
                } else if( string_slice_find( temp, token_graphics_backend, NULL ) ) {
                    StringSlice backend;
                    backend.str = temp.str + token_graphics_backend.len;
                    backend.len = temp.len - token_graphics_backend.len;

                    if( string_slice_find( backend, token_graphics_opengl, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_OPENGL;
                    } else if( string_slice_find( backend, token_graphics_vulkan, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_VULKAN;
                    } else if( string_slice_find( backend, token_graphics_metal, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_METAL;
                    } else if( string_slice_find( backend, token_graphics_webgl, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_WEBGL;
                    } else if( string_slice_find( backend, token_graphics_directx11, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_DX11;
                    } else if( string_slice_find( backend, token_graphics_directx12, NULL ) ) {
                        parse_result.backend = RENDERER_BACKEND_DX12;
                    }
                }
            } break;
            case SECTION_AUDIO: {
                if( string_slice_find( temp, token_audio_volume_master, NULL ) ) {
                    if( ___settings_parse_float(
                        temp, token_audio_volume_master, &float_parse
                    ) ) {
                        parse_result.audio_volume_master = clamp01( float_parse );
                    }
                } else if( string_slice_find( temp, token_audio_volume_music, NULL ) ) {
                    if( ___settings_parse_float( temp, token_audio_volume_music, &float_parse ) ) {
                        parse_result.audio_volume_music = clamp01( float_parse );
                    }
                } else if( string_slice_find( temp, token_audio_volume_sfx, NULL ) ) {
                    if( ___settings_parse_float(
                        temp, token_audio_volume_sfx, &float_parse
                    ) ) {
                        parse_result.audio_volume_sfx = clamp01( float_parse );
                    }
                }

            } break;
            default: break;
        }

    skip:
        line.str += temp.len;
        line.len -= temp.len;
    }

    if( !renderer_backend_is_supported( parse_result.backend ) ) {
        warn_log(
            "Backend '{cc}' is not supported on current platform!",
            renderer_backend_to_string( parse_result.backend ) );
        parse_result.backend = DEFAULT_RENDERER_BACKEND;
    }

    system_free( settings_file_buffer, settings_file_size );
    fs_file_close( settings_file );

    *out_parse_result = parse_result;
    return true;
}

LD_API void engine_exit(void) {
    note_log( "Application requested program to exit." );
    global_application_is_running = false;
}
LD_API void engine_set_fullscreen( b32 is_fullscreen ) {
    assert( ENGINE_SURFACE );
    media_surface_set_fullscreen( ENGINE_SURFACE, is_fullscreen );
}
LD_API b32 engine_query_fullscreen(void) {
    assert( ENGINE_SURFACE );
    return media_surface_query_fullscreen( ENGINE_SURFACE );
}
LD_API void engine_toggle_fullscreen(void) {
    engine_set_fullscreen( !engine_query_fullscreen() );
}

internal void lib_logging(
    LoggingLevel level, usize message_len, const char* message, void* params
) {
    unused(params);

    b32 trace      = bitfield_check( level, LOGGING_LEVEL_TRACE );
    b32 always_log = !level;

    const char* console_color = CONSOLE_COLOR_RESET;

    b32 is_error = bitfield_check( level, LOGGING_LEVEL_ERROR );

    LoggingType type = LOGGING_TYPE_NOTE;

    if( !level ) {
        console_color = CONSOLE_COLOR_MAGENTA;
        type          = LOGGING_TYPE_FATAL;
    } else if( is_error ) {
        console_color = CONSOLE_COLOR_RED;
        type          = LOGGING_TYPE_ERROR;
    } else if( bitfield_check( level, LOGGING_LEVEL_WARN ) ) {
        console_color = CONSOLE_COLOR_YELLOW;
        type          = LOGGING_TYPE_WARN;
    } else if( bitfield_check( level, LOGGING_LEVEL_INFO ) ) {
        console_color = CONSOLE_COLOR_WHITE;
        type          = LOGGING_TYPE_INFO;
    }

    if( bitfield_check( level, LOGGING_LEVEL_MEMORY ) && !is_error ) {
        console_color = CONSOLE_COLOR_CYAN;
        type          = LOGGING_TYPE_INFO;
    }

    ___internal_logging_output_fmt_locked(
        type, console_color,
        trace, always_log, true, true, message_len, message );
}

internal b32 check_instructions( SystemInfo* system_info ) {
#if defined(LD_ARCH_X86)
    CPUFeatureFlags missing_features = 0;

    #if LD_SIMD_WIDTH >= 4

    missing_features = system_info_feature_check_x86_sse( system_info );

    if( missing_features ) {
        print_err( CONSOLE_COLOR_MAGENTA );
        println_err( "fatal error: SSE instructions are missing!" );

        string_buffer_empty( missing_names, 64 );

        if( bitfield_check( missing_features, CPU_FEATURE_SSE ) ) {
            string_buffer_fmt( &missing_names, "SSE, " );
        }
        if( bitfield_check( missing_features, CPU_FEATURE_SSE2 ) ) {
            string_buffer_fmt( &missing_names, "SSE2, " );
        }
        if( bitfield_check( missing_features, CPU_FEATURE_SSE3 ) ) {
            string_buffer_fmt( &missing_names, "SSE3, " );
        }
        if( bitfield_check( missing_features, CPU_FEATURE_SSSE3 ) ) {
            string_buffer_fmt( &missing_names, "SSSE3, " );
        }
        if( bitfield_check( missing_features, CPU_FEATURE_SSE4_1 ) ) {
            string_buffer_fmt( &missing_names, "SSE4.1, " );
        }
        if( bitfield_check( missing_features, CPU_FEATURE_SSE4_2 ) ) {
            string_buffer_fmt( &missing_names, "SSE4.2, " );
        }

        // NOTE(alicia): gets rid of trailing comma
        string_buffer_pop( &missing_names, NULL );
        string_buffer_pop( &missing_names, NULL );

        println_err(
            "missing instructions: {s}",
            string_buffer_to_slice( &missing_names ) );
        print_err( CONSOLE_COLOR_RESET );

        media_fatal_message_box_blocking( "Fatal Error", missing_names.str );

        return false;
    }

    #endif /* SIMD >= 4 */

    #if LD_SIMD_WIDTH < 8
    missing_features = system_info_feature_check_x86_avx( system_info );

    if( missing_features ) {
        print_err( CONSOLE_COLOR_MAGENTA );
        println_err( "fatal error: SSE instructions are missing!" );

        string_buffer_empty( missing_names, 64 );

        if( bitfield_check( missing_features, CPU_FEATURE_AVX ) ) {
            string_buffer_fmt( &missing_names, "AVX, " );
        }
        if( bitfield_check( missing_features, CPU_FEATURE_AVX2 ) ) {
            string_buffer_fmt( &missing_names, "AVX2, " );
        }

        // NOTE(alicia): gets rid of trailing comma
        string_buffer_pop( &missing_names, NULL );
        string_buffer_pop( &missing_names, NULL );

        println_err(
            "missing instructions: {s}",
            string_buffer_to_slice( &missing_names ) );
        print_err( CONSOLE_COLOR_RESET );

        media_fatal_message_box_blocking( "Fatal Error", missing_names.str );

        return false;
    }
    #endif /* SIMD >= 8 */

#endif /* Arch x86 */
    
    return true;
}

#include "engine/generated_dependencies.inl"
#define NO_CSTD
#include "platform/platform_dllmain.c"

