#if !defined(PLATFORM_WIN32_HPP)
#define PLATFORM_WIN32_HPP
/**
 * Description:  Platform Win32
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 08, 2023
*/
#include "defines.h"
#if defined(LD_PLATFORM_WINDOWS)

#include "core/audio.h"
#include "core/logging.h"
#include "core/library.h"
#include "platform/platform.h"

#define NOGDI
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <xinput.h>
#include <dsound.h>

struct Win32MutexHandle {
    HANDLE handle;
};
STATIC_ASSERT( sizeof(Win32MutexHandle) == MUTEX_HANDLE_SIZE );

struct Win32SemaphoreHandle {
    HANDLE handle;
};
STATIC_ASSERT( sizeof(Win32SemaphoreHandle) == SEMAPHORE_HANDLE_SIZE );

struct Win32FileHandle {
    HANDLE handle;
};
STATIC_ASSERT( sizeof(Win32FileHandle) == FILE_HANDLE_SIZE );

struct Win32ThreadHandle {
    HANDLE       thread_handle;
    ThreadProcFN thread_proc;
    void*        thread_proc_user_params;
    DWORD        thread_id;
};

struct Win32DirectSound {
    LPDIRECTSOUND handle;
    LPDIRECTSOUNDBUFFER hardware_handle;
    LPDIRECTSOUNDBUFFER buffer;

    u32 running_sample_index;
};
#define LIBRARY_COUNT (5)
struct Win32Platform {
    Platform platform;
    struct Win32Window {
        HWND handle;
        HDC  device_context;
    } window;
    struct Win32Cursor {
        CursorStyle style;
        b32         is_visible;
    } cursor;
    HINSTANCE instance;

    union {
        struct {
            LibraryHandle lib_user32;
            LibraryHandle lib_xinput;
            LibraryHandle lib_gl;
            LibraryHandle lib_gdi32;
            LibraryHandle lib_dsound;
        };
        LibraryHandle libraries[LIBRARY_COUNT];
    };

    Win32DirectSound direct_sound;

    LARGE_INTEGER performance_frequency;
    LARGE_INTEGER performance_counter;

    Win32ThreadHandle       xinput_polling_thread;
    PlatformSemaphoreHandle xinput_polling_thread_semaphore;
    u32 event_pump_count;
};


global const char* WIN32_VULKAN_EXTENSIONS[] = {
    "VK_KHR_win32_surface"
};
b32 win32_load_user32( Win32Platform* platform );
b32 win32_load_xinput( Win32Platform* platform );
b32 win32_load_opengl( Win32Platform* platform );
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
            "[NOTE WIN32  | {cc}() | {cc}:{i}] " format,\
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
            "[INFO WIN32  | {cc}() | {cc}:{i}] " format,\
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
            "[DEBUG WIN32 | {cc}() | {cc}:{i}] " format,\
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
            "[WARN WIN32  | {cc}() | {cc}:{i}] " format,\
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
            "[ERROR WIN32 | {cc}() | {cc}:{i}] " format,\
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
#endif

#define WIN32_LOG_FATAL( format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        LOG_COLOR_RED,\
        LOG_FLAG_NEW_LINE | LOG_FLAG_ALWAYS_PRINT,\
        "[FATAL WIN32 | {cc}() | {cc}:{i}] " format,\
        __FUNCTION__,\
        __FILE__,\
        __LINE__,\
        ##__VA_ARGS__\
    )

DWORD win32_log_error( b32 present_message_box );

#define BLACK_BRUSH 4
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

#define DEFFUNC_( return_name, function_name, ... )\
    typedef return_name ( * function_name##FN )( __VA_ARGS__ );\
    [[maybe_unused]]\
    global function_name##FN in_##function_name = nullptr

#define DEFFUNC_STUB( return_name, function_name, return_value, ... )\
    typedef return_name ( * function_name##FN )( __VA_ARGS__ );\
    internal return_name function_name##_stub( __VA_ARGS__ ) { return return_value; }\
    [[maybe_unused]]\
    global function_name##FN in_##function_name = function_name##_stub

namespace impl {
    
    DEFFUNC_( BOOL, SetProcessDpiAwarenessContext, DPI_AWARENESS_CONTEXT );
    DEFFUNC_( UINT, GetDpiForSystem );
    DEFFUNC_( BOOL, AdjustWindowRectExForDpi, LPRECT, DWORD, BOOL, DWORD, UINT );
    DEFFUNC_( DWORD, XInputGetState, DWORD, XINPUT_STATE* );
    DEFFUNC_( DWORD, XInputSetState, DWORD, XINPUT_VIBRATION* );
    DEFFUNC_( HGDIOBJ, GetStockObject, int );
    DEFFUNC_( HGLRC, wglCreateContext, HDC );
    DEFFUNC_( BOOL, wglMakeCurrent, HDC, HGLRC );
    DEFFUNC_( BOOL, wglDeleteContext, HGLRC );
    DEFFUNC_( PROC, wglGetProcAddress, LPCSTR );
    DEFFUNC_( HGLRC, wglCreateContextAttribsARB, HDC, HGLRC, const int* );
    DEFFUNC_( int, DescribePixelFormat, HDC, int, UINT, LPPIXELFORMATDESCRIPTOR );
    DEFFUNC_( int, ChoosePixelFormat, HDC, const PIXELFORMATDESCRIPTOR* );
    DEFFUNC_( BOOL, SetPixelFormat, HDC, int, const PIXELFORMATDESCRIPTOR* );
    DEFFUNC_( BOOL, SwapBuffers, HDC );
    DEFFUNC_STUB( void, XInputEnable, , BOOL );
    DEFFUNC_( HRESULT WINAPI, DirectSoundCreate, LPGUID, LPDIRECTSOUND*, LPUNKNOWN );
    DEFFUNC_( HANDLE, LoadImageA, HINSTANCE, LPCSTR, UINT, int, int, UINT );
    DEFFUNC_( LONG_PTR, GetWindowLongPtrA, HWND, int );
    DEFFUNC_( LRESULT, DefWindowProcA, HWND, UINT, WPARAM, LPARAM );
    DEFFUNC_( BOOL, GetClientRect, HWND, LPRECT );
    DEFFUNC_( UINT, MapVirtualKeyA, UINT, UINT );
    DEFFUNC_( BOOL, DestroyWindow, HWND );
    DEFFUNC_( BOOL, PeekMessageA, LPMSG, HWND, UINT, UINT, UINT );
    DEFFUNC_( BOOL, TranslateMessage, const MSG* );
    DEFFUNC_( BOOL, DestroyIcon, HICON );
    DEFFUNC_( HDC, GetDC, HWND );
    DEFFUNC_( BOOL, ShowWindow, HWND, int );
    DEFFUNC_( LONG_PTR, SetWindowLongPtrA, HWND, int, LONG_PTR );
    DEFFUNC_( int, MessageBoxA, HWND, LPCSTR, LPCSTR, UINT );
    DEFFUNC_( LRESULT, DispatchMessageA, const MSG* );
    DEFFUNC_( BOOL, SetWindowTextA, HWND, LPCSTR );
    DEFFUNC_( int, GetWindowTextLengthA, HWND );
    DEFFUNC_( int, GetWindowTextA, HWND, LPSTR, int );
    DEFFUNC_( BOOL, SetCursorPos, int, int );
    DEFFUNC_( BOOL, ClientToScreen, HWND, LPPOINT );
    DEFFUNC_( int, ShowCursor, BOOL );
    DEFFUNC_( HCURSOR, SetCursor, HCURSOR );
    DEFFUNC_( HWND, CreateWindowExA, DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID );
    DEFFUNC_( HCURSOR, LoadCursorA, HINSTANCE, LPCSTR );
    DEFFUNC_( ATOM, RegisterClassExA, const WNDCLASSEXA* );
    DEFFUNC_( BOOL, AdjustWindowRectEx, LPRECT, DWORD, BOOL, DWORD );
    DEFFUNC_( int, GetSystemMetrics, int );

} // namespace impl

#define GetSystemMetrics              ::impl::in_GetSystemMetrics
#define AdjustWindowRectEx            ::impl::in_AdjustWindowRectEx
#define RegisterClassExA              ::impl::in_RegisterClassExA
#define LoadCursorA                   ::impl::in_LoadCursorA
#define CreateWindowExA               ::impl::in_CreateWindowExA
#define SetCursor                     ::impl::in_SetCursor
#define ShowCursor                    ::impl::in_ShowCursor
#define ClientToScreen                ::impl::in_ClientToScreen
#define SetCursorPos                  ::impl::in_SetCursorPos
#define GetWindowTextA                ::impl::in_GetWindowTextA
#define GetWindowTextLengthA          ::impl::in_GetWindowTextLengthA
#define SetWindowTextA                ::impl::in_SetWindowTextA
#define DispatchMessageA              ::impl::in_DispatchMessageA
#define MessageBoxA                   ::impl::in_MessageBoxA
#define SetWindowLongPtrA             ::impl::in_SetWindowLongPtrA
#define ShowWindow                    ::impl::in_ShowWindow
#define GetDC                         ::impl::in_GetDC
#define DestroyIcon                   ::impl::in_DestroyIcon
#define TranslateMessage              ::impl::in_TranslateMessage
#define PeekMessageA                  ::impl::in_PeekMessageA
#define DestroyWindow                 ::impl::in_DestroyWindow
#define MapVirtualKeyA                ::impl::in_MapVirtualKeyA
#define GetClientRect                 ::impl::in_GetClientRect
#define DefWindowProcA                ::impl::in_DefWindowProcA
#define GetWindowLongPtrA             ::impl::in_GetWindowLongPtrA
#define LoadImageA                    ::impl::in_LoadImageA
#define DirectSoundCreate             ::impl::in_DirectSoundCreate
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
#define GetStockObject                ::impl::in_GetStockObject

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

#endif // if platform windows
#endif // header guard
