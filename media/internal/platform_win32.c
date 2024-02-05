/**
 * Description:  Media Win32 Functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 28, 2023
*/
#include "shared/defines.h"

#if defined(LD_PLATFORM_WINDOWS)
#include "media/input.h"
#include "media/surface.h"
#include "media/audio.h"
#include "media/internal/logging.h"

#include "core/memory.h"
#include "core/string.h"
#include "core/thread.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <hidusage.h>
#include <xinput.h>
#include <psapi.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <dshow.h>
#include <audioclient.h>

typedef struct Win32Surface {
    HWND  hwnd;
    DWORD dwStyle;
    DWORD dwExStyle;
    char name[MEDIA_SURFACE_NAME_CAPACITY];
    u32  name_len;
    i32 width, height;
    MediaSurfaceFlags flags;
    MediaSurfaceGraphicsBackend backend;
    MediaSurfaceCallbackFN* callback;
    void* callback_params;
    ATOM class_atom;
    WINDOWPLACEMENT placement;
    union {
        struct {
            HDC hdc;
            HGLRC glrc;
        } gl;
    };
} Win32Surface;
static_assert( sizeof(Win32Surface) <= sizeof(MediaSurface) );
#define get_surface(surface) Win32Surface* win32_surface = (Win32Surface*)surface

typedef struct Win32AudioContext {
    IAudioClient*        client;
    IAudioRenderClient*  render_client;
    IMMDeviceEnumerator* device_enumerator;
    IMMDevice*           device;

    WAVEFORMATEX format;

    UINT32 buffer_frame_count;
    u32    buffer_size;

    HMODULE OLE32;
} Win32AudioContext;
static_assert( sizeof(Win32AudioContext) <= sizeof(MediaAudioContext) );
#define get_audio(context) Win32AudioContext* ctx = (Win32AudioContext*)in_ctx

global b32 global_xinput_gamepad_active[XUSER_MAX_COUNT] = {};
global XINPUT_VIBRATION global_xinput_vibration[XUSER_MAX_COUNT] = {};

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB               0x2093
#define WGL_CONTEXT_FLAGS_ARB                     0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB                 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB          0x20A9
#define ERROR_INVALID_VERSION_ARB                 0x2095
#define ERROR_INVALID_PROFILE_ARB                 0x2096

#define declare( ret, prot, ... )\
    typedef ret ___internal_##prot##FN( __VA_ARGS__ );\
    global ___internal_##prot##FN* ___internal_##prot = NULL

// NOTE(alicia): function declarations!

// User32

declare( ATOM, RegisterClassExA, const WNDCLASSEXA* unnamedParam1 );
#define RegisterClassExA ___internal_RegisterClassExA

declare( BOOL, UnregisterClassA, LPCSTR lpClassName, HINSTANCE hInstance );
#define UnregisterClassA ___internal_UnregisterClassA

declare( BOOL, AdjustWindowRectEx,
    LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle );
#define AdjustWindowRectEx ___internal_AdjustWindowRectEx

declare( BOOL, GetClientRect, HWND hwnd, LPRECT lpRect );
#define GetClientRect ___internal_GetClientRect

declare(
    HWND, CreateWindowExA,
    DWORD dwExStyle, LPCSTR lpClassName,
    LPCSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight,
    HWND hwndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam );
#define CreateWindowExA ___internal_CreateWindowExA

declare( BOOL, DestroyWindow, HWND hwnd );
#define DestroyWindow ___internal_DestroyWindow

declare( BOOL, SetWindowTextA, HWND hwnd, LPCSTR lpString );
#define SetWindowTextA ___internal_SetWindowTextA

declare(
    BOOL, SetWindowPos, HWND hwnd, HWND hwndInsertAfter,
    int X, int Y, int cx, int cy, UINT uFlags );
#define SetWindowPos ___internal_SetWindowPos

declare( BOOL, GetWindowPlacement, HWND hwnd, WINDOWPLACEMENT* lpwndpl );
#define GetWindowPlacement ___internal_GetWindowPlacement

declare( BOOL, SetWindowPlacement, HWND hwnd, const WINDOWPLACEMENT* lpwndpl );
#define SetWindowPlacement ___internal_SetWindowPlacement

declare( BOOL, RegisterRawInputDevices,
    PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize );
#define RegisterRawInputDevices ___internal_RegisterRawInputDevices

declare(
    UINT, GetRawInputData,
    HRAWINPUT hRawInput, UINT uiCommand,
    LPVOID pData, PUINT pcbSize, UINT cbSizeHeader );
#define GetRawInputData ___internal_GetRawInputData

declare( LONG_PTR, SetWindowLongPtrA, HWND hwnd, int nIndex, LONG_PTR dwNewLong );
#define SetWindowLongPtrA ___internal_SetWindowLongPtrA

declare( LONG_PTR, GetWindowLongPtrA, HWND hwnd, int nIndex );
#define GetWindowLongPtrA ___internal_GetWindowLongPtrA

declare(
    BOOL, PeekMessageA, LPMSG lpMsg,
    HWND hwnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );
#define PeekMessageA ___internal_PeekMessageA

declare( BOOL, TranslateMessage, const MSG* lpMsg );
#define TranslateMessage ___internal_TranslateMessage

declare( LRESULT, DispatchMessageA, const MSG* lpMsg );
#define DispatchMessageA ___internal_DispatchMessageA

declare( LRESULT, DefWindowProcA, HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam );
#define DefWindowProcA ___internal_DefWindowProcA

declare( HMONITOR, MonitorFromWindow, HWND hwnd, DWORD dwFlags );
#define MonitorFromWindow ___internal_MonitorFromWindow

declare( HMONITOR, MonitorFromPoint, POINT pt, DWORD dwFlags );
#define MonitorFromPoint ___internal_MonitorFromPoint

declare( BOOL, GetMonitorInfoA, HMONITOR hMonitor, LPMONITORINFO lpmi );
#define GetMonitorInfoA ___internal_GetMonitorInfoA

declare( BOOL, ClientToScreen, HWND hwnd, LPPOINT lpPoint );
#define ClientToScreen ___internal_ClientToScreen

declare( int, ShowCursor, BOOL bShow );
#define ShowCursor ___internal_ShowCursor

declare( BOOL, SetCursorPos, int X, int Y );
#define SetCursorPos ___internal_SetCursorPos

declare( HDC, GetDC, HWND hwnd );
#define GetDC ___internal_GetDC

declare( int, ReleaseDC, HWND hwnd, HDC hdc );
#define ReleaseDC ___internal_ReleaseDC

declare( BOOL, ShowWindow, HWND hwnd, int nCmdShow );
#define ShowWindow ___internal_ShowWindow

