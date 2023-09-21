/**
 * Description:  Win32 Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"

#if defined(LD_PLATFORM_WINDOWS)
#include "corec.inl"

#include "core/log.h"
#include "core/strings.h"
#include "core/cstr.h"
#include "core/mem.h"
#include "core/collections.h"
#include "core/events.h"
#include "core/mathf.h"
#include "core/engine.h"

#include "renderer.h"
#include "renderer/opengl/loader.h"

#include <intrin.h>
#include "platform/platform_win32.h"

// NOTE(alicia): GLOBALS

global Win32Platform* PLATFORM = NULL;
#define LOAD_GDI32_FN(function_name) do {\
    function_name = (function_name##FN*)\
    library_load_function( &PLATFORM->lib_gdi32, #function_name );\
    if( !function_name ) { return false; }\
} while(0)
#define WIN32_FULLSCREEN_DWSTYLE         WS_POPUP
#define WIN32_FULLSCREEN_DWEXSTYLE       0

#define WIN32_WINDOWED_RESIZEABLE_DWSTYLE  WS_OVERLAPPEDWINDOW
#define WIN32_WINDOWED_DWSTYLE             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
#define WIN32_WINDOWED_DWEXSTYLE           WS_EX_OVERLAPPEDWINDOW

void* memmove( void* str1, const void* str2, size_t n ) {
    mem_copy_overlapped( str1, str2, n );
    return str1;
}
void* memset( void* ptr, int value, size_t num ) {
    u8 value_8 = *(u8*)&value;
    for( size_t i = 0; i < num; ++i ) {
        *((u8*)ptr + i) = value_8;
    }
    return ptr;
}
void* memcpy( void* destination, const void* source, size_t num ) {
    mem_copy( destination, source, num );
    return destination;
}
char* strcpy( char* dst, const char* src ) {
    char* dst_ = dst;
    const char* src_ = src;
    *dst_++ = *src_;
    while( *src_++ ) {
        *dst_++ = *src_;
    }
    return dst;
}

/*************************************************************************
 * CommandLineToArgvA            [SHELL32.@]
 * 
 * MODIFIED FROM https://www.winehq.org/ project
 * We must interpret the quotes in the command line to rebuild the argv
 * array correctly:
 * - arguments are separated by spaces or tabs
 * - quotes serve as optional argument delimiters
 *   '"a b"'   -> 'a b'
 * - escaped quotes must be converted back to '"'
 *   '\"'      -> '"'
 * - consecutive backslashes preceding a quote see their number halved with
 *   the remainder escaping the quote:
 *   2n   backslashes + quote -> n backslashes + quote as an argument delimiter
 *   2n+1 backslashes + quote -> n backslashes + literal quote
 * - backslashes that are not followed by a quote are copied literally:
 *   'a\b'     -> 'a\b'
 *   'a\\b'    -> 'a\\b'
 * - in quoted strings, consecutive quotes see their number divided by three
 *   with the remainder modulo 3 deciding whether to close the string or not.
 *   Note that the opening quote must be counted in the consecutive quotes,
 *   that's the (1+) below:
 *   (1+) 3n   quotes -> n quotes
 *   (1+) 3n+1 quotes -> n quotes plus closes the quoted string
 *   (1+) 3n+2 quotes -> n+1 quotes plus closes the quoted string
 * - in unquoted strings, the first quote opens the quoted string and the
 *   remaining consecutive quotes follow the above rule.
 */
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
        ( cstr_len( lpCmdline ) + 1 ) *
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

