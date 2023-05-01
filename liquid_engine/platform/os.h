#if !defined(PLATFORM_HPP)
#define PLATFORM_HPP
/**
 * Description:  Operating System related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#include "core/smath.h"
#include "core/input.h"

typedef u32 PlatformInitFlags;

#define PLATFORM_INIT_DPI_AWARE ( 1 << 0 )

typedef void* PlatformState;

SM_API b32 platform_init(
    PlatformInitFlags flags,
    PlatformState* out_state
);
SM_API void platform_shutdown(
    PlatformState* state
);

typedef void* SurfaceHandle;
struct Surface {
    SurfaceHandle handle;
    union {
        ivec2 position;
        struct { i32 x; i32 y; };
    };
    union {
        ivec2 dimensions;
        struct { i32 width; i32 height; };
    };
    b32 is_focused;
    void* platform_data;
};

typedef u32 SurfaceCreateFlags;
#define SURFACE_FLAG_SHOW_ON_CREATE    ( 1 << 1 )
#define SURFACE_FLAG_POSITION_CENTERED ( 1 << 2 )

SM_API b32 surface_create(
    const char*        surface_name,
    ivec2              dimensions,
    SurfaceCreateFlags flags,
    SurfaceHandle      opt_parent_surface,
    Surface*           out_surface
);

/// Types of message boxes
enum MessageBoxType : u32 {
    MBTYPE_OK,
    MBTYPE_OKCANCEL,
    MBTYPE_RETRYCANCEL,
    MBTYPE_YESNO,
    MBTYPE_YESNOCANCEL,

    MBTYPE_COUNT
};
inline const char* to_string( MessageBoxType type ) {
    static const char* strings[MBTYPE_COUNT] = {
        "Message Box with OK button.",
        "Message Box with OK and CANCEL buttons.",
        "Message Box with RETRY and CANCEL buttons.",
        "Message Box with YES and NO buttons.",
        "Message Box with YES, NO and CANCEL buttons.",
    };
    if( type >= MBTYPE_COUNT ) {
        return "Unknown Message Box type.";
    }
    return strings[type];
}

/// Message box icons
enum MessageBoxIcon : u32 {
    MBICON_INFORMATION,
    MBICON_WARNING,
    MBICON_ERROR,

    MBICON_COUNT
};
inline const char* to_string( MessageBoxIcon icon ) {
    static const char* strings[MBICON_COUNT] = {
        "Message Box \"information\" icon.",
        "Message Box \"warning\" icon.",
        "Message Box \"error\" icon.",
    };
    if( icon >= MBICON_COUNT ) {
        return "Unknown Message Box icon.";
    }
    return strings[icon];
}

/// User selection from a message box or an error from creating message box.
enum MessageBoxResult : u32 {
    MBRESULT_OK,
    MBRESULT_CANCEL,
    MBRESULT_RETRY,
    MBRESULT_YES,
    MBRESULT_NO,

    MBRESULT_UNKNOWN_ERROR,

    MBRESULT_COUNT
};
inline const char* to_string( MessageBoxResult result ) {
    static const char* strings[MBRESULT_COUNT] = {
        "Message Box OK selected.",
        "Message Box CANCEL selected.",
        "Message Box RETRY selected.",
        "Message Box YES selected.",
        "Message Box NO selected.",
        "An unknown error occurred.",
    };
    if( result >= MBRESULT_COUNT ) {
        return "Unknown Message Box result.";
    }
    return strings[result];
}

/// Create a message box to report urgent information.
SM_API MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
);

#define MESSAGE_BOX_FATAL( title, message ) \
    message_box(\
        title,\
        message,\
        MBTYPE_OK,\
        MBICON_ERROR\
    )

/// Put the current thread to sleep for the specified amount of milliseconds.
SM_API void sleep( u32 ms );

typedef u16 ProcessorFeatures;

#define SSE_MASK    (1 << 0)
#define SSE2_MASK   (1 << 1)
#define SSE3_MASK   (1 << 2)
#define SSSE3_MASK  (1 << 3)
#define SSE4_1_MASK (1 << 4)
#define SSE4_2_MASK (1 << 5)
#define AVX_MASK    (1 << 6)
#define AVX2_MASK   (1 << 7)
#define AVX512_MASK (1 << 8)

#define IS_SSE_AVAILABLE(processor_features)\
    ( (processor_features & SSE_MASK) == SSE_MASK )
#define IS_SSE2_AVAILABLE(processor_features)\
    ( (processor_features & SSE2_MASK) == SSE2_MASK )
#define IS_SSE3_AVAILABLE(processor_features)\
    ( (processor_features & SSE3_MASK) == SSE3_MASK )
#define IS_SSSE3_AVAILABLE(processor_features)\
    ( (processor_features & SSSE3_MASK) == SSSE3_MASK )
#define IS_SSE4_1_AVAILABLE(processor_features)\
    ( (processor_features & SSE4_1_MASK) == SSE4_1_MASK )
#define IS_SSE4_2_AVAILABLE(processor_features)\
    ( (processor_features & SSE4_2_MASK) == SSE4_2_MASK )

#define ARE_SSE_INSTRUCTIONS_AVAILABLE(processor_features) (\
    IS_SSE_AVAILABLE(processor_features)    &&\
    IS_SSE2_AVAILABLE(processor_features)   &&\
    IS_SSE3_AVAILABLE(processor_features)   &&\
    IS_SSSE3_AVAILABLE(processor_features)  &&\
    IS_SSE4_1_AVAILABLE(processor_features) &&\
    IS_SSE4_2_AVAILABLE(processor_features)   \
)

#define IS_AVX_AVAILABLE(processor_features)\
    ( (processor_features & AVX_MASK) == AVX_MASK )
#define IS_AVX2_AVAILABLE(processor_features)\
    ( (processor_features & AVX2_MASK) == AVX2_MASK )
#define IS_AVX512_AVAILABLE(processor_features)\
    ( (processor_features & AVX512_MASK) == AVX512_MASK )

#if defined(SM_ARCH_X86)
    #define CPU_NAME_BUFFER_LEN 68
#else
    #error "Architecture is not yet supported!"
#endif

/// System information
struct SystemInfo {
    usize thread_count;
    usize total_memory;
    char  cpu_name_buffer[CPU_NAME_BUFFER_LEN];
    ProcessorFeatures features;
};
/// Get information about the current processor.
SM_API SystemInfo query_system_info();

/// Platform events.
enum EventType : u32 {
    EVENT_SURFACE_FOCUS_CHANGE,
    EVENT_SURFACE_DESTROY,
    EVENT_SURFACE_DIMENSIONS_CHANGE,
    EVENT_SURFACE_POSITION_CHANGE,

    EVENT_KEY_PRESS,

    EVENT_MOUSE_BUTTON_PRESS,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_WHEEL,

    EVENT_TYPE_COUNT
};
inline const char* to_string( EventType event_type ) {
    static const char* strings[EVENT_TYPE_COUNT] = {
        "Surface Focus Change",
        "Surface Destroy",
        "Surface Dimensions Change",
        "Surface Position Change",
        "Key Press",
        "Mouse Button Press",
        "Mouse Moved",
        "Mouse Wheel",
    };
    if( event_type >= EVENT_TYPE_COUNT ) {
        return "Unknown";
    }
    return strings[event_type];
}

/// OS events
struct Event {
    EventType type;
    union {
        struct {
            b32 is_focused;
        } focus_change;
        union {
            union {
                ivec2  dimensions;
                struct { i32 width; i32 height; };
            } dimensions_change;
            union {
                ivec2 position;
                struct { i32 x; i32 y; };
            } position_change;
            union {
                ivec2 position;
                struct { i32 x; i32 y; };
            } mouse_move;
            ivec2 int2;
        };
        struct {
            KeyCode keycode;
            b8      is_down;
        } key_press;
        struct {
            MouseCode mousecode;
            b8        is_down;
        } mouse_button_press;
        struct {
            i32 delta;
            b8  is_horizontal;
        } mouse_wheel;
    };
};

/// Format a string buffer with event information
SM_API isize format_event(
    usize buffer_size,
    char* buffer,
    Event* event
);

/// Pop event from event queue.
/// Returns true if there was a pending event in the queue.
SM_API b32 next_event( SurfaceHandle surface, Event* event );

#endif