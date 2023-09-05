// * Description:  Linux Platform Layer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 04, 2023
#include "defines.h"
#if defined(LD_PLATFORM_LINUX)
#include "corec.inl"
#include "core/ldengine.h"
#include "core/ldgraphics.h"
#include "core/ldmath.h"
#include "ldcstd.c"
#include "ldposixthread.c"
#include "ldlinux.h"
#include <dlfcn.h>
#include <cpuid.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <SDL2/SDL_keycode.h>

// NOTE(alicia): globals
global struct timespec START_TIME = {};

int main( int argc, char** argv ) {
    return engine_entry( argc, argv ) ? 0 : -1;
}

LinuxPlatform* PLATFORM = NULL;
usize PLATFORM_SUBSYSTEM_SIZE = sizeof(LinuxPlatform);
b32 platform_subsystem_init( void* buffer ) {
    LinuxPlatform* platform = buffer;
    mem_zero( platform, sizeof(LinuxPlatform) );

    Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS;
    int result = SDL_Init( flags );
    if( result ) {
        LINUX_LOG_ERROR( "Failed to initialize SDL2!" );
        return false;
    }

    clock_gettime( CLOCK_MONOTONIC_RAW, &START_TIME );

    PLATFORM = platform;
    return true;
}
void platform_subsystem_shutdown(void) {
    SDL_Quit();
    for( usize i = 0; i < LINUX_LIB_COUNT; ++i ) {
        if( PLATFORM->libs[i] ) {
            platform_library_free( PLATFORM->libs[i] );
        }
    }
}