#if defined(DEBUG)
    void __stdcall mainCRTStartup(void) {
#else
    void __stdcall WinMainCRTStartup(void) {
#endif

    DWORD dwMode = 0;
    GetConsoleMode(
        GetStdHandle( STD_OUTPUT_HANDLE ),
        &dwMode
    );
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(
        GetStdHandle( STD_OUTPUT_HANDLE ),
        dwMode
    );

    int argc;
    char** argv = CommandLineToArgvA( GetCommandLineA(), &argc );
    b32 success = engine_entry( argc, argv );
    LocalFree( argv );
    ExitProcess(success ? 0 : -1);
}

// BOOL WINAPI DllMainCRTStartup(
//     HINSTANCE const instance,
//     DWORD     const reason,
//     LPVOID    const reserved
// ) {
//     unused(instance);
//     unused(reserved);
//     switch( reason ) {
//         case DLL_PROCESS_ATTACH:
//         case DLL_THREAD_ATTACH:
//             break;
//         case DLL_PROCESS_DETACH:
//         case DLL_THREAD_DETACH:
//             break;
//     }
//     return TRUE;
// }

internal MONITORINFO win32_monitor_info( HWND opt_window_handle ) {
    HMONITOR monitor = NULL;
    if( opt_window_handle ) {
        monitor = MonitorFromWindow( opt_window_handle, MONITOR_DEFAULTTONEAREST );
    } else {
        // TODO(alicia): cache last point window was at
        // before closing so that the application can always
        // open on the last used monitor
        POINT pt = {};
        monitor = MonitorFromPoint( pt, MONITOR_DEFAULTTOPRIMARY );
    }
    MONITORINFO monitor_info = {};
    monitor_info.cbSize = sizeof(monitor_info);
    if( !GetMonitorInfoA( monitor, &monitor_info ) ) {
        win32_log_error(false);
        panic();
    }
    return monitor_info;
}

internal DWORD WINAPI win32_xinput_polling_thread( void* params ) {
    PlatformSemaphore* semaphore = params;
    loop {
        platform_semaphore_wait( semaphore, true, 0 );

        Event event = {};
        event.data.gamepad_active.active = true;

        XINPUT_STATE unused_gamepad_state = {};
        for( u32 i = 0; i < GAMEPAD_MAX_INDEX; ++i ) {
            if( !input_gamepad_is_active( i ) ) {
                if( SUCCEEDED(XInputGetState( i, &unused_gamepad_state )) ) {
                    input_set_gamepad_active( i, true );
                    event.data.gamepad_active.index = i;
                    event_fire( event );
                }
            }
        }
    }
    return 0;
}

usize PLATFORM_SUBSYSTEM_SIZE = sizeof(Win32Platform);
b32 platform_subsystem_init( void* buffer ) {

    assert( buffer );
    PLATFORM = buffer;

    if( !library_load(
        "USER32.DLL",
        &PLATFORM->lib_user32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load user32.dll!"
        );
        return false;
    }

    #define LOAD_USER32_FN(function_name) do {\
        function_name = (function_name##FN*)\
        library_load_function( &PLATFORM->lib_user32, #function_name );\
        if( !function_name ) { return false; }\
    } while(0)

    LOAD_USER32_FN(SetWindowPlacement);
    LOAD_USER32_FN(GetWindowPlacement);
    LOAD_USER32_FN(GetMonitorInfoA);
    LOAD_USER32_FN(MonitorFromPoint);
    LOAD_USER32_FN(MonitorFromWindow);
    LOAD_USER32_FN(SetWindowPos);
    LOAD_USER32_FN(CreateWindowExA);
    LOAD_USER32_FN(RegisterClassExA);
    LOAD_USER32_FN(AdjustWindowRectEx);
    LOAD_USER32_FN(LoadImageA);
    LOAD_USER32_FN(GetWindowLongPtrA);
    LOAD_USER32_FN(DefWindowProcA);
    LOAD_USER32_FN(GetClientRect);
    LOAD_USER32_FN(MapVirtualKeyA);
    LOAD_USER32_FN(DestroyWindow);
    LOAD_USER32_FN(PeekMessageA);
    LOAD_USER32_FN(TranslateMessage);
    LOAD_USER32_FN(DestroyIcon);
    LOAD_USER32_FN(GetDC);
    LOAD_USER32_FN(ShowWindow);
    LOAD_USER32_FN(SetWindowLongPtrA);
    LOAD_USER32_FN(MessageBoxA);
    LOAD_USER32_FN(DispatchMessageA);
    LOAD_USER32_FN(SetWindowTextA);
    LOAD_USER32_FN(GetWindowTextLengthA);
    LOAD_USER32_FN(GetWindowTextA);
    LOAD_USER32_FN(SetCursorPos);
    LOAD_USER32_FN(ClientToScreen);
    LOAD_USER32_FN(ShowCursor);
    LOAD_USER32_FN(SetCursor);
    LOAD_USER32_FN(LoadCursorA);
    LOAD_USER32_FN(GetSystemMetrics);

    if( !library_load(
        "XINPUT1_4.DLL",
        &PLATFORM->lib_xinput
    ) ) {
        if( !library_load(
            "XINPUT9_1_0.DLL",
            &PLATFORM->lib_xinput
        ) ) {
            if( !library_load(
                "XINPUT1_3.DLL",
                &PLATFORM->lib_xinput
            ) ) {
                MESSAGE_BOX_FATAL(
                    "Failed to load library!",
                    "Failed to load any version of XInput!"
                );
                return false;
            }
        }
    }

    #define LOAD_XINPUT_FN(function_name) do {\
        function_name = (function_name##FN*)\
        library_load_function( &PLATFORM->lib_xinput, #function_name );\
        if( !function_name ) { return false; }\
    } while(0)

    LOAD_XINPUT_FN(XInputGetState);
    LOAD_XINPUT_FN(XInputSetState);

    XInputEnableFN* xinput_enable =
    (XInputEnableFN*)library_load_function(
        &PLATFORM->lib_xinput,
        "XInputEnable"
    );
    if( xinput_enable ) {
        XInputEnable = xinput_enable;
    }

    if( !library_load(
        "GDI32.DLL",
        &PLATFORM->lib_gdi32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load gdi32.dll!"
        );
        return false;
    }
    LOAD_GDI32_FN( GetStockObject );
    LOAD_GDI32_FN( GetDeviceCaps );

    read_write_fence();
    
    /// create a thread to poll for new xinput devices because
    /// of XInputGetState stall. Thanks Microsoft!
    PlatformSemaphore* xinput_polling_thread_semaphore =
        platform_semaphore_create( 0, 1 );
    if( !xinput_polling_thread_semaphore ) {
        return false;
    }
    PLATFORM->xinput_polling_thread_semaphore =
        xinput_polling_thread_semaphore;

    read_write_fence();

    DWORD xinput_polling_thread_id = 0;
    HANDLE xinput_polling_thread_handle = CreateThread(
        NULL, STACK_SIZE,
        win32_xinput_polling_thread,
        PLATFORM->xinput_polling_thread_semaphore,
        0, &xinput_polling_thread_id
    );

    if( !xinput_polling_thread_handle ) {
        win32_log_error( true );
        return false;
    }
    PLATFORM->xinput_polling_thread    = xinput_polling_thread_handle;
    PLATFORM->xinput_polling_thread_id = xinput_polling_thread_id;

    WIN32_LOG_NOTE(
        "Created XInput polling thread. ID: {u}",
        PLATFORM->xinput_polling_thread_id
    );

    PLATFORM->instance = GetModuleHandle( NULL );

    HICON window_icon = NULL;
    window_icon = (HICON)LoadImageA(
        NULL,
        SURFACE_ICON_PATH,
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTSIZE | LR_LOADFROMFILE
    );
    if( !window_icon ) {
        WIN32_LOG_WARN("Failed to load window icon!");
    }
    PLATFORM->icon = window_icon;

    PLATFORM->cursor_style   = CURSOR_STYLE_ARROW;
    PLATFORM->cursor_visible = true;

    QueryPerformanceFrequency( &PLATFORM->performance_frequency );
    QueryPerformanceCounter( &PLATFORM->performance_counter );

    WIN32_LOG_INFO( "Platform subsystem successfully initialized." );
    return true;
}

usize PLATFORM_SURFACE_BUFFER_SIZE = sizeof(Win32Surface);
b32 PLATFORM_SUPPORTS_MULTIPLE_SURFACES = true;
b32 platform_surface_create(
    ivec2 surface_dimensions, const char* surface_name,
    enum RendererBackend backend,
    PlatformSurfaceCreateFlags flags, PlatformSurface* out_surface
) {
    unused(backend);
    assert( out_surface );
    Win32Surface* win32_surface = out_surface;

    b32 show_on_create =
        !bitfield_check( flags, PLATFORM_SURFACE_CREATE_HIDDEN );
    b32 is_resizeable =
        bitfield_check( flags, PLATFORM_SURFACE_CREATE_RESIZEABLE );
    b32 create_fullscreen =
        bitfield_check( flags, PLATFORM_SURFACE_CREATE_FULLSCREEN );

    WNDCLASSEX window_class = {};
    window_class.cbSize        = sizeof(WNDCLASSEX);
    window_class.lpfnWndProc   = win32_winproc;
    window_class.hInstance     = PLATFORM->instance;
    window_class.lpszClassName = "LiquidEngineWindowClass";
    window_class.hbrBackground = (HBRUSH)GetStockBrush( BLACK_BRUSH );
    window_class.hIcon         = PLATFORM->icon;
    window_class.hCursor =
        LoadCursorA( PLATFORM->instance, IDC_ARROW );

    if( !RegisterClassEx( &window_class ) ) {
        win32_log_error( true );
        return false;
    }


    MONITORINFO monitor_info = win32_monitor_info( NULL );
    ivec2 dimensions = {};

    DWORD dwExStyle = 0;
    DWORD dwStyle   = 0;

    if( create_fullscreen ) {
        dimensions.x =
            monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
        dimensions.y =
            monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;

        dwStyle   = WIN32_FULLSCREEN_DWSTYLE;
        dwExStyle = WIN32_FULLSCREEN_DWEXSTYLE;
    } else {
        dwExStyle = WIN32_WINDOWED_DWEXSTYLE;
        if( is_resizeable ) {
            dwStyle = WIN32_WINDOWED_RESIZEABLE_DWSTYLE;
        } else {
            dwStyle = WIN32_WINDOWED_DWSTYLE;
        }
        dimensions = surface_dimensions;
    }

    RECT window_rect = {};
    
    window_rect.right  = dimensions.x;
    window_rect.bottom = dimensions.y;

    if( !AdjustWindowRectEx(
        &window_rect,
        dwStyle, FALSE,
        dwExStyle
    ) ) {
        win32_log_error( true );
        return false;
    }

    HWND handle = CreateWindowEx(
        dwExStyle,
        window_class.lpszClassName,
        surface_name,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        NULL,
        NULL,
        PLATFORM->instance,
        NULL
    );
    if( !handle ) {
        win32_log_error( true );
        return false;
    }
    HDC device_context = GetDC( handle );
    if( !device_context ) {
        win32_log_error( true );
        return false;
    }

    win32_surface->handle         = handle;
    win32_surface->device_context = device_context;
    win32_surface->dwStyle        = dwStyle;
    win32_surface->dwExStyle      = dwExStyle;
    win32_surface->dimensions     = dimensions;
    win32_surface->creation_flags = flags;
    win32_surface->on_resize      = NULL;
    win32_surface->on_close       = NULL;

    SetWindowLongPtrA( handle, GWLP_USERDATA, (LONG_PTR)out_surface );

    b32 is_active = false;
    if( show_on_create ) {
        is_active = true;
        ShowWindow( handle, SW_SHOW );
    }

    win32_surface->is_active = is_active;

    return true;
}
void platform_surface_destroy( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    DestroyWindow( win32_surface->handle );
}
void platform_surface_set_dimensions(
    PlatformSurface* surface, ivec2 dimensions
) {
    assert( surface );
    Win32Surface* win32_surface = surface;

    if( win32_surface->mode != PLATFORM_SURFACE_MODE_FLOATING_WINDOW ) {
        return;
    }

    ivec2 old_dimensions = win32_surface->dimensions;

    RECT window_rect = {};

    window_rect.right  = dimensions.width;
    window_rect.bottom = dimensions.height;

    win32_surface->dimensions = dimensions;

    AdjustWindowRectEx(
        &window_rect,
        win32_surface->dwStyle,
        FALSE,
        win32_surface->dwExStyle
    );

    HWND hwnd_insert_after = NULL;
    if( win32_surface->mode == PLATFORM_SURFACE_MODE_FULLSCREEN ) {
        hwnd_insert_after = HWND_TOP;
    }

    SetWindowPos(
        win32_surface->handle,
        hwnd_insert_after,
        0, 0,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        SWP_NOMOVE | SWP_NOREPOSITION
    );

    if( win32_surface->on_resize ) {
        win32_surface->on_resize(
            win32_surface,
            old_dimensions,
            dimensions,
            win32_surface->on_resize_user_params
        );
    }
}
ivec2 platform_surface_query_dimensions( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    return win32_surface->dimensions;
}
void platform_surface_set_mode(
    PlatformSurface* surface, PlatformSurfaceMode mode
) {
    assert( surface );
    Win32Surface* win32_surface = surface;

    if( win32_surface->mode == mode ) {
        return;
    }

    win32_surface->mode = mode;
    switch( mode ) {
        case PLATFORM_SURFACE_MODE_FLOATING_WINDOW: {
            b32 is_resizeable = bitfield_check(
                win32_surface->creation_flags,
                PLATFORM_SURFACE_CREATE_RESIZEABLE
            );

            win32_surface->dwExStyle = WIN32_WINDOWED_DWEXSTYLE;
            if( is_resizeable ) {
                win32_surface->dwStyle = WIN32_WINDOWED_RESIZEABLE_DWSTYLE;
            } else {
                win32_surface->dwStyle = WIN32_WINDOWED_DWSTYLE;
            }

            SetWindowLongPtrA(
                win32_surface->handle,
                GWL_STYLE,
                win32_surface->dwStyle
            );
            SetWindowLongPtrA(
                win32_surface->handle,
                GWL_EXSTYLE,
                win32_surface->dwExStyle
            );

            SetWindowPlacement(
                win32_surface->handle, &win32_surface->placement );

            SetWindowPos(
                win32_surface->handle,
                NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED
            );

        } break;
        case PLATFORM_SURFACE_MODE_FULLSCREEN: {

            mem_zero( &win32_surface->placement, sizeof(WINDOWPLACEMENT) );
            win32_surface->placement.length = sizeof(WINDOWPLACEMENT);
            BOOL placement_result = GetWindowPlacement(
                win32_surface->handle, &win32_surface->placement );
            if( !placement_result ) {
                win32_log_error(false);
                panic();
            }

            win32_surface->dwExStyle = WIN32_FULLSCREEN_DWEXSTYLE;
            win32_surface->dwStyle   = WIN32_FULLSCREEN_DWSTYLE;

            MONITORINFO monitor_info =
                win32_monitor_info( win32_surface->handle );

            SetWindowLongPtrA(
                win32_surface->handle,
                GWL_STYLE,
                win32_surface->dwStyle
            );
            SetWindowLongPtrA(
                win32_surface->handle,
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
                win32_surface->handle,
                HWND_TOP,
                x, y,
                width, height,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW
            );

            ivec2 old_dimensions = win32_surface->dimensions;
            win32_surface->dimensions = (ivec2){ width, height };

            if( win32_surface->on_resize ) {
                win32_surface->on_resize(
                    win32_surface,
                    old_dimensions,
                    win32_surface->dimensions,
                    win32_surface->on_resize_user_params
                );
            }
        } break;
        default: {
            UNIMPLEMENTED();
        } break;
    }
}
PlatformSurfaceMode platform_surface_query_mode( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    return win32_surface->mode;
}
void platform_surface_set_name( PlatformSurface* surface, const char* name ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    SetWindowTextA( win32_surface->handle, name );
}
void platform_surface_query_name(
    PlatformSurface* surface, usize* surface_name_buffer_size,
    char* surface_name_buffer
) {
    assert( surface );
    Win32Surface* win32_surface = surface;

    int text_length = GetWindowTextLengthA( win32_surface->handle );

    if( !surface_name_buffer ) {
        *surface_name_buffer_size = (usize)text_length;
        return;
    }

    GetWindowTextA(
        win32_surface->handle,
        surface_name_buffer,
        *surface_name_buffer_size
    );
    *surface_name_buffer_size = (usize)text_length;

}
b32 platform_surface_query_active( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    return win32_surface->is_active;
}
void platform_surface_show( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    ShowWindow( win32_surface->handle, SW_SHOW );
}
void platform_surface_hide( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    ShowWindow( win32_surface->handle, SW_HIDE );
}
void platform_surface_center( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    if( win32_surface->mode != PLATFORM_SURFACE_MODE_FLOATING_WINDOW ) {
        return;
    }

    MONITORINFO monitor_info = win32_monitor_info( win32_surface->handle );

    /// window dimensions
    i32 window_width  = win32_surface->dimensions.x;
    i32 window_height = win32_surface->dimensions.y;

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

    SetWindowPos(
        win32_surface->handle,
        NULL,
        x, y,
        0, 0,
        SWP_NOSIZE
    );
}
void platform_surface_set_close_callback(
    PlatformSurface* surface,
    PlatformSurfaceOnCloseFN* close_callback,
    void* user_params
) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    if( win32_surface->on_close ) {
        WIN32_LOG_WARN(
            "Setting window close callback when "
            "the callback was already set, was this intended? "
            "Call platform_surface_clear_close_callback() "
            "before this function just to be sure."
        );
    }

    win32_surface->on_close = close_callback;
    win32_surface->on_close_user_params = user_params;
}
void platform_surface_clear_close_callback( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    win32_surface->on_close = NULL;
    win32_surface->on_close_user_params = NULL;
}
void platform_surface_set_activate_callback(
    PlatformSurface* surface,
    PlatformSurfaceOnActivateFN* activate_callback,
    void* user_params
) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    win32_surface->on_activate  = activate_callback;
    win32_surface->on_activate_user_params = user_params;
}
void platform_surface_clear_activate_callback( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    win32_surface->on_activate  = NULL;
    win32_surface->on_activate_user_params = NULL;
}
void platform_surface_set_resize_callback(
    PlatformSurface* surface,
    PlatformSurfaceOnResizeFN* resize_callback,
    void* user_params
) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    if( win32_surface->on_resize ) {
        WIN32_LOG_WARN(
            "Setting window resize callback when "
            "the callback was already set, was this intended? "
            "Call platform_surface_clear_resize_callback() "
            "before this function just to be sure."
        );
    }

    win32_surface->on_resize = resize_callback;
    win32_surface->on_resize_user_params = user_params;
}
void platform_surface_clear_resize_callback( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    win32_surface->on_resize = NULL;
    win32_surface->on_resize_user_params = NULL;
}
void platform_surface_pump_events( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;

    MSG message = {};
    while( PeekMessage(
        &message,
        win32_surface->handle,
        0, 0,
        PM_REMOVE
    ) ) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }
}
void platform_win32_signal_xinput_polling_thread(void) {
    platform_semaphore_increment( PLATFORM->xinput_polling_thread_semaphore );
}

