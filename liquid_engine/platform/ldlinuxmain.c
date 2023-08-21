// * Description:  Linux Platform Layer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 19, 2023
#include "defines.h"
#if defined(LD_PLATFORM_LINUX)
#include "corec.inl"

#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <cpuid.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIB_OPENGL_PATH "libGL.so"

global b32 IS_DPI_AWARE = false;
global struct timespec START_TIME;

int main( int argc, char** argv ) {
    b32 result = engine_entry( argc, argv );
    return result ? 0 : -1;
}

global const u32 VALUE_MASK = XCB_CW_COLORMAP | XCB_CW_EVENT_MASK;

global const u32 EVENT_VALUES =
    XCB_EVENT_MASK_BUTTON_PRESS   |
    XCB_EVENT_MASK_BUTTON_RELEASE |
    XCB_EVENT_MASK_KEY_PRESS      |
    XCB_EVENT_MASK_KEY_RELEASE    |
    XCB_EVENT_MASK_EXPOSURE       |
    XCB_EVENT_MASK_POINTER_MOTION |
    XCB_EVENT_MASK_STRUCTURE_NOTIFY;

b32 linux_create_window(
    xcb_connection_t* xcb_connection,
    xcb_screen_t*     xcb_screen,
    xcb_window_t* window_handle,
    ivec2 surface_dimensions,
    u32 visual_id,
    u32 value_mask,
    u32* value_list
) {
    *window_handle = xcb_generate_id( xcb_connection );

    #define XCB_WINDOW_BORDER 0
    xcb_create_window(
        xcb_connection,
        XCB_COPY_FROM_PARENT,
        *window_handle,
        xcb_screen->root,
        0, 0,
        surface_dimensions.width,
        surface_dimensions.height,
        XCB_WINDOW_BORDER,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        visual_id,
        value_mask,
        value_list
    );

    return true;
}

void linux_map_window( LinuxPlatform* platform ) {
    xcb_map_window(
        platform->window.xcb_connection,
        platform->window.handle
    );
}

void linux_destroy_window( LinuxPlatform* platform ) {
    xcb_destroy_window(
        platform->window.xcb_connection,
        platform->window.handle
    );
}

b32 platform_init(
    StringView opt_icon_path,
    ivec2 surface_dimensions,
    PlatformFlags flags,
    Platform* out_platform
) {
    ASSERT(out_platform);
    LinuxPlatform* linux_platform = (LinuxPlatform*)out_platform;
    LinuxPlatform::LinuxWindow* linux_window = &linux_platform->window;

    clock_gettime( CLOCK_MONOTONIC_RAW, &START_TIME );

    IS_DPI_AWARE = CHECK_BITS( flags, PLATFORM_DPI_AWARE );

    read_write_fence();

    linux_window->x_display = XOpenDisplay( nullptr );
    XAutoRepeatOff( linux_window->x_display );

    linux_window->xcb_connection = XGetXCBConnection(
        linux_window->x_display
    );
    if( xcb_connection_has_error( linux_window->xcb_connection ) ) {
        LINUX_LOG_FATAL( "Failed to connect to X server via XCB!" );
        return false;
    }

    const struct xcb_setup_t* xcb_setup =
        xcb_get_setup( linux_window->xcb_connection );

    // NOTE(alicia): utter nonsense
    xcb_screen_iterator_t screen_iterator =
        xcb_setup_roots_iterator( xcb_setup );
    i32 screen_index = 0;
    for( i32 s = screen_index; s > 0; s-- ) {
        xcb_screen_next( &screen_iterator );
    }

    linux_window->xcb_screen = screen_iterator.data;

    u32 value_list[] = {
        linux_window->xcb_screen->black_pixel,
        EVENT_VALUES
    };

    // tell server to notify when window is being destroyed
    const char* DELETE_COOKIE_NAME = "WM_DELETE_WINDOW";
    xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(
        linux_window->xcb_connection,
        0,
        str_length(DELETE_COOKIE_NAME),
        DELETE_COOKIE_NAME
    );
    const char* PROTOCOL_COOKIE_NAME = "WM_PROTOCOLS";
    xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(
        linux_window->xcb_connection,
        0,
        str_length(PROTOCOL_COOKIE_NAME),
        PROTOCOL_COOKIE_NAME
    );
    xcb_intern_atom_reply_t* delete_reply = xcb_intern_atom_reply(
        linux_window->xcb_connection,
        delete_cookie,
        nullptr
    );
    xcb_intern_atom_reply_t* protocols_reply = xcb_intern_atom_reply(
        linux_window->xcb_connection,
        protocols_cookie,
        nullptr
    );

    linux_window->delete_atom    = delete_reply->atom;
    linux_window->protocols_atom = protocols_reply->atom;

    b32 create_window_result = linux_create_window(
        linux_window->xcb_connection,
        linux_window->xcb_screen,
        &linux_window->handle,
        surface_dimensions,
        linux_window->xcb_screen->root_visual,
        VALUE_MASK,
        value_list
    );

    if( !create_window_result ) {
        return false;
    }

    const char* DEFAULT_TITLE = "Liquid Engine";
    StringView default_title_view = DEFAULT_TITLE;
    platform_surface_set_name(
        out_platform,
        default_title_view
    );
    linux_map_window( linux_platform );

    i32 stream_result = xcb_flush( linux_window->xcb_connection );
    if( stream_result <= 0 ) {
        // NOTE(alicia): am i gonna be professional? no teehee
        LINUX_LOG_FATAL( "A fatal error occurred when flushing the toilet!" );
        return false;
    }

    out_platform->surface.dimensions = surface_dimensions;
    out_platform->is_active          = true;

    unused(opt_icon_path);

    LINUX_LOG_INFO( "Linux Platform has been initialized successfully." );
    return true;
}