usize PLATFORM_SURFACE_BUFFER_SIZE      = sizeof(LinuxSurface);
b32 PLATFORM_SUPPORTS_MULTIPLE_SURFACES = true;
b32 platform_surface_create(
    ivec2 surface_dimensions, const char* surface_name,
    enum RendererBackend backend,
    PlatformSurfaceCreateFlags flags, PlatformSurface* out_surface
) {
    LinuxSurface* surface = out_surface;
    mem_zero( surface, sizeof(LinuxSurface) );

    b32 create_hidden     =
        CHECK_BITS( flags, PLATFORM_SURFACE_CREATE_HIDDEN );
    b32 create_resizeable =
        CHECK_BITS( flags, PLATFORM_SURFACE_CREATE_RESIZEABLE );
    b32 create_fullscreen =
        CHECK_BITS( flags, PLATFORM_SURFACE_CREATE_FULLSCREEN );

    Uint32 sdlflags = 0;
    if( create_hidden ) {
        sdlflags |= SDL_WINDOW_HIDDEN;
    }
    if( create_resizeable ) {
        sdlflags |= SDL_WINDOW_RESIZABLE;
    }
    if( create_fullscreen ) {
        sdlflags |= SDL_WINDOW_FULLSCREEN;
    }

    switch( backend ) {
        case RENDERER_BACKEND_OPENGL: {
            sdlflags |= SDL_WINDOW_OPENGL;

            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_MAJOR_VERSION, GL_VERSION_MAJOR );
            SDL_GL_SetAttribute(
                SDL_GL_CONTEXT_MINOR_VERSION, GL_VERSION_MINOR );
            SDL_GL_SetAttribute(
                SDL_GL_DOUBLEBUFFER, 1 );
            SDL_GL_SetAttribute(
                SDL_GL_DEPTH_SIZE, 24 );
            SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
        } break;
        default: {
            // TODO(alicia): 
            UNIMPLEMENTED();
        } break;
    }

    int x, y;
    x = y = SDL_WINDOWPOS_UNDEFINED;
    SDL_Window* window = SDL_CreateWindow(
        surface_name,
        x, y,
        surface_dimensions.x,
        surface_dimensions.y,
        sdlflags
    );
    if( !window ) {
        LINUX_LOG_ERROR( "Failed to create window!" );
        return false;
    }

    surface->backend        = backend;
    surface->creation_flags = flags;
    surface->handle         = window;
    surface->dimensions     = surface_dimensions;

    surface->is_active = !create_hidden;

    return true;
}
void platform_surface_destroy( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    SDL_DestroyWindow( linux_surface->handle );
    mem_zero( linux_surface, sizeof(LinuxSurface) );
}
void platform_surface_show( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    linux_surface->is_active = true;
    if( linux_surface->on_activate ) {
        linux_surface->on_activate(
            linux_surface,
            true,
            linux_surface->on_activate_user_params
        );
    }
    SDL_ShowWindow( linux_surface->handle );
}
void platform_surface_hide( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    SDL_HideWindow( linux_surface->handle );
}
void platform_surface_set_dimensions(
    PlatformSurface* surface, ivec2 dimensions
) {
    LinuxSurface* linux_surface = surface;
    if( linux_surface->mode == PLATFORM_SURFACE_MODE_FULLSCREEN ) {
        return;
    }
    SDL_SetWindowSize( linux_surface->handle, dimensions.x, dimensions.y );
}
ivec2 platform_surface_query_dimensions( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    ivec2 result = {};
    SDL_GetWindowSize( linux_surface->handle, &result.x, &result.y );
    return result;
}
void platform_surface_set_mode(
    PlatformSurface* surface, PlatformSurfaceMode mode
) {
    LinuxSurface* linux_surface = surface;
    linux_surface->mode = mode;
    switch( mode ) {
        case PLATFORM_SURFACE_MODE_FLOATING_WINDOW: {
            SDL_SetWindowFullscreen( linux_surface->handle, 0 );
        } break;
        case PLATFORM_SURFACE_MODE_FULLSCREEN: {
            SDL_SetWindowFullscreen(
                linux_surface->handle,
                SDL_WINDOW_FULLSCREEN_DESKTOP
            );
        } break;
    }
}
PlatformSurfaceMode platform_surface_query_mode( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    return linux_surface->mode;
}
void platform_surface_set_name( PlatformSurface* surface, const char* name ) {
    LinuxSurface* linux_surface = surface;
    SDL_SetWindowTitle( linux_surface->handle, name );
}
void platform_surface_query_name(
    PlatformSurface* surface, usize* surface_name_buffer_size,
    char* surface_name_buffer
) {
    LinuxSurface* linux_surface = surface;
    const char* title = SDL_GetWindowTitle( linux_surface->handle );
    usize title_len = str_length( title );
    if( !surface_name_buffer ) {
        *surface_name_buffer_size = title_len;
        return;
    }
    
    StringView name;
    name.buffer = surface_name_buffer;
    name.len    = *surface_name_buffer_size;

    usize max_copy = title_len > name.len ? name.len : title_len;
    mem_copy( name.buffer, title, max_copy );
}
b32 platform_surface_query_active( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    return linux_surface->is_active;
}
void platform_surface_center( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    SDL_SetWindowPosition(
        linux_surface->handle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED
    );
}
void platform_surface_pump_events( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    unused(linux_surface);
    SDL_PumpEvents();
    SDL_Event event = {};
    while( SDL_PollEvent( &event ) > 0 ) {
        switch( event.type ) {
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                if( !linux_surface->is_active ) {
                    break;
                }
                b32 is_down = event.key.state == SDL_PRESSED;
                KeyboardCode key = sdl_key_to_keycode( event.key.keysym );
                input_set_key( key, is_down );
            } break;
            case SDL_MOUSEMOTION: {
                // TODO(alicia): check where x,y origin is, should be
                // bottom left corner
                i32 x = event.motion.x;
                i32 y = event.motion.y;
                ivec2 mousepos = { x, y };
                
                input_set_mouse_position( mousepos );
            } break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                if( !linux_surface->is_active ) {
                    break;
                }
                b32 is_down = event.button.state == SDL_PRESSED;
                MouseCode mouse_button = MOUSE_BUTTON_UNKNOWN;

                switch( event.button.button ) {
                    case SDL_BUTTON_LEFT: {
                        mouse_button = MOUSE_BUTTON_LEFT;
                    } break;
                    case SDL_BUTTON_RIGHT: {
                        mouse_button = MOUSE_BUTTON_RIGHT;
                    } break;
                    case SDL_BUTTON_MIDDLE: {
                        mouse_button = MOUSE_BUTTON_MIDDLE;
                    } break;
                    case SDL_BUTTON_X1: {
                        mouse_button = MOUSE_BUTTON_EXTRA_1;
                    } break;
                    case SDL_BUTTON_X2: {
                        mouse_button = MOUSE_BUTTON_EXTRA_2;
                    } break;
                }

                input_set_mouse_button( mouse_button, is_down );
            } break;
            case SDL_MOUSEWHEEL: {
                if( !linux_surface->is_active ) {
                    break;
                }

                i32 x = event.wheel.x;
                i32 y = event.wheel.y;

                input_set_horizontal_mouse_wheel( x );
                input_set_mouse_wheel( y );
            } break;
            case SDL_WINDOWEVENT: {
                switch( event.window.event ) {
                    case SDL_WINDOWEVENT_CLOSE: {
                        if( linux_surface->on_close ) {
                            linux_surface->on_close(
                                linux_surface,
                                linux_surface->on_close_user_params
                            );
                        }
                    } break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED: {
                        if( linux_surface->on_activate ) {
                            linux_surface->on_activate(
                                linux_surface,
                                true,
                                linux_surface->on_activate_user_params
                            );
                        }
                    } break;
                    case SDL_WINDOWEVENT_FOCUS_LOST: {
                        if( linux_surface->on_activate ) {
                            linux_surface->on_activate(
                                linux_surface,
                                false,
                                linux_surface->on_activate_user_params
                            );
                        }
                    } break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        #define MIN_DIMENSIONS 1
                        local ivec2 last_size = {};
                        ivec2 new_size = {
                            max( event.window.data1, MIN_DIMENSIONS ),
                            max( event.window.data2, MIN_DIMENSIONS )
                        };
                        if( !iv2_cmp_eq( last_size, new_size ) ) {
                            if( linux_surface->on_resize ) {
                                linux_surface->on_resize(
                                    linux_surface,
                                    last_size,
                                    new_size,
                                    linux_surface->on_resize_user_params
                                );
                            }
                            linux_surface->dimensions = new_size;
                        }
                    } break;
                    default: {} break;
                }
            } break;
            default: {}break;
        }
    }
}
void platform_surface_set_close_callback(
    PlatformSurface* surface,
    PlatformSurfaceOnCloseFN* close_callback,
    void* user_params
) {
    LinuxSurface* s         = surface;
    s->on_close             = close_callback;
    s->on_close_user_params = user_params;
}
void platform_surface_clear_close_callback( PlatformSurface* surface ) {
    LinuxSurface* s         = surface;
    s->on_close             = NULL; 
    s->on_close_user_params = NULL; 
}
void platform_surface_set_resize_callback(
    PlatformSurface* surface,
    PlatformSurfaceOnResizeFN* resize_callback,
    void* user_params
) {
    LinuxSurface* s          = surface;
    s->on_resize             = resize_callback;
    s->on_resize_user_params = user_params;
}
void platform_surface_clear_resize_callback( PlatformSurface* surface ) {
    LinuxSurface* s          = surface;
    s->on_resize             = NULL;
    s->on_resize_user_params = NULL;
}
void platform_surface_set_activate_callback(
    PlatformSurface* surface,
    PlatformSurfaceOnActivateFN* activate_callback,
    void* user_params
) {
    LinuxSurface* s            = surface;
    s->on_activate             = activate_callback;
    s->on_activate_user_params = user_params;
}
void platform_surface_clear_activate_callback( PlatformSurface* surface ) {
    LinuxSurface* s          = surface;
    s->on_activate             = NULL;
    s->on_activate_user_params = NULL;
}