f64 platform_us_elapsed(void) {
    LARGE_INTEGER current_ticks;
    QueryPerformanceCounter( &current_ticks );
    u64 ticks_elapsed = current_ticks.QuadPart -
        PLATFORM->performance_counter.QuadPart;

    return (f64)(ticks_elapsed * 1000000.0) /
        (f64)PLATFORM->performance_frequency.QuadPart;
}
f64 platform_ms_elapsed(void) {
    LARGE_INTEGER current_ticks;
    QueryPerformanceCounter( &current_ticks );
    u64 ticks_elapsed = current_ticks.QuadPart -
        PLATFORM->performance_counter.QuadPart;

    return (f64)(ticks_elapsed * 1000.0) /
        (f64)PLATFORM->performance_frequency.QuadPart;
}
f64 platform_s_elapsed(void) {
    LARGE_INTEGER current_ticks;
    QueryPerformanceCounter( &current_ticks );
    u64 ticks_elapsed = current_ticks.QuadPart -
        PLATFORM->performance_counter.QuadPart;

    return (f64)(ticks_elapsed) /
        (f64)PLATFORM->performance_frequency.QuadPart;
}

internal inline LPCTSTR cursor_style_to_win32_style( CursorStyle style ) {
    const LPCTSTR styles[CURSOR_STYLE_COUNT] = {
        IDC_ARROW,
        IDC_SIZENS,
        IDC_SIZEWE,
        IDC_SIZENESW,
        IDC_SIZENWSE,
        IDC_IBEAM,
        IDC_HAND,
        IDC_WAIT,
        IDC_NO
    };
    assert( style < CURSOR_STYLE_COUNT );
    return styles[style];
}
CursorStyle platform_cursor_style(void) {
    return PLATFORM->cursor_style;
}
b32 platform_cursor_visible(void) {
    return PLATFORM->cursor_visible;
}
void platform_cursor_set_style( CursorStyle cursor_style ) {
    PLATFORM->cursor_style = cursor_style;

    LPCTSTR win32_style = cursor_style_to_win32_style( cursor_style );
    SetCursor( LoadCursor( NULL, win32_style ) );
}
void platform_cursor_set_visible( b32 visible ) {
    PLATFORM->cursor_visible = visible;
    ShowCursor( visible );
}
void platform_cursor_center( PlatformSurface* surface ) {

    assert( surface );
    Win32Surface* win32_surface = surface;

    POINT center = {};
    center.x = win32_surface->dimensions.width  / 2;
    center.y = win32_surface->dimensions.height / 2;

    ClientToScreen( win32_surface->handle, &center );
    SetCursorPos( center.x, center.y );
}
void platform_sleep( u32 ms ) {
    DWORD dwMilliseconds = ms;
    Sleep( dwMilliseconds );
}
void platform_set_gamepad_motor_state(
    u32 gamepad_index, u32 motor, f32 value
) {
    XINPUT_VIBRATION vibration = {};
    if( motor == GAMEPAD_MOTOR_LEFT ) {
        f32 right_motor = input_gamepad_motor_state(
            gamepad_index,
            GAMEPAD_MOTOR_RIGHT
        );
        vibration.wLeftMotorSpeed  = (WORD)( value * (f32)U16_MAX );
        vibration.wRightMotorSpeed = (WORD)( right_motor * (f32)U16_MAX );
    } else {
        f32 left_motor = input_gamepad_motor_state(
            gamepad_index,
            GAMEPAD_MOTOR_LEFT
        );
        vibration.wLeftMotorSpeed  = (WORD)( left_motor * (f32)U16_MAX );
        vibration.wRightMotorSpeed = (WORD)( value * (f32)U16_MAX );
    }

    XInputSetState( gamepad_index, &vibration );
}
void platform_poll_gamepad(void) {
    XINPUT_STATE gamepad_state = {};
    DWORD max_index = XUSER_MAX_COUNT > GAMEPAD_MAX_INDEX ?
        GAMEPAD_MAX_INDEX : XUSER_MAX_COUNT;

    Event event = {};
    for(
        DWORD gamepad_index = 0;
        gamepad_index < max_index;
        ++gamepad_index
    ) {

        b32 is_active = input_gamepad_is_active( gamepad_index );
        if( !is_active ) {
            continue;
        }
        b32 xinput_get_state_success = XInputGetState(
            gamepad_index,
            &gamepad_state
        ) == ERROR_SUCCESS;

        // if failed to get xinput state, fire an event
        if( !xinput_get_state_success ) {
            event.code = EVENT_CODE_GAMEPAD_ACTIVE;
            event.data.gamepad_active.index  = gamepad_index;
            event.data.gamepad_active.active = false;
            event_fire( event );
            input_set_gamepad_active( gamepad_index, false );
            continue;
        }

        XINPUT_GAMEPAD gamepad = gamepad_state.Gamepad;

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_LEFT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_RIGHT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_UP,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_DPAD_DOWN,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_LEFT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_X )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_RIGHT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_B )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_UP,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_Y )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_FACE_DOWN,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_A )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_START,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_START )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_SELECT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_BACK )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_BUMPER_LEFT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_BUMPER_RIGHT,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER )
        );

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_LEFT_CLICK,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB )
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_RIGHT_CLICK,
            bitfield_check( gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB )
        );

        f32 trigger_press_threshold =
            input_gamepad_trigger_press_threshold( gamepad_index );

        f32 trigger_left_deadzone =
            input_gamepad_trigger_left_deadzone( gamepad_index );
        f32 trigger_right_deadzone =
            input_gamepad_trigger_right_deadzone( gamepad_index );

        f32 trigger_left  = normalize_range_u8_f32( gamepad.bLeftTrigger );
        f32 trigger_right = normalize_range_u8_f32( gamepad.bRightTrigger );

        if( trigger_left >= trigger_left_deadzone ) {
            trigger_left = remap32(
                trigger_left_deadzone, 1.0f,
                0.0f, 1.0f,
                trigger_left
            );
        } else {
            trigger_left = 0;
        }
        if( trigger_right >= trigger_right_deadzone ) {
            trigger_right = remap32(
                trigger_right_deadzone, 1.0f,
                0.0f, 1.0f,
                trigger_right
            );
        } else {
            trigger_right = 0;
        }

        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_TRIGGER_LEFT,
            trigger_left >= trigger_press_threshold
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_TRIGGER_RIGHT,
            trigger_right >= trigger_press_threshold
        );

        input_set_gamepad_trigger_left(
            gamepad_index,
            trigger_left
        );
        input_set_gamepad_trigger_right(
            gamepad_index,
            trigger_right
        );

        vec2 stick_left = v2(
            normalize_range_i16_f32( gamepad.sThumbLX ),
            normalize_range_i16_f32( gamepad.sThumbLY )
        );
        vec2 stick_right = v2(
            normalize_range_i16_f32( gamepad.sThumbRX ),
            normalize_range_i16_f32( gamepad.sThumbRY )
        );

        f32 stick_left_magnitude  = v2_mag( stick_left );
        f32 stick_right_magnitude = v2_mag( stick_right );

        vec2 stick_left_direction = stick_left_magnitude >= 0 ?
            v2_div( stick_left, stick_left_magnitude ) : VEC2_ZERO;
        vec2 stick_right_direction = stick_right_magnitude >= 0 ?
            v2_div( stick_right, stick_right_magnitude ) : VEC2_ZERO;

        f32 stick_left_deadzone  =
            input_gamepad_stick_left_deadzone( gamepad_index );
        f32 stick_right_deadzone =
            input_gamepad_stick_right_deadzone( gamepad_index );

        if( stick_left_magnitude >= stick_left_deadzone ) {
            stick_left_magnitude = remap32(
                stick_left_deadzone, 1.0f,
                0.0f, 1.0f,
                stick_left_magnitude
            );
        } else {
            stick_left_magnitude = 0;
        }
        if( stick_right_magnitude >= stick_right_deadzone ) {
            stick_right_magnitude = remap32(
                stick_right_deadzone, 1.0f,
                0.0f, 1.0f,
                stick_right_magnitude
            );
        } else {
            stick_right_magnitude = 0;
        }

        stick_left  = v2_mul( stick_left_direction, stick_left_magnitude );
        stick_right = v2_mul( stick_right_direction, stick_right_magnitude );
        
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_LEFT,
            stick_left_magnitude  >= 0
        );
        input_set_gamepad_button(
            gamepad_index,
            GAMEPAD_CODE_STICK_RIGHT,
            stick_right_magnitude >= 0
        );
        
        input_set_gamepad_stick_left(
            gamepad_index,
            stick_left
        );

        input_set_gamepad_stick_right(
            gamepad_index,
            stick_right
        );
    }
}
void platform_gl_surface_swap_buffers( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    SwapBuffers( win32_surface->device_context );
}