void platform_shutdown( Platform* platform ) {
    LinuxPlatform* linux_platform = (LinuxPlatform*)platform;
    XAutoRepeatOn( linux_platform->window.x_display );
    linux_destroy_window( linux_platform );
    
    for( u32 i = 0; i < MODULE_COUNT; ++i ) {
        if( linux_platform->modules[i] ) {
            library_free( linux_platform->modules[i] );
        }
    }
}

b32 platform_pump_events( Platform* platform ) {
    LinuxPlatform* linux_platform = (LinuxPlatform*)platform;
    xcb_generic_event_t*        xcb_event;
    xcb_client_message_event_t* client_message;

    Event event;
    while( ( xcb_event = xcb_poll_for_event(
        linux_platform->window.xcb_connection
    ) ) ) {
        // NOTE(alicia): not sure what the AND is for
        switch( xcb_event->response_type & ~0x80 ) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: {
                xcb_key_press_event_t* kb_event =
                    (xcb_key_press_event_t*)xcb_event;
                b32 is_down = kb_event->response_type == XCB_KEY_PRESS;
                xcb_keycode_t xcb_keycode = kb_event->detail;
                KeySym keysym = XkbKeycodeToKeysym(
                    linux_platform->window.x_display,
                    (KeyCode)xcb_keycode,
                    0, 0
                );
                KeyboardCode keycode = x_key_to_keycode( keysym );
                if( keycode != KEY_UNKNOWN ) {
                    input_set_key( keycode, is_down );
                }
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: {
                xcb_button_press_event_t* mouse_event =
                    (xcb_button_press_event_t*)xcb_event;
                b32 is_down = mouse_event->response_type == XCB_BUTTON_PRESS;
                MouseCode mouse_code = MOUSE_BUTTON_UNKNOWN;
                switch( mouse_event->detail ) {
                    case XCB_BUTTON_INDEX_1:
                        mouse_code = MOUSE_BUTTON_LEFT;
                        break;
                    case XCB_BUTTON_INDEX_2:
                        mouse_code = MOUSE_BUTTON_MIDDLE;
                        break;
                    case XCB_BUTTON_INDEX_3:
                        mouse_code = MOUSE_BUTTON_RIGHT;
                        break;
                    case XCB_BUTTON_INDEX_4:
                        mouse_code = MOUSE_BUTTON_EXTRA_1;
                        break;
                    case XCB_BUTTON_INDEX_5:
                        mouse_code = MOUSE_BUTTON_EXTRA_2;
                        break;
                    default: break;
                }
                if( mouse_code != MOUSE_BUTTON_UNKNOWN ) {
                    input_set_mouse_button( mouse_code, is_down );
                }
            } break;
            case XCB_MOTION_NOTIFY: {
                xcb_motion_notify_event_t* move_event =
                    (xcb_motion_notify_event_t*)xcb_event;
                // TODO(alicia): make sure this is correct
                input_set_mouse_position({
                    move_event->event_x,
                    move_event->event_y
                });
            } break;
            case XCB_CONFIGURE_NOTIFY: {
                xcb_configure_notify_event_t* configure =
                    (xcb_configure_notify_event_t*)xcb_event;
                ivec2 dimensions = {
                    configure->width,
                    configure->height
                };
                if( platform->surface.dimensions != dimensions ) {
                    platform->surface.dimensions = dimensions;
                    event = {};
                    event.code = EVENT_CODE_RESIZE;
                    event.data.int32[0] = dimensions.width;
                    event.data.int32[1] = dimensions.height;

                    event_fire( event );
                }
            } break;
            case XCB_CLIENT_MESSAGE: {
                client_message =
                    (xcb_client_message_event_t*)xcb_event;
                if(
                    client_message->data.data32[0] ==
                    linux_platform->window.delete_atom
                ) {
                    event = {};
                    event.code = EVENT_CODE_EXIT;
                    event_fire( event );
                }
            } break;
        }

        free( xcb_event );
    }

    return true;
}
// TODO(alicia): actually implement gamepad polling
void platform_poll_gamepad( Platform* platform ) {
    unused(platform);
}
// TODO(alicia): actually implement gamepad motor state
void platform_set_pad_motor_state(
    Platform* platform,
    u32 gamepad_index, u32 motor, f32 value
) {
    unused(platform);
    unused(gamepad_index);
    unused(motor);
    unused(value);
}
void platform_surface_set_name(
    Platform* platform,
    StringView name
) {
    LinuxPlatform* linux_platform = (LinuxPlatform*)platform;
    LinuxPlatform::LinuxWindow* linux_window = &linux_platform->window;

    if( name.len >= WINDOW_TITLE_SIZE ) {
        name.len = WINDOW_TITLE_SIZE - 1;
    }

    if( name.buffer != linux_window->title ) {
        StringView title_view;
        title_view.len    = WINDOW_TITLE_SIZE;
        title_view.buffer = linux_window->title;
        string_format(
            title_view,
            "{sv}{c}",
            name, 0
        );
    }

    xcb_change_property(
        linux_window->xcb_connection,
        XCB_PROP_MODE_REPLACE,
        linux_window->handle,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        str_length( linux_window->title ),
        linux_window->title
    );
}
// TODO(alicia): implement surface read name
i32 platform_surface_read_name(
    Platform* platform,
    char* buffer, usize max_buffer_size
) {
    unused(platform);
    unused(buffer);
    unused(max_buffer_size);
    return 0;
}
// TODO(alicia): implement cursor set style
void platform_cursor_set_style(
    Platform* platform, CursorStyle cursor_style
) {
    unused(platform);
    unused(cursor_style);
}
// TODO(alicia): implement cursor set visible
void platform_cursor_set_visible( Platform* platform, b32 visible ) {
    unused(platform);
    unused(visible);
}
// TODO(alicia): implement cursor center
void platform_cursor_center( Platform* platform ) {
    unused(platform);
}

