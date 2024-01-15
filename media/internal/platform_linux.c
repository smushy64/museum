/**
 * Description:  Media Library Linux Implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 14, 2024
*/
#include "shared/defines.h"

#if defined(LD_PLATFORM_LINUX)
#include "shared/constants.h"

#include "media/internal/logging.h"
#include "media/audio.h"
#include "media/input.h"
#include "media/surface.h"

#include "core/memory.h"
#include "core/string.h"

#include "SDL2/SDL.h"

global b32 global_initialized = false;

MEDIA_API b32 media_initialize(void) {
    if( global_initialized ) {
        return true;
    }

    // TODO(alicia): check for SDL2 version!
    int error = SDL_Init(
        SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK |
        SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS );
    if( error ) {
        media_log_error( "failed to initialize SDL2! SDL: {cc}", SDL_GetError() );
        return false;
    }

    global_initialized = true;
    return true;
}

MEDIA_API void media_shutdown(void) {
    if( global_initialized ) {
        SDL_Quit();
    }

    global_initialized = false;
}

struct LinuxSDLMediaSurface {
    SDL_Window* handle;
    char name[MEDIA_SURFACE_NAME_CAPACITY];
    u8   name_len;
    MediaSurfaceCallbackFN* callback;
    void* callback_params;
    MediaSurfaceFlags flags;
    i32 windowed_w, windowed_h;

    MediaSurfaceGraphicsBackend backend;

    union {
        struct {
            SDL_GLContext ctx;
        } gl;
    };
};
static_assert(
    sizeof( struct LinuxSDLMediaSurface ) <= sizeof(MediaSurface),
    "exceeded MEDIA_SURFACE_OPAQUE_DATA_SIZE!" );

#define get_surface_named( surface ) struct LinuxSDLMediaSurface* lsurface = (struct LinuxSDLMediaSurface*)surface
#define get_surface() get_surface_named(surface)

