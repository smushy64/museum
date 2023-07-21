/**
 * Description:  Input implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 03, 2023
*/
#include "input.h"
#include "logging.h"
#include "memory.h"
#include "platform/platform.h"

#define KEY_STATE_COUNT 255
#define DEFAULT_STICK_DEADZONE   0.05f
#define DEFAULT_TRIGGER_DEADZONE 0.05f
#define DEFAULT_TRIGGER_PRESS_THRESHOLD 0.5f

struct GamepadState {
    b8 last_buttons[PAD_CODE_COUNT];
    b8 buttons[PAD_CODE_COUNT];

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
            f32 stick_left_deadzone  = DEFAULT_STICK_DEADZONE;
            f32 stick_right_deadzone = DEFAULT_STICK_DEADZONE;
        };
        f32 stick_deadzones[2];
    };
    union {
        struct {
            f32 trigger_left_deadzone  = DEFAULT_TRIGGER_DEADZONE;
            f32 trigger_right_deadzone = DEFAULT_TRIGGER_DEADZONE;
        };
        f32 trigger_deadzones[2];
    };

    f32 trigger_press_threshold = DEFAULT_TRIGGER_PRESS_THRESHOLD;

    b32 is_active;
};

struct InputState {
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

    GamepadState gamepads[MAX_GAMEPAD_INDEX];

    Platform* platform;
};

global InputState* INPUT_STATE = nullptr;

u32 query_input_subsystem_size() {
    return sizeof(InputState);
}
b32 input_init( Platform* platform, void* buffer ) {
    INPUT_STATE = (InputState*)buffer;
    INPUT_STATE->platform = platform;
    LOG_INFO("Input subsystem successfully initialized.");
    return true;
}
void input_shutdown() {
    LOG_INFO("Input subsystem shutdown.");
}

void input_set_key(
    KeyboardCode keycode,
    b8 is_down
) {
    INPUT_STATE->keys[keycode] = is_down;
}
void input_set_mouse_button(
    MouseCode mousecode,
    b8 is_down
) {
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
void input_set_pad_button(
    u32 gamepad_index,
    PadCode code,
    b32 is_down
) {
    INPUT_STATE->gamepads[gamepad_index].buttons[code] = is_down;
}
void input_set_pad_trigger_left(
    u32 gamepad_index,
    f32 value
) {
    INPUT_STATE->gamepads[gamepad_index].trigger_left = value;
}
void input_set_pad_trigger_right(
    u32 gamepad_index,
    f32 value
) {
    INPUT_STATE->gamepads[gamepad_index].trigger_right = value;
}
void input_set_pad_stick_left(
    u32 gamepad_index,
    vec2 value
) {
    INPUT_STATE->gamepads[gamepad_index].stick_left = value;
}
void input_set_pad_stick_right(
    u32 gamepad_index,
    vec2 value
) {
    INPUT_STATE->gamepads[gamepad_index].stick_right = value;
}
void input_set_pad_active(
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

    for( usize i = 0; i < MAX_GAMEPAD_INDEX; ++i ) {
        GamepadState* gamepad = (GamepadState*)&INPUT_STATE->gamepads[i];
        if( gamepad->is_active ) {
            mem_copy(
                gamepad->last_buttons,
                gamepad->buttons,
                PAD_CODE_COUNT
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

b32 input_is_key_down( KeyboardCode keycode ) {
    return INPUT_STATE->keys[keycode];
}
b32 input_was_key_down( KeyboardCode keycode ) {
    return INPUT_STATE->last_keys[keycode];
}

b32 input_is_mousebutton_down( MouseCode mousecode ) {
    return INPUT_STATE->mouse_buttons[mousecode];
}
b32 input_was_mousebutton_down( MouseCode mousecode ) {
    return INPUT_STATE->last_mouse_buttons[mousecode];
}

ivec2 input_mouse_position() {
    return INPUT_STATE->mouse_position;
}
ivec2 input_last_mouse_position() {
    return INPUT_STATE->last_mouse_position;
}

i32 input_mouse_wheel() {
    return INPUT_STATE->mouse_wheel;
}
i32 input_last_mouse_wheel() {
    return INPUT_STATE->last_mouse_wheel;
}

i32 input_horizontal_mouse_wheel() {
    return INPUT_STATE->horizontal_mouse_wheel;
}
i32 input_last_horizontal_mouse_wheel() {
    return INPUT_STATE->last_horizontal_mouse_wheel;
}

b32 input_is_pad_button_down(
    u32 gamepad_index,
    PadCode code
) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].buttons[code];
}
b32 input_was_pad_button_down(
    u32 gamepad_index,
    PadCode code
) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].last_buttons[code];
}

vec2 input_pad_stick_left( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].stick_left;
}
vec2 input_pad_last_stick_left( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].last_stick_left;
}