// TODO(alicia): actually implement message box
MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
) {
    unused(window_title);
    unused(message);
    unused(type);
    unused(icon);
    return MBRESULT_OK;
}

u32 query_platform_subsystem_size() {
    return sizeof(LinuxPlatform);
}

f64 platform_us_elapsed() {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
    time_t seconds_elapsed     = ts.tv_sec  - START_TIME.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - START_TIME.tv_nsec;
    f64 microseconds_elapsed =
        ( (f64)seconds_elapsed     * 1000000.0 ) +
        ( (f64)nanoseconds_elapsed / 1000.0 );
    return microseconds_elapsed;
}
f64 platform_ms_elapsed() {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
    time_t seconds_elapsed     = ts.tv_sec  - START_TIME.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - START_TIME.tv_nsec;
    f64 milliseconds_elapsed =
        ( (f64)seconds_elapsed     * 1000.0 ) +
        ( (f64)nanoseconds_elapsed / 1000000.0 );
    return milliseconds_elapsed;
}
f64 platform_s_elapsed() {
    struct timespec ts;
    clock_gettime( CLOCK_MONOTONIC_RAW, &ts );
    time_t seconds_elapsed     = ts.tv_sec  - START_TIME.tv_sec;
    time_t nanoseconds_elapsed = ts.tv_nsec - START_TIME.tv_nsec;
    return 
        ( (f64)seconds_elapsed ) +
        ( (f64)nanoseconds_elapsed / 1000000000.0 );
}

