// * Description:  Liquid Engine Win32 Platform Layer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 21, 2023
#include "shared/defines.h"
#if defined(LD_PLATFORM_WINDOWS)

#include "platform/api.h"
#include "shared/custom_cstd.h"

#undef CORE_API
#define CORE_API

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <xinput.h>
#include <intrin.h>
#include <hidusage.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <dshow.h>
#include <audioclient.h>
#include <mmreg.h>

#if defined(strcpy)
    #undef strcpy
#endif

// NOTE(alicia): globals

#define WIN32_ERROR_MESSAGE_BUFFER_SIZE (512)
global char  WIN32_ERROR_MESSAGE_BUFFER[WIN32_ERROR_MESSAGE_BUFFER_SIZE] = {};
global usize WIN32_ERROR_MESSAGE_LENGTH = 0;

global LARGE_INTEGER global_performance_counter;
global LARGE_INTEGER global_performance_frequency;

global PlatformInfo global_win32_info;

#if !defined(LD_HEADLESS)

global HINSTANCE global_instance;

#define WIN32_FULLSCREEN_DWSTYLE   WS_POPUP
#define WIN32_FULLSCREEN_DWEXSTYLE 0

#define WIN32_WINDOWED_RESIZEABLE_DWSTYLE\
    WS_OVERLAPPEDWINDOW

#define WIN32_WINDOWED_DWSTYLE\
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU

#define WIN32_WINDOWED_DWEXSTYLE\
    WS_EX_OVERLAPPEDWINDOW

#endif // if not headless

#define WIN32_DECLARE_FUNCTION( ret, fn, ... )\
    typedef ret ___internal_##fn##FN( __VA_ARGS__ );\
    global ___internal_##fn##FN* ___internal_##fn = NULL

