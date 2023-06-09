#if !defined(INPUT_HPP)
#define INPUT_HPP
/**
 * Description:  Input Codes
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
 * Notes:        Mouse Position Coordinates
 *                 -X: Left  -Y: Bottom
 *                  X: Right  Y: Top
*/
#include "defines.h"
#include "math/types.h"

/// Key Codes
enum KeyCode : u8 {
    KEY_BACKSPACE = 8,
    KEY_TAB = 9,

    KEY_ENTER = 13,

    KEY_SHIFT_LEFT = 16,
    KEY_CONTROL_LEFT,
    KEY_ALT_LEFT,
    KEY_PAUSE,
    KEY_CAPSLOCK = 20,

    KEY_ESCAPE = 27,

    KEY_SPACE = 32,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_END,
    KEY_HOME,
    KEY_ARROW_LEFT,
    KEY_ARROW_UP,
    KEY_ARROW_RIGHT,
    KEY_ARROW_DOWN = 40,

    KEY_PRINT_SCREEN = 44,
    KEY_INSERT,
    KEY_DELETE = 46,

    KEY_0 = '0',
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',

    KEY_A = 'A',
    KEY_B = 'B',
    KEY_C = 'C',
    KEY_D = 'D',
    KEY_E = 'E',
    KEY_F = 'F',
    KEY_G = 'G',
    KEY_H = 'H',
    KEY_I = 'I',
    KEY_J = 'J',
    KEY_K = 'K',
    KEY_L = 'L',
    KEY_M = 'M',
    KEY_N = 'N',
    KEY_O = 'O',
    KEY_P = 'P',
    KEY_Q = 'Q',
    KEY_R = 'R',
    KEY_S = 'S',
    KEY_T = 'T',
    KEY_U = 'U',
    KEY_V = 'V',
    KEY_W = 'W',
    KEY_X = 'X',
    KEY_Y = 'Y',
    KEY_Z = 'Z',
    KEY_SUPER_LEFT,
    KEY_SUPER_RIGHT = 92,

    KEY_PAD_0 = 96,
    KEY_PAD_1,
    KEY_PAD_2,
    KEY_PAD_3,
    KEY_PAD_4,
    KEY_PAD_5,
    KEY_PAD_6,
    KEY_PAD_7,
    KEY_PAD_8,
    KEY_PAD_9 = 105,

    KEY_F1 = 112,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_F21,
    KEY_F22,
    KEY_F23,
    KEY_F24 = 135,

    KEY_NUM_LOCK = 144,
    KEY_SCROLL_LOCK = 145,

    KEY_SEMICOLON = 186,
    KEY_EQUALS,
    KEY_MINUS,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_SLASH_FORWARD,
    KEY_BACKTICK = 192,

    KEY_BRACKET_LEFT = 219,
    KEY_SLASH_BACKWARD,
    KEY_BRACKET_RIGHT,
    KEY_QUOTE,
    KEY_SHIFT_RIGHT,
    KEY_ALT_RIGHT,
    KEY_CONTROL_RIGHT = 225,