global LinuxModule LIB_GL = nullptr;
void* linux_gl_load_proc( const char* function_name ) {
    void* function = (void*)glXGetProcAddressARB(
        (const GLubyte*)function_name
    );
    if( !function ) {
        function = (void*)dlsym( LIB_GL, function_name );

#if defined(LD_LOGGING)
        if( !function ) {
            LINUX_LOG_WARN(
                "Failed to load GL function \"{cc}\"!",
                function_name
            );
        }
#endif
    }

    return function;
}
b32 linux_load_opengl( LinuxPlatform* platform ) {
    if( !library_load( LIB_OPENGL_PATH, &platform->lib_gl ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load " LIB_OPENGL_PATH "!"
        );
        return false;
    }
    LIB_GL = platform->lib_gl;

    #define LOAD_GL_FUNCTION( function_name )\
        function_name = (::impl:: function_name##FN )library_load_function(\
            platform->lib_gl,\
            #function_name\
        );\
        if( !( function_name ) )\
            return false

    LOAD_GL_FUNCTION( glXCreateNewContext );
    LOAD_GL_FUNCTION( glXSwapBuffers );
    LOAD_GL_FUNCTION( glXGetProcAddressARB );
    LOAD_GL_FUNCTION( glXDestroyContext );
    LOAD_GL_FUNCTION( glXMakeContextCurrent );
    LOAD_GL_FUNCTION( glXCreateWindow );
    LOAD_GL_FUNCTION( glXChooseFBConfig );
    LOAD_GL_FUNCTION( glXGetFBConfigAttrib );

    return true;
}
void* platform_gl_init( Platform* platform ) {
    LinuxPlatform* linux_platform = (LinuxPlatform*)platform;
 
    if( !linux_load_opengl( linux_platform ) ) {
        LINUX_LOG_ERROR( "Failed to load any OpenGL functions!" );
        return nullptr;
    }

    local const int visual_attribs[] = {
        GLX_X_RENDERABLE,            true,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,     GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE,                   8,
        GLX_GREEN_SIZE,                 8,
        GLX_BLUE_SIZE,                  8,
        GLX_ALPHA_SIZE,                 8,
        GLX_DEPTH_SIZE,                24,
        GLX_STENCIL_SIZE,               8,
        GLX_DOUBLEBUFFER,            true,
        0
    };

    GLXFBConfig* fb_configs = nullptr;
    int fb_config_count = 0;
    fb_configs = glXChooseFBConfig(
        linux_platform->window.x_display,
        0, visual_attribs,
        &fb_config_count
    );
    if( !fb_configs || !fb_config_count ) {
        LINUX_LOG_FATAL( "Failed to choose framebuffer config!" );
        return nullptr;
    }

    int visual_id = 0;
    GLXFBConfig fb_config = fb_configs[0];
    glXGetFBConfigAttrib(
        linux_platform->window.x_display,
        fb_config,
        GLX_VISUAL_ID,
        &visual_id
    );

    GLXContext context = glXCreateNewContext(
        linux_platform->window.x_display,
        fb_config,
        GLX_RGBA_TYPE,
        0,
        true
    );
    if( !context ) {
        LINUX_LOG_FATAL( "Failed to create OpenGL context!" );
        return nullptr;
    }

    xcb_colormap_t colormap =
        xcb_generate_id( linux_platform->window.xcb_connection );
    xcb_create_colormap(
        linux_platform->window.xcb_connection,
        XCB_COLORMAP_ALLOC_NONE,
        colormap,
        linux_platform->window.xcb_screen->root,
        visual_id
    );

    // TODO(alicia): check if window needs to be
    // rebuilt or if you can just change the value list
    // and value id
    u32 value_list[] = {
        EVENT_VALUES, colormap
    };
    linux_destroy_window( linux_platform );
    linux_create_window(
        linux_platform->window.xcb_connection,
        linux_platform->window.xcb_screen,
        &linux_platform->window.handle,
        platform->surface.dimensions,
        visual_id,
        VALUE_MASK,
        value_list
    );
    platform_surface_set_name(
        platform,
        linux_platform->window.title
    );
    linux_map_window( linux_platform );

    GLXWindow glx_window = glXCreateWindow(
        linux_platform->window.x_display,
        fb_config,
        linux_platform->window.handle,
        0
    );
    linux_platform->window.glx_drawable = glx_window;

    if( !glXMakeContextCurrent(
        linux_platform->window.x_display,
        linux_platform->window.glx_drawable,
        linux_platform->window.glx_drawable,
        context
    ) ) {
        LINUX_LOG_FATAL( "Failed to make context current!" );
        return nullptr;
    }

    if( !gl_load( linux_gl_load_proc ) ) {
        LINUX_LOG_FATAL( "Failed to load OpenGL functions!" );
        return nullptr;
    }

    return context;
}
void platform_gl_shutdown( Platform* platform, void* glrc ) {
    LinuxPlatform* linux_platform = (LinuxPlatform*)platform;
    glXDestroyContext(
        linux_platform->window.x_display,
        (GLXContext)glrc
    );
}
void platform_gl_swap_buffers( Platform* platform ) {
    LinuxPlatform::LinuxWindow* linux_window = &(((LinuxPlatform*)platform)->window);
    glXSwapBuffers(
        linux_window->x_display,
        linux_window->glx_drawable
    );
}

// TODO(alicia): audio subsystem
b32 platform_init_audio( Platform* platform ) {
    unused(platform);
    return true;
}
void platform_shutdown_audio( Platform* platform ) {
    unused(platform);
}
void platform_audio_test( Platform* platform, i16 volume ) {
    unused( platform );
    unused( volume );
}

void platform_sleep( Platform* platform, u32 ms ) {
    unused(platform);

#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts = make_timespec_ms( ms );
    nanosleep( &ts, nullptr );
#else
    if( ms >= 1000 ) {
        sleep( ms / 1000 );
    }
    usleep( (ms % 1000) * 1000 );
#endif
}

void platform_write_console(
    void* output_handle,
    u32 write_count,
    const char* buffer
) {
    size_t written = fwrite(
        buffer,
        1, write_count,
        (FILE*)output_handle
    );
    if( written != write_count ) {
        linux_log_error( false );
    }
}
void* platform_stdout_handle() {
    return stdout;
}
void* platform_stderr_handle() {
    return stderr;
}

