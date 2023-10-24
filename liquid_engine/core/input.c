/**
 * Description:  Input implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 03, 2023
*/
#include "defines.h"
#include "constants.h"
#include "core/input.h"
#include "core/math.h"
#include "core/memory.h"
#include "core/internal.h"
#include "core/logging.h"

struct GamepadState {
    GamepadCode buttons;
    GamepadCode last_buttons;
    b8          is_active;

    union {
        struct {
            i16 normalized_stick_left_x;
            i16 normalized_stick_left_y;
        };
        u32 normalized_stick_left;
    };

    union {
        struct {
            i16 normalized_stick_right_x;
            i16 normalized_stick_right_y;
        };
        u32 normalized_stick_right;
    };

    union {
        struct {
            u16 normalized_trigger_left;
            u16 normalized_trigger_right;
        };
        u32 normalized_triggers;
    };

    u16 rumble_left, rumble_right;
};

struct KeyboardState {
    u8 buttons[KEY_COUNT];
    u8 last_buttons[KEY_COUNT];
};

struct MouseState {
    MouseCode buttons;
    MouseCode last_buttons;
    i32 x_absolute, y_absolute;
    f32 x_01, y_01;
    f32 x_rel, y_rel;
    i8  wheel_x, wheel_y;
    b8  was_locked, is_locked;
};

struct InputState {
    struct GamepadState  gamepad[INPUT_GAMEPAD_COUNT];
    struct KeyboardState keyboard;
    struct MouseState    mouse;
};

global struct InputState* global_input = NULL;

usize input_subsystem_query_size(void) {
    return sizeof( struct InputState );
}
void input_subsystem_initialize( void* buffer ) {
    global_input   = buffer;
}
void input_subsystem_swap_state(void) {
    memory_copy(
        global_input->keyboard.last_buttons,
        global_input->keyboard.buttons,
        sizeof( global_input->keyboard.buttons ) );
    global_input->mouse.last_buttons = global_input->mouse.buttons;

    if( global_input->mouse.is_locked ) {
        if( global_input->mouse.was_locked != global_input->mouse.is_locked ) {
            platform->io.set_mouse_visible( false );
        }
    } else {
        if( global_input->mouse.was_locked != global_input->mouse.is_locked ) {
            platform->io.set_mouse_visible( true );
        }
    }

    global_input->mouse.x_rel = 0.0f;
    global_input->mouse.y_rel = 0.0f;
    global_input->mouse.was_locked = global_input->mouse.is_locked;
}
void input_subsystem_update_gamepads(void) {
    PlatformGamepad platform_gamepads[INPUT_GAMEPAD_COUNT];
    platform->io.read_gamepads( platform_gamepads );

    for( usize i = 0; i < INPUT_GAMEPAD_COUNT; ++i ) {
        PlatformGamepad* platform_current = platform_gamepads + i;
        struct GamepadState* current = global_input->gamepad + i;

        if( !platform_current->is_active ) {
            if( current->is_active ) {
                memory_zero( current, sizeof(struct GamepadState) );
            }
            current->is_active = false;
        }
        current->is_active              = true;
        current->last_buttons           = current->buttons;
        current->buttons                = platform_current->buttons;
        current->normalized_stick_left  = platform_current->stick_left;
        current->normalized_stick_right = platform_current->stick_right;
        current->normalized_triggers    = platform_current->triggers;
    }
}
void input_subsystem_set_key( KeyCode code, b32 is_down ) {
    if( code != KEY_UNKNOWN ) {
        global_input->keyboard.buttons[code] = is_down;
    }
}
void input_subsystem_set_mouse_button( PlatformMouseCode code, b32 is_down ) {
    MouseCode mouse_code = 0;
    switch( code ) {
        case PLATFORM_MOUSE_BUTTON_LEFT: {
            mouse_code = MOUSE_BUTTON_LEFT;
        } break;
        case PLATFORM_MOUSE_BUTTON_MIDDLE: {
            mouse_code = MOUSE_BUTTON_MIDDLE;
        } break;
        case PLATFORM_MOUSE_BUTTON_RIGHT: {
            mouse_code = MOUSE_BUTTON_RIGHT;
        } break;
        case PLATFORM_MOUSE_BUTTON_EXTRA_1: {
            mouse_code = MOUSE_BUTTON_X1;
        } break;
        case PLATFORM_MOUSE_BUTTON_EXTRA_2: {
            mouse_code = MOUSE_BUTTON_X2;
        } break;
    }

    if( is_down ) {
        global_input->mouse.buttons |= mouse_code;
    } else {
        global_input->mouse.buttons &= ~(mouse_code);
    }
}
void input_subsystem_set_mouse_wheel( i32 wheel ) {
    global_input->mouse.wheel_y = wheel;
}
void input_subsystem_set_mouse_wheel_horizontal( i32 wheel ) {
    global_input->mouse.wheel_x = wheel;
}
void input_subsystem_set_mouse_position( i32 x, i32 y, f32 x01, f32 y01 ) {
    global_input->mouse.x_absolute = x;
    global_input->mouse.y_absolute = y;
    global_input->mouse.x_01 = x01;
    global_input->mouse.y_01 = y01;
}
void input_subsystem_set_mouse_relative( f32 x_rel, f32 y_rel ) {
    global_input->mouse.x_rel = x_rel;
    global_input->mouse.y_rel = y_rel;
}

