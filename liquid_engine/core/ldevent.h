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
        pvoid pointer[4];

        mat2 m2[2];

        vec4  v4[2];
        ivec4 iv4[2];

        vec3  v3[2];
        ivec3 iv3[2];

        i64 int64[4];
        u64 uint64[4];
        f64 float64[4];

        vec2  v2[4];
        ivec2 iv2[4];

        i32 int32[8];
        u32 uint32[8];
        f32 float32[8];
        b32 bool32[8];

        i16 int16[16];
        u16 uint16[16];

        i8 int8[32];
        u8 uint8[32];
        b8 bool8[32];

        char str[32];

        struct EventAppActive {
            b32 active;
        } app_active;

        struct EventResize {
            ivec2 surface_dimensions;
            ivec2 render_resolution;
            f32   render_resolution_scale;
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
header_only void event_fire( Event event ) {
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
    extern usize EVENT_SUBSYSTEM_SIZE;
    /// Initialize Event Subsystem.
    b32 event_subsystem_init( void* event_subsystem_buffer );
    /// Fire end of frame events.
    void event_fire_end_of_frame(void);

#endif

#endif // header guard