    KEY_UNKNOWN = U8::MAX,
};
#define KEY_COUNT 109
inline const char* to_string( KeyCode keycode ) {
    switch( keycode ) {
        case KEY_SPACE:          return "Space";
        case KEY_A:              return "A";
        case KEY_B:              return "B";
        case KEY_C:              return "C";
        case KEY_D:              return "D";
        case KEY_E:              return "E";
        case KEY_F:              return "F";
        case KEY_G:              return "G";
        case KEY_H:              return "H";
        case KEY_I:              return "I";
        case KEY_J:              return "J";
        case KEY_K:              return "K";
        case KEY_L:              return "L";
        case KEY_M:              return "M";
        case KEY_N:              return "N";
        case KEY_O:              return "O";
        case KEY_P:              return "P";
        case KEY_Q:              return "Q";
        case KEY_R:              return "R";
        case KEY_S:              return "S";
        case KEY_T:              return "T";
        case KEY_U:              return "U";
        case KEY_V:              return "V";
        case KEY_W:              return "W";
        case KEY_X:              return "X";
        case KEY_Y:              return "Y";
        case KEY_Z:              return "Z";
        case KEY_ESCAPE:         return "Escape";
        case KEY_F1:             return "F1";
        case KEY_F2:             return "F2";
        case KEY_F3:             return "F3";
        case KEY_F4:             return "F4";
        case KEY_F5:             return "F5";
        case KEY_F6:             return "F6";
        case KEY_F7:             return "F7";
        case KEY_F8:             return "F8";
        case KEY_F9:             return "F9";
        case KEY_F10:            return "F10";
        case KEY_F11:            return "F11";
        case KEY_F12:            return "F12";
        case KEY_F13:            return "F13";
        case KEY_F14:            return "F14";
        case KEY_F15:            return "F15";
        case KEY_F16:            return "F16";
        case KEY_F17:            return "F17";
        case KEY_F18:            return "F18";
        case KEY_F19:            return "F19";
        case KEY_F20:            return "F20";
        case KEY_F21:            return "F21";
        case KEY_F22:            return "F22";
        case KEY_F23:            return "F23";
        case KEY_F24:            return "F24";
        case KEY_PRINT_SCREEN:   return "Print Screen";
        case KEY_SCROLL_LOCK:    return "Scroll Lock";
        case KEY_PAUSE:          return "Pause";
        case KEY_BACKTICK:       return "`~";
        case KEY_0:              return "0";
        case KEY_1:              return "1";
        case KEY_2:              return "2";
        case KEY_3:              return "3";
        case KEY_4:              return "4";
        case KEY_5:              return "5";
        case KEY_6:              return "6";
        case KEY_7:              return "7";
        case KEY_8:              return "8";
        case KEY_9:              return "9";
        case KEY_MINUS:          return "-_";
        case KEY_EQUALS:         return "=+";
        case KEY_BACKSPACE:      return "Backspace";
        case KEY_INSERT:         return "Insert";
        case KEY_HOME:           return "Home";
        case KEY_PAGE_UP:        return "Page Up";
        case KEY_TAB:            return "Tab";
        case KEY_BRACKET_LEFT:   return "[{";
        case KEY_BRACKET_RIGHT:  return "]}";
        case KEY_SLASH_BACKWARD: return "\\|";
        case KEY_DELETE:         return "Delete";
        case KEY_END:            return "End";
        case KEY_PAGE_DOWN:      return "Page Down";
        case KEY_CAPSLOCK:       return "Capslock";
        case KEY_SEMICOLON:      return ";:";
        case KEY_QUOTE:          return "\'\"";
        case KEY_ENTER:          return "Enter";
        case KEY_SHIFT_LEFT:     return "Left Shift";
        case KEY_COMMA:          return ",<";
        case KEY_PERIOD:         return ".>";
        case KEY_SLASH_FORWARD:  return "/?";
        case KEY_SHIFT_RIGHT:    return "Right Shift";
        case KEY_CONTROL_LEFT:   return "Left Control";
        case KEY_SUPER_LEFT:     return "Left Super";
        case KEY_ALT_LEFT:       return "Left Alt";
        case KEY_ALT_RIGHT:      return "Right Alt";
        case KEY_SUPER_RIGHT:    return "Right Super";
        case KEY_CONTROL_RIGHT:  return "Right Control";
        case KEY_ARROW_LEFT:     return "Left Arrow";
        case KEY_ARROW_RIGHT:    return "Right Arrow";
        case KEY_ARROW_UP:       return "Up Arrow";
        case KEY_ARROW_DOWN:     return "Down Arrow";
        case KEY_NUM_LOCK:       return "Numlock";
        case KEY_PAD_0:          return "Keypad 0";
        case KEY_PAD_1:          return "Keypad 1";
        case KEY_PAD_2:          return "Keypad 2";
        case KEY_PAD_3:          return "Keypad 3";
        case KEY_PAD_4:          return "Keypad 4";
        case KEY_PAD_5:          return "Keypad 5";
        case KEY_PAD_6:          return "Keypad 6";
        case KEY_PAD_7:          return "Keypad 7";
        case KEY_PAD_8:          return "Keypad 8";
        case KEY_PAD_9:          return "Keypad 9";
        default:                 return "Unknown";
    }
}

/// Mouse Button Codes
enum MouseCode : u8 {
    MBC_UNKNOWN = U8::MAX,
    MBC_BUTTON_LEFT = 0,
    MBC_BUTTON_MIDDLE,
    MBC_BUTTON_RIGHT,
    MBC_BUTTON_EXTRA_1,
    MBC_BUTTON_EXTRA_2,

    MBC_COUNT
};
inline const char* to_string( MouseCode mousecode ) {
    local const char* strings[MBC_COUNT] = {
        "Mouse Button Left",
        "Mouse Button Middle",
        "Mouse Button Right",
        "Mouse Button Extra 1",
        "Mouse Button Extra 2",
    };
    if( mousecode >= MBC_COUNT ) {
        return "Unknown";
    }
    return strings[mousecode];
}

/// Pad Codes
enum PadCode : u8 {
    PAD_CODE_UNKNOWN = 0,

    PAD_CODE_STICK_LEFT,
    PAD_CODE_STICK_RIGHT,

