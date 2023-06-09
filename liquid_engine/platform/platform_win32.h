
#if !defined(PLATFORM_WIN32_HPP)
#define PLATFORM_WIN32_HPP
/**
 * Description:  Platform Win32
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 08, 2023
*/
#include "defines.h"
#if defined(SM_PLATFORM_WINDOWS)

#include "core/logging.h"
#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <xinput.h>

struct Win32Cursor {
    CursorStyle style;
    b32         is_visible;
};

struct Win32Window {
    HWND handle;
    HDC  device_context;
};

#define MAX_WINDOW_TITLE_BUFFER_SIZE 512ULL
#define ERROR_MESSAGE_BUFFER_SIZE 512
#define MAX_MODULE_COUNT 4
struct Win32Platform {
    Win32Window window;
    Win32Cursor cursor;
    HINSTANCE instance;
    struct OpenGLContext* gl_context;
    union {
        struct {
            HMODULE lib_user32;
            HMODULE lib_xinput;
            HMODULE lib_gl;
            HMODULE lib_gdi32;
        };
        HMODULE modules[MAX_MODULE_COUNT];
    };
    LARGE_INTEGER performance_frequency;
    LARGE_INTEGER performance_counter;

    char error_message_buffer[ERROR_MESSAGE_BUFFER_SIZE];
    wchar_t window_title_buffer[MAX_WINDOW_TITLE_BUFFER_SIZE];
};
global const char* WIN32_VULKAN_EXTENSIONS[] = {
    "VK_KHR_win32_surface"
};
b32 win32_load_user32( HMODULE* out_module );
b32 win32_load_xinput( HMODULE* out_module );
b32 win32_load_opengl_gdi32( HMODULE* out_gl, HMODULE* out_gdi32 );
LRESULT win32_winproc( HWND, UINT, WPARAM, LPARAM );

#if defined(LD_LOGGING)
    #define WIN32_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE WIN32  ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO WIN32  ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG WIN32 ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN WIN32  ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR WIN32 ] " format,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE WIN32  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO WIN32  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG WIN32 | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define WIN32_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN WIN32  | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR WIN32 | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_FATAL( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
            "[FATAL WIN32 | %s() | %s:%i] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
#else
    #define WIN32_LOG_NOTE( format, ... )
    #define WIN32_LOG_INFO( format, ... )
    #define WIN32_LOG_DEBUG( format, ... )
    #define WIN32_LOG_WARN( format, ... )
    #define WIN32_LOG_ERROR( format, ... )
    #define WIN32_LOG_NOTE_TRACE( format, ... )
    #define WIN32_LOG_INFO_TRACE( format, ... )
    #define WIN32_LOG_DEBUG_TRACE( format, ... )
    #define WIN32_LOG_WARN_TRACE( format, ... )
    #define WIN32_LOG_ERROR_TRACE( format, ... )
    #define WIN32_LOG_FATAL( format, ... )
#endif

DWORD win32_log_error( b32 present_message_box );