#define WIN32_LOAD_FUNCTION( module, fn )\
    (___internal_##fn = (___internal_##fn##FN *)GetProcAddress( module, #fn ))

// NOTE(alicia): User32

#if !defined(LD_HEADLESS)

WIN32_DECLARE_FUNCTION( HWND, SetFocus, HWND hWnd );
#define SetFocus( hWnd ) ___internal_SetFocus( hWnd )

WIN32_DECLARE_FUNCTION(
    HANDLE, LoadImageA,
    HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad );
#define LoadImageA( hInst, name, type, cx, cy, fuLoad )\
    ___internal_LoadImageA( hInst, name, type, cx, cy, fuLoad )

WIN32_DECLARE_FUNCTION( BOOL, RegisterRawInputDevices, PCRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize );
#define RegisterRawInputDevices( pRawInputDevices, uiNumDevices, cbSize )\
    ___internal_RegisterRawInputDevices( pRawInputDevices, uiNumDevices, cbSize )

WIN32_DECLARE_FUNCTION( UINT, GetRawInputData, HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader );
#define GetRawInputData( hRawInput, uiCommand, pData, pcbSize, cbSizeHeader )\
    ___internal_GetRawInputData( hRawInput, uiCommand, pData, pcbSize, cbSizeHeader )

WIN32_DECLARE_FUNCTION(
    BOOL, SetWindowPlacement, HWND hWnd, WINDOWPLACEMENT* lpwndpl );
#define SetWindowPlacement( hWnd, lpwndpl )\
    ___internal_SetWindowPlacement( hWnd, lpwndpl )

WIN32_DECLARE_FUNCTION(
    BOOL, GetWindowPlacement, HWND hWnd, WINDOWPLACEMENT* lpwndpl );
#define GetWindowPlacement( hWnd, lpwndpl )\
    ___internal_GetWindowPlacement( hWnd, lpwndpl )

WIN32_DECLARE_FUNCTION(
    BOOL, GetMonitorInfoA, HMONITOR hMonitor, LPMONITORINFO lpmi );
#define GetMonitorInfoA( hMonitor, lpmi )\
    ___internal_GetMonitorInfoA( hMonitor, lpmi )

WIN32_DECLARE_FUNCTION(
    HMONITOR, MonitorFromPoint, POINT pt, DWORD dwFlags );
#define MonitorFromPoint( pt, dwFlags )\
    ___internal_MonitorFromPoint( pt, dwFlags )

WIN32_DECLARE_FUNCTION(
    HMONITOR, MonitorFromWindow, HWND hWnd, DWORD dwFlags );
#define MonitorFromWindow( hWnd, dwFlags )\
    ___internal_MonitorFromWindow( hWnd, dwFlags )

WIN32_DECLARE_FUNCTION(
    LONG_PTR, GetWindowLongPtrA, HWND hWnd, int nIndex );
#define GetWindowLongPtrA( hWnd, nIndex )\
    ___internal_GetWindowLongPtrA( hWnd, nIndex )

WIN32_DECLARE_FUNCTION(
    LONG_PTR, SetWindowLongPtrA, HWND hWnd, int nIndex, LONG_PTR dwNewLong );
#define SetWindowLongPtrA( hWnd, nIndex, dwNewLong )\
    ___internal_SetWindowLongPtrA( hWnd, nIndex, dwNewLong )

WIN32_DECLARE_FUNCTION(
    LRESULT, DefWindowProcA,
    HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
#define DefWindowProcA( hWnd, Msg, wParam, lParam )\
    ___internal_DefWindowProcA( hWnd, Msg, wParam, lParam )

WIN32_DECLARE_FUNCTION( BOOL, GetClientRect, HWND hWnd, LPRECT lpRect );
#define GetClientRect( hWnd, lpRect )\
    ___internal_GetClientRect( hWnd, lpRect )

WIN32_DECLARE_FUNCTION( UINT, MapVirtualKeyA, UINT uCode, UINT uMapType );
#define MapVirtualKeyA( uCode, uMapType )\
    ___internal_MapVirtualKeyA( uCode, uMapType )

WIN32_DECLARE_FUNCTION( BOOL, DestroyWindow, HWND hWnd );
#define DestroyWindow( hWnd ) ___internal_DestroyWindow( hWnd )

WIN32_DECLARE_FUNCTION(
    BOOL, PeekMessageA,
    LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin,
    UINT wMsgFilterMax, UINT wRemoveMsg );
#define PeekMessageA( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg )\
    ___internal_PeekMessageA(\
        lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg )

WIN32_DECLARE_FUNCTION(
    BOOL, TranslateMessage, const MSG* lpMsg );
#define TranslateMessage( lpMsg ) ___internal_TranslateMessage( lpMsg )

WIN32_DECLARE_FUNCTION( BOOL, DestroyIcon, HICON hIcon );
#define DestroyIcon( hIcon ) ___internal_DestroyIcon( hIcon )

WIN32_DECLARE_FUNCTION( HDC, GetDC, HWND hWnd );
#define GetDC( hWnd ) ___internal_GetDC( hWnd )

WIN32_DECLARE_FUNCTION( int, ReleaseDC, HWND hWnd, HDC hDC );
#define ReleaseDC( hWnd, hDC ) ___internal_ReleaseDC( hWnd, hDC )

WIN32_DECLARE_FUNCTION( BOOL, ShowWindow, HWND hWnd, int nCmdShow );
#define ShowWindow( hWnd, nCmdShow ) ___internal_ShowWindow( hWnd, nCmdShow )

WIN32_DECLARE_FUNCTION(
    int, MessageBoxA,
    HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType );
#define MessageBoxA( hWnd, lpText, lpCaption, uType )\
    ___internal_MessageBoxA( hWnd, lpText, lpCaption, uType )

WIN32_DECLARE_FUNCTION(
    LRESULT, DispatchMessageA, const MSG* lpMsg );
#define DispatchMessageA( lpMsg ) ___internal_DispatchMessageA( lpMsg )

WIN32_DECLARE_FUNCTION( BOOL, SetWindowTextA, HWND hWnd, LPCSTR lpString );
#define SetWindowTextA( hWnd, lpString )\
    ___internal_SetWindowTextA( hWnd, lpString )

WIN32_DECLARE_FUNCTION( int, GetWindowTextLengthA, HWND hWnd );
#define GetWindowTextLengthA( hWnd ) ___internal_GetWindowTextLengthA( hWnd )

WIN32_DECLARE_FUNCTION(
    int, GetWindowTextA, HWND hWnd, LPSTR lpString, int nMaxCount );
#define GetWindowTextA( hWnd, lpString, nMaxCount )\
    ___internal_GetWindowTextA( hWnd, lpString, nMaxCount )

WIN32_DECLARE_FUNCTION( BOOL, SetCursorPos, int X, int Y );
#define SetCursorPos( X, Y )\
    ___internal_SetCursorPos( X, Y )

WIN32_DECLARE_FUNCTION( BOOL, ClientToScreen, HWND hWnd, LPPOINT lpPoint );
#define ClientToScreen( hWnd, lpPoint )\
    ___internal_ClientToScreen( hWnd, lpPoint )

WIN32_DECLARE_FUNCTION( int, ShowCursor, BOOL bShow );
#define ShowCursor( bShow ) ___internal_ShowCursor( bShow )

WIN32_DECLARE_FUNCTION( HCURSOR, SetCursor, HCURSOR hCursor );
#define SetCursor( hCursor ) ___internal_SetCursor( hCursor )

WIN32_DECLARE_FUNCTION( HWND, CreateWindowExA, DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam );
#define CreateWindowExA( dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam )\
    ___internal_CreateWindowExA( dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam )

WIN32_DECLARE_FUNCTION(
    ATOM, RegisterClassExA, const WNDCLASSEXA* unnamedParam1 );
#define RegisterClassExA( unnamedParam1 )\
    ___internal_RegisterClassExA( unnamedParam1 )

WIN32_DECLARE_FUNCTION(
    BOOL, AdjustWindowRectEx,
    LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle );
#define AdjustWindowRectEx( lpRect, dwStyle, bMenu, dwExStyle )\
    ___internal_AdjustWindowRectEx( lpRect, dwStyle, bMenu, dwExStyle )

WIN32_DECLARE_FUNCTION(
    HCURSOR, LoadCursorA, HINSTANCE hInstance, LPCSTR lpCursorName );
#define LoadCursorA( hInstance, lpCursorName )\
    ___internal_LoadCursorA( hInstance, lpCursorName )

WIN32_DECLARE_FUNCTION( int, GetSystemMetrics, int nIndex );
#define GetSystemMetrics( nIndex ) ___internal_GetSystemMetrics( nIndex )

WIN32_DECLARE_FUNCTION(
    BOOL, SetWindowPos,
    HWND hWnd, HWND hWndInsertAfter,
    int X, int Y, int cx, int cy, UINT uFlags );
#define SetWindowPos( hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags )\
    ___internal_SetWindowPos( hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags )

#endif // if not headless

// NOTE(alicia): Gdi32

#if !defined(LD_HEADLESS)

WIN32_DECLARE_FUNCTION( int, GetDeviceCaps, HDC hdc, int index );
#define GetDeviceCaps( hdc, index ) ___internalGetDeviceCaps( hdc, index )

WIN32_DECLARE_FUNCTION( HGDIOBJ, GetStockObject, int i );
#define GetStockObject( i ) ___internal_GetStockObject( i )

WIN32_DECLARE_FUNCTION(
    int, DescribePixelFormat,
    HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd );
#define DescribePixelFormat( hdc, iPixelFormat, nBytes, ppfd )\
    ___internal_DescribePixelFormat( hdc, iPixelFormat, nBytes, ppfd )

WIN32_DECLARE_FUNCTION(
    int, ChoosePixelFormat, HDC hdc, const PIXELFORMATDESCRIPTOR* ppfd );
#define ChoosePixelFormat( hdc, ppfd )\
    ___internal_ChoosePixelFormat( hdc, ppfd )

WIN32_DECLARE_FUNCTION(
    BOOL, SetPixelFormat,
    HDC hdc, int format, const PIXELFORMATDESCRIPTOR* ppfd );
#define SetPixelFormat( hdc, format, ppfd )\
    ___internal_SetPixelFormat( hdc, format, ppfd )

WIN32_DECLARE_FUNCTION( BOOL, SwapBuffers, HDC unnamedParam1 );
#define SwapBuffers( unnamedParam1 ) ___internal_SwapBuffers( unnamedParam1 )

#endif // if not headless

// NOTE(alicia): Xinput

#if !defined(LD_HEADLESS)

WIN32_DECLARE_FUNCTION(
    DWORD, XInputGetState, DWORD dwUserIndex, XINPUT_STATE* pState );
#define XInputGetState( dwUserIndex, pState )\
    ___internal_XInputGetState( dwUserIndex, pState )

WIN32_DECLARE_FUNCTION(
    DWORD, XInputSetState, DWORD dwUserIndex, XINPUT_VIBRATION* pVibration );
#define XInputSetState( dwUserIndex, pVibration )\
    ___internal_XInputSetState( dwUserIndex, pVibration )

WIN32_DECLARE_FUNCTION( void, XInputEnable, BOOL enable );
void ___internal_XInputEnable_stub( BOOL enable ) {
    unused(enable);
}

#endif // if not headless

// NOTE(alicia): DWM

#if !defined(LD_HEADLESS)

WIN32_DECLARE_FUNCTION(
    HRESULT, DwmSetWindowAttribute,
    HWND hWnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute );
#define DwmSetWindowAttribute( hWnd, dwAttribute, pvAttribute, cbAttribute )\
    ___internal_DwmSetWindowAttribute( hWnd, dwAttribute, pvAttribute, cbAttribute )

#endif // if not headless

// NOTE(alicia): WGL

#if !defined(LD_HEADLESS)

WIN32_DECLARE_FUNCTION( HGLRC, wglGetCurrentContext, void );
#define wglGetCurrentContext() ___internal_wglGetCurrentContext()

WIN32_DECLARE_FUNCTION( HGLRC, wglCreateContext, HDC unnamedParam1 );
#define wglCreateContext( unnamedParam1 )\
    ___internal_wglCreateContext( unnamedParam1 )

WIN32_DECLARE_FUNCTION(
    BOOL, wglMakeCurrent, HDC unnamedParam1, HGLRC unnamedParam2 );
#define wglMakeCurrent( unnamedParam1, unnamedParam2 )\
    ___internal_wglMakeCurrent( unnamedParam1, unnamedParam2 )

WIN32_DECLARE_FUNCTION(
    BOOL, wglDeleteContext, HGLRC unnamedParam1 );
#define wglDeleteContext( unnamedParam1 )\
    ___internal_wglDeleteContext( unnamedParam1 )

WIN32_DECLARE_FUNCTION( PROC, wglGetProcAddress, LPCSTR unnamedParam1 );
#define wglGetProcAddress( unnamedParam1 )\
    ___internal_wglGetProcAddress( unnamedParam1 )

WIN32_DECLARE_FUNCTION(
    HGLRC, wglCreateContextAttribsARB,
    HDC hDC, HGLRC hShareContext, const int* attribList );
#define wglCreateContextAttribsARB( hDC, hShareContext, attribList )\
    ___internal_wglCreateContextAttribsARB( hDC, hShareContext, attribList )

WIN32_DECLARE_FUNCTION( BOOL, wglSwapIntervalEXT, int interval );
#define wglSwapIntervalEXT( interval )\
    ___internal_wglSwapIntervalEXT( interval )

#endif // if not headless

#if !defined(LD_HEADLESS)

WIN32_DECLARE_FUNCTION( HRESULT, CoCreateInstance, REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv );
#define CoCreateInstance ___internal_CoCreateInstance

WIN32_DECLARE_FUNCTION( HRESULT, CoInitialize, LPVOID pvReserved );
#define CoInitialize ___internal_CoInitialize

WIN32_DECLARE_FUNCTION( void, CoUninitialize, void );
#define CoUninitialize ___internal_CoUninitialize

WIN32_DECLARE_FUNCTION( void, CoTaskMemFree, LPVOID pv );
#define CoTaskMemFree ___internal_CoTaskMemFree

#endif /* if not headless */

#if !defined(LD_HEADLESS)

PlatformSurface* win32_surface_create(
    i32 width, i32 height, const char* name,
    b32 create_hidden, b32 resizeable,
    PlatformSurfaceGraphicsBackend backend );
void win32_surface_destroy( PlatformSurface* surface );
void win32_surface_set_callbacks(
    PlatformSurface* surface, PlatformSurfaceCallbacks* callbacks );
void win32_surface_clear_callbacks( PlatformSurface* surface );
void win32_surface_set_visible(
    PlatformSurface* surface, b32 is_visible );
b32 win32_surface_query_visibility( PlatformSurface* surface );
void win32_surface_set_dimensions(
    PlatformSurface* surface, i32 width, i32 height );
void win32_surface_query_dimensions(
    PlatformSurface* surface, i32* out_width, i32* out_height );
void win32_surface_set_mode(
    PlatformSurface* surface, PlatformSurfaceMode mode );
PlatformSurfaceMode win32_surface_query_mode(
    PlatformSurface* surface );
void win32_surface_set_name(
    PlatformSurface* surface, const char* name );
void win32_surface_query_name(
    PlatformSurface* surface, usize* buffer_size, char* buffer );
void win32_surface_center( PlatformSurface* surface );
void win32_surface_center_cursor( PlatformSurface* surface );
void win32_surface_gl_swap_buffers( PlatformSurface* surface );
b32  win32_surface_gl_init( PlatformSurface* surface );
void win32_surface_gl_swap_interval(
    PlatformSurface* surface, int interval );
void win32_surface_pump_events(void);

PlatformAudioContext* win32_audio_initialize( u64 buffer_length_ms );
void win32_audio_shutdown( PlatformAudioContext* ctx );
PlatformAudioBufferFormat win32_audio_query_buffer_format(
    PlatformAudioContext* ctx );
b32 win32_audio_lock_buffer(
    PlatformAudioContext* ctx, usize* out_sample_count,
    usize* out_buffer_size, void** out_buffer );
void win32_audio_unlock_buffer( PlatformAudioContext* ctx, usize sample_count );
void win32_audio_start( PlatformAudioContext* ctx );
void win32_audio_stop( PlatformAudioContext* ctx );

#endif // if not headless

#if !defined(LD_HEADLESS)
void win32_read_gamepads( PlatformGamepad gamepads[4] );
void win32_set_gamepad_rumble(
    u32 gamepad_index, u16 left_motor, u16 right_motor );
void win32_set_mouse_visible( b32 is_visible );
#endif // if not headless

PlatformInfo* win32_query_info(void);

#if !defined(LD_HEADLESS)

void* win32_gl_load_proc( const char* function_name );
void win32_fatal_message_box( const char* title, const char* message );

#endif // if not headless

void win32_last_error( usize* out_error_len, const char** out_error );

usize win32_write_stdout( void* target, usize count, char* characters ) {
    unused(target);
    WriteConsoleA(
        GetStdHandle( STD_OUTPUT_HANDLE ), characters, count, NULL, NULL );
    return 0;
}

#define WIN32_SUCCESS              ( 0)
#define WIN32_ERROR_OPEN_CORE      ( 1)
#define WIN32_ERROR_LOAD_CORE_INIT ( 2)
#define WIN32_ERROR_OPEN_USER32    ( 3)
#define WIN32_ERROR_OPEN_GDI32     ( 4)
#define WIN32_ERROR_OPEN_XINPUT    ( 5)
#define WIN32_ERROR_OPEN_DWM       ( 6)
#define WIN32_ERROR_OPEN_XAUDIO2   ( 7)
#define WIN32_ERROR_OPEN_OPENGL    ( 8)
#define WIN32_ERROR_LOAD_FUNCTION  ( 9)
#define WIN32_MISSING_INSTRUCTIONS (10)

DWORD win32_report_last_error(void);
LPSTR* WINAPI CommandLineToArgvA(LPSTR lpCmdline, int* numargs);
#if defined(LD_CONSOLE_APP)
_Noreturn void __stdcall mainCRTStartup(void) {
#else
_Noreturn void __stdcall WinMainCRTStartup(void) {
#endif

    int argc    = 0;
    char** argv = CommandLineToArgvA( GetCommandLineA(), &argc );

#if !defined(LD_HEADLESS)
    global_instance = GetModuleHandle( NULL );
#endif // if not headless

    DWORD dwMode = 0;
    GetConsoleMode(
        GetStdHandle( STD_OUTPUT_HANDLE ), &dwMode );
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(
        GetStdHandle( STD_OUTPUT_HANDLE ), dwMode );

    #define EXIT(code) ExitProcess(code)

#if defined(LD_DEVELOPER_MODE)
    #define WIN32_REPORT_ERROR( message ) do {\
        WriteConsoleA( GetStdHandle( STD_ERROR_HANDLE ), message, sizeof(message), NULL, NULL );\
        OutputDebugStringA( message );\
    } while(0)
#else
    #define WIN32_REPORT_ERROR( ... )
#endif

#if defined(LD_DEVELOPER_MODE)

#if defined(LD_HEADLESS)
    #define WIN32_REPORT_FATAL_ERROR( error_code, message ) do {\
        if( win32_report_last_error() != ERROR_SUCCESS ) {\
            usize       last_error_len = 0;\
            const char* last_error     = NULL;\
            win32_last_error( &last_error_len, &last_error );\
            WriteConsoleA( GetStdHandle( STD_ERROR_HANDLE ), message, sizeof(message), NULL, NULL );\
            WriteConsoleA( GetStdHandle( STD_ERROR_HANDLE ), last_error, last_error_len, NULL, NULL );\
            OutputDebugStringA( message );\
            OutputDebugStringA( last_error );\
        }\
    } while(0)
#else
    #define WIN32_REPORT_FATAL_ERROR( error_code, message ) do {\
        if( win32_report_last_error() != ERROR_SUCCESS ) {\
            usize       last_error_len = 0;\
            const char* last_error     = NULL;\
            win32_last_error( &last_error_len, &last_error );\
            WriteConsoleA( GetStdHandle( STD_ERROR_HANDLE ), message, sizeof(message), NULL, NULL );\
            WriteConsoleA( GetStdHandle( STD_ERROR_HANDLE ), last_error, last_error_len, NULL, NULL );\
            OutputDebugStringA( message );\
            OutputDebugStringA( last_error );\
        }\
        if( ___internal_MessageBoxA ) {\
            win32_fatal_message_box(\
                "Fatal Error" macro_value_to_string( error_code ),\
                message );\
        }\
    } while(0)
#endif

#else

#if defined(LD_HEADLESS)

    #define WIN32_REPORT_FATAL_ERROR( error_code, message ) do {\
        win32_report_last_error();\
        unused(message);\
    } while(0)