f64 platform_us_elapsed(void) {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
    time_t seconds_elapsed     = ts.tv_sec  - START_TIME.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - START_TIME.tv_nsec;
    f64 microseconds_elapsed =
        ( (f64)seconds_elapsed     * 1000000.0 ) +
        ( (f64)nanoseconds_elapsed / 1000.0 );
    return microseconds_elapsed;
}
f64 platform_ms_elapsed(void) {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
    time_t seconds_elapsed     = ts.tv_sec  - START_TIME.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - START_TIME.tv_nsec;
    f64 milliseconds_elapsed =
        ( (f64)seconds_elapsed     * 1000.0 ) +
        ( (f64)nanoseconds_elapsed / 1000000.0 );
    return milliseconds_elapsed;
}
f64 platform_s_elapsed(void) {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
    time_t seconds_elapsed     = ts.tv_sec  - START_TIME.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - START_TIME.tv_nsec;
    return 
        ( (f64)seconds_elapsed ) +
        ( (f64)nanoseconds_elapsed / 1000000000.0 );
}
void platform_sleep( u32 ms ) {

#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = ( ms % 1000 ) * 1000 * 1000;

    nanosleep( &ts, NULL );
#else
    if( ms >= 1000 ) {
        sleep( ms / 1000 );
    }
    usleep( (ms % 1000) * 1000 );
#endif

}