MEDIA_API b32 media_surface_create(
    i32 width, i32 height, u32 name_len, const char* name, MediaSurfaceFlags flags,
    MediaSurfaceCallbackFN* opt_callback, void* opt_callback_params,
    MediaSurfaceGraphicsBackend backend, MediaSurface* out_surface
) {
    #define error( format, ... )\
        media_log_error( "create surface: " format, ##__VA_ARGS__ )

    struct LinuxSDLMediaSurface surface = {};

    b32 result = true;

    /* create window */ {
        StringBuffer name_buffer = {};
        name_buffer.buffer   = surface.name;
        name_buffer.capacity = MEDIA_SURFACE_NAME_CAPACITY;

        StringSlice name_slice = { (char*)name, name_len };

        if( string_buffer_fmt( &name_buffer, "{s}{0}", name_slice ) ) {
            name_buffer.buffer[name_buffer.len - 1] = 0;
        }

        Uint32 sdl_flags = 0;
        if( bitfield_check( flags, MEDIA_SURFACE_FLAG_HIDDEN ) ) {
            sdl_flags |= SDL_WINDOW_HIDDEN;
        }
        if( bitfield_check( flags, MEDIA_SURFACE_FLAG_RESIZEABLE ) ) {
            sdl_flags |= SDL_WINDOW_RESIZABLE;
        }
        if( bitfield_check( flags, MEDIA_SURFACE_FLAG_FULLSCREEN ) ) {
            sdl_flags |= SDL_WINDOW_BORDERLESS;
            SDL_DisplayMode display_mode = {};
            // TODO(alicia): check for error
            if( SDL_GetCurrentDisplayMode( 0, &display_mode ) ) {
                error( "failed to get display mode! SDL: {cc}", SDL_GetError() );
                result = false;
                goto media_surface_create_end;
            }

            width  = display_mode.w;
            height = display_mode.h;
        }

        surface.windowed_w = width;
        surface.windowed_h = height;

        switch( backend ) {
            case MEDIA_SURFACE_GRAPHICS_BACKEND_OPENGL: {
                sdl_flags |= SDL_WINDOW_OPENGL;
                SDL_GL_SetAttribute(
                    SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
                SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, GL_VERSION_MAJOR );
                SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, GL_VERSION_MINOR );
                SDL_GL_SetAttribute(
                    SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
            } break;
            default: {
                error( "provided backend is not supported! backend: {u,X}", backend );
                result = false;
                goto media_surface_create_end;
            } break;
        }

        SDL_Window* window = SDL_CreateWindow(
            name_buffer.buffer, width, height,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            sdl_flags );

        if( !window ) {
            error( "failed to create window! SDL: {cc}", SDL_GetError() );
            result = false;
            goto media_surface_create_end;
        }

        surface.handle = window;
    }

    #undef error

    surface.callback        = opt_callback;
    surface.callback_params = opt_callback_params;
    surface.flags           = flags;

media_surface_create_end:
    if( !result ) {
        if( surface.handle ) {
            SDL_DestroyWindow( surface.handle );
        }
    } else {
        memory_copy( out_surface, &surface, sizeof(surface) );
    }
    return result;
}
MEDIA_API void media_surface_destroy( MediaSurface* surface ) {
    get_surface();

    if( lsurface->handle ) {
        switch( lsurface->backend ) {
            case MEDIA_SURFACE_GRAPHICS_BACKEND_OPENGL: {
                if( lsurface->gl.ctx ) {
                    SDL_GL_DeleteContext( lsurface->gl.ctx );
                }
            } break;
        }

        SDL_DestroyWindow( lsurface->handle );
    }

    system_free( lsurface, sizeof( *lsurface ) );
}
MEDIA_API void media_surface_set_callback(
    MediaSurface* surface, MediaSurfaceCallbackFN* callback, void* params
) {
    get_surface();

    lsurface->callback        = callback;
    lsurface->callback_params = params;
}
MEDIA_API void media_surface_clear_callback( MediaSurface* surface ) {
    get_surface();
    lsurface->callback        = NULL;
    lsurface->callback_params = NULL;
}
MEDIA_API void media_surface_set_name(
    MediaSurface* surface, const char* ptr, u32 len
) {
    get_surface();

    StringBuffer title = {};
    title.buffer   = lsurface->name;
    title.capacity = MEDIA_SURFACE_NAME_CAPACITY;

    StringSlice name = {};
    name.buffer = (char*)ptr;
    name.len    = len;
    if( string_buffer_fmt( &title, "{s}{0}", name ) ) {
        title.buffer[title.len - 1] = 0;
    }

    SDL_SetWindowTitle( lsurface->handle, title.buffer );
}
MEDIA_API usize media_surface_query_name(
    MediaSurface* surface, char* buffer, u32 buffer_size
) {
    get_surface();

    if( !buffer ) {
        return lsurface->name_len;
    }

    u32 max_copy = buffer_size;
    if( max_copy > lsurface->name_len ) {
        max_copy = lsurface->name_len;
    }

    memory_copy( buffer, lsurface->name, max_copy );

    return lsurface->name_len - max_copy;
}
MEDIA_API void media_surface_set_dimensions(
    MediaSurface* surface, i32 new_width, i32 new_height
) {
    get_surface();

    if( !bitfield_check( lsurface->flags, MEDIA_SURFACE_FLAG_FULLSCREEN ) ) {
        SDL_SetWindowSize( lsurface->handle, new_width, new_height );
    }
}
MEDIA_API void media_surface_query_dimensions(
    MediaSurface* surface, i32* out_width, i32* out_height
) {
    get_surface();
    SDL_GetWindowSize( lsurface->handle, out_width, out_height );
}
MEDIA_API void media_surface_set_fullscreen(
    MediaSurface* surface, b32 is_fullscreen
) {
    get_surface();

    #define error( format, ... ) do{\
        StringSlice window_name = { (char*)lsurface->name, lsurface->name_len };\
        const char* state = is_fullscreen ? "fullscreen" : "windowed";\
        media_log_error(\
            "failed to make surface {s} {cc}!" format "SDL: {cc}",\
            window_name, state, ##__VA_ARGS__ , SDL_GetError() );\
    } while(0)

    if(
        bitfield_check( lsurface->flags, MEDIA_SURFACE_FLAG_FULLSCREEN ) ==
        is_fullscreen
    ) {
        return;
    }

    if( is_fullscreen ) {
        SDL_DisplayMode display_mode = {};
        int display_index = SDL_GetWindowDisplayIndex( lsurface->handle );
        if( display_index < 0 ) {
            error( "failed to get display index!" );
            return;
        }
        if( SDL_GetCurrentDisplayMode( display_index, &display_mode ) ) {
            error( "failed to get display mode!" );
            return;
        }

        SDL_SetWindowBordered( lsurface->handle, true );
        SDL_SetWindowSize( lsurface->handle, display_mode.w, display_mode.h );

        lsurface->flags |= MEDIA_SURFACE_FLAG_FULLSCREEN;
    } else {
        SDL_SetWindowBordered( lsurface->handle, false );
        SDL_SetWindowSize(
            lsurface->handle, lsurface->windowed_w, lsurface->windowed_h );

        lsurface->flags = bitfield_clear(
            lsurface->flags, MEDIA_SURFACE_FLAG_FULLSCREEN );
    }

    #undef error
}
MEDIA_API b32 media_surface_query_fullscreen( MediaSurface* surface ) {
    get_surface();
    return bitfield_check( lsurface->flags, MEDIA_SURFACE_FLAG_FULLSCREEN );
}
MEDIA_API void media_surface_set_hidden( MediaSurface* surface, b32 is_hidden ) {
    get_surface();

    if( bitfield_check( lsurface->flags, MEDIA_SURFACE_FLAG_HIDDEN ) == is_hidden ) {
        return;
    }

    if( is_hidden ) {
        SDL_HideWindow( lsurface->handle );
        lsurface->flags |= MEDIA_SURFACE_FLAG_HIDDEN;
    } else {
        SDL_ShowWindow( lsurface->handle );
        lsurface->flags =
            bitfield_clear( lsurface->flags, MEDIA_SURFACE_FLAG_HIDDEN );
    }
}
MEDIA_API b32 media_surface_query_hidden( MediaSurface* surface ) {
    get_surface();
    return bitfield_check( lsurface->flags, MEDIA_SURFACE_FLAG_HIDDEN );
}
MEDIA_API void media_surface_center( MediaSurface* surface ) {
    get_surface();
    SDL_SetWindowPosition(
        lsurface->handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
}
MEDIA_API void media_surface_cursor_center( MediaSurface* surface ) {
    i32 width, height;
    media_surface_query_dimensions( surface, &width, &height );

    i32 x = width / 2;
    i32 y = height / 2;

    get_surface();

    SDL_WarpMouseInWindow( lsurface->handle, x, y );
}

void ___callback(
    struct LinuxSDLMediaSurface* surface, struct MediaSurfaceCallbackData* data
) {
    if( surface->callback ) {
        surface->callback( (MediaSurface*)surface, data, surface->callback_params );
    }
}

MEDIA_API void media_surface_pump_events( MediaSurface* surface ) {
    get_surface();
    #define call()\
        ___callback( lsurface, &data )

    // TODO(alicia): properly handle multiple windows!
    SDL_Event event = {};
    while( SDL_PollEvent( &event ) ) {
        MediaSurfaceCallbackData data = {};
        switch( event.type ) {
            case SDL_QUIT: {
                data.type = MEDIA_SURFACE_CALLBACK_TYPE_CLOSE;
                call();
            } break;
            case SDL_WINDOWEVENT: {
                SDL_WindowEvent wevent = event.window;
                switch( wevent.event ) {
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                    case SDL_WINDOWEVENT_FOCUS_LOST: {
                        data.type = MEDIA_SURFACE_CALLBACK_TYPE_ACTIVATE;
                        data.activate.is_active =
                            wevent.event == SDL_WINDOWEVENT_FOCUS_GAINED;
                        call();
                    } break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                        if(
                            lsurface->windowed_w == wevent.data1 &&
                            lsurface->windowed_h == wevent.data2
                        ) {
                            break;
                        }

                        data.type = MEDIA_SURFACE_CALLBACK_TYPE_RESOLUTION_CHANGE;
                        data.resolution_change.old_width  = lsurface->windowed_w;
                        data.resolution_change.old_height = lsurface->windowed_h;
                        data.resolution_change.new_width  = wevent.data1;
                        data.resolution_change.new_height = wevent.data2;

                        lsurface->windowed_w = wevent.data1;
                        lsurface->windowed_h = wevent.data2;

                        call();
                    } break;
                }
            } break;
            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                data.type = MEDIA_SURFACE_CALLBACK_TYPE_KEYBOARD_KEY;
                data.key.is_down = event.type == SDL_KEYDOWN;
                // TODO(alicia): properly process key!
                data.key.key = event.key.keysym.sym;

                call();
            } break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN: {
                data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_BUTTON;
                data.mouse_button.is_down = event.type == SDL_MOUSEBUTTONDOWN;
                data.mouse_button.button  = event.button.button;

                call();
            } break;
            case SDL_MOUSEWHEEL: {
                data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_WHEEL;
                if( event.wheel.x ) {
                    data.mouse_wheel.is_horizontal = true;
                    data.mouse_wheel.value         = event.wheel.x;

                    call();
                }

                if( event.wheel.y ) {
                    data.mouse_wheel.is_horizontal = false;
                    data.mouse_wheel.value         = event.wheel.y;

                    call();
                }
            } break;
            case SDL_MOUSEMOTION: {
                data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE;
                data.mouse_move.x = event.motion.x;
                data.mouse_move.y = event.motion.y;

                call();

                data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE_RELATIVE;
                data.mouse_move.x = event.motion.xrel;
                data.mouse_move.y = event.motion.yrel;

                call();
            } break;
        }
    }
    #undef call
}