#else

    #define WIN32_REPORT_FATAL_ERROR( error_code, message ) do {\
        win32_report_last_error();\
        if( ___internal_MessageBoxA ) {\
            win32_fatal_message_box(\
                "Fatal Error" macro_value_to_string( error_code ),\
                message );\
        }\
    } while(0)

#endif // if headless

#endif

    #define WIN32_LOAD_REQUIRED( module, fn ) do {\
        if( !WIN32_LOAD_FUNCTION( module, fn ) ) {\
            WIN32_REPORT_FATAL_ERROR(\
                WIN32_ERROR_LOAD_FUNCTION,\
                "Fatal Error: Failed to load function " #fn " from module " #module "!"\
            );\
            EXIT( WIN32_ERROR_LOAD_FUNCTION );\
        }\
    } while(0)

#if !defined(LD_HEADLESS)
    HMODULE user32 = LoadLibraryA( "USER32.DLL" );
    if( !user32 ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_ERROR_OPEN_USER32, "Failed to open library USER32.DLL!" );
        EXIT( WIN32_ERROR_OPEN_USER32 );
    }
    if( !WIN32_LOAD_FUNCTION( user32, MessageBoxA ) ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_ERROR_LOAD_FUNCTION, "Failed to load MessageBoxA!" );
        EXIT( WIN32_ERROR_LOAD_FUNCTION );
    }
#endif // if not headless

#if !defined(LD_CORE_STATIC_BUILD)
    HMODULE core = LoadLibraryA( LIQUID_ENGINE_PATH );
    if( !core ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_ERROR_OPEN_CORE,
            "Fatal Error: Failed to open Engine Core library! "
            "Core Library Path: " LIQUID_ENGINE_PATH );
        EXIT( WIN32_ERROR_OPEN_CORE );
    }

    CoreInitFN* core_init =
        (CoreInitFN*)GetProcAddress( core, "core_init" );
    if( !core_init ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_ERROR_LOAD_CORE_INIT,
            "Fatal Error: "
            "Failed to load Engine Core library initalize function!");
        EXIT( WIN32_ERROR_LOAD_CORE_INIT );
    }
#endif // if core is statically compiled

#if !defined(LD_HEADLESS)
    HMODULE gdi32 = LoadLibraryA( "GDI32.DLL" );
    if( !gdi32 ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_ERROR_OPEN_GDI32,
            "Fatal Error: Failed to open library GDI32.DLL!" );
        EXIT( WIN32_ERROR_OPEN_GDI32 );
    }
    HMODULE xinput = LoadLibraryA( "XINPUT1_4.DLL" );
    if( !xinput ) {
        xinput = LoadLibraryA( "XINPUT9_1_0.DLL" );
        if( !xinput ) {
            xinput = LoadLibraryA( "XINPUT1_3.DLL" );
            if( !xinput ) {
                WIN32_REPORT_FATAL_ERROR(
                    WIN32_ERROR_OPEN_XINPUT,
                    "Fatal Error: Failed to open library XINPUT!" );
                EXIT( WIN32_ERROR_OPEN_XINPUT );
            }
        }
    }

    HMODULE dwm = LoadLibraryA( "DWMAPI.DLL" );
    if( !dwm ) {
        WIN32_REPORT_ERROR( "Failed to load DWMAPI.DLL :(" );
    }

#endif // if not headless

