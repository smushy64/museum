#if !defined(CORE_EVENT_HPP)
#define CORE_EVENT_HPP
// * Description:  Event Subsystem
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 24, 2023
#include "defines.h"
#include "core/ldlog.h"
#include "core/ldmath/types.h"

/// Events can only have this many listeners subscribed to it.
#define EVENT_LISTENER_MAX (10ull)

/// Event Callback function return codes.
typedef enum EventCallbackResult : b32 {
    /// Other listeners will also receive this event.
    EVENT_CALLBACK_NOT_CONSUMED,
    /// Othere listeners will not receive this event.
    EVENT_CALLBACK_CONSUMED,
} EventCallbackResult;

struct Event;

/// Event Callback function.
typedef EventCallbackResult EventCallbackFN(
    struct Event* event, void* params
);

/// Event Code.
/// User defined event codes must be greater than MAX_ENGINE_EVENT_CODE 
/// and less than MAX_EVENT_CODE.
typedef u8 EventCode;
/// Invalid event code.
/// Any event code with this value will fail an assertion and crash the program.
#define EVENT_CODE_INVALID ((EventCode)(0))
/// Exit code.
/// Triggers engine shutdown.
/// Contains no data.
#define EVENT_CODE_EXIT ((EventCode)(1))
/// Application Active code.
/// Tells engine when application is active/inactive.
/// Contains struct EventAppActive.
#define EVENT_CODE_APP_ACTIVE ((EventCode)(2))
/// Surface Resize code.
/// Tells engine when surface is resized.
/// Contains struct EventResize.
#define EVENT_CODE_SURFACE_RESIZE ((EventCode)(3))
/// Gamepad Active code.
/// Tells engine when a gamepad is active/inactive.
/// Contains struct EventGamepadActive.
#define EVENT_CODE_GAMEPAD_ACTIVE ((EventCode)(4))
/// Max Engine Event Code.
/// This is the maximum event code that the engine is allowed to use.
/// Any codes greater than this are user event codes.
#define EVENT_CODE_MAX_ENGINE_CODE ((EventCode)(5))
/// Max Event Code.
/// This is the maximum event code that is allowed.
#define EVENT_CODE_MAX ((EventCode)(128))

/// Event.
/// EventCode tells you what type of event it is.
/// EventData is any data that goes along with this type of event.
typedef struct Event {
    EventCode code;
    union EventData {
        pvoid pointer[2];

        mat2 m2;

        vec4  v4;
        ivec4 iv4;

        vec3  v3;
        ivec3 iv3;

        i64 int64[2];
        u64 uint64[2];
        f64 float64[2];

        vec2  v2[2];
        ivec2 iv2[2];

        i32 int32[4];
        u32 uint32[4];
        f32 float32[4];
        b32 bool32[4];

        i16 int16[8];
        u16 uint16[8];

        i8 int8[16];
        u8 uint8[16];
        b8 bool8[16];

        char char8[16];

        struct EventAppActive {
            b32 active;
        } app_active;

        struct EventResize {
            ivec2 new_dimensions;
        } resize;

        struct EventGamepadActive {
            u32 index;
            b32 active;
        } gamepad_active;

    } data;
} Event;

/// Event Priority.
/// Tells the event subsystem when callbacks should be run.
typedef enum EventPriority : u32 {
    /// Immediate events are always run on the main thread.
    EVENT_PRIORITY_IMMEDIATE,
    /// Delayed events are always run on worker threads.
    EVENT_PRIORITY_CONCURRENT,
    /// End of frame events run only at the end of frame.
    EVENT_PRIORITY_END_OF_FRAME,
} EventPriority;

/// Fire an event. All callbacks bound to this event will be called.
/// Priority determines if callbacks will be called immediately or delayed.
LD_API void event_fire_priority( Event event, EventPriority priority );
/// Fire an event. All callbacks bound to this event will be called immediately.
headerfn void event_fire( Event event ) {
    event_fire_priority( event, EVENT_PRIORITY_IMMEDIATE );
}

/// ID for referring to an event listener.
/// This value should be cached in order to unsubscribe unless
/// the listener lives until the end of the program.
typedef u16 EventListenerID;
/// Event Listener Invalid ID.
/// If event_subscribe returns this id,
/// it is a fatal error and the program should shutdown.
#define EVENT_LISTENER_INVALID_ID ((EventListenerID)(0))

/// Subscribe a listener callback to an event.
/// Returns an id that must be cached in order to unsubscribe.
LD_API EventListenerID event_subscribe(
    EventCode        event_code,
    EventCallbackFN* callback,
    void*            callback_params
);
/// Unsubscribe listener from event.
LD_API void event_unsubscribe( EventListenerID id );

#if defined(LD_API_INTERNAL)

    /// Size of event subsystem.
    usize event_subsystem_query_size();
    /// Initialize Event Subsystem.
    b32 event_subsystem_init( void* event_subsystem_buffer );
    /// Fire end of frame events.
    void event_fire_end_of_frame();

#endif

#endif // header guard
