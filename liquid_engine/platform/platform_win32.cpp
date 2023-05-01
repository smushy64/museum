/**
 * Description:  Win32 Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#if defined(SM_PLATFORM_WINDOWS)

#include "os.h"
#include "memory.h"
#include "threading.h"
#include "core/logging.h"

#include <intrin.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <xinput.h>

#include <stdio.h>

SM_GLOBAL b32 IS_DPI_AWARE = false;

// LOGGING | BEGIN --------------------------------------------------------

#if defined(LD_LOGGING)
    #define WIN_LOG_NOTE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            "[NOTE WIN32  ] " __VA_ARGS__\
        )
    #define WIN_LOG_INFO( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            "[INFO WIN32  ] " __VA_ARGS__\
        )
    #define WIN_LOG_DEBUG( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            "[DEBUG WIN32 ] " __VA_ARGS__\
        )
    #define WIN_LOG_WARN( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            "[WARN WIN32  ] " __VA_ARGS__\
        )
    #define WIN_LOG_ERROR( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            "[ERROR WIN32 ] " __VA_ARGS__\
        )


    #define WIN_LOG_NOTE_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET, 0,\
            "[NOTE WIN32  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE | LOG_LEVEL_VERBOSE,\
            LOG_COLOR_RESET,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )

    #define WIN_LOG_INFO_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE, 0,\
            "[INFO WIN32  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_WHITE,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define WIN_LOG_DEBUG_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE, 0,\
            "[DEBUG WIN32 | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_DEBUG | LOG_LEVEL_TRACE,\
            LOG_COLOR_BLUE,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define WIN_LOG_WARN_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW, 0,\
            "[WARN WIN32  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_WARN | LOG_LEVEL_TRACE,\
            LOG_COLOR_YELLOW,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
    #define WIN_LOG_ERROR_TRACE( ... ) \
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED, 0,\
            "[ERROR WIN32 | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
            LOG_COLOR_RED,\
            LOG_FLAG_NEW_LINE,\
            __VA_ARGS__\
        )
#else
    #define WIN_LOG_NOTE( ... )
    #define WIN_LOG_INFO( ... )
    #define WIN_LOG_DEBUG( ... )
    #define WIN_LOG_WARN( ... )
    #define WIN_LOG_ERROR( ... )
    #define WIN_LOG_NOTE_TRACE( ... )
    #define WIN_LOG_INFO_TRACE( ... )
    #define WIN_LOG_DEBUG_TRACE( ... )
    #define WIN_LOG_WARN_TRACE( ... )
    #define WIN_LOG_ERROR_TRACE( ... )
#endif

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

            usize new_message_buffer_size = message_buffer_size + 128;
            char new_message_buffer[new_message_buffer_size];
            snprintf(
                new_message_buffer,
                new_message_buffer_size,
                "Please contact me at smushybusiness@gmail.com\n%ls",
                message_buffer
            );

            MESSAGE_BOX_FATAL(
                title_buffer,
                new_message_buffer
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

namespace internal {
    [[maybe_unused]]
    SM_INTERNAL b32 win_library_load(
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
    SM_INTERNAL b32 win_library_load_trace(
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
    SM_INTERNAL void win_library_free(
        HMODULE module
    ) {
        FreeLibrary( module );
    }
    [[maybe_unused]]
    SM_INTERNAL void win_library_free_trace(
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
    SM_GLOBAL SetProcessDpiAwarenessContext_fn in_SetProcessDpiAwarenessContext = nullptr;

    typedef UINT (*GetDpiForSystem_fn)();
    [[maybe_unused]]
    SM_GLOBAL GetDpiForSystem_fn in_GetDpiForSystem = nullptr;

    typedef BOOL (*AdjustWindowRectExForDpi_fn)( LPRECT, DWORD, BOOL, DWORD, UINT );
    [[maybe_unused]]
    SM_GLOBAL AdjustWindowRectExForDpi_fn in_AdjustWindowRectExForDpi = nullptr;

    typedef DWORD (*XInputGetState_fn)( DWORD, XINPUT_STATE* );
    [[maybe_unused]]
    SM_GLOBAL XInputGetState_fn in_XInputGetState = nullptr;

    typedef DWORD (*XInputSetState_fn)( DWORD, XINPUT_VIBRATION* );
    [[maybe_unused]]
    SM_GLOBAL XInputSetState_fn in_XInputSetState = nullptr;

    typedef HGLRC (*wglCreateContext_fn)(HDC);
    [[maybe_unused]]
    SM_GLOBAL wglCreateContext_fn in_wglCreateContext = nullptr;

    typedef BOOL (*wglMakeCurrent_fn)(HDC, HGLRC);
    [[maybe_unused]]
    SM_GLOBAL wglMakeCurrent_fn in_wglMakeCurrent = nullptr;

    typedef BOOL (*wglDeleteContext_fn)(HGLRC);
    [[maybe_unused]]
    SM_GLOBAL wglDeleteContext_fn in_wglDeleteContext = nullptr;

    typedef PROC (*wglGetProcAddress_fn)(LPCSTR);
    [[maybe_unused]]
    SM_GLOBAL wglGetProcAddress_fn in_wglGetProcAddress = nullptr;

    typedef HGLRC (*wglCreateContextAttribsARB_fn)(HDC, HGLRC, const int*);
    [[maybe_unused]]
    SM_GLOBAL wglCreateContextAttribsARB_fn in_wglCreateContextAttribsARB = nullptr;

    typedef int (*DescribePixelFormat_fn)(HDC, int, UINT, LPPIXELFORMATDESCRIPTOR);
    [[maybe_unused]]
    SM_GLOBAL DescribePixelFormat_fn in_DescribePixelFormat = nullptr;

    typedef int (*ChoosePixelFormat_fn)(HDC, const PIXELFORMATDESCRIPTOR*);
    [[maybe_unused]]
    SM_GLOBAL ChoosePixelFormat_fn in_ChoosePixelFormat = nullptr;

    typedef BOOL (*SetPixelFormat_fn)(HDC, int, const PIXELFORMATDESCRIPTOR*);
    [[maybe_unused]]
    SM_GLOBAL SetPixelFormat_fn in_SetPixelFormat = nullptr;

    typedef BOOL (*SwapBuffers_fn)(HDC);
    [[maybe_unused]]
    SM_GLOBAL SwapBuffers_fn in_SwapBuffers = nullptr;

    typedef void (*XInputEnable_fn)( BOOL );
    SM_INTERNAL void XInputEnableStub( BOOL enable ) { SM_UNUSED( enable ); }
    [[maybe_unused]]
    SM_GLOBAL XInputEnable_fn in_XInputEnable = XInputEnableStub;

} // namespace internal

#if defined(LD_LOGGING)
    #define win_library_load( module_name, out_module )\
        internal::win_library_load_trace(\
            module_name,\
            out_module,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define win_library_free( module )\
        internal::win_library_free_trace(\
            module,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define win_library_load( module_name, out_module ) \
        internal::win_library_load( module_name, out_module )
    #define win_library_free( module )\
        internal::win_library_free( module )
#endif

#define SetProcessDpiAwarenessContext ::internal::in_SetProcessDpiAwarenessContext
#define GetDpiForSystem               ::internal::in_GetDpiForSystem
#define AdjustWindowRectExForDpi      ::internal::in_AdjustWindowRectExForDpi
#define XInputGetState                ::internal::in_XInputGetState
#define XInputSetState                ::internal::in_XInputSetState
#define XInputEnable                  ::internal::in_XInputEnable
#define wglCreateContext              ::internal::in_wglCreateContext
#define wglMakeCurrent                ::internal::in_wglMakeCurrent
#define wglDeleteContext              ::internal::in_wglDeleteContext
#define wglGetProcAddress             ::internal::in_wglGetProcAddress
#define wglCreateContextAttribsARB    ::internal::in_wglCreateContextAttribsARB
#define DescribePixelFormat           ::internal::in_DescribePixelFormat
#define ChoosePixelFormat             ::internal::in_ChoosePixelFormat
#define SetPixelFormat                ::internal::in_SetPixelFormat
#define SwapBuffers                   ::internal::in_SwapBuffers

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
SM_INTERNAL void* win_proc_address(
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
            "Function \"%s\" loaded from module \"%ls\".",
            proc_name,
            module_name_buffer
        );
    } else {
        WIN_LOG_ERROR(
            "Failed to load function \"%s\" from module \"%ls\"!",
            proc_name,
            module_name_buffer
        );
    }
#endif

    return result;
}
[[maybe_unused]]
SM_INTERNAL void* win_proc_address_required(
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
            "Function \"%s\" loaded from module \"%ls\".",
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

struct Win32State {
    HINSTANCE hInstace;

    HMODULE   libUser32;
    HMODULE   libXinput;
    HMODULE   libGl;
    HMODULE   libGdi32;
};

SM_API b32 platform_init(
    PlatformInitFlags flags,
    PlatformState* out_state
) {
    void* win32_state_buffer = heap_alloc( sizeof(Win32State) );
    if( !win32_state_buffer ) {
        MESSAGE_BOX_FATAL(
            "Out of Memory",
            "Could not allocate space for Win32 State!"
        );
        return false;
    }
    *out_state = win32_state_buffer;
    Win32State* state = (Win32State*)out_state;

    if( !win_library_load(
        L"USER32.DLL",
        &state->libUser32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load user32.dll!"
        );
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
                return false;
            }
        }
    }
    if( !win_library_load(
        L"OPENGL32.DLL",
        &state->libGl
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load opengl32.dll!"
        );
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
        return false;
    }

    SetProcessDpiAwarenessContext =
    (::internal::SetProcessDpiAwarenessContext_fn)win_proc_address_required(
        state->libUser32,
        "SetProcessDpiAwarenessContext"
    );
    if( !SetProcessDpiAwarenessContext ) {
        return false;
    }

    GetDpiForSystem =
    (::internal::GetDpiForSystem_fn)win_proc_address_required(
        state->libUser32,
        "GetDpiForSystem"
    );
    if( !GetDpiForSystem ) {
        return false;
    }

    AdjustWindowRectExForDpi =
    (::internal::AdjustWindowRectExForDpi_fn)win_proc_address_required(
        state->libUser32,
        "AdjustWindowRectExForDpi"
    );
    if( !AdjustWindowRectExForDpi ) {
        return false;
    }

    XInputGetState =
    (::internal::XInputGetState_fn)win_proc_address_required(
        state->libXinput,
        "XInputGetState"
    );
    if( !XInputGetState ) {
        return false;
    }
    XInputSetState =
    (::internal::XInputSetState_fn)win_proc_address_required(
        state->libXinput,
        "XInputSetState"
    );
    if( !XInputSetState ) {
        return false;
    }

    wglCreateContext =
    (::internal::wglCreateContext_fn)win_proc_address_required(
        state->libGl,
        "wglCreateContext"
    );
    if( !wglCreateContext ) {
        return false;
    }
    wglMakeCurrent =
    (::internal::wglMakeCurrent_fn)win_proc_address_required(
        state->libGl,
        "wglMakeCurrent"
    );
    if( !wglMakeCurrent ) {
        return false;
    }
    wglDeleteContext =
    (::internal::wglDeleteContext_fn)win_proc_address_required(
        state->libGl,
        "wglDeleteContext"
    );
    if( !wglDeleteContext ) {
        return false;
    }
    wglGetProcAddress =
    (::internal::wglGetProcAddress_fn)win_proc_address_required(
        state->libGl,
        "wglGetProcAddress"
    );
    if( !wglGetProcAddress ) {
        return false;
    }

    DescribePixelFormat =
    (::internal::DescribePixelFormat_fn)win_proc_address_required(
        state->libGdi32,
        "DescribePixelFormat"
    );
    if( !DescribePixelFormat ) {
        return false;
    }
    ChoosePixelFormat =
    (::internal::ChoosePixelFormat_fn)win_proc_address_required(
        state->libGdi32,
        "ChoosePixelFormat"
    );
    if( !ChoosePixelFormat ) {
        return false;
    }
    SetPixelFormat =
    (::internal::SetPixelFormat_fn)win_proc_address_required(
        state->libGdi32,
        "SetPixelFormat"
    );
    if( !SetPixelFormat ) {
        return false;
    }
    SwapBuffers =
    (::internal::SwapBuffers_fn)win_proc_address_required(
        state->libGdi32,
        "SwapBuffers"
    );
    if( !SwapBuffers ) {
        return false;
    }

    if( (flags & PLATFORM_INIT_DPI_AWARE) == PLATFORM_INIT_DPI_AWARE ) {
        SetProcessDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        );
        IS_DPI_AWARE = true;
    }

    return true;
}

SM_API void platform_shutdown( PlatformState* pstate ) {
    Win32State* state = (Win32State*)pstate;

    win_library_free( state->libUser32 );
    win_library_free( state->libXinput );
    win_library_free( state->libGl );
    win_library_free( state->libGdi32 );

    heap_free( state );
}

// PLATFORM INIT | END ----------------------------------------------------

// MEMORY | BEGIN ---------------------------------------------------------

SM_GLOBAL usize HEAP_MEMORY_USAGE = 0;
SM_GLOBAL usize PAGE_MEMORY_USAGE = 0;
SM_API usize query_heap_memory_usage() {
    return HEAP_MEMORY_USAGE;
}
SM_API usize query_page_memory_usage() {
    return PAGE_MEMORY_USAGE;
}

SM_API void* heap_alloc( usize size ) {
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
SM_API void* heap_realloc( void* memory, usize new_size ) {

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
SM_API void heap_free( void* memory ) {

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

SM_API void* page_alloc( usize size ) {
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
SM_API void page_free( void* memory ) {
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

struct Win32ThreadHandle {
    HANDLE     handle;
    ThreadProc proc;
    void*      params;
    DWORD      id;
};

SM_INTERNAL DWORD WINAPI win_thread_proc( void* params ) {
    Win32ThreadHandle* thread_handle = (Win32ThreadHandle*)params;

    DWORD return_value = thread_handle->proc(
        thread_handle->params
    );

    heap_free( params );

    return return_value;
}

#define THREAD_STACK_SIZE_SAME_AS_MAIN 0
#define THREAD_RUN_ON_CREATE 0
SM_API ThreadHandle thread_create(
    ThreadProc thread_proc,
    void*      params,
    b32        run_on_creation
) {
    void* handle_buffer = heap_alloc( sizeof(Win32ThreadHandle) );
    if( !handle_buffer ) {
        return nullptr;
    }
    Win32ThreadHandle* thread_handle = (Win32ThreadHandle*)handle_buffer;
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
        thread_resume( handle_buffer );
    }

    return handle_buffer;
}
SM_API void thread_resume( ThreadHandle thread ) {
    Win32ThreadHandle* win32_thread = (Win32ThreadHandle*)thread;
    ResumeThread( win32_thread->handle );
}

SM_API Semaphore semaphore_create(
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
SM_API void semaphore_increment(
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
SM_API void semaphore_wait_for(
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
SM_API void semaphore_wait_for_multiple(
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
SM_API void semaphore_destroy( Semaphore semaphore ) {
    HANDLE win32_handle = (HANDLE)semaphore;
    CloseHandle( win32_handle );
}

SM_API u32 interlocked_increment( volatile u32* addend ) {
    return InterlockedIncrement( addend );
}
SM_API u32 interlocked_decrement( volatile u32* addend ) {
    return InterlockedDecrement( addend );
}
SM_API u32 interlocked_exchange( volatile u32* target, u32 value ) {
    return InterlockedExchange( target, value );
}
SM_API void* interlocked_compare_exchange_pointer(
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
SM_API u32 interlocked_compare_exchange(
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

SM_API void mem_fence() {
    _ReadWriteBarrier();
#if defined(SM_ARCH_X86)
    _mm_mfence();
#elif
    #error "mem_fence: Platform is not supported!"
#endif
}
SM_API void read_fence() {
    _ReadBarrier();
#if defined(SM_ARCH_X86)
    _mm_lfence();
#elif
    #error "read_fence: Platform is not supported!"
#endif
}
SM_API void write_fence() {
    _WriteBarrier();
#if defined(SM_ARCH_X86)
    _mm_sfence();
#elif
    #error "write_fence: Platform is not supported!"
#endif
}

// MULTI-THREADING | END   ------------------------------------------------

// MESSAGE BOX | BEGIN ----------------------------------------------------

SM_API MessageBoxResult message_box(
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

struct Win32SurfaceData {
    HDC hDc;
};

SM_GLOBAL HINSTANCE HINSTANCE_HANDLE = nullptr;
SM_INTERNAL HINSTANCE get_hinstance() {
    if( !HINSTANCE_HANDLE ) {
        HINSTANCE_HANDLE = GetModuleHandleA(0);
    }
    return HINSTANCE_HANDLE;
}

SM_INTERNAL LRESULT win_proc(
    HWND hWnd, UINT Msg,
    WPARAM wParam, LPARAM lParam
) {
    switch( Msg ) {
        default: return DefWindowProc(
            hWnd,
            Msg,
            wParam,
            lParam
        );
    }
}

SM_API b32 surface_create(
    const char*        surface_name,
    ivec2              dimensions,
    SurfaceCreateFlags flags,
    SurfaceHandle      opt_parent_surface,
    Surface*           out_surface
) {
    HWND hWndParent = (HWND)opt_parent_surface;

    void* win32_surface_data_buffer = HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        sizeof(Win32SurfaceData)
    );
    if( !win32_surface_data_buffer ) {
        MESSAGE_BOX_FATAL(
            "Out of memory",
            "Could not allocate Win32 surface data!"
        );
        return false;
    }
    
    Win32SurfaceData* win32_surface = (Win32SurfaceData*)win32_surface_data_buffer;

    WNDCLASSEX window_class  = {};
    window_class.cbSize      = sizeof(WNDCLASSEX);
    window_class.lpfnWndProc = win_proc;
    window_class.hInstance   = get_hinstance();

    static const wchar_t* BASE_CLASS_NAME = L"LiquidEngineWindowClass";
    static const usize BASE_CLASS_NAME_LEN = wcslen(BASE_CLASS_NAME);
    static int SURFACE_COUNT = 0;

    static const usize CLASS_NAME_BUFFER_SIZE = BASE_CLASS_NAME_LEN + 8;
    wchar_t class_name_buffer[CLASS_NAME_BUFFER_SIZE];
    snwprintf(
        class_name_buffer,
        CLASS_NAME_BUFFER_SIZE,
        L"%ls%i",
        BASE_CLASS_NAME,
        SURFACE_COUNT
    );

    window_class.lpszClassName = class_name_buffer;

    if( !RegisterClassEx( &window_class ) ) {
        win_log_error( true );
        HeapFree(
            GetProcessHeap(),
            0,
            win32_surface
        );
        return false;
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;

    i32 width = 0, height = 0;
    RECT window_rect = {};
    if( IS_DPI_AWARE ) {
        UINT dpi = GetDpiForSystem();

        width = MulDiv(
            dimensions.x,
            dpi,
            96
        );
        height = MulDiv(
            dimensions.y,
            dpi,
            96
        );

        out_surface->width  = width;
        out_surface->height = height;

        window_rect.right  = width;
        window_rect.bottom = height;

        if( !AdjustWindowRectExForDpi(
            &window_rect,
            dwStyle,
            FALSE,
            0,
            dpi
        ) ) {
            win_log_error( true );
            HeapFree(
                GetProcessHeap(),
                0,
                win32_surface
            );
            return false;
        }
    } else {
        DEBUG_UNIMPLEMENTED();
    }

    int window_name_len = strlen(surface_name) + 1;
    wchar_t window_name_buffer[window_name_len];
    mbstowcs(
        window_name_buffer,
        surface_name,
        window_name_len
    );

    HWND hWnd = CreateWindowEx(
        0,
        window_class.lpszClassName,
        window_name_buffer,
        dwStyle,
        // TODO(alicia): center window!
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        window_rect.right - window_rect.left,
        window_rect.bottom - window_rect.top,
        hWndParent,
        nullptr,
        get_hinstance(),
        nullptr
    );
    if( !hWnd ) {
        win_log_error( true );
        HeapFree(
            GetProcessHeap(),
            0,
            win32_surface
        );
        return false;
    }

    // TODO(alicia): set window position!
    out_surface->handle = hWnd;

    if( (flags & SURFACE_FLAG_SHOW_ON_CREATE) == SURFACE_FLAG_SHOW_ON_CREATE ) {
        out_surface->is_focused = true;
        ShowWindow( hWnd, SW_SHOW );
    }

    return true;
}

SM_API b32 next_event( SurfaceHandle surface, Event* event ) {
    HWND window = (HWND)surface;

    *event = {};

    MSG message = {};
    if( !PeekMessage(
        &message,
        window,
        0, 0,
        PM_REMOVE
    ) ) {
        return false;
    }

    switch( message.message ) {
        case WM_ACTIVATE: {
            event->type = EVENT_SURFACE_FOCUS_CHANGE;
            event->focus_change.is_focused =
                message.wParam == WA_ACTIVE ||
                message.wParam == WA_CLICKACTIVE;
        } break;

        case WM_DESTROY: {
            event->type = EVENT_SURFACE_DESTROY;
        } break;

        case WM_WINDOWPOSCHANGED: {
            WINDOWPOS* lpWindowPos =
                (WINDOWPOS*)message.lParam;

            if( !lpWindowPos ) {
                break;
            }
            
            // moved
            if(
                ((lpWindowPos->flags & SWP_NOSIZE) == SWP_NOSIZE) &&
                !((lpWindowPos->flags & SWP_NOMOVE) == SWP_NOMOVE)
            ) {
                event->type = EVENT_SURFACE_POSITION_CHANGE;
                event->position_change.x = lpWindowPos->x;
                event->position_change.y = lpWindowPos->y;

                return true;
            // resized
            } else {
                RECT client_rect = {};
                if( GetClientRect(
                    window,
                    &client_rect
                ) ) {
                    event->type = EVENT_SURFACE_DIMENSIONS_CHANGE;

                    static const i64 MIN_DIMENSIONS = 1;
                    event->dimensions_change.width =
                        max( client_rect.right, MIN_DIMENSIONS );
                    event->dimensions_change.height =
                        max( client_rect.bottom, MIN_DIMENSIONS );
                    
                    return true;
                }
            }

        } break;

        default: {
            TranslateMessage( &message );
            DispatchMessage( &message );
        } return false;
    }

    return true;
}

// SURFACE | END ----------------------------------------------------------

SM_API void sleep( u32 ms ) {
    DWORD dwMilliseconds = ms;
    Sleep( dwMilliseconds );
}

SM_API SystemInfo query_system_info() {
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
#endif

    return result;
}

#endif // SM_PLATFORM_WINDOWS
