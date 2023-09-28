/**
 * Description:  Input implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 03, 2023
*/
#include "core/input.h"
#include "core/mathf.h"
#include "core/memoryf.h"
#include "core/internal.h"
#include "core/log.h"

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
    u8 buttons[INPUT_KEY_COUNT];
    u8 last_buttons[INPUT_KEY_COUNT];
};

struct MouseState {
    MouseCode buttons;
    MouseCode last_buttons;
    f32 x, y;
    f32 last_x, last_y;
    i8  wheel_x, wheel_y;
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
    global_input = buffer;
}
void input_subsystem_swap_state(void) {
    mem_copy(
        global_input->keyboard.last_buttons,
        global_input->keyboard.buttons,
        sizeof( global_input->keyboard.buttons ) );
    global_input->mouse.last_buttons = global_input->mouse.buttons;
    global_input->mouse.last_x       = global_input->mouse.x;
    global_input->mouse.last_y       = global_input->mouse.y;
}
void input_subsystem_update_gamepads(void) {
    PlatformGamepad platform_gamepads[INPUT_GAMEPAD_COUNT];
    platform->io.read_gamepads( platform_gamepads );

    for( usize i = 0; i < INPUT_GAMEPAD_COUNT; ++i ) {
        PlatformGamepad* platform_current = platform_gamepads + i;
        struct GamepadState* current = global_input->gamepad + i;

        if( !platform_current->is_active ) {
            if( current->is_active ) {
                mem_zero( current, sizeof(struct GamepadState) );
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
    global_input->keyboard.buttons[code] = is_down;
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
void input_subsystem_set_mouse_position( f32 x, f32 y ) {
    global_input->mouse.x = x;
    global_input->mouse.y = y;
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

LD_API f32 input_mouse_x(void) {
    return global_input->mouse.x;
}
LD_API f32 input_mouse_y(void) {
    return global_input->mouse.y;
}
LD_API f32 input_mouse_delta_x(void) {
    f32 last_x = global_input->mouse.x;
    f32 x      = global_input->mouse.x;

    return last_x - x;
}
LD_API f32 input_mouse_delta_y(void) {
    f32 last_y = global_input->mouse.y;
    f32 y      = global_input->mouse.y;

    return last_y - y;
}

LD_API i32 input_mouse_wheel(void) {
    return global_input->mouse.wheel_y;
}
LD_API i32 input_mouse_wheel_horizontal(void) {
    return global_input->mouse.wheel_x;
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
        LOG_WARN( "Attempted to rumble disconnected gamepad {usize}!", gamepad );
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

// #define KEY_STATE_COUNT 255
// typedef struct GamepadState {
//     b8 last_buttons[GAMEPAD_CODE_COUNT];
//     b8 buttons[GAMEPAD_CODE_COUNT];
//
//     f32 last_trigger_left;
//     f32 trigger_left;
//
//     f32 last_trigger_right;
//     f32 trigger_right;
//
//     vec2 last_stick_left;
//     vec2 stick_left;
//     vec2 last_stick_right;
//     vec2 stick_right;
//
//     union {
//         struct {
//             f32 left_motor;
//             f32 right_motor;
//         };
//         f32 motors[2];
//     };
//
//     union {
//         struct {
//             f32 stick_left_deadzone;
//             f32 stick_right_deadzone;
//         };
//         f32 stick_deadzones[2];
//     };
//     union {
//         struct {
//             f32 trigger_left_deadzone;
//             f32 trigger_right_deadzone;
//         };
//         f32 trigger_deadzones[2];
//     };
//
//     f32 trigger_press_threshold;
//
//     b32 is_active;
// } GamepadState;
//
// internal GamepadState gamepad_state_default(void) {
//     GamepadState result = {0};
//     result.stick_left_deadzone     = GAMEPAD_DEFAULT_STICK_DEADZONE;
//     result.stick_right_deadzone    = GAMEPAD_DEFAULT_STICK_DEADZONE;
//     result.trigger_left_deadzone   = GAMEPAD_DEFAULT_TRIGGER_DEADZONE;
//     result.trigger_right_deadzone  = GAMEPAD_DEFAULT_TRIGGER_DEADZONE;
//     result.trigger_press_threshold = GAMEPAD_DEFAULT_TRIGGER_PRESS_THRESHOLD;
//
//     return result;
// }
//
// typedef struct InputState {
//     b8 last_keys[KEY_STATE_COUNT];
//     b8 keys[KEY_STATE_COUNT];
//
//     b8 last_mouse_buttons[MOUSE_BUTTON_COUNT];
//     b8 mouse_buttons[MOUSE_BUTTON_COUNT];
//
//     ivec2 last_mouse_position;
//     ivec2 mouse_position;
//
//     i32 last_mouse_wheel;
//     i32 mouse_wheel;
//     i32 last_horizontal_mouse_wheel;
//     i32 horizontal_mouse_wheel;
//
//     GamepadState gamepads[GAMEPAD_MAX_INDEX];
// } InputState;
//
// global InputState* INPUT_STATE = NULL;
//
// usize INPUT_SUBSYSTEM_SIZE = sizeof(InputState);
// void input_subsystem_init( void* buffer ) {
//     INPUT_STATE = (InputState*)buffer;
//     for( u32 i = 0; i < GAMEPAD_MAX_INDEX; ++i ) {
//         INPUT_STATE->gamepads[i] = gamepad_state_default();
//     }
//     LOG_INFO("Input subsystem successfully initialized.");
// }
// void input_set_key( KeyboardCode keycode, b32 is_down ) {
//     INPUT_STATE->keys[keycode] = is_down;
// }
// void input_set_mouse_button( MouseCode mousecode, b32 is_down ) {
//     INPUT_STATE->mouse_buttons[mousecode] = is_down;
// }
// void input_set_mouse_position( ivec2 position ) {
//     INPUT_STATE->mouse_position = position;
// }
// void input_set_mouse_wheel( i32 delta ) {
//     INPUT_STATE->mouse_wheel = delta;
// }
// void input_set_horizontal_mouse_wheel( i32 delta ) {
//     INPUT_STATE->horizontal_mouse_wheel = delta;
// }
// void input_set_gamepad_button(
//     u32 gamepad_index, GamepadCode code, b32 is_down
// ) {
//     INPUT_STATE->gamepads[gamepad_index].buttons[code] = is_down;
// }
// void input_set_gamepad_trigger_left( u32 gamepad_index, f32 value ) {
//     INPUT_STATE->gamepads[gamepad_index].trigger_left = value;
// }
// void input_set_gamepad_trigger_right( u32 gamepad_index, f32 value ) {
//     INPUT_STATE->gamepads[gamepad_index].trigger_right = value;
// }
// void input_set_gamepad_stick_left(
//     u32 gamepad_index,
//     vec2 value
// ) {
//     INPUT_STATE->gamepads[gamepad_index].stick_left = value;
// }
// void input_set_gamepad_stick_right(
//     u32 gamepad_index,
//     vec2 value
// ) {
//     INPUT_STATE->gamepads[gamepad_index].stick_right = value;
// }
// void input_set_gamepad_active(
//     u32 gamepad_index,
//     b32 is_active
// ) {
//     INPUT_STATE->gamepads[gamepad_index].is_active = is_active;
// }
// void input_swap(void) {
//     mem_copy(
//         INPUT_STATE->last_keys,
//         INPUT_STATE->keys,
//         KEY_STATE_COUNT
//     );
//
//     mem_copy(
//         INPUT_STATE->last_mouse_buttons,
//         INPUT_STATE->mouse_buttons,
//         MOUSE_BUTTON_COUNT
//     );
//
//     for( usize i = 0; i < GAMEPAD_MAX_INDEX; ++i ) {
//         GamepadState* gamepad = (GamepadState*)&INPUT_STATE->gamepads[i];
//         if( gamepad->is_active ) {
//             mem_copy(
//                 gamepad->last_buttons,
//                 gamepad->buttons,
//                 GAMEPAD_CODE_COUNT
//             );
//             gamepad->last_stick_left    = gamepad->stick_left;
//             gamepad->last_stick_right   = gamepad->stick_right;
//             gamepad->last_trigger_left  = gamepad->trigger_left;
//             gamepad->last_trigger_right = gamepad->trigger_right;
//         }
//     }
//
//     INPUT_STATE->last_mouse_position = INPUT_STATE->mouse_position;
//
//     INPUT_STATE->last_mouse_wheel = INPUT_STATE->mouse_wheel;
//     INPUT_STATE->last_horizontal_mouse_wheel =
//         INPUT_STATE->horizontal_mouse_wheel;
// }
//
// LD_API b32 input_is_key_down( KeyboardCode keycode ) {
//     return INPUT_STATE->keys[keycode];
// }
// LD_API b32 input_was_key_down( KeyboardCode keycode ) {
//     return INPUT_STATE->last_keys[keycode];
// }
// LD_API b32 input_is_mouse_button_down( MouseCode mousecode ) {
//     return INPUT_STATE->mouse_buttons[mousecode];
// }
// LD_API b32 input_was_mouse_button_down( MouseCode mousecode ) {
//     return INPUT_STATE->last_mouse_buttons[mousecode];
// }
// LD_API ivec2 input_mouse_position(void) {
//     return INPUT_STATE->mouse_position;
// }
// LD_API ivec2 input_last_mouse_position(void) {
//     return INPUT_STATE->last_mouse_position;
// }
// LD_API i32 input_mouse_wheel(void) {
//     return INPUT_STATE->mouse_wheel;
// }
// LD_API i32 input_last_mouse_wheel(void) {
//     return INPUT_STATE->last_mouse_wheel;
// }
// LD_API i32 input_horizontal_mouse_wheel(void) {
//     return INPUT_STATE->horizontal_mouse_wheel;
// }
// LD_API i32 input_last_horizontal_mouse_wheel(void) {
//     return INPUT_STATE->last_horizontal_mouse_wheel;
// }
// LD_API b32 input_is_gamepad_button_down( u32 index, GamepadCode code ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].buttons[code];
// }
// LD_API b32 input_was_gamepad_button_down( u32 index, GamepadCode code ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].last_buttons[code];
// }
// LD_API vec2 input_gamepad_stick_left( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].stick_left;
// }
// LD_API vec2 input_gamepad_last_stick_left( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].last_stick_left;
// }
// LD_API vec2 input_gamepad_stick_right( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].stick_right;
// }
// LD_API vec2 input_gamepad_last_stick_right( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].last_stick_right;
// }
// LD_API f32 input_gamepad_trigger_left( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].trigger_left;
// }
// LD_API f32 input_gamepad_last_trigger_left( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].last_trigger_left;
// }
// LD_API f32 input_gamepad_trigger_right( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].trigger_right;
// }
// LD_API f32 input_gamepad_last_trigger_right( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].last_trigger_right;
// }
// LD_API b32 input_gamepad_is_active( u32 index ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].is_active;
// }
// LD_API f32 input_gamepad_motor_state( u32 index, u32 motor ) {
//     LOG_ASSERT(
//         GAMEPAD_MAX_INDEX > index,
//         "Gamepad index out of bounds!"
//     );
//     LOG_ASSERT(
//         motor == 0 || motor == 1,
//         "Motor index out of bounds!"
//     );
//     return INPUT_STATE->gamepads[index].motors[motor];
// }
// LD_API void input_gamepad_set_motor_state( u32 index, f32 left, f32 right ) {
//     INPUT_STATE->gamepads[index].motors[0] = left;
//     INPUT_STATE->gamepads[index].motors[1] = right;
//     platform->io.set_gamepad_rumble( index, left, right );
// }
// LD_API f32 input_gamepad_stick_left_deadzone( u32 index ) {
//     return INPUT_STATE->gamepads[index].stick_left_deadzone;
// }
// LD_API f32 input_gamepad_stick_right_deadzone( u32 index ) {
//     return INPUT_STATE->gamepads[index].stick_right_deadzone;
// }
// LD_API f32 input_gamepad_trigger_left_deadzone( u32 index ) {
//     return INPUT_STATE->gamepads[index].trigger_left_deadzone;
// }
// LD_API f32 input_gamepad_trigger_right_deadzone( u32 index ) {
//     return INPUT_STATE->gamepads[index].trigger_right_deadzone;
// }
// LD_API void input_gamepad_set_stick_left_deadzone( u32 index, f32 deadzone ) {
//     INPUT_STATE->gamepads[index].stick_left_deadzone = deadzone;
// }
// LD_API void input_gamepad_set_stick_right_deadzone( u32 index, f32 deadzone ) {
//     INPUT_STATE->gamepads[index].stick_right_deadzone = deadzone;
// }
// LD_API void input_gamepad_set_trigger_left_deadzone( u32 index, f32 deadzone ) {
//     INPUT_STATE->gamepads[index].trigger_left_deadzone = deadzone;
// }
// LD_API void input_gamepad_set_trigger_right_deadzone( u32 index, f32 deadzone ) {
//     INPUT_STATE->gamepads[index].trigger_right_deadzone = deadzone;
// }
// LD_API f32 input_gamepad_trigger_press_threshold( u32 index ) {
//     return INPUT_STATE->gamepads[index].trigger_press_threshold;
// }
// LD_API void input_gamepad_set_trigger_press_threshold( u32 index, f32 threshold ) {
//     INPUT_STATE->gamepads[index].trigger_press_threshold = threshold;
// }