declare( UINT, MapVirtualKeyA, UINT uCode, UINT uMapType );
#define MapVirtualKeyA ___internal_MapVirtualKeyA

declare( int, MessageBoxA, HWND hwnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType );
#define MessageBoxA ___internal_MessageBoxA

// Gdi32

declare( HGDIOBJ, GetStockObject, int i );
#define GetStockObject ___internal_GetStockObject

declare(
    int, DescribePixelFormat, HDC hdc,
    int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd );
#define DescribePixelFormat ___internal_DescribePixelFormat

declare( int, ChoosePixelFormat, HDC hdc, const PIXELFORMATDESCRIPTOR* ppfd );
#define ChoosePixelFormat ___internal_ChoosePixelFormat

declare( BOOL, SetPixelFormat, HDC hdc, int format, const PIXELFORMATDESCRIPTOR* ppfd );
#define SetPixelFormat ___internal_SetPixelFormat

declare( BOOL, SwapBuffers, HDC unnamedParam1 );
#define SwapBuffers ___internal_SwapBuffers

// wgl

declare( HGLRC, wglCreateContext, HDC unnamedParam1 );
#define wglCreateContext ___internal_wglCreateContext

declare( BOOL, wglMakeCurrent, HDC unnamedParam1, HGLRC unnamedParam2 );
#define wglMakeCurrent ___internal_wglMakeCurrent

declare( BOOL, wglDeleteContext, HGLRC unnamedParam1 );
#define wglDeleteContext ___internal_wglDeleteContext

declare( HGLRC, wglGetCurrentContext, void );
#define wglGetCurrentContext ___internal_wglGetCurrentContext

declare( PROC, wglGetProcAddress, LPCSTR unnamedParam1 );
#define wglGetProcAddress ___internal_wglGetProcAddress

declare(
    HGLRC, wglCreateContextAttribsARB,
    HDC hdc, HGLRC hShareContext, const int* attribList );
#define wglCreateContextAttribsARB ___internal_wglCreateContextAttribsARB

declare( BOOL, wglSwapIntervalEXT, int interval );
#define wglSwapIntervalEXT ___internal_wglSwapIntervalEXT

// Dwmapi

declare(
    HRESULT, DwmSetWindowAttribute,
    HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute );
#define DwmSetWindowAttribute ___internal_DwmSetWindowAttribute

// xinput

declare( DWORD, XInputGetState, DWORD dwUserIndex, XINPUT_STATE* pState );
#define XInputGetState ___internal_XInputGetState

declare( DWORD, XInputSetState, DWORD dwUserIndex, XINPUT_VIBRATION* pVibration );
#define XInputSetState ___internal_XInputSetState

declare( void, XInputEnable, BOOL enable );
#define XInputEnable ___internal_XInputEnable

// OLE32

declare( HRESULT, CoInitialize, LPVOID pvReserved );
#define CoInitialize ___internal_CoInitialize

declare( void, CoUninitialize, void );
#define CoUninitialize ___internal_CoUninitialize

declare(
    HRESULT, CoCreateInstance,
    REFCLSID rclsid, LPUNKNOWN pUnkOuter,
    DWORD dwClsContext, REFIID riid, LPVOID* ppv );
#define CoCreateInstance ___internal_CoCreateInstance

declare( void, CoTaskMemFree, LPVOID pv );
#define CoTaskMemFree ___internal_CoTaskMemFree

// NOTE(alicia): internal functions!

internal MONITORINFO get_monitor_info( HWND opt_hwnd );

internal DWORD ___win32_report_last_error( usize format_len, const char* format, ... );
#define win32_error( format, ... )\
    ___win32_report_last_error( sizeof(format " | "), format " | ", ##__VA_ARGS__ )

LRESULT win32_winproc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam );

