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
#include "core/engine.h"
#include "core/string.h"
#include "flags.h"

#define MAX_PLATFORM_SURFACE_TITLE_SIZE 255

/// Platform state
struct Platform {
    union Surface {
        ivec2  dimensions;
        struct { i32 width, height; };
    } surface;
    b32 is_active;
};
/// Returns how many bytes the current platform requires.
u32 query_platform_subsystem_size();
/// Initialize platform state. Returns true if successful.
b32 platform_init(
    StringView opt_icon_path,
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
/// StringView MUST have a null-terminator
/// Does nothing on platforms that don't use windows.
void platform_surface_set_name(
    Platform* platform,
    StringView name
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
/// Platform exit program.
void platform_exit();

/// Initialize Audio backend.
b32 platform_init_audio( Platform* platform );
/// Shutdown Audio backend.
void platform_shutdown_audio( Platform* platform );

void platform_audio_test( Platform* platform, i16 volume );

/// Get stdout handle.
void* platform_stdout_handle();
/// Get stderr handle.
void* platform_stderr_handle();

/// Write a null-terminated string to the console.
/// Get console handle using platform_stdout_handle() or
/// platform_stderr_handle().
void platform_write_console(
    void* output_handle,
    u32 write_count,
    const char* buffer
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

/// User selection from a message box or an error
/// from creating message box.
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
MessageBoxResult message_box(
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
void* platform_page_alloc( usize size );
/// Free page allocated memory.
void platform_page_free( void* memory );

#define CPU_NAME_BUFFER_SIZE 68
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
struct SystemInfo {
    usize logical_processor_count;
    usize total_memory;
    char  cpu_name_buffer[CPU_NAME_BUFFER_SIZE];
    ProcessorFeatures features;
};
/// Query CPU and memory information.
struct SystemInfo query_system_info();

#endif