CursorStyle platform_cursor_style(void) {
    // TODO(alicia): 
    return CURSOR_STYLE_ARROW;
}
b32 platform_cursor_visible(void) {
    // TODO(alicia): 
    return true;
}
void platform_cursor_set_style( CursorStyle cursor_style ) {
    // TODO(alicia): 
    unused(cursor_style);
}
void platform_cursor_set_visible( b32 visible ) {
    // TODO(alicia): 
    unused(visible);
}
void platform_cursor_center( PlatformSurface* surface ) {
    // TODO(alicia): 
    unused(surface);
}

global SDL_GameController* SDL_CONTROLLERS[GAMEPAD_MAX_INDEX];
void platform_poll_gamepad(void) {
    for( u32 i = 0; i < GAMEPAD_MAX_INDEX; ++i ) {
        if( SDL_IsGameController( i ) ) {
            SDL_CONTROLLERS[i] = SDL_GameControllerOpen( i );
        } else {
            continue;
        }
        u32 gamepad_index = i;

        SDL_GameController* controller = SDL_CONTROLLERS[i];

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_LEFT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_RIGHT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_UP,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_DPAD_UP )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_DOWN,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_LEFT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_X )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_RIGHT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_B )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_UP,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_Y )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_DOWN,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_A )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_START,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_START )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_SELECT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_BACK )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_BUMPER_LEFT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_BUMPER_RIGHT,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_LEFT_CLICK,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_LEFTSTICK )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_RIGHT_CLICK,
            SDL_GameControllerGetButton( controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK )
        );

        f32 trigger_press_threshold =
            input_gamepad_trigger_press_threshold( gamepad_index );
        f32 trigger_left_deadzone =
            input_gamepad_trigger_left_deadzone( gamepad_index );
        f32 trigger_right_deadzone =
            input_gamepad_trigger_right_deadzone( gamepad_index );

        Sint16 trigger_left_s16 =
            SDL_GameControllerGetAxis( controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT );
        Sint16 trigger_right_s16 =
            SDL_GameControllerGetAxis( controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT );
        
        f32 trigger_left  = normalize_range_i16_f32( trigger_left_s16 );
        f32 trigger_right = normalize_range_i16_f32( trigger_right_s16 );

        if( trigger_left >= trigger_left_deadzone ) {
            trigger_left = remap32(
                trigger_left_deadzone, 1.0f,
                0.0f, 1.0f,
                trigger_left
            );
        } else {
            trigger_left = 0;
        }
        if( trigger_right >= trigger_right_deadzone ) {
            trigger_right = remap32(
                trigger_right_deadzone, 1.0f,
                0.0f, 1.0f,
                trigger_right
            );
        } else {
            trigger_right = 0;
        }

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_TRIGGER_LEFT,
            trigger_left >= trigger_press_threshold
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_TRIGGER_RIGHT,
            trigger_right >= trigger_press_threshold
        );

        input_set_gamepad_trigger_left(
            gamepad_index, trigger_left );
        input_set_gamepad_trigger_right(
            gamepad_index, trigger_right );

        i16 stick_left_x_s16 = SDL_GameControllerGetAxis( controller, SDL_CONTROLLER_AXIS_LEFTX );
        i16 stick_left_y_s16 = SDL_GameControllerGetAxis( controller, SDL_CONTROLLER_AXIS_LEFTY );

        i16 stick_right_x_s16 = SDL_GameControllerGetAxis( controller, SDL_CONTROLLER_AXIS_RIGHTX );
        i16 stick_right_y_s16 = SDL_GameControllerGetAxis( controller, SDL_CONTROLLER_AXIS_RIGHTY );

        vec2 stick_left = {
            normalize_range_i16_f32( stick_left_x_s16 ),
            normalize_range_i16_f32( stick_left_y_s16 )
        };
        vec2 stick_right = {
            normalize_range_i16_f32( stick_right_x_s16 ),
            normalize_range_i16_f32( stick_right_y_s16 )
        };

        f32 stick_left_magnitude  = v2_mag( stick_left );
        f32 stick_right_magnitude = v2_mag( stick_right );

        vec2 stick_left_direction = stick_left_magnitude >= 0 ?
            v2_div( stick_left, stick_left_magnitude ) : VEC2_ZERO;
        vec2 stick_right_direction = stick_right_magnitude >= 0 ?
            v2_div( stick_right, stick_right_magnitude ) : VEC2_ZERO;

        f32 stick_left_deadzone  =
            input_gamepad_stick_left_deadzone( gamepad_index );
        f32 stick_right_deadzone =
            input_gamepad_stick_right_deadzone( gamepad_index );

        if( stick_left_magnitude >= stick_left_deadzone ) {
            stick_left_magnitude = remap32(
                stick_left_deadzone, 1.0f,
                0.0f, 1.0f,
                stick_left_magnitude
            );
        } else {
            stick_left_magnitude = 0;
        }
        if( stick_right_magnitude >= stick_right_deadzone ) {
            stick_right_magnitude = remap32(
                stick_right_deadzone, 1.0f,
                0.0f, 1.0f,
                stick_right_magnitude
            );
        } else {
            stick_right_magnitude = 0;
        }

        stick_left  = v2_mul( stick_left_direction, stick_left_magnitude );
        stick_right = v2_mul( stick_right_direction, stick_right_magnitude );
        
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_LEFT,
            stick_left_magnitude  >= 0
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_RIGHT,
            stick_right_magnitude >= 0
        );
        
        input_set_gamepad_stick_left(
            gamepad_index,
            stick_left
        );

        input_set_gamepad_stick_right(
            gamepad_index,
            stick_right
        );
    }
}
typedef struct SDLControllerMotorState {
    f32 left, right;
} SDLControllerMotorState;
global SDLControllerMotorState SDL_CONTROLLER_MOTOR_STATES[GAMEPAD_MAX_INDEX];
void platform_set_gamepad_motor_state(
    u32 gamepad_index,
    u32 motor, f32 value
) {
    ASSERT( gamepad_index < GAMEPAD_MAX_INDEX );
    SDL_GameController* controller = SDL_CONTROLLERS[gamepad_index];
    if( !controller ) {
        LINUX_LOG_WARN( "Attempted to set motor state of disconnected controller {u}!", gamepad_index );
        return;
    }
    SDLControllerMotorState motor_state = SDL_CONTROLLER_MOTOR_STATES[gamepad_index];

    f32 left  = motor == 0 ? value : motor_state.left;
    f32 right = motor == 1 ? value : motor_state.right;

    u16 left_normalized  = normalize_range_f32_u16( left );
    u16 right_normalized = normalize_range_f32_u16( right );

    motor_state.left  = motor == 0 ? value : motor_state.left;
    motor_state.right = motor == 1 ? value : motor_state.right;

    SDL_GameControllerRumble(
        controller,
        left_normalized, right_normalized,
        10000
    );
}
void platform_gl_surface_swap_buffers( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    SDL_GL_SwapWindow( linux_surface->handle );
}