MEDIA_API b32 media_surface_create(
    i32 width, i32 height, u32 name_len, const char* name, MediaSurfaceFlags flags,
    MediaSurfaceCallbackFN* opt_callback, void* opt_callback_params,
    MediaSurfaceGraphicsBackend backend, MediaSurface* out_surface
) {
    get_surface(out_surface);
    win32_surface->flags = flags;

    HMODULE module_handle = GetModuleHandleA(NULL);

    WNDCLASSEX window_class    = {};
    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.lpfnWndProc   = win32_winproc;
    window_class.hInstance     = module_handle;
    window_class.lpszClassName = "LiquidMediaWindowClass";
    window_class.hbrBackground = (HBRUSH)GetStockBrush( BLACK_BRUSH );

    ATOM class_atom = RegisterClassExA( &window_class );
    if( !class_atom ) {
        win32_error( "failed to register window class!" );
        return false;
    }

    win32_surface->class_atom = class_atom;

    u32 max_name_len = name_len;
    if( max_name_len >= MEDIA_SURFACE_NAME_CAPACITY - 1 ) {
        max_name_len = MEDIA_SURFACE_NAME_CAPACITY - 1;
    }
    memory_copy( win32_surface->name, name, max_name_len );
    win32_surface->name_len = max_name_len;
    win32_surface->name[win32_surface->name_len++] = 0;

    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;
    DWORD dwStyle   = 0;

    if( bitfield_check( flags, MEDIA_SURFACE_FLAG_RESIZEABLE ) ) {
        dwStyle = WS_OVERLAPPEDWINDOW;
    } else {
        dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    }

    win32_surface->dwStyle   = dwStyle;
    win32_surface->dwExStyle = dwExStyle;

    win32_surface->width  = width;
    win32_surface->height = height;

    RECT rect = {};
    rect.right  = width;
    rect.bottom = height;
    if( !AdjustWindowRectEx( &rect, dwStyle, false, dwExStyle ) ) {
        win32_error( "failed to get window rect!" );
        return false;
    }

    HWND   hwndParent = NULL;
    HMENU  hMenu      = NULL;
    LPVOID lpParam    = NULL;

    HWND hwnd = CreateWindowExA(
        dwExStyle, MAKEINTATOM( win32_surface->class_atom ),
        win32_surface->name, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        hwndParent, hMenu, module_handle, lpParam );
    if( !hwnd ) {
        win32_error( "failed to create window!" );
        return false;
    }
    win32_surface->hwnd = hwnd;

    win32_surface->backend = backend;
    switch( win32_surface->backend ) {
        case MEDIA_SURFACE_GRAPHICS_BACKEND_OPENGL: {
            win32_surface->gl.hdc = GetDC( win32_surface->hwnd );
        } break;
        default: break;
    }

    RAWINPUTDEVICE rid;
    rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid.usUsage     = HID_USAGE_GENERIC_MOUSE;
    rid.dwFlags     = RIDEV_INPUTSINK;
    rid.hwndTarget  = win32_surface->hwnd;
    RegisterRawInputDevices( &rid, 1, sizeof(rid) );

    win32_surface->callback        = opt_callback;
    win32_surface->callback_params = opt_callback_params;

    SetWindowLongPtrA( win32_surface->hwnd, GWLP_USERDATA, (LONG_PTR)win32_surface );

    #define DWMWA_USE_IMMERSIVE_DARK_MODE (20)

    BOOL dwm_value = TRUE;
    DwmSetWindowAttribute(
        win32_surface->hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
        &dwm_value, sizeof(dwm_value) );

    #undef DWMWA_USE_IMMERSIVE_DARK_MODE

    if( !bitfield_check( flags, MEDIA_SURFACE_FLAG_HIDDEN ) ) {
        ShowWindow( win32_surface->hwnd, SW_SHOW );
    }

    return true;
}
MEDIA_API void media_surface_destroy( MediaSurface* surface ) {
    get_surface(surface);

    // TODO(alicia): deinitialize graphics backend!

    switch( win32_surface->backend ) {
        case MEDIA_SURFACE_GRAPHICS_BACKEND_OPENGL: {
            if( win32_surface->hwnd && win32_surface->gl.glrc ) {
                assert( wglMakeCurrent( NULL, NULL ) );
                assert( wglDeleteContext( win32_surface->gl.glrc ) );
                ReleaseDC( win32_surface->hwnd, win32_surface->gl.hdc );
            }
        } break;
    }

    if( win32_surface->hwnd ) {
        if( DestroyWindow( win32_surface->hwnd ) != 0 ) {
            win32_error( "failed to destroy window!" );
        }
    }

    HMODULE module_handle = GetModuleHandleA(NULL);
    if( !UnregisterClassA(
        MAKEINTATOM( win32_surface->class_atom ),
        module_handle
    ) ) {
        win32_error( "failed to unregister window class!" );
    }

    memory_zero( win32_surface, sizeof(*win32_surface) );
}
MEDIA_API void media_surface_set_callback(
    MediaSurface* surface, MediaSurfaceCallbackFN* callback, void* params
) {
    get_surface(surface);
    win32_surface->callback        = callback;
    win32_surface->callback_params = params;
}
MEDIA_API void media_surface_clear_callback( MediaSurface* surface ) {
    get_surface(surface);

    win32_surface->callback        = NULL;
    win32_surface->callback_params = NULL;
}
MEDIA_API void media_surface_set_name(
    MediaSurface* surface, const char* name, u32 len
) {
    get_surface(surface);

    u32 max_name_len = len;
    if( max_name_len >= MEDIA_SURFACE_NAME_CAPACITY - 1 ) {
        max_name_len = MEDIA_SURFACE_NAME_CAPACITY - 1;
    }
    memory_copy( win32_surface->name, name, max_name_len );
    win32_surface->name_len = max_name_len;
    win32_surface->name[win32_surface->name_len++] = 0;

    if( !SetWindowTextA( win32_surface->hwnd, win32_surface->name ) ) {
        win32_error( "failed to set window title!" );
    }
}
MEDIA_API usize media_surface_query_name(
    MediaSurface* surface, char* buffer, u32 buffer_size
) {
    get_surface(surface);

    if( !buffer ) {
        return win32_surface->name_len;
    }

    u32 max_copy = buffer_size;
    if( max_copy >= win32_surface->name_len ) {
        max_copy = win32_surface->name_len;
    }

    memory_copy( buffer, win32_surface->name, max_copy );

    return win32_surface->name_len - max_copy;
}
MEDIA_API void media_surface_set_dimensions(
    MediaSurface* surface, i32 new_width, i32 new_height
) {
    if( media_surface_query_fullscreen( surface ) ) {
        return;
    }

    get_surface(surface);
    RECT rect = {};
    rect.right  = new_width;
    rect.bottom = new_height;

    AdjustWindowRectEx(
        &rect, win32_surface->dwExStyle, FALSE, win32_surface->dwExStyle );

    SetWindowPos(
        win32_surface->hwnd, NULL, 0, 0,
        rect.right - rect.left, rect.bottom - rect.top,
        SWP_NOMOVE | SWP_NOREPOSITION );
}
MEDIA_API void media_surface_query_dimensions(
    MediaSurface* surface, i32* out_width, i32* out_height
) {
    get_surface(surface);
    *out_width  = win32_surface->width;
    *out_height = win32_surface->height;
}
MEDIA_API void media_surface_set_fullscreen(
    MediaSurface* surface, b32 is_fullscreen
) {
    if( media_surface_query_fullscreen( surface ) == is_fullscreen ) {
        return;
    }
    get_surface(surface);

    if( is_fullscreen ) {
        bitfield_clear( win32_surface->flags, MEDIA_SURFACE_FLAG_FULLSCREEN );
    } else {
        win32_surface->flags |= MEDIA_SURFACE_FLAG_FULLSCREEN;
    }

    if( is_fullscreen ) {
        memory_zero( &win32_surface->placement, sizeof(WINDOWPLACEMENT) );
        win32_surface->placement.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement( win32_surface->hwnd, &win32_surface->placement );

        win32_surface->dwStyle   = WS_POPUP;
        win32_surface->dwExStyle = 0;

        MONITORINFO monitor_info = get_monitor_info( win32_surface->hwnd );

        SetWindowLongPtrA( win32_surface->hwnd, GWL_STYLE, win32_surface->dwStyle );
        SetWindowLongPtrA( win32_surface->hwnd, GWL_EXSTYLE, win32_surface->dwExStyle );

        i32 x = monitor_info.rcMonitor.left;
        i32 y = monitor_info.rcMonitor.top;
        i32 w = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
        i32 h = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

        SetWindowPos(
            win32_surface->hwnd, HWND_TOP, x, y, w, h,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW );
    } else {
        if( bitfield_check( win32_surface->flags, MEDIA_SURFACE_FLAG_RESIZEABLE ) ) {
            win32_surface->dwStyle = WS_OVERLAPPEDWINDOW;
        } else {
            win32_surface->dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
        }
        win32_surface->dwExStyle = WS_EX_OVERLAPPEDWINDOW;

        SetWindowLongPtrA( win32_surface->hwnd, GWL_EXSTYLE, win32_surface->dwExStyle );
        SetWindowLongPtrA( win32_surface->hwnd, GWL_EXSTYLE, win32_surface->dwExStyle );

        SetWindowPlacement( win32_surface->hwnd, &win32_surface->placement );

        SetWindowPos(
            win32_surface->hwnd, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
    }
}
MEDIA_API b32 media_surface_query_fullscreen( MediaSurface* surface ) {
    get_surface(surface);
    return bitfield_check( win32_surface->flags, MEDIA_SURFACE_FLAG_FULLSCREEN );
}
MEDIA_API void media_surface_set_hidden( MediaSurface* surface, b32 is_hidden ) {
    get_surface(surface);
    if( is_hidden ) {
        win32_surface->flags |= MEDIA_SURFACE_FLAG_HIDDEN;
    } else {
        bitfield_clear( win32_surface->flags, MEDIA_SURFACE_FLAG_HIDDEN );
    }

    int nCmdShow = is_hidden ? SW_HIDE : SW_SHOW;

    ShowWindow( win32_surface->hwnd, nCmdShow );
}
MEDIA_API b32 media_surface_query_hidden( MediaSurface* surface ) {
    get_surface( surface );
    return bitfield_check( win32_surface->flags, MEDIA_SURFACE_FLAG_HIDDEN );
}
MEDIA_API void media_surface_center( MediaSurface* surface ) {
    if( media_surface_query_fullscreen( surface ) ) {
        return;
    }
    get_surface(surface);

    MONITORINFO monitor = get_monitor_info( win32_surface->hwnd );

    i32 w = win32_surface->width;
    i32 h = win32_surface->height;

    i32 mon_w = monitor.rcMonitor.right - monitor.rcMonitor.left;
    i32 mon_h = monitor.rcMonitor.bottom - monitor.rcMonitor.top;

    i32 x = 0, y = 0;

    i32 center_x = mon_w / 2;
    i32 center_y = mon_h / 2;

    x = center_x - ( w / 2 );
    y = center_y - ( h / 2 );

    x += monitor.rcMonitor.left;
    y += monitor.rcMonitor.top;

    SetWindowPos( win32_surface->hwnd, NULL, x, y, 0, 0, SWP_NOSIZE );
}
MEDIA_API void media_surface_cursor_center( MediaSurface* surface ) {
    get_surface(surface);

    POINT center = {};
    center.x = win32_surface->width / 2;
    center.y = win32_surface->height / 2;

    ClientToScreen( win32_surface->hwnd, &center );
    SetCursorPos( center.x, center.y );
}
MEDIA_API void media_surface_pump_events( MediaSurface* surface ) {
    get_surface(surface);
    MSG message = {};
    while( PeekMessageA( &message, win32_surface->hwnd, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &message );
        DispatchMessageA( &message );
    }
}

MEDIA_API b32 media_surface_gl_init( MediaSurface* surface ) {
    get_surface(surface);

    HMODULE OpenGL32 = GetModuleHandleA( "OpenGL32.DLL" );
    if( !OpenGL32 ) {
        OpenGL32 = LoadLibraryA( "OpenGL32.DLL" );
        if( !OpenGL32 ) {
            win32_error( "failed to open library 'OpenGL32'!" );
            return false;
        }
    }

    HMODULE Gdi32 = GetModuleHandleA( "Gdi32.DLL" );

    #define load_fn( dll, fn ) do {\
        if( !___internal_##fn ) {\
            ___internal_##fn = ( ___internal_##fn##FN *)GetProcAddress( dll, #fn );\
            if( !___internal_##fn ) {\
                win32_error( "failed to load fn '" #fn "' from '" #dll "'!" );\
            }\
        }\
    } while(0)

    #define wgl_load_fn( fn ) do {\
        if( !___internal_##fn ) {\
            ___internal_##fn = ( ___internal_##fn##FN *)wglGetProcAddress( #fn );\
            if( !___internal_##fn ) {\
                win32_error( "failed to load fn '" #fn "' from 'wgl'!" );\
            }\
        }\
    } while(0)

    load_fn( OpenGL32, wglCreateContext );
    load_fn( OpenGL32, wglMakeCurrent );
    load_fn( OpenGL32, wglDeleteContext );
    load_fn( OpenGL32, wglGetCurrentContext );
    load_fn( OpenGL32, wglGetProcAddress );

    load_fn( Gdi32, DescribePixelFormat );
    load_fn( Gdi32, ChoosePixelFormat );
    load_fn( Gdi32, SetPixelFormat );
    load_fn( Gdi32, SwapBuffers );

    PIXELFORMATDESCRIPTOR desired_pixel_format = {};
    u16 pixel_format_size = sizeof( PIXELFORMATDESCRIPTOR );
    desired_pixel_format.nSize      = pixel_format_size;
    desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
    desired_pixel_format.nVersion   = 1;
    desired_pixel_format.dwFlags    = PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    desired_pixel_format.cColorBits = 32;
    desired_pixel_format.cAlphaBits = 8;
    desired_pixel_format.iLayerType = PFD_MAIN_PLANE;

    HDC hdc = win32_surface->gl.hdc;

    i32 pixelFormatIndex = ChoosePixelFormat(
        hdc, &desired_pixel_format );

    PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
    DescribePixelFormat(
        hdc, pixelFormatIndex,
        pixel_format_size, &suggested_pixel_format
    );

    if( SetPixelFormat(
        hdc,
        pixelFormatIndex,
        &suggested_pixel_format
    ) == FALSE ) {
        win32_error( "failed to set pixel format!" );
        return false;
    }

    HGLRC temp = wglCreateContext( hdc );
    if(!temp) {
        win32_error( "failed to create opengl temp context!" );
        return false;
    }

    wglMakeCurrent( hdc, temp );

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, GL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB, GL_VERSION_MINOR,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,
        0
    };

    wgl_load_fn( wglCreateContextAttribsARB );

    win32_surface->gl.glrc = wglCreateContextAttribsARB(
        hdc, NULL, attribs );

    wglDeleteContext( temp );
    if( !win32_surface->gl.glrc ) {
        win32_error( "failed to create opengl context!" );
        return false;
    }
    wglMakeCurrent( hdc, win32_surface->gl.glrc );

    wgl_load_fn( wglSwapIntervalEXT );

    #undef load_fn
    #undef wgl_load_fn
    return true;
}
MEDIA_API void media_surface_gl_swap_buffers( MediaSurface* surface ) {
    get_surface(surface);
    SwapBuffers( win32_surface->gl.hdc );
}
MEDIA_API void media_surface_gl_swap_interval(
    MediaSurfaceHandle* surface_handle, int interval
) {
    unused(surface_handle);
    wglSwapIntervalEXT( interval );
}
MEDIA_API void* media_gl_load_proc( const char* function_name ) {
    void* result = (void*)wglGetProcAddress( function_name );
    if( !result ) {
        HMODULE OpenGL32 = GetModuleHandleA( "OpenGL32.DLL" );
        result = (void*)GetProcAddress( OpenGL32, function_name );
    }
    return result;
}

