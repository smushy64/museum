// * Description:  Event Subsystem Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "core/ldevent.h"
#include "core/ldmemory.h"

#define EVENT_LISTENER_ID_GET_EVENT_CODE( id )\
    ((u8)(((EventListenerID)(id)) >> (u16)7))
#define EVENT_LISTENER_ID_GET_INDEX( id )\
    ((u8)(((EventListenerID)(id)) & 0x00FF))
#define MAKE_EVENT_LISTENER_ID( event_code, index )\
    (EventListenerID)( ((u16)(event_code)) << 7 | ((u16)(index)) )

typedef struct CallbackContext {
    EventCallbackFN* callback;
    void* params;
    EventListenerID id;
} CallbackContext;

typedef struct CallbackRegistry {
    CallbackContext callbacks[EVENT_CODE_MAX][EVENT_LISTENER_MAX];
} CallbackRegistry;

global CallbackRegistry* REGISTRY = NULL;

usize event_subsystem_query_size() {
    return sizeof(CallbackRegistry);
}
b32 event_subsystem_init( void* buffer ) {
    REGISTRY = buffer;
    LOG_INFO( "Event subsystem successfully initialized." );
    return true;
}
void event_fire_end_of_frame() {
    // TODO(alicia): Fire events at the end of a frame.
}

LD_API void event_fire_priority( Event event, EventPriority priority ) {
    switch( priority ) {
        case EVENT_PRIORITY_IMMEDIATE: {
            CallbackContext* callbacks = REGISTRY->callbacks[event.code];
            for( usize i = 0; i < EVENT_LISTENER_MAX; ++i ) {
                CallbackContext* current = &callbacks[i];
                if( !current->id ) {
                    continue;
                }
                if( current->callback( &event, current->params ) ) {
                    break;
                }
            }
        } break;
        default: { UNIMPLEMENTED(); } break;
    }
}
LD_API EventListenerID event_subscribe(
    EventCode event, EventCallbackFN* callback, void* params
) {
    EventListenerID id = EVENT_LISTENER_INVALID_ID;
    if( event >= EVENT_CODE_MAX ) {
        LOG_ERROR(
            "Attempted to subscribe listener to "
            "invalid event: {u8}!",
            event
        );
        return id;
    }

    CallbackContext* callbacks = REGISTRY->callbacks[event];
    u8 empty_callback_index = U8_MAX;

    for( u8 i = 0; i < EVENT_LISTENER_MAX; ++i ) {
        if( !callbacks->id ) {
            empty_callback_index = i;
            break;
        }
    }
    /// empty callback not found
    if( empty_callback_index == U8_MAX ) {
        LOG_ERROR(
            "Attempted to subscribe to event but "
            "this event already has too many listeners!"
        );
        LOG_ERROR(
            "Max number of listeners an "
            "event can have is: {u8}",
            EVENT_LISTENER_MAX
        );
        return id;
    }

    id = MAKE_EVENT_LISTENER_ID( event, empty_callback_index );

    CallbackContext* empty_callback = &callbacks[empty_callback_index];
    empty_callback->id       = id;
    empty_callback->callback = callback;
    empty_callback->params   = params;

    return id;
}
LD_API void event_unsubscribe( EventListenerID id ) {
    u8 event_code = EVENT_LISTENER_ID_GET_EVENT_CODE(id);
    u8 index      = EVENT_LISTENER_ID_GET_INDEX(id);

    ASSERT( event_code < EVENT_CODE_MAX );
    ASSERT( index < EVENT_LISTENER_MAX );

    mem_zero(
        &REGISTRY->callbacks[event_code][index],
        sizeof(CallbackContext)
    );
}

