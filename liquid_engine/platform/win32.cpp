/**
 * Description:  Win32 Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "win32.h"

#if defined(LD_PLATFORM_WINDOWS)
#include "corecpp.inl"

#include "core/logging.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/collections.h"
#include "core/event.h"
#include "core/math.h"
#include "core/engine.h"

#include "renderer/renderer.h"

// #define VK_USE_PLATFORM_WIN32_KHR
// #include <vulkan/vulkan.h>
// #include "renderer/vulkan/vk_backend.h"

#include "renderer/opengl/gl_backend.h"

#include <intrin.h>

global b32 IS_DPI_AWARE = false;

extern "C" {

#if defined(LD_COMPILER_MSVC)
    #pragma function(memset)
    int _fltused{0x9875}
#endif
void* memset( void* ptr, int value, size_t num ) {
    u8 value_8 = *(u8*)&value;
    for( size_t i = 0; i < num; ++i ) {
        *((u8*)ptr + i) = value_8;
    }
    return ptr;
}

#if defined(LD_COMPILER_MSVC)
    #pragma function(memcpy)
#endif
void* memcpy( void* destination, const void* source, size_t num ) {
    mem_copy( destination, source, num );
    return destination;
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

char* strcpy( char* dst, const char* src ) {
    char* dst_ = dst;
    const char* src_ = src;
    *dst_++ = *src_;
    while( *src_++ ) {
        *dst_++ = *src_;
    }
    return dst;
}

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
        ( str_length( lpCmdline ) + 1 ) *
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


void __stdcall mainCRTStartup() {

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
//     [[maybe_unused]]
//     HINSTANCE const instance,
//     [[maybe_unused]]
//     DWORD     const reason,
//     [[maybe_unused]]
//     LPVOID    const reserved
// ) {
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

}

/// Every x number of frames, check if xinput gamepad is active
#define POLL_FOR_NEW_XINPUT_GAMEPAD_RATE (20000)
internal DWORD WINAPI win32_xinput_polling_thread( void* params ) {
    SemaphoreHandle semaphore = (SemaphoreHandle)params;
    loop {
        semaphore_wait( semaphore, true, 0 );

        Event event = {};
        event.data.bool32[1] = true;
        XINPUT_STATE unused_gamepad_state = {};
        for( u32 i = 0; i < MAX_GAMEPAD_INDEX; ++i ) {
            if( !input_pad_is_active( i ) ) {
                if( SUCCEEDED(XInputGetState( i, &unused_gamepad_state )) ) {
                    input_set_pad_active( i, true );
                    event.data.uint32[0] = i;
                    event_fire( event );
                }
            }
        }
    }
    return 0;
}

u32 query_platform_subsystem_size() {
    return sizeof(Win32Platform);
}
b32 platform_init(
    StringView opt_icon_path,
    ivec2 surface_dimensions,
    PlatformFlags flags,
    Platform* out_platform
) {

    ASSERT( out_platform );
    Win32Platform* win32_platform = (Win32Platform*)out_platform;

    IS_DPI_AWARE = ARE_BITS_SET( flags, PLATFORM_DPI_AWARE );

    // load libraries
    if( !win32_load_user32( &win32_platform->lib_user32 ) ) {
        return false;
    }
    if( !win32_load_xinput( &win32_platform->lib_xinput ) ) {
        return false;
    }

    read_write_fence();
    
    /// create a thread to poll for new xinput devices because
    /// of XInputGetState stall. Thanks Microsoft!
    SemaphoreHandle xinput_polling_thread_semaphore = {};
    if( !semaphore_create( 0, 1, &xinput_polling_thread_semaphore ) ) {
        return false;
    }

    Win32ThreadHandle xinput_polling_thread_handle = {};
    xinput_polling_thread_handle.thread_handle = CreateThread(
        nullptr,
        STACK_SIZE,
        win32_xinput_polling_thread,
        nullptr,
        0,
        &xinput_polling_thread_handle.thread_id
    );
    if( !xinput_polling_thread_handle.thread_handle ) {
        win32_log_error( true );
        return false;
    }

    win32_platform->xinput_polling_thread = xinput_polling_thread_handle;
    win32_platform->xinput_polling_thread_semaphore = xinput_polling_thread_semaphore;
    WIN32_LOG_NOTE(
        "Created XInput polling thread. ID: {u}",
        win32_platform->xinput_polling_thread.thread_id
    );

    if( !library_load(
        "GDI32.DLL",
        (void**)&win32_platform->lib_gdi32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load gdi32.dll!"
        );
        return false;
    }
    GetStockObject = (::impl::GetStockObjectFN)library_load_function(
        win32_platform->lib_gdi32,
        "GetStockObject"
    );
    if( !GetStockObject ) {
        MESSAGE_BOX_FATAL(
            "Failed to load function!",
            "Failed to load GetStockObject!"
        );
        return false;
    }

    win32_platform->instance = GetModuleHandle( nullptr );

    HICON window_icon = nullptr;
    if( opt_icon_path.buffer ) {
        window_icon = (HICON)LoadImageA(
            nullptr,
            opt_icon_path.buffer,
            IMAGE_ICON,
            0, 0,
            LR_DEFAULTSIZE | LR_LOADFROMFILE
        );
        if( !window_icon ) {
            win32_log_error(true);
            return false;
        }
    }

    // create window
    WNDCLASSEX windowClass    = {};
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc   = win32_winproc;
    windowClass.hInstance     = win32_platform->instance;
    windowClass.lpszClassName = "LiquidEngineWindowClass";
    windowClass.hbrBackground = (HBRUSH)GetStockBrush(BLACK_BRUSH);
    windowClass.hIcon         = window_icon;
    windowClass.hCursor       = LoadCursor(
        win32_platform->instance,
        IDC_ARROW
    );

    if( !RegisterClassEx( &windowClass ) ) {
        win32_log_error( true );
        return false;
    }

    DWORD dwStyle   = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

    i32 width = 0, height = 0;
    RECT window_rect = {};
    if( IS_DPI_AWARE ) {
        SetProcessDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        );
        UINT dpi = GetDpiForSystem();

        width = MulDiv(
            surface_dimensions.width,
            dpi, 96
        );
        height = MulDiv(
            surface_dimensions.height,
            dpi, 96
        );

        window_rect.right  = width;
        window_rect.bottom = height;
        if( !AdjustWindowRectExForDpi(
            &window_rect,
            dwStyle,
            FALSE,
            dwExStyle,
            dpi
        ) ) {
            win32_log_error( true );
            return false;
        }
    } else {
        width  = surface_dimensions.width;
        height = surface_dimensions.height;

        window_rect.right  = surface_dimensions.width;
        window_rect.bottom = surface_dimensions.height;
        if( !AdjustWindowRectEx(
            &window_rect,
            dwStyle,
            FALSE,
            dwExStyle
        ) ) {
            win32_log_error( true );
            return false;
        }
    }

    i32 x = 0, y = 0; {
        ivec2 screen_center = ivec2{
            GetSystemMetrics( SM_CXSCREEN ),
            GetSystemMetrics( SM_CYSCREEN )
        } / 2;

        x = screen_center.x - (width / 2);
        y = screen_center.y - (height / 2);
    }
    
    HWND hWnd = CreateWindowEx(
        dwExStyle,
        windowClass.lpszClassName,
        "Liquid Engine",
        dwStyle,
        x, y,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        nullptr,
        nullptr,
        win32_platform->instance,
        nullptr
    );
    if( !hWnd ) {
        win32_log_error( true );
        return false;
    }
    DestroyIcon( window_icon );

    HDC dc = GetDC( hWnd );
    if( !dc ) {
        win32_log_error( true );
        return false;
    }

    win32_platform->window.handle         = hWnd;
    win32_platform->window.device_context = dc;
    win32_platform->cursor.style      = CURSOR_ARROW;
    win32_platform->cursor.is_visible = true;

    ShowWindow(
        win32_platform->window.handle,
        SW_SHOW
    );

    QueryPerformanceFrequency(
        &win32_platform->performance_frequency
    );
    QueryPerformanceCounter(
        &win32_platform->performance_counter
    );

    out_platform->surface.dimensions = { width, height };
    out_platform->is_active = true;

    SetWindowLongPtr(
        win32_platform->window.handle,
        GWLP_USERDATA,
        (LONG_PTR)out_platform
    );

    WIN32_LOG_INFO( "Platform subsystem successfully initialized." );
    return true;
}
void platform_shutdown( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;

    semaphore_destroy( win32_platform->xinput_polling_thread_semaphore );

    for( u32 i = 0; i < MODULE_COUNT; ++i ) {
        if( win32_platform->modules[i] ) {
            library_free( win32_platform->modules[i] );
        }
    }

    DestroyWindow( win32_platform->window.handle );
}

void platform_exit() {
    ExitProcess( 0 );
}
u64 platform_ticks() {
    LARGE_INTEGER result = {};
    QueryPerformanceCounter( &result );
    return (u64)result.QuadPart;
}
u64 platform_ticks_per_second() {
    LARGE_INTEGER result = {};
    QueryPerformanceFrequency( &result );
    return result.QuadPart;
}
u64 platform_ticks_elapsed( Platform* generic_platform ) {
    Win32Platform* platform = (Win32Platform*)generic_platform;
    u64 ticks = platform_ticks();
    u64 start_ticks = platform->performance_counter.QuadPart;
    return ticks - start_ticks;
}
f64 platform_us_elapsed( Platform* generic_platform ) {
    Win32Platform* platform = (Win32Platform*)generic_platform;
    u64 frequency = platform->performance_frequency.QuadPart;
    u64 ticks_elapsed = platform_ticks_elapsed( generic_platform );

    return (f64)(ticks_elapsed * 1000000.0) / (f64)frequency;
}
f64 platform_ms_elapsed( Platform* generic_platform ) {
    Win32Platform* platform = (Win32Platform*)generic_platform;
    u64 frequency = platform->performance_frequency.QuadPart;
    u64 ticks_elapsed = platform_ticks_elapsed( generic_platform );

    return (f64)(ticks_elapsed * 1000.0) / (f64)frequency;
}
f64 platform_s_elapsed( Platform* generic_platform ) {
    Win32Platform* platform = (Win32Platform*)generic_platform;
    u64 frequency = platform->performance_frequency.QuadPart;
    u64 ticks_elapsed = platform_ticks_elapsed( generic_platform );

    return (f64)(ticks_elapsed) / (f64)frequency;
}
b32 platform_pump_events( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    MSG message = {};
    while( PeekMessage(
        &message,
        win32_platform->window.handle,
        0, 0,
        PM_REMOVE
    ) ) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }

    if( ( win32_platform->event_pump_count %
        POLL_FOR_NEW_XINPUT_GAMEPAD_RATE
    ) == 0 ) {
        semaphore_increment(
            win32_platform->xinput_polling_thread_semaphore,
            1, 0
        );
    }

    win32_platform->event_pump_count++;

    return true;
}
void platform_surface_set_name(
    Platform* platform,
    StringView name
) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    SetWindowTextA(
        win32_platform->window.handle,
        name.buffer
    );
}
i32 platform_surface_read_name(
    Platform* platform,
    char* buffer, usize max_buffer_size
) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    usize text_length = GetWindowTextLengthA(
        win32_platform->window.handle
    );
    b32 window_text_longer_than_buffer = text_length > max_buffer_size;

    GetWindowTextA(
        win32_platform->window.handle,
        buffer,
        window_text_longer_than_buffer ?
            max_buffer_size : text_length
    );

    return window_text_longer_than_buffer ? text_length : 0;
}
inline internal LPCTSTR cursor_style_to_win32_style(
    CursorStyle style
) {
    local const LPCTSTR styles[CURSOR_COUNT] = {
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
    if( style >= CURSOR_COUNT ) {
        return IDC_ARROW;
    }
    return styles[style];
}
void platform_cursor_set_style(
    Platform* platform, CursorStyle cursor_style
) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    win32_platform->cursor.style = cursor_style;

    LPCTSTR win32_style = cursor_style_to_win32_style( cursor_style );
    SetCursor(
        LoadCursor( nullptr, win32_style )
    );
}
void platform_cursor_set_visible( Platform* platform, b32 visible ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    ShowCursor( visible );
    win32_platform->cursor.is_visible = visible;
}
void platform_cursor_center( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    POINT center = {};
    center.x = platform->surface.width  / 2;
    center.y = platform->surface.height / 2;

    ClientToScreen(
        win32_platform->window.handle,
        &center
    );
    SetCursorPos( center.x, center.y );
}
void platform_sleep( Platform*, u32 ms ) {
    DWORD dwMilliseconds = ms;
    Sleep( dwMilliseconds );
}
void platform_set_pad_motor_state(
    Platform*,
    u32 gamepad_index, u32 motor, f32 value
) {
    XINPUT_VIBRATION vibration = {};
    if( motor == GAMEPAD_MOTOR_LEFT ) {
        f32 right_motor = input_pad_read_motor_state(
            gamepad_index,
            GAMEPAD_MOTOR_RIGHT
        );
        vibration.wLeftMotorSpeed  = (WORD)( value * (f32)U16::MAX );
        vibration.wRightMotorSpeed = (WORD)( right_motor * (f32)U16::MAX );
    } else {
        f32 left_motor = input_pad_read_motor_state(
            gamepad_index,
            GAMEPAD_MOTOR_LEFT
        );
        vibration.wLeftMotorSpeed  = (WORD)( left_motor * (f32)U16::MAX );
        vibration.wRightMotorSpeed = (WORD)( value * (f32)U16::MAX );
    }

    XInputSetState( gamepad_index, &vibration );
}
void platform_poll_gamepad( Platform* platform ) {
    if( !platform->is_active ) {
        return;
    }

    XINPUT_STATE gamepad_state = {};
    DWORD max_index = XUSER_MAX_COUNT > MAX_GAMEPAD_INDEX ?
        MAX_GAMEPAD_INDEX : XUSER_MAX_COUNT;

    Event event = {};
    for(
        DWORD gamepad_index = 0;
        gamepad_index < max_index;
        ++gamepad_index
    ) {

        b32 is_active = input_pad_is_active( gamepad_index );
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
            event.data.uint32[0] = gamepad_index;
            event.data.bool32[1] = false;
            event_fire( event );
            input_set_pad_active( gamepad_index, false );
            continue;
        }

        XINPUT_GAMEPAD gamepad = gamepad_state.Gamepad;

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_LEFT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_RIGHT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_UP,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_DOWN,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN )
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_LEFT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_X )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_RIGHT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_B )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_UP,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_Y )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_DOWN,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_A )
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_START,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_START )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_SELECT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_BACK )
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_BUMPER_LEFT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_BUMPER_RIGHT,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER )
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_LEFT_CLICK,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB )
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_RIGHT_CLICK,
            ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB )
        );

        f32 trigger_press_threshold =
            input_pad_read_trigger_press_threshold( gamepad_index );

        f32 trigger_left_deadzone =
            input_pad_read_trigger_left_deadzone( gamepad_index );
        f32 trigger_right_deadzone =
            input_pad_read_trigger_right_deadzone( gamepad_index );

        f32 trigger_left  = normalize_range( gamepad.bLeftTrigger );
        f32 trigger_right = normalize_range( gamepad.bRightTrigger );

        if( trigger_left >= trigger_left_deadzone ) {
            trigger_left = remap(
                trigger_left_deadzone, 1.0f,
                0.0f, 1.0f,
                trigger_left
            );
        } else {
            trigger_left = 0;
        }
        if( trigger_right >= trigger_right_deadzone ) {
            trigger_right = remap(
                trigger_right_deadzone, 1.0f,
                0.0f, 1.0f,
                trigger_right
            );
        } else {
            trigger_right = 0;
        }

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_TRIGGER_LEFT,
            trigger_left >= trigger_press_threshold
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_TRIGGER_RIGHT,
            trigger_right >= trigger_press_threshold
        );

        input_set_pad_trigger_left(
            gamepad_index,
            trigger_left
        );
        input_set_pad_trigger_right(
            gamepad_index,
            trigger_right
        );

        vec2 stick_left = v2(
            normalize_range( gamepad.sThumbLX ),
            normalize_range( gamepad.sThumbLY )
        );
        vec2 stick_right = v2(
            normalize_range( gamepad.sThumbRX ),
            normalize_range( gamepad.sThumbRY )
        );

        f32 stick_left_magnitude  = mag( stick_left );
        f32 stick_right_magnitude = mag( stick_right );

        vec2 stick_left_direction  = stick_left_magnitude >= 0 ?
            stick_left  / stick_left_magnitude  : VEC2::ZERO;
        vec2 stick_right_direction = stick_right_magnitude >= 0 ?
            stick_right / stick_right_magnitude : VEC2::ZERO;

        f32 stick_left_deadzone  = input_pad_read_stick_left_deadzone( gamepad_index );
        f32 stick_right_deadzone = input_pad_read_stick_right_deadzone( gamepad_index );

        if( stick_left_magnitude >= stick_left_deadzone ) {
            stick_left_magnitude = remap(
                stick_left_deadzone, 1.0f,
                0.0f, 1.0f,
                stick_left_magnitude
            );
        } else {
            stick_left_magnitude = 0;
        }
        if( stick_right_magnitude >= stick_right_deadzone ) {
            stick_right_magnitude = remap(
                stick_right_deadzone, 1.0f,
                0.0f, 1.0f,
                stick_right_magnitude
            );
        } else {
            stick_right_magnitude = 0;
        }

        stick_left  = stick_left_magnitude * stick_left_direction;
        stick_right = stick_right_magnitude * stick_right_direction;
        
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_LEFT,
            stick_left_magnitude  >= 0
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_RIGHT,
            stick_right_magnitude >= 0
        );
        
        input_set_pad_stick_left(
            gamepad_index,
            stick_left
        );

        input_set_pad_stick_right(
            gamepad_index,
            stick_right
        );
    }
}
// struct VkSurfaceKHR_T* platform_vk_create_surface(
//     Platform* platform,
//     struct VulkanRendererContext* ctx
// ) {
//     Win32Platform* win32_platform = (Win32Platform*)platform;
    
//     VkWin32SurfaceCreateInfoKHR create_info = {};
//     create_info.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
//     create_info.hinstance = win32_platform->instance;
//     create_info.hwnd      = win32_platform->window.handle;

//     VkSurfaceKHR vk_surface = {};

//     VkResult result = vkCreateWin32SurfaceKHR(
//         ctx->instance,
//         &create_info,
//         ctx->allocator,
//         &vk_surface
//     );

//     if( result != VK_SUCCESS ) {
//         return nullptr;
//     }

//     return vk_surface;

// }
usize platform_vk_read_ext_names(
    Platform*,
    usize max_names,
    usize* name_count,
    const char** names
) {
    usize win32_ext_count = STATIC_ARRAY_COUNT( WIN32_VULKAN_EXTENSIONS );
    usize max_count = win32_ext_count > max_names ?
        max_names : win32_ext_count;
    
    usize count = *name_count;
    for( usize i = 0; i < max_count; ++i ) {
        names[count++] = WIN32_VULKAN_EXTENSIONS[i];
        win32_ext_count--;
    }

    *name_count = count;
    return win32_ext_count;
}
void platform_gl_swap_buffers( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    SwapBuffers( win32_platform->window.device_context );
}
internal HGLRC win32_gl_create_context( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;

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
        win32_platform->window.device_context,
        &desired_pixel_format
    );
    PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
    DescribePixelFormat(
        win32_platform->window.device_context, pixelFormatIndex,
        pixel_format_size, &suggested_pixel_format
    );

    if( SetPixelFormat(
        win32_platform->window.device_context,
        pixelFormatIndex,
        &suggested_pixel_format
    ) == FALSE ) {
        win32_log_error( true );
        return nullptr;
    }

    HGLRC temp = wglCreateContext( win32_platform->window.device_context );
    if(!temp) {
        win32_log_error( false );
        return nullptr;
    }

    if( wglMakeCurrent(
        win32_platform->window.device_context,
        temp ) == FALSE
    ) {
        WIN32_LOG_ERROR("Failed to make temp OpenGL context current!");
        return nullptr;
    }

    wglCreateContextAttribsARB = (impl::wglCreateContextAttribsARBFN)
        wglGetProcAddress("wglCreateContextAttribsARB");

    if(!wglCreateContextAttribsARB) {
        WIN32_LOG_ERROR(
            "Failed to load function "
            "\"wglCreateContextAttribsARB\"!"
        );
        return nullptr;
    }

    i32 attribs[] = {
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_MAJOR_VERSION_ARB, GL_VERSION_MAJOR,
        WGL_CONTEXT_MINOR_VERSION_ARB, GL_VERSION_MINOR,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };

    HGLRC result = wglCreateContextAttribsARB(
        win32_platform->window.device_context, nullptr, attribs
    );
    wglDeleteContext( temp );
    if(!result) {
        WIN32_LOG_ERROR(
            "wglCreateContextAttribsARB "
            "failed to create OpenGL context!"
        );
        return nullptr;
    }
    wglMakeCurrent( win32_platform->window.device_context, result );

    return result;
}
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
void* platform_gl_init( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;

    if( !win32_load_opengl( win32_platform ) ) {
        return nullptr;
    }

    HGLRC gl_context = win32_gl_create_context( platform );
    if( !gl_context ) {
        return nullptr;
    }

    if( !gl_load( win32_gl_load_proc ) ) {
        WIN32_LOG_FATAL( "Failed to load OpenGL functions!" );
        return nullptr;
    }

    return (void*)gl_context;
}
void platform_gl_shutdown( Platform* platform, void* glrc ) {
    Win32Platform* win32_platform = (Win32Platform*)platform;
    wglMakeCurrent(
        win32_platform->window.device_context,
        nullptr
    );
    wglDeleteContext( (HGLRC)glrc );
}
SystemInfo query_system_info() {
    SystemInfo result = {};

    SYSTEM_INFO win32_info = {};
    GetSystemInfo( &win32_info );

    if( IsProcessorFeaturePresent(
        PF_XMMI_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= SSE_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_XMMI64_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= SSE2_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= SSE3_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= SSSE3_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_1_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= SSE4_1_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_2_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= SSE4_2_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= AVX_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX2_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= AVX2_MASK;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX512F_INSTRUCTIONS_AVAILABLE
    ) ) {
        result.features |= AVX512_MASK;
    }

    MEMORYSTATUSEX memory_status = {};
    memory_status.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx( &memory_status );

    result.total_memory = memory_status.ullTotalPhys;
    result.logical_processor_count = win32_info.dwNumberOfProcessors;

#if defined(LD_ARCH_X86)
    mem_set(
        ' ',
        CPU_NAME_BUFFER_SIZE,
        result.cpu_name_buffer
    );
    result.cpu_name_buffer[CPU_NAME_BUFFER_SIZE - 1] = 0;

    int cpu_info[4] = {};
    __cpuid( cpu_info, 0x80000002 );
    mem_copy(
        result.cpu_name_buffer,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000003 );
    mem_copy(
        result.cpu_name_buffer + 16,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000004 );
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

LRESULT win32_winproc(
    HWND hWnd, UINT Msg,
    WPARAM wParam, LPARAM lParam
) {
    #define TRANSITION_STATE_MASK (1 << 31)
    #define EXTENDED_KEY_MASK     (1 << 24)
    #define SCANCODE_MASK         0x00FF0000

    Platform* platform = (Platform*)GetWindowLongPtr(
        hWnd,
        GWLP_USERDATA
    );

    if( !platform ) {
        return DefWindowProc(
            hWnd,
            Msg,
            wParam,
            lParam
        );
    }

    Event event = {};
    switch( Msg ) {
        case WM_CLOSE:
        case WM_DESTROY: {
            event.code = EVENT_CODE_EXIT;
            event_fire( event );
        } return 0;

        case WM_ACTIVATE: {
            b32 is_active = wParam == WA_ACTIVE ||
                wParam == WA_CLICKACTIVE;

            XInputEnable( (BOOL)is_active );
            event.code = EVENT_CODE_ACTIVE;
            event.data.bool32[0] = is_active;
            event_fire( event );

            if( !is_active ) {
                platform_cursor_set_visible( platform, true );
            }
            platform->is_active = is_active;
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
                    break;
                }

                #define MIN_DIMENSIONS 1
                ivec2 dimensions = {
                    max( rect.right, MIN_DIMENSIONS ),
                    max( rect.bottom, MIN_DIMENSIONS ),
                };

                platform->surface.dimensions = dimensions;
                event.code = EVENT_CODE_RESIZE;
                event.data.int32[0] = dimensions.width;
                event.data.int32[1] = dimensions.height;
                event_fire( event );

                last_rect = rect;
            }
        } return FALSE;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP: {

            if( !platform->is_active ) {
                break;
            }

            b32 previous_key_state = (lParam >> 30) == 1;
            if( previous_key_state ) {
                break;
            }
            u8 keycode = wParam;

            if( ARE_BITS_SET( lParam, EXTENDED_KEY_MASK ) ) {
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
            input_set_key( (KeyCode)keycode, is_down );

        } return TRUE;
        
        case WM_MOUSEMOVE: {

            if( !platform->is_active ) {
                break;
            }

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

            if( !platform->is_active ) {
                break;
            }

            b32 is_down =
                Msg == WM_LBUTTONDOWN ||
                Msg == WM_MBUTTONDOWN ||
                Msg == WM_RBUTTONDOWN;
            MouseCode code;
            if( Msg == WM_LBUTTONDOWN || Msg == WM_LBUTTONUP ) {
                code = MBC_BUTTON_LEFT;
            } else if( Msg == WM_RBUTTONDOWN || Msg == WM_RBUTTONUP ) {
                code = MBC_BUTTON_RIGHT;
            } else if( Msg == WM_MBUTTONDOWN || Msg == WM_MBUTTONUP ) {
                code = MBC_BUTTON_MIDDLE;
            } else {
                break;
            }

            input_set_mouse_button( code, is_down );

        } return TRUE;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: {

            if( !platform->is_active ) {
                break;
            }

            UINT button = GET_XBUTTON_WPARAM(wParam);
            b32 is_down = Msg == WM_XBUTTONDOWN;
            MouseCode code = (MouseCode)(button + (MBC_BUTTON_EXTRA_1 - 1));

            input_set_mouse_button( code, is_down );

        } return TRUE;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL: {

            if( !platform->is_active ) {
                break;
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
                    platform,
                    CURSOR_RESIZE_HORIZONTAL
                );
            } break;

            case HTTOP:
            case HTBOTTOM: {
                platform_cursor_set_style(
                    platform,
                    CURSOR_RESIZE_VERTICAL
                );
            } break;

            case HTBOTTOMLEFT:
            case HTTOPRIGHT: {
                platform_cursor_set_style(
                    platform,
                    CURSOR_RESIZE_TOP_RIGHT_BOTTOM_LEFT
                );
            } break;

            case HTBOTTOMRIGHT:
            case HTTOPLEFT: {
                platform_cursor_set_style(
                    platform,
                    CURSOR_RESIZE_TOP_LEFT_BOTTOM_RIGHT
                );
            } break;

            default: {
                platform_cursor_set_style(
                    platform,
                    CURSOR_ARROW
                );
            } break;
        } return TRUE;

    }

    return DefWindowProc(
        hWnd,
        Msg,
        wParam,
        lParam
    );
    return false;
}

MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
) {
    HWND   hWnd      = nullptr;
    LPCSTR lpText    = message;
    LPCSTR lpCaption = window_title;

    b32  valid_type = true;
    UINT uType = 0;
    switch( type ) {
        case MBTYPE_OK:
            uType |= MB_OK;
            break;
        case MBTYPE_OKCANCEL:
            uType |= MB_OKCANCEL;
            break;
        case MBTYPE_RETRYCANCEL:
            uType |= MB_RETRYCANCEL;
            break;
        case MBTYPE_YESNO:
            uType |= MB_YESNO;
            break;
        case MBTYPE_YESNOCANCEL:
            uType |= MB_YESNOCANCEL;
            break;
        default:
            valid_type = false;
            break;
    }

    if( !valid_type ) {
        WIN32_LOG_ERROR("Message Box requires a valid type.");
        return MBRESULT_UNKNOWN_ERROR;
    }

    switch( icon ) {
        case MBICON_INFORMATION:
            uType |= MB_ICONASTERISK;
            break;
        case MBICON_WARNING:
            uType |= MB_ICONWARNING;
            break;
        case MBICON_ERROR:
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
            result = MBRESULT_OK;
            break;
        case IDYES:
            result = MBRESULT_YES;
            break;
        case IDNO:
            result = MBRESULT_NO;
            break;
        case IDRETRY:
            result = MBRESULT_RETRY;
            break;
        case IDCANCEL:
            result = MBRESULT_CANCEL;
            break;
        default:
            WIN32_LOG_ERROR("Message Box returned an unknown result.");
            result = MBRESULT_UNKNOWN_ERROR;
            break;
    }

    return result;
}

b32 platform_file_open(
    const char*   path,
    FileOpenFlags flags,
    FileHandle*   out_handle
) {
    DWORD dwDesiredAccess = 0;
    if( ARE_BITS_SET( flags, PLATFORM_FILE_OPEN_READ ) ) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if( ARE_BITS_SET( flags, PLATFORM_FILE_OPEN_WRITE ) ) {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    DWORD dwShareMode = 0;
    if( ARE_BITS_SET( flags, PLATFORM_FILE_OPEN_SHARE_READ ) ) {
        dwShareMode |= FILE_SHARE_READ;
    }
    if( ARE_BITS_SET( flags, PLATFORM_FILE_OPEN_SHARE_WRITE ) ) {
        dwShareMode |= FILE_SHARE_WRITE;
    }

    DWORD dwCreationDisposition = 0;
    if( ARE_BITS_SET( flags, PLATFORM_FILE_OPEN_EXISTING ) ) {
        dwCreationDisposition |= OPEN_EXISTING;
    } else {
        dwCreationDisposition |= OPEN_ALWAYS;
    }

    HANDLE handle = CreateFile(
        path,
        dwDesiredAccess,
        dwShareMode,
        nullptr,
        dwCreationDisposition,
        0,
        nullptr
    );
    if( handle == INVALID_HANDLE_VALUE ) {
        WIN32_LOG_ERROR( "Path: {cc}", path );
        return false;
    }

    out_handle->platform = (void*)handle;
    return true;
}
void platform_file_close( FileHandle handle ) {
    HANDLE win32_handle = (HANDLE)handle.platform;
    CloseHandle( win32_handle );
}
b32 platform_file_read(
    FileHandle handle,
    usize read_size,
    usize buffer_size,
    void* buffer
) {
    LOG_ASSERT(
        read_size < U32::MAX,
        "platform_file_read does not support reads over 4GB on Win32!"
    );

    HANDLE win32_handle = (HANDLE)handle.platform;

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
        win32_handle,
        buffer,
        bytes_to_read,
        &bytes_read,
        nullptr
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
usize platform_file_query_size( FileHandle handle ) {
    HANDLE win32_handle  = (HANDLE)handle.platform;
    LARGE_INTEGER result = {};
    if( GetFileSizeEx( win32_handle, &result ) ) {
        return result.QuadPart;
    } else {
        win32_log_error( false );
        return 0;
    }
}
usize platform_file_query_offset( FileHandle handle ) {
    HANDLE win32_handle  = (HANDLE)handle.platform;

    LARGE_INTEGER offset = {};
    LARGE_INTEGER result = {};
    SetFilePointerEx(
        win32_handle,
        offset,
        &result,
        FILE_CURRENT
    );

    return result.QuadPart;
}
b32 platform_file_set_offset( FileHandle handle, usize offset ) {
    HANDLE win32_handle = (HANDLE)handle.platform;

    LARGE_INTEGER large_offset = {};
    large_offset.QuadPart = offset;

    if( !SetFilePointerEx(
        win32_handle,
        large_offset,
        nullptr,
        FILE_BEGIN
    ) ) {
        win32_log_error( false );
        return false;
    } else {
        return true;
    }
}
internal inline void fill_sound_buffer(
    i16* sample_out,
    DWORD sample_count,
    Win32DirectSound* ds,
    i16 volume
) {
    f32 wave_period = (f32)AUDIO_KHZ / (f32)256;
    for(
        DWORD sample_index = 0;
        sample_index < sample_count;
        ++sample_index
    ) {
        f32 t = F32::TAU * ((f32)ds->running_sample_index / wave_period);
        f32 sine_value = sin(t);
        i16 sample_value = (i16)(sine_value * volume);
        *sample_out++ = sample_value;
        *sample_out++ = sample_value;

        ds->running_sample_index++;
    }
}
b32 platform_init_audio( Platform* generic_platform ) {
    Win32Platform* platform = (Win32Platform*)generic_platform;

    if( !library_load(
        "DSOUND.DLL",
        (void**)&platform->lib_dsound
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load user32.dll!"
        );
        return false;
    }
    DirectSoundCreate =
    (::impl::DirectSoundCreateFN)library_load_function(
        platform->lib_dsound,
        "DirectSoundCreate"
    );
    if( !DirectSoundCreate ) {
        return false;
    }

    LPDIRECTSOUND direct_sound = nullptr;
    HRESULT hResult = DirectSoundCreate(
        NULL, &direct_sound, NULL
    );
    if( !SUCCEEDED(hResult) ) {
        win32_log_error( true );
        return false;
    }
    
    // Set cooperative level
    hResult = direct_sound->SetCooperativeLevel(
        platform->window.handle,
        DSSCL_PRIORITY
    );
    if( !SUCCEEDED(hResult) ) {
        win32_log_error( true );
        return false;
    }

    /// Create primary buffer
    DSBUFFERDESC buffer_description = {};
    buffer_description.dwSize  = sizeof(buffer_description);
    buffer_description.dwFlags = DSBCAPS_PRIMARYBUFFER;

    LPDIRECTSOUNDBUFFER direct_sound_primary_buffer = {};
    hResult = direct_sound->CreateSoundBuffer(
        &buffer_description,
        &direct_sound_primary_buffer,
        NULL
    );
    if( !SUCCEEDED( hResult ) ) {
        win32_log_error( true );
        return false;
    }

    /// Set primary buffer format
    WAVEFORMATEX wave_format = {};
    wave_format.wFormatTag     = WAVE_FORMAT_PCM;
    wave_format.nChannels      = AUDIO_CHANNEL_COUNT;
    wave_format.wBitsPerSample = AUDIO_BITS_PER_SAMPLE;
    wave_format.nSamplesPerSec = AUDIO_KHZ;
    wave_format.nBlockAlign =
        (wave_format.nChannels * wave_format.wBitsPerSample) / 8;
    wave_format.nAvgBytesPerSec =
        wave_format.nSamplesPerSec * wave_format.nBlockAlign;

    hResult = direct_sound_primary_buffer->SetFormat( &wave_format );
    if( !SUCCEEDED( hResult ) ) {
        win32_log_error( true );
        return false;
    }

    /// Create secondary buffer
    buffer_description = {};
    buffer_description.dwSize        = sizeof(buffer_description);
    buffer_description.dwBufferBytes = AUDIO_BUFFER_SIZE;
    buffer_description.lpwfxFormat   = &wave_format;
    LPDIRECTSOUNDBUFFER direct_sound_secondary_buffer = {};
    hResult = direct_sound->CreateSoundBuffer(
        &buffer_description,
        &direct_sound_secondary_buffer,
        NULL
    );

    if( !SUCCEEDED(hResult) ) {
        win32_log_error( true );
        return false;
    }

    platform->direct_sound.handle          = direct_sound;
    platform->direct_sound.hardware_handle = direct_sound_primary_buffer;
    platform->direct_sound.buffer          = direct_sound_secondary_buffer;
    platform->direct_sound.running_sample_index = 0;

    void* audio_ptr[2];
    DWORD audio_bytes[2];
    hResult = direct_sound_secondary_buffer->Lock(
        0, AUDIO_BUFFER_SIZE,
        &audio_ptr[0], &audio_bytes[0],
        &audio_ptr[1], &audio_bytes[1],
        DSBLOCK_ENTIREBUFFER
    );
    LOG_ASSERT( SUCCEEDED(hResult), "Failed to lock" );
    
    if( audio_ptr[0] ) {
        // NOTE(alicia): should probably clear to zero instead
        fill_sound_buffer(
            (i16*)audio_ptr[0],
            audio_bytes[0] / AUDIO_BYTES_PER_SAMPLE,
            &platform->direct_sound,
            400
        );
    }

    hResult = direct_sound_secondary_buffer->Unlock(
        audio_ptr[0], audio_bytes[0],
        audio_ptr[1], audio_bytes[1]
    );
    LOG_ASSERT( SUCCEEDED(hResult), "Failed to unlock" );

    direct_sound_secondary_buffer->Play(
        0, 0,
        DSBPLAY_LOOPING
    );


    return true;
}
void platform_shutdown_audio( Platform* platform ) {
    LPDIRECTSOUNDBUFFER buffer = ((Win32Platform*)platform)->direct_sound.buffer;
    buffer->Stop();
}

void platform_audio_test( Platform* generic_platform, i16 volume ) {
    Win32DirectSound* ds = &((Win32Platform*)generic_platform)->direct_sound;
    LPDIRECTSOUNDBUFFER buffer = ds->buffer;

    DWORD play_cursor  = 0;
    DWORD write_cursor = 0;

    HRESULT hResult = buffer->GetCurrentPosition(
        &play_cursor,
        &write_cursor
    );
    LOG_ASSERT( SUCCEEDED(hResult), "Failed to get play/write cursor!" );

    DWORD byte_to_lock =
        (ds->running_sample_index * AUDIO_BYTES_PER_SAMPLE) % AUDIO_BUFFER_SIZE;
    DWORD bytes_to_write = 0;
    if( ds->running_sample_index == 0 ) {
        bytes_to_write = AUDIO_BUFFER_SIZE;
    } else {
        if( byte_to_lock == play_cursor ) {
            return;
        } else if( byte_to_lock > play_cursor ) {
            bytes_to_write = ( AUDIO_BUFFER_SIZE - byte_to_lock );
            bytes_to_write += play_cursor;
        } else {
            bytes_to_write = play_cursor - byte_to_lock;
        }
    }

    void* audio_ptr[2];
    DWORD audio_bytes[2];
    hResult = buffer->Lock(
        byte_to_lock, bytes_to_write,
        &audio_ptr[0], &audio_bytes[0],
        &audio_ptr[1], &audio_bytes[1],
        0
    );
    LOG_ASSERT( SUCCEEDED(hResult), "Failed to lock" );

    i16*  sample_out   = (i16*)(audio_ptr[0]);
    DWORD sample_count = audio_bytes[0] / AUDIO_BYTES_PER_SAMPLE;

    fill_sound_buffer(
        sample_out,
        sample_count,
        ds, volume
    );
    sample_out   = (i16*)(audio_ptr[1]);
    sample_count = audio_bytes[1] / AUDIO_BYTES_PER_SAMPLE;
    fill_sound_buffer(
        sample_out,
        sample_count,
        ds, volume
    );

    hResult = buffer->Unlock(
        audio_ptr[0], audio_bytes[0],
        audio_ptr[1], audio_bytes[1]
    );
    LOG_ASSERT( SUCCEEDED(hResult), "Failed to unlock" );
}

b32 win32_load_user32( HMODULE* out_module ) {
    HMODULE lib_user32 = nullptr;

    if( !library_load(
        "USER32.DLL",
        (void**)&lib_user32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load user32.dll!"
        );
        return false;
    }

    #define LOAD_FUNCTION(function_name)do {\
        function_name = ( ::impl:: function_name##FN )\
        library_load_function( lib_user32, #function_name );\
        ASSERT( function_name );\
    } while(0)

    LOAD_FUNCTION(CreateWindowExA);
    LOAD_FUNCTION(RegisterClassExA);
    LOAD_FUNCTION(AdjustWindowRectEx);
    LOAD_FUNCTION(LoadImageA);
    LOAD_FUNCTION(GetWindowLongPtrA);
    LOAD_FUNCTION(DefWindowProcA);
    LOAD_FUNCTION(GetClientRect);
    LOAD_FUNCTION(MapVirtualKeyA);
    LOAD_FUNCTION(DestroyWindow);
    LOAD_FUNCTION(PeekMessageA);
    LOAD_FUNCTION(TranslateMessage);
    LOAD_FUNCTION(DestroyIcon);
    LOAD_FUNCTION(GetDC);
    LOAD_FUNCTION(ShowWindow);
    LOAD_FUNCTION(SetWindowLongPtrA);
    LOAD_FUNCTION(MessageBoxA);
    LOAD_FUNCTION(DispatchMessageA);
    LOAD_FUNCTION(SetWindowTextA);
    LOAD_FUNCTION(GetWindowTextLengthA);
    LOAD_FUNCTION(GetWindowTextA);
    LOAD_FUNCTION(SetCursorPos);
    LOAD_FUNCTION(ClientToScreen);
    LOAD_FUNCTION(ShowCursor);
    LOAD_FUNCTION(SetCursor);
    LOAD_FUNCTION(LoadCursorA);
    LOAD_FUNCTION(GetSystemMetrics);
    LOAD_FUNCTION(SetProcessDpiAwarenessContext);
    LOAD_FUNCTION(GetDpiForSystem);
    LOAD_FUNCTION(AdjustWindowRectExForDpi);

    *out_module = lib_user32;
    return true;
}
b32 win32_load_xinput( HMODULE* out_module ) {
    HMODULE lib_xinput = nullptr;

    if( !library_load(
        "XINPUT1_4.DLL",
        (void**)&lib_xinput
    ) ) {
        if( !library_load(
            "XINPUT9_1_0.DLL",
            (void**)&lib_xinput
        ) ) {
            if( !library_load(
                "XINPUT1_3.DLL",
                (void**)&lib_xinput
            ) ) {
                MESSAGE_BOX_FATAL(
                    "Failed to load library!",
                    "Failed to load any version of XInput!"
                );
                return false;
            }
        }
    }

    XInputGetState =
    (::impl::XInputGetStateFN)library_load_function(
        lib_xinput,
        "XInputGetState"
    );
    if( !XInputGetState ) {
        return false;
    }
    XInputSetState =
    (::impl::XInputSetStateFN)library_load_function(
        lib_xinput,
        "XInputSetState"
    );
    if( !XInputSetState ) {
        return false;
    }
    ::impl::XInputEnableFN xinput_enable =
    (::impl::XInputEnableFN)library_load_function(
        lib_xinput,
        "XInputEnable"
    );
    if( xinput_enable ) {
        XInputEnable = xinput_enable;
    }

    *out_module = lib_xinput;
    return true;
}
b32 win32_load_opengl( Win32Platform* platform ) {
    HMODULE lib_gl = nullptr;

    if( !library_load(
        "OPENGL32.DLL",
        (void**)&lib_gl
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load opengl32.dll!"
        );
        return false;
    }

    wglCreateContext =
    (::impl::wglCreateContextFN)library_load_function(
        lib_gl,
        "wglCreateContext"
    );
    if( !wglCreateContext ) {
        return false;
    }
    wglMakeCurrent =
    (::impl::wglMakeCurrentFN)library_load_function(
        lib_gl,
        "wglMakeCurrent"
    );
    if( !wglMakeCurrent ) {
        return false;
    }
    wglDeleteContext =
    (::impl::wglDeleteContextFN)library_load_function(
        lib_gl,
        "wglDeleteContext"
    );
    if( !wglDeleteContext ) {
        return false;
    }
    wglGetProcAddress =
    (::impl::wglGetProcAddressFN)library_load_function(
        lib_gl,
        "wglGetProcAddress"
    );
    if( !wglGetProcAddress ) {
        return false;
    }

    DescribePixelFormat =
    (::impl::DescribePixelFormatFN)library_load_function(
        platform->lib_gdi32,
        "DescribePixelFormat"
    );
    if( !DescribePixelFormat ) {
        return false;
    }
    ChoosePixelFormat =
    (::impl::ChoosePixelFormatFN)library_load_function(
        platform->lib_gdi32,
        "ChoosePixelFormat"
    );
    if( !ChoosePixelFormat ) {
        return false;
    }
    SetPixelFormat =
    (::impl::SetPixelFormatFN)library_load_function(
        platform->lib_gdi32,
        "SetPixelFormat"
    );
    if( !SetPixelFormat ) {
        return false;
    }

    SwapBuffers =
    (::impl::SwapBuffersFN)library_load_function(
        platform->lib_gdi32,
        "SwapBuffers"
    );
    if( !SwapBuffers ) {
        return false;
    }

    platform->lib_gl = lib_gl;
    return true;
}

namespace impl {
    #define LIBRARY_NAME_BUFFER_SIZE 128

    b32 _library_load(
        const char* library_name, 
        LibraryHandle* out_library
    ) {
        HMODULE module = LoadLibraryA( library_name );
        if( !module ) {
            return false;
        }
        *out_library = (LibraryHandle)module;
        return true;
    }
    b32 _library_load_trace(
        const char* library_name, 
        LibraryHandle* out_library,
        const char* function,
        const char* file,
        i32 line
    ) {
        LibraryHandle result = nullptr;
        if( !_library_load( library_name, &result ) ) {
            log_formatted_locked(
                LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,
                LOG_COLOR_RED, LOG_FLAG_NEW_LINE,
                "[ERROR WIN32  | {cc}() | {cc}:{i}] "
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
            "[NOTE WIN32 | {cc}() | {cc}:{i}] "
            "Library \"{cc}\" has been loaded successfully.",
            function, file, line,
            library_name
        );

        return true;
    }
    void _library_free( LibraryHandle library ) {
        HMODULE module = (HMODULE)library;
        FreeLibrary( module );
    }
    void _library_free_trace( 
        LibraryHandle library,
        const char* function,
        const char* file,
        i32 line
    ) {
        HMODULE module = (HMODULE)library;

        char library_name_buffer[LIBRARY_NAME_BUFFER_SIZE];
        GetModuleBaseNameA(
            GetCurrentProcess(),
            module,
            library_name_buffer,
            LIBRARY_NAME_BUFFER_SIZE
        );
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
            LOG_COLOR_RESET, LOG_FLAG_NEW_LINE,
            "[NOTE WIN32 | {cc}() | {cc}:{i}] "
            "Library \"{cc}\" has been freed.",
            function, file, line,
            library_name_buffer
        );
        _library_free( library );
    }
    void* _library_load_function(
        LibraryHandle library,
        const char* function_name
    ) {
        HMODULE module = (HMODULE)library;
        FARPROC proc   = GetProcAddress(
            module,
            function_name
        );
        return (void*)proc;
    }
    void* _library_load_function_trace(
        LibraryHandle library,
        const char* function_name,
        const char* function,
        const char* file,
        i32 line
    ) {
        HMODULE module = (HMODULE)library;
        char library_name_buffer[LIBRARY_NAME_BUFFER_SIZE];
        GetModuleBaseNameA(
            GetCurrentProcess(),
            module,
            library_name_buffer,
            LIBRARY_NAME_BUFFER_SIZE
        );
        void* result = _library_load_function(
            library,
            function_name
        );

        LogLevel level = result ?
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE :
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE;
        LogColor color = result ? LOG_COLOR_RESET : LOG_COLOR_RED;

        LogFlags flags = result ? 0 : LOG_FLAG_ALWAYS_PRINT;
        flags |= LOG_FLAG_NEW_LINE;
        const char* type = result ? "NOTE" : "ERROR";

        if( result ) {
            log_formatted_locked(
                level, color, flags,
                "[{cc} WIN32 | {cc}() | {cc}:{i}] "
                "Function \"{cc}\" loaded from library \"{cc}\" successfully.",
                type, function, file, line,
                function_name, library_name_buffer
            );
        } else {
            log_formatted_locked(
                level, color, flags,
                "[{cc} WIN32 | {cc}() | {cc}:{i}] "
                "Unable to load function \"{cc}\" from library \"{cc}\"!",
                type, function, file, line,
                function_name, library_name_buffer
            );
        }
        return result;
    }

} // namespace impl

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
        nullptr,
        error_code,
        0,
        ERROR_MESSAGE_BUFFER,
        ERROR_MESSAGE_BUFFER_SIZE,
        nullptr
    );

    if( message_length ) {
        WIN32_LOG_ERROR(
            "{u}: {cc}",
            error_code,
            ERROR_MESSAGE_BUFFER
        );

        if( present_message_box ) {
            StringView error_message_buffer_view = {};
            error_message_buffer_view.buffer = ERROR_MESSAGE_BUFFER + message_length;
            error_message_buffer_view.len    = ERROR_MESSAGE_BUFFER_SIZE - (message_length + 1);
            string_format(
                error_message_buffer_view,
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

void* heap_alloc( usize size ) {
    void* pointer = (void*)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        size
    );
    return pointer;
}
void* heap_realloc( void* memory, usize new_size ) {

    void* pointer = (void*)HeapReAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        memory,
        new_size
    );

    return pointer;
}
void heap_free( void* memory ) {
    HeapFree( GetProcessHeap(), 0, memory );
}

void* platform_page_alloc( usize size ) {
    // VirtualAlloc returns automatically zeroed memory.
    void* pointer = (void*)VirtualAlloc(
        nullptr,
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
    Win32ThreadHandle* win32_thread_handle = (Win32ThreadHandle*)params;

    DWORD result = win32_thread_handle->thread_proc(
        win32_thread_handle->thread_proc_user_params
    );

    return result;
}
b32 platform_thread_create(
    struct Platform*,
    ThreadProcFN     thread_proc,
    void*            user_params,
    usize            thread_stack_size,
    b32              run_on_create,
    ThreadHandle*    out_thread_handle
) {
    Win32ThreadHandle* win32_thread_handle =
        (Win32ThreadHandle*)out_thread_handle;

    win32_thread_handle->thread_proc             = thread_proc;
    win32_thread_handle->thread_proc_user_params = user_params;

    read_write_fence();

    win32_thread_handle->thread_handle = CreateThread(
        nullptr,
        thread_stack_size,
        win32_thread_proc,
        win32_thread_handle,
        CREATE_SUSPENDED,
        &win32_thread_handle->thread_id
    );

    if( !win32_thread_handle->thread_handle ) {
        win32_log_error( true );
        return false;
    }

    read_write_fence();
    if( run_on_create ) {
        if(!platform_thread_resume(
            out_thread_handle
        )) {
            return false;
        }
    }

    WIN32_LOG_NOTE("New thread created. ID: {u}", win32_thread_handle->thread_id);
    return true;
}
b32 platform_thread_resume( ThreadHandle* thread_handle ) {
    Win32ThreadHandle* win32_thread = (Win32ThreadHandle*)thread_handle;
    DWORD result = ResumeThread( win32_thread->thread_handle );
    if( result == (DWORD)-1 ) {
        win32_log_error( false );
        return false;
    } else {
        return true;
    }
}
b32 semaphore_create(
    u32 initial_count, u32 maximum_count,
    SemaphoreHandle* out_semaphore_handle
) {

    HANDLE result = CreateSemaphoreEx(
        nullptr,
        initial_count,
        maximum_count,
        nullptr,
        0,
        SEMAPHORE_ALL_ACCESS
    );
    if( !result ) {
        win32_log_error( false );
        return false;
    }

    *out_semaphore_handle = (void*)result;
    return true;
}
void semaphore_increment(
    SemaphoreHandle semaphore_handle,
    u32 increment, u32* out_opt_previous_count
) {
    HANDLE win32_semaphore_handle = (HANDLE)semaphore_handle;
    ReleaseSemaphore(
        win32_semaphore_handle,
        increment,
        (LONG*)out_opt_previous_count
    );
}
void semaphore_wait(
    SemaphoreHandle semaphore_handle,
    b32 infinite_timeout, u32 opt_timeout_ms
) {
    HANDLE win32_semaphore_handle = (HANDLE)semaphore_handle;
    WaitForSingleObjectEx(
        win32_semaphore_handle,
        infinite_timeout ?
            INFINITE : opt_timeout_ms,
        FALSE
    );
}
void semaphore_wait_multiple(
    usize            semaphore_handle_count,
    SemaphoreHandle* semaphore_handles,
    b32 wait_for_all, b32 infinite_timeout,
    u32 opt_timeout_ms
) {
    WaitForMultipleObjects(
        semaphore_handle_count,
        (HANDLE*)semaphore_handles,
        wait_for_all ? TRUE : FALSE,
        infinite_timeout ? INFINITE : opt_timeout_ms
    );
}
void semaphore_destroy( SemaphoreHandle semaphore_handle ) {
    HANDLE win32_semaphore_handle = (HANDLE)semaphore_handle;
    CloseHandle( win32_semaphore_handle );
}
LD_API b32 mutex_create( MutexHandle* out_mutex ) {
    HANDLE result = CreateMutexA(
        nullptr,
        false,
        nullptr
    );
    if( !result ) {
        return false;
    }

    *out_mutex = (MutexHandle)result;

    return true;
}
LD_API void mutex_lock( MutexHandle mutex ) {
    WaitForSingleObject(
        (HANDLE)mutex,
        INFINITE
    );
}
LD_API void mutex_unlock( MutexHandle mutex ) {
    ReleaseMutex( (HANDLE)mutex );
}
LD_API void mutex_destroy( MutexHandle mutex ) {
    CloseHandle( (HANDLE)mutex );
}
u32 platform_interlocked_increment( volatile u32* addend ) {
    return InterlockedIncrement( addend );
}
u32 platform_interlocked_decrement( volatile u32* addend ) {
    return InterlockedDecrement( addend );
}
u32 platform_interlocked_exchange( volatile u32* target, u32 value ) {
    return InterlockedExchange( target, value );
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
u32 platform_interlocked_compare_exchange(
    u32 volatile* dst,
    u32 exchange, u32 comperand
) {
    return InterlockedCompareExchange(
        dst,
        exchange,
        comperand
    );
}

void read_write_fence() {
    _ReadWriteBarrier();
#if defined(LD_ARCH_X86)
    _mm_mfence();
#elif
    #error "mem_fence: Platform is not supported!"
#endif
}
void read_fence() {
    _ReadBarrier();
#if defined(LD_ARCH_X86)
    _mm_lfence();
#elif
    #error "read_fence: Platform is not supported!"
#endif
}
void write_fence() {
    _WriteBarrier();
#if defined(LD_ARCH_X86)
    _mm_sfence();
#elif
    #error "write_fence: Platform is not supported!"
#endif
}

void* platform_stdout_handle() {
    return GetStdHandle( STD_OUTPUT_HANDLE );
}
void* platform_stderr_handle() {
    return GetStdHandle( STD_ERROR_HANDLE );
}
void platform_write_console(
    void* output_handle,
    u32 write_count,
    const char* buffer
) {
    WriteConsole(
        output_handle,
        buffer,
        write_count,
        0, 0
    );
}

void platform_win32_output_debug_string( const char* str ) {
    OutputDebugStringA( str );
}


#endif // LD_PLATFORM_WINDOWS