MEDIA_API b32 media_input_query_gamepad_state(
    u32 gamepad_index, MediaGamepadState* out_state
) {
    XINPUT_STATE state = {};
    // NOTE(alicia): this is to prevent several ms stall
    // when polling a gamepad that is currently inactive.
    if( !global_xinput_gamepad_active[gamepad_index] ) {
        return false;
    }
    if( XInputGetState( gamepad_index, &state ) != ERROR_SUCCESS ) {
        global_xinput_gamepad_active[gamepad_index] = false;
        return false;
    }

    XINPUT_GAMEPAD gamepad = state.Gamepad;

    out_state->buttons = gamepad.wButtons;
    out_state->buttons &= ~(
        MEDIA_GAMEPAD_EXT_BUTTON_TRIGGER_LEFT |
        MEDIA_GAMEPAD_EXT_BUTTON_TRIGGER_RIGHT );
    out_state->trigger_left  = gamepad.bLeftTrigger;
    out_state->trigger_right = gamepad.bRightTrigger;
    out_state->stick_left_x  = gamepad.sThumbLX;
    out_state->stick_left_y  = gamepad.sThumbLY;
    out_state->stick_right_x = gamepad.sThumbRX;
    out_state->stick_right_y = gamepad.sThumbRY;

    out_state->buttons |= ( ( gamepad.bLeftTrigger > 25 ) << 2 );
    out_state->buttons |= ( ( gamepad.bRightTrigger > 25 ) << 3 );

    return true;
}
MEDIA_API b32 media_input_set_gamepad_rumble(
    u32 gamepad_index, u16 motor_left, u16 motor_right
) {
    b32 is_active = global_xinput_gamepad_active[gamepad_index];
    if( !is_active ) {
        return false;
    }

    XINPUT_VIBRATION* gamepad_vibration = global_xinput_vibration + gamepad_index;
    gamepad_vibration->wLeftMotorSpeed  = motor_left;
    gamepad_vibration->wRightMotorSpeed = motor_right;

    XInputSetState( gamepad_index, gamepad_vibration );

    return is_active;
}
MEDIA_API void media_input_query_gamepad_rumble(
    u32 gamepad_index, u16* out_motor_left, u16* out_motor_right
) {
    XINPUT_VIBRATION* gamepad_vibration = global_xinput_vibration + gamepad_index;

    *out_motor_left  = gamepad_vibration->wLeftMotorSpeed;
    *out_motor_right = gamepad_vibration->wRightMotorSpeed;
}
MEDIA_API void media_input_set_cursor_visible( b32 is_visible ) {
    ShowCursor( is_visible ? TRUE : FALSE );
}