    PAD_CODE_STICK_LEFT_CLICK,
    PAD_CODE_STICK_RIGHT_CLICK,

    PAD_CODE_TRIGGER_LEFT,
    PAD_CODE_TRIGGER_RIGHT,

    PAD_CODE_BUMPER_LEFT,
    PAD_CODE_BUMPER_RIGHT,

    PAD_CODE_DPAD_LEFT,
    PAD_CODE_DPAD_RIGHT,
    PAD_CODE_DPAD_UP,
    PAD_CODE_DPAD_DOWN,

    PAD_CODE_FACE_LEFT,
    PAD_CODE_FACE_RIGHT,
    PAD_CODE_FACE_UP,
    PAD_CODE_FACE_DOWN,

    PAD_CODE_START,
    PAD_CODE_SELECT,

    PAD_CODE_COUNT
};
inline const char* to_string( PadCode padcode ) {
    local const char* strings[PAD_CODE_COUNT] = {
        "Unknown",
        "STICK Left",
        "STICK Right",
        "STICK Left Click",
        "STICK Right Click",
        "TRIGGER Left",
        "TRIGGER Right",
        "BUMPER Left",
        "BUMPER Right",
        "DPAD Left",
        "DPAD Right",
        "DPAD Up",
        "DPAD Down",
        "Face Left",
        "Face Right",
        "Face Up",
        "Face Down",
        "START",
        "SELECT",
    };
    if( padcode >= PAD_CODE_COUNT ) {
        return strings[0];
    }
    return strings[padcode];
}

#define GAMEPAD_MOTOR_LEFT  0
#define GAMEPAD_MOTOR_RIGHT 1

#define MAX_GAMEPAD_INDEX 4

#if defined(SM_API_INTERNAL)

    b32  input_init( struct Platform* platform );
    void input_shutdown();

    void input_set_key(
        KeyCode keycode,
        b8 is_down
    );
    void input_set_mouse_button(
        MouseCode mousecode,
        b8 is_down
    );
    void input_set_mouse_position( ivec2 position );
    void input_set_mouse_wheel( i32 delta );
    void input_set_horizontal_mouse_wheel( i32 delta );

    void input_set_pad_button(
        u32 gamepad_index,
        PadCode code,
        b32 is_down
    );
    void input_set_pad_trigger_left(
        u32 gamepad_index,
        f32 value
    );
    void input_set_pad_trigger_right(
        u32 gamepad_index,
        f32 value
    );
    void input_set_pad_stick_left(
        u32 gamepad_index,
        vec2 value
    );
    void input_set_pad_stick_right(
        u32 gamepad_index,
        vec2 value
    );
    void input_set_pad_active(
        u32 gamepad_index,
        b32 is_active
    );

    void input_swap();

#endif // internal

SM_API b32 input_is_key_down( KeyCode code );
SM_API b32 input_was_key_down( KeyCode code );

SM_API b32 input_is_mousebutton_down( MouseCode code );
SM_API b32 input_was_mousebutton_down( MouseCode code );

SM_API ivec2 input_mouse_position();
SM_API ivec2 input_last_mouse_position();

SM_API i32 input_mouse_wheel();
SM_API i32 input_last_mouse_wheel();

SM_API i32 input_horizontal_mouse_wheel();
SM_API i32 input_last_horizontal_mouse_wheel();

SM_API b32 input_is_pad_button_down(
    u32 gamepad_index,
    PadCode code
);
SM_API b32 input_was_pad_button_down(
    u32 gamepad_index,
    PadCode code
);

SM_API vec2 input_pad_stick_left( u32 gamepad_index );
SM_API vec2 input_pad_last_stick_left( u32 gamepad_index );

SM_API vec2 input_pad_stick_right( u32 gamepad_index );
SM_API vec2 input_pad_last_stick_right( u32 gamepad_index );

SM_API f32 input_pad_trigger_left( u32 gamepad_index );
SM_API f32 input_pad_last_trigger_left( u32 gamepad_index );
SM_API f32 input_pad_trigger_right( u32 gamepad_index );
SM_API f32 input_pad_last_trigger_right( u32 gamepad_index );

SM_API void input_set_pad_motor_state(
    u32 gamepad_index,
    u32 motor,
    f32 value
);
SM_API b32 input_is_pad_active( u32 gamepad_index );
SM_API f32 input_query_motor_state(
    u32 gamepad_index,
    u32 motor
);

inline vec2 mouse_position_to_ndc(
    ivec2 position,
    ivec2 surface_dimensions
) {
    vec2 result = {
        (f32)position.x / (f32)surface_dimensions.x,
        (f32)position.y / (f32)surface_dimensions.y,
    };

    return (result - v2(0.5f)) * 2.0f;
}

#endif // header guard