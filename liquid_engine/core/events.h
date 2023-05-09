#if !defined(EVENTS_HPP)
#define EVENTS_HPP
/**
 * Description:  Events
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"
#include "input.h"
#include "core/math/types.h"

enum EventCode : u32 {
    EVENT_CODE_UNKNOWN,

    EVENT_CODE_SURFACE_DESTROY,
    EVENT_CODE_SURFACE_ACTIVE,
    EVENT_CODE_SURFACE_RESIZE,
    EVENT_CODE_SURFACE_MOVE,

    EVENT_CODE_INPUT_KEY,
    EVENT_CODE_INPUT_MOUSE_BUTTON,
    EVENT_CODE_INPUT_MOUSE_MOVE,
    EVENT_CODE_INPUT_MOUSE_WHEEL,
    EVENT_CODE_INPUT_HORIZONTAL_MOUSE_WHEEL,
    EVENT_CODE_INPUT_GAMEPAD_BUTTON,
    EVENT_CODE_INPUT_GAMEPAD_STICK_LEFT,
    EVENT_CODE_INPUT_GAMEPAD_STICK_RIGHT,
    EVENT_CODE_INPUT_GAMEPAD_TRIGGER_LEFT,
    EVENT_CODE_INPUT_GAMEPAD_TRIGGER_RIGHT,
    EVENT_CODE_INPUT_GAMEPAD_ACTIVATE,

    EVENT_CODE_MOUSE_CURSOR_CHANGED,

    EVENT_CODE_LAST_RESERVED,
    MAX_ENGINE_EVENT_CODE = 0xFF,

    MAX_EVENT_CODE = 0x200
};
inline const char* engine_event_code_to_string(
    EventCode code
) {
    local const char* strings[EVENT_CODE_LAST_RESERVED] = {
        "Event Unknown",

        "Event Surface Destroy",
        "Event Surface Active",
        "Event Surface Resize",
        "Event Surface Move",

        "Event Input Key",
        "Event Mouse Button",
        "Event Mouse Move",
        "Event Mouse Wheel",
        "Event Mouse Horizontal Wheel",
        "Event Gamepad Button",
        "Event Gamepad Stick Left",
        "Event Gamepad Stick Right",
        "Event Gamepad Trigger Left",
        "Event Gamepad Trigger Right",
        "Event Gamepad Activate",
        "Event Mouse Cursor Changed"
    };
    if( code >= EVENT_CODE_LAST_RESERVED ) {
        return nullptr;
    }

    return strings[code];
}

enum EventConsumption : u32 {
    EVENT_NOT_CONSUMED,
    EVENT_CONSUMED
};

struct Event {
    EventCode code;
    union EventData {
        union {
            pvoid pointer[2];

            i64 int64[2];
            u64 uint64[2];
            f64 float64[2];

            i32 int32[4];
            u32 uint32[4];
            f32 float32[4];

            i16 int16[8];
            u16 uint16[8];

            i8 int8[16];
            u8 uint8[16];

            char c[16];
        } raw;

        struct {
            void* surface;
        } surface_destroy;
        struct {
            void* surface;
            b8    is_active;
        } surface_active;
        struct {
            void* surface;
            union {
                struct { i32 width, height; };
                ivec2 dimensions;
            };
        } surface_resize;
        struct {
            void* surface;
            union {
                struct { i32 x, y; };
                ivec2 position;
            };
        } surface_move;

        struct {
            KeyCode code;
            b32     is_down;
        } keyboard;
        struct {
            MouseCode code;
            b32       is_down;
        } mouse_button;
        union {
            struct { i32 x, y; };
            ivec2 coord;
        } mouse_move;
        struct {
            i32 delta;
        } mouse_wheel;
        struct {
            u32 gamepad_index;
        } gamepad_activate;
        struct {
            PadCode code;
            u8      gamepad_index;
            b32     is_down;
        } gamepad_button;
        struct {
            f32 value;
            u8  gamepad_index;
        } gamepad_trigger;
        struct {
            vec2 value;
            u8   gamepad_index;
        } gamepad_stick;
    } data;
};

#if defined(SM_API_INTERNAL)

    b32 event_init();
    b32 event_shutdown();

#endif // api internal

typedef EventConsumption (*EventListener)(
    Event* event,
    void* params
);

SM_API void event_fire( Event event );

SM_API b32 event_subscribe(
    EventCode     code,
    EventListener listener,
    void*         listener_params
);

SM_API b32 event_subscribe_multiple_codes(
    usize         code_count,
    EventCode*    codes,
    EventListener listener,
    void*         params
);

SM_API b32 event_unsubscribe(
    EventCode     code,
    EventListener listener,
    void*         listener_params
);

SM_API b32 event_unsubscribe_multiple_codes(
    usize         code_count,
    EventCode*    codes,
    EventListener listener,
    void*         listener_params
);

#endif // header guard