LD_API b32 input_key_down( KeyCode code ) {
    b32 was_down = global_input->keyboard.last_buttons[code];
    b32 is_down  = global_input->keyboard.buttons[code];
    return was_down != is_down && is_down;
}
LD_API b32 input_key_up( KeyCode code ) {
    b32 was_down = global_input->keyboard.last_buttons[code];
    b32 is_down  = global_input->keyboard.buttons[code];
    return was_down != is_down && !is_down;
}
LD_API b32 input_key( KeyCode code ) {
    return global_input->keyboard.buttons[code];
}

LD_API b32 input_mouse_down( MouseCode code ) {
    b32 was_down = bitfield_check( global_input->mouse.last_buttons, code );
    b32 is_down  = bitfield_check( global_input->mouse.buttons, code );

    return was_down != is_down && is_down;
}
LD_API b32 input_mouse_up( MouseCode code ) {
    b32 was_down = bitfield_check( global_input->mouse.last_buttons, code );
    b32 is_down  = bitfield_check( global_input->mouse.buttons, code );

    return was_down != is_down && !is_down;
}
LD_API b32 input_mouse_button( MouseCode code ) {
    return bitfield_check( global_input->mouse.buttons, code );
}
LD_API i32 input_mouse_absolute_position_x(void) {
    return global_input->mouse.x_absolute;
}
LD_API i32 input_mouse_absolute_position_y(void) {
    return global_input->mouse.y_absolute;
}
LD_API f32 input_mouse_position_x(void) {
    return global_input->mouse.x_01;
}
LD_API f32 input_mouse_position_y(void) {
    return global_input->mouse.y_01;
}
LD_API f32 input_mouse_relative_x(void) {
    return global_input->mouse.x_rel;
}
LD_API f32 input_mouse_relative_y(void) {
    return global_input->mouse.y_rel;
}

LD_API i32 input_mouse_wheel(void) {
    return global_input->mouse.wheel_y;
}
LD_API i32 input_mouse_wheel_horizontal(void) {
    return global_input->mouse.wheel_x;
}
LD_API void input_mouse_set_locked( b32 is_locked ) {
    global_input->mouse.is_locked = is_locked;
}
LD_API b32 input_is_mouse_locked(void) {
    return global_input->mouse.is_locked;
}

LD_API b32 input_gamepad_down( usize gamepad, GamepadCode code ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return false;
    }
    b32 was_down = bitfield_check( global_input->gamepad[gamepad].last_buttons, code );
    b32 is_down  = bitfield_check( global_input->gamepad[gamepad].buttons, code );

    return was_down != is_down && is_down;
}
LD_API b32 input_gamepad_up( usize gamepad, GamepadCode code ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return false;
    }
    b32 was_down = bitfield_check( global_input->gamepad[gamepad].last_buttons, code );
    b32 is_down  = bitfield_check( global_input->gamepad[gamepad].buttons, code );

    return was_down != is_down && !is_down;
}
LD_API b32 input_gamepad_button( usize gamepad, GamepadCode code ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return false;
    }
    return bitfield_check( global_input->gamepad[gamepad].buttons, code );
}

LD_API f32 input_gamepad_stick_left_x( usize gamepad ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return 0.0f;
    }

    return normalize_range_i16_f32(
        global_input->gamepad[gamepad].normalized_stick_left_x );
}
LD_API f32 input_gamepad_stick_left_y( usize gamepad ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return 0.0f;
    }

    return normalize_range_i16_f32(
        global_input->gamepad[gamepad].normalized_stick_left_y );
}
LD_API f32 input_gamepad_stick_right_x( usize gamepad ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return 0.0f;
    }

    return normalize_range_i16_f32(
        global_input->gamepad[gamepad].normalized_stick_right_x );
}
LD_API f32 input_gamepad_stick_right_y( usize gamepad ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return 0.0f;
    }

    return normalize_range_i16_f32(
        global_input->gamepad[gamepad].normalized_stick_right_y );
}

LD_API f32 input_gamepad_trigger_left( usize gamepad ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return 0.0f;
    }

    return normalize_range_u16_f32(
        global_input->gamepad[gamepad].normalized_trigger_left );
}
LD_API f32 input_gamepad_trigger_right( usize gamepad ) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return 0.0f;
    }

    return normalize_range_u16_f32(
        global_input->gamepad[gamepad].normalized_trigger_right );
}

LD_API void input_gamepad_set_rumble(
    usize gamepad, f32 rumble_left, f32 rumble_right
) {
    if( !global_input->gamepad[gamepad].is_active ) {
        warn_log( "Attempted to rumble disconnected gamepad {usize}!", gamepad );
        return;
    }
    global_input->gamepad[gamepad].rumble_left  = normalize_range32_u16( rumble_left );
    global_input->gamepad[gamepad].rumble_right = normalize_range32_u16( rumble_right );

    platform->io.set_gamepad_rumble(
        gamepad,
        global_input->gamepad[gamepad].rumble_left,
        global_input->gamepad[gamepad].rumble_right );
}
LD_API void input_gamepad_query_rumble(
    usize gamepad, f32* out_rumble_left, f32* out_rumble_right
) {
    if( !global_input->gamepad[gamepad].is_active ) {
        return;
    }
    *out_rumble_left = normalize_range_u16_f32(
        global_input->gamepad[gamepad].rumble_left );
    *out_rumble_right = normalize_range_u16_f32(
        global_input->gamepad[gamepad].rumble_right );
}

LD_API b32 input_gamepad_is_active( usize gamepad ) {
    return global_input->gamepad[gamepad].is_active;
}