void* linux_gl_load_proc( const char* function_name ) {
    void* function = (void*)SDL_GL_GetProcAddress( function_name );
#if defined(LD_LOGGING)
    if( !function ) {
        LINUX_LOG_WARN(
            "Failed to load GL function \"{cc}\"!",
            function_name
        );
    }
#endif
    return function;
}


global b32 GL_FUNCTIONS_LOADED = false;
b32 platform_gl_surface_init( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    ASSERT( linux_surface->backend == RENDERER_BACKEND_OPENGL );

    SDL_GLContext glrc = SDL_GL_CreateContext( linux_surface->handle );
    if( !glrc ) {
        return false;
    }

    if( !GL_FUNCTIONS_LOADED ) {
        if( !gl_load_functions( linux_gl_load_proc ) ) {
            return false;
        }
        GL_FUNCTIONS_LOADED = true;
    }

    linux_surface->glrc = glrc;
    return true;
}
void platform_gl_surface_shutdown( PlatformSurface* surface ) {
    LinuxSurface* linux_surface = surface;
    ASSERT( linux_surface->backend == RENDERER_BACKEND_OPENGL );
    SDL_GL_DeleteContext( linux_surface->glrc );
}

MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
) {
    SDL_MessageBoxData data = {};
    data.title   = window_title;
    data.message = message;
    data.window  = NULL;

    switch( icon ) {
        case MESSAGE_BOX_ICON_INFORMATION: {
            data.flags = SDL_MESSAGEBOX_INFORMATION;
        } break;
        case MESSAGE_BOX_ICON_WARNING: {
            data.flags = SDL_MESSAGEBOX_WARNING;
        } break;
        case MESSAGE_BOX_ICON_ERROR: {
            data.flags = SDL_MESSAGEBOX_ERROR;
        } break;
        default: { UNIMPLEMENTED(); } return 0;
    }

    SDL_MessageBoxButtonData buttons[3] = {};
    
    switch( type ) {
        case MESSAGE_BOX_TYPE_OK: {
            data.numbuttons = 1;
            data.buttons    = buttons;

            buttons[0].buttonid = 0;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].text     = "Ok";
        } break;
        case MESSAGE_BOX_TYPE_OKCANCEL: {
            data.numbuttons = 2;
            data.buttons    = buttons;

            buttons[0].buttonid = 0;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].text     = "Ok";

            buttons[1].buttonid = 1;
            buttons[1].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].text     = "Cancel";
        } break;
        case MESSAGE_BOX_TYPE_RETRYCANCEL: {
            data.numbuttons = 2;
            data.buttons    = buttons;

            buttons[0].buttonid = 0;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].text     = "Retry";

            buttons[1].buttonid = 1;
            buttons[1].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].text     = "Cancel";
        } break;
        case MESSAGE_BOX_TYPE_YESNO: {
            data.numbuttons = 2;
            data.buttons    = buttons;

            buttons[0].buttonid = 0;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].text     = "Yes";

            buttons[1].buttonid = 1;
            buttons[1].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].text     = "No";
        } break;
        case MESSAGE_BOX_TYPE_YESNOCANCEL: {
            data.numbuttons = 3;
            data.buttons    = buttons;

            buttons[0].buttonid = 0;
            buttons[0].flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            buttons[0].text     = "Yes";

            buttons[1].buttonid = 1;
            buttons[1].text     = "No";

            buttons[2].buttonid = 2;
            buttons[2].flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].text     = "Cancel";
        } break;
        default: {
            UNIMPLEMENTED();
        } return 0;
    }

    int button_id = 0;
    SDL_ShowMessageBox( &data, &button_id );

    switch( type ) {
        case MESSAGE_BOX_TYPE_OK: {
            return MESSAGE_BOX_RESULT_OK;
        } break;
        case MESSAGE_BOX_TYPE_OKCANCEL: {
            if( button_id ) {
                return MESSAGE_BOX_RESULT_CANCEL;
            } else {
                return MESSAGE_BOX_RESULT_OK;
            }
        } break;
        case MESSAGE_BOX_TYPE_RETRYCANCEL: {
            if( button_id ) {
                return MESSAGE_BOX_RESULT_CANCEL;
            } else {
                return MESSAGE_BOX_RESULT_RETRY;
            }
        } break;
        case MESSAGE_BOX_TYPE_YESNO: {
            if( button_id ) {
                return MESSAGE_BOX_RESULT_NO;
            } else {
                return MESSAGE_BOX_RESULT_YES;
            }
        } break;
        case MESSAGE_BOX_TYPE_YESNOCANCEL: {
            switch( button_id ) {
                case 0: return MESSAGE_BOX_RESULT_YES;
                case 1: return MESSAGE_BOX_RESULT_NO;
                case 2: return MESSAGE_BOX_RESULT_CANCEL;
            }
        } break;
        default: {
            UNIMPLEMENTED();
        } return 0;
    }

    return 0;
}