/// from https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
typedef struct tagPIXELFORMATDESCRIPTOR {
    WORD  nSize;
    WORD  nVersion;
    DWORD dwFlags;
    BYTE  iPixelType;
    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cAccumBits;
    BYTE  cAccumRedBits;
    BYTE  cAccumGreenBits;
    BYTE  cAccumBlueBits;
    BYTE  cAccumAlphaBits;
    BYTE  cDepthBits;
    BYTE  cStencilBits;
    BYTE  cAuxBuffers;
    BYTE  iLayerType;
    BYTE  bReserved;
    DWORD dwLayerMask;
    DWORD dwVisibleMask;
    DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR, *PPIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;

/// The buffer can draw to a window or device surface.
#define PFD_DRAW_TO_WINDOW 0x00000004
/// The buffer can draw to a memory bitmap.
#define PFD_DRAW_TO_BITMAP 0x00000008
/// The buffer supports GDI drawing.
/// This flag and PFD_DOUBLEBUFFER are mutually exclusive
/// in the current generic implementation.
#define PFD_SUPPORT_GDI 0x00000010
/// The buffer supports OpenGL drawing.
#define PFD_SUPPORT_OPENGL 0x00000020
/// The pixel format is supported by a device driver
/// that accelerates the generic implementation.
/// If this flag is clear and the PFD_GENERIC_FORMAT flag is set,
/// the pixel format is supported by the generic implementation only.
#define PFD_GENERIC_ACCELERATED 0x00001000
/// The pixel format is supported by the GDI software implementation,
/// which is also known as the generic implementation.
/// If this bit is clear, the pixel format is supported by
/// a device driver or hardware.
#define PFD_GENERIC_FORMAT 0x00000040
/// The buffer uses RGBA pixels on a palette-managed device.
/// A logical palette is required to achieve the best results
/// for this pixel type. Colors in the palette should be specified
/// according to the values of the cRedBits, cRedShift, cGreenBits,
/// cGreenShift, cBluebits, and cBlueShift members.
/// The palette should be created and realized in the device context
/// before calling wglMakeCurrent.
#define PFD_NEED_PALETTE 0x00000080
/// Defined in the pixel format descriptors of hardware
/// that supports one hardware palette in 256-color mode only.
/// For such systems to use hardware acceleration,
/// the hardware palette must be in a fixed order (for example, 3-3-2)
/// when in RGBA mode or must match the logical palette when in
/// color-index mode. When this flag is set, you must call
/// SetSystemPaletteUse in your program to force a one-to-one mapping
/// of the logical palette and the system palette. If your OpenGL
/// hardware supports multiple hardware palettes and the device
/// driver can allocate spare hardware palettes for OpenGL,
/// this flag is typically clear.
/// This flag is not set in the generic pixel formats.
#define PFD_NEED_SYSTEM_PALETTE 0x00000100

/// The buffer is double-buffered. This flag and
/// PFD_SUPPORT_GDI are mutually exclusive in the current generic implementation.
#define PFD_DOUBLEBUFFER 0x00000001
/// The buffer is stereoscopic.
/// This flag is not supported in the current generic implementation.
#define PFD_STEREO 0x00000002
/// Indicates whether a device can swap individual layer
/// planes with pixel formats that include double-buffered
/// overlay or underlay planes. Otherwise all layer planes are
/// swapped together as a group. When this flag is set,
/// wglSwapLayerBuffers is supported.
#define PFD_SWAP_LAYER_BUFFERS 0x00000800
/// The requested pixel format can either have or not have a depth buffer.
/// To select a pixel format without a depth buffer,
/// you must specify this flag. The requested pixel format
/// can be with or without a depth buffer. Otherwise, only pixel formats
/// with a depth buffer are considered.
#define PFD_DEPTH_DONTCARE 0x20000000
/// The requested pixel format can be either single- or double-buffered.
#define PFD_DOUBLEBUFFER_DONTCARE 0x40000000
/// The requested pixel format can be either monoscopic or stereoscopic.
#define PFD_STEREO_DONTCARE 0x80000000
/// Specifies the content of the back buffer in the
/// double-buffered main color plane following a buffer swap.
/// Swapping the color buffers causes the content of the back buffer
/// to be copied to the front buffer. The content of the back buffer
/// is not affected by the swap. PFD_SWAP_COPY is a hint only
/// and might not be provided by a driver.
#define PFD_SWAP_COPY 0x00000400
/// Specifies the content of the back buffer in the double-buffered
/// main color plane following a buffer swap. Swapping the color buffers
/// causes the exchange of the back buffer's content with the
/// front buffer's content. Following the swap, the back buffer's
/// content contains the front buffer's content before the swap.
/// PFD_SWAP_EXCHANGE is a hint only and might not be provided by a driver.
#define PFD_SWAP_EXCHANGE 0x00000200
/// RGBA pixels. Each pixel has four components
/// in this order: red, green, blue, and alpha.
#define PFD_TYPE_RGBA 0
/// Color-index pixels. Each pixel uses a color-index value.
#define PFD_TYPE_COLORINDEX 1

#define PFD_MAIN_PLANE     0
#define PFD_OVERLAY_PLANE  1
#define PFD_UNDERLAY_PLANE (-1)

namespace impl {
    [[maybe_unused]]
    internal b32 _win32_library_load(
        const wchar_t* module_name,
        HMODULE* out_module
    );

    [[maybe_unused]]
    internal b32 _win32_library_load_trace(
        const wchar_t* module_name,
        HMODULE* out_module,
        const char* function,
        const char* file,
        i32 line
    );

    [[maybe_unused]]
    internal void _win32_library_free( HMODULE module );

    [[maybe_unused]]
    internal void _win32_library_free_trace(
        HMODULE module,
        const char* function,
        const char* file,
        i32 line
    );

    typedef BOOL (*SetProcessDpiAwarenessContext_fn)( DPI_AWARENESS_CONTEXT );
    [[maybe_unused]]
    global SetProcessDpiAwarenessContext_fn in_SetProcessDpiAwarenessContext = nullptr;

    typedef UINT (*GetDpiForSystem_fn)();
    [[maybe_unused]]
    global GetDpiForSystem_fn in_GetDpiForSystem = nullptr;

    typedef BOOL (*AdjustWindowRectExForDpi_fn)( LPRECT, DWORD, BOOL, DWORD, UINT );
    [[maybe_unused]]
    global AdjustWindowRectExForDpi_fn in_AdjustWindowRectExForDpi = nullptr;

    typedef DWORD (*XInputGetState_fn)( DWORD, XINPUT_STATE* );
    [[maybe_unused]]
    global XInputGetState_fn in_XInputGetState = nullptr;

    typedef DWORD (*XInputSetState_fn)( DWORD, XINPUT_VIBRATION* );
    [[maybe_unused]]
    global XInputSetState_fn in_XInputSetState = nullptr;

    typedef HGLRC (*wglCreateContext_fn)(HDC);
    [[maybe_unused]]
    global wglCreateContext_fn in_wglCreateContext = nullptr;

    typedef BOOL (*wglMakeCurrent_fn)(HDC, HGLRC);
    [[maybe_unused]]
    global wglMakeCurrent_fn in_wglMakeCurrent = nullptr;

    typedef BOOL (*wglDeleteContext_fn)(HGLRC);
    [[maybe_unused]]
    global wglDeleteContext_fn in_wglDeleteContext = nullptr;

    typedef PROC (*wglGetProcAddress_fn)(LPCSTR);
    [[maybe_unused]]
    global wglGetProcAddress_fn in_wglGetProcAddress = nullptr;

    typedef HGLRC (*wglCreateContextAttribsARB_fn)(HDC, HGLRC, const int*);
    [[maybe_unused]]
    global wglCreateContextAttribsARB_fn in_wglCreateContextAttribsARB = nullptr;

    typedef int (*DescribePixelFormat_fn)(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
    [[maybe_unused]]
    global DescribePixelFormat_fn in_DescribePixelFormat = nullptr;

    typedef int (*ChoosePixelFormat_fn)(HDC, const PIXELFORMATDESCRIPTOR*);
    [[maybe_unused]]
    global ChoosePixelFormat_fn in_ChoosePixelFormat = nullptr;

    typedef BOOL (*SetPixelFormat_fn)(HDC, int, const PIXELFORMATDESCRIPTOR*);
    [[maybe_unused]]
    global SetPixelFormat_fn in_SetPixelFormat = nullptr;

    typedef BOOL (*SwapBuffers_fn)(HDC);
    [[maybe_unused]]
    global SwapBuffers_fn in_SwapBuffers = nullptr;

    typedef void (*XInputEnable_fn)( BOOL );
    internal void XInputEnableStub( BOOL enable ) { SM_UNUSED( enable ); }
    [[maybe_unused]]
    global XInputEnable_fn in_XInputEnable = XInputEnableStub;

} // namespace impl

#if defined(LD_LOGGING)
    #define win32_library_load( module_name, out_module )\
        ::impl::_win32_library_load_trace(\
            module_name,\
            out_module,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define win32_library_free( module )\
        ::impl::_win32_library_free_trace(\
            module,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define win32_library_load( module_name, out_module ) \
        ::impl::_win32_library_load( module_name, out_module )
    #define win32_library_free( module )\
        ::impl::_win32_library_free( module )
#endif

#define SetProcessDpiAwarenessContext ::impl::in_SetProcessDpiAwarenessContext
#define GetDpiForSystem               ::impl::in_GetDpiForSystem
#define AdjustWindowRectExForDpi      ::impl::in_AdjustWindowRectExForDpi
#define XInputGetState                ::impl::in_XInputGetState
#define XInputSetState                ::impl::in_XInputSetState
#define XInputEnable                  ::impl::in_XInputEnable
#define wglCreateContext              ::impl::in_wglCreateContext
#define wglMakeCurrent                ::impl::in_wglMakeCurrent
#define wglDeleteContext              ::impl::in_wglDeleteContext
#define wglGetProcAddress             ::impl::in_wglGetProcAddress
#define wglCreateContextAttribsARB    ::impl::in_wglCreateContextAttribsARB
#define DescribePixelFormat           ::impl::in_DescribePixelFormat
#define ChoosePixelFormat             ::impl::in_ChoosePixelFormat
#define SetPixelFormat                ::impl::in_SetPixelFormat
#define SwapBuffers                   ::impl::in_SwapBuffers

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB               0x2093
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB                 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define ERROR_INVALID_VERSION_ARB                 0x2095
#define ERROR_INVALID_PROFILE_ARB                 0x2096

[[maybe_unused]]
internal void* win32_proc_address(
    HMODULE module,
    const char* proc_name
);
[[maybe_unused]]
internal void* win32_proc_address_required(
    HMODULE module,
    const char* proc_name
);

#endif // if platform windows
#endif // header guard