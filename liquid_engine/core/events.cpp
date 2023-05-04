/**
 * Description:  Events
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "events.h"
#include "collections.h"
#include "logging.h"
#include "memory.h"

#define MIN_LISTENERS 2

struct ListenerContext {
    EventListener listener;
    void*         params;

    b32 operator==(const ListenerContext& other) {
        return
            (usize)this->listener == (usize)other.listener &&
            (usize)this->params   == (usize)other.params;
    }
    b32 operator!=(const ListenerContext& other) {
        return !(this->operator==(other));
    }
};

struct ListenerRegistry {
    // list
    ListenerContext* listeners = nullptr;
};

struct EventSystem {
    ListenerRegistry* registry = nullptr;
};

SM_GLOBAL EventSystem SYSTEM = {};

b32 event_init() {
    void* registry_buffer = mem_alloc(
        sizeof(ListenerRegistry) * MAX_EVENT_CODE,
        MEMTYPE_EVENT_LISTENER_REGISTRY
    );
    if( !registry_buffer ) {
        LOG_FATAL("Failed to allocate listener registry!");
        return false;
    }
    SYSTEM.registry = (ListenerRegistry*)registry_buffer;

    LOG_NOTE("Event subsystem successfully initialized.");
    return true;
}

b32 event_shutdown() {
    for( usize i = 0; i < MAX_EVENT_CODE; ++i ) {
        ListenerRegistry* registry = &SYSTEM.registry[i];
        if( registry->listeners ) {
            list_free( registry->listeners );
        }
    }
    mem_free( SYSTEM.registry );
    LOG_NOTE("Event subsystem successfully shut down.");
    return true;
}

void event_fire( Event event ) {
    LOG_ASSERT(
        event.code < MAX_EVENT_CODE,
        "Event code is invalid! Exceeded max event code!"
    );
    ListenerRegistry* registry = &SYSTEM.registry[event.code];

    if( !registry->listeners ) {
        return;
    }

    usize listener_count = list_count( registry->listeners );
    for( usize i = 0; i < listener_count; ++i ) {
        ListenerContext* current = &registry->listeners[i];
        // check if event has been handled
        if( current->listener( &event, current->params ) ) {
            return;
        }
    }

}

b32 event_subscribe(
    EventCode     code,
    EventListener listener,
    void*         listener_params
) {
    LOG_ASSERT(
        code < MAX_EVENT_CODE,
        "Event code is invalid! Exceeded max event code!"
    );
    ListenerRegistry* registry = &SYSTEM.registry[code];

    if( !registry->listeners ) {
        registry->listeners = list_reserve(
            ListenerContext,
            MIN_LISTENERS
        );

        if( !registry->listeners ) {
            return false;
        }
    }

    ListenerContext context = {
        listener,
        listener_params
    };
    usize listener_count = list_count(registry->listeners);
    for( usize i = 0; i < listener_count; ++i ) {
        ListenerContext* current = &registry->listeners[i];
        if( context == *current ) {
            LOG_ERROR(
                "Attempted to subscribe listener "
                "that is already subscribed!"
            );
            return false;
        }
    }

    list_push( registry->listeners, context );

    return true;
}

b32 event_unsubscribe(
    EventCode     code,
    EventListener listener,
    void*         listener_params
) {
    LOG_ASSERT(
        code < MAX_EVENT_CODE,
        "Event code is invalid! Exceeded max event code!"
    );
    ListenerRegistry* registry = &SYSTEM.registry[code];
    LOG_ASSERT(
        registry->listeners,
        "Listener registry for code 0x%X does not exist!",
        (u32)code
    );
    
    b32   listener_found = false;
    usize listener_index = 0;

    ListenerContext context = {
        listener,
        listener_params
    };

    usize listener_count = list_count( registry->listeners );
    if( !listener_count ) {
        LOG_ERROR(
            "Attempted to unsubscribe from event "
            "which has no listeners!"
        );
        return false;
    }
    for( usize i = 0; i < listener_count; ++i ) {
        ListenerContext* current = &registry->listeners[i];
        if( context == *current ) {
            listener_found = true;
            listener_index = i;
            break;
        }
    }

    if( listener_found ) {
        list_remove(
            registry->listeners,
            listener_index,
            nullptr
        );
        return true;
    } else {
        LOG_ERROR(
            "Attempted to unsubscribe a listener "
            "that was never subscribed! "
            "Event Code: 0x%X",
            (u32)code
        );
        return false;
    }

}
