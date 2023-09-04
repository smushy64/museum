#if !defined(PLATFORM_WIN32_HPP)
#define PLATFORM_WIN32_HPP
/**
 * Description:  Platform Win32
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 08, 2023
*/
#include "defines.h"
#if defined(LD_PLATFORM_WINDOWS)

#include "core/ldlog.h"
#include "core/ldlibrary.h"
#include "ldplatform.h"

#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <xinput.h>
#include <dsound.h>

typedef struct Win32Thread {
    HANDLE        thread_handle;
    ThreadProcFN* thread_proc;
    void*         thread_proc_user_params;
    DWORD         thread_id;
} Win32Thread;

typedef struct Win32DirectSound {
    LPDIRECTSOUND handle;
    LPDIRECTSOUNDBUFFER hardware_handle;
    LPDIRECTSOUNDBUFFER buffer;

    u32 running_sample_index;
} Win32DirectSound;

typedef struct Win32Surface {
    HWND handle;
    HDC  device_context;
    ivec2 dimensions;
    DWORD dwStyle;
    DWORD dwExStyle;
    PlatformSurfaceOnResizeFN* on_resize;
    void* on_resize_user_params;
    PlatformSurfaceOnActivateFN*  on_activate;
    void* on_activate_user_params;
    PlatformSurfaceOnCloseFN*  on_close;
    void* on_close_user_params;

    WINDOWPLACEMENT placement;

    PlatformSurfaceCreateFlags creation_flags;
    PlatformSurfaceMode mode;
    b8 is_active;

    HGLRC glrc;
} Win32Surface;

#define LIBRARY_COUNT (5)
typedef struct Win32Platform {
    CursorStyle cursor_style;
    b32 cursor_visible;

    HINSTANCE instance;
    HICON icon;

    union {
        struct {
            DynamicLibrary lib_user32;
            DynamicLibrary lib_xinput;
            DynamicLibrary lib_gl;
            DynamicLibrary lib_gdi32;
            DynamicLibrary lib_dsound;
        };
        DynamicLibrary libraries[LIBRARY_COUNT];
    };

    Win32DirectSound direct_sound;

    LARGE_INTEGER performance_frequency;
    LARGE_INTEGER performance_counter;

    Win32Thread xinput_polling_thread;
    PlatformSemaphore* xinput_polling_thread_semaphore;
    u32 event_pump_count;
} Win32Platform;


global const char* WIN32_VULKAN_EXTENSIONS[] = {
    "VK_KHR_win32_surface"
};
b32 win32_load_user32( Win32Platform* platform );
b32 win32_load_xinput( Win32Platform* platform );
b32 win32_load_opengl( Win32Platform* platform );
LRESULT win32_winproc( HWND, UINT, WPARAM, LPARAM );

DWORD win32_log_error( b32 present_message_box );

#define DECLARE_WIN_FUNCTION( ret, fn, ... )\
    typedef ret fn##FN ( __VA_ARGS__ );\
    global fn##FN* ___internal_##fn = NULL

#define DECLARE_WIN_FUNCTION_STUB( ret, fn, ret_val, ... )\
    typedef ret fn##FN ( __VA_ARGS__ );\
    internal ret fn##_stub( __VA_ARGS__ ) { return ret_val; }\
    global fn##FN* ___internal_##fn = fn##_stub