PlatformLibrary* platform_library_load( const char* library_path ) {
    return dlopen( library_path, RTLD_LAZY );
}
void platform_library_free( PlatformLibrary* library ) {
    dlclose( library );
}
void* platform_library_load_function(
    PlatformLibrary* library,
    const char*      function_name
) {
    return dlsym( library, function_name );
}

KeyboardCode sdl_key_to_keycode( SDL_Keysym key ) {

    switch( key.sym ) {
        case SDLK_BACKSPACE:    return KEY_BACKSPACE;
        case SDLK_TAB:          return KEY_TAB;
        case SDLK_RETURN:       return KEY_ENTER;
        case SDLK_LSHIFT:       return KEY_SHIFT_LEFT;
        case SDLK_RSHIFT:       return KEY_SHIFT_RIGHT;
        case SDLK_LCTRL:        return KEY_CONTROL_LEFT;
        case SDLK_RCTRL:        return KEY_CONTROL_RIGHT;
        case SDLK_LALT:         return KEY_ALT_LEFT;
        case SDLK_RALT:         return KEY_ALT_RIGHT;
        case SDLK_ESCAPE:       return KEY_ESCAPE;
        case SDLK_SPACE:        return KEY_SPACE;
        case SDLK_PAGEUP:       return KEY_PAGE_UP;
        case SDLK_PAGEDOWN:     return KEY_PAGE_DOWN;
        case SDLK_END:          return KEY_END;
        case SDLK_HOME:         return KEY_HOME;
        case SDLK_LEFT:         return KEY_ARROW_LEFT;
        case SDLK_UP:           return KEY_ARROW_UP;
        case SDLK_RIGHT:        return KEY_ARROW_RIGHT;
        case SDLK_DOWN:         return KEY_ARROW_DOWN;
        case SDLK_PRINTSCREEN:  return KEY_PRINT_SCREEN;
        case SDLK_INSERT:       return KEY_INSERT;
        case SDLK_DELETE:       return KEY_DELETE;
        case SDLK_0 ... SDLK_9: return (KeyboardCode)key.sym;
        case SDLK_a ... SDLK_z: {
            u8 offset  = (u8)(key.sym - SDLK_a);
            return (KeyboardCode)((u8)KEY_A + offset);
        } break;
        case SDLK_LGUI:      return KEY_SUPER_LEFT;
        case SDLK_RGUI:      return KEY_SUPER_RIGHT;
        case SDLK_KP_0:      return KEY_PAD_0;
        case SDLK_KP_1:      return KEY_PAD_1;
        case SDLK_KP_2:      return KEY_PAD_2;
        case SDLK_KP_3:      return KEY_PAD_3;
        case SDLK_KP_4:      return KEY_PAD_4;
        case SDLK_KP_5:      return KEY_PAD_5;
        case SDLK_KP_6:      return KEY_PAD_6;
        case SDLK_KP_7:      return KEY_PAD_7;
        case SDLK_KP_8:      return KEY_PAD_8;
        case SDLK_KP_9:      return KEY_PAD_9;

        case SDLK_F1:  return KEY_F1;
        case SDLK_F2:  return KEY_F2;
        case SDLK_F3:  return KEY_F3;
        case SDLK_F4:  return KEY_F4;
        case SDLK_F5:  return KEY_F5;
        case SDLK_F6:  return KEY_F6;
        case SDLK_F7:  return KEY_F7;
        case SDLK_F8:  return KEY_F8;
        case SDLK_F9:  return KEY_F9;
        case SDLK_F10: return KEY_F10;
        case SDLK_F11: return KEY_F11;
        case SDLK_F12: return KEY_F12;
        case SDLK_F13: return KEY_F13;
        case SDLK_F14: return KEY_F14;
        case SDLK_F15: return KEY_F15;
        case SDLK_F16: return KEY_F16;
        case SDLK_F17: return KEY_F17;
        case SDLK_F18: return KEY_F18;
        case SDLK_F19: return KEY_F19;
        case SDLK_F20: return KEY_F20;
        case SDLK_F21: return KEY_F21;
        case SDLK_F22: return KEY_F22;
        case SDLK_F23: return KEY_F23;
        case SDLK_F24: return KEY_F24;

        case SDLK_NUMLOCKCLEAR: return KEY_NUM_LOCK;
        case SDLK_SCROLLLOCK:   return KEY_SCROLL_LOCK;

        case SDLK_SEMICOLON:    return KEY_SEMICOLON;
        case SDLK_EQUALS:       return KEY_EQUALS;
        case SDLK_MINUS:        return KEY_MINUS;
        case SDLK_COMMA:        return KEY_COMMA;
        case SDLK_PERIOD:       return KEY_PERIOD;
        case SDLK_SLASH:        return KEY_SLASH_FORWARD;
        case SDLK_BACKQUOTE:    return KEY_BACKTICK;
        case SDLK_LEFTBRACKET:  return KEY_BRACKET_LEFT;
        case SDLK_RIGHTBRACKET: return KEY_BRACKET_RIGHT;
        case SDLK_BACKSLASH:    return KEY_SLASH_BACKWARD;
        case SDLK_QUOTE:        return KEY_QUOTE;

        case SDLK_PAUSE:     return KEY_PAUSE;
        case SDLK_CAPSLOCK:  return KEY_CAPSLOCK;
        default: return KEY_UNKNOWN;
    }
}

