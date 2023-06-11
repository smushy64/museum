/**
 * Description:  Win32 Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "platform_win32.h"

#if defined(SM_PLATFORM_WINDOWS)

#include "platform.h"
#include "core/logging.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/collections.h"
#include "core/events.h"
#include "core/math.h"

#include "renderer/renderer.h"

// #define VK_USE_PLATFORM_WIN32_KHR
// #include <vulkan/vulkan.h>
// #include "renderer/vulkan/vk_backend.h"

#include <glad/glad.h>
#include "renderer/opengl/gl_backend.h"

#include <intrin.h>

// TODO(alicia): custom formatting and printing
#include <stdio.h>

global char* ERROR_MESSAGE_BUFFER = nullptr;
global b32 IS_DPI_AWARE = false;

b32 platform_init(
    const char* opt_surface_name,
    ivec2 surface_dimensions,
    PlatformFlags flags,
    Platform* out_platform
) {

    IS_DPI_AWARE = ARE_BITS_SET( flags, PLATFORM_DPI_AWARE );

    Win32Platform* win32_platform = (Win32Platform*)mem_alloc(
        sizeof(Win32Platform),
        MEMTYPE_PLATFORM_DATA
    );
    if( !win32_platform ) {
        MESSAGE_BOX_FATAL(
            "Fatal Windows Error",
            "Out of memory.\n"
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    // load libraries
    if( !win32_load_user32( &win32_platform->lib_user32 ) ) {
        mem_free( win32_platform );
        return false;
    }
    if( !win32_load_xinput( &win32_platform->lib_xinput ) ) {
        mem_free( win32_platform );
        return false;
    }
    if( !win32_library_load(
        L"GDI32.DLL",
        &win32_platform->lib_gdi32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load gdi32.dll!"
        );
        return false;
    }
    GetStockObject = (::impl::GetStockObjectFN)win32_proc_address_required(
        win32_platform->lib_gdi32,
        "GetStockObject"
    );
    if( !GetStockObject ) {
        return false;
    }

    ERROR_MESSAGE_BUFFER = win32_platform->error_message_buffer;
    win32_platform->instance = GetModuleHandleA( nullptr );

    // create window
    WNDCLASSEX windowClass    = {};
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc   = win32_winproc;
    windowClass.hInstance     = win32_platform->instance;
    windowClass.lpszClassName = L"LiquidEngineWindowClass";
    windowClass.hbrBackground = (HBRUSH)GetStockBrush(BLACK_BRUSH);
    windowClass.hCursor       = LoadCursor(
        win32_platform->instance,
        IDC_ARROW
    );
    // TODO(alicia): window icon!
    windowClass.hIcon = nullptr;

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
        i32 screen_width  = GetSystemMetrics( SM_CXSCREEN );
        i32 screen_height = GetSystemMetrics( SM_CYSCREEN );

        i32 x_center = screen_width / 2;
        i32 y_center = screen_height / 2;

        i32 half_width  = width / 2;
        i32 half_height = height / 2;

        x = x_center - half_width;
        y = y_center - half_height;
    }

    const char* surface_name  = DEFAULT_SURFACE_NAME;
    usize surface_name_length = DEFAULT_SURFACE_NAME_LENGTH;
    if( opt_surface_name ) {
        surface_name        = opt_surface_name;
        surface_name_length = str_length( opt_surface_name );
        surface_name_length = min(
            MAX_WINDOW_TITLE_BUFFER_SIZE,
            surface_name_length
        );
    }

    mbstowcs(
        win32_platform->window_title_buffer,
        surface_name,
        surface_name_length
    );
    
    HWND hWnd = CreateWindowEx(
        dwExStyle,
        windowClass.lpszClassName,
        win32_platform->window_title_buffer,
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
        mem_free( win32_platform );
        return false;
    }
    HDC dc = GetDC( hWnd );
    if( !dc ) {
        win32_log_error( true );
        mem_free( win32_platform );
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
    out_platform->surface.platform   = (void*)&win32_platform->window;
    out_platform->platform  = (void*)win32_platform;
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
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;

    for( u32 i = 0; i < MAX_MODULE_COUNT; ++i ) {
        HMODULE module = win32_platform->modules[i];
        if( !module ) {
            continue;
        }

        win32_library_free( module );
    }

    ERROR_MESSAGE_BUFFER = nullptr;
    DestroyWindow( win32_platform->window.handle );

    mem_free( platform->platform );
}
u64 platform_read_absolute_time( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;

    LARGE_INTEGER counter = {};
    QueryPerformanceCounter( &counter );
    return counter.QuadPart - win32_platform->performance_counter.QuadPart;
}
f64 platform_read_seconds_elapsed( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
    u64 counter = platform_read_absolute_time( platform );
    return (f64)counter /
        (f64)(win32_platform->performance_frequency.QuadPart);
}
b32 platform_pump_events( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
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

    return true;
}
void platform_surface_set_name(
    Platform* platform,
    usize name_length,
    const char* name
) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
    usize max_length = min(
        MAX_WINDOW_TITLE_BUFFER_SIZE,
        name_length
    );
    mbstowcs(
        win32_platform->window_title_buffer,
        name,
        max_length
    );
    SetWindowText(
        win32_platform->window.handle,
        win32_platform->window_title_buffer
    );
}
i32 platform_surface_read_name(
    Platform* platform,
    char* buffer, usize max_buffer_size
) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
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
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
    win32_platform->cursor.style = cursor_style;

    LPCTSTR win32_style = cursor_style_to_win32_style( cursor_style );
    SetCursor(
        LoadCursor( nullptr, win32_style )
    );

    Event event = {};
    event.code = EVENT_CODE_MOUSE_CURSOR_STYLE_CHANGED;
    event.data.raw.uint32[0] = cursor_style;
    event_fire( event );
}
void platform_cursor_set_visible( Platform* platform, b32 visible ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
    ShowCursor( visible );
    win32_platform->cursor.is_visible = visible;
}
void platform_cursor_set_locked( Platform* platform, b32 locked ) {
    if( locked ) {
        platform_cursor_center( platform );
        platform_cursor_set_visible( platform, false );
    } else {
        platform_cursor_set_visible( platform, true );
    }
}
void platform_cursor_center( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
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

        DWORD is_active = XInputGetState(
            gamepad_index,
            &gamepad_state
        );
        // if gamepad activated this frame, fire an event
        b32 was_active = input_is_pad_active( gamepad_index );
        if( was_active != is_active && is_active ) {
            event.code = EVENT_CODE_INPUT_GAMEPAD_ACTIVATE;
            event.data.gamepad_activate.gamepad_index = gamepad_index;
            event_fire( event );
        }
        input_set_pad_active( gamepad_index, is_active );

        if( is_active != ERROR_SUCCESS ) {
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
// struct VkSurfaceKHR_T* platform_vk_create_surface(
//     Platform* platform,
//     struct VulkanRendererContext* ctx
// ) {
//     Win32Platform* win32_platform = (Win32Platform*)platform->platform;
    
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
// usize platform_vk_read_ext_names(
//     Platform*,
//     usize max_names,
//     usize* name_count,
//     const char** names
// ) {
//     usize win32_ext_count = STATIC_ARRAY_COUNT( WIN32_VULKAN_EXTENSIONS );
//     usize max_count = win32_ext_count > max_names ?
//         max_names : win32_ext_count;
    
//     usize count = *name_count;
//     for( usize i = 0; i < max_count; ++i ) {
//         names[count++] = WIN32_VULKAN_EXTENSIONS[i];
//         win32_ext_count--;
//     }

//     *name_count = count;
//     return win32_ext_count;
// }


void platform_gl_swap_buffers( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
#if defined(DEBUG)
    if( !SwapBuffers( win32_platform->window.device_context ) ) {
        win32_log_error( true );
    }
#else
    SwapBuffers( win32_platform->window.device_context );
#endif
}
internal HGLRC win32_gl_create_context( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;

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
        HMODULE lib_gl = GetModuleHandle( L"OPENGL32.DLL" );
        LOG_ASSERT( lib_gl, "OpenGL module was not loaded!" );
        function = (void*)GetProcAddress(
            lib_gl,
            function_name
        );

#if defined(LD_LOGGING)
        if( !function ) {
            WIN32_LOG_WARN(
                "Failed to load GL function \"%s\"!",
                function_name
            );
        }
#endif
    }

    return function;
}
void* platform_gl_init( Platform* platform ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;

    if( !win32_load_opengl( win32_platform ) ) {
        return nullptr;
    }

    HGLRC gl_context = win32_gl_create_context( platform );
    if( !gl_context ) {
        return nullptr;
    }

    if( !gladLoadGLLoader( win32_gl_load_proc ) ) {
        GL_LOG_FATAL( "Failed to load OpenGL functions!" );
        return nullptr;
    }

    return (void*)gl_context;
}
void platform_gl_shutdown( Platform* platform, void* glrc ) {
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;
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

    [[maybe_unused]]
    Win32Platform* win32_platform = (Win32Platform*)platform->platform;

    Event event = {};
    switch( Msg ) {
        case WM_DESTROY: {
            event.code = EVENT_CODE_SURFACE_DESTROY;
            event_fire( event );
        } break;

        case WM_ACTIVATE: {
            b32 is_active = wParam == WA_ACTIVE ||
                wParam == WA_CLICKACTIVE;

            XInputEnable( (BOOL)is_active );
            event.code = EVENT_CODE_SURFACE_ACTIVE;
            event.data.surface_active.is_active = is_active;
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
                event.code = EVENT_CODE_SURFACE_RESIZE;
                event.data.surface_resize.dimensions = dimensions;
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

            event.code = EVENT_CODE_INPUT_KEY;
            event.data.keyboard.code    = (KeyCode)keycode;
            event.data.keyboard.is_down = is_down;
            event_fire( event );

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
            event.code = EVENT_CODE_INPUT_MOUSE_BUTTON;
            event.data.mouse_button.code    = code;
            event.data.mouse_button.is_down = is_down;
            event_fire( event );

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
            event.code = EVENT_CODE_INPUT_MOUSE_BUTTON;
            event.data.mouse_button.code    = code;
            event.data.mouse_button.is_down = is_down;
            event_fire( event );

        } return TRUE;

        case WM_MOUSEHWHEEL:
        case WM_MOUSEWHEEL: {

            if( !platform->is_active ) {
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

b32 win32_load_user32( HMODULE* out_module ) {
    HMODULE lib_user32 = nullptr;

    if( !win32_library_load(
        L"USER32.DLL",
        &lib_user32
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load user32.dll!"
        );
        return false;
    }

    SetProcessDpiAwarenessContext =
    (::impl::SetProcessDpiAwarenessContextFN)win32_proc_address_required(
        lib_user32,
        "SetProcessDpiAwarenessContext"
    );
    if( !SetProcessDpiAwarenessContext ) {
        return false;
    }

    GetDpiForSystem =
    (::impl::GetDpiForSystemFN)win32_proc_address_required(
        lib_user32,
        "GetDpiForSystem"
    );
    if( !GetDpiForSystem ) {
        return false;
    }

    AdjustWindowRectExForDpi =
    (::impl::AdjustWindowRectExForDpiFN)win32_proc_address_required(
        lib_user32,
        "AdjustWindowRectExForDpi"
    );
    if( !AdjustWindowRectExForDpi ) {
        return false;
    }

    *out_module = lib_user32;
    return true;
}
b32 win32_load_xinput( HMODULE* out_module ) {
    HMODULE lib_xinput = nullptr;

    if( !win32_library_load(
        L"XINPUT1_4.DLL",
        &lib_xinput
    ) ) {
        if( !win32_library_load(
            L"XINPUT9_1_0.DLL",
            &lib_xinput
        ) ) {
            if( !win32_library_load(
                L"XINPUT1_3.DLL",
                &lib_xinput
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
    (::impl::XInputGetStateFN)win32_proc_address_required(
        lib_xinput,
        "XInputGetState"
    );
    if( !XInputGetState ) {
        return false;
    }
    XInputSetState =
    (::impl::XInputSetStateFN)win32_proc_address_required(
        lib_xinput,
        "XInputSetState"
    );
    if( !XInputSetState ) {
        return false;
    }
    ::impl::XInputEnableFN xinput_enable =
    (::impl::XInputEnableFN)win32_proc_address(
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

    if( !win32_library_load(
        L"OPENGL32.DLL",
        &lib_gl
    ) ) {
        MESSAGE_BOX_FATAL(
            "Failed to load library!",
            "Failed to load opengl32.dll!"
        );
        return false;
    }

    wglCreateContext =
    (::impl::wglCreateContextFN)win32_proc_address_required(
        lib_gl,
        "wglCreateContext"
    );
    if( !wglCreateContext ) {
        return false;
    }
    wglMakeCurrent =
    (::impl::wglMakeCurrentFN)win32_proc_address_required(
        lib_gl,
        "wglMakeCurrent"
    );
    if( !wglMakeCurrent ) {
        return false;
    }
    wglDeleteContext =
    (::impl::wglDeleteContextFN)win32_proc_address_required(
        lib_gl,
        "wglDeleteContext"
    );
    if( !wglDeleteContext ) {
        return false;
    }
    wglGetProcAddress =
    (::impl::wglGetProcAddressFN)win32_proc_address_required(
        lib_gl,
        "wglGetProcAddress"
    );
    if( !wglGetProcAddress ) {
        return false;
    }

    DescribePixelFormat =
    (::impl::DescribePixelFormatFN)win32_proc_address_required(
        platform->lib_gdi32,
        "DescribePixelFormat"
    );
    if( !DescribePixelFormat ) {
        return false;
    }
    ChoosePixelFormat =
    (::impl::ChoosePixelFormatFN)win32_proc_address_required(
        platform->lib_gdi32,
        "ChoosePixelFormat"
    );
    if( !ChoosePixelFormat ) {
        return false;
    }
    SetPixelFormat =
    (::impl::SetPixelFormatFN)win32_proc_address_required(
        platform->lib_gdi32,
        "SetPixelFormat"
    );
    if( !SetPixelFormat ) {
        return false;
    }

    SwapBuffers =
    (::impl::SwapBuffersFN)win32_proc_address_required(
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
    internal b32 _win32_library_load(
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

    internal b32 _win32_library_load_trace(
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

    internal void _win32_library_free( HMODULE module ) {
        FreeLibrary( module );
    }

    internal void _win32_library_free_trace(
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

} // namespace impl

internal void* win32_proc_address(
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
        WIN32_LOG_NOTE(
            "Function \"%s\" loaded from library \"%ls\".",
            proc_name,
            module_name_buffer
        );
    } else {
        WIN32_LOG_WARN(
            "Failed to load function \"%s\" from library \"%ls\"!",
            proc_name,
            module_name_buffer
        );
    }
#endif

    return result;
}

internal void* win32_proc_address_required(
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
        WIN32_LOG_NOTE(
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
        WIN32_LOG_ERROR("%s", message_buffer);
        MESSAGE_BOX_FATAL(
            "Failed to load function.",
            message_buffer
        );
    }

    return result;
}

DWORD win32_log_error( b32 present_message_box ) {
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

    if( message_buffer ) {
        if( message_buffer_size > 0 ) {
            WIN32_LOG_ERROR(
                "%u: %ls",
                error_code,
                message_buffer
            );

            if( present_message_box ) {
                if( ERROR_MESSAGE_BUFFER ) {
                    snprintf(
                        ERROR_MESSAGE_BUFFER,
                        ERROR_MESSAGE_BUFFER_SIZE,
                        "Encountered a fatal Windows error!\n"
                        LD_CONTACT_MESSAGE "\n"
                        "%ls",
                        message_buffer
                    );

                    MESSAGE_BOX_FATAL(
                        "Fatal Windows Error",
                        ERROR_MESSAGE_BUFFER
                    );
                } else {
                    WIN32_LOG_WARN(
                        "Attempted to present error message box while "
                        "message buffer is null!"
                    );
                }
            }
        }

        LocalFree( message_buffer );
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

void* page_alloc( usize size ) {
    void* pointer = (void*)VirtualAlloc(
        nullptr,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    return pointer;
}
void page_free( void* memory ) {
    VirtualFree(
        memory,
        0,
        MEM_RELEASE | MEM_DECOMMIT
    );
}

// struct Win32ThreadHandle {
//     HANDLE     handle;
//     ThreadProc proc;
//     void*      params;
//     DWORD      id;
// };


// // MULTI-THREADING | BEGIN ------------------------------------------------

// internal DWORD WINAPI win32_thread_proc( void* params ) {
//     Win32ThreadHandle* thread_handle = (Win32ThreadHandle*)params;

//     DWORD return_value = thread_handle->proc(
//         thread_handle->params
//     );

//     mem_free( params );

//     return return_value;
// }

// #define THREAD_STACK_SIZE_SAME_AS_MAIN 0
// #define THREAD_RUN_ON_CREATE 0
// ThreadHandle thread_create(
//     OldPlatformState* state,
//     ThreadProc thread_proc,
//     void*      params,
//     b32        run_on_creation
// ) {

//     Win32State* win32_state = (Win32State*)state->platform_data;

//     Win32ThreadHandle* thread_handle = get_next_handle( win32_state );
//     if( !thread_handle ) {
//         LOG_ERROR("Out of thread handles!");
//         return nullptr;
//     }
//     thread_handle->proc   = thread_proc;
//     thread_handle->params = params;

//     // we don't care about this
//     LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;

//     SIZE_T dwStackSize     = THREAD_STACK_SIZE_SAME_AS_MAIN;
//     DWORD  dwCreationFlags = CREATE_SUSPENDED;

//     mem_fence();

//     thread_handle->handle = CreateThread(
//         lpThreadAttributes,
//         dwStackSize,
//         win32_thread_proc,
//         thread_handle,
//         dwCreationFlags,
//         &thread_handle->id
//     );

//     if( !thread_handle->handle ) {
//         win32_log_error( true );
//         return nullptr;
//     }

//     if( run_on_creation ) {
//         thread_resume( thread_handle );
//     }

//     return thread_handle;
// }
// void thread_resume( ThreadHandle thread ) {
//     Win32ThreadHandle* win32_thread = (Win32ThreadHandle*)thread;
//     ResumeThread( win32_thread->handle );
// }

// Semaphore semaphore_create(
//     u32 initial_count,
//     u32 maximum_count
// ) {
//     // we don't care about these
//     LPSECURITY_ATTRIBUTES security_attributes = nullptr;
//     LPCWSTR name = nullptr;
//     DWORD flags = 0;

//     DWORD desired_access = SEMAPHORE_ALL_ACCESS;

//     HANDLE semaphore_handle = CreateSemaphoreExW(
//         security_attributes,
//         initial_count,
//         maximum_count,
//         name,
//         flags,
//         desired_access
//     );

//     return (Semaphore*)semaphore_handle;
// }
// void semaphore_increment(
//     Semaphore semaphore,
//     u32       increment,
//     u32*      opt_out_previous_count
// ) {
//     HANDLE win32_handle = (HANDLE)semaphore;
//     ReleaseSemaphore(
//         win32_handle,
//         increment,
//         (LONG*)opt_out_previous_count
//     );
// }
// void semaphore_wait_for(
//     Semaphore semaphore,
//     u32       timeout_ms
// ) {
//     HANDLE win32_handle = (HANDLE)semaphore;
//     WaitForSingleObjectEx(
//         win32_handle,
//         timeout_ms,
//         FALSE
//     );
// }
// void semaphore_wait_for_multiple(
//     usize      count,
//     Semaphore* semaphores,
//     b32        wait_for_all,
//     u32        timeout_ms
// ) {
//     const HANDLE* win32_handles = (const HANDLE*)semaphores;
//     WaitForMultipleObjects(
//         count,
//         win32_handles,
//         wait_for_all ? TRUE : FALSE,
//         timeout_ms
//     );
// }
// void semaphore_destroy( Semaphore semaphore ) {
//     HANDLE win32_handle = (HANDLE)semaphore;
//     CloseHandle( win32_handle );
// }

// u32 interlocked_increment( volatile u32* addend ) {
//     return InterlockedIncrement( addend );
// }
// u32 interlocked_decrement( volatile u32* addend ) {
//     return InterlockedDecrement( addend );
// }
// u32 interlocked_exchange( volatile u32* target, u32 value ) {
//     return InterlockedExchange( target, value );
// }
// void* interlocked_compare_exchange_pointer(
//     void* volatile* dst,
//     void* exchange,
//     void* comperand
// ) {
//     return InterlockedCompareExchangePointer(
//         dst,
//         exchange,
//         comperand
//     );
// }
// u32 interlocked_compare_exchange(
//     u32 volatile* dst,
//     u32 exchange,
//     u32 comperand
// ) {
//     return InterlockedCompareExchange(
//         dst,
//         exchange,
//         comperand
//     );
// }

// void mem_fence() {
//     _ReadWriteBarrier();
// #if defined(SM_ARCH_X86)
//     _mm_mfence();
// #elif
//     #error "mem_fence: Platform is not supported!"
// #endif
// }
// void read_fence() {
//     _ReadBarrier();
// #if defined(SM_ARCH_X86)
//     _mm_lfence();
// #elif
//     #error "read_fence: Platform is not supported!"
// #endif
// }
// void write_fence() {
//     _WriteBarrier();
// #if defined(SM_ARCH_X86)
//     _mm_sfence();
// #elif
//     #error "write_fence: Platform is not supported!"
// #endif
// }

// // MULTI-THREADING | END   ------------------------------------------------

#endif // SM_PLATFORM_WINDOWS