DECLARE_WIN_FUNCTION( BOOL, SetWindowPlacement, HWND, WINDOWPLACEMENT* );
DECLARE_WIN_FUNCTION( BOOL, GetWindowPlacement, HWND, WINDOWPLACEMENT* );
DECLARE_WIN_FUNCTION( BOOL, GetMonitorInfoA, HMONITOR, LPMONITORINFO );
DECLARE_WIN_FUNCTION( HMONITOR, MonitorFromPoint, POINT, DWORD );
DECLARE_WIN_FUNCTION( HMONITOR, MonitorFromWindow, HWND, DWORD );
DECLARE_WIN_FUNCTION( int, GetDeviceCaps, HDC, int );
DECLARE_WIN_FUNCTION( DWORD, XInputGetState, DWORD, XINPUT_STATE* );
DECLARE_WIN_FUNCTION( DWORD, XInputSetState, DWORD, XINPUT_VIBRATION* );
DECLARE_WIN_FUNCTION( HGDIOBJ, GetStockObject, int );
DECLARE_WIN_FUNCTION( HGLRC, wglGetCurrentContext, void );
DECLARE_WIN_FUNCTION( HGLRC, wglCreateContext, HDC );
DECLARE_WIN_FUNCTION( BOOL, wglMakeCurrent, HDC, HGLRC );
DECLARE_WIN_FUNCTION( BOOL, wglDeleteContext, HGLRC );
DECLARE_WIN_FUNCTION( PROC, wglGetProcAddress, LPCSTR );
DECLARE_WIN_FUNCTION( HGLRC, wglCreateContextAttribsARB, HDC, HGLRC, const int* );
DECLARE_WIN_FUNCTION( int, DescribePixelFormat, HDC, int, UINT, LPPIXELFORMATDESCRIPTOR );
DECLARE_WIN_FUNCTION( int, ChoosePixelFormat, HDC, const PIXELFORMATDESCRIPTOR* );
DECLARE_WIN_FUNCTION( BOOL, SetPixelFormat, HDC, int, const PIXELFORMATDESCRIPTOR* );
DECLARE_WIN_FUNCTION( BOOL, SwapBuffers, HDC );
DECLARE_WIN_FUNCTION_STUB( void, XInputEnable, , BOOL foo __attribute__((__unused__)) );
DECLARE_WIN_FUNCTION( HRESULT WINAPI, DirectSoundCreate, LPGUID, LPDIRECTSOUND*, LPUNKNOWN );
DECLARE_WIN_FUNCTION( HANDLE, LoadImageA, HINSTANCE, LPCSTR, UINT, int, int, UINT );
DECLARE_WIN_FUNCTION( LONG_PTR, GetWindowLongPtrA, HWND, int );
DECLARE_WIN_FUNCTION( LRESULT, DefWindowProcA, HWND, UINT, WPARAM, LPARAM );
DECLARE_WIN_FUNCTION( BOOL, GetClientRect, HWND, LPRECT );
DECLARE_WIN_FUNCTION( UINT, MapVirtualKeyA, UINT, UINT );
DECLARE_WIN_FUNCTION( BOOL, DestroyWindow, HWND );
DECLARE_WIN_FUNCTION( BOOL, PeekMessageA, LPMSG, HWND, UINT, UINT, UINT );
DECLARE_WIN_FUNCTION( BOOL, TranslateMessage, const MSG* );
DECLARE_WIN_FUNCTION( BOOL, DestroyIcon, HICON );
DECLARE_WIN_FUNCTION( HDC, GetDC, HWND );
DECLARE_WIN_FUNCTION( BOOL, ShowWindow, HWND, int );
DECLARE_WIN_FUNCTION( LONG_PTR, SetWindowLongPtrA, HWND, int, LONG_PTR );
DECLARE_WIN_FUNCTION( int, MessageBoxA, HWND, LPCSTR, LPCSTR, UINT );
DECLARE_WIN_FUNCTION( LRESULT, DispatchMessageA, const MSG* );
DECLARE_WIN_FUNCTION( BOOL, SetWindowTextA, HWND, LPCSTR );
DECLARE_WIN_FUNCTION( int, GetWindowTextLengthA, HWND );
DECLARE_WIN_FUNCTION( int, GetWindowTextA, HWND, LPSTR, int );
DECLARE_WIN_FUNCTION( BOOL, SetCursorPos, int, int );
DECLARE_WIN_FUNCTION( BOOL, ClientToScreen, HWND, LPPOINT );
DECLARE_WIN_FUNCTION( int, ShowCursor, BOOL );
DECLARE_WIN_FUNCTION( HCURSOR, SetCursor, HCURSOR );
DECLARE_WIN_FUNCTION( HWND, CreateWindowExA, DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID );
DECLARE_WIN_FUNCTION( HCURSOR, LoadCursorA, HINSTANCE, LPCSTR );
DECLARE_WIN_FUNCTION( ATOM, RegisterClassExA, const WNDCLASSEXA* );
DECLARE_WIN_FUNCTION( BOOL, AdjustWindowRectEx, LPRECT, DWORD, BOOL, DWORD );
DECLARE_WIN_FUNCTION( int, GetSystemMetrics, int );
DECLARE_WIN_FUNCTION( BOOL, SetWindowPos, HWND, HWND, int, int, int, int, UINT );

#define SetWindowPlacement            ___internal_SetWindowPlacement
#define GetWindowPlacement            ___internal_GetWindowPlacement
#define GetMonitorInfoA               ___internal_GetMonitorInfoA
#define MonitorFromPoint              ___internal_MonitorFromPoint
#define MonitorFromWindow             ___internal_MonitorFromWindow
#define GetDeviceCaps                 ___internal_GetDeviceCaps
#define SetWindowPos                  ___internal_SetWindowPos
#define GetSystemMetrics              ___internal_GetSystemMetrics
#define AdjustWindowRectEx            ___internal_AdjustWindowRectEx
#define RegisterClassExA              ___internal_RegisterClassExA
#define LoadCursorA                   ___internal_LoadCursorA
#define CreateWindowExA               ___internal_CreateWindowExA
#define SetCursor                     ___internal_SetCursor
#define ShowCursor                    ___internal_ShowCursor
#define ClientToScreen                ___internal_ClientToScreen
#define SetCursorPos                  ___internal_SetCursorPos
#define GetWindowTextA                ___internal_GetWindowTextA
#define GetWindowTextLengthA          ___internal_GetWindowTextLengthA
#define SetWindowTextA                ___internal_SetWindowTextA
#define DispatchMessageA              ___internal_DispatchMessageA
#define MessageBoxA                   ___internal_MessageBoxA
#define SetWindowLongPtrA             ___internal_SetWindowLongPtrA
#define ShowWindow                    ___internal_ShowWindow
#define GetDC                         ___internal_GetDC
#define DestroyIcon                   ___internal_DestroyIcon
#define TranslateMessage              ___internal_TranslateMessage
#define PeekMessageA                  ___internal_PeekMessageA
#define DestroyWindow                 ___internal_DestroyWindow
#define MapVirtualKeyA                ___internal_MapVirtualKeyA
#define GetClientRect                 ___internal_GetClientRect
#define DefWindowProcA                ___internal_DefWindowProcA
#define GetWindowLongPtrA             ___internal_GetWindowLongPtrA
#define LoadImageA                    ___internal_LoadImageA
#define DirectSoundCreate             ___internal_DirectSoundCreate
#define XInputGetState                ___internal_XInputGetState
#define XInputSetState                ___internal_XInputSetState
#define XInputEnable                  ___internal_XInputEnable
#define wglGetCurrentContext          ___internal_wglGetCurrentContext
#define wglCreateContext              ___internal_wglCreateContext
#define wglMakeCurrent                ___internal_wglMakeCurrent
#define wglDeleteContext              ___internal_wglDeleteContext
#define wglGetProcAddress             ___internal_wglGetProcAddress
#define wglCreateContextAttribsARB    ___internal_wglCreateContextAttribsARB
#define DescribePixelFormat           ___internal_DescribePixelFormat
#define ChoosePixelFormat             ___internal_ChoosePixelFormat
#define SetPixelFormat                ___internal_SetPixelFormat
#define SwapBuffers                   ___internal_SwapBuffers
#define GetStockObject                ___internal_GetStockObject

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

#if defined(LD_LOGGING)
    #define WIN32_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[WIN32 NOTE] " format,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            false, true,\
            LOG_COLOR_WHITE\
            "[WIN32 INFO] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            false, true,\
            LOG_COLOR_BLUE\
            "[WIN32 DEBUG] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[WIN32 WARN] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )
    #define WIN32_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            false, true,\
            LOG_COLOR_RED\
            "[WIN32 ERROR] " format\
            LOG_COLOR_RESET,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_NOTE_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            false, true,\
            LOG_COLOR_RESET\
            "[WIN32 NOTE | {cc}() | {cc}:{i}] " format,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_INFO_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_WHITE\
            "[WIN32 INFO | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_DEBUG_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_BLUE\
            "[WIN32 DEBUG | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
    #define WIN32_LOG_WARN_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_YELLOW\
            "[WIN32 WARN | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )

    #define WIN32_LOG_ERROR_TRACE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            false, true,\
            LOG_COLOR_RED\
            "[WIN32 ERROR | {cc}() | {cc}:{i}] " format\
            LOG_COLOR_RESET,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__,\
            ##__VA_ARGS__\
        )
        
#else
    #define WIN32_LOG_NOTE( format, ... ) unused(format)
    #define WIN32_LOG_INFO( format, ... ) unused(format)
    #define WIN32_LOG_DEBUG( format, ... ) unused(format)
    #define WIN32_LOG_WARN( format, ... ) unused(format)
    #define WIN32_LOG_ERROR( format, ... ) unused(format)

    #define WIN32_LOG_NOTE_TRACE( format, ... ) unused(format)
    #define WIN32_LOG_INFO_TRACE( format, ... ) unused(format)
    #define WIN32_LOG_DEBUG_TRACE( format, ... ) unused(format)
    #define WIN32_LOG_WARN_TRACE( format, ... ) unused(format)
    #define WIN32_LOG_ERROR_TRACE( format, ... ) unused(format)

#endif

#define WIN32_LOG_FATAL( format, ... ) \
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        true, true,\
        LOG_COLOR_RED\
        "[WIN32 FATAL | {cc}() | {cc}:{i}] " format\
        LOG_COLOR_RESET,\
        __FUNCTION__,\
        __FILE__,\
        __LINE__,\
        ##__VA_ARGS__\
    )


#endif // if platform windows
#endif // header guard