#if !defined(LD_HEADLESS)
    WIN32_LOAD_REQUIRED( user32, SetFocus );
    WIN32_LOAD_REQUIRED( user32, RegisterRawInputDevices );
    WIN32_LOAD_REQUIRED( user32, GetRawInputData );
    WIN32_LOAD_REQUIRED( user32, SetWindowPlacement );
    WIN32_LOAD_REQUIRED( user32, GetWindowPlacement );
    WIN32_LOAD_REQUIRED( user32, GetMonitorInfoA );
    WIN32_LOAD_REQUIRED( user32, MonitorFromPoint );
    WIN32_LOAD_REQUIRED( user32, MonitorFromWindow );
    WIN32_LOAD_REQUIRED( user32, LoadImageA );
    WIN32_LOAD_REQUIRED( user32, GetWindowLongPtrA );
    WIN32_LOAD_REQUIRED( user32, SetWindowLongPtrA );
    WIN32_LOAD_REQUIRED( user32, DefWindowProcA );
    WIN32_LOAD_REQUIRED( user32, GetClientRect );
    WIN32_LOAD_REQUIRED( user32, MapVirtualKeyA );
    WIN32_LOAD_REQUIRED( user32, DestroyWindow );
    WIN32_LOAD_REQUIRED( user32, PeekMessageA );
    WIN32_LOAD_REQUIRED( user32, TranslateMessage );
    WIN32_LOAD_REQUIRED( user32, DestroyIcon );
    WIN32_LOAD_REQUIRED( user32, GetDC );
    WIN32_LOAD_REQUIRED( user32, ReleaseDC );
    WIN32_LOAD_REQUIRED( user32, ShowWindow );
    WIN32_LOAD_REQUIRED( user32, DispatchMessageA );
    WIN32_LOAD_REQUIRED( user32, SetWindowTextA );
    WIN32_LOAD_REQUIRED( user32, GetWindowTextLengthA );
    WIN32_LOAD_REQUIRED( user32, GetWindowTextA );
    WIN32_LOAD_REQUIRED( user32, SetCursorPos );
    WIN32_LOAD_REQUIRED( user32, ClientToScreen );
    WIN32_LOAD_REQUIRED( user32, ShowCursor );
    WIN32_LOAD_REQUIRED( user32, SetCursor );
    WIN32_LOAD_REQUIRED( user32, CreateWindowExA );
    WIN32_LOAD_REQUIRED( user32, RegisterClassExA );
    WIN32_LOAD_REQUIRED( user32, AdjustWindowRectEx );
    WIN32_LOAD_REQUIRED( user32, LoadCursorA );
    WIN32_LOAD_REQUIRED( user32, GetSystemMetrics );
    WIN32_LOAD_REQUIRED( user32, SetWindowPos );

    WIN32_LOAD_REQUIRED( gdi32, GetDeviceCaps );
    WIN32_LOAD_REQUIRED( gdi32, GetStockObject );

    WIN32_LOAD_REQUIRED( xinput, XInputGetState );
    WIN32_LOAD_REQUIRED( xinput, XInputSetState );

    if( !WIN32_LOAD_FUNCTION( dwm, DwmSetWindowAttribute ) ) {
        WIN32_REPORT_ERROR( "Failed to load DwmSetWindowAttribute :(" );
    }
    
    if( !WIN32_LOAD_FUNCTION( xinput, XInputEnable ) ) {
        ___internal_XInputEnable = ___internal_XInputEnable_stub;
    }

#endif // if not headless

    SYSTEM_INFO win32_info = {};
    GetSystemInfo( &win32_info );

    global_win32_info.page_size = win32_info.dwPageSize;

    if( IsProcessorFeaturePresent(
        PF_XMMI_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_SSE;
    }
    if( IsProcessorFeaturePresent(
        PF_XMMI64_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_SSE2;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_SSE3;
    }
    if( IsProcessorFeaturePresent(
        PF_SSSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_SSSE3;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_1_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_SSE4_1;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_2_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_SSE4_2;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_AVX;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX2_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_AVX2;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX512F_INSTRUCTIONS_AVAILABLE
    ) ) {
        global_win32_info.features |= PLATFORM_PROCESSOR_FEATURE_AVX_512;
    }

    MEMORYSTATUSEX memory_status = {};
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx( &memory_status );

    global_win32_info.total_memory =
        memory_status.ullTotalPhys;
    global_win32_info.logical_processor_count =
        win32_info.dwNumberOfProcessors;

#if defined( LD_ARCH_X86 )
    memset(
        global_win32_info.cpu_name,
        ' ',
        PLATFORM_CPU_NAME_BUFFER_SIZE );
    global_win32_info.cpu_name[PLATFORM_CPU_NAME_BUFFER_SIZE - 1] = 0;

    int cpu_info[4] = {};
    __cpuid( cpu_info, 0x80000002 );
    memcpy(
        global_win32_info.cpu_name,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000003 );
    memcpy(
        global_win32_info.cpu_name + 16,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000004 );
    memcpy(
        global_win32_info.cpu_name + 32,
        cpu_info,
        sizeof(cpu_info)
    );
#endif

#if LD_SIMD_WIDTH >= 4

#if defined(LD_ARCH_X86)
    b32 sse_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_SSE );
    b32 sse2_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_SSE2 );
    b32 sse3_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_SSE3 );
    b32 ssse3_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_SSSE3 );
    b32 sse4_1_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_SSE4_1 );
    b32 sse4_2_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_SSE4_2 );

    if( !(
        sse_available    &&
        sse2_available   &&
        sse3_available   &&
        ssse3_available  &&
        sse4_1_available &&
        sse4_2_available
    ) ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_MISSING_INSTRUCTIONS,
            "Fatal Error: "
            "This version of Liquid Engine was compiled "
            "with SSE instructions but the current system is "
            "missing those instructions!");
        EXIT( WIN32_MISSING_INSTRUCTIONS );
    }

#endif // x86

#endif // simd width >= 4

#if LD_SIMD_WIDTH >= 8

#if defined(LD_ARCH_X86)

    b32 avx_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_AVX );
    b32 avx2_available = bitfield_check(
        global_win32_info.features, PLATFORM_PROCESSOR_FEATURE_AVX2 );

    if( !( avx_available && avx2_available ) ) {
        WIN32_REPORT_FATAL_ERROR(
            WIN32_MISSING_INSTRUCTIONS,
            "Fatal Error: "
            "This version of Liquid Engine was compiled "
            "width AVX instructions but the current system is "
            "missing those instructions!" );
        EXIT( WIN32_MISSING_INSTRUCTIONS );
    }

#endif // x86

#endif // simd width >= 8

    PlatformAPI api = {};

#if !defined(LD_HEADLESS)
    api.surface.create           = win32_surface_create;
    api.surface.destroy          = win32_surface_destroy;
    api.surface.set_callbacks    = win32_surface_set_callbacks;
    api.surface.clear_callbacks  = win32_surface_clear_callbacks;
    api.surface.set_visible      = win32_surface_set_visible;
    api.surface.query_visibility = win32_surface_query_visibility;
    api.surface.set_dimensions   = win32_surface_set_dimensions;
    api.surface.query_dimensions = win32_surface_query_dimensions;
    api.surface.set_mode         = win32_surface_set_mode;
    api.surface.query_mode       = win32_surface_query_mode;
    api.surface.set_name         = win32_surface_set_name;
    api.surface.query_name       = win32_surface_query_name;
    api.surface.center           = win32_surface_center;
    api.surface.center_cursor    = win32_surface_center_cursor;
    api.surface.gl_init          = win32_surface_gl_init;
    api.surface.gl_swap_buffers  = win32_surface_gl_swap_buffers;
    api.surface.gl_swap_interval = win32_surface_gl_swap_interval;
    api.surface.pump_events      = win32_surface_pump_events;

    api.audio.initialize          = win32_audio_initialize;
    api.audio.shutdown            = win32_audio_shutdown;
    api.audio.query_buffer_format = win32_audio_query_buffer_format;
    api.audio.lock_buffer         = win32_audio_lock_buffer;
    api.audio.unlock_buffer       = win32_audio_unlock_buffer;
    api.audio.start               = win32_audio_start;
    api.audio.stop                = win32_audio_stop;

    api.io.read_gamepads       = win32_read_gamepads;
    api.io.set_gamepad_rumble  = win32_set_gamepad_rumble;
    api.io.set_mouse_visible   = win32_set_mouse_visible;

    api.gl_load_proc      = win32_gl_load_proc;
    api.fatal_message_box = win32_fatal_message_box;
#endif // if not headless
    
    api.query_info        = win32_query_info;
    api.last_error        = win32_last_error;

    QueryPerformanceCounter( &global_performance_counter );
    QueryPerformanceFrequency( &global_performance_frequency );

    int return_code = core_init( argc, argv, &api );

    LocalFree( argv );
    EXIT( return_code );
}

// NOTE(alicia): Surface API

#if !defined(LD_HEADLESS)

struct Win32Surface {
    HWND            hWnd;
    HDC             hDc;
    i32             width, height;
    DWORD           dwStyle;
    DWORD           dwExStyle;
    WINDOWPLACEMENT placement;

    b32 resizeable;
    b32 is_visible;

    PlatformSurfaceCallbacks callbacks;

    PlatformSurfaceMode mode;

    PlatformSurfaceGraphicsBackend backend;
    union {
        HGLRC glrc;
    };
};

internal MONITORINFO win32_monitor_info( HWND opt_window_handle ) {
    HMONITOR monitor = NULL;
    if( opt_window_handle ) {
        monitor = MonitorFromWindow(
            opt_window_handle, MONITOR_DEFAULTTONEAREST );
    } else {
        // TODO(alicia): cache last point window was at
        // before closing so that the application can always
        // open on the last used monitor
        POINT pt = {};
        monitor = MonitorFromPoint( pt, MONITOR_DEFAULTTOPRIMARY );
    }
    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(monitor_info);
    assert( GetMonitorInfoA( monitor, &monitor_info ) );
    return monitor_info;
}

