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

#define MAX_EVENT_SUBSCRIPTIONS 16

enum EventCallbackReturnCode : u32 {
    EVENT_CALLBACK_NOT_CONSUMED,
    EVENT_CALLBACK_CONSUMED
};
typedef EventCallbackReturnCode (*EventCallbackFN)(
    struct Event* event,
    void* params
);

enum EventCode : u32 {
    EVENT_CODE_UNKNOWN,

    EVENT_CODE_APP_EXIT,

    EVENT_CODE_SURFACE_DESTROY,
    EVENT_CODE_SURFACE_ACTIVE,
    EVENT_CODE_SURFACE_RESIZE,
    EVENT_CODE_SURFACE_MOVE,

    EVENT_CODE_GAMEPAD_ACTIVATE,

    EVENT_CODE_MOUSE_CURSOR_STYLE_CHANGED,

    EVENT_CODE_LAST_RESERVED,
    MAX_ENGINE_EVENT_CODE = 0xFF,

    MAX_EVENT_CODE = 0x200
};
/// Subscribe a listener to an event.
LD_API b32 event_subscribe(
    EventCode       code,
    EventCallbackFN callback_function,
    void*           callback_params
);
/// Unsubscribe a listener from an event.
LD_API b32 event_unsubscribe(
    EventCode       code,
    EventCallbackFN callback_function,
    void*           callback_params
);
/// Check how many more listeners the specified event can have.
/// Returns negative if event code is invalid.
LD_API i32 event_available_listener_count( EventCode code );

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
            b8 is_active;
        } surface_active;
        struct {
            union {
                struct { i32 width, height; };
                ivec2 dimensions;
            };
        } surface_resize;
        struct {
            union {
                struct { i32 x, y; };
                ivec2 position;
            };
        } surface_move;

        struct {
            u32 gamepad_index;
        } gamepad_activate;
    } data;
};
/// Fire an event.
LD_API void event_fire( Event event );

#if defined(LD_API_INTERNAL)

    b32  event_init();
    void event_shutdown();

#endif // api internal

inline const char* engine_event_code_to_string(
    EventCode code
) {
    local const char* strings[EVENT_CODE_LAST_RESERVED] = {
        "Event Unknown",

        "Event App Exit",

        "Event Surface Destroy",
        "Event Surface Active",
        "Event Surface Resize",
        "Event Surface Move",

        "Event Gamepad Activate",
        "Event Mouse Cursor Changed"
    };
    if( code >= EVENT_CODE_LAST_RESERVED ) {
        return nullptr;
    }

    return strings[code];
}

#endif // header guard