MEDIA_API MediaMessageBoxResult media_message_box_blocking(
    const char* title, const char* message,
    MediaMessageBoxType type, MediaMessageBoxOptions options
) {
    UINT uType = 0;
    
    switch( type ) {
        case MEDIA_MESSAGE_BOX_TYPE_INFO: {
            uType |= MB_ICONINFORMATION;
        } break;
        case MEDIA_MESSAGE_BOX_TYPE_WARNING: {
            uType |= MB_ICONWARNING;
        } break;
        case MEDIA_MESSAGE_BOX_TYPE_ERROR: {
            uType |= MB_ICONERROR;
        } break;
    }

    switch( options ) {
        case MEDIA_MESSAGE_BOX_OPTIONS_OK: {
            uType |= MB_OK;
        } break;
        case MEDIA_MESSAGE_BOX_OPTIONS_OK_CANCEL: {
            uType |= MB_OKCANCEL;
        } break;
        case MEDIA_MESSAGE_BOX_OPTIONS_YES_NO: {
            uType |= MB_YESNO;
        } break;
    }

    int result = MessageBoxA( NULL, message, title, uType );

    switch( result ) {
        case IDCANCEL: return MEDIA_MESSAGE_BOX_RESULT_CANCEL;
        case IDOK:     return MEDIA_MESSAGE_BOX_RESULT_OK;
        case IDYES:    return MEDIA_MESSAGE_BOX_RESULT_YES;
        case IDNO:     return MEDIA_MESSAGE_BOX_RESULT_NO;
        default: {
            if( !result ) {
                win32_error( "message box returned error!" );
            } else {
                // NOTE(alicia): should never happen
                panic();
            }
        } return MEDIA_MESSAGE_BOX_RESULT_ERROR;
    }
}

#define safe_call( fn ) do {\
    HRESULT hresult = fn;\
    if( FAILED( hresult ) ) {\
        media_log_error( "'" #fn "' failed!" );\
        return false;\
    }\
} while(0)

#define DeviceEnumeratorGetDefaultAudioEndpoint( dataFlow, role, ppEndpoint )\
ctx->device_enumerator->lpVtbl->GetDefaultAudioEndpoint( ctx->device_enumerator, dataFlow, role, ppEndpoint )

#define DeviceActivate( iid, dwClsCtx, pActivationParams, ppInterface )\
ctx->device->lpVtbl->Activate( ctx->device, iid, dwClsCtx, pActivationParams, ppInterface )

#define AudioClientInitialize( ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid )\
ctx->client->lpVtbl->Initialize( ctx->client, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid )

#define AudioClientGetBufferSize( pNumBufferFrames )\
ctx->client->lpVtbl->GetBufferSize( ctx->client, pNumBufferFrames )

#define AudioClientGetService( riid, ppv )\
ctx->client->lpVtbl->GetService( ctx->client, riid, ppv )

#define AudioClientStart()\
ctx->client->lpVtbl->Start( ctx->client )

#define AudioClientStop()\
ctx->client->lpVtbl->Stop( ctx->client )

#define AudioClientGetCurrentPadding( pNumPaddingFrames )\
ctx->client->lpVtbl->GetCurrentPadding( ctx->client, pNumPaddingFrames )

#define AudioRenderClientGetBuffer( NumFramesRequested, ppData )\
ctx->render_client->lpVtbl->GetBuffer( ctx->render_client, NumFramesRequested, ppData )

#define AudioRenderClientReleaseBuffer( NumFramesWritten, dwFlags )\
ctx->render_client->lpVtbl->ReleaseBuffer( ctx->render_client, NumFramesWritten, dwFlags )