internal HGLRC win32_gl_create_context( HDC device_context ) {

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
        device_context, &desired_pixel_format );

    PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
    DescribePixelFormat(
        device_context, pixelFormatIndex,
        pixel_format_size, &suggested_pixel_format
    );

    if( SetPixelFormat(
        device_context,
        pixelFormatIndex,
        &suggested_pixel_format
    ) == FALSE ) {
        win32_log_error( true );
        return NULL;
    }

    HGLRC temp = wglCreateContext( device_context );
    if(!temp) {
        win32_log_error( false );
        return NULL;
    }

    if( wglMakeCurrent( device_context, temp ) == FALSE ) {
        WIN32_LOG_ERROR("Failed to make temp OpenGL context current!");
        return NULL;
    }

    wglCreateContextAttribsARB = (wglCreateContextAttribsARBFN*)
        wglGetProcAddress("wglCreateContextAttribsARB");

    if(!wglCreateContextAttribsARB) {
        WIN32_LOG_ERROR(
            "Failed to load function "
            "\"wglCreateContextAttribsARB\"!"
        );
        return NULL;
    }

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, GL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB, GL_VERSION_MINOR,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,
        0
    };

    HGLRC result = wglCreateContextAttribsARB(
        device_context, NULL, attribs );

    wglDeleteContext( temp );
    if(!result) {
        WIN32_LOG_ERROR(
            "wglCreateContextAttribsARB "
            "failed to create OpenGL context!"
        );
        return NULL;
    }
    wglMakeCurrent( device_context, result );

    return result;
}
global b32 GL_FUNCTIONS_LOADED = false;
void* win32_gl_load_proc( const char* function_name ) {
    void* function = (void*)wglGetProcAddress( function_name );
    if( !function ) {
        HMODULE lib_gl = GetModuleHandle( "OPENGL32.DLL" );
        LOG_ASSERT( lib_gl, "OpenGL module was not loaded!" );
        function = (void*)GetProcAddress(
            lib_gl,
            function_name
        );

#if defined(LD_LOGGING)
        if( !function ) {
            WIN32_LOG_WARN(
                "Failed to load GL function \"{cc}\"!",
                function_name
            );
        }
#endif
    }

    return function;
}