LRESULT win32_winproc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
PlatformSurface* win32_surface_create(
    i32 width, i32 height, const char* name,
    b32 create_hidden, b32 resizeable,
    PlatformSurfaceGraphicsBackend backend
) {
    struct Win32Surface* win32_surface =
        HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(struct Win32Surface) );
    if( !win32_surface ) {
        return NULL;
    }

    WNDCLASSEX window_class = {};
    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.lpfnWndProc   = win32_winproc;
    window_class.hInstance     = global_instance;
    // TODO(alicia): find out if this needs to be unique.
    window_class.lpszClassName = "LiquidEngineWindowClass";
    window_class.hbrBackground = (HBRUSH)GetStockBrush( BLACK_BRUSH );

    if( !RegisterClassExA( &window_class ) ) {
        win32_report_last_error();
        return NULL;
    }

    DWORD dwExStyle = WIN32_WINDOWED_DWEXSTYLE;
    DWORD dwStyle   = 0;

    if( resizeable ) {
        dwStyle = WIN32_WINDOWED_RESIZEABLE_DWSTYLE;
    } else {
        dwStyle = WIN32_WINDOWED_DWSTYLE;
    }

    RECT window_rect = {};
    window_rect.right  = width;
    window_rect.bottom = height;

    if( !AdjustWindowRectEx(
        &window_rect,
        dwStyle, false,
        dwExStyle
    ) ) {
        win32_report_last_error();
        return NULL;
    }

    HWND   hWndParent = NULL;
    HMENU  hMenu      = NULL;
    LPVOID lpParam    = NULL;
    HWND handle = CreateWindowExA(
        dwExStyle,
        window_class.lpszClassName,
        name,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        hWndParent, hMenu,
        global_instance,
        lpParam
    );
    if( !handle ) {
        win32_report_last_error();
        return false;
    }
    HDC hdc = GetDC( handle );

    /* setup raw input */ {
        RAWINPUTDEVICE rid;
        rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid.usUsage     = HID_USAGE_GENERIC_MOUSE;
        rid.dwFlags     = RIDEV_INPUTSINK;
        rid.hwndTarget  = handle;
        RegisterRawInputDevices( &rid, 1, sizeof(rid) );
    }

    win32_surface->hWnd       = handle;
    win32_surface->hDc        = hdc;
    win32_surface->width      = width;
    win32_surface->height     = height;
    win32_surface->dwStyle    = dwStyle;
    win32_surface->dwExStyle  = dwExStyle;
    win32_surface->resizeable = resizeable;
    win32_surface->backend    = backend;
    win32_surface->mode       = PLATFORM_SURFACE_WINDOWED;

    SetWindowLongPtrA( handle, GWLP_USERDATA, (LONG_PTR)win32_surface );

    if( ___internal_DwmSetWindowAttribute ) {
        #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
        BOOL dwm_value = TRUE;
        DwmSetWindowAttribute(
            win32_surface->hWnd,
            DWMWA_USE_IMMERSIVE_DARK_MODE,
            &dwm_value, sizeof(dwm_value) );
    }

    if( !create_hidden ) {
        win32_surface->is_visible = true;
        ShowWindow( handle, SW_SHOW );
    }

    return win32_surface;
}
void win32_surface_destroy( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;

    switch( win32_surface->backend ) {
        case PLATFORM_SURFACE_GRAPHICS_BACKEND_OPENGL: {
            assert( wglMakeCurrent( NULL, NULL ) );
            assert( wglDeleteContext( win32_surface->glrc ) );
        } break;
        default: panic();
    }

    ReleaseDC( win32_surface->hWnd, win32_surface->hDc );
    DestroyWindow( win32_surface->hWnd );

    HeapFree( GetProcessHeap(), HEAP_ZERO_MEMORY, win32_surface );
}
void win32_surface_set_callbacks(
    PlatformSurface* surface, PlatformSurfaceCallbacks* callbacks
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    memcpy(
        &win32_surface->callbacks,
        callbacks,
        sizeof( PlatformSurfaceCallbacks ) );
}
void win32_surface_clear_callbacks( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    memset( &win32_surface->callbacks, 0, sizeof(PlatformSurfaceCallbacks) );
}
void win32_surface_set_visible(
    PlatformSurface* surface, b32 is_visible
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    int nCmdShow = is_visible ? SW_SHOW : SW_HIDE;
    ShowWindow( win32_surface->hWnd, nCmdShow );
    win32_surface->is_visible = is_visible;
    SetFocus( win32_surface->hWnd );
}
b32 win32_surface_query_visibility( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    return win32_surface->is_visible;
}
void win32_surface_set_dimensions(
    PlatformSurface* surface, i32 width, i32 height
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;

    if( win32_surface->mode != PLATFORM_SURFACE_WINDOWED ) {
        return;
    }

    RECT window_rect = {};

    window_rect.right  = width;
    window_rect.bottom = height;

    win32_surface->width  = width;
    win32_surface->height = height;

    AdjustWindowRectEx(
        &window_rect,
        win32_surface->dwStyle,
        FALSE,
        win32_surface->dwExStyle
    );

    HWND hwnd_insert_after = NULL;
    if( win32_surface->mode == PLATFORM_SURFACE_FULLSCREEN ) {
        hwnd_insert_after = HWND_TOP;
    }

    SetWindowPos(
        win32_surface->hWnd,
        hwnd_insert_after,
        0, 0,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        SWP_NOMOVE | SWP_NOREPOSITION
    );
}
void win32_surface_query_dimensions(
    PlatformSurface* surface, i32* out_width, i32* out_height
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    if( out_width ) {
        *out_width = win32_surface->width;
    }
    if( out_height ) {
        *out_height = win32_surface->height;
    }
}
void win32_surface_set_mode(
    PlatformSurface* surface, PlatformSurfaceMode mode
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;

    if( win32_surface->mode == mode ) {
        return;
    }

    win32_surface->mode = mode;
    switch( mode ) {
        case PLATFORM_SURFACE_WINDOWED: {
            b32 is_resizeable = win32_surface->resizeable;

            win32_surface->dwExStyle = WIN32_WINDOWED_DWEXSTYLE;
            if( is_resizeable ) {
                win32_surface->dwStyle = WIN32_WINDOWED_RESIZEABLE_DWSTYLE;
            } else {
                win32_surface->dwStyle = WIN32_WINDOWED_DWSTYLE;
            }

            SetWindowLongPtrA(
                win32_surface->hWnd,
                GWL_STYLE,
                win32_surface->dwStyle
            );
            SetWindowLongPtrA(
                win32_surface->hWnd,
                GWL_EXSTYLE,
                win32_surface->dwExStyle
            );

            SetWindowPlacement(
                win32_surface->hWnd, &win32_surface->placement );

            SetWindowPos(
                win32_surface->hWnd,
                NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED
            );
        } break;
        case PLATFORM_SURFACE_FULLSCREEN: {

            memset( &win32_surface->placement, 0, sizeof(WINDOWPLACEMENT) );
            win32_surface->placement.length = sizeof(WINDOWPLACEMENT);
            BOOL placement_result = GetWindowPlacement(
                win32_surface->hWnd, &win32_surface->placement );
            assert( placement_result );

            win32_surface->dwExStyle = WIN32_FULLSCREEN_DWEXSTYLE;
            win32_surface->dwStyle   = WIN32_FULLSCREEN_DWSTYLE;

            MONITORINFO monitor_info =
                win32_monitor_info( win32_surface->hWnd );

            SetWindowLongPtrA(
                win32_surface->hWnd,
                GWL_STYLE,
                win32_surface->dwStyle
            );
            SetWindowLongPtrA(
                win32_surface->hWnd,
                GWL_EXSTYLE,
                win32_surface->dwExStyle
            );

            i32 x = monitor_info.rcMonitor.left;
            i32 y = monitor_info.rcMonitor.top;
            i32 width =
                monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
            i32 height =
                monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

            SetWindowPos(
                win32_surface->hWnd,
                HWND_TOP,
                x, y,
                width, height,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW
            );
        } break;
        default: panic();
    }

}
PlatformSurfaceMode win32_surface_query_mode(
    PlatformSurface* surface
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    return win32_surface->mode;
}
void win32_surface_set_name(
    PlatformSurface* surface, const char* name
) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    SetWindowTextA( win32_surface->hWnd, name );
}
void win32_surface_query_name(
    PlatformSurface* surface, usize* buffer_size, char* buffer
) {
    struct Win32Surface* win32_surface = surface;

    usize text_length = (usize)GetWindowTextLengthA( win32_surface->hWnd );

    if( !buffer ) {
        assert( buffer_size );
        *buffer_size = (usize)text_length;
        return;
    }

    GetWindowTextA( win32_surface->hWnd, buffer, *buffer_size );
    if( text_length > *buffer_size ) {
        *buffer_size = text_length - *buffer_size;
    } else {
        *buffer_size = 0;
    }
}
void win32_surface_center( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;
    if( win32_surface->mode != PLATFORM_SURFACE_WINDOWED ) {
        return;
    }

    MONITORINFO monitor_info = win32_monitor_info( win32_surface->hWnd );

    /// window dimensions
    i32 window_width  = win32_surface->width;
    i32 window_height = win32_surface->height;

    /// absolute dimensions
    i32 monitor_width =
        monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
    i32 monitor_height =
        monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

    i32 x = 0, y = 0;
    /// absolute center
    i32 center_x = monitor_width  / 2;
    i32 center_y = monitor_height / 2;

    x = center_x - ( window_width / 2 );
    y = center_y - ( window_height / 2 );

    /// relative to monitor position
    x += monitor_info.rcMonitor.left;
    y += monitor_info.rcMonitor.top;

    HWND hWndInsertAfter = NULL;
    SetWindowPos(
        win32_surface->hWnd,
        hWndInsertAfter,
        x, y,
        0, 0,
        SWP_NOSIZE );
}
void win32_surface_center_cursor( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;

    POINT center = {};
    center.x = win32_surface->width / 2;
    center.y = win32_surface->height / 2;

    ClientToScreen( win32_surface->hWnd, &center );
    SetCursorPos( center.x, center.y );
}
void win32_surface_pump_events(void) {
    MSG message = {};
    while( PeekMessageA( &message, NULL, 0, 0, PM_REMOVE ) ) {
        TranslateMessage( &message );
        DispatchMessageA( &message );
    }
}

