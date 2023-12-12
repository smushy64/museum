#if !defined(LD_CORE_INPUT_H)
#define LD_CORE_INPUT_H
/**
 * Description:  Input Codes
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
 * Notes:        Mouse Position Coordinates
 *                 -X: Left  -Y: Bottom
 *                  X: Right  Y: Top
*/
#include "defines.h"
#include "constants.h"

#define INPUT_GAMEPAD_COUNT (4)

/// Key Codes
typedef enum KeyCode : u8 {
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

    KEY_COUNT,
    KEY_UNKNOWN = U8_MAX,
} KeyCode;

header_only const char* keyboard_code_to_cstr( KeyCode keycode ) {
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

typedef u8 MouseCode;
#define MOUSE_BUTTON_LEFT   (1 << 0)
#define MOUSE_BUTTON_MIDDLE (1 << 1)
#define MOUSE_BUTTON_RIGHT  (1 << 2)
#define MOUSE_BUTTON_X1     (1 << 3)
#define MOUSE_BUTTON_X2     (1 << 4)

typedef u16 GamepadCode;
#define GAMEPAD_CODE_DPAD_UP       (1 << 0)
#define GAMEPAD_CODE_DPAD_DOWN     (1 << 1)
#define GAMEPAD_CODE_DPAD_LEFT     (1 << 2)
#define GAMEPAD_CODE_DPAD_RIGHT    (1 << 3)
#define GAMEPAD_CODE_START         (1 << 4)
#define GAMEPAD_CODE_SELECT        (1 << 5)
#define GAMEPAD_CODE_STICK_LEFT    (1 << 6)
#define GAMEPAD_CODE_STICK_RIGHT   (1 << 7)
#define GAMEPAD_CODE_BUMPER_LEFT   (1 << 8)
#define GAMEPAD_CODE_BUMPER_RIGHT  (1 << 9)
#define GAMEPAD_CODE_TRIGGER_LEFT  (1 << 10)
#define GAMEPAD_CODE_TRIGGER_RIGHT (1 << 11)
#define GAMEPAD_CODE_FACE_DOWN     (1 << 12)
#define GAMEPAD_CODE_FACE_RIGHT    (1 << 13)
#define GAMEPAD_CODE_FACE_LEFT     (1 << 14)
#define GAMEPAD_CODE_FACE_UP       (1 << 15)

/// Check if key went down this frame.
LD_API b32 input_key_down( KeyCode code );
/// Check if key went up this frame.
LD_API b32 input_key_up( KeyCode code );
/// Check if key is down.
LD_API b32 input_key( KeyCode code );

/// Check if mouse button went down this frame.
LD_API b32 input_mouse_down( MouseCode code );
/// Check if mouse button went up this frame.
LD_API b32 input_mouse_up( MouseCode code );
/// Check if mouse button is down.
LD_API b32 input_mouse_button( MouseCode code );

/// Get mouse absolute position (0 to resolution x).
LD_API i32 input_mouse_absolute_position_x(void);
/// Get mouse absolute position (0 to resolution y).
LD_API i32 input_mouse_absolute_position_y(void);

/// Get mouse position (0 to 1, 1 being right-most position).
LD_API f32 input_mouse_position_x(void);
/// Get mouse position (0 to 1, 1 being top-most position).
LD_API f32 input_mouse_position_y(void);

/// Get mouse position relative to last frame's mouse position in x.
LD_API f32 input_mouse_relative_x(void);
/// Get mouse position relative to last frame's mouse position in y.
LD_API f32 input_mouse_relative_y(void);

/// Get mouse wheel (-1, 0 or 1).
LD_API i32 input_mouse_wheel(void);
/// Get horizontal mouse wheel (-1, 0 or 1).
LD_API i32 input_mouse_wheel_horizontal(void);

/// Lock or unlock mouse from screen.
LD_API void input_mouse_set_locked( b32 is_locked );
/// Check if mouse is currently locked to screen.
LD_API b32 input_is_mouse_locked(void);

/// Check if gamepad button went down this frame.
/// Multiple buttons can be checked simultaneously by OR'ing gamepad codes.
LD_API b32 input_gamepad_down( usize gamepad, GamepadCode code );
/// Check if gamepad button went up this frame.
/// Multiple buttons can be checked simultaneously by OR'ing gamepad codes.
LD_API b32 input_gamepad_up( usize gamepad, GamepadCode code );
/// Check if gamepad button is down.
/// Multiple buttons can be checked simultaneously by OR'ing gamepad codes.
LD_API b32 input_gamepad_button( usize gamepad, GamepadCode code );

/// Get gamepad left stick x (-1 to 1).
LD_API f32 input_gamepad_stick_left_x( usize gamepad );
/// Get gamepad left stick y (-1 to 1).
LD_API f32 input_gamepad_stick_left_y( usize gamepad );
/// Get gamepad right stick x (-1 to 1).
LD_API f32 input_gamepad_stick_right_x( usize gamepad );
/// Get gamepad right stick y (-1 to 1).
LD_API f32 input_gamepad_stick_right_y( usize gamepad );

/// Get gamepad left trigger (0 to 1).
LD_API f32 input_gamepad_trigger_left( usize gamepad );
/// Get gamepad right trigger (0 to 1).
LD_API f32 input_gamepad_trigger_right( usize gamepad );

/// Set rumble values for gamepad.
LD_API void input_gamepad_set_rumble(
    usize gamepad, f32 rumble_left, f32 rumble_right );
/// Query what the rumble values are for gamepad.
LD_API void input_gamepad_query_rumble(
    usize gamepad, f32* out_rumble_left, f32* out_rumble_right );

/// Check if gamepad is active.
LD_API b32 input_gamepad_is_active( usize gamepad );

#if defined(LD_API_INTERNAL)
    enum PlatformMouseCode : u8;

    /// Query input subsystem memory requirement.
    usize input_subsystem_query_memory_requirement(void);
    /// Initialize input subsystem.
    /// Provided buffer must be large enough to hold the result of
    /// input_subsystem_query_memory_requirement().
    void input_subsystem_initialize( void* buffer );
    /// Swap input states.
    void input_subsystem_swap_state(void);
    /// Update gamepads.
    void input_subsystem_update_gamepads(void);
    /// Set key state.
    void input_subsystem_set_key( KeyCode code, b32 is_down );
    /// Set mouse button state.
    void input_subsystem_set_mouse_button( enum PlatformMouseCode code, b32 is_down );
    /// Set mouse wheel.
    void input_subsystem_set_mouse_wheel( i32 wheel );
    /// Set horizontal mouse wheel.
    void input_subsystem_set_mouse_wheel_horizontal( i32 wheel );
    /// Set mouse position.
    void input_subsystem_set_mouse_position( i32 x, i32 y, f32 x01, f32 y01 );
    /// Set mouse relative positions.
    void input_subsystem_set_mouse_relative( f32 x_rel, f32 y_rel );
#endif

#endif // header guard
