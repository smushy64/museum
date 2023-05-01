/**
 * Description:  Platform independent OS functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 30, 2023
*/
#include "os.h"
// TODO(alicia): custom formatting functions!
#include <stdio.h>

SM_API isize format_event(
    usize buffer_size,
    char* buffer,
    Event* event
) {
    switch( event->type ) {
        case EVENT_MOUSE_MOVE:
        case EVENT_SURFACE_POSITION_CHANGE:
        case EVENT_SURFACE_DIMENSIONS_CHANGE:
            return snprintf(
                buffer,
                buffer_size,
                "%s: { %i, %i }",
                to_string(event->type),
                event->int2.x,
                event->int2.y
            );
        case EVENT_SURFACE_FOCUS_CHANGE:
            return snprintf(
                buffer,
                buffer_size,
                "%s: %s",
                to_string( event->type ),
                event->focus_change.is_focused ?
                    "Focused" : "Out of focus"
            );
        case EVENT_KEY_PRESS:
            return snprintf(
                buffer,
                buffer_size,
                "%s: %s %s",
                to_string( event->type ),
                to_string( event->key_press.keycode ),
                event->key_press.is_down ?
                    "Down" : "Up"
            );
        case EVENT_MOUSE_BUTTON_PRESS:
            return snprintf(
                buffer,
                buffer_size,
                "%s: %s %s",
                to_string( event->type ),
                to_string( event->mouse_button_press.mousecode ),
                event->mouse_button_press.is_down ?
                    "Down" : "Up"
            );
        case EVENT_MOUSE_WHEEL:
            return snprintf(
                buffer,
                buffer_size,
                "%s: %s Delta %i",
                to_string( event->type ),
                event->mouse_wheel.is_horizontal ?
                    "Horizontal" : "Vertical",
                event->mouse_wheel.delta
            );
        default: return snprintf(
            buffer,
            buffer_size,
            "%s",
            to_string( event->type )
        );
    }
}
