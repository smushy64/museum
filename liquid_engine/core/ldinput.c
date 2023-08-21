/**
 * Description:  Input implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 03, 2023
*/
#include "core/ldinput.h"
#include "core/ldlog.h"
#include "core/ldmemory.h"
#include "ldplatform.h"

#define KEY_STATE_COUNT 255
typedef struct GamepadState {
    b8 last_buttons[GAMEPAD_CODE_COUNT];
    b8 buttons[GAMEPAD_CODE_COUNT];

    f32 last_trigger_left;
    f32 trigger_left;

    f32 last_trigger_right;
    f32 trigger_right;

    vec2 last_stick_left;
    vec2 stick_left;
    vec2 last_stick_right;
    vec2 stick_right;

    union {
        struct {
            f32 left_motor;
            f32 right_motor;
        };
        f32 motors[2];
    };

    union {
        struct {
            f32 stick_left_deadzone;
            f32 stick_right_deadzone;
        };
        f32 stick_deadzones[2];
    };
    union {
        struct {
            f32 trigger_left_deadzone;
            f32 trigger_right_deadzone;
        };
        f32 trigger_deadzones[2];
    };

    f32 trigger_press_threshold;

    b32 is_active;
} GamepadState;

internal GamepadState gamepad_state_default() {
    GamepadState result = {0};
    result.stick_left_deadzone     = GAMEPAD_DEFAULT_STICK_DEADZONE;
    result.stick_right_deadzone    = GAMEPAD_DEFAULT_STICK_DEADZONE;
    result.trigger_left_deadzone   = GAMEPAD_DEFAULT_TRIGGER_DEADZONE;
    result.trigger_right_deadzone  = GAMEPAD_DEFAULT_TRIGGER_DEADZONE;
    result.trigger_press_threshold = GAMEPAD_DEFAULT_TRIGGER_PRESS_THRESHOLD;

    return result;
}

typedef struct InputState {
    b8 last_keys[KEY_STATE_COUNT];
    b8 keys[KEY_STATE_COUNT];

    b8 last_mouse_buttons[MOUSE_BUTTON_COUNT];
    b8 mouse_buttons[MOUSE_BUTTON_COUNT];

    ivec2 last_mouse_position;
    ivec2 mouse_position;

    i32 last_mouse_wheel;
    i32 mouse_wheel;
    i32 last_horizontal_mouse_wheel;
    i32 horizontal_mouse_wheel;

    GamepadState gamepads[GAMEPAD_MAX_INDEX];
} InputState;

internal void input_state_init( InputState* state ) {
    for( u32 i = 0; i < GAMEPAD_MAX_INDEX; ++i ) {
        state->gamepads[i] = gamepad_state_default();
    }
}

global InputState* INPUT_STATE = NULL;

usize input_subsystem_query_size() {
    return sizeof(InputState);
}
b32 input_subsystem_init( void* buffer ) {
    INPUT_STATE = (InputState*)buffer;
    input_state_init( INPUT_STATE );
    LOG_INFO("Input subsystem successfully initialized.");
    return true;
}
void input_set_key( KeyboardCode keycode, b32 is_down ) {
    INPUT_STATE->keys[keycode] = is_down;
}
void input_set_mouse_button( MouseCode mousecode, b32 is_down ) {
    INPUT_STATE->mouse_buttons[mousecode] = is_down;
}
void input_set_mouse_position( ivec2 position ) {
    INPUT_STATE->mouse_position = position;
}
void input_set_mouse_wheel( i32 delta ) {
    INPUT_STATE->mouse_wheel = delta;
}
void input_set_horizontal_mouse_wheel( i32 delta ) {
    INPUT_STATE->horizontal_mouse_wheel = delta;
}
void input_set_gamepad_button(
    u32 gamepad_index, GamepadCode code, b32 is_down
) {
    INPUT_STATE->gamepads[gamepad_index].buttons[code] = is_down;
}
void input_set_gamepad_trigger_left( u32 gamepad_index, f32 value ) {
    INPUT_STATE->gamepads[gamepad_index].trigger_left = value;
}
void input_set_gamepad_trigger_right( u32 gamepad_index, f32 value ) {
    INPUT_STATE->gamepads[gamepad_index].trigger_right = value;
}
void input_set_gamepad_stick_left(
    u32 gamepad_index,
    vec2 value
) {
    INPUT_STATE->gamepads[gamepad_index].stick_left = value;
}
void input_set_gamepad_stick_right(
    u32 gamepad_index,
    vec2 value
) {
    INPUT_STATE->gamepads[gamepad_index].stick_right = value;
}
void input_set_gamepad_active(
    u32 gamepad_index,
    b32 is_active
) {
    INPUT_STATE->gamepads[gamepad_index].is_active = is_active;
}
void input_swap() {
    mem_copy(
        INPUT_STATE->last_keys,
        INPUT_STATE->keys,
        KEY_STATE_COUNT
    );

    mem_copy(
        INPUT_STATE->last_mouse_buttons,
        INPUT_STATE->mouse_buttons,
        MOUSE_BUTTON_COUNT
    );

    for( usize i = 0; i < GAMEPAD_MAX_INDEX; ++i ) {
        GamepadState* gamepad = (GamepadState*)&INPUT_STATE->gamepads[i];
        if( gamepad->is_active ) {
            mem_copy(
                gamepad->last_buttons,
                gamepad->buttons,
                GAMEPAD_CODE_COUNT
            );
            gamepad->last_stick_left    = gamepad->stick_left;
            gamepad->last_stick_right   = gamepad->stick_right;
            gamepad->last_trigger_left  = gamepad->trigger_left;
            gamepad->last_trigger_right = gamepad->trigger_right;
        }
    }

    INPUT_STATE->last_mouse_position = INPUT_STATE->mouse_position;

    INPUT_STATE->last_mouse_wheel = INPUT_STATE->mouse_wheel;
    INPUT_STATE->last_horizontal_mouse_wheel =
        INPUT_STATE->horizontal_mouse_wheel;
}

