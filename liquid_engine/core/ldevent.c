// * Description:  Event Subsystem Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "core/ldevent.h"
#include "core/ldcollections.h"

#define MIN_LISTENERS 2

typedef struct ListenerContext {
    EventCallbackFN* callback;
    void*            callback_params;

    u16 id;
} ListenerContext;

typedef struct ListenerRegistry {
    struct {
        ListenerContext* listeners;
    } event_listeners[EVENT_CODE_MAX];
} ListenerRegistry;

typedef struct EventBuffer {
    Event* list_concurrent_events;
    Event* list_end_of_frame_events;
} EventBuffer;

global EventBuffer       EVENT_BUFFER;
global ListenerRegistry* REGISTRY = NULL;
global EventListenerID   ID = 1;

LD_API void event_fire_priority( Event event, EventPriority priority ) {
    if( !REGISTRY ) {
        return;
    }

    switch( priority ) {
        case EVENT_PRIORITY_IMMEDIATE: {
            ListenerContext* listeners =
                REGISTRY->event_listeners[event.code].listeners;
            u32 listener_count = list_count( listeners );
            for( u32 i = 0; i < listener_count; ++i ) {
                listeners[i].callback( &event, listeners[i].callback_params );
            }
        } break;
        case EVENT_PRIORITY_END_OF_FRAME: {
            ASSERT( EVENT_BUFFER.list_end_of_frame_events );
            list_push(
                EVENT_BUFFER.list_end_of_frame_events,
                event
            );
        } break;
        case EVENT_PRIORITY_CONCURRENT: {
            // TODO(alicia): concurrent events!
            UNIMPLEMENTED();
        } break;
    }
}
void event_fire_end_of_frame() {
    ASSERT( EVENT_BUFFER.list_end_of_frame_events );
    ASSERT( REGISTRY );
    return;
    while( list_count( EVENT_BUFFER.list_end_of_frame_events ) ) {
        Event event = {};
        list_pop( EVENT_BUFFER.list_end_of_frame_events, &event );
        ListenerContext* listeners =
            REGISTRY->event_listeners[event.code].listeners;
        u32 listener_count = list_count( listeners );
        for( u32 i = 0; i < listener_count; ++i ) {
            listeners[i].callback( &event, listeners[i].callback_params );
        }
    }
}

LD_API EventListenerID event_subscribe(
    EventCode       event_code,
    EventCallbackFN callback,
    void*           callback_params
) {
    ASSERT( REGISTRY );

    EventListenerID result = (event_code << 8) | ID++;

    ListenerContext* listeners = REGISTRY->event_listeners[event_code].listeners;
    
    ListenerContext ctx = {};
    ctx.id              = result;
    ctx.callback        = callback;
    ctx.callback_params = callback_params;

    listeners = (ListenerContext*)_list_push( listeners, &ctx );

    return result;
}
LD_API void event_unsubscribe( EventListenerID event_listener_id ) {
    ASSERT( REGISTRY );

    EventCode event_code = (EventCode)(event_listener_id >> 8);

    ListenerContext* listeners =
        REGISTRY->event_listeners[event_code].listeners;
    u32 listener_count = list_count( listeners );

    b32 listener_found = false;
    u32 listener_index = 0;

    for( u32 i = 0; i < listener_count; ++i ) {
        if( event_listener_id == listeners[i].id ) {
            listener_index = i;
            listener_found = true;
            break;
        }
    }

    if( !listener_found ) {
        LOG_ERROR("Could not find event listener {u16}!", event_listener_id);
        return;
    }

    listeners = (ListenerContext*)list_remove(
        listeners,
        listener_index,
        NULL
    );

}

usize event_query_subsystem_size() {
    return sizeof(ListenerRegistry);
}
b32 event_subsystem_init( void* event_subsystem_buffer ) {
    REGISTRY = (ListenerRegistry*)event_subsystem_buffer;
    EVENT_BUFFER.list_concurrent_events   = list_reserve( Event, 10 );
    EVENT_BUFFER.list_end_of_frame_events = list_reserve( Event, 10 );

    if( !EVENT_BUFFER.list_concurrent_events ) {
        LOG_FATAL( "Failed to allocate concurrent event buffer!" );
        return false;
    }
    if( !EVENT_BUFFER.list_end_of_frame_events ) {
        LOG_FATAL( "Failed to allocate end of frame event buffer!" );
        return false;
    }

    for( u32 i = 0; i < EVENT_CODE_MAX; ++i ) {
        void* listener_list = _list_create(
            MIN_LISTENERS,
            sizeof(ListenerContext)
        );
        if( !listener_list ) {
            LOG_FATAL("Failed to create listener list!");
            return false;
        }
        REGISTRY->event_listeners[i].listeners = (ListenerContext*)listener_list;
    }

    LOG_INFO("Event subsystem successfully initialized.");
    return true;
}