struct Win32AudioContext {
    IAudioClient*        client;
    IAudioRenderClient*  render_client;
    IMMDeviceEnumerator* device_enumerator;
    IMMDevice*           device;

    WAVEFORMATEX format;

    UINT32 buffer_frame_count;
    usize  buffer_size;

    HMODULE ole32;
};

PlatformAudioContext* win32_audio_initialize( u64 buffer_length_ms ) {

    struct Win32AudioContext* ctx = HeapAlloc(
        GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof( struct Win32AudioContext ) );
    if( !ctx ) {
        return NULL;
    }

    ctx->ole32 = LoadLibraryA( "OLE32.DLL" );
    if( !ctx->ole32 ) {
        win32_report_last_error();
        return NULL;
    }

    #define WIN32_AUDIO_LOAD( function ) do {\
        ___internal_##function = (___internal_##function##FN*)GetProcAddress( ctx->ole32, #function );\
        if( !___internal_##function ) {\
            HeapFree( GetProcessHeap(), 0, ctx );\
            return NULL;\
        }\
    } while(0)
    
    WIN32_AUDIO_LOAD( CoInitialize );
    WIN32_AUDIO_LOAD( CoUninitialize );
    WIN32_AUDIO_LOAD( CoCreateInstance );
    WIN32_AUDIO_LOAD( CoTaskMemFree );

    // TODO(alicia): logging!
    #define AUDFN( function_call ) do {\
        HRESULT hresult = function_call;\
        if( FAILED( hresult ) ) {\
            HeapFree( GetProcessHeap(), 0, ctx );\
            return NULL;\
        }\
    } while(0)

    /// Initialize COM.
    AUDFN( CoInitialize( NULL ) );
    AUDFN( CoCreateInstance(
        &CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, &IID_IMMDeviceEnumerator,
        (void**)&ctx->device_enumerator ) );

    /// Get default device.
    AUDFN( ctx->device_enumerator->lpVtbl->GetDefaultAudioEndpoint(
        ctx->device_enumerator, eRender, eConsole, &ctx->device ) );

    /// Activate audio client.
    AUDFN( ctx->device->lpVtbl->Activate(
        ctx->device, &IID_IAudioClient,
        CLSCTX_ALL, NULL, (void**)&ctx->client ) );

    /// Define audio buffer format.
    ctx->format.nChannels       = 2;
    ctx->format.wFormatTag      = WAVE_FORMAT_PCM;
    ctx->format.nSamplesPerSec  = 44100;
    ctx->format.wBitsPerSample  = 16;
    ctx->format.nBlockAlign     =
        ( ctx->format.nChannels * ctx->format.wBitsPerSample ) / 8;
    ctx->format.nAvgBytesPerSec =
        ctx->format.nSamplesPerSec * ctx->format.nBlockAlign;
    ctx->format.cbSize          = 0;

    /// Create audio buffer.
    #define REFTIMES_PER_MS 10000
    REFTIME buffer_length_reftime = buffer_length_ms * REFTIMES_PER_MS;
    DWORD initialize_stream_flags = 
        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
        AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;

    AUDFN( ctx->client->lpVtbl->Initialize(
        ctx->client,
        AUDCLNT_SHAREMODE_SHARED, initialize_stream_flags,
        buffer_length_reftime, 0,
        &ctx->format, NULL ) );

    /// Calculate buffer size.
    AUDFN( ctx->client->lpVtbl->GetBufferSize(
        ctx->client, &ctx->buffer_frame_count ) );

    ctx->buffer_size = ctx->buffer_frame_count *
        ( ctx->format.nChannels * ( ctx->format.wBitsPerSample / 8 ) );

    /// Get render client.
    AUDFN( ctx->client->lpVtbl->GetService(
        ctx->client, &IID_IAudioRenderClient, (void**)&ctx->render_client ) );

    /// Clear audio buffer.
    BYTE* buffer = NULL;
    AUDFN( ctx->render_client->lpVtbl->GetBuffer(
        ctx->render_client, ctx->buffer_frame_count, &buffer ) );

    memset( buffer, 0, ctx->buffer_size );

    AUDFN( ctx->render_client->lpVtbl->ReleaseBuffer(
        ctx->render_client, ctx->buffer_frame_count, 0 ) );

    /// Start playing buffer.
    AUDFN( ctx->client->lpVtbl->Start( ctx->client ) );

    return ctx;
}
void win32_audio_shutdown( PlatformAudioContext* context ) {
    struct Win32AudioContext* ctx = context;

    #define COM_RELEASE( punk ) do {\
        if( (punk) != NULL ) {\
            (punk)->lpVtbl->Release( (punk) );\
            (punk) = NULL;\
        }\
    } while(0)

    ctx->client->lpVtbl->Stop( ctx->client );

    COM_RELEASE( ctx->device_enumerator );
    COM_RELEASE( ctx->device );
    COM_RELEASE( ctx->client );
    COM_RELEASE( ctx->render_client );

    CoUninitialize();

    FreeLibrary( ctx->ole32 );

    HeapFree( GetProcessHeap(), 0, ctx );
}
PlatformAudioBufferFormat win32_audio_query_buffer_format(
    PlatformAudioContext* context
) {
    struct Win32AudioContext* ctx = context;
    PlatformAudioBufferFormat format = {};

    format.number_of_channels  = ctx->format.nChannels;
    format.bits_per_sample     = ctx->format.wBitsPerSample;
    format.bytes_per_sample    = format.bits_per_sample / 8;
    format.samples_per_second  = ctx->format.nSamplesPerSec;
    format.buffer_sample_count = ctx->buffer_frame_count;
    format.buffer_size         = ctx->buffer_size;

    return format;
}
b32 win32_audio_lock_buffer(
    PlatformAudioContext* context, usize* out_sample_count,
    usize* out_buffer_size, void** out_buffer
) {
    struct Win32AudioContext* ctx = context;

    UINT32 buffer_frame_padding_count = 0;
    HRESULT hresult = ctx->client->lpVtbl->GetCurrentPadding(
        ctx->client, &buffer_frame_padding_count );

    if( FAILED( hresult ) ) {
        // TODO(alicia): logging!
        return false;
    }

    if( buffer_frame_padding_count > ctx->buffer_frame_count ) {
        // TODO(alicia): logging!
        return false;
    }

    UINT32 buffer_frames_to_request =
        ctx->buffer_frame_count - buffer_frame_padding_count;

    if( !buffer_frames_to_request ) {
        return false;
    }

    BYTE* buffer = NULL;
    hresult = ctx->render_client->lpVtbl->GetBuffer(
        ctx->render_client, buffer_frames_to_request, &buffer );
    if( hresult != S_OK ) {
        // TODO(alicia): logging!
        return false;
    }

    if( !buffer ) {
        return false;
    }

    *out_sample_count = buffer_frames_to_request;
    *out_buffer_size  = *out_sample_count * ctx->format.nBlockAlign;
    // *out_buffer_size  = buffer_frames_to_request *
    //     ( ctx->format.nChannels * ( ctx->format.wBitsPerSample / 8 ) );
    *out_buffer       = buffer;

    return true;
}
void win32_audio_unlock_buffer( PlatformAudioContext* context, usize sample_count ) {
    struct Win32AudioContext* ctx = context;

    ctx->render_client->lpVtbl->ReleaseBuffer(
        ctx->render_client, (UINT32)sample_count, 0 );
}
void win32_audio_start( PlatformAudioContext* context ) {
    struct Win32AudioContext* ctx = context;
    ctx->client->lpVtbl->Start( ctx->client );
}
void win32_audio_stop( PlatformAudioContext* context ) {
    struct Win32AudioContext* ctx = context;
    ctx->client->lpVtbl->Stop( ctx->client );
}

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