// TODO(alicia): rewrite with syscalls
b32 platform_file_open(
    const char*   path,
    FileOpenFlags flags,
    FileHandle*   out_handle
) {

    const char* mode = "";

    if( CHECK_BITS( flags, PLATFORM_FILE_OPEN_READ ) ) {
        if( CHECK_BITS( flags, PLATFORM_FILE_OPEN_WRITE ) ) {
            mode = "wb+";
        } else {
            mode = "rb";
        }
    } else if( CHECK_BITS( flags, PLATFORM_FILE_OPEN_WRITE ) ) {
        mode = "wb";
    } else {
        LINUX_LOG_ERROR( "Failed to open file, flags are invalid!" );
        return false;
    }

    FILE* file_handle = fopen( path, mode );
    if( !file_handle ) {
        linux_log_error( false );
        LINUX_LOG_ERROR( "Failed to open file \"{cc}\"!", path );
        return false;
    }

    out_handle->platform = file_handle;
    return true;
}
void platform_file_close( FileHandle handle ) {
    FILE* file = (FILE*)handle.platform;
    fclose( file );
    handle.platform = 0;
}
b32 platform_file_read(
    FileHandle handle,
    usize read_size,
    usize buffer_size,
    void* buffer
) {
    ASSERT( buffer_size >= read_size );
    FILE* file = (FILE*)handle.platform;
    size_t result = fread( buffer, sizeof(u8), read_size, file );

    if( result == read_size ) {
        return true;
    } else {
        if( feof( file ) ) {
            return true;
        } else {
            linux_log_error( false );
            return false;
        }
    }
}
usize platform_file_query_size( FileHandle handle ) {
    FILE* file = (FILE*)handle.platform;
    fseeko( file, 0, SEEK_END );
    usize size = ftello( file );
    rewind( file );
    return size;
}
usize platform_file_query_offset( FileHandle handle ) {
    FILE* file = (FILE*)handle.platform;
    return ftello( file );
}
b32 platform_file_set_offset( FileHandle handle, usize offset ) {
    FILE* file = (FILE*)handle.platform;
    LinuxErrorCode result = fseeko( file, offset, SEEK_SET );
    if( result != 0 ) {
        linux_log_error( result, false );
        return false;
    }

    return true;
}

// TODO(alicia): replace with mmap/sbrk, not sure
// how those work yet unfortunately
void* heap_alloc( usize size ) {
    void* buffer = malloc( size );
    if( buffer ) {
        mem_zero( buffer, size );
    }
    return buffer;
}
void* heap_realloc( void* memory, usize new_size ) {
    return realloc( memory, new_size );
}
void heap_free( void* memory ) {
    free( memory );
}

void* platform_page_alloc( usize size ) {
    return malloc( size );
}
void platform_page_free( void* memory ) {
    free( memory );
}

b32 platform_thread_create(
    struct Platform* platform,
    ThreadProcFN     thread_proc,
    void*            user_params,
    usize            thread_stack_size,
    ThreadHandle*    out_thread_handle
) {
    ASSERT(thread_proc);
    ASSERT(out_thread_handle);
    unused(platform);
    LinuxThreadHandle* linux_thread =
        (LinuxThreadHandle*)out_thread_handle;

    linux_thread->thread_proc             = thread_proc;
    linux_thread->thread_proc_user_params = user_params;

    read_write_fence();

    pthread_attr_t attributes;
    pthread_attr_init( &attributes );
    pthread_attr_setstacksize( &attributes, thread_stack_size );
    i32 create_result = pthread_create(
        &linux_thread->handle,
        &attributes,
        thread_proc,
        &linux_thread
    );
    if( create_result != 0 ) {
        linux_log_error( create_result, false );
        return false;
    }

    usize id = (usize)&linux_thread->handle;
    LINUX_LOG_NOTE( "New thread created. ID: {u64}", id );
    return true;
}

NO_OPTIMIZE LD_API void read_write_fence() {
    asm volatile ("" ::: "memory");
}
NO_OPTIMIZE LD_API void read_fence() {
    asm volatile ("" ::: "memory");
}
NO_OPTIMIZE LD_API void write_fence() {
    asm volatile ("" ::: "memory");
}

u32 platform_interlocked_increment( volatile u32* addend ) {
    return __sync_fetch_and_add( addend, 1 );
}
u32 platform_interlocked_decrement( volatile u32* addend ) {
    return __sync_fetch_and_sub( addend, 1 );
}
u32 platform_interlocked_exchange( volatile u32* target, u32 value ) {
    return __sync_swap( target, value );
}
void* platform_interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
) {
    return __sync_val_compare_and_swap( dst, exchange, comperand );
}
u32 platform_interlocked_compare_exchange(
    u32 volatile* dst,
    u32 exchange, u32 comperand
) {
    return __sync_val_compare_and_swap( dst, exchange, comperand );
}