MEDIA_API b32 media_audio_initialize( u64 buffer_length_ms, MediaAudioContext* out_ctx ) {
    Win32AudioContext* ctx = (Win32AudioContext*)out_ctx;
    
    ctx->OLE32 = LoadLibraryA( "OLE32.DLL" );
    if( !ctx->OLE32 ) {
        win32_error( "failed to load ole32.dll!" );
        return false;
    }

    #define load_fn( fn ) do {\
        fn = (___internal_##fn##FN*)GetProcAddress( ctx->OLE32, #fn );\
        if( !fn ) {\
            win32_error( "failed to load audio function '" #fn "'!" );\
            return false;\
        }\
    } while(0)

    load_fn( CoInitialize );
    load_fn( CoUninitialize );
    load_fn( CoCreateInstance );
    load_fn( CoTaskMemFree );

    // NOTE(alicia): initialize COM.
    safe_call( CoInitialize( NULL ) );
    safe_call( CoCreateInstance(
        &CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, &IID_IMMDeviceEnumerator,
        (void**)&ctx->device_enumerator ) );

    // NOTE(alicia): Get default device.
    safe_call( DeviceEnumeratorGetDefaultAudioEndpoint(
        eRender, eConsole, &ctx->device ) );

    // NOTE(alicia): Activate audio client.
    safe_call( DeviceActivate(
        &IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&ctx->client ) );

    // NOTE(alicia): Define audio buffer format.
    ctx->format.nChannels       = 2;
    ctx->format.wFormatTag      = WAVE_FORMAT_PCM;
    ctx->format.nSamplesPerSec  = 44100;
    ctx->format.wBitsPerSample  = 16;
    ctx->format.nBlockAlign     =
        ( ctx->format.nChannels * ctx->format.wBitsPerSample ) / 8;
    ctx->format.nAvgBytesPerSec =
        ctx->format.nSamplesPerSec * ctx->format.nBlockAlign;
    ctx->format.cbSize          = 0;

    // NOTE(alicia): Create audio buffer.
    #define REFTIMES_PER_MS (10000)
    REFTIME buffer_length_reftime = buffer_length_ms * REFTIMES_PER_MS;
    DWORD initialize_stream_flags =
        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;

    safe_call( AudioClientInitialize(
        AUDCLNT_SHAREMODE_SHARED, initialize_stream_flags,
        buffer_length_reftime, 0, &ctx->format, NULL ) );

    // NOTE(alicia): Calculate buffer size.   
    safe_call( AudioClientGetBufferSize( &ctx->buffer_frame_count ) );

    ctx->buffer_size = ctx->buffer_frame_count *
        ( ctx->format.nChannels * ( ctx->format.wBitsPerSample / 8 ) );

    // NOTE(alicia): Get render client.
    safe_call( AudioClientGetService(
        &IID_IAudioRenderClient, (void**)&ctx->render_client ) );

    // NOTE(alicia): Clear audio buffer.
    BYTE* buffer = NULL;
    safe_call( AudioRenderClientGetBuffer( ctx->buffer_frame_count, &buffer ) );

    memory_zero( buffer, ctx->buffer_size );

    safe_call( AudioRenderClientReleaseBuffer( ctx->buffer_frame_count, 0 ) );

    // NOTE(alicia): Start playing buffer.
    safe_call( AudioClientStart() );

    #undef REFTIMES_PER_MS
    #undef load_fn
    return true;
}
MEDIA_API void media_audio_shutdown( MediaAudioContext* in_ctx ) {
#if defined(LD_DEVELOPER_MODE)
    if( !media_audio_is_context_valid( in_ctx ) ) {
        return;
    }
#endif
    get_audio();

    #define com_release( punk ) do {\
        if( (punk) != NULL ) {\
            (punk)->lpVtbl->Release( (punk) );\
            (punk) = NULL;\
        }\
    } while(0)

    AudioClientStop();

    com_release( ctx->device_enumerator );
    com_release( ctx->device );
    com_release( ctx->client );
    com_release( ctx->render_client );

    CoUninitialize();

    FreeLibrary( ctx->OLE32 );

    memory_zero( in_ctx, sizeof(*in_ctx) );
}
MEDIA_API b32 media_audio_is_context_valid( MediaAudioContext* in_ctx ) {
    get_audio();
    return ctx->OLE32 != NULL;
}
MEDIA_API MediaAudioBufferFormat media_audio_query_buffer_format( MediaAudioContext* in_ctx ) {
    get_audio();
    MediaAudioBufferFormat format = {};

    format.channel_count       = ctx->format.nChannels;
    format.bits_per_sample     = ctx->format.wBitsPerSample;
    format.samples_per_second  = ctx->format.nSamplesPerSec;
    format.buffer_sample_count = ctx->buffer_frame_count;
    format.buffer_size         = ctx->buffer_size;

    return format;
}

MEDIA_API b32 media_audio_buffer_lock(
    MediaAudioContext* in_ctx, MediaAudioBuffer* out_buffer
) {
    get_audio();
    UINT32 buffer_frame_padding_count = 0;
    
    safe_call( AudioClientGetCurrentPadding( &buffer_frame_padding_count ) );
    if( buffer_frame_padding_count > ctx->buffer_frame_count ) {
        // TODO(alicia): err?
        return false;
    }

    UINT32 buffer_frames_to_request =
        ctx->buffer_frame_count - buffer_frame_padding_count;
    if( !buffer_frames_to_request ) {
        return false;
    }

    BYTE* buffer = NULL;
    safe_call( AudioRenderClientGetBuffer( buffer_frames_to_request, &buffer ) );

    if( !buffer ) {
        return false;
    }

    out_buffer->buffer       = buffer;
    out_buffer->sample_count = buffer_frames_to_request;
    out_buffer->buffer_size  = out_buffer->sample_count * ctx->format.nBlockAlign;

    return true;
}
MEDIA_API void media_audio_buffer_unlock(
    MediaAudioContext* in_ctx, MediaAudioBuffer* buffer
) {
    get_audio();
    AudioRenderClientReleaseBuffer( buffer->sample_count, 0 );
    memory_zero( buffer, sizeof(*buffer) );
}
MEDIA_API void media_audio_start( MediaAudioContext* in_ctx ) {
    get_audio();
    AudioClientStart();
}
MEDIA_API void media_audio_stop( MediaAudioContext* in_ctx ) {
    get_audio();
    AudioClientStop();
}

#undef safe_call
#undef DeviceEnumeratorGetDefaultAudioEndpoint
#undef DeviceActivate
#undef AudioClientInitialize
#undef AudioClientGetBufferSize
#undef AudioClientGetService
#undef AudioClientGetCurrentPadding
#undef AudioClientStart
#undef AudioClientStop
#undef AudioRenderClientGetBuffer
#undef AudioRenderClientReleaseBuffer

