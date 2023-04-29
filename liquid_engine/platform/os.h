#if !defined(PLATFORM_HPP)
#define PLATFORM_HPP
/**
 * Description:  Operating System related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#include "core/smath.h"

typedef u32 PlatformInitFlags;

#define PLATFORM_INIT_DPI_AWARE ( 1 << 0 )

SM_API b32 platform_init( PlatformInitFlags flags );
SM_API void platform_shutdown();

typedef void* SurfaceHandle;
struct Surface {
    SurfaceHandle handle;
    union {
        ivec2 position;
        struct { i32 x; i32 y; };
    };
    union {
        ivec2 dimensions;
        struct { i32 width; i32 height; };
    };
    b32 is_focused;
    void* platform_data;
};

typedef u32 SurfaceCreateFlags;

#define SURFACE_FLAG_SHOW_ON_CREATE    ( 1 << 1 )
#define SURFACE_FLAG_POSITION_CENTERED ( 1 << 2 )

SM_API b32 surface_create(
    const char*        surface_name,
    ivec2              dimensions,
    SurfaceCreateFlags flags,
    SurfaceHandle      opt_parent_surface,
    Surface*           out_surface
);

/// Types of message boxes
enum MessageBoxType : u32 {
    MBTYPE_OK,
    MBTYPE_OKCANCEL,
    MBTYPE_RETRYCANCEL,
    MBTYPE_YESNO,
    MBTYPE_YESNOCANCEL,

    MBTYPE_COUNT
};
inline const char* to_string( MessageBoxType type ) {
    static const char* strings[MBTYPE_COUNT] = {
        "Message Box with OK button.",
        "Message Box with OK and CANCEL buttons.",
        "Message Box with RETRY and CANCEL buttons.",
        "Message Box with YES and NO buttons.",
        "Message Box with YES, NO and CANCEL buttons.",
    };
    if( type >= MBTYPE_COUNT ) {
        return "Unknown Message Box type.";
    }
    return strings[type];
}

/// Message box icons
enum MessageBoxIcon : u32 {
    MBICON_INFORMATION,
    MBICON_WARNING,
    MBICON_ERROR,

    MBICON_COUNT
};
inline const char* to_string( MessageBoxIcon icon ) {
    static const char* strings[MBICON_COUNT] = {
        "Message Box \"information\" icon.",
        "Message Box \"warning\" icon.",
        "Message Box \"error\" icon.",
    };
    if( icon >= MBICON_COUNT ) {
        return "Unknown Message Box icon.";
    }
    return strings[icon];
}

/// User selection from a message box or an error from creating message box.
enum MessageBoxResult : u32 {
    MBRESULT_OK,
    MBRESULT_CANCEL,
    MBRESULT_RETRY,
    MBRESULT_YES,
    MBRESULT_NO,

    MBRESULT_UNKNOWN_ERROR,

    MBRESULT_COUNT
};
inline const char* to_string( MessageBoxResult result ) {
    static const char* strings[MBRESULT_COUNT] = {
        "Message Box OK selected.",
        "Message Box CANCEL selected.",
        "Message Box RETRY selected.",
        "Message Box YES selected.",
        "Message Box NO selected.",
        "An unknown error occurred.",
    };
    if( result >= MBRESULT_COUNT ) {
        return "Unknown Message Box result.";
    }
    return strings[result];
}

/// Create a message box to report urgent information.
SM_API MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
);

#define MESSAGE_BOX_FATAL( title, message ) \
    message_box(\
        title,\
        message,\
        MBTYPE_OK,\
        MBICON_ERROR\
    )

/// Put the current thread to sleep for the specified amount of milliseconds.
SM_API void sleep( u32 ms );

typedef u16 ProcessorFeatures;

#define SSE_MASK    (1 << 0)
#define SSE2_MASK   (1 << 1)
#define SSE3_MASK   (1 << 2)
#define SSSE3_MASK  (1 << 3)
#define SSE4_1_MASK (1 << 4)
#define SSE4_2_MASK (1 << 5)
#define AVX_MASK    (1 << 6)
#define AVX2_MASK   (1 << 7)
#define AVX512_MASK (1 << 8)

#define IS_SSE_AVAILABLE(processor_features)\
    ( (processor_features & SSE_MASK) == SSE_MASK )
#define IS_SSE2_AVAILABLE(processor_features)\
    ( (processor_features & SSE2_MASK) == SSE2_MASK )
#define IS_SSE3_AVAILABLE(processor_features)\
    ( (processor_features & SSE3_MASK) == SSE3_MASK )
#define IS_SSSE3_AVAILABLE(processor_features)\
    ( (processor_features & SSSE3_MASK) == SSSE3_MASK )
#define IS_SSE4_1_AVAILABLE(processor_features)\
    ( (processor_features & SSE4_1_MASK) == SSE4_1_MASK )
#define IS_SSE4_2_AVAILABLE(processor_features)\
    ( (processor_features & SSE4_2_MASK) == SSE4_2_MASK )

#define ARE_SSE_INSTRUCTIONS_AVAILABLE(processor_features) (\
    IS_SSE_AVAILABLE(processor_features)    &&\
    IS_SSE2_AVAILABLE(processor_features)   &&\
    IS_SSE3_AVAILABLE(processor_features)   &&\
    IS_SSSE3_AVAILABLE(processor_features)  &&\
    IS_SSE4_1_AVAILABLE(processor_features) &&\
    IS_SSE4_2_AVAILABLE(processor_features)   \
)

#define IS_AVX_AVAILABLE(processor_features)\
    ( (processor_features & AVX_MASK) == AVX_MASK )
#define IS_AVX2_AVAILABLE(processor_features)\
    ( (processor_features & AVX2_MASK) == AVX2_MASK )
#define IS_AVX512_AVAILABLE(processor_features)\
    ( (processor_features & AVX512_MASK) == AVX512_MASK )

/// System information
struct SystemInfo {
    ProcessorFeatures features;
    char  cpu_name_buffer[65];
    usize total_memory;
    usize thread_count;
};
/// Get information about the current processor.
SM_API SystemInfo query_system_info();

/// Platform events.
enum EventType : u32 {
    EVENT_SURFACE_FOCUS_CHANGE,
    EVENT_SURFACE_DESTROY,
    EVENT_SURFACE_DIMENSIONS_CHANGE,
    EVENT_SURFACE_POSITION_CHANGE,

    EVENT_KEY_PRESS,

    EVENT_MOUSE_BUTTON_PRESS,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_WHEEL,

    EVENT_TYPE_COUNT
};
inline const char* to_string( EventType event_type ) {
    static const char* strings[EVENT_TYPE_COUNT] = {
        "Surface Focus Change",
        "Surface Destroy",
        "Surface Dimensions Change",
        "Surface Position Change",
        "Key Press",
        "Mouse Button Press",
        "Mouse Moved",
        "Mouse Wheel",
    };
    if( event_type >= EVENT_TYPE_COUNT ) {
        return "Unknown";
    }
    return strings[event_type];
}

/// Key Codes
enum KeyCode : u16 {
    KEYCODE_UNKNOWN = U16::MAX,
    KEYCODE_SPACE = 0,
    KEYCODE_A,
    KEYCODE_B,
    KEYCODE_C,
    KEYCODE_D,
    KEYCODE_E,
    KEYCODE_F,
    KEYCODE_G,
    KEYCODE_H,
    KEYCODE_I,
    KEYCODE_J,
    KEYCODE_K,
    KEYCODE_L,
    KEYCODE_M,
    KEYCODE_N,
    KEYCODE_O,
    KEYCODE_P,
    KEYCODE_Q,
    KEYCODE_R,
    KEYCODE_S,
    KEYCODE_T,
    KEYCODE_U,
    KEYCODE_V,
    KEYCODE_W,
    KEYCODE_X,
    KEYCODE_Y,
    KEYCODE_Z,
    KEYCODE_ESCAPE,
    KEYCODE_F1,
    KEYCODE_F2,
    KEYCODE_F3,
    KEYCODE_F4,
    KEYCODE_F5,
    KEYCODE_F6,
    KEYCODE_F7,
    KEYCODE_F8,
    KEYCODE_F9,
    KEYCODE_F10,
    KEYCODE_F11,
    KEYCODE_F12,
    KEYCODE_F13,
    KEYCODE_F14,
    KEYCODE_F15,
    KEYCODE_F16,
    KEYCODE_F17,
    KEYCODE_F18,
    KEYCODE_F19,
    KEYCODE_F20,
    KEYCODE_F21,
    KEYCODE_F22,
    KEYCODE_F23,
    KEYCODE_F24,
    KEYCODE_PRINT_SCREEN,
    KEYCODE_SCROLL_LOCK,
    KEYCODE_PAUSE,
    KEYCODE_BACKTICK,
    KEYCODE_0,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_8,
    KEYCODE_9,
    KEYCODE_MINUS,
    KEYCODE_EQUALS,
    KEYCODE_BACKSPACE,
    KEYCODE_INSERT,
    KEYCODE_HOME,
    KEYCODE_PAGE_UP,
    KEYCODE_TAB,
    KEYCODE_BRACKET_LEFT,
    KEYCODE_BRACKET_RIGHT,
    KEYCODE_SLASH_BACKWARD,
    KEYCODE_DELETE,
    KEYCODE_END,
    KEYCODE_PAGE_DOWN,
    KEYCODE_CAPSLOCK,
    KEYCODE_SEMICOLON,
    KEYCODE_QUOTE,
    KEYCODE_ENTER,
    KEYCODE_SHIFT_LEFT,
    KEYCODE_COMMA,
    KEYCODE_PERIOD,
    KEYCODE_SLASH_FORWARD,
    KEYCODE_SHIFT_RIGHT,
    KEYCODE_CONTROL_LEFT,
    KEYCODE_SUPER_LEFT,
    KEYCODE_ALT_LEFT,
    KEYCODE_ALT_RIGHT,
    KEYCODE_SUPER_RIGHT,
    KEYCODE_CONTROL_RIGHT,
    KEYCODE_ARROW_LEFT,
    KEYCODE_ARROW_RIGHT,
    KEYCODE_ARROW_UP,
    KEYCODE_ARROW_DOWN,
    KEYCODE_NUM_LOCK,
    KEYCODE_PAD_0,
    KEYCODE_PAD_1,
    KEYCODE_PAD_2,
    KEYCODE_PAD_3,
    KEYCODE_PAD_4,
    KEYCODE_PAD_5,
    KEYCODE_PAD_6,
    KEYCODE_PAD_7,
    KEYCODE_PAD_8,
    KEYCODE_PAD_9,

    KEYCODE_COUNT
};
inline const char* to_string( KeyCode keycode ) {
    static const char* strings[KEYCODE_COUNT] = {
        "Space",
        "A",
        "B",
        "C",
        "D",
        "E",
        "F",
        "G",
        "H",
        "I",
        "J",
        "K",
        "L",
        "M",
        "N",
        "O",
        "P",
        "Q",
        "R",
        "S",
        "T",
        "U",
        "V",
        "W",
        "X",
        "Y",
        "Z",
        "Escape",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "F11",
        "F12",
        "F13",
        "F14",
        "F15",
        "F16",
        "F17",
        "F18",
        "F19",
        "F20",
        "F21",
        "F22",
        "F23",
        "F24",
        "Print Screen",
        "Scroll Lock",
        "Pause",
        "`~",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "-_",
        "=+",
        "Backspace",
        "Insert",
        "Home",
        "PageUp",
        "Tab",
        "[{",
        "]}",
        "\\|",
        "Delete",
        "End",
        "Page Down",
        "Capslock",
        ";:",
        "\'\"",
        "Enter",
        "Left Shift",
        ",<",
        ".>",
        "/?",
        "Right Shift",
        "Left Control",
        "Left Super",
        "Left Alt",
        "Right Alt",
        "Right Super",
        "Right Control",
        "Left Arrow",
        "Right Arrow",
        "Up Arrow",
        "Down Arrow",
        "Numlock",
        "Keypad 0",
        "Keypad 1",
        "Keypad 2",
        "Keypad 3",
        "Keypad 4",
        "Keypad 5",
        "Keypad 6",
        "Keypad 7",
        "Keypad 8",
        "Keypad 9",
    };
    if( keycode >= KEYCODE_COUNT ) {
        return "Unknown";
    }
    return strings[keycode];
}

/// Mouse Button Codes
enum MouseCode : u16 {
    MOUSECODE_UNKNOWN = U16::MAX,
    MOUSECODE_BUTTON_LEFT = 0,
    MOUSECODE_BUTTON_MIDDLE,
    MOUSECODE_BUTTON_RIGHT,
    MOUSECODE_BUTTON_EXTRA_1,
    MOUSECODE_BUTTON_EXTRA_2,

    MOUSECODE_COUNT
};
inline const char* to_string( MouseCode mousecode ) {
    static const char* strings[MOUSECODE_COUNT] = {
        "Mouse Button Left",
        "Mouse Button Middle",
        "Mouse Button Right",
        "Mouse Button Extra 1",
        "Mouse Button Extra 2",
    };
    if( mousecode >= MOUSECODE_COUNT ) {
        return "Unknown";
    }
    return strings[mousecode];
}

/// OS events
struct Event {
    EventType type;
    union {
        struct {
            b32 is_focused;
        } focus_change;
        union {
            ivec2  dimensions;
            struct { i32 width; i32 height; };
        } dimensions_change;
        union {
            ivec2 position;
            struct { i32 x; i32 y; };
        } position_change;
        struct {
            KeyCode keycode;
            b8      is_down;
        } key_press;
        struct {
            MouseCode mousecode;
            b8        is_down;
        } mouse_button_press;
        union {
            struct { i32 x; i32 y; };
            ivec2 position;
        } mouse_move;
        struct {
            i32 delta;
            b8  is_horizontal;
        } mouse_wheel;
    };
};
/// Pop event from event queue.
/// Returns true if there was a pending event in the queue.
SM_API b32 next_event( SurfaceHandle surface, Event* event );

#endif