LD_API b32 input_is_key_down( KeyboardCode keycode ) {
    return INPUT_STATE->keys[keycode];
}
LD_API b32 input_was_key_down( KeyboardCode keycode ) {
    return INPUT_STATE->last_keys[keycode];
}
LD_API b32 input_is_mouse_button_down( MouseCode mousecode ) {
    return INPUT_STATE->mouse_buttons[mousecode];
}
LD_API b32 input_was_mouse_button_down( MouseCode mousecode ) {
    return INPUT_STATE->last_mouse_buttons[mousecode];
}
LD_API ivec2 input_mouse_position() {
    return INPUT_STATE->mouse_position;
}
LD_API ivec2 input_last_mouse_position() {
    return INPUT_STATE->last_mouse_position;
}
LD_API i32 input_mouse_wheel() {
    return INPUT_STATE->mouse_wheel;
}
LD_API i32 input_last_mouse_wheel() {
    return INPUT_STATE->last_mouse_wheel;
}
LD_API i32 input_horizontal_mouse_wheel() {
    return INPUT_STATE->horizontal_mouse_wheel;
}
LD_API i32 input_last_horizontal_mouse_wheel() {
    return INPUT_STATE->last_horizontal_mouse_wheel;
}
LD_API b32 input_is_gamepad_button_down( u32 index, GamepadCode code ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].buttons[code];
}
LD_API b32 input_was_gamepad_button_down( u32 index, GamepadCode code ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].last_buttons[code];
}
LD_API vec2 input_gamepad_stick_left( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].stick_left;
}
LD_API vec2 input_gamepad_last_stick_left( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].last_stick_left;
}
LD_API vec2 input_gamepad_stick_right( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].stick_right;
}
LD_API vec2 input_gamepad_last_stick_right( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].last_stick_right;
}
LD_API f32 input_gamepad_trigger_left( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].trigger_left;
}
LD_API f32 input_gamepad_last_trigger_left( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].last_trigger_left;
}
LD_API f32 input_gamepad_trigger_right( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].trigger_right;
}
LD_API f32 input_gamepad_last_trigger_right( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].last_trigger_right;
}
LD_API b32 input_gamepad_is_active( u32 index ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].is_active;
}
LD_API f32 input_gamepad_motor_state( u32 index, u32 motor ) {
    LOG_ASSERT(
        GAMEPAD_MAX_INDEX > index,
        "Gamepad index out of bounds!"
    );
    LOG_ASSERT(
        motor == 0 || motor == 1,
        "Motor index out of bounds!"
    );
    return INPUT_STATE->gamepads[index].motors[motor];
}
LD_API void input_gamepad_set_motor_state( u32 index, u32 motor, f32 value ) {
    platform_set_gamepad_motor_state( index, motor, value );
}
LD_API f32 input_gamepad_stick_left_deadzone( u32 index ) {
    return INPUT_STATE->gamepads[index].stick_left_deadzone;
}
LD_API f32 input_gamepad_stick_right_deadzone( u32 index ) {
    return INPUT_STATE->gamepads[index].stick_right_deadzone;
}
LD_API f32 input_gamepad_trigger_left_deadzone( u32 index ) {
    return INPUT_STATE->gamepads[index].trigger_left_deadzone;
}
LD_API f32 input_gamepad_trigger_right_deadzone( u32 index ) {
    return INPUT_STATE->gamepads[index].trigger_right_deadzone;
}
LD_API void input_gamepad_set_stick_left_deadzone( u32 index, f32 deadzone ) {
    INPUT_STATE->gamepads[index].stick_left_deadzone = deadzone;
}
LD_API void input_gamepad_set_stick_right_deadzone( u32 index, f32 deadzone ) {
    INPUT_STATE->gamepads[index].stick_right_deadzone = deadzone;
}
LD_API void input_gamepad_set_trigger_left_deadzone( u32 index, f32 deadzone ) {
    INPUT_STATE->gamepads[index].trigger_left_deadzone = deadzone;
}
LD_API void input_gamepad_set_trigger_right_deadzone( u32 index, f32 deadzone ) {
    INPUT_STATE->gamepads[index].trigger_right_deadzone = deadzone;
}
LD_API f32 input_gamepad_trigger_press_threshold( u32 index ) {
    return INPUT_STATE->gamepads[index].trigger_press_threshold;
}
LD_API void input_gamepad_set_trigger_press_threshold( u32 index, f32 threshold ) {
    INPUT_STATE->gamepads[index].trigger_press_threshold = threshold;
}