vec2 input_pad_stick_right( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].stick_right;
}
vec2 input_pad_last_stick_right( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].last_stick_right;
}

f32 input_pad_trigger_left( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].trigger_left;
}
f32 input_pad_last_trigger_left( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].last_trigger_left;
}
f32 input_pad_trigger_right( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].trigger_right;
}
f32 input_pad_last_trigger_right( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].last_trigger_right;
}
b32 input_pad_is_active( u32 gamepad_index ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].is_active;
}

f32 input_pad_read_motor_state( u32 gamepad_index, u32 motor ) {
    LOG_ASSERT(
        MAX_GAMEPAD_INDEX > gamepad_index,
        "Gamepad index out of bounds!"
    );
    LOG_ASSERT(
        motor == 0 || motor == 1,
        "Motor index out of bounds!"
    );
    return INPUT_STATE->gamepads[gamepad_index].motors[motor];
}

void input_pad_write_motor_state(
    u32 gamepad_index,
    u32 motor,
    f32 value
) {
    platform_set_pad_motor_state(
        INPUT_STATE->platform,
        gamepad_index,
        motor,
        value
    );
}

f32 input_pad_read_stick_left_deadzone( u32 gamepad_index ) {
    return INPUT_STATE->gamepads[gamepad_index].stick_left_deadzone;
}
f32 input_pad_read_stick_right_deadzone( u32 gamepad_index ) {
    return INPUT_STATE->gamepads[gamepad_index].stick_right_deadzone;
}
f32 input_pad_read_trigger_left_deadzone( u32 gamepad_index ) {
    return INPUT_STATE->gamepads[gamepad_index].trigger_left_deadzone;
}
f32 input_pad_read_trigger_right_deadzone( u32 gamepad_index ) {
    return INPUT_STATE->gamepads[gamepad_index].trigger_right_deadzone;
}

void input_pad_write_stick_left_deadzone( u32 gamepad_index, f32 deadzone ) {
    INPUT_STATE->gamepads[gamepad_index].stick_left_deadzone = deadzone;
}
void input_pad_write_stick_right_deadzone( u32 gamepad_index, f32 deadzone ) {
    INPUT_STATE->gamepads[gamepad_index].stick_right_deadzone = deadzone;
}
void input_pad_write_trigger_left_deadzone( u32 gamepad_index, f32 deadzone ) {
    INPUT_STATE->gamepads[gamepad_index].trigger_left_deadzone = deadzone;
}
void input_pad_write_trigger_right_deadzone( u32 gamepad_index, f32 deadzone ) {
    INPUT_STATE->gamepads[gamepad_index].trigger_right_deadzone = deadzone;
}

f32 input_pad_read_trigger_press_threshold( u32 gamepad_index ) {
    return INPUT_STATE->gamepads[gamepad_index].trigger_press_threshold;
}

void input_pad_write_trigger_press_threshold( u32 gamepad_index, f32 threshold ) {
    INPUT_STATE->gamepads[gamepad_index].trigger_press_threshold = threshold;
}
