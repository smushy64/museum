#if !defined(PLATFORM_HPP)
#define PLATFORM_HPP
/**
 * Description:  Operating System related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#include "core/math/types.h"
#include "core/input.h"
#include "flags.h"

inline const char* DEFAULT_SURFACE_NAME = "Surface";
#define DEFAULT_SURFACE_NAME_LENGTH 7

#define DEFAULT_SURFACE_WIDTH  800
#define DEFAULT_SURFACE_HEIGHT 600
/// Platform independent drawing surface
struct Surface {
    union {
        ivec2 dimensions;
        struct { i32 width, height; };
    };
    void* platform;
};
/// Supported cursor styles
enum CursorStyle : u32 {
    CURSOR_ARROW,
    CURSOR_RESIZE_VERTICAL,
    CURSOR_RESIZE_HORIZONTAL,
    CURSOR_RESIZE_TOP_RIGHT_BOTTOM_LEFT,
    CURSOR_RESIZE_TOP_LEFT_BOTTOM_RIGHT,
    CURSOR_BEAM,
    CURSOR_CLICK,
    CURSOR_WAIT,
    CURSOR_FORBIDDEN,

    CURSOR_COUNT
};
inline const char* to_string( CursorStyle cursor_style ) {
    const char* strings[CURSOR_COUNT] = {
        "Arrow",
        "Resize Vertical",
        "Resize Horizontal",
        "Resize Top Right Bottom Left",
        "Resize Top Left Bottom Right",
        "Beam",
        "Click",
        "Wait",
        "Forbidden",
    };
    if( cursor_style >= CURSOR_COUNT ) {
        return "Unknown";
    }
    return strings[cursor_style];
}
/// Platform state
struct Platform {
    Surface surface;
    void*   platform;

    b32 is_active;
};

typedef u32 PlatformFlags;
#define PLATFORM_DPI_AWARE ( 1 << 0 )

/// Initialize platform state. Returns true if successful.
b32 platform_init(
    const char* opt_surface_name,
    ivec2 surface_dimensions,
    PlatformFlags flags,
    Platform* out_platform
);
/// Shutdown platform subsystem.
void platform_shutdown( Platform* platform );
/// Read absolute time since program start.
u64 platform_read_absolute_time( Platform* platform );
/// Read seconds elapsed since program start.
f64 platform_read_seconds_elapsed( Platform* platform );
/// Pump platform events.
b32 platform_pump_events( Platform* platform );
/// Set platform surface name.
/// Does nothing on platforms that don't use windows.
void platform_surface_set_name(
    Platform* platform,
    usize name_length,
    const char* name
);
/// Read platform surface name.
/// Returns:
///          >Zero: required buffer size. writes up to buffer size.
///          Zero:  success.
i32 platform_surface_read_name(
    Platform* platform,
    char* buffer, usize max_buffer_size
);
/// Set cursor style.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_set_style(
    Platform* platform, CursorStyle cursor_style
);
/// Set cursor visibility.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_set_visible( Platform* platform, b32 visible );
/// Lock cursor to surface.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_set_locked( Platform* platform, b32 locked );
/// Set cursor position to surface center.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_center( Platform* platform );
/// Put the current thread to sleep
/// for the specified amount of milliseconds.
void platform_sleep( Platform* platform, u32 ms );
/// Set given gamepad motor state.
void platform_set_pad_motor_state(
    Platform* platform,
    u32 gamepad_index, u32 motor, f32 value
);
/// Poll gamepad.
void platform_poll_gamepad( Platform* platform );
/// Create a vulkan surface.
struct VkSurfaceKHR_T* platform_vk_create_surface(
    Platform* platform,
    struct VulkanRendererContext* ctx
);
/// Get required vulkan extension names.
usize platform_vk_read_ext_names(
    Platform* platform,
    usize max_names,
    usize* name_count,
    const char** names
);
/// Swap buffers. OpenGL only.
void platform_gl_swap_buffers( Platform* platform );
/// Initialize OpenGL.
void* platform_gl_init( Platform* platform );
/// Shutdown OpenGL.
void platform_gl_shutdown( Platform* platform, void* glrc );

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

#if defined(SM_API_INTERNAL)

/// Allocate memory on the heap.
/// All platforms must zero out memory before returning pointer.
void* heap_alloc( usize size );
/// Reallocate memory on the heap.
/// All platforms must zero out new memory before returning pointer.
void* heap_realloc( void* memory, usize new_size );
/// Free heap allocated memory.
void heap_free( void* memory );

/// Page allocate memory.
/// All platforms must zero out memory before returning pointer.
void* page_alloc( usize size );
/// Free page allocated memory.
void page_free( void* memory );

#endif // internal

#endif