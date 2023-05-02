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

enum EventType : u32 {
    EVENT_TYPE_SURFACE_DESTROY,
};

#define EVENT_MAX_USER_DATA_SIZE 16

struct Event {
    EventType type;
    union EventData {
        u8 bytes[EVENT_MAX_USER_DATA_SIZE];
        struct {
            KeyCode code;
            b32     is_down;
        } keyboard;
        struct {
            MouseCode code;
            b32       is_down;
        } mouse_button;
        union {
            i32   x, y;
            ivec2 coord;
        } mouse_position;
    } data;
};

#endif