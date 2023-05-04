#if !defined(EVENTS_HPP)
#define EVENTS_HPP
/**
 * Description:  Events
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"
#include "input.h"
#include "smath.h"

#define MIN_EVENT_BUFFER_SIZE 32
#define MAX_INTERNAL_EVENT_CODE 256

#define MAX_EVENT_CODE 512

#define EVENT_CODE_INPUT_KEY              0x2
#define EVENT_CODE_INPUT_MOUSE_BUTTON     0x3
#define EVENT_CODE_INPUT_INPUT_MOUSE_MOVE 0x4

#define EVENT_CONSUMED     true
#define EVENT_NOT_CONSUMED false

typedef u32 EventCode;
#if defined( SM_API_INTERNAL )

    enum InternalEventCode : u32 {
        INTERNAL_EVENT_CODE_UNKNOWN            = 0x0,
        INTERNAL_EVENT_CODE_SURFACE_DESTROY    = 0x1,

        INTERNAL_EVENT_CODE_INPUT_KEY          = 0x2,
        INTERNAL_EVENT_CODE_INPUT_MOUSE_BUTTON = 0x3,
        INTERNAL_EVENT_CODE_INPUT_MOUSE_MOVE   = 0x4,

    };

#endif // api internal

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
    } data;
};

#if defined(SM_API_INTERNAL)

    b32 event_init();
    b32 event_shutdown();

#endif // api internal

typedef b32 (*EventListener)( Event* event, void* params );

SM_API void event_fire( Event event );

SM_API b32 event_subscribe(
    EventCode     code,
    EventListener listener,
    void*         listener_params
);

SM_API b32 event_unsubscribe(
    EventCode     code,
    EventListener listener,
    void*         listener_params
);

#endif // header guard