LD_API b32 mutex_create( MutexHandle* out_mutex ) {
    LinuxMutexHandle* linux_mutex = (LinuxMutexHandle*)out_mutex;
    i32 result = pthread_mutex_init( &linux_mutex->handle, 0 );
    if( result != 0 ) {
        linux_log_error( result, false );
        LINUX_LOG_ERROR( "Failed to create mutex!" );
        return false;
    }

    return true;
}
LD_API void mutex_lock( MutexHandle* mutex ) {
    LinuxMutexHandle* linux_mutex = (LinuxMutexHandle*)mutex;
    pthread_mutex_lock( &linux_mutex->handle );
}
LD_API void mutex_unlock( MutexHandle* mutex ) {
    LinuxMutexHandle* linux_mutex = (LinuxMutexHandle*)mutex;
    pthread_mutex_unlock( &linux_mutex->handle );
}
LD_API void mutex_destroy( MutexHandle* mutex ) {
    LinuxMutexHandle* linux_mutex = (LinuxMutexHandle*)mutex;
    pthread_mutex_destroy( &linux_mutex->handle );
    *linux_mutex = {};
}

global int RUNNING_SEMAPHORE_INDEX = 0;
LD_API b32 semaphore_create(
    const char* opt_name, u32 initial_count,
    SemaphoreHandle* out_semaphore_handle
) {
    LinuxSemaphoreHandle* linux_semaphore =
        (LinuxSemaphoreHandle*)out_semaphore_handle;

    char name_buffer[32];
    StringView name_view;
    if( opt_name ) {
        name_view = StringView(opt_name);
    } else {
        name_view.len    = 32;
        name_view.buffer = name_buffer;
        string_format(
            name_view,
            "/sem_{i}{c}",
            RUNNING_SEMAPHORE_INDEX++,
            0
        );
    }

    mode_t mode = S_IRWXU;

    sem_t* semaphore = (sem_t*)sem_open(
        name_view.buffer,
        O_CREAT,
        mode,
        initial_count
    );

    if( !semaphore ) {
        linux_log_error( false );
        LINUX_LOG_ERROR( "Failed to create semaphore!" );
        return false;
    }

    linux_semaphore->handle = semaphore;
    return true;
}
LD_API void semaphore_increment( SemaphoreHandle* semaphore_handle ) {
    LinuxSemaphoreHandle* linux_semaphore =
        (LinuxSemaphoreHandle*)semaphore_handle;
    sem_post( linux_semaphore->handle );
}
LD_API void semaphore_wait(
    SemaphoreHandle* semaphore_handle,
    b32 infinite_timeout, u32 opt_timeout_ms
) {
    LinuxSemaphoreHandle* linux_semaphore =
        (LinuxSemaphoreHandle*)semaphore_handle;
    if( infinite_timeout ) {
        sem_wait( linux_semaphore->handle );
    } else {
        struct timespec ts = make_timespec_ms( opt_timeout_ms );
        sem_timedwait( linux_semaphore->handle, &ts );
    }
}
LD_API void semaphore_destroy( SemaphoreHandle* semaphore_handle ) {
    LinuxSemaphoreHandle* linux_semaphore =
        (LinuxSemaphoreHandle*)semaphore_handle;
    sem_close( linux_semaphore->handle );
    *linux_semaphore = {};
}

FORCE_INLINE void cpuid( int* cpu_info, unsigned int i ) {
    asm volatile (
        "cpuid"
        : "=a" (cpu_info[0]),
        "=b" (cpu_info[1]),
        "=c" (cpu_info[2]),
        "=d" (cpu_info[3])
        : "a" (i), "c"(0)
    );
}

FORCE_INLINE void cpuinfo( int info[4], int info_type ) {
    __cpuid_count( info_type, 0, info[0], info[1], info[2], info[3] );
}

