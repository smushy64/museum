/**
 * Description:  Input implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 03, 2023
*/
#include "input.h"
#include "logging.h"
#include "memory.h"

#define KEY_STATE_COUNT 255

struct InputState {
    b8 last_keys[KEY_STATE_COUNT];
    b8 keys[KEY_STATE_COUNT];

    b8 last_mouse_buttons[MBC_COUNT];
    b8 mouse_buttons[MBC_COUNT];

    ivec2 last_mouse_position;
    ivec2 mouse_position;

    i32 last_mouse_wheel;
    i32 mouse_wheel;
    i32 last_horizontal_mouse_wheel;
    i32 horizontal_mouse_wheel;
};

SM_GLOBAL InputState INPUT_STATE = {};

b32 input_init() {
    LOG_NOTE("Input subsystem successfully initialized.");
    return true;
}
b32 input_shutdown() {
    LOG_NOTE("Input subsystem successfully shutdown.");
    return true;
}

void input_set_key(
    KeyCode keycode,
    b8 is_down
) {
    INPUT_STATE.keys[keycode] = is_down;
}
void input_set_mouse_button(
    MouseCode mousecode,
    b8 is_down
) {
    INPUT_STATE.mouse_buttons[mousecode] = is_down;
}
void input_set_mouse_position( ivec2 position ) {
    INPUT_STATE.mouse_position = position;
}
void input_set_mouse_wheel( i32 delta ) {
    INPUT_STATE.mouse_wheel = delta;
}
void input_set_horizontal_mouse_wheel( i32 delta ) {
    INPUT_STATE.horizontal_mouse_wheel = delta;
}

void input_swap() {
    mem_copy(
        INPUT_STATE.last_keys,
        INPUT_STATE.keys,
        KEY_STATE_COUNT
    );

    mem_copy(
        INPUT_STATE.last_mouse_buttons,
        INPUT_STATE.mouse_buttons,
        MBC_COUNT
    );

    INPUT_STATE.last_mouse_position = INPUT_STATE.mouse_position;

    INPUT_STATE.last_mouse_wheel = INPUT_STATE.mouse_wheel;
    INPUT_STATE.last_horizontal_mouse_wheel =
        INPUT_STATE.horizontal_mouse_wheel;
}

b32 input_is_key_down( KeyCode keycode ) {
    return INPUT_STATE.keys[keycode];
}
b32 input_was_key_down( KeyCode keycode ) {
    return INPUT_STATE.last_keys[keycode];
}

b32 input_is_mousebutton_down( MouseCode mousecode ) {
    return INPUT_STATE.mouse_buttons[mousecode];
}
b32 input_was_mousebutton_down( MouseCode mousecode ) {
    return INPUT_STATE.last_mouse_buttons[mousecode];
}

ivec2 input_mouse_position() {
    return INPUT_STATE.mouse_position;
}
ivec2 input_last_mouse_position() {
    return INPUT_STATE.last_mouse_position;
}

i32 input_mouse_wheel() {
    return INPUT_STATE.mouse_wheel;
}
i32 input_last_mouse_wheel() {
    return INPUT_STATE.last_mouse_wheel;
}

i32 input_horizontal_mouse_wheel() {
    return INPUT_STATE.horizontal_mouse_wheel;
}
i32 input_last_horizontal_mouse_wheel() {
    return INPUT_STATE.last_horizontal_mouse_wheel;
}