LRESULT win32_winproc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam ) {
    Win32Surface* win32_surface =
        (Win32Surface*)GetWindowLongPtrA( hwnd, GWLP_USERDATA );
    if( !win32_surface ) {
        goto skip_winproc;
    }

    MediaSurface* media_surface = (MediaSurface*)win32_surface;

    MediaSurfaceCallbackFN* callback = win32_surface->callback;
    void* callback_params = win32_surface->callback_params;

    if( !callback ) {
        goto skip_winproc;
    }

    #define TRANSITION_STATE_MASK (1 << 31)
    #define EXTENDED_KEY_MASK     (1 << 24)
    #define SCANCODE_MASK         (0x00FF0000)

    #define KEY_SHIFT_LEFT  (16)
    #define KEY_SHIFT_RIGHT (223)

    #define KEY_CONTROL_LEFT  (17)
    #define KEY_CONTROL_RIGHT (225)

    #define KEY_ALT_LEFT  (18)
    #define KEY_ALT_RIGHT (224)

    #define WM_CLOSE_RETURN_VALUE            (0)
    #define WM_ACTIVATEAPP_RETURN_VALUE      (0)
    #define WM_WINDOWPOSCHANGED_RETURN_VALUE (0)
    #define WM_MOUSEMOVE_RETURN_VALUE        (0)
    #define WM_MOUSEBUTTON_RETURN_VALUE      (0)
    #define WM_MOUSEWHEEL_RETURN_VALUE       (0)
    #define WM_KEY_RETURN_VALUE              (0)
    #define WM_INPUT_RETURN_VALUE            (0)

    #define s_call()\
        callback( media_surface, &data, callback_params )
    MediaSurfaceCallbackData data = {};

    switch( Msg ) {
        case WM_CLOSE: {
            data.type = MEDIA_SURFACE_CALLBACK_TYPE_CLOSE;
            s_call();
        } return WM_CLOSE_RETURN_VALUE;

        case WM_ACTIVATEAPP: {
            data.type = MEDIA_SURFACE_CALLBACK_TYPE_ACTIVATE;
            data.activate.is_active = wParam == TRUE;

            s_call();
        } return WM_ACTIVATEAPP_RETURN_VALUE;

        case WM_WINDOWPOSCHANGED: {
            i32 last_w = win32_surface->width;
            i32 last_h = win32_surface->height;

            RECT rect = {};
            if( GetClientRect( win32_surface->hwnd, &rect ) ) {

                i32 new_w = rect.right < 1 ? 1 : rect.right;
                i32 new_h = rect.bottom < 1 ? 1 : rect.bottom;

                if( new_w == last_w && new_h == last_h ) {
                    return WM_WINDOWPOSCHANGED_RETURN_VALUE;
                }

                data.type = MEDIA_SURFACE_CALLBACK_TYPE_RESOLUTION_CHANGE;

                data.resolution_change.old_width  = last_w;
                data.resolution_change.old_height = last_h;
                data.resolution_change.new_width  = new_w;
                data.resolution_change.new_height = new_h;

                win32_surface->width  = new_w;
                win32_surface->height = new_h;

                s_call();
            }

        } return WM_WINDOWPOSCHANGED_RETURN_VALUE;

        case WM_MOUSEMOVE: {
            RECT client = {};
            if( GetClientRect( win32_surface->hwnd, &client ) ) {
                i32 x = GET_X_LPARAM( lParam );
                i32 y = client.bottom - GET_Y_LPARAM( lParam );

                data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE;
                data.mouse_move.x = x;
                data.mouse_move.y = y;

                s_call();
            }
        } return WM_MOUSEMOVE_RETURN_VALUE;

        case WM_INPUT: {
            UINT dwSize = sizeof(RAWINPUT);
            BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData(
                (HRAWINPUT)lParam, RID_INPUT,
                lpb, &dwSize, sizeof(RAWINPUTHEADER) );

            RAWINPUT* raw = (RAWINPUT*)lpb;
            if( raw->header.dwType == RIM_TYPEMOUSE ) {
                i32 x_rel = raw->data.mouse.lLastX;
                i32 y_rel = raw->data.mouse.lLastY;

                data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_MOVE_RELATIVE;
                data.mouse_move.x = -x_rel;
                data.mouse_move.y = -y_rel;

                s_call();
            }
        } return WM_INPUT_RETURN_VALUE;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            b32 is_down =
                Msg == WM_LBUTTONDOWN ||
                Msg == WM_MBUTTONDOWN ||
                Msg == WM_RBUTTONDOWN;
            MediaMouseButton button;
            if( Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP ) {
                button = MEDIA_MOUSE_BUTTON_LEFT;
            } else if( Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONUP ) {
                button = MEDIA_MOUSE_BUTTON_RIGHT;
            } else if( Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONUP ) {
                button = MEDIA_MOUSE_BUTTON_MIDDLE;
            } else {
                return WM_MOUSEBUTTON_RETURN_VALUE;
            }

            data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_BUTTON;
            data.mouse_button.button  = button;
            data.mouse_button.is_down = is_down;

            s_call();
        } return WM_MOUSEBUTTON_RETURN_VALUE;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: {
            UINT value = GET_XBUTTON_WPARAM(wParam);
            b32 is_down = Msg == WM_XBUTTONDOWN;
            MediaMouseButton button = value + ( MEDIA_MOUSE_BUTTON_EXTRA_1 - 1);

            data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_BUTTON;
            data.mouse_button.button  = button;
            data.mouse_button.is_down = is_down;

            s_call();
        } return WM_MOUSEBUTTON_RETURN_VALUE;

        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            i32 delta = GET_WHEEL_DELTA_WPARAM( wParam );
            delta = -delta;

            data.type = MEDIA_SURFACE_CALLBACK_TYPE_MOUSE_WHEEL;
            data.mouse_wheel.value         = delta;
            data.mouse_wheel.is_horizontal = Msg == WM_MOUSEWHEEL;

            s_call();
        } return WM_MOUSEBUTTON_RETURN_VALUE;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            b32 previous_key_state = (lParam >> 30) == 1;
            if( previous_key_state ) {
                return DefWindowProcA( hwnd, Msg, wParam, lParam );
            }
            u8 keycode = wParam;

            if( bitfield_check( lParam, EXTENDED_KEY_MASK ) ) {
                if( keycode == KEY_CONTROL_LEFT ) {
                    keycode = KEY_CONTROL_RIGHT;
                } else if( keycode == KEY_ALT_LEFT ) {
                    keycode = KEY_ALT_RIGHT;
                }
            }

            if( keycode == KEY_SHIFT_LEFT ) {
                LPARAM scancode = (lParam & SCANCODE_MASK) >> 16;
                WPARAM new_vkcode = MapVirtualKeyA(
                    scancode,
                    MAPVK_VSC_TO_VK_EX
                );
                if( new_vkcode == VK_RSHIFT ) {
                    keycode = KEY_SHIFT_RIGHT;
                }
            }

            b32 is_down = !((lParam & TRANSITION_STATE_MASK) != 0);

            data.type = MEDIA_SURFACE_CALLBACK_TYPE_KEYBOARD_KEY;
            data.key.key     = keycode;
            data.key.is_down = is_down;

            s_call();
        } return WM_KEY_RETURN_VALUE;

        default: return DefWindowProcA( hwnd, Msg, wParam, lParam );
    }

    #undef s_call