#if 0
KeyboardCode x_key_to_keycode( u32 x_key ) {
    switch( x_key ) {
        case XK_BackSpace:   return KEY_BACKSPACE;
        case XK_Return:      return KEY_ENTER;
        case XK_Tab:         return KEY_TAB;
        case XK_Pause:       return KEY_PAUSE;
        case XK_Caps_Lock:   return KEY_CAPSLOCK;
        case XK_Escape:      return KEY_ESCAPE;
        case XK_space:       return KEY_SPACE;
        case XK_Prior:       return KEY_PAGE_UP;
        case XK_Next:        return KEY_PAGE_DOWN;
        case XK_End:         return KEY_END;
        case XK_Home:        return KEY_HOME;
        case XK_Print:       return KEY_PRINT_SCREEN;
        case XK_Insert:      return KEY_INSERT;
        case XK_Delete:      return KEY_DELETE;
        case XK_Meta_L:      return KEY_SUPER_LEFT;
        case XK_Meta_R:      return KEY_SUPER_RIGHT;
        case XK_Num_Lock:    return KEY_NUM_LOCK;
        case XK_Scroll_Lock: return KEY_SCROLL_LOCK;
        case XK_Shift_L:     return KEY_SHIFT_LEFT;
        case XK_Shift_R:     return KEY_SHIFT_RIGHT;
        case XK_Control_L:   return KEY_CONTROL_LEFT;
        case XK_Control_R:   return KEY_CONTROL_RIGHT;
        case XK_Alt_L:       return KEY_ALT_LEFT;
        case XK_Alt_R:       return KEY_ALT_RIGHT;
        case XK_semicolon:   return KEY_SEMICOLON;
        case XK_comma:       return KEY_COMMA;
        case XK_minus:       return KEY_MINUS;
        case XK_period:      return KEY_PERIOD;
        case XK_slash:       return KEY_SLASH_FORWARD;
        case XK_grave:       return KEY_BACKTICK;

        case XK_Left:  return KEY_ARROW_LEFT;
        case XK_Right: return KEY_ARROW_RIGHT;
        case XK_Up:    return KEY_ARROW_UP;
        case XK_Down:  return KEY_ARROW_DOWN;

        case '0' ... '9':
            return (KeyboardCode)((u32)KEY_0 + (x_key - '0'));

        case XK_KP_0 ... XK_KP_9:
            return (KeyboardCode)((u32)KEY_PAD_0 + (x_key - XK_KP_0));

        case 'A' ... 'Z':
            return (KeyboardCode)(x_key);
        case 'a' ... 'z':
            return (KeyboardCode)(x_key - 32);

        case XK_KP_Equal:    
        case XK_plus:     return KEY_EQUALS;

        case XK_F1:  return KEY_F1; 
        case XK_F2:  return KEY_F2; 
        case XK_F3:  return KEY_F3; 
        case XK_F4:  return KEY_F4; 
        case XK_F5:  return KEY_F5; 
        case XK_F6:  return KEY_F6; 
        case XK_F7:  return KEY_F7; 
        case XK_F8:  return KEY_F8; 
        case XK_F9:  return KEY_F9; 
        case XK_F10: return KEY_F10; 
        case XK_F11: return KEY_F11; 
        case XK_F12: return KEY_F12; 
        case XK_F13: return KEY_F13; 
        case XK_F14: return KEY_F14; 
        case XK_F15: return KEY_F15; 
        case XK_F16: return KEY_F16; 
        case XK_F17: return KEY_F17; 
        case XK_F18: return KEY_F18; 
        case XK_F19: return KEY_F19; 
        case XK_F20: return KEY_F20; 
        case XK_F21: return KEY_F21; 
        case XK_F22: return KEY_F22; 
        case XK_F23: return KEY_F23; 
        case XK_F24: return KEY_F24; 

        case XK_KP_Decimal:
        case XK_KP_Add:
        case XK_KP_Separator:
        case XK_KP_Divide:
        case XK_multiply:
        case XK_Select:
        case XK_Help:
        case XK_Execute:
        case XK_Mode_switch: 
        default:
            return KEY_UNKNOWN;
    }
}
#endif

void platform_query_system_info( struct SystemInfo* out_info ) {
    mem_zero( out_info, sizeof(SystemInfo) );
    out_info->logical_processor_count = sysconf( _SC_NPROCESSORS_ONLN );
    struct sysinfo sys_info = {};
    sysinfo( &sys_info );
    out_info->total_memory = sys_info.totalram;
    out_info->cpu_name_buffer[0] = ' ';
    out_info->cpu_name_buffer[1] = 0;
    out_info->features = SSE_MASK | SSE2_MASK | SSE3_MASK | SSSE3_MASK | SSE4_1_MASK | SSE4_2_MASK | AVX_MASK | AVX2_MASK;
}

#endif // platform linux

