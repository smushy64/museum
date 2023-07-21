#if !defined(PLATFORM_LINUX_HPP)
#define PLATFORM_LINUX_HPP
// * Description:  Platform Linux
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 19, 2023
#include "defines.h"
#if defined(LD_PLATFORM_LINUX)
#include "core/logging.h"
#include "core/audio.h"
#include "core/engine.h"
#include "core/input.h"
#include "platform/platform.h"
#include "platform/io.h"
#include "platform/threading.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "renderer/opengl/gl_types.h"
#define __gl_h_
#include <GL/glx.h>

typedef void* LinuxModule;

struct LinuxMutexHandle {
    pthread_mutex_t handle;
};
STATIC_ASSERT( sizeof(LinuxMutexHandle) == MUTEX_HANDLE_SIZE );

struct LinuxSemaphoreHandle {
    sem_t* handle;
};
STATIC_ASSERT( sizeof(LinuxSemaphoreHandle) == SEMAPHORE_HANDLE_SIZE );

struct LinuxThreadHandle {
    pthread_t    handle;
    ThreadProcFN thread_proc;
    void*        thread_proc_user_params;
};
STATIC_ASSERT( sizeof(LinuxThreadHandle) == THREAD_HANDLE_SIZE );

#define MODULE_COUNT (1)
#define WINDOW_TITLE_SIZE (255)
struct LinuxPlatform {
    Platform platform;

    struct LinuxWindow {
        Display*          x_display;
        xcb_connection_t* xcb_connection;
        xcb_screen_t*     xcb_screen;
        xcb_window_t      handle;
        xcb_atom_t        delete_atom;
        xcb_atom_t        protocols_atom;
        GLXDrawable       glx_drawable;
        char title[WINDOW_TITLE_SIZE];
    } window;

    struct LinuxCursor {
        CursorStyle style;
        b32         is_visible;
    } cursor;

    union {
        struct {
            LinuxModule lib_gl;
        };
        LinuxModule modules[MODULE_COUNT];
    };

};
/// Make a timespec for specified milliseconds
FORCE_INLINE struct timespec make_timespec_ms( u32 ms ) {
    struct timespec result;
    result.tv_sec  = ms / 1000;
    result.tv_nsec = ( ms % 1000 ) * 1000 * 1000;
    return result;
}

typedef int LinuxErrorCode;

KeyboardCode x_key_to_keycode( u32 x_key );

b32 linux_load_opengl( LinuxPlatform* platform );
/// Log the last posted error.
LinuxErrorCode linux_log_error( b32 present_message_box );
/// Log given error code.
void linux_log_error( LinuxErrorCode error_code, b32 present_message_box );

#define LDEFINEFN( return_name, function_name, ... )\
    typedef return_name ( * function_name##FN )( __VA_ARGS__ );\
    [[maybe_unused]]\
    global function_name##FN in_##function_name = nullptr

#define LDEFINEFN_STUB( return_name, function_name, return_value, ... )\
    typedef return_name ( * function_name##FN )( __VA_ARGS__ );\
    internal return_name function_name##_stub( __VA_ARGS__ ) { return return_value; }\
    [[maybe_unused]]\
    global function_name##FN in_##function_name = function_name##_stub

namespace impl {
    LDEFINEFN( void, glXSwapBuffers, Display*, GLXDrawable);
    LDEFINEFN( GLXContext, glXCreateNewContext, Display*, GLXFBConfig, int, GLXContext, int );
    LDEFINEFN( __GLXextFuncPtr, glXGetProcAddressARB, const GLubyte* );
    LDEFINEFN( GLXFBConfig*, glXChooseFBConfig, Display*, int, const int*, int* );
    LDEFINEFN( int, glXGetFBConfigAttrib, Display*, GLXFBConfig, int, int* );
    LDEFINEFN( void, glXDestroyContext, Display*, GLXContext );
    LDEFINEFN( int, glXMakeContextCurrent, Display*, GLXDrawable, GLXDrawable, GLXContext );
    LDEFINEFN( GLXWindow, glXCreateWindow, Display*, GLXFBConfig, Window, const int* );
} // namespace impl

#define glXSwapBuffers        ::impl::in_glXSwapBuffers
#define glXCreateNewContext   ::impl::in_glXCreateNewContext
#define glXGetProcAddressARB  ::impl::in_glXGetProcAddressARB
#define glXDestroyContext     ::impl::in_glXDestroyContext
#define glXMakeContextCurrent ::impl::in_glXMakeContextCurrent
#define glXCreateWindow       ::impl::in_glXCreateWindow
#define glXChooseFBConfig     ::impl::in_glXChooseFBConfig
#define glXGetFBConfigAttrib  ::impl::in_glXGetFBConfigAttrib

#if defined(LD_LOGGING)
    #define LINUX_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE LINUX  ] " format,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO LINUX  ] " format,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG LINUX ] " format,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN LINUX  ] " format,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR LINUX ] " format,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE LINUX  | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO LINUX  | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG LINUX | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define LINUX_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN LINUX  | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define LINUX_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR LINUX | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
    #define LINUX_LOG_FATAL( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
            "[FATAL LINUX | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
#else
    #define LINUX_LOG_NOTE( format, ... )
    #define LINUX_LOG_INFO( format, ... )
    #define LINUX_LOG_DEBUG( format, ... )
    #define LINUX_LOG_WARN( format, ... )
    #define LINUX_LOG_ERROR( format, ... )
    #define LINUX_LOG_NOTE_TRACE( format, ... )
    #define LINUX_LOG_INFO_TRACE( format, ... )
    #define LINUX_LOG_DEBUG_TRACE( format, ... )
    #define LINUX_LOG_WARN_TRACE( format, ... )
    #define LINUX_LOG_ERROR_TRACE( format, ... )
    #define LINUX_LOG_FATAL( format, ... )
#endif

#endif // if linux
#endif // header guard