b32 win32_surface_gl_init( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;

    HMODULE opengl32 = GetModuleHandle( "OPENGL32.DLL" );
    if( !opengl32 ) {
        opengl32 = LoadLibraryA( "OPENGL32.DLL" );
        if( !opengl32 ) {
            win32_report_last_error();
            return false;
        }
    }

    HMODULE gdi32 = GetModuleHandle( "GDI32.DLL" );
    assert( gdi32 );

    #define WGL_LOAD_FUNCTION( fn ) do {\
        ___internal_##fn =\
            (___internal_##fn##FN*)wglGetProcAddress( #fn );\
        if( !___internal_##fn ) {\
            win32_report_last_error();\
            return false;\
        }\
    } while(0)

    #define WIN32_LOAD_OPENGL( module, fn ) do {\
        ___internal_##fn =\
            (___internal_##fn##FN*)GetProcAddress(module, #fn);\
        if( !___internal_##fn ) {\
            win32_report_last_error();\
            return false;\
        }\
    } while(0)

    opengl32 = LoadLibraryA( "OPENGL32.DLL" );
    if( !opengl32 ) {
        win32_report_last_error();
        return false;
    }

    WIN32_LOAD_OPENGL(opengl32, wglGetCurrentContext);
    WIN32_LOAD_OPENGL(opengl32, wglCreateContext);
    WIN32_LOAD_OPENGL(opengl32, wglMakeCurrent);
    WIN32_LOAD_OPENGL(opengl32, wglDeleteContext);
    WIN32_LOAD_OPENGL(opengl32, wglGetProcAddress);

    WIN32_LOAD_OPENGL(gdi32, DescribePixelFormat);
    WIN32_LOAD_OPENGL(gdi32, ChoosePixelFormat);
    WIN32_LOAD_OPENGL(gdi32, SetPixelFormat);
    WIN32_LOAD_OPENGL(gdi32, SwapBuffers);

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

    i32 pixelFormatIndex = ChoosePixelFormat(
        win32_surface->hDc, &desired_pixel_format );

    PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
    DescribePixelFormat(
        win32_surface->hDc, pixelFormatIndex,
        pixel_format_size, &suggested_pixel_format
    );

    if( SetPixelFormat(
        win32_surface->hDc,
        pixelFormatIndex,
        &suggested_pixel_format
    ) == FALSE ) {
        win32_report_last_error();
        return false;
    }

    HGLRC temp = wglCreateContext( win32_surface->hDc );
    if(!temp) {
        win32_report_last_error();
        return false;
    }

    wglMakeCurrent( win32_surface->hDc, temp );

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, GL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB, GL_VERSION_MINOR,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,
        0
    };

    WGL_LOAD_FUNCTION(wglCreateContextAttribsARB);

    win32_surface->glrc = wglCreateContextAttribsARB(
        win32_surface->hDc, NULL, attribs );

    wglDeleteContext( temp );
    if( !win32_surface->glrc ) {
        win32_report_last_error();
        return false;
    }
    wglMakeCurrent( win32_surface->hDc, win32_surface->glrc );

    WGL_LOAD_FUNCTION(wglSwapIntervalEXT);

    return true;

}
void win32_surface_gl_swap_buffers( PlatformSurface* surface ) {
    assert( surface );
    struct Win32Surface* win32_surface = surface;

    SwapBuffers( win32_surface->hDc );
}
void win32_surface_gl_swap_interval(
    PlatformSurface* surface, int interval
) {
    assert( surface );
    wglSwapIntervalEXT( interval );
}
void* win32_gl_load_proc( const char* function_name ) {
    void* result = (void*)wglGetProcAddress( function_name );
    if( !result ) {
        HMODULE opengl32 = GetModuleHandle( "OPENGL32.DLL" );
        result = (void*)GetProcAddress( opengl32, function_name );
    }
    return result;
}

