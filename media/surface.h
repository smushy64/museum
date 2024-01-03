#if !defined(LD_MEDIA_SURFACE_H)
#define LD_MEDIA_SURFACE_H
/**
 * Description:  Surface API
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"
#include "media/input.h"

struct MediaSurface;
struct MediaSurfaceCallbackData;

/// Opaque Handle to a surface.
/// On Win32, this is HWND.
typedef void MediaSurfaceHandle;

/// Opaque (Platform) data size.
#define MEDIA_SURFACE_OPAQUE_DATA_SIZE (504)
/// Max surface name buffer capacity.
#define MEDIA_SURFACE_NAME_CAPACITY (255)

/// Media surface flags.
typedef u32 MediaSurfaceFlags;
#define MEDIA_SURFACE_FLAG_HIDDEN     (1 << 0)
#define MEDIA_SURFACE_FLAG_RESIZEABLE (1 << 1)
#define MEDIA_SURFACE_FLAG_FULLSCREEN (1 << 2)

/// Surface callback prototype.
typedef void MediaSurfaceCallbackFN(
    struct MediaSurface* surface, struct MediaSurfaceCallbackData* data, void* params );

/// Prototype for loading OpenGL functions.
typedef void* MediaOpenGLLoadFN( const char* function_name );

/// Media Surface.
typedef struct MediaSurface {
    MediaSurfaceHandle* handle;
    u8 opaque[MEDIA_SURFACE_OPAQUE_DATA_SIZE];
} MediaSurface;

/// Type of surface callback.
typedef enum MediaSurfaceCallbackType {
    MEDIA_SURFACE_CALLBACK_TYPE_CLOSE,
    MEDIA_SURFACE_CALLBACK_TYPE_ACTIVATE,
    MEDIA_SURFACE_CALLBACK_TYPE_RESOLUTION_CHANGE,
    MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_BUTTON,
    MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE,
    MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE_RELATIVE,
    MEDIA_SURFACE_CALLBACK_TYPE_KEYBOARD_KEY,
    MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_WHEEL,
} MediaSurfaceCallbackType;

/// Surface callback data.
typedef struct MediaSurfaceCallbackData {
    MediaSurfaceCallbackType type;
    union {
        struct {
            b32 is_active;
        } activate;
        struct {
            i32 old_width, old_height;
            i32 new_width, new_height;
        } resolution_change;
        struct {
            MediaMouseButton button;
            b8 is_down;
        } mouse_button;
        struct {
            MediaKeyboardKey key;
            b8 is_down;
        } key;
        struct {
            i32 x, y;
        } mouse_move;
        struct {
            i32 value;
            b8  is_horizontal;
        } mouse_wheel;
    };
} MediaSurfaceCallbackData;

/// Media surface graphics backends.
enum : u32 {
    MEDIA_SURFACE_GRAPHICS_BACKEND_OPENGL,
    MEDIA_SURFACE_GRAPHICS_BACKEND_VULKAN,
    MEDIA_SURFACE_GRAPHICS_BACKEND_DIRECTX11,
    MEDIA_SURFACE_GRAPHICS_BACKEND_DIRECTX12,
    MEDIA_SURFACE_GRAPHICS_BACKEND_WEBGL,
    MEDIA_SURFACE_GRAPHICS_BACKEND_METAL,
};
/// Media surface graphics backends.
typedef u32 MediaSurfaceGraphicsBackend;

/// Types of message boxes.
typedef enum MediaMessageBoxType: u32 {
    MEDIA_MESSAGE_BOX_TYPE_INFO,
    MEDIA_MESSAGE_BOX_TYPE_WARNING,
    MEDIA_MESSAGE_BOX_TYPE_ERROR
} MediaMessageBoxType;

/// Options to display in message box.
typedef enum MediaMessageBoxOptions : u32 {
    MEDIA_MESSAGE_BOX_OPTIONS_OK,
    MEDIA_MESSAGE_BOX_OPTIONS_OK_CANCEL,
    MEDIA_MESSAGE_BOX_OPTIONS_YES_NO,
} MediaMessageBoxOptions;

/// What user selected in message box or error if
/// failed to create message box.
typedef enum MediaMessageBoxResult : u32 {
    MEDIA_MESSAGE_BOX_RESULT_ERROR,
    MEDIA_MESSAGE_BOX_RESULT_OK,
    MEDIA_MESSAGE_BOX_RESULT_CANCEL,
    MEDIA_MESSAGE_BOX_RESULT_YES,
    MEDIA_MESSAGE_BOX_RESULT_NO
} MediaMessageBoxResult;

/// Create a media surface.
MEDIA_API b32 media_surface_create(
    i32 width, i32 height, u32 name_len, const char* name, MediaSurfaceFlags flags,
    MediaSurfaceCallbackFN* opt_callback, void* opt_callback_params,
    MediaSurfaceGraphicsBackend backend, MediaSurface* out_surface );
/// Destroy a media surface.
MEDIA_API void media_surface_destroy( MediaSurface* surface );
/// Set callback function.
MEDIA_API void media_surface_set_callback(
    MediaSurface* surface, MediaSurfaceCallbackFN* callback, void* params );
/// Clear callback function.
MEDIA_API void media_surface_clear_callback( MediaSurface* surface );
/// Set media surface name.
/// Max len is SURFACE_NAME_CAPACITY.
/// If provided name is longer, name is truncated.
MEDIA_API void media_surface_set_name(
    MediaSurface* surface, const char* name, u32 len );
/// Query media surface name.
/// Copies up to buffer size.
MEDIA_API void media_surface_query_name(
    MediaSurface* surface, char* buffer, u32 buffer_size );
/// Set media surface dimensions.
/// Calls callback function after setting.
MEDIA_API void media_surface_set_dimensions(
    MediaSurface* surface, i32 new_width, i32 new_height );
/// Query media surface dimensions.
MEDIA_API void media_surface_query_dimensions(
    MediaSurface* surface, i32* out_width, i32* out_height );
/// Set media surface fullscreen.
MEDIA_API void media_surface_set_fullscreen(
    MediaSurface* surface, b32 is_fullscreen );
/// Query if media surface is fullscreen.
MEDIA_API b32 media_surface_query_fullscreen( MediaSurface* surface );
/// Hide media surface.
MEDIA_API void media_surface_set_hidden( MediaSurface* surface, b32 is_hidden );
/// Query if media surface is hidden.
MEDIA_API b32 media_surface_query_hidden( MediaSurface* surface );
/// Move media surface to center of screen.
/// Does nothing on platforms that do not support moving the media surface.
MEDIA_API void media_surface_center( MediaSurface* surface );
/// Move cursor to center of media surface.
/// Does nothing on platforms that do not support cursors.
MEDIA_API void media_surface_cursor_center( MediaSurface* surface );
/// Pump platform events for surface.
/// On some platforms, this pumps events for all surfaces.
MEDIA_API void media_surface_pump_events( MediaSurface* surface );

/// Initialize OpenGL for drawing to media surface.
MEDIA_API b32 media_surface_gl_init( MediaSurface* surface );
/// Call platform's version of glSwapBuffers.
/// Does nothing if OpenGL is not initialized.
MEDIA_API void media_surface_gl_swap_buffers( MediaSurface* surface );
/// Call platform's version of glSwapInterval.
/// Does nothing if OpenGL is not initialized.
MEDIA_API void media_surface_gl_swap_interval(
    MediaSurfaceHandle* surface_handle, int interval );
/// Function for loading OpenGL functions.
MEDIA_API void* media_gl_load_proc( const char* function_name );

/// Create a message box.
/// Blocks calling thread until user dismisses message box.
/// Strings provided MUST be null-terminated.
MEDIA_API MediaMessageBoxResult media_message_box_blocking(
    const char* title, const char* message,
    MediaMessageBoxType type, MediaMessageBoxOptions options );

/// Create a message box.
/// Blocks calling thread until user dismisses message box.
/// Strings provided MUST be null-terminated.
header_only MediaMessageBoxResult media_fatal_message_box_blocking(
    const char* title, const char* message
) {
    return media_message_box_blocking(
        title, message, MEDIA_MESSAGE_BOX_TYPE_ERROR, MEDIA_MESSAGE_BOX_OPTIONS_OK );
}

#endif /* header guard */
