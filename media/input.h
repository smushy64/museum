#if !defined(LD_MEDIA_INPUT_H)
#define LD_MEDIA_INPUT_H
/**
 * Description:  Media input.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"

/// Media Mouse Button.
typedef enum MediaMouseButton : u8 {
    MEDIA_MOUSE_BUTTON_LEFT,
    MEDIA_MOUSE_BUTTON_MIDDLE,
    MEDIA_MOUSE_BUTTON_RIGHT,
    MEDIA_MOUSE_BUTTON_EXTRA_1,
    MEDIA_MOUSE_BUTTON_EXTRA_2
} MediaMouseButton;

/// Media Keyboard Key.
/// Corresponds to Liquid Engine Keyboard Codes.
typedef u8 MediaKeyboardKey;

/// Media Gamepad Button bitfield.
typedef u16 MediaGamepadButtons;
#define MEDIA_GAMEPAD_BUTTON_DPAD_UP           (1 << 0)
#define MEDIA_GAMEPAD_BUTTON_DPAD_DOWN         (1 << 1)
#define MEDIA_GAMEPAD_BUTTON_DPAD_LEFT         (1 << 2)
#define MEDIA_GAMEPAD_BUTTON_DPAD_RIGHT        (1 << 3)
#define MEDIA_GAMEPAD_BUTTON_START             (1 << 4)
#define MEDIA_GAMEPAD_BUTTON_SELECT            (1 << 5)
#define MEDIA_GAMEPAD_BUTTON_STICK_LEFT_CLICK  (1 << 6)
#define MEDIA_GAMEPAD_BUTTON_STICK_RIGHT_CLICK (1 << 7)
#define MEDIA_GAMEPAD_BUTTON_BUMPER_LEFT       (1 << 8)
#define MEDIA_GAMEPAD_BUTTON_BUMPER_RIGHT      (1 << 9)
#define MEDIA_GAMEPAD_EXT_BUTTON_TRIGGER_LEFT  (1 << 10)
#define MEDIA_GAMEPAD_EXT_BUTTON_TRIGGER_RIGHT (1 << 11)
#define MEDIA_GAMEPAD_BUTTON_FACE_DOWN         (1 << 12)
#define MEDIA_GAMEPAD_BUTTON_FACE_RIGHT        (1 << 13)
#define MEDIA_GAMEPAD_BUTTON_FACE_LEFT         (1 << 14)
#define MEDIA_GAMEPAD_BUTTON_FACE_UP           (1 << 15)

/// Media Gamepad State.
typedef struct MediaGamepadState {
    MediaGamepadButtons buttons;
    u16 trigger_left;
    u16 trigger_right;

    i16 stick_left_x;
    i16 stick_left_y;

    i16 stick_right_x;
    i16 stick_right_y;
} MediaGamepadState;

/// Query state of a gamepad.
/// Returns true if gamepad is active and reading state was successful.
/// Returns false if gamepad is inactive.
MEDIA_API b32 media_input_query_gamepad_state(
    u32 gamepad_index, MediaGamepadState* out_state );
/// Set rumble for gamepad.
/// Returns true if successful (gamepad is connected).
MEDIA_API b32 media_input_set_gamepad_rumble(
    u32 gamepad_index, u16 motor_left, u16 motor_right );
/// Query a gamepad's rumble state.
MEDIA_API void media_input_query_gamepad_rumble(
    u32 gamepad_index, u16* out_motor_left, u16* out_motor_right );
/// Set cursor visible or invisible.
MEDIA_API void media_input_set_cursor_visible( b32 is_visible );

#endif /* header guard */
