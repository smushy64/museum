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
#include "core/ldmath/types.h"
#include "core/ldmath/type_functions.h"

#define GAMEPAD_MOTOR_LEFT  (0)
#define GAMEPAD_MOTOR_RIGHT (1)

#define GAMEPAD_MAX_INDEX (4)
#define GAMEPAD_DEFAULT_STICK_DEADZONE          (0.05f)
#define GAMEPAD_DEFAULT_TRIGGER_DEADZONE        (0.05f)
#define GAMEPAD_DEFAULT_TRIGGER_PRESS_THRESHOLD (0.50f)

#define KEY_COUNT (109)

/// Key Codes
typedef enum KeyboardCode : u8 {
    KEY_BACKSPACE  = 8,
    KEY_TAB        = 9,

    KEY_ENTER      = 13,

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

    KEY_UNKNOWN = U8_MAX,
} KeyboardCode;

maybe_unused
header_only const char* keyboard_code_to_string( KeyboardCode keycode ) {
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
typedef enum MouseCode : u8 {
    MOUSE_BUTTON_UNKNOWN = U8_MAX,
    MOUSE_BUTTON_LEFT = 0,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_EXTRA_1,
    MOUSE_BUTTON_EXTRA_2,

    MOUSE_BUTTON_COUNT
} MouseCode;
maybe_unused
header_only const char* mouse_code_to_string( MouseCode mouse_code ) {
    const char* strings[MOUSE_BUTTON_COUNT] = {
        "Mouse Button Left",
        "Mouse Button Middle",
        "Mouse Button Right",
        "Mouse Button Extra 1",
        "Mouse Button Extra 2",
    };
    if( mouse_code >= MOUSE_BUTTON_COUNT ) {
        return "Unknown";
    }
    return strings[mouse_code];
}

/// Gamepad Codes
typedef enum GamepadCode : u8 {
    GAMEPAD_CODE_UNKNOWN = 0,

    GAMEPAD_CODE_STICK_LEFT,
    GAMEPAD_CODE_STICK_RIGHT,

    GAMEPAD_CODE_STICK_LEFT_CLICK,
    GAMEPAD_CODE_STICK_RIGHT_CLICK,

    GAMEPAD_CODE_TRIGGER_LEFT,
    GAMEPAD_CODE_TRIGGER_RIGHT,

    GAMEPAD_CODE_BUMPER_LEFT,
    GAMEPAD_CODE_BUMPER_RIGHT,

    GAMEPAD_CODE_DPAD_LEFT,
    GAMEPAD_CODE_DPAD_RIGHT,
    GAMEPAD_CODE_DPAD_UP,
    GAMEPAD_CODE_DPAD_DOWN,

    GAMEPAD_CODE_FACE_LEFT,
    GAMEPAD_CODE_FACE_RIGHT,
    GAMEPAD_CODE_FACE_UP,
    GAMEPAD_CODE_FACE_DOWN,

    GAMEPAD_CODE_START,
    GAMEPAD_CODE_SELECT,

    GAMEPAD_CODE_COUNT
} GamepadCode;

maybe_unused
header_only const char* gamepad_code_to_string( GamepadCode gamepad_code ) {
    const char* strings[GAMEPAD_CODE_COUNT] = {
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
    if( gamepad_code >= GAMEPAD_CODE_COUNT ) {
        return strings[0];
    }
    return strings[gamepad_code];
}

#if defined(LD_API_INTERNAL)

    /// Input subsystem size.
    extern usize INPUT_SUBSYSTEM_SIZE;
    /// Initialize input subsystem.
    b32  input_subsystem_init( void* buffer );

    /// Set a key's state.
    void input_set_key( KeyboardCode keycode, b32 is_down );
    /// Set a mouse button's state.
    void input_set_mouse_button( MouseCode mousecode, b32 is_down );
    /// Set mouse's position.
    void input_set_mouse_position( ivec2 position );
    /// Set mouse wheel's state.
    void input_set_mouse_wheel( i32 delta );
    /// Set horizontal mouse wheel's state.
    void input_set_horizontal_mouse_wheel( i32 delta );

    /// Set gamepad button state.
    void input_set_gamepad_button(
        u32 gamepad_index, GamepadCode code, b32 is_down );
    /// Set gamepad trigger left state.
    void input_set_gamepad_trigger_left( u32 gamepad_index, f32 value );
    /// Set gamepad trigger right state.
    void input_set_gamepad_trigger_right( u32 gamepad_index, f32 value );
    /// Set gamepad stick left state.
    void input_set_gamepad_stick_left( u32 gamepad_index, vec2 value );
    /// Set gamepad stick right state.
    void input_set_gamepad_stick_right( u32 gamepad_index, vec2 value );
    /// Set gamepad active state.
    void input_set_gamepad_active( u32 gamepad_index, b32 is_active );

    /// Swap input states.
    void input_swap(void);

#endif // internal

/// Is key down this frame?
LD_API b32 input_is_key_down( KeyboardCode code );
/// Was key down last frame?
LD_API b32 input_was_key_down( KeyboardCode code );
/// Has key been pressed this frame?
maybe_unused
header_only b32 input_key_press( KeyboardCode code ) {
    b32 is  = input_is_key_down( code );
    b32 was = input_was_key_down( code );
    return (is != was) && is;
}
/// Is mouse button down this frame?
LD_API b32 input_is_mouse_button_down( MouseCode code );
/// Was mouse button down last frame?
LD_API b32 input_was_mouse_button_down( MouseCode code );
/// Has mouse button been pressed this frame?
maybe_unused
header_only b32 input_mouse_button_press( MouseCode code ) {
    b32 is  = input_is_mouse_button_down( code );
    b32 was = input_was_mouse_button_down( code );
    return (is != was) && is;
}
/// Current frame's mouse position.
LD_API ivec2 input_mouse_position(void);
/// Last frame's mouse position.
LD_API ivec2 input_last_mouse_position(void);
/// Current frame's mouse wheel state.
LD_API i32 input_mouse_wheel(void);
/// Last frame's mouse wheel state.
LD_API i32 input_last_mouse_wheel(void);
/// Did mouse wheel change this frame?
maybe_unused
header_only b32 input_mouse_wheel_moved(void) {
    i32 mw  = input_mouse_wheel();
    i32 lmw = input_last_mouse_wheel();
    return mw != lmw;
}
/// Current frame's horizontal mouse wheel state.
LD_API i32 input_horizontal_mouse_wheel(void);
/// Last frame's horizontal mouse wheel state.
LD_API i32 input_last_horizontal_mouse_wheel(void);
/// Did horizontal mouse wheel change this frame?
maybe_unused
header_only b32 input_horizontal_mouse_wheel_moved(void) {
    i32 mw  = input_horizontal_mouse_wheel();
    i32 lmw = input_last_horizontal_mouse_wheel();
    return mw != lmw;
}
/// Is gamepad active?
LD_API b32 input_gamepad_is_active( u32 index );
/// Is gamepad button down this frame?
LD_API b32 input_is_gamepad_button_down( u32 index, GamepadCode code );
/// Was gamepad button down last frame?
LD_API b32 input_was_gamepad_button_down( u32 index, GamepadCode code );
/// Has gamepad button been pressed this frame?
maybe_unused
header_only b32 input_gamepad_button_press( u32 index, GamepadCode code ) {
    b32 is  = input_is_gamepad_button_down( index, code );
    b32 was = input_was_gamepad_button_down( index, code );
    return (is != was) && is;
}
/// Current frame's gamepad stick left state.
LD_API vec2 input_gamepad_stick_left( u32 index );
/// Last frame's gamepad stick left state.
LD_API vec2 input_gamepad_last_stick_left( u32 index );
/// Current frame's gamepad stick right state.
LD_API vec2 input_gamepad_stick_right( u32 index );
/// Last frame's gamepad stick right state.
LD_API vec2 input_gamepad_last_stick_right( u32 index );
/// Current frame's gamepad trigger left state.
LD_API f32 input_gamepad_trigger_left( u32 index );
/// Last frame's gamepad trigger left state.
LD_API f32 input_gamepad_last_trigger_left( u32 index );
/// Current frame's gamepad trigger right state.
LD_API f32 input_gamepad_trigger_right( u32 index );
/// Last frame's gamepad trigger left state.
LD_API f32 input_gamepad_last_trigger_right( u32 index );
/// Set gamepad's motor state.
LD_API void input_gamepad_set_motor_state( u32 index, u32 motor, f32 value );
/// Get gamepad's motor state.
LD_API f32 input_gamepad_motor_state( u32 index, u32 motor );
/// Get gamepad stick left deadzone.
LD_API f32 input_gamepad_stick_left_deadzone( u32 index );
/// Get gamepad stick right deadzone.
LD_API f32 input_gamepad_stick_right_deadzone( u32 index );
/// Get gamepad trigger left deadzone.
LD_API f32 input_gamepad_trigger_left_deadzone( u32 index );
/// Get gamepad trigger right deadzone.
LD_API f32 input_gamepad_trigger_right_deadzone( u32 index );
/// Get gamepad trigger press threshold.
/// This is the value at which the trigger registers as a button press.
LD_API f32 input_gamepad_trigger_press_threshold( u32 index );
/// Set stick left deadzone.
LD_API void input_gamepad_set_stick_left_deadzone( u32 index, f32 deadzone );
/// Set stick right deadzone.
LD_API void input_gamepad_set_stick_right_deadzone( u32 index, f32 deadzone );
/// Set trigger left deadzone.
LD_API void input_gamepad_set_trigger_left_deadzone( u32 index, f32 deadzone );
/// Set trigger right deadzone.
LD_API void input_gamepad_set_trigger_right_deadzone( u32 index, f32 deadzone );
/// Set trigger press threshold.
/// This is the value at which the trigger registers as a button press.
LD_API void input_gamepad_set_trigger_press_threshold( u32 index, f32 threshold );
/// Convert mouse pixel position to normalized device coordinates.
maybe_unused
header_only vec2 mouse_position_to_ndc( ivec2 position, ivec2 surface_dimensions ) {
    vec2 result = {
        (f32)position.x / (f32)surface_dimensions.x,
        (f32)position.y / (f32)surface_dimensions.y,
    };

    return v2_mul( v2_sub( result, v2_scalar( 0.5f ) ), 2.0f );
}

#endif // header guard
