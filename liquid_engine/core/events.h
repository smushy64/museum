#if !defined(EVENTS_HPP)
#define EVENTS_HPP
/**
 * Description:  Events
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "defines.h"
#include "input.h"

#define MIN_EVENT_BUFFER_SIZE 32

enum EventType : u32 {
    EVENT_TYPE_SURFACE_DESTROY,
};

struct Event {
    EventType type;
    u32 surface_id;
    union {
        i64 int64_2[2];
        u64 uint64_2[2];
        f64 double_2[2];

        i32 int32_4[4];
        u32 uint32_4[4];
        f32 float_4[4];

        i16 int16_8[8];
        u16 uint16_8[8];

        i8 int8_16[16];
        u8 uint8_16[16];

        char char16[16];
    } data;
};

#endif