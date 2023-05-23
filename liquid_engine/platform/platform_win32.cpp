/**
 * Description:  Win32 Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#if defined(SM_PLATFORM_WINDOWS)

#include "platform.h"
#include "core/logging.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/collections.h"
#include "core/events.h"
#include "core/math.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define NOGDI
#include <vulkan/vulkan.h>
#include "renderer/renderer_defines.h"
#include "renderer/vulkan/vk_defines.h"

#include <intrin.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <xinput.h>

// TODO(alicia): custom formatting and printing
#include <stdio.h>

struct Win32ThreadHandle {
    HANDLE     handle;
    ThreadProc proc;
    void*      params;
    DWORD      id;
};

struct Win32Cursor {
    MouseCursorStyle style;
    b32              is_visible;
    Surface*         surface_locked;
};

global Win32Cursor CURSOR_STATE = {};

struct Win32Surface {
    Surface        surface;
    HWND           hWnd;
    HDC            hDc;
    PlatformState* state;
};

global usize SCRATCH_BUFFER_SIZE = 512;
struct Win32State {
    HINSTANCE hInstance;

    HMODULE   libUser32;
    HMODULE   libXinput;
    HMODULE   libGl;
    HMODULE   libGdi32;

    Win32Surface      surface;

    usize             thread_count;
    Win32ThreadHandle threads[MAX_THREAD_COUNT];

    wchar_t* wide_char_scratch_buffer;
};

global LARGE_INTEGER PERF_FREQUENCY;
global LARGE_INTEGER INITIAL_COUNTER;

Win32ThreadHandle* get_next_handle( Win32State* state ) {
    if( state->thread_count >= MAX_THREAD_COUNT ) {
        return nullptr;
    }
    usize next_handle = state->thread_count++;
    return &state->threads[next_handle];
}

union Win32GamepadMotorState {
    struct {
        f32 motor_left;
        f32 motor_right;
    };
    f32 motors[2];
};

global b32 IS_DPI_AWARE = false;
global b32 IS_ACTIVE    = true;

// LOGGING | BEGIN --------------------------------------------------------

#if defined(LD_LOGGING)
    #define WIN_LOG_NOTE( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE WIN32  ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN_LOG_INFO( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO WIN32  ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN_LOG_DEBUG( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG WIN32 ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN_LOG_WARN( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN WIN32  ] " format,\
            ##__VA_ARGS__\
        )
    #define WIN_LOG_ERROR( format, ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR WIN32 ] " format,\
            ##__VA_ARGS__\
        )

    #define WIN_LOG_NOTE_TRACE( format, ... ) \
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

    #define WIN_LOG_INFO_TRACE( format, ... ) \
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

    #define WIN_LOG_DEBUG_TRACE( format, ... ) \
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
        
    #define WIN_LOG_WARN_TRACE( format, ... ) \
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

    #define WIN_LOG_ERROR_TRACE( format, ... ) \
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
#else
    #define WIN_LOG_NOTE( format, ... )
    #define WIN_LOG_INFO( format, ... )
    #define WIN_LOG_DEBUG( format, ... )
    #define WIN_LOG_WARN( format, ... )
    #define WIN_LOG_ERROR( format, ... )
    #define WIN_LOG_NOTE_TRACE( format, ... )
    #define WIN_LOG_INFO_TRACE( format, ... )
    #define WIN_LOG_DEBUG_TRACE( format, ... )
    #define WIN_LOG_WARN_TRACE( format, ... )
    #define WIN_LOG_ERROR_TRACE( format, ... )
#endif

#define ERROR_MESSAGE_BUFFER_SIZE 128
global char ERROR_MESSAGE_BUFFER[ERROR_MESSAGE_BUFFER_SIZE];
DWORD win_log_error( b32 present_message_box ) {
    DWORD error_code = GetLastError();
    if( error_code == ERROR_SUCCESS ) {
        return error_code;
    }

    wchar_t* message_buffer = nullptr;
    DWORD message_buffer_size = FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_MAX_WIDTH_MASK,
        nullptr,
        error_code,
        0,
        (LPWSTR)&message_buffer,
        0,
        nullptr
    );

    if( message_buffer_size > 0 ) {
        WIN_LOG_ERROR(
            "%u: %ls",
            error_code,
            message_buffer
        );

        if( present_message_box ) {
            static const usize TITLE_BUFFER_SIZE = 128;
            char title_buffer[TITLE_BUFFER_SIZE];
            snprintf(
                title_buffer,
                TITLE_BUFFER_SIZE,
                "Windows Error 0x%X",
                error_code
            );

            snprintf(
                ERROR_MESSAGE_BUFFER,
                ERROR_MESSAGE_BUFFER_SIZE,
                "Please contact me at smushybusiness@gmail.com\n%ls",
                message_buffer
            );

            MESSAGE_BOX_FATAL(
                title_buffer,
                ERROR_MESSAGE_BUFFER
            );
        }
    }

    return error_code;
}

// LOGGING | END ----------------------------------------------------------

// PLATFORM INIT | BEGIN --------------------------------------------------

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

namespace impl {
    [[maybe_unused]]
    internal b32 win_library_load(
        const wchar_t* module_name,
        HMODULE* out_module
    ) {
        HMODULE module = LoadLibrary(
            module_name
        );
        if( !module ) {
            return false;
        }
        *out_module = module;
        return true;
    }
    [[maybe_unused]]
    internal b32 win_library_load_trace(
        const wchar_t* module_name,
        HMODULE* out_module,
        const char* function,
        const char* file,
        i32 line
    ) {
        HMODULE module = LoadLibrary(
            module_name
        );
        if( !module ) {
            log_formatted_locked(
                LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,
                LOG_COLOR_RED, 0,
                "[ERROR WIN32  | %s | %s:%i] ",
                function,
                file,
                line
            );
            log_formatted_locked(
                LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,
                LOG_COLOR_RED,
                LOG_FLAG_NEW_LINE,
                "Failed to load library \"%ls\"!",
                module_name
            );
            return false;
        }

        *out_module = module;
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
            LOG_COLOR_RESET, 0,
            "[NOTE WIN32  | %s | %s:%i] ",
            function,
            file,
            line
        );
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
            LOG_COLOR_RESET,
            LOG_FLAG_NEW_LINE,
            "Library \"%ls\" has been loaded successfully.",
            module_name
        );

        return true;
    }

    [[maybe_unused]]
    internal void win_library_free(
        HMODULE module
    ) {
        FreeLibrary( module );
    }
    [[maybe_unused]]
    internal void win_library_free_trace(
        HMODULE module,
        const char* function,
        const char* file,
        i32 line
    ) {
        static const u32 NAME_BUFFER_SIZE = 128;
        wchar_t name_buffer[NAME_BUFFER_SIZE];
        GetModuleBaseName(
            GetCurrentProcess(),
            module,
            name_buffer,
            NAME_BUFFER_SIZE
        );
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
            LOG_COLOR_RESET, 0,
            "[NOTE WIN32  | %s | %s:%i] ",
            function,
            file,
            line
        );
        log_formatted_locked(
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,
            LOG_COLOR_RESET,
            LOG_FLAG_NEW_LINE,
            "Library \"%ls\" has been freed.",
            name_buffer
        );
        FreeLibrary( module );
    }

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
    #define win_library_load( module_name, out_module )\
        ::impl::win_library_load_trace(\
            module_name,\
            out_module,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define win_library_free( module )\
        ::impl::win_library_free_trace(\
            module,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define win_library_load( module_name, out_module ) \
        ::impl::win_library_load( module_name, out_module )
    #define win_library_free( module )\
        ::impl::win_library_free( module )
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
internal void* win_proc_address(
    HMODULE module,
    const char* proc_name
) {
    void* result = (void*)GetProcAddress(
        module,
        proc_name
    );

#if defined(LD_LOGGING)
    static const usize MODULE_NAME_BUFFER_SIZE = 128;
    wchar_t module_name_buffer[MODULE_NAME_BUFFER_SIZE];
    GetModuleBaseName(
        GetCurrentProcess(),
        module,
        module_name_buffer,
        MODULE_NAME_BUFFER_SIZE
    );
    if( result ) {
        WIN_LOG_NOTE(
            "Function \"%s\" loaded from library \"%ls\".",
            proc_name,
            module_name_buffer
        );
    } else {
        WIN_LOG_WARN(
            "Failed to load function \"%s\" from library \"%ls\"!",
            proc_name,
            module_name_buffer
        );
    }
#endif

    return result;
}
[[maybe_unused]]
internal void* win_proc_address_required(
    HMODULE module,
    const char* proc_name
) {
    void* result = (void*)GetProcAddress(
        module,
        proc_name
    );

    if( result ) {
#if defined(LD_LOGGING)
        static const usize MODULE_NAME_BUFFER_SIZE = 128;
        wchar_t module_name_buffer[MODULE_NAME_BUFFER_SIZE];
        GetModuleBaseName(
            GetCurrentProcess(),
            module,
            module_name_buffer,
            MODULE_NAME_BUFFER_SIZE
        );
        WIN_LOG_NOTE(
            "Function \"%s\" loaded from library \"%ls\".",
            proc_name,
            module_name_buffer
        );
#endif
    } else {
        static const usize MODULE_NAME_BUFFER_SIZE = 128;
        wchar_t module_name_buffer[MODULE_NAME_BUFFER_SIZE];
        GetModuleBaseName(
            GetCurrentProcess(),
            module,
            module_name_buffer,
            MODULE_NAME_BUFFER_SIZE
        );
        static const usize MESSAGE_BUFFER_SIZE = 512;
        char message_buffer[MESSAGE_BUFFER_SIZE];
        snprintf(
            message_buffer,
            MESSAGE_BUFFER_SIZE,
            "Failed to load function \"%s\" from module \"%ls\"!",
            proc_name,
            module_name_buffer
        );
        WIN_LOG_ERROR("%s", message_buffer);
        MESSAGE_BOX_FATAL(
            "Failed to load function.",
            message_buffer
        );
    }

    return result;
}

b32 platform_init(
    PlatformInitFlags flags,
    RendererBackendType   backend,
    PlatformState* out_state
) {
    void* win_state_buffer = mem_alloc(
        sizeof(Win32State),
        MEMTYPE_PLATFORM_DATA
    );
    if( !win_state_buffer ) {
        MESSAGE_BOX_FATAL(
            "Out of Memory",
            "Could not allocate space for Win32 State!"
        );
        return false;
    }
    out_state->platform_data = win_state_buffer;
    Win32State* state = (Win32State*)out_state->platform_data;

    void* wide_char_scratch_buffer = mem_alloc(
        SCRATCH_BUFFER_SIZE,
        MEMTYPE_PLATFORM_DATA
    );
    if( !wide_char_scratch_buffer ) {
        MESSAGE_BOX_FATAL(
            "Out of Memory",
            "Could not allocate space for Win32 scratch buffer!"
        );
        return false;
    }
    state->wide_char_scratch_buffer = (wchar_t*)wide_char_scratch_buffer;

    state->hInstance = GetModuleHandleA(0);

    if( !win_library_load(
        L"USER32.DLL",
        &state->libUser32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load user32.dll!"
        );
        mem_free( state );
        return false;
    }
    if( !win_library_load(
        L"XINPUT1_4.DLL",
        &state->libXinput
    ) ) {
        if( !win_library_load(
            L"XINPUT9_1_0.DLL",
            &state->libXinput
        ) ) {
            if( !win_library_load(
                L"XINPUT1_3.DLL",
                &state->libXinput
            ) ) {
                MESSAGE_BOX_FATAL(
                    "Failed to load library!",
                    "Failed to load any version of XInput!"
                );
                mem_free( state );
                return false;
            }
        }
    }
    if( backend == BACKEND_OPENGL ) {
        if( !win_library_load(
            L"OPENGL32.DLL",
            &state->libGl
        ) ) {
            MESSAGE_BOX_FATAL(
                "Failed to load library!",
                "Failed to load opengl32.dll!"
            );
            mem_free( state );
            return false;
        }
        if( !win_library_load(
            L"GDI32.DLL",
            &state->libGdi32
        ) ) {
            MESSAGE_BOX_FATAL(
                "Failed to load library!",
                "Failed to load gdi32.dll!"
            );
            mem_free( state );
            return false;
        }
    }

    SetProcessDpiAwarenessContext =
    (::impl::SetProcessDpiAwarenessContext_fn)win_proc_address_required(
        state->libUser32,
        "SetProcessDpiAwarenessContext"
    );
    if( !SetProcessDpiAwarenessContext ) {
        mem_free( state );
        return false;
    }

    GetDpiForSystem =
    (::impl::GetDpiForSystem_fn)win_proc_address_required(
        state->libUser32,
        "GetDpiForSystem"
    );
    if( !GetDpiForSystem ) {
        mem_free( state );
        return false;
    }

    AdjustWindowRectExForDpi =
    (::impl::AdjustWindowRectExForDpi_fn)win_proc_address_required(
        state->libUser32,
        "AdjustWindowRectExForDpi"
    );
    if( !AdjustWindowRectExForDpi ) {
        mem_free( state );
        return false;
    }

    XInputGetState =
    (::impl::XInputGetState_fn)win_proc_address_required(
        state->libXinput,
        "XInputGetState"
    );
    if( !XInputGetState ) {
        mem_free( state );
        return false;
    }
    XInputSetState =
    (::impl::XInputSetState_fn)win_proc_address_required(
        state->libXinput,
        "XInputSetState"
    );
    if( !XInputSetState ) {
        mem_free( state );
        return false;
    }
    ::impl::XInputEnable_fn xinput_enable =
    (::impl::XInputEnable_fn)win_proc_address(
        state->libXinput,
        "XInputEnable"
    );
    if( xinput_enable ) {
        XInputEnable = xinput_enable;
    }

    if( backend == BACKEND_OPENGL ) {

        wglCreateContext =
        (::impl::wglCreateContext_fn)win_proc_address_required(
            state->libGl,
            "wglCreateContext"
        );
        if( !wglCreateContext ) {
            mem_free( state );
            return false;
        }
        wglMakeCurrent =
        (::impl::wglMakeCurrent_fn)win_proc_address_required(
            state->libGl,
            "wglMakeCurrent"
        );
        if( !wglMakeCurrent ) {
            mem_free( state );
            return false;
        }
        wglDeleteContext =
        (::impl::wglDeleteContext_fn)win_proc_address_required(
            state->libGl,
            "wglDeleteContext"
        );
        if( !wglDeleteContext ) {
            mem_free( state );
            return false;
        }
        wglGetProcAddress =
        (::impl::wglGetProcAddress_fn)win_proc_address_required(
            state->libGl,
            "wglGetProcAddress"
        );
        if( !wglGetProcAddress ) {
            mem_free( state );
            return false;
        }

        DescribePixelFormat =
        (::impl::DescribePixelFormat_fn)win_proc_address_required(
            state->libGdi32,
            "DescribePixelFormat"
        );
        if( !DescribePixelFormat ) {
            mem_free( state );
            return false;
        }
        ChoosePixelFormat =
        (::impl::ChoosePixelFormat_fn)win_proc_address_required(
            state->libGdi32,
            "ChoosePixelFormat"
        );
        if( !ChoosePixelFormat ) {
            mem_free( state );
            return false;
        }
        SetPixelFormat =
        (::impl::SetPixelFormat_fn)win_proc_address_required(
            state->libGdi32,
            "SetPixelFormat"
        );
        if( !SetPixelFormat ) {
            mem_free( state );
            return false;
        }

        SwapBuffers =
        (::impl::SwapBuffers_fn)win_proc_address_required(
            state->libGdi32,
            "SwapBuffers"
        );
        if( !SwapBuffers ) {
            mem_free( state );
            return false;
        }
    }

    if( ARE_BITS_SET( flags, PLATFORM_DPI_AWARE ) ) {
        SetProcessDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        );
        IS_DPI_AWARE = true;
        WIN_LOG_NOTE( "Program is DPI Aware." );
    } else {
        WIN_LOG_NOTE( "Program is NOT DPI Aware." );
    }

    QueryPerformanceFrequency( &PERF_FREQUENCY );
    QueryPerformanceCounter( &INITIAL_COUNTER );

    WIN_LOG_NOTE("Platform services successfully initialized.");

    return true;
}

void platform_shutdown( PlatformState* platform_state ) {
    Win32State* state = (Win32State*)platform_state->platform_data;

    if( state->libGdi32 ) {
        win_library_free( state->libGdi32 );
    }
    if( state->libGl ) {
        win_library_free( state->libGl );
    }
    win_library_free( state->libXinput );
    win_library_free( state->libUser32 );

    mem_free( state );

    WIN_LOG_NOTE("Platform subsytem successfully shutdown.");
}

u64 platform_absolute_time() {
    LARGE_INTEGER counter = {};
    QueryPerformanceCounter( &counter );
    return counter.QuadPart - INITIAL_COUNTER.QuadPart;
}

f64 platform_seconds_elapsed() {
    u64 counter = platform_absolute_time();
    return (f64)counter / (f64)(PERF_FREQUENCY.QuadPart);
}

global const char* WIN32_VULKAN_EXTENSIONS[] = {
    "VK_KHR_win32_surface"
};

usize platform_get_vulkan_extension_names(
    usize max_names,
    usize* name_count,
    const char** names
) {
    usize win32_extension_count = STATIC_ARRAY_COUNT(WIN32_VULKAN_EXTENSIONS);
    usize max_iter = win32_extension_count > max_names ?
        max_names : win32_extension_count;

    usize count = *name_count;
    for( usize i = 0; i < max_iter; ++i ) {
        names[count++] = WIN32_VULKAN_EXTENSIONS[i];
        win32_extension_count--;
    }
    *name_count = count;
    return win32_extension_count;
}

b32 platform_create_vulkan_surface(
    PlatformState* state,
    VulkanContext* context
) {
    Win32State* win_state = (Win32State*)state->platform_data;
    Win32Surface* surface = &win_state->surface;
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = win_state->hInstance;
    create_info.hwnd      = surface->hWnd;

    VkSurfaceKHR vk_surface = {};

    VkResult result = vkCreateWin32SurfaceKHR(
        context->instance,
        &create_info,
        context->allocator,
        &vk_surface
    );

    if( result != VK_SUCCESS ) {
        VK_LOG_ERROR("Failed to create Vulkan surface!");
        return false;
    }
    
    context->surface.surface = vk_surface;
    context->surface.width   = surface->surface.width;
    context->surface.height  = surface->surface.height;

    return true;
}

// PLATFORM INIT | END ----------------------------------------------------

// MEMORY | BEGIN ---------------------------------------------------------

global usize HEAP_MEMORY_USAGE = 0;
global usize PAGE_MEMORY_USAGE = 0;
usize query_heap_usage() {
    return HEAP_MEMORY_USAGE;
}
usize query_page_usage() {
    return PAGE_MEMORY_USAGE;
}

void* heap_alloc( usize size ) {
    void* pointer = (void*)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        size
    );
#if defined(LD_PROFILING)
    if( pointer ) {
        HEAP_MEMORY_USAGE += size;
    }
#endif
    return pointer;
}
void* heap_realloc( void* memory, usize new_size ) {

#if defined(LD_PROFILING)
    SIZE_T previous_size = HeapSize(
        GetProcessHeap(),
        0,
        memory
    );
#endif

    void* pointer = (void*)HeapReAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        memory,
        new_size
    );

#if defined(LD_PROFILING)
    if( pointer && previous_size != (SIZE_T)-1 ) {
        usize diff = new_size - previous_size;
        HEAP_MEMORY_USAGE += diff;
    }
#endif

    return pointer;
}
void heap_free( void* memory ) {

#if defined(LD_PROFILING)
    SIZE_T mem_size = HeapSize(
        GetProcessHeap(),
        0,
        memory
    );
    if( mem_size != (SIZE_T)-1 ) {
        HEAP_MEMORY_USAGE -= mem_size;
    }

#endif

    HeapFree( GetProcessHeap(), 0, memory );
}

void* page_alloc( usize size ) {
    void* pointer = (void*)VirtualAlloc(
        nullptr,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

#if defined(LD_PROFILING)
    PAGE_MEMORY_USAGE += size;
#endif
    return pointer;
}
void page_free( void* memory ) {
#if defined(LD_PROFILING)
    MEMORY_BASIC_INFORMATION info;
    if(VirtualQuery(
        memory,
        &info,
        sizeof(MEMORY_BASIC_INFORMATION)
    )) {
        PAGE_MEMORY_USAGE -= info.RegionSize;
    }
#endif
    VirtualFree(
        memory,
        0,
        MEM_RELEASE | MEM_DECOMMIT
    );
}

// MEMORY | END   ---------------------------------------------------------

// MULTI-THREADING | BEGIN ------------------------------------------------

internal DWORD WINAPI win_thread_proc( void* params ) {
    Win32ThreadHandle* thread_handle = (Win32ThreadHandle*)params;

    DWORD return_value = thread_handle->proc(
        thread_handle->params
    );

    mem_free( params );

    return return_value;
}

#define THREAD_STACK_SIZE_SAME_AS_MAIN 0
#define THREAD_RUN_ON_CREATE 0
ThreadHandle thread_create(
    PlatformState* state,
    ThreadProc thread_proc,
    void*      params,
    b32        run_on_creation
) {

    Win32State* win_state = (Win32State*)state->platform_data;

    Win32ThreadHandle* thread_handle = get_next_handle( win_state );
    if( !thread_handle ) {
        LOG_ERROR("Out of thread handles!");
        return nullptr;
    }
    thread_handle->proc   = thread_proc;
    thread_handle->params = params;

    // we don't care about this
    LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;

    SIZE_T dwStackSize     = THREAD_STACK_SIZE_SAME_AS_MAIN;
    DWORD  dwCreationFlags = CREATE_SUSPENDED;

    mem_fence();

    thread_handle->handle = CreateThread(
        lpThreadAttributes,
        dwStackSize,
        win_thread_proc,
        thread_handle,
        dwCreationFlags,
        &thread_handle->id
    );

    if( !thread_handle->handle ) {
        win_log_error( true );
        return nullptr;
    }

    if( run_on_creation ) {
        thread_resume( thread_handle );
    }

    return thread_handle;
}
void thread_resume( ThreadHandle thread ) {
    Win32ThreadHandle* win32_thread = (Win32ThreadHandle*)thread;
    ResumeThread( win32_thread->handle );
}

Semaphore semaphore_create(
    u32 initial_count,
    u32 maximum_count
) {
    // we don't care about these
    LPSECURITY_ATTRIBUTES security_attributes = nullptr;
    LPCWSTR name = nullptr;
    DWORD flags = 0;

    DWORD desired_access = SEMAPHORE_ALL_ACCESS;

    HANDLE semaphore_handle = CreateSemaphoreExW(
        security_attributes,
        initial_count,
        maximum_count,
        name,
        flags,
        desired_access
    );

    return (Semaphore*)semaphore_handle;
}
void semaphore_increment(
    Semaphore semaphore,
    u32       increment,
    u32*      opt_out_previous_count
) {
    HANDLE win32_handle = (HANDLE)semaphore;
    ReleaseSemaphore(
        win32_handle,
        increment,
        (LONG*)opt_out_previous_count
    );
}
void semaphore_wait_for(
    Semaphore semaphore,
    u32       timeout_ms
) {
    HANDLE win32_handle = (HANDLE)semaphore;
    WaitForSingleObjectEx(
        win32_handle,
        timeout_ms,
        FALSE
    );
}
void semaphore_wait_for_multiple(
    usize      count,
    Semaphore* semaphores,
    b32        wait_for_all,
    u32        timeout_ms
) {
    const HANDLE* win32_handles = (const HANDLE*)semaphores;
    WaitForMultipleObjects(
        count,
        win32_handles,
        wait_for_all ? TRUE : FALSE,
        timeout_ms
    );
}
void semaphore_destroy( Semaphore semaphore ) {
    HANDLE win32_handle = (HANDLE)semaphore;
    CloseHandle( win32_handle );
}

u32 interlocked_increment( volatile u32* addend ) {
    return InterlockedIncrement( addend );
}
u32 interlocked_decrement( volatile u32* addend ) {
    return InterlockedDecrement( addend );
}
u32 interlocked_exchange( volatile u32* target, u32 value ) {
    return InterlockedExchange( target, value );
}
void* interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange,
    void* comperand
) {
    return InterlockedCompareExchangePointer(
        dst,
        exchange,
        comperand
    );
}
u32 interlocked_compare_exchange(
    u32 volatile* dst,
    u32 exchange,
    u32 comperand
) {
    return InterlockedCompareExchange(
        dst,
        exchange,
        comperand
    );
}

void mem_fence() {
    _ReadWriteBarrier();
#if defined(SM_ARCH_X86)
    _mm_mfence();
#elif
    #error "mem_fence: Platform is not supported!"
#endif
}
void read_fence() {
    _ReadBarrier();
#if defined(SM_ARCH_X86)
    _mm_lfence();
#elif
    #error "read_fence: Platform is not supported!"
#endif
}
void write_fence() {
    _WriteBarrier();
#if defined(SM_ARCH_X86)
    _mm_sfence();
#elif
    #error "write_fence: Platform is not supported!"
#endif
}

// MULTI-THREADING | END   ------------------------------------------------

// MESSAGE BOX | BEGIN ----------------------------------------------------

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
        WIN_LOG_ERROR("Message Box requires a valid type.");
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
            WIN_LOG_ERROR("Message Box returned an unknown result.");
            result = MBRESULT_UNKNOWN_ERROR;
            break;
    }

    return result;
}

// MESSAGE BOX | END   ----------------------------------------------------

// SURFACE | BEGIN --------------------------------------------------------

internal LRESULT window_proc(
    HWND hWnd, UINT Msg,
    WPARAM wParam, LPARAM lParam
);

global usize BASE_CLASS_NAME_LEN;
Surface* surface_create(
    const char* surface_name,
    ivec2 position,
    ivec2 dimensions,
    SurfaceCreateFlags flags,
    PlatformState* platform_state,
    Surface* opt_parent
) {
    Win32State* state = (Win32State*)platform_state->platform_data;

    Win32Surface* win_surface = &state->surface;

    usize surface_name_length = str_length( surface_name ) + 1;
    if( surface_name_length > MAX_SURFACE_NAME_LENGTH ) {
        MESSAGE_BOX_FATAL(
            "Exceeded surface name length!",
            "Surface name is too long!"
        );
        WIN_LOG_ERROR(
            "Surface name is too long! length: %llu",
            surface_name_length
        );
        return nullptr;
    }

    mem_copy(
        win_surface->surface.name,
        surface_name,
        surface_name_length
    );

    WNDCLASSEX windowClass  = {};
    windowClass.cbSize      = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = window_proc;
    windowClass.hInstance   = state->hInstance;
    // TODO(alicia): 
    windowClass.hIcon       = nullptr;
    windowClass.hCursor     = LoadCursor(
        state->hInstance,
        IDC_ARROW
    );

    local const wchar_t* BASE_CLASS_NAME  = L"LiquidEngineWindowClass";
    BASE_CLASS_NAME_LEN = wstr_length(BASE_CLASS_NAME);
    local int SURFACE_COUNT = 0;

    const usize CLASS_NAME_BUFFER_SIZE = BASE_CLASS_NAME_LEN + 8;
    snwprintf(
        state->wide_char_scratch_buffer,
        CLASS_NAME_BUFFER_SIZE,
        L"%ls%i",
        BASE_CLASS_NAME,
        SURFACE_COUNT
    );

    windowClass.lpszClassName = state->wide_char_scratch_buffer;

    if( !RegisterClassEx( &windowClass ) ) {
        win_log_error( true );
        return nullptr;
    }

    DWORD dwStyle   = WS_OVERLAPPEDWINDOW;
    DWORD dwExStyle = WS_EX_OVERLAPPEDWINDOW;

    i32 width = 0, height = 0;
    RECT window_rect = {};
    if( IS_DPI_AWARE ) {
        UINT dpi = GetDpiForSystem();

        width = MulDiv(
            dimensions.width,
            dpi,
            96
        );
        height = MulDiv(
            dimensions.height,
            dpi,
            96
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
            win_log_error( true );
            return nullptr;
        }
    } else {
        width  = dimensions.width;
        height = dimensions.height;

        window_rect.right  = dimensions.width;
        window_rect.bottom = dimensions.height;
        if( !AdjustWindowRectEx(
            &window_rect,
            dwStyle,
            FALSE,
            dwExStyle
        ) ) {
            win_log_error( true );
            return nullptr;
        }
    }

    win_surface->surface.dimensions = { width, height };

    i32 x = 0, y = 0;
    if( ARE_BITS_SET( flags, SURFACE_CREATE_CENTERED ) ) {
        i32 screen_width  = GetSystemMetrics( SM_CXSCREEN );
        i32 screen_height = GetSystemMetrics( SM_CYSCREEN );

        i32 x_center = screen_width  / 2;
        i32 y_center = screen_height / 2;

        i32 half_width  = width / 2;
        i32 half_height = height / 2;

        x = x_center - half_width;
        y = y_center - half_height;
    } else {
        x = position.x;
        y = position.y;
    }

    HWND hWndParent = nullptr;
    if( opt_parent ) {
        Win32Surface* parent_surface = (Win32Surface*)opt_parent;
        hWndParent = parent_surface->hWnd;
    }

    wchar_t* lpWindowName = state->wide_char_scratch_buffer + CLASS_NAME_BUFFER_SIZE + 1;
    mbstowcs(
        lpWindowName,
        surface_name,
        surface_name_length
    );
    HWND hWnd = CreateWindowEx(
        dwExStyle,
        windowClass.lpszClassName,
        lpWindowName,
        dwStyle,
        x, y,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        hWndParent,
        nullptr,
        state->hInstance,
        nullptr
    );
    if( !hWnd ) {
        win_log_error( true );
        return nullptr;
    }
    HDC dc = GetDC( hWnd );
    if( !dc ) {
        win_log_error( true );
        return nullptr;
    }

    win_surface->hWnd             = hWnd;
    win_surface->hDc              = dc;
    win_surface->surface.position = { x, y };
    
    win_surface->state = platform_state;

    SetWindowLongPtr(
        win_surface->hWnd,
        GWLP_USERDATA,
        (LONG_PTR)win_surface
    );

    if( ARE_BITS_SET( flags, SURFACE_CREATE_VISIBLE ) ) {
        win_surface->surface.is_visible = true;
        ShowWindow( hWnd, SW_SHOW );
    }

    return (Surface*)win_surface;
}

void surface_destroy(
    PlatformState*,
    Surface* surface
) {
    Win32Surface* win_surface = (Win32Surface*)surface;
    
    DestroyWindow( win_surface->hWnd );
}

b32 surface_pump_events( Surface* surface ) {
    Win32Surface* win_surface = (Win32Surface*)surface;
    MSG message;
    while(PeekMessage(
        &message,
        win_surface->hWnd,
        0, 0,
        PM_REMOVE
    )) {
        TranslateMessage( &message );
        DispatchMessage( &message );
    }

    return true;
}
void surface_swap_buffers( Surface* surface ) {
    Win32Surface* win_surface = (Win32Surface*)surface;
    SwapBuffers( win_surface->hDc );
}
void surface_set_name( Surface* surface, const char* new_name ) {
    usize new_name_length = str_length( new_name ) + 1;
    if( new_name_length > MAX_SURFACE_NAME_LENGTH ) {
        LOG_WARN(
            "Surface name \"%s\" is too long! "
            "It will be truncated!"
        );
        new_name_length = MAX_SURFACE_NAME_LENGTH;
    }

    mem_copy(
        surface->name,
        new_name,
        new_name_length
    );
    surface->name[new_name_length - 1] = 0;
    
    Win32Surface* win_surface = (Win32Surface*)surface;
    SetWindowTextA( win_surface->hWnd, surface->name );
}

#define TRANSITION_STATE_MASK (1 << 31)
#define EXTENDED_KEY_MASK     (1 << 24)
#define SCANCODE_MASK         0x00FF0000

internal LRESULT window_proc(
    HWND hWnd, UINT Msg,
    WPARAM wParam, LPARAM lParam
) {

    Win32Surface* surface = (Win32Surface*)GetWindowLongPtr(
        hWnd,
        GWLP_USERDATA
    );

    if( !surface ) {
        return DefWindowProc(
            hWnd,
            Msg,
            wParam,
            lParam
        );
    }

    Event event = {};
    switch( Msg ) {
        case WM_DESTROY: {
            event.code = EVENT_CODE_SURFACE_DESTROY;
            event.data.surface_destroy.surface = surface;
            event_fire( event );
        } break;

        case WM_ACTIVATE: {
            b32 is_active = wParam == WA_ACTIVE ||
                wParam == WA_CLICKACTIVE;
            XInputEnable( (BOOL)is_active );
            event.code = EVENT_CODE_SURFACE_ACTIVE;
            event.data.surface_active.is_active = is_active;
            event.data.surface_active.surface   = surface;
            event_fire( event );

            if( !is_active ) {
                platform_cursor_set_visible( true );
            }

            IS_ACTIVE = is_active;
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

                surface->surface.dimensions = dimensions;

                event.code = EVENT_CODE_SURFACE_RESIZE;
                event.data.surface_resize.surface    = surface;
                event.data.surface_resize.dimensions = dimensions;
                event_fire( event );

                last_rect = rect;
            }

            local ivec2 last_position = {};
            WINDOWPOS* window_pos = (WINDOWPOS*)lParam;
            ivec2 position = {
                window_pos->x,
                window_pos->y
            };

            if( position != last_position ) {
                surface->surface.position = position;

                event.code = EVENT_CODE_SURFACE_MOVE;
                event.data.surface_move.surface  = surface;
                event.data.surface_move.position = position;
                event_fire( event );
            }
            last_position = position;

        } return FALSE;

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP: {

            if( !IS_ACTIVE ) {
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

            event.code = EVENT_CODE_INPUT_KEY;
            event.data.keyboard.code    = (KeyCode)keycode;
            event.data.keyboard.is_down = is_down;
            event_fire( event );

        } return TRUE;
        
        case WM_MOUSEMOVE: {

            if( !IS_ACTIVE ) {
                break;
            }

            RECT client_rect = {};
            GetClientRect( hWnd, &client_rect );

            ivec2 mouse_position = {};
            mouse_position.x = GET_X_LPARAM(lParam);
            mouse_position.y = client_rect.bottom - GET_Y_LPARAM(lParam);
            input_set_mouse_position( mouse_position );

            event.code = EVENT_CODE_INPUT_MOUSE_MOVE;
            event.data.mouse_move.coord = mouse_position;
            event_fire( event );

        } return TRUE;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {

            if( !IS_ACTIVE ) {
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
            event.code = EVENT_CODE_INPUT_MOUSE_BUTTON;
            event.data.mouse_button.code    = code;
            event.data.mouse_button.is_down = is_down;
            event_fire( event );

        } return TRUE;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP: {
            if( !IS_ACTIVE ) {
                break;
            }

            UINT button = GET_XBUTTON_WPARAM(wParam);
            b32 is_down = Msg == WM_XBUTTONDOWN;
            MouseCode code = (MouseCode)(button + (MBC_BUTTON_EXTRA_1 - 1));

            input_set_mouse_button( code, is_down );
            event.code = EVENT_CODE_INPUT_MOUSE_BUTTON;
            event.data.mouse_button.code    = code;
            event.data.mouse_button.is_down = is_down;
            event_fire( event );

        } return TRUE;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL: {

            if( !IS_ACTIVE ) {
                break;
            }

            i64 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            delta = delta == 0 ? 0 : absolute(delta);
            event.data.mouse_wheel.delta = delta;

            if( Msg == WM_MOUSEWHEEL ) {
                event.code = EVENT_CODE_INPUT_MOUSE_WHEEL;
                input_set_mouse_wheel( delta );
            } else {
                event.code = EVENT_CODE_INPUT_HORIZONTAL_MOUSE_WHEEL;
                input_set_horizontal_mouse_wheel( delta );
            }

            event_fire( event );

        } return TRUE;

        case WM_SETCURSOR: switch( LOWORD( lParam ) ) {
            case HTRIGHT:
            case HTLEFT: {
                platform_cursor_set_style( CURSOR_RESIZE_HORIZONTAL );
            } break;

            case HTTOP:
            case HTBOTTOM: {
                platform_cursor_set_style( CURSOR_RESIZE_VERTICAL );
            } break;

            case HTBOTTOMLEFT:
            case HTTOPRIGHT: {
                platform_cursor_set_style( CURSOR_RESIZE_TOP_RIGHT_BOTTOM_LEFT );
            } break;

            case HTBOTTOMRIGHT:
            case HTTOPLEFT: {
                platform_cursor_set_style( CURSOR_RESIZE_TOP_LEFT_BOTTOM_RIGHT );
            } break;

            default: {
                platform_cursor_set_style( CURSOR_ARROW );
            } break;
        } return TRUE;

    }

    return DefWindowProc(
        hWnd,
        Msg,
        wParam,
        lParam
    );
}

// SURFACE | END ----------------------------------------------------------

inline LPCTSTR cursor_style_to_win_style( MouseCursorStyle style ) {
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



void platform_cursor_set_style( MouseCursorStyle style ) {
    LPCTSTR win_style = cursor_style_to_win_style( style );
    SetCursor(
        LoadCursor(
            nullptr,
            win_style
        )
    );

    Event event = {};
    event.code = EVENT_CODE_MOUSE_CURSOR_CHANGED;
    event.data.raw.uint32[0] = style;
    event_fire( event );

    CURSOR_STATE.style = style;
}

void platform_cursor_set_visible( b32 visible ) {
    ShowCursor( visible );

    CURSOR_STATE.is_visible = visible;
}

void platform_cursor_set_locked( Surface* surface, b32 lock ) {
    if( lock ) {
        CURSOR_STATE.surface_locked = surface;
        platform_cursor_center( surface );
        platform_cursor_set_visible( false );
    } else {
        CURSOR_STATE.surface_locked = nullptr;
        platform_cursor_set_visible( true );
    }
}

void platform_cursor_center( Surface* surface ) {
    POINT center = {};
    center.x = surface->dimensions.x / 2;
    center.y = surface->dimensions.y / 2;

    Win32Surface* win_surface = (Win32Surface*)surface;

    ClientToScreen( win_surface->hWnd, &center );
    SetCursorPos( center.x, center.y );
}

void platform_poll_gamepad() {
    if( !IS_ACTIVE ) {
        return;
    }

    XINPUT_STATE gamepad_state = {};
    DWORD max_index = XUSER_MAX_COUNT > MAX_GAMEPAD_INDEX ?
        MAX_GAMEPAD_INDEX :
        XUSER_MAX_COUNT;

    Event event = {};
    for(
        DWORD gamepad_index = 0;
        gamepad_index < max_index;
        ++gamepad_index
    ) {

        DWORD is_active = XInputGetState(
            gamepad_index,
            &gamepad_state
        );
        if( is_active != ERROR_SUCCESS ) {
            // if gamepad activated this frame, fire an event
            b32 was_active = input_is_pad_active( gamepad_index );
            if( was_active != is_active && is_active ) {
                event.code = EVENT_CODE_INPUT_GAMEPAD_ACTIVATE;
                event.data.gamepad_activate.gamepad_index = gamepad_index;
                event_fire( event );
            }

            input_set_pad_active( gamepad_index, is_active );
            continue;
        }

        XINPUT_GAMEPAD gamepad = gamepad_state.Gamepad;

        b32 dpad_left  = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT );
        b32 dpad_right = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT );
        b32 dpad_up    = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP );
        b32 dpad_down  = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN );

        b32 face_left  = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_X );
        b32 face_right = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_B );
        b32 face_up    = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_Y );
        b32 face_down  = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_A );

        b32 start  = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_START );
        b32 select = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_BACK );

        b32 bumper_left  = ARE_BITS_SET(
            gamepad.wButtons,
            XINPUT_GAMEPAD_LEFT_SHOULDER
        );
        b32 bumper_right = ARE_BITS_SET(
            gamepad.wButtons,
            XINPUT_GAMEPAD_RIGHT_SHOULDER
        );

        b32 click_left  = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB );
        b32 click_right = ARE_BITS_SET( gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB );

        #define HALF_TRIGGER_PRESS 127;

        b32 trigger_left  = gamepad.bLeftTrigger  >= HALF_TRIGGER_PRESS;
        b32 trigger_right = gamepad.bRightTrigger >= HALF_TRIGGER_PRESS;

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_LEFT,
            dpad_left
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_RIGHT,
            dpad_right
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_UP,
            dpad_up
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_DPAD_DOWN,
            dpad_down
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_LEFT,
            face_left
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_RIGHT,
            face_right
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_UP,
            face_up
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_FACE_DOWN,
            face_down
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_START,
            start
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_SELECT,
            select
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_BUMPER_LEFT,
            bumper_left
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_BUMPER_RIGHT,
            bumper_right
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_TRIGGER_LEFT,
            trigger_left
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_TRIGGER_RIGHT,
            trigger_right
        );

        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_LEFT_CLICK,
            click_left
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_RIGHT_CLICK,
            click_right
        );

        f32 trigger_left_axis  = normalize_range( gamepad.bLeftTrigger );
        f32 trigger_right_axis = normalize_range( gamepad.bRightTrigger );

        input_set_pad_trigger_left(
            gamepad_index,
            trigger_left_axis
        );
        input_set_pad_trigger_right(
            gamepad_index,
            trigger_right_axis
        );

        // TODO(alicia): this may not be correct . . .

        vec2 stick_left = v2(
            normalize_range( gamepad.sThumbLX ),
            normalize_range( gamepad.sThumbLY )
        );
        vec2 stick_right = v2(
            normalize_range( gamepad.sThumbRX ),
            normalize_range( gamepad.sThumbRY )
        );
        
        b32 stick_left_moved = absolute( gamepad.sThumbLX ) >=
            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
        b32 stick_right_moved = absolute( gamepad.sThumbRX ) >=
            XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_LEFT,
            stick_left_moved
        );
        input_set_pad_button(
            gamepad_index,
            PAD_CODE_STICK_RIGHT,
            stick_right_moved
        );
        
        if( stick_left_moved ) {
            input_set_pad_stick_left(
                gamepad_index,
                stick_left
            );
            event = {};
            event.code = EVENT_CODE_INPUT_GAMEPAD_STICK_LEFT;
            event.data.gamepad_stick.gamepad_index = gamepad_index;
            event.data.gamepad_stick.value         = stick_left;
            event_fire( event );
        }

        if( stick_right_moved ) {
            input_set_pad_stick_right(
                gamepad_index,
                stick_right
            );
            event = {};
            event.code = EVENT_CODE_INPUT_GAMEPAD_STICK_RIGHT;
            event.data.gamepad_stick.gamepad_index = gamepad_index;
            event.data.gamepad_stick.value         = stick_right;
            event_fire( event );
        }
    }
}

void platform_set_pad_motor_state(
    u32 gamepad_index,
    u32 motor,
    f32 value
) {
    XINPUT_VIBRATION vibration = {};
    if( motor == GAMEPAD_MOTOR_LEFT ) {
        f32 right_motor = input_query_motor_state(
            gamepad_index,
            GAMEPAD_MOTOR_RIGHT
        );
        vibration.wLeftMotorSpeed  = (WORD)( value * (f32)U16::MAX );
        vibration.wRightMotorSpeed = (WORD)( right_motor * (f32)U16::MAX );
    } else {
        f32 left_motor = input_query_motor_state(
            gamepad_index,
            GAMEPAD_MOTOR_LEFT
        );
        vibration.wLeftMotorSpeed  = (WORD)( left_motor * (f32)U16::MAX );
        vibration.wRightMotorSpeed = (WORD)( value * (f32)U16::MAX );
    }

    XInputSetState( gamepad_index, &vibration );
}

void sleep( u32 ms ) {
    DWORD dwMilliseconds = ms;
    Sleep( dwMilliseconds );
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
    result.thread_count = win32_info.dwNumberOfProcessors;

#if defined(SM_ARCH_X86)
    mem_set(
        ' ',
        CPU_NAME_BUFFER_LEN,
        result.cpu_name_buffer
    );
    result.cpu_name_buffer[CPU_NAME_BUFFER_LEN - 1] = 0;

    int cpu_info[4] = {};
    __cpuid( cpu_info, 0x80000002 );
    memcpy(
        result.cpu_name_buffer,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000003 );
    memcpy(
        result.cpu_name_buffer + 16,
        cpu_info,
        sizeof(cpu_info)
    );
    __cpuid( cpu_info, 0x80000004 );
    memcpy(
        result.cpu_name_buffer + 32,
        cpu_info,
        sizeof(cpu_info)
    );

    str_trim_trailing_whitespace(
        CPU_NAME_BUFFER_LEN,
        result.cpu_name_buffer
    );
#endif

    return result;
}

#endif // SM_PLATFORM_WINDOWS