void glSwapInterval( PlatformSurface* surface, int interval ) {
    unused( surface );
    wglSwapIntervalEXT( interval );
}

b32 platform_gl_surface_init( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;

    if( !GL_FUNCTIONS_LOADED ) {
        if( !library_load(
            "OPENGL32.DLL",
            &PLATFORM->lib_gl
        ) ) {
            MESSAGE_BOX_FATAL(
                "Failed to load library!",
                "Failed to load opengl32.dll!"
            );
            return false;
        }

        #define LOAD_OPENGL_FN(function_name) do {\
            function_name = (function_name##FN*)\
            library_load_function( &PLATFORM->lib_gl, #function_name );\
            if( !function_name ) { return false; }\
        } while(0)

        LOAD_OPENGL_FN(wglGetCurrentContext);
        LOAD_OPENGL_FN(wglCreateContext);
        LOAD_OPENGL_FN(wglMakeCurrent);
        LOAD_OPENGL_FN(wglDeleteContext);
        LOAD_OPENGL_FN(wglGetProcAddress);

        LOAD_GDI32_FN(DescribePixelFormat);
        LOAD_GDI32_FN(ChoosePixelFormat);
        LOAD_GDI32_FN(SetPixelFormat);
        LOAD_GDI32_FN(SwapBuffers);

    }

    HGLRC glrc =
        win32_gl_create_context( win32_surface->device_context );
    if( !glrc ) {
        return false;
    }

    if( !GL_FUNCTIONS_LOADED ) {
        if( !gl_load_functions( win32_gl_load_proc ) ) {
            WIN32_LOG_FATAL( "Failed to load OpenGL functions!" );
            return false;
        }

        #define LOAD_WGL_FN(wgl_name) do {\
            wgl_name = (wgl_name##FN *)\
                wglGetProcAddress( #wgl_name );\
            if( !wgl_name ) {\
                WIN32_LOG_ERROR( "Failed to load wgl function '" #wgl_name "'!" );\
            }\
        } while(0)

        LOAD_WGL_FN( wglSwapIntervalEXT );

        GL_FUNCTIONS_LOADED = true;
    }

    win32_surface->glrc = glrc;
    return true;
}
void platform_gl_surface_shutdown( PlatformSurface* surface ) {
    assert( surface );
    Win32Surface* win32_surface = surface;
    wglMakeCurrent(
        win32_surface->device_context, NULL );
    wglDeleteContext( win32_surface->glrc );
}
void platform_query_system_info( struct SystemInfo* sysinfo ) {
    SYSTEM_INFO win32_info = {};
    GetSystemInfo( &win32_info );

    if( IsProcessorFeaturePresent(
        PF_XMMI_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= SSE_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_XMMI64_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= SSE2_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= SSE3_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= SSSE3_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_1_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= SSE4_1_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_2_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= SSE4_2_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= AVX_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX2_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= AVX2_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX512F_INSTRUCTIONS_AVAILABLE
    ) ) {
        sysinfo->features |= AVX512_MASK;
    }

    MEMORYSTATUSEX memory_status = {};
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx( &memory_status );

    sysinfo->total_memory = memory_status.ullTotalPhys;
    sysinfo->logical_processor_count = win32_info.dwNumberOfProcessors;

#if defined(LD_ARCH_X86)
    mem_set(
        sysinfo->cpu_name_buffer,
        ' ',
        CPU_NAME_BUFFER_SIZE
    );
    sysinfo->cpu_name_buffer[CPU_NAME_BUFFER_SIZE - 1] = 0;

    int cpu_info[4] = {};
    __cpuid( cpu_info, 0x80000002 );
    mem_copy(
        sysinfo->cpu_name_buffer,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000003 );
    mem_copy(
        sysinfo->cpu_name_buffer + 16,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000004 );
    mem_copy(
        sysinfo->cpu_name_buffer + 32,
        cpu_info,
        sizeof(cpu_info)
    );
#endif
}

LRESULT win32_winproc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam ) {

    Win32Surface* win32_surface =
        (Win32Surface*)GetWindowLongPtrA( hWnd, GWLP_USERDATA );

    #define DEFAULT_IMPLEMENTATION()\
        DefWindowProc( hWnd, Msg, wParam, lParam )

    if( !win32_surface ) {
        return DEFAULT_IMPLEMENTATION();
    }

    #define TRANSITION_STATE_MASK (1 << 31)
    #define EXTENDED_KEY_MASK     (1 << 24)
    #define SCANCODE_MASK         0x00FF0000

    switch( Msg ) {
        case WM_CLOSE: {
            if( win32_surface->on_close ) {
                win32_surface->on_close(
                    win32_surface,
                    win32_surface->on_close_user_params
                );
            }
        } return 0;

        case WM_ACTIVATE: {
            b32 is_active = wParam == WA_ACTIVE ||
                wParam == WA_CLICKACTIVE;
            if( !is_active ) {
                platform_cursor_set_visible( true );
            }

            if( win32_surface->on_activate ) {
                win32_surface->on_activate(
                    win32_surface, is_active,
                    win32_surface->on_activate_user_params
                );
            }
            win32_surface->is_active = is_active;
        } break;

        case WM_WINDOWPOSCHANGED: {
            local RECT last_rect = {};
            RECT rect = {};
            if( GetClientRect( hWnd, &rect ) ) {
                if(
                    last_rect.right  == rect.right &&
                    last_rect.bottom == rect.bottom
                ) {
                    last_rect = rect;
                    return DEFAULT_IMPLEMENTATION();
                }

                #define MIN_DIMENSIONS 1
                ivec2 dimensions = {
                    max( rect.right, MIN_DIMENSIONS ),
                    max( rect.bottom, MIN_DIMENSIONS ),
                };
                if( win32_surface->on_resize ) {
                    win32_surface->on_resize(
                        win32_surface,
                        win32_surface->dimensions,
                        dimensions,
                        win32_surface->on_resize_user_params
                    );
                }
                win32_surface->dimensions = dimensions;

                last_rect = rect;
            }
        } return FALSE;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP: {

            if( !win32_surface->is_active ) {
                return DEFAULT_IMPLEMENTATION();
            }

            b32 previous_key_state = (lParam >> 30) == 1;
            if( previous_key_state ) {
                return DEFAULT_IMPLEMENTATION();
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
            input_set_key( (KeyboardCode)keycode, is_down );

        } return TRUE;
        
        case WM_MOUSEMOVE: {

            if( !win32_surface->is_active ) {
                return DEFAULT_IMPLEMENTATION();
            }

            // TODO(alicia): figure this out with multiple windows
            RECT client_rect = {};
            GetClientRect( hWnd, &client_rect );

            ivec2 mouse_position = {};
            mouse_position.x = GET_X_LPARAM(lParam);
            mouse_position.y = client_rect.bottom - GET_Y_LPARAM(lParam);
            input_set_mouse_position( mouse_position );

        } return TRUE;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {

            if( !win32_surface->is_active ) {
                return DEFAULT_IMPLEMENTATION();
            }

            b32 is_down =
                Msg == WM_LBUTTONDOWN ||
                Msg == WM_MBUTTONDOWN ||
                Msg == WM_RBUTTONDOWN;
            MouseCode code;
            if( Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP ) {
                code = MOUSE_BUTTON_LEFT;
            } else if( Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONUP ) {
                code = MOUSE_BUTTON_RIGHT;
            } else if( Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONUP ) {
                code = MOUSE_BUTTON_MIDDLE;
            } else {
                break;
            }

            input_set_mouse_button( code, is_down );

        } return TRUE;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: {

            if( !win32_surface->is_active ) {
                return DEFAULT_IMPLEMENTATION();
            }

            UINT button = GET_XBUTTON_WPARAM(wParam);
            b32 is_down = Msg == WM_XBUTTONDOWN;
            MouseCode code = (MouseCode)(button + (MOUSE_BUTTON_EXTRA_1 - 1));

            input_set_mouse_button( code, is_down );

        } return TRUE;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL: {

            if( !win32_surface->is_active ) {
                return DEFAULT_IMPLEMENTATION();
            }

            i64 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            delta = delta == 0 ? 0 : absolute(delta);

            if( Msg == WM_MOUSEWHEEL ) {
                input_set_mouse_wheel( delta );
            } else {
                input_set_horizontal_mouse_wheel( delta );
            }


        } return TRUE;

        case WM_SETCURSOR: switch( LOWORD( lParam ) ) {
            case HTRIGHT:
            case HTLEFT: {
                platform_cursor_set_style(
                    CURSOR_STYLE_RESIZE_HORIZONTAL
                );
            } break;

            case HTTOP:
            case HTBOTTOM: {
                platform_cursor_set_style(
                    CURSOR_STYLE_RESIZE_VERTICAL
                );
            } break;

            case HTBOTTOMLEFT:
            case HTTOPRIGHT: {
                platform_cursor_set_style(
                    CURSOR_STYLE_RESIZE_TOP_RIGHT_BOTTOM_LEFT
                );
            } break;

            case HTBOTTOMRIGHT:
            case HTTOPLEFT: {
                platform_cursor_set_style(
                    CURSOR_STYLE_RESIZE_TOP_LEFT_BOTTOM_RIGHT
                );
            } break;

            default: {
                platform_cursor_set_style(
                    CURSOR_STYLE_ARROW
                );
            } break;
        } return TRUE;

    }

    return DEFAULT_IMPLEMENTATION();
}

MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
) {
    HWND   hWnd      = NULL;
    LPCSTR lpText    = message;
    LPCSTR lpCaption = window_title;

    b32  valid_type = true;
    UINT uType = 0;
    switch( type ) {
        case MESSAGE_BOX_TYPE_OK:
            uType |= MB_OK;
            break;
        case MESSAGE_BOX_TYPE_OKCANCEL:
            uType |= MB_OKCANCEL;
            break;
        case MESSAGE_BOX_TYPE_RETRYCANCEL:
            uType |= MB_RETRYCANCEL;
            break;
        case MESSAGE_BOX_TYPE_YESNO:
            uType |= MB_YESNO;
            break;
        case MESSAGE_BOX_TYPE_YESNOCANCEL:
            uType |= MB_YESNOCANCEL;
            break;
        default:
            valid_type = false;
            break;
    }

    if( !valid_type ) {
        WIN32_LOG_ERROR("Message Box requires a valid type.");
        return MESSAGE_BOX_RESULT_UNKNOWN_ERROR;
    }

    switch( icon ) {
        case MESSAGE_BOX_ICON_INFORMATION:
            uType |= MB_ICONASTERISK;
            break;
        case MESSAGE_BOX_ICON_WARNING:
            uType |= MB_ICONWARNING;
            break;
        case MESSAGE_BOX_ICON_ERROR:
            uType |= MB_ICONERROR;
            break;
        default: break;
    }

    int mb_result = MessageBoxA(
        hWnd,
        lpText,
        lpCaption,
        uType
    );

    MessageBoxResult result;
    switch( mb_result ) {
        case IDOK:
            result = MESSAGE_BOX_RESULT_OK;
            break;
        case IDYES:
            result = MESSAGE_BOX_RESULT_YES;
            break;
        case IDNO:
            result = MESSAGE_BOX_RESULT_NO;
            break;
        case IDRETRY:
            result = MESSAGE_BOX_RESULT_RETRY;
            break;
        case IDCANCEL:
            result = MESSAGE_BOX_RESULT_CANCEL;
            break;
        default:
            WIN32_LOG_ERROR("Message Box returned an unknown result.");
            result = MESSAGE_BOX_RESULT_UNKNOWN_ERROR;
            break;
    }

    return result;
}

PlatformFile* platform_file_open(
    const char*          path,
    PlatformFileOpenFlag flags
) {

    DWORD dwDesiredAccess = 0;
    if( bitfield_check( flags, PLATFORM_FILE_OPEN_READ ) ) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if( bitfield_check( flags, PLATFORM_FILE_OPEN_WRITE ) ) {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    DWORD dwShareMode = 0;
    if( bitfield_check( flags, PLATFORM_FILE_OPEN_SHARE_READ ) ) {
        dwShareMode |= FILE_SHARE_READ;
    }
    if( bitfield_check( flags, PLATFORM_FILE_OPEN_SHARE_WRITE ) ) {
        dwShareMode |= FILE_SHARE_WRITE;
    }

    DWORD dwCreationDisposition = 0;
    if( bitfield_check( flags, PLATFORM_FILE_OPEN_EXISTING ) ) {
        dwCreationDisposition |= OPEN_EXISTING;
    } else {
        dwCreationDisposition |= OPEN_ALWAYS;
    }

    HANDLE handle = CreateFile(
        path,
        dwDesiredAccess,
        dwShareMode,
        NULL,
        dwCreationDisposition,
        0,
        NULL
    );
    if( handle == INVALID_HANDLE_VALUE ) {
        WIN32_LOG_ERROR( "Path: {cc}", path );
        return false;
    }

    WIN32_LOG_NOTE(
        "File {u64,x} at path \"{cc}\" opened.",
        (u64)handle, path
    );

    return handle;
}
void platform_file_close( PlatformFile* file ) {
    WIN32_LOG_NOTE( "File {u64,x} closed.", (u64)file );
    CloseHandle( (HANDLE)file );
}
b32 platform_file_read(
    PlatformFile* file,
    usize read_size,
    usize buffer_size,
    void* buffer
) {
    LOG_ASSERT(
        read_size < U32_MAX,
        "platform_file_read does not support reads over 4GB on Win32!"
    );

    if( read_size > buffer_size ) {
        WIN32_LOG_ERROR(
            "Attempted to read a file into a buffer that isn't large enough! "
            "Read size: {u64} Buffer size: {u64}",
            read_size, buffer_size
        );
        return false;
    }

    DWORD bytes_to_read = (DWORD)read_size;
    DWORD bytes_read = 0;
    if( !ReadFile(
        (HANDLE)file,
        buffer,
        bytes_to_read,
        &bytes_read,
        NULL
    ) ) {
        win32_log_error( false );
        return false;
    } else {
        if( bytes_read != bytes_to_read ) {
            WIN32_LOG_ERROR(
                "Failed to read requested bytes! "
                "Requested bytes: {u} Bytes read: {u}",
                bytes_to_read, bytes_read
            );
            return false;
        }
        return true;
    }
}
b32 platform_file_write(
    PlatformFile* file,
    usize write_size,
    usize buffer_size,
    void* buffer
) {
    assert( buffer_size >= write_size );
    assert( (u64)U32_MAX >= write_size );
    DWORD bytes_to_write = write_size;
    DWORD bytes_written = 0;

    BOOL write_result = WriteFile(
        (HANDLE)file,
        buffer,
        bytes_to_write,
        &bytes_written,
        NULL
    );
    if( !write_result || bytes_written != bytes_to_write ) {
        win32_log_error( false );
        return false;
    }

    return true;
}
usize platform_file_query_size( PlatformFile* file ) {
    LARGE_INTEGER result = {};
    if( GetFileSizeEx( (HANDLE)file, &result ) ) {
        return result.QuadPart;
    } else {
        win32_log_error( false );
        return 0;
    }
}
usize platform_file_query_offset( PlatformFile* file ) {

    LARGE_INTEGER offset = {};
    LARGE_INTEGER result = {};
    SetFilePointerEx(
        (HANDLE)file,
        offset,
        &result,
        FILE_CURRENT
    );

    return result.QuadPart;
}
b32 platform_file_set_offset( PlatformFile* file, usize offset ) {
    LARGE_INTEGER large_offset = {};
    large_offset.QuadPart = offset;

    if( !SetFilePointerEx(
        (HANDLE)file,
        large_offset,
        NULL,
        FILE_BEGIN
    ) ) {
        win32_log_error( false );
        return false;
    } else {
        return true;
    }
}

PlatformLibrary* platform_library_load( const char* library_path ) {
    HMODULE module = LoadLibraryA( library_path );
    if( !module ) {
        win32_log_error(false);
    }
    return module;
}
void platform_library_free( PlatformLibrary* library ) {
    FreeLibrary( (HMODULE)library );
}
void* platform_library_load_function(
    PlatformLibrary* library,
    const char*      function_name
) {
    void* function = (void*)GetProcAddress( (HMODULE)library, function_name );
    if( !function ) {
        win32_log_error( false );
    }
    return function;
}

#define ERROR_MESSAGE_BUFFER_SIZE 512ULL
global char ERROR_MESSAGE_BUFFER[ERROR_MESSAGE_BUFFER_SIZE] = {};
DWORD win32_log_error( b32 present_message_box ) {
    DWORD error_code = GetLastError();
    if( error_code == ERROR_SUCCESS ) {
        return error_code;
    }

    DWORD message_length = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL,
        error_code,
        0,
        ERROR_MESSAGE_BUFFER,
        ERROR_MESSAGE_BUFFER_SIZE,
        NULL
    );

    if( message_length ) {
        WIN32_LOG_ERROR(
            "{u}: {cc}",
            error_code,
            ERROR_MESSAGE_BUFFER
        );

        if( present_message_box ) {
            StringSlice error_message = {};
            error_message.buffer = ERROR_MESSAGE_BUFFER + message_length;
            error_message.len    =
                ERROR_MESSAGE_BUFFER_SIZE - ( message_length + 1 );
            ss_mut_fmt(
                &error_message,
                "Encountered a fatal Windows error!\n"
                LD_CONTACT_MESSAGE "\n{c}",
                0
            );

            MESSAGE_BOX_FATAL(
                "Fatal Windows Error",
                ERROR_MESSAGE_BUFFER
            );
        }
    }

    return error_code;
}

void* platform_heap_alloc( usize size ) {
    void* pointer = (void*)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        size
    );
    return pointer;
}
void* platform_heap_realloc( void* memory, usize old_size, usize new_size ) {
    unused(old_size);
    void* pointer = (void*)HeapReAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        memory,
        new_size
    );

    return pointer;
}
void platform_heap_free( void* memory ) {
    HeapFree( GetProcessHeap(), 0, memory );
}