MEDIA_API b32 media_surface_gl_init( MediaSurface* surface ) {
    get_surface();
    #define error( format, ... ) do {\
        StringSlice name = { (char*)lsurface->name, lsurface->name_len };\
        media_log_error( "gl: " format "surface: '{s}' SDL: {cc}", ##__VA_ARGS__, name, SDL_GetError() );\
    } while(0)

    SDL_GLContext context = SDL_GL_CreateContext( lsurface->handle );
    if( !context ) {
        error( "failed to create gl context!" );
        return false;
    }

    #undef error
    return true;
}
MEDIA_API void media_surface_gl_swap_buffers( MediaSurface* surface ) {
    get_surface();
    SDL_GL_SwapWindow( lsurface->handle );
}
MEDIA_API void media_surface_gl_swap_interval(
    MediaSurfaceHandle* surface_handle, int interval
) {
    unused(surface_handle);
    SDL_GL_SetSwapInterval( interval );
}
MEDIA_API void* media_gl_load_proc( const char* function_name ) {
    return SDL_GL_GetProcAddress( function_name );
}

MEDIA_API MediaMessageBoxResult media_message_box_blocking(
    const char* title, const char* message,
    MediaMessageBoxType type, MediaMessageBoxOptions options
) {
    SDL_MessageBoxData data = {};
    switch( type ) {
        case MEDIA_MESSAGE_BOX_TYPE_ERROR: {
            data.flags = SDL_MESSAGEBOX_ERROR;
        } break;
        case MEDIA_MESSAGE_BOX_TYPE_WARNING: {
            data.flags = SDL_MESSAGEBOX_WARNING;
        } break;
        case MEDIA_MESSAGE_BOX_TYPE_INFO: {
            data.flags = SDL_MESSAGEBOX_INFORMATION;
        } break;
    }

    data.title   = title;
    data.message = message;

    #define OK_BUTTON_INDEX     (0)
    #define CANCEL_BUTTON_INDEX (1)
    #define YES_BUTTON_INDEX    (0)
    #define NO_BUTTON_INDEX     (1)

    #define OK_BUTTON     (0)
    #define CANCEL_BUTTON (1)
    #define YES_BUTTON    (2)
    #define NO_BUTTON     (3)

    SDL_MessageBoxButtonData buttons[MEDIA_MESSAGE_BOX_MAX_BUTTON_COUNT] = {};
    int button_count = MEDIA_MESSAGE_BOX_MAX_BUTTON_COUNT;
    switch( options ) {
        case MEDIA_MESSAGE_BOX_OPTIONS_OK: {
            SDL_MessageBoxButtonData* ok = buttons + OK_BUTTON_INDEX;
            ok->buttonid = OK_BUTTON;
            ok->text     = "Ok";
            ok->flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            button_count = 1;
        } break;
        case MEDIA_MESSAGE_BOX_OPTIONS_OK_CANCEL: {
            SDL_MessageBoxButtonData* ok = buttons + OK_BUTTON_INDEX;
            ok->buttonid = OK_BUTTON;
            ok->text     = "Ok";
            ok->flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;

            SDL_MessageBoxButtonData* cancel = buttons + CANCEL_BUTTON_INDEX;
            cancel->buttonid = CANCEL_BUTTON;
            cancel->text     = "Cancel";
            cancel->flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        } break;
        case MEDIA_MESSAGE_BOX_OPTIONS_YES_NO: {
            SDL_MessageBoxButtonData* yes = buttons + YES_BUTTON_INDEX;
            yes->buttonid = YES_BUTTON;
            yes->text     = "Yes";
            yes->flags    = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;

            SDL_MessageBoxButtonData* no = buttons + NO_BUTTON_INDEX;
            no->buttonid = NO_BUTTON;
            no->text     = "No";
            no->flags    = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        } break;
    }

    data.numbuttons = button_count;
    data.buttons    = buttons;

    int button_hit = 0;
    int result = SDL_ShowMessageBox( &data, &button_hit );

    if( result < 0 ) {
        media_log_error(
            "media_message_box_blocking failed! SDL: {cc}", SDL_GetError() );
        return MEDIA_MESSAGE_BOX_RESULT_ERROR;
    }

    switch( button_hit ) {
        case OK_BUTTON:     return MEDIA_MESSAGE_BOX_RESULT_OK;
        case CANCEL_BUTTON: return MEDIA_MESSAGE_BOX_RESULT_CANCEL;
        case YES_BUTTON:    return MEDIA_MESSAGE_BOX_RESULT_YES;
        case NO_BUTTON:     return MEDIA_MESSAGE_BOX_RESULT_NO;
        default: unreachable();
    }

    #undef OK_BUTTON    
    #undef CANCEL_BUTTON
    #undef YES_BUTTON   
    #undef NO_BUTTON    
    #undef OK_BUTTON_INDEX
    #undef CANCEL_BUTTON_INDEX
    #undef YES_BUTTON_INDEX   
    #undef NO_BUTTON_INDEX    
}