SystemInfo query_system_info() {
    SystemInfo result = {};
    result.logical_processor_count = sysconf( _SC_NPROCESSORS_ONLN );
    struct sysinfo sys_info = {};
    sysinfo( &sys_info );
    result.total_memory = sys_info.totalram;

#if defined(LD_ARCH_X86)
    int info[4];
    cpuid( info, 0 );
    int n_ids = info[0];

    if( n_ids >= 0x00000001 ) {
        cpuid( info, 0x00000001 );
        if( (info[3] & ((int)1 << 25)) != 0 ) {
            result.features |= SSE_MASK;
        }
        if( (info[3] & ((int)1 << 26)) != 0 ) {
            result.features |= SSE2_MASK;
        }
        if( (info[2] & ((int)1 << 0)) != 0 ) {
            result.features |= SSE3_MASK;
        }
        if( (info[2] & ((int)1 << 9)) != 0 ) {
            result.features |= SSSE3_MASK;
        }
        if( (info[2] & ((int)1 << 19)) != 0 ) {
            result.features |= SSE4_1_MASK;
        }
        if( (info[2] & ((int)1 << 20)) != 0 ) {
            result.features |= SSE4_2_MASK;
        }
        if( (info[2] & ((int)1 << 28)) != 0 ) {
            result.features |= AVX_MASK;
        }
    }
    if( n_ids >= 0x00000007 ) {
        cpuid( info, 0x00000007 );
        if( (info[1] & ((int)1 << 5)) != 0 ) {
            result.features |= AVX2_MASK;
        }
    }

    mem_set(
        ' ',
        CPU_NAME_BUFFER_SIZE,
        result.cpu_name_buffer
    );
    result.cpu_name_buffer[CPU_NAME_BUFFER_SIZE - 1] = 0;

    int cpu_info[4];
    cpuid( cpu_info, 0x80000002 );
    mem_copy(
        result.cpu_name_buffer,
        cpu_info,
        sizeof(cpu_info)
    );
    cpuid( cpu_info, 0x80000003 );
    mem_copy(
        result.cpu_name_buffer + 16,
        cpu_info,
        sizeof(cpu_info)
    );
    cpuid( cpu_info, 0x80000004 );
    mem_copy(
        result.cpu_name_buffer + 32,
        cpu_info,
        sizeof(cpu_info)
    );

    StringView cpu_name = result.cpu_name_buffer;
    string_trim_trailing_whitespace( cpu_name );
#endif

    return result;
}

namespace impl {

LD_API b32 _library_load(
    const char* library_name, 
    LibraryHandle* out_library
) {
    LinuxModule handle = dlopen( library_name, RTLD_LAZY );
    if( handle ) {
        *out_library = handle;
        return true;
    } else {
        return false;
    }
}
LD_API b32 _library_load_trace(
    const char* library_name, 
    LibraryHandle* out_library,
    const char* function,
    const char* file,
    i32 line
) {
    LinuxModule result = nullptr;
    if( !_library_load( library_name, &result ) ) {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,
            LOG_COLOR_RED, LOG_FLAG_NEW_LINE,
            "[ERROR LINUX | {cc}() | {cc}:{i}] "
            "Failed to load library \"{cc}\"!",
            function, file, line,
            library_name
        );
        return false;
    }
    *out_library = result;
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
        LOG_COLOR_RESET, LOG_FLAG_NEW_LINE,
        "[NOTE LINUX  | {cc}() | {cc}:{i}] "
        "Library \"{cc}\" has been loaded successfully. "
        "Pointer: {u64,x}",
        function, file, line,
        library_name, (u64)result
    );

    return true;
}
LD_API void _library_free( LibraryHandle library ) {
    dlclose( library );
}
LD_API void _library_free_trace( 
    LibraryHandle library,
    const char* function,
    const char* file,
    i32 line
) {
    log_formatted_locked(
        LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
        LOG_COLOR_RESET, LOG_FLAG_NEW_LINE,
        "[NOTE LINUX  | {cc}() | {cc}:{i}] "
        "Library has been freed. "
        "Pointer: {u64,x}",
        function, file, line,
        (u64)library
    );
    _library_free( library );
}
LD_API void* _library_load_function(
    LibraryHandle library,
    const char* function_name
) {
    return dlsym( library, function_name );
    return nullptr;
}
LD_API void* _library_load_function_trace(
    LibraryHandle library,
    const char* function_name,
    const char* function,
    const char* file,
    i32 line
) {
    void* result = _library_load_function( library, function_name );
    if( result ) {
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
            LOG_COLOR_RESET,
            LOG_FLAG_NEW_LINE,
            "[NOTE LINUX  | {cc}() | {cc}:{i}] "
            "Function \"{cc}\" loaded from library {u64,x} successfully.",
            function, file, line,
            function_name,
            (u64)library
        );
    } else {
        log_formatted_locked(
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,
            LOG_COLOR_RED, LOG_FLAG_NEW_LINE,
            "[ERROR LINUX | {cc}() | {cc}:{i}] "
            "Unable to load function \"{cc}\" from library {u64,x}!",
            function, file, line,
            function_name,
            (u64)library
        );
    }

    return result;
}

} // namespace impl

#define ERROR_MESSAGE_BUFFER_SIZE (1024)
global char ERROR_MESSAGE_BUFFER[ERROR_MESSAGE_BUFFER_SIZE];