void* platform_page_alloc( usize size ) {
    // VirtualAlloc returns automatically zeroed memory.
    void* pointer = (void*)VirtualAlloc(
        NULL,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    return pointer;
}
void platform_page_free( void* memory ) {
    VirtualFree(
        memory,
        0,
        MEM_RELEASE | MEM_DECOMMIT
    );
}

internal DWORD WINAPI win32_thread_proc( void* params ) {
    struct PlatformThread* thread  = params;
    ThreadProcFN*      proc        = thread->proc;
    void*              user_params = thread->params;
    PlatformSemaphore* ready       = thread->ready;

    read_write_fence();

    platform_semaphore_increment( ready );

    b32 result = proc( user_params );

    return result ? ERROR_SUCCESS : -1;
}

b32 platform_thread_create(
    ThreadProcFN*   thread_proc,
    void*           user_params,
    usize           thread_stack_size
) {

    struct PlatformThread thread = {};
    thread.proc   = thread_proc;
    thread.params = user_params;
    thread.ready  = platform_semaphore_create( NULL, 0 );
    if( !thread.ready ) {
        WIN32_LOG_ERROR( "Failed to create thread!" );
        WIN32_LOG_ERROR( "Failed to create thread ready semaphore!" );
        return false;
    }

    read_write_fence();

    DWORD thread_id = 0;
    HANDLE thread_handle = CreateThread(
        NULL, thread_stack_size,
        win32_thread_proc, &thread,
        0, &thread_id
    );
    if( !thread_handle ) {
        win32_log_error( true );
        return false;
    }

    read_write_fence();

    platform_semaphore_wait( thread.ready, true, 0 );

    WIN32_LOG_NOTE( "New thread created! ID: {u}", thread_id );
    return true;

}
PlatformSemaphore* platform_semaphore_create(
    const char* opt_name, u32 initial_count
) {
    HANDLE result = CreateSemaphoreEx(
        NULL,
        initial_count,
        I32_MAX,
        opt_name,
        0,
        SEMAPHORE_ALL_ACCESS
    );
    if( !result ) {
        win32_log_error( false );
        return NULL;
    }

    return (PlatformSemaphore*)result;
}
void platform_semaphore_increment( PlatformSemaphore* semaphore ) {
    ReleaseSemaphore(
        (HANDLE)semaphore,
        1, NULL
    );
}
void platform_semaphore_wait(
    PlatformSemaphore* semaphore,
    b32 infinite_timeout, u32 opt_timeout_ms
) {
    WaitForSingleObjectEx(
        (HANDLE)semaphore,
        infinite_timeout ?
            INFINITE : opt_timeout_ms,
        FALSE
    );
}
void platform_semaphore_destroy( PlatformSemaphore* semaphore ) {
    CloseHandle( (HANDLE)semaphore );
}
PlatformMutex* platform_mutex_create(void) {
    return CreateMutexA( NULL, false, NULL );
}
void platform_mutex_lock( PlatformMutex* mutex ) {
    WaitForSingleObject( mutex, INFINITE );
}
void platform_mutex_unlock( PlatformMutex* mutex ) {
    ReleaseMutex( (HANDLE)mutex );
}
void platform_mutex_destroy( PlatformMutex* mutex ) {
    CloseHandle( (HANDLE)mutex );
}
u32 platform_interlocked_increment_u32( volatile u32* addend ) {
    return InterlockedIncrement( (LONG volatile*)addend );
}
u32 platform_interlocked_decrement_u32( volatile u32* addend ) {
    return InterlockedDecrement( (LONG volatile*)addend );
}
u32 platform_interlocked_exchange_u32( volatile u32* target, u32 value ) {
    return InterlockedExchange( (LONG volatile*)target, (LONG)value );
}
u32 platform_interlocked_compare_exchange_u32(
    volatile u32* dst,
    u32 exchange, u32 comperand
) {
    return InterlockedCompareExchange(
        (LONG volatile*)dst,
        (LONG)exchange,
        (LONG)comperand
    );
}
void* platform_interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
) {
    return InterlockedCompareExchangePointer(
        dst,
        exchange,
        comperand
    );
}

void* platform_stdout_handle(void) {
    return GetStdHandle( STD_OUTPUT_HANDLE );
}
void* platform_stderr_handle(void) {
    return GetStdHandle( STD_ERROR_HANDLE );
}
void platform_write_console(
    void* output_handle,
    usize write_count,
    const char* buffer
) {
    WriteConsole(
        output_handle,
        buffer,
        write_count,
        NULL, NULL
    );
}

void platform_win32_output_debug_string( const char* str ) {
    OutputDebugStringA( str );
}

#endif // LD_PLATFORM_WINDOWS