MEDIA_API b32 media_input_query_gamepad_state(
    u32 gamepad_index, MediaGamepadState* out_state
) {
    if( !SDL_IsGameController( gamepad_index ) ) {
        return false;
    }

    SDL_GameController* controller = SDL_GameControllerOpen( gamepad_index );
    if( !controller ) {
        media_log_error(
            "failed to read valid game controller {u}! SDL: {cc}",
            gamepad_index, SDL_GetError() );
        return false;
    }

    #define button( enum )\
        SDL_GameControllerGetButton( controller, enum )

    if( button( SDL_CONTROLLER_BUTTON_A ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_FACE_DOWN;
    }
    if( button( SDL_CONTROLLER_BUTTON_X ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_FACE_LEFT;
    }
    if( button( SDL_CONTROLLER_BUTTON_Y ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_FACE_UP;
    }
    if( button( SDL_CONTROLLER_BUTTON_B ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_FACE_RIGHT;
    }

    if( button( SDL_CONTROLLER_BUTTON_DPAD_DOWN ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_DPAD_DOWN;
    }
    if( button( SDL_CONTROLLER_BUTTON_DPAD_LEFT ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_DPAD_LEFT;
    }
    if( button( SDL_CONTROLLER_BUTTON_DPAD_UP ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_DPAD_UP;
    }
    if( button( SDL_CONTROLLER_BUTTON_DPAD_RIGHT ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_DPAD_RIGHT;
    }

    if( button( SDL_CONTROLLER_BUTTON_LEFTSHOULDER ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_BUMPER_LEFT;
    }
    if( button( SDL_CONTROLLER_BUTTON_RIGHTSHOULDER ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_BUMPER_RIGHT;
    }

    if( button( SDL_CONTROLLER_BUTTON_LEFTSTICK ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_STICK_LEFT_CLICK;
    }
    if( button( SDL_CONTROLLER_BUTTON_RIGHTSTICK ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_STICK_RIGHT_CLICK;
    }

    if( button( SDL_CONTROLLER_BUTTON_START ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_START;
    }
    if( button( SDL_CONTROLLER_BUTTON_BACK ) ) {
        out_state->buttons |= MEDIA_GAMEPAD_BUTTON_SELECT;
    }

    #define axis( enum )\
        SDL_GameControllerGetAxis( controller, enum )

    out_state->stick_left_x  = axis( SDL_CONTROLLER_AXIS_LEFTX );
    out_state->stick_left_y  = axis( SDL_CONTROLLER_AXIS_LEFTY );
    out_state->stick_right_x = axis( SDL_CONTROLLER_AXIS_RIGHTX );
    out_state->stick_right_y = axis( SDL_CONTROLLER_AXIS_RIGHTY );

    f32 trigger_left  = (f32)axis( SDL_CONTROLLER_AXIS_TRIGGERLEFT ) / (f32)I16_MAX;
    f32 trigger_right = (f32)axis( SDL_CONTROLLER_AXIS_TRIGGERRIGHT ) / (f32)I16_MAX;

    out_state->trigger_left  = (u16)(trigger_left * (f32)U16_MAX);
    out_state->trigger_right = (u16)(trigger_right * (f32)U16_MAX);

    if( trigger_left > 0.25f ) {
        out_state->buttons |= MEDIA_GAMEPAD_EXT_BUTTON_TRIGGER_LEFT;
    }
    if( trigger_right > 0.25f ) {
        out_state->buttons |= MEDIA_GAMEPAD_EXT_BUTTON_TRIGGER_RIGHT;
    }

    SDL_GameControllerClose( controller );
    #undef button
    #undef axis
    return true;
}

global u16 global_motor_state[MEDIA_GAMEPAD_MAX_COUNT][MEDIA_GAMEPAD_MOTOR_COUNT] = {};

MEDIA_API b32 media_input_set_gamepad_rumble(
    u32 gamepad_index, u16 motor_left, u16 motor_right
) {
    if( !SDL_IsGameController( gamepad_index ) ) {
        return false;
    }

    SDL_Joystick* joystick = SDL_JoystickOpen( gamepad_index );
    if( !joystick ) {
        media_log_error(
            "failed to get valid gamepad {u}! SDL: {cc}",
            gamepad_index, SDL_GetError() );
        return false;
    }
    b32 result = true;

    if( !SDL_JoystickHasRumble( joystick ) ) {
        result = false;
        goto media_input_set_gamepad_rumble_end;
    }

    if( SDL_JoystickRumble( joystick, motor_left, motor_right, U32_MAX ) == -1 ) {
        result = false;
        goto media_input_set_gamepad_rumble_end;
    }
    global_motor_state[gamepad_index][0] = motor_left;
    global_motor_state[gamepad_index][1] = motor_right;

media_input_set_gamepad_rumble_end:
    SDL_JoystickClose( joystick );
    return result;
}
MEDIA_API void media_input_query_gamepad_rumble(
    u32 gamepad_index, u16* out_motor_left, u16* out_motor_right
) {
    assert( gamepad_index < MEDIA_GAMEPAD_MAX_COUNT );

    *out_motor_left  = global_motor_state[gamepad_index][0];
    *out_motor_right = global_motor_state[gamepad_index][1];
}
MEDIA_API void media_input_set_cursor_visible( b32 is_visible ) {
    SDL_ShowCursor( is_visible ? SDL_ENABLE : SDL_DISABLE );
}

// TODO(alicia): audio!
MEDIA_API b32 media_audio_initialize( u64 buffer_length_ms, MediaAudioContext* out_ctx ) {
    unused( buffer_length_ms, out_ctx );
    return true;
}
MEDIA_API void media_audio_shutdown( MediaAudioContext* ctx ) {
    unused(ctx);
}
MEDIA_API b32 media_audio_is_context_valid( MediaAudioContext* ctx ) {
    unused(ctx);
    return true;
}
MEDIA_API MediaAudioBufferFormat media_audio_query_buffer_format( MediaAudioContext* ctx ) {
    unused(ctx);
    return (MediaAudioBufferFormat){};
}
MEDIA_API b32 media_audio_buffer_lock( MediaAudioContext* ctx, MediaAudioBuffer* out_buffer ) {
    unused(ctx, out_buffer);
    return false;
}
MEDIA_API void media_audio_buffer_unlock( MediaAudioContext* ctx, MediaAudioBuffer* buffer ) {
    unused(ctx, buffer);
}
MEDIA_API void media_audio_start( MediaAudioContext* ctx ) {
    unused(ctx);
}
MEDIA_API void media_audio_stop( MediaAudioContext* ctx ) {
    unused(ctx);
}

#endif /* Platform Linux */