void linux_log_error( LinuxErrorCode error_code, b32 present_message_box ) {
    const char* error_message = strerror( error_code );
    LINUX_LOG_ERROR(
        "{i}: {cc}",
        error_code,
        error_message
    );

    if( present_message_box ) {
        StringView error_message_buffer_view;
        error_message_buffer_view.buffer = ERROR_MESSAGE_BUFFER;
        error_message_buffer_view.len    = ERROR_MESSAGE_BUFFER_SIZE - 1;
        string_format(
            error_message_buffer_view,
            "Encountered a fatal Linux error!\n{cc}\n"
            LD_CONTACT_MESSAGE "\n{c}",
            error_message, 0
        );

        MESSAGE_BOX_FATAL(
            "Fatal Linux Error",
            ERROR_MESSAGE_BUFFER
        );
    }
}

LinuxErrorCode linux_log_error( b32 present_message_box ) {
    LinuxErrorCode error_code = errno;
    if( error_code <= 0 ) {
        return error_code;
    }

    linux_log_error( error_code, present_message_box );

    return error_code;
}

KeyboardCode x_key_to_keycode( u32 x_key ) {
    switch( x_key ) {
        case XK_BackSpace:   return KEY_BACKSPACE;
        case XK_Return:      return KEY_ENTER;
        case XK_Tab:         return KEY_TAB;
        case XK_Pause:       return KEY_PAUSE;
        case XK_Caps_Lock:   return KEY_CAPSLOCK;
        case XK_Escape:      return KEY_ESCAPE;
        case XK_space:       return KEY_SPACE;
        case XK_Prior:       return KEY_PAGE_UP;
        case XK_Next:        return KEY_PAGE_DOWN;
        case XK_End:         return KEY_END;
        case XK_Home:        return KEY_HOME;
        case XK_Print:       return KEY_PRINT_SCREEN;
        case XK_Insert:      return KEY_INSERT;
        case XK_Delete:      return KEY_DELETE;
        case XK_Meta_L:      return KEY_SUPER_LEFT;
        case XK_Meta_R:      return KEY_SUPER_RIGHT;
        case XK_Num_Lock:    return KEY_NUM_LOCK;
        case XK_Scroll_Lock: return KEY_SCROLL_LOCK;
        case XK_Shift_L:     return KEY_SHIFT_LEFT;
        case XK_Shift_R:     return KEY_SHIFT_RIGHT;
        case XK_Control_L:   return KEY_CONTROL_LEFT;
        case XK_Control_R:   return KEY_CONTROL_RIGHT;
        case XK_Alt_L:       return KEY_ALT_LEFT;
        case XK_Alt_R:       return KEY_ALT_RIGHT;
        case XK_semicolon:   return KEY_SEMICOLON;
        case XK_comma:       return KEY_COMMA;
        case XK_minus:       return KEY_MINUS;
        case XK_period:      return KEY_PERIOD;
        case XK_slash:       return KEY_SLASH_FORWARD;
        case XK_grave:       return KEY_BACKTICK;

        case XK_Left:  return KEY_ARROW_LEFT;
        case XK_Right: return KEY_ARROW_RIGHT;
        case XK_Up:    return KEY_ARROW_UP;
        case XK_Down:  return KEY_ARROW_DOWN;

        case '0' ... '9':
            return (KeyboardCode)((u32)KEY_0 + (x_key - '0'));

        case XK_KP_0 ... XK_KP_9:
            return (KeyboardCode)((u32)KEY_PAD_0 + (x_key - XK_KP_0));

        case 'A' ... 'Z':
            return (KeyboardCode)(x_key);
        case 'a' ... 'z':
            return (KeyboardCode)(x_key - 32);

        case XK_KP_Equal:    
        case XK_plus:     return KEY_EQUALS;

        case XK_F1:  return KEY_F1; 
        case XK_F2:  return KEY_F2; 
        case XK_F3:  return KEY_F3; 
        case XK_F4:  return KEY_F4; 
        case XK_F5:  return KEY_F5; 
        case XK_F6:  return KEY_F6; 
        case XK_F7:  return KEY_F7; 
        case XK_F8:  return KEY_F8; 
        case XK_F9:  return KEY_F9; 
        case XK_F10: return KEY_F10; 
        case XK_F11: return KEY_F11; 
        case XK_F12: return KEY_F12; 
        case XK_F13: return KEY_F13; 
        case XK_F14: return KEY_F14; 
        case XK_F15: return KEY_F15; 
        case XK_F16: return KEY_F16; 
        case XK_F17: return KEY_F17; 
        case XK_F18: return KEY_F18; 
        case XK_F19: return KEY_F19; 
        case XK_F20: return KEY_F20; 
        case XK_F21: return KEY_F21; 
        case XK_F22: return KEY_F22; 
        case XK_F23: return KEY_F23; 
        case XK_F24: return KEY_F24; 

        case XK_KP_Decimal:
        case XK_KP_Add:
        case XK_KP_Separator:
        case XK_KP_Divide:
        case XK_multiply:
        case XK_Select:
        case XK_Help:
        case XK_Execute:
        case XK_Mode_switch: 
        default:
            return KEY_UNKNOWN;
    }
}

#endif // if linux
