#if !defined(CORE_EVENT_HPP)
#define CORE_EVENT_HPP
// * Description:  Event Subsystem
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "defines.h"
#include "core/logging.h"

/// Event Callback function return codes.
enum EventCallbackReturn : b32 {
    EVENT_CALLBACK_NOT_CONSUMED,
    EVENT_CALLBACK_CONSUMED,
};

/// Event Callback function.
typedef EventCallbackReturn (*EventCallbackFN)( struct Event* event, void* params );

/// Event Code.
/// User defined event codes must be greater than MAX_ENGINE_EVENT_CODE 
/// and less than MAX_EVENT_CODE.
typedef u32 EventCode;
/// Invalid event code.
/// Any event code with this value will fail an assertion and crash the program.
global const EventCode EVENT_CODE_INVALID = 0;
/// Exit code.
/// Triggers engine shutdown.
/// Contains no data.
global const EventCode EVENT_CODE_EXIT = 1;
/// Application Active code.
/// Tells engine when application is active/inactive.
/// Contains 1 bool32 which is true when app is active, false when it isn't.
global const EventCode EVENT_CODE_ACTIVE = 2;
/// Surface Resize code.
/// Tells engine when surface is resized.
/// Contains 2 int32 which correspond to the new width and height of the surface.
global const EventCode EVENT_CODE_RESIZE = 3;
/// Gamepad Active code.
/// Tells engine when a gamepad is active/inactive.
/// Contains 1 uint32[0] for the index of the gamepad being talked about and
/// 1 bool32[1] for whether or not the gamepad is now active or inactive.
global const EventCode EVENT_CODE_GAMEPAD_ACTIVE = 4;
/// Max Engine Event Code.
/// This is the maximum event code that the engine is allowed to use.
/// Any codes greater than this are user event codes.
global const EventCode MAX_ENGINE_EVENT_CODE = 5;
/// Max Event Code.
/// This is the maximum event code that is allowed.
/// Any event code that goes above this value will fail an assertion and crash
/// the program.
global const EventCode MAX_EVENT_CODE = 255;

/// Event.
/// EventCode tells you what type of event it is.
/// EventData is any data that goes along with this type of event.
struct Event {
    EventCode code;
    union EventData {
        pvoid pointer[2];

        i64 int64[2];
        u64 uint64[2];
        f64 float64[2];

        i32 int32[4];
        u32 uint32[4];
        f32 float32[4];
        b32 bool32[4];

        i16 int16[8];
        u16 uint16[8];

        i8 int8[16];
        u8 uint8[16];
        b8 bool8[16];

        char c[16];
    } data;
};

/// Event Priority.
/// Tells the event subsystem if listener callbacks should be called
/// immediately or if they can be delayed.
enum EventPriority : u32 {
    EVENT_PRIORITY_IMMEDIATE,
    EVENT_PRIORITY_DELAYED
};

/// Fire an event. All callbacks bound to this event will be called.
/// Priority determines if callbacks will be called immediately or delayed.
LD_API void event_fire( Event event, EventPriority priority );
/// Fire an event. All callbacks bound to this event will be called immediately.
inline void event_fire( Event event ) {
    event_fire( event, EVENT_PRIORITY_IMMEDIATE );
}

/// ID for referring to an event listener.
/// This value should be cached in order to unsubscribe unless
/// the listener lives until the end of the program.
typedef u16 EventListenerID;
/// Event Listener Invalid ID.
/// If event_subscribe returns this id, it is an error and the program should
/// shutdown.
global const EventListenerID EVENT_LISTENER_INVALID_ID = 0;

/// Subscribe a listener callback to an event.
/// Returns an id that must be cached in order to unsubscribe.
LD_API EventListenerID event_subscribe(
    EventCode       event_code,
    EventCallbackFN callback,
    void*           callback_params
);
/// Unsubscribe listener from event.
LD_API void event_unsubscribe( EventListenerID id );

#if defined(LD_API_INTERNAL)

    /// Size of event subsystem.
    u32 event_subsystem_size();
    /// Initialize Event Subsystem.
    b32 event_init( void* event_subsystem_buffer );
    /// Shutdown Event Subsystem.
    void event_shutdown();

#endif

#endif // header guard
