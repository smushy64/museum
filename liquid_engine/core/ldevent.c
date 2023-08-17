// * Description:  Event Subsystem Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "core/ldevent.h"
#include "core/ldcollections.h"

#define MIN_LISTENERS 2

typedef struct ListenerContext {
    EventCallbackFN callback;
    void*           callback_params;

    u16 id;
} ListenerContext;

typedef struct ListenerRegistry {
    struct {
        ListenerContext* listeners;
    } event_listeners[MAX_EVENT_CODE];
} ListenerRegistry;

global ListenerRegistry* REGISTRY = NULL;
global EventListenerID   ID = 1;

LD_API void event_fire_priority( Event event, EventPriority priority ) {
    // TODO(alicia): HANDLE PRIORITY
    unused( priority );
    if( !REGISTRY ) {
        return;
    }
    ListenerContext* listeners = REGISTRY->event_listeners[event.code].listeners;
    u32 listener_count = list_count( listeners );
    for( u32 i = 0; i < listener_count; ++i ) {
        listeners[i].callback( &event, listeners[i].callback_params );
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

    EventCode event_code = event_listener_id >> 8;

    ListenerContext* listeners = REGISTRY->event_listeners[event_code].listeners;
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

u32 query_event_subsystem_size() {
    return sizeof(ListenerRegistry);
}
b32 event_init( void* event_subsystem_buffer ) {
    REGISTRY = (ListenerRegistry*)event_subsystem_buffer;

    for( u32 i = 0; i < MAX_EVENT_CODE; ++i ) {
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
void event_shutdown() {
    for( u32 i = 0; i < MAX_EVENT_CODE; ++i ) {
        _list_free( REGISTRY->event_listeners[i].listeners );
    }
    REGISTRY = NULL;
    LOG_INFO("Event subsystem shutdown.");
}
