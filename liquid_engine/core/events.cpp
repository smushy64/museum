/**
 * Description:  Events
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 01, 2023
*/
#include "events.h"
#include "collections.h"
#include "logging.h"
#include "memory.h"

struct CallbackContext {
    EventCode       callback_event;
    EventCallbackFN callback_function;
    void*           callback_params;

    b32 operator==(const CallbackContext& other) {
        return
            (usize)this->callback_function == (usize)other.callback_function &&
            (usize)this->callback_params   == (usize)other.callback_params;
    }
    b32 operator!=(const CallbackContext& other) {
        return !operator==( other );
    }
};

struct CallbackRegistry {
    struct EventCallbackList {
        CallbackContext callbacks[MAX_EVENT_SUBSCRIPTIONS];
        u32 listener_count;
    } lists[MAX_EVENT_CODE];
};
inline internal b32 callback_registry_push(
    CallbackRegistry* registry,
    CallbackContext callback_context
) {
    if( callback_context.callback_event >= MAX_EVENT_CODE ) {
        LOG_WARN(
            "Event code %u is an invalid event code!",
            callback_context.callback_event
        );
        return false;
    }

    CallbackRegistry::EventCallbackList* callback_list =
        (CallbackRegistry::EventCallbackList*)&registry->
            lists[callback_context.callback_event];
    if( callback_list->listener_count == MAX_EVENT_SUBSCRIPTIONS ) {
        LOG_WARN(
            "Event \"%s\" cannot take anymore listeners!",
            engine_event_code_to_string( callback_context.callback_event )
        );
        return false;
    }

    callback_list->callbacks[callback_list->listener_count++] =
        callback_context;

    return true;
}
inline internal b32 callback_registry_remove(
    CallbackRegistry* registry,
    CallbackContext callback_context
) {
    if( callback_context.callback_event >= MAX_EVENT_CODE ) {
        LOG_WARN(
            "Event code %u is an invalid event code!",
            callback_context.callback_event
        );
        return false;
    }
    // find callback
    // if callback is last item, only decrease total count
    // if callback is in the middle, copy the next callback and rest of list back by one
    //     and decrease total count
    i32 callback_index = -1;
    CallbackRegistry::EventCallbackList* callback_list =
        (CallbackRegistry::EventCallbackList*)&registry->
            lists[callback_context.callback_event];

    for( u32 i = 0; i < callback_list->listener_count; ++i ) {
        if( callback_list->callbacks[i] == callback_context ) {
            callback_index = i;
        }
    }

    if( callback_index < 0 ) {
        LOG_ERROR(
            "Attempted to unsubscribe a listener "
            "that was never subscribed! "
            "Event Code: 0x%X \"%s\"",
            (u32)callback_context.callback_event,
            engine_event_code_to_string(callback_context.callback_event)
        );
        return false;
    }

    callback_list->listener_count--;

    if( (u32)callback_index == callback_list->listener_count ) {
        return true;
    }

    void* dst  = callback_list->callbacks + callback_index;
    void* src  = (void*)((usize)dst + 1);
    usize size = sizeof(CallbackContext) *
        (usize)(callback_list->callbacks - callback_index);
    mem_copy( dst, src, size );

    return true;
}

global CallbackRegistry* REGISTRY = nullptr;

b32 event_init() {
    void* registry_buffer = mem_alloc(
        sizeof(CallbackRegistry),
        MEMTYPE_EVENT_LISTENER_REGISTRY
    );
    if( !registry_buffer ) {
        LOG_FATAL("Failed to allocate listener registry!");
        return false;
    }
    REGISTRY = (CallbackRegistry*)registry_buffer;

    LOG_INFO("Event subsystem successfully initialized.");
    return true;
}
void event_shutdown() {
    mem_free( REGISTRY );
    LOG_INFO("Event subsystem successfully shut down.");
}

void event_fire( Event event ) {

    if( !REGISTRY ) {
        return;
    }

    LOG_ASSERT( event.code < MAX_EVENT_CODE, "Invalid event code %u.", event.code );

    CallbackRegistry::EventCallbackList* list = &REGISTRY->lists[event.code];
    for( u32 i = 0; i < list->listener_count; ++i ) {
        CallbackContext* current = &list->callbacks[i];
        EventCallbackReturnCode return_code =
            current->callback_function( &event, current->callback_params );
        if( return_code == EVENT_CALLBACK_CONSUMED ) {
            return;
        }
    }

}
b32 event_subscribe(
    EventCode       event,
    EventCallbackFN callback_function,
    void*           callback_params
) {
    CallbackContext ctx   = {};
    ctx.callback_event    = event;
    ctx.callback_function = callback_function;
    ctx.callback_params   = callback_params;
    if( !callback_registry_push( REGISTRY, ctx ) ) {
        return false;
    }

    if( event < EVENT_CODE_LAST_RESERVED ) {
        const char* event_name = engine_event_code_to_string( event );
        LOG_NOTE( "Subscribed to %s!", event_name );
    } else {
        LOG_NOTE( "Subscribed to user event!" );
    }

    return true;
}

b32 event_unsubscribe(
    EventCode       event,
    EventCallbackFN callback_function,
    void*           callback_params
) {
    CallbackContext ctx   = {};
    ctx.callback_event    = event;
    ctx.callback_function = callback_function;
    ctx.callback_params   = callback_params;
    if( !callback_registry_remove( REGISTRY, ctx ) ) {
        return false;
    }

    if( event < EVENT_CODE_LAST_RESERVED ) {
        const char* event_name = engine_event_code_to_string( event );
        LOG_NOTE( "Unsubscribed to %s!", event_name );
    } else {
        LOG_NOTE( "Unsubscribed to user event!" );
    }
    
    return true;
}
i32 event_available_listener_count( EventCode code ) {
    if( code >= MAX_EVENT_CODE ) {
        LOG_WARN(
            "Event code %u is an invalid event code!",
            code
        );
        return false;
    }

    return MAX_EVENT_SUBSCRIPTIONS - REGISTRY->lists[code].listener_count;
}