skip_winproc:
    return DefWindowProcA( hwnd, Msg, wParam, lParam );
}

internal MONITORINFO get_monitor_info( HWND opt_hwnd ) {
    HMONITOR monitor = NULL;
    if( opt_hwnd ) {
        monitor = MonitorFromWindow( opt_hwnd, MONITOR_DEFAULTTONEAREST );
    } else {
        // TODO(alicia): cache last point window was at so app always opens there
        POINT pt = {};
        monitor = MonitorFromPoint( pt, MONITOR_DEFAULTTOPRIMARY );
    }
    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof( monitor_info );
    GetMonitorInfoA( monitor, &monitor_info );
    return monitor_info;
}

HRESULT DwmSetWindowAttribute_STUB(
    HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute
) {
    unused(hwnd);
    unused(dwAttribute);
    unused(pvAttribute);
    unused(cbAttribute);
    return 0;
}

void XInputEnable_STUB( BOOL enable ) {
    unused(enable);
}

internal int xinput_poll_thread( void* params ) {
    unused(params);

    loop {
        Sleep( MEDIA_WIN32_XINPUT_POLL_RATE );

#if defined(LD_DEVELOPER_MODE)
        if( !XInputGetState ) {
            continue;
        }
#endif

        for( DWORD i = 0; i < XUSER_MAX_COUNT; ++i ) {
            XINPUT_STATE dummy = {};
            DWORD result = XInputGetState( i, &dummy );

            global_xinput_gamepad_active[i] = result == ERROR_SUCCESS;
        }
    }
}

MEDIA_API void media_shutdown(void) {
    return;
}

MEDIA_API b32 media_initialize(void) {

    if( !thread_create( xinput_poll_thread, NULL ) ) {
        media_log_error( "failed to create xinput polling thread!" );
        return false;
    }

    #define open_dll( dll )\
        LoadLibraryA( #dll ".DLL" );\
    do {\
        if( ! dll ) {\
            win32_error( "failed to open library '" #dll "'!" );\
            return false;\
        }\
    } while(0)

    #define load_fn( dll, fn ) do {\
        ___internal_##fn = ( ___internal_##fn##FN *)GetProcAddress( dll, #fn );\
        if( !___internal_##fn ) {\
            win32_error( "failed to load fn '" #fn "' from '" #dll "'!" );\
            return false;\
        }\
    } while(0)

    HMODULE User32 = GetModuleHandleA( "USER32.DLL" );
    if( !User32 ) {
        User32 = open_dll( User32 );
    }
    HMODULE Gdi32 = open_dll( Gdi32 );

    HMODULE XInput = LoadLibraryA( "XINPUT1_4.DLL" );
    if( !XInput ) {
        XInput = LoadLibraryA( "XINPUT9_1_0.DLL" );
        if( !XInput ) {
            XInput = LoadLibraryA( "XINPUT1_3.DLL" );
            if( !XInput ) {
                win32_error( "failed to open any XInput library!" );
                return false;
            }
        }
    }

    HMODULE Dwmapi = LoadLibraryA( "Dwmapi.DLL" );

    load_fn( User32, RegisterClassExA );
    load_fn( User32, UnregisterClassA );
    load_fn( User32, AdjustWindowRectEx );
    load_fn( User32, GetClientRect );
    load_fn( User32, CreateWindowExA );
    load_fn( User32, DestroyWindow );
    load_fn( User32, SetWindowTextA );
    load_fn( User32, SetWindowPos );
    load_fn( User32, GetWindowPlacement );
    load_fn( User32, SetWindowPlacement );
    load_fn( User32, RegisterRawInputDevices );
    load_fn( User32, GetRawInputData );
    load_fn( User32, SetWindowLongPtrA );
    load_fn( User32, GetWindowLongPtrA );
    load_fn( User32, PeekMessageA );
    load_fn( User32, TranslateMessage );
    load_fn( User32, DispatchMessageA );
    load_fn( User32, DefWindowProcA );
    load_fn( User32, MonitorFromWindow );
    load_fn( User32, MonitorFromPoint );
    load_fn( User32, GetMonitorInfoA );
    load_fn( User32, ClientToScreen );
    load_fn( User32, ShowCursor );
    load_fn( User32, SetCursorPos );
    load_fn( User32, GetDC );
    load_fn( User32, ReleaseDC );
    load_fn( User32, ShowWindow );
    load_fn( User32, MapVirtualKeyA );
    load_fn( User32, MessageBoxA );

    load_fn( Gdi32, GetStockObject );

    load_fn( XInput, XInputGetState );
    load_fn( XInput, XInputSetState );

    XInputEnable = (___internal_XInputEnableFN*)GetProcAddress(
        XInput, "XInputEnable" );
    if( !XInputEnable ) {
        XInputEnable = XInputEnable_STUB;
    }

    DwmSetWindowAttribute =
        (___internal_DwmSetWindowAttributeFN*)GetProcAddress(
            Dwmapi, "DwmSetWindowAttribute" );
    if( !DwmSetWindowAttribute ) {
        DwmSetWindowAttribute = DwmSetWindowAttribute_STUB;
    }

    #undef open_dll
    #undef load_fn
    return true;
}

DWORD ___win32_report_last_error( usize format_len, const char* format, ... ) {
    #define WIN32_ERROR_BUFFER_MAX_CAPACITY (512)
    char error_buffer[WIN32_ERROR_BUFFER_MAX_CAPACITY] = {};

    StringBuffer error = {};
    error.c   = error_buffer;
    error.cap = WIN32_ERROR_BUFFER_MAX_CAPACITY;

    DWORD error_code = GetLastError();
    if( error_code == ERROR_SUCCESS ) {
        return error_code;
    }

    va_list va;
    va_start( va, format );
    string_buffer_fmt_cstr_va( &error, format_len, format, va );
    va_end( va );
    // NOTE(alicia): get rid of trailing null
    if( error.len ) {
        error.len--;
    }

    DWORD dwFlags =
        FORMAT_MESSAGE_FROM_SYSTEM    |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK;
    LPCVOID lpSource     = NULL;
    DWORD   dwLanguageId = 0;

    FormatMessageA(
        dwFlags, lpSource, error_code,
        dwLanguageId, error.c + error.len, error.cap,
        NULL );

    ___internal_media_log( LOGGING_LEVEL_ERROR, error.len, error.str );

    return error_code;
}

#endif /* Platform Windows */ 