LRESULT win32_winproc(
    HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam
) {
    struct Win32Surface* win32_surface =
        (struct Win32Surface*)GetWindowLongPtrA( hWnd, GWLP_USERDATA );
    if( !win32_surface ) {
        return DefWindowProcA( hWnd, Msg, wParam, lParam );
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

    switch( Msg ) {
        case WM_CLOSE: {
            if( win32_surface->callbacks.on_close ) {
                win32_surface->callbacks.on_close(
                    win32_surface,
                    win32_surface->callbacks.on_close_params
                );
            }
        } return WM_CLOSE_RETURN_VALUE;

        case WM_ACTIVATEAPP: {
            b32 is_active = wParam == TRUE;

            if( win32_surface->callbacks.on_activate ) {
                win32_surface->callbacks.on_activate(
                    win32_surface, is_active,
                    win32_surface->callbacks.on_activate_params );
            }
        } return WM_ACTIVATEAPP_RETURN_VALUE;

        case WM_WINDOWPOSCHANGED: {
            local RECT last_rect = {};
            RECT rect = {};
            if( GetClientRect( hWnd, &rect ) ) {
                if(
                    rect.right == last_rect.right &&
                    rect.bottom == last_rect.bottom
                ) {
                    return WM_WINDOWPOSCHANGED_RETURN_VALUE;
                }

                i32 width  = rect.right < 1 ? 1 : rect.right;
                i32 height = rect.bottom < 1 ? 1 : rect.bottom;

                i32 old_width  = win32_surface->width;
                i32 old_height = win32_surface->height;

                win32_surface->width  = width;
                win32_surface->height = height;

                if( win32_surface->callbacks.on_resolution_change ) {
                    win32_surface->callbacks.on_resolution_change(
                        win32_surface,
                        old_width, old_height,
                        width, height,
                        win32_surface->callbacks.on_resolution_change_params
                    );
                }

                last_rect = rect;
            }
        } return WM_WINDOWPOSCHANGED_RETURN_VALUE;

        case WM_MOUSEMOVE: {
            RECT client_rect = {};
            GetClientRect( hWnd, &client_rect );

            i32 x = GET_X_LPARAM(lParam);
            i32 y = client_rect.bottom - GET_Y_LPARAM(lParam);

            if( win32_surface->callbacks.on_mouse_move ) {
                win32_surface->callbacks.on_mouse_move(
                    win32_surface, x, y,
                    win32_surface->callbacks.on_mouse_move_params );
            }
        } return WM_MOUSEMOVE_RETURN_VALUE;

        case WM_INPUT: {
            UINT dwSize = sizeof(RAWINPUT);
            BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData(
                (HRAWINPUT)lParam,
                RID_INPUT,
                lpb, &dwSize,
                sizeof(RAWINPUTHEADER) );

            RAWINPUT* raw = (RAWINPUT*)lpb;
            if( raw->header.dwType == RIM_TYPEMOUSE ) {
                i32 x_pos_relative = raw->data.mouse.lLastX;
                i32 y_pos_relative = raw->data.mouse.lLastY;

                if( win32_surface->callbacks.on_mouse_move_relative ) {
                    win32_surface->callbacks.on_mouse_move_relative(
                        win32_surface,
                        -x_pos_relative, -y_pos_relative,
                        win32_surface->callbacks.on_mouse_move_relative_params );
                }
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
            PlatformMouseCode code;
            if( Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP ) {
                code = PLATFORM_MOUSE_BUTTON_LEFT;
            } else if( Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONUP ) {
                code = PLATFORM_MOUSE_BUTTON_RIGHT;
            } else if( Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONUP ) {
                code = PLATFORM_MOUSE_BUTTON_MIDDLE;
            } else {
                return WM_MOUSEBUTTON_RETURN_VALUE;
            }

            if( win32_surface->callbacks.on_mouse_button ) {
                win32_surface->callbacks.on_mouse_button(
                    win32_surface,
                    is_down, code,
                    win32_surface->callbacks.on_mouse_button_params
                );
            }
        } return WM_MOUSEBUTTON_RETURN_VALUE;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: {

            UINT button = GET_XBUTTON_WPARAM(wParam);
            b32 is_down = Msg == WM_XBUTTONDOWN;
            PlatformMouseCode code = button + (PLATFORM_MOUSE_BUTTON_EXTRA_1 - 1);

            if( win32_surface->callbacks.on_mouse_button ) {
                win32_surface->callbacks.on_mouse_button(
                    win32_surface,
                    is_down, code,
                    win32_surface->callbacks.on_mouse_button_params
                );
            }
        } return WM_MOUSEBUTTON_RETURN_VALUE;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL: {

            i64 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            delta = delta == 0 ? 0 : delta;
            delta = delta < 0 ? -delta : delta;

            if( Msg == WM_MOUSEWHEEL ) {
                if( win32_surface->callbacks.on_mouse_wheel ) {
                    win32_surface->callbacks.on_mouse_wheel(
                        win32_surface,
                        false, delta,
                        win32_surface->callbacks.on_mouse_wheel_params
                    );
                }
            } else {
                if( win32_surface->callbacks.on_mouse_wheel ) {
                    win32_surface->callbacks.on_mouse_wheel(
                        win32_surface,
                        true, delta,
                        win32_surface->callbacks.on_mouse_wheel_params
                    );
                }
            }

        } return WM_MOUSEBUTTON_RETURN_VALUE;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            b32 previous_key_state = (lParam >> 30) == 1;
            if( previous_key_state ) {
                return DefWindowProcA( hWnd, Msg, wParam, lParam );
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
                WPARAM new_vkcode = MapVirtualKey(
                    scancode,
                    MAPVK_VSC_TO_VK_EX
                );
                if( new_vkcode == VK_RSHIFT ) {
                    keycode = KEY_SHIFT_RIGHT;
                }
            }

            b32 is_down = !((lParam & TRANSITION_STATE_MASK) != 0);

            if( win32_surface->callbacks.on_key ) {
                win32_surface->callbacks.on_key(
                    win32_surface, is_down,
                    keycode, win32_surface->callbacks.on_key_params
                );
            }
        } return WM_KEY_RETURN_VALUE;

        default: return DefWindowProcA( hWnd, Msg, wParam, lParam );
    }
}

#endif // if not headless

// NOTE(alicia): Time API

void win32_sleep_milliseconds( u32 ms ) {
    DWORD dwMilliseconds = ms;
    Sleep( dwMilliseconds );
}

// NOTE(alicia): IO API

#if !defined(LD_HEADLESS)

void win32_read_gamepads( PlatformGamepad gamepads[4] ) {
    // TODO(alicia): account for XInputGetState stall
    XINPUT_STATE state = {};
    for( DWORD i = 0; i < XUSER_MAX_COUNT; ++i ) {
        PlatformGamepad* current = gamepads + i;
        b32 success = XInputGetState( i, &state ) == ERROR_SUCCESS;
        current->is_active = success;
        if( !success ) {
            continue;
        }
        XINPUT_GAMEPAD   gamepad = state.Gamepad;
        current->buttons = gamepad.wButtons;
        // clear these bits in case xinput uses them
        current->buttons &= ~(
            PLATFORM_GAMEPAD_EXT_BUTTON_TRIGGER_LEFT |
            PLATFORM_GAMEPAD_EXT_BUTTON_TRIGGER_RIGHT );
        current->trigger_left_normalized  = gamepad.bLeftTrigger;
        current->trigger_right_normalized = gamepad.bRightTrigger;
        current->stick_left_x_normalized  = gamepad.sThumbLX;
        current->stick_left_y_normalized  = gamepad.sThumbLY;
        current->stick_right_x_normalized = gamepad.sThumbRX;
        current->stick_right_x_normalized = gamepad.sThumbRY;

        current->buttons |= ( (gamepad.bLeftTrigger > 25) << 2 );
        current->buttons |= ( (gamepad.bRightTrigger > 25) << 3 );
    }
}
void win32_set_gamepad_rumble(
    u32 gamepad_index, u16 motor_left, u16 motor_right
) {
    assert( gamepad_index < 4 );

    XINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed  = motor_left;
    vibration.wRightMotorSpeed = motor_right;

    XInputSetState( gamepad_index, &vibration );
}
void win32_set_mouse_visible( b32 is_visible ) {
    ShowCursor( is_visible );
}

#endif // if not headless

// NOTE(alicia): Misc

PlatformInfo* win32_query_info(void) {
    return &global_win32_info;
}

DWORD win32_report_last_error(void) {
    DWORD error_code = GetLastError();
    if( error_code == ERROR_SUCCESS ) {
        WIN32_ERROR_MESSAGE_BUFFER[0] = ' ';
        WIN32_ERROR_MESSAGE_BUFFER[1] = 0;
        WIN32_ERROR_MESSAGE_LENGTH    = 1;
        return ERROR_SUCCESS;
    }

    DWORD dwFlags = 
        FORMAT_MESSAGE_FROM_SYSTEM     |
        FORMAT_MESSAGE_IGNORE_INSERTS  |
        FORMAT_MESSAGE_MAX_WIDTH_MASK;
    LPCVOID lpSource   = NULL;
    DWORD dwLanguageId = 0;

    DWORD message_length = FormatMessageA(
        dwFlags, lpSource,
        error_code, dwLanguageId,
        WIN32_ERROR_MESSAGE_BUFFER,
        WIN32_ERROR_MESSAGE_BUFFER_SIZE,
        NULL );

    WIN32_ERROR_MESSAGE_LENGTH = message_length;

    return error_code;
}
void win32_last_error( usize* out_error_len, const char** out_error ) {
    *out_error     = WIN32_ERROR_MESSAGE_BUFFER;
    *out_error_len = WIN32_ERROR_MESSAGE_LENGTH;
}

#if !defined(LD_HEADLESS)
void win32_fatal_message_box( const char* title, const char* message ) {
    MessageBoxA( NULL, message, title, MB_ICONERROR );
}
#endif // if not headless

LPSTR* WINAPI CommandLineToArgvA(LPSTR lpCmdline, int* numargs) {
    DWORD argc;
    LPSTR *argv;
    LPSTR s;
    LPSTR d;
    LPSTR cmdline;
    int qcount, bcount;

    if( !numargs || *lpCmdline == 0 ) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    /* --- First count the arguments */
    argc = 1;
    s = lpCmdline;
    /* The first argument, the executable path, follows special rules */
    if( *s == '"' ) {
        /* The executable path ends at the next quote, no matter what */
        s++;
        while( *s ) {
            if( *s++ == '"' ) {
                break;
            }
        }
    } else {
        /* The executable path ends at the next space, no matter what */
        while( *s && *s != ' ' && *s != '\t' ) {
            s++;
        }
    }
    /* skip to the first argument, if any */
    while( *s == ' ' || *s == '\t' ) {
        s++;
    }
    if( *s ) {
        argc++;
    }

    /* Analyze the remaining arguments */
    qcount = bcount=0;
    while( *s ) {
        if( ( *s == ' ' || *s == '\t' ) && qcount == 0 ) {
            /* skip to the next argument and count it if any */
            while( *s == ' ' || *s == '\t' ) {
                s++;
            }
            if( *s ) {
                argc++;
            }
            bcount = 0;
        } else if( *s == '\\' ) {
            /* '\', count them */
            bcount++;
            s++;
        } else if( *s == '"' ) {
            /* '"' */
            if( ( bcount & 1 ) == 0 ) {
                qcount++; /* unescaped '"' */
            }
            s++;
            bcount = 0;
            /* consecutive quotes, see comment in copying code below */
            while( *s == '"' ) {
                qcount++;
                s++;
            }
            qcount = qcount % 3;
            if( qcount == 2 ) {
                qcount=0;
            }
        } else {
            /* a regular character */
            bcount = 0;
            s++;
        }
    }

    /* Allocate in a single lump, the string array, and the strings that go
     * with it. This way the caller can make a single LocalFree() call to free
     * both, as per MSDN.
     */
    argv = (LPSTR*)LocalAlloc(
        LMEM_FIXED,
        ( argc + 1 ) *
        sizeof(LPSTR) +
        ( strlen( lpCmdline ) + 1 ) *
        sizeof(char)
    );
    if( !argv ) {
        return NULL;
    }
    cmdline = (LPSTR)( argv + argc + 1 );
    strcpy( cmdline, lpCmdline );

    /* --- Then split and copy the arguments */
    argv[0] = d = cmdline;
    argc = 1;
    /* The first argument, the executable path, follows special rules */
    if( *d == '"' ) {
        /* The executable path ends at the next quote, no matter what */
        s = d + 1;
        while( *s ) {
            if( *s == '"' ) {
                s++;
                break;
            }
            *d++ = *s++;
        }
    } else {
        /* The executable path ends at the next space, no matter what */
        while( *d && *d != ' ' && *d != '\t' ) {
            d++;
        }
        s = d;
        if( *s ) {
            s++;
        }
    }
    /* close the executable path */
    *d++ = 0;
    /* skip to the first argument and initialize it if any */
    while( *s == ' ' || *s == '\t' ) {
        s++;
    }
    if( !*s ) {
        /* There are no parameters so we are all done */
        argv[argc] = NULL;
        *numargs = argc;
        return argv;
    }

    /* Split and copy the remaining arguments */
    argv[argc++] = d;
    qcount = bcount = 0;
    while( *s ) {
        if( ( *s == ' ' || *s == '\t' ) && qcount == 0 ) {
            /* close the argument */
            *d++ = 0;
            bcount = 0;

            /* skip to the next one and initialize it if any */
            do {
                s++;
            } while( *s == ' ' || *s == '\t' );
            if (*s) {
                argv[argc++] = d;
            }
        } else if( *s == '\\' ) {
            *d++ = *s++;
            bcount++;
        } else if( *s == '"' ) {
            if( ( bcount & 1 ) == 0 ) {
                /* Preceded by an even number of '\', this is half that
                 * number of '\', plus a quote which we erase.
                 */
                d -= bcount/2;
                qcount++;
            } else {
                /* Preceded by an odd number of '\', this is half that
                 * number of '\' followed by a '"'
                 */
                d = d - (bcount / 2) - 1;
                *d++ = '"';
            }
            s++;
            bcount = 0;
            /* Now count the number of consecutive quotes. Note that qcount
             * already takes into account the opening quote if any, as well as
             * the quote that lead us here.
             */
            while( *s == '"' ) {
                if( ++qcount == 3 ) {
                    *d++ = '"';
                    qcount = 0;
                }
                s++;
            }
            if( qcount == 2 ) {
                qcount = 0;
            }
        } else {
            /* a regular character */
            *d++ = *s++;
            bcount = 0;
        }
    }
    *d = '\0';
    argv[argc] = NULL;
    *numargs = argc;

    return argv;
}

#include "shared/custom_cstd.c"

#endif /* Platform Windows */

