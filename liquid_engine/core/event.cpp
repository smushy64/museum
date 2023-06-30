// * Description:  Event Subsystem Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "event.h"
#include "collections.h"

#define MIN_LISTENERS 2

struct ListenerContext {
    EventCallbackFN callback;
    void*           callback_params;

    u16 id;

    b32 operator==(const ListenerContext& other) {
        return id == other.id;
    }
    b32 operator!=(const ListenerContext& other) {
        return !operator==(other);
    }
};

struct ListenerRegistry {
    struct {
        ListenerContext* listeners;
    } event_listeners[MAX_EVENT_CODE];
};

global ListenerRegistry* REGISTRY = nullptr;
global EventListenerID   ID = 1;

LD_API void event_fire( Event event, EventPriority priority ) {
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
    LD_ASSERT( REGISTRY );

    EventListenerID result = (event_code << 16) | ID;
    ID++;

    ListenerContext* listeners = REGISTRY->event_listeners[event_code].listeners;
    
    ListenerContext ctx = {};
    ctx.id              = result;
    ctx.callback        = callback;
    ctx.callback_params = callback_params;

    listeners = (ListenerContext*)::impl::_list_push( listeners, &ctx );

    return result;
}
LD_API void event_unsubscribe( EventListenerID event_listener_id ) {
    LD_ASSERT( REGISTRY );

    EventCode event_code = event_listener_id >> 16;
    u16 listener_id      = event_listener_id & 0x0000FFFF;

    ListenerContext* listeners = REGISTRY->event_listeners[event_code].listeners;
    u32 listener_count = list_count( listeners );

    i32 listener_index = -1;

    for( u32 i = 0; i < listener_count; ++i ) {
        if( listener_id == listeners[i].id ) {
            listener_index = i;
            break;
        }
    }

    if( listener_index < 0 ) {
        LOG_ERROR("Could not find event listener {u}!", event_listener_id);
        return;
    }

    listeners = (ListenerContext*)list_remove(
        listeners,
        listener_index,
        nullptr
    );

}

u32 query_event_subsystem_size() {
    return sizeof(ListenerRegistry);
}
b32 event_init( void* event_subsystem_buffer ) {
    REGISTRY = (ListenerRegistry*)event_subsystem_buffer;

    for( u32 i = 0; i < MAX_EVENT_CODE; ++i ) {
        void* listener_list = ::impl::_list_create(
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
        ::impl::_list_free( REGISTRY->event_listeners[i].listeners );
    }
    REGISTRY = nullptr;
    LOG_INFO("Event subsystem shutdown.");
}
