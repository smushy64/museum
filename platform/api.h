#if !defined(LD_PLATFORM_H)
#define LD_PLATFORM_H
/**
 * Description:  Liquid Engine Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 21, 2023
*/
#include "shared/defines.h"

#if defined(LD_PLATFORM_INTERNAL) && !defined(LIQUID_ENGINE_PATH)
    #if !defined(LD_CORE_STATIC_BUILD)
        #error "LIQUID_ENGINE_PATH must be defined!"
    #endif
#endif

// NOTE(alicia): Types

struct PlatformAPI;

#if defined(LD_CORE_STATIC_BUILD)
    /// Engine core init function prototype.
    c_linkage int core_init( int argc, char** argv, struct PlatformAPI* platform );
#else
    /// Engine core init function prototype.
    typedef int CoreInitFN( int argc, char** argv, struct PlatformAPI* platform );
#endif

/// Handle to surface.
typedef void PlatformSurface;

/// Handle to platform audio context.
typedef void PlatformAudioContext;

typedef struct PlatformAudioBufferFormat {
    u8    number_of_channels;
    u8    bits_per_sample;
    u8    bytes_per_sample;
    u32   samples_per_second;
    u32   buffer_sample_count;
    usize buffer_size;
} PlatformAudioBufferFormat;

#if !defined(LD_HEADLESS)
/// This keycode corresponds to Liquid Engine core keycodes.
typedef u8 PlatformKeyboardCode;
/// Mouse codes.
typedef enum PlatformMouseCode : u8 {
    PLATFORM_MOUSE_BUTTON_LEFT,
    PLATFORM_MOUSE_BUTTON_MIDDLE,
    PLATFORM_MOUSE_BUTTON_RIGHT,
    PLATFORM_MOUSE_BUTTON_EXTRA_1,
    PLATFORM_MOUSE_BUTTON_EXTRA_2
} PlatformMouseCode;

typedef void PlatformSurfaceCallbackOnResolutionChangeFN(
    PlatformSurface* surface,
    i32 old_width, i32 old_height,
    i32 new_width, i32 new_height, void* user_params );
typedef void PlatformSurfaceCallbackOnCloseFN(
    PlatformSurface* surface, void* user_params );
typedef void PlatformSurfaceCallbackOnActivateFN(
    PlatformSurface* surface, b32 is_active, void* user_params );
typedef void PlatformSurfaceCallbackOnKeyFN(
    PlatformSurface* surface, b32 is_down,
    PlatformKeyboardCode keycode, void* user_params );
typedef void PlatformSurfaceCallbackOnMouseButtonFN(
    PlatformSurface* surface, b32 is_down,
    PlatformMouseCode mousecode, void* user_params );
typedef void PlatformSurfaceCallbackOnMouseMoveFN(
    PlatformSurface* surface, i32 x, i32 y, void* user_params );
typedef void PlatformSurfaceCallbackOnMouseMoveRelativeFN(
    PlatformSurface* surface, i32 relative_x, i32 relative_y, void* user_params );
typedef void PlatformSurfaceCallbackOnMouseWheelFN(
    PlatformSurface* surface, b32 is_horizontal, i32 value, void* user_params );

typedef struct {
    PlatformSurfaceCallbackOnResolutionChangeFN* on_resolution_change;
    void* on_resolution_change_params;
    PlatformSurfaceCallbackOnCloseFN* on_close;
    void* on_close_params;
    PlatformSurfaceCallbackOnActivateFN* on_activate;
    void* on_activate_params;
    PlatformSurfaceCallbackOnKeyFN* on_key;
    void* on_key_params;
    PlatformSurfaceCallbackOnMouseButtonFN* on_mouse_button;
    void* on_mouse_button_params;
    PlatformSurfaceCallbackOnMouseMoveFN* on_mouse_move;
    void* on_mouse_move_params;
    PlatformSurfaceCallbackOnMouseMoveRelativeFN* on_mouse_move_relative;
    void* on_mouse_move_relative_params;
    PlatformSurfaceCallbackOnMouseWheelFN* on_mouse_wheel;
    void* on_mouse_wheel_params;
} PlatformSurfaceCallbacks;

#endif // if not headless

#if !defined(LD_HEADLESS)

typedef u16 PlatformGamepadButtons;
#define PLATFORM_GAMEPAD_BUTTON_DPAD_UP           (1 << 0)
#define PLATFORM_GAMEPAD_BUTTON_DPAD_DOWN         (1 << 1)
#define PLATFORM_GAMEPAD_BUTTON_DPAD_LEFT         (1 << 2)
#define PLATFORM_GAMEPAD_BUTTON_DPAD_RIGHT        (1 << 3)
#define PLATFORM_GAMEPAD_BUTTON_START             (1 << 4)
#define PLATFORM_GAMEPAD_BUTTON_SELECT            (1 << 5)
#define PLATFORM_GAMEPAD_BUTTON_STICK_LEFT_CLICK  (1 << 6)
#define PLATFORM_GAMEPAD_BUTTON_STICK_RIGHT_CLICK (1 << 7)
#define PLATFORM_GAMEPAD_BUTTON_BUMPER_LEFT       (1 << 8)
#define PLATFORM_GAMEPAD_BUTTON_BUMPER_RIGHT      (1 << 9)
#define PLATFORM_GAMEPAD_EXT_BUTTON_TRIGGER_LEFT  (1 << 10)
#define PLATFORM_GAMEPAD_EXT_BUTTON_TRIGGER_RIGHT (1 << 11)
#define PLATFORM_GAMEPAD_BUTTON_FACE_DOWN         (1 << 12)
#define PLATFORM_GAMEPAD_BUTTON_FACE_RIGHT        (1 << 13)
#define PLATFORM_GAMEPAD_BUTTON_FACE_LEFT         (1 << 14)
#define PLATFORM_GAMEPAD_BUTTON_FACE_UP           (1 << 15)

typedef struct {
    PlatformGamepadButtons buttons;
    b8 is_active;
    union {
        struct { u16 trigger_left_normalized, trigger_right_normalized; };
        u32 triggers;
    };
    union {
        struct { i16 stick_left_x_normalized, stick_left_y_normalized; };
        u32 stick_left;
    };
    union {
        struct { i16 stick_right_x_normalized, stick_right_y_normalized; };
        u32 stick_right;
    };
} PlatformGamepad;

enum : u32 {
    PLATFORM_SURFACE_GRAPHICS_BACKEND_OPENGL,
    PLATFORM_SURFACE_GRAPHICS_BACKEND_VULKAN,
    PLATFORM_SURFACE_GRAPHICS_BACKEND_DIRECTX11,
    PLATFORM_SURFACE_GRAPHICS_BACKEND_DIRECTX12,
    PLATFORM_SURFACE_GRAPHICS_BACKEND_WEBGL,
    PLATFORM_SURFACE_GRAPHICS_BACKEND_METAL
};
typedef u32 PlatformSurfaceGraphicsBackend;

enum : u32 {
    PLATFORM_SURFACE_WINDOWED,
    PLATFORM_SURFACE_FULLSCREEN
};
typedef u32 PlatformSurfaceMode;

#endif // if not headless

typedef u16 PlatformProcessorFeatures;
#define PLATFORM_PROCESSOR_FEATURE_SSE     (1 << 0)
#define PLATFORM_PROCESSOR_FEATURE_SSE2    (1 << 1)
#define PLATFORM_PROCESSOR_FEATURE_SSE3    (1 << 2)
#define PLATFORM_PROCESSOR_FEATURE_SSSE3   (1 << 3)
#define PLATFORM_PROCESSOR_FEATURE_SSE4_1  (1 << 4)
#define PLATFORM_PROCESSOR_FEATURE_SSE4_2  (1 << 5)
#define PLATFORM_PROCESSOR_FEATURE_AVX     (1 << 6)
#define PLATFORM_PROCESSOR_FEATURE_AVX2    (1 << 7)
#define PLATFORM_PROCESSOR_FEATURE_AVX_512 (1 << 8)

#define PLATFORM_CPU_NAME_BUFFER_SIZE (255)
typedef struct {
    char                      cpu_name[PLATFORM_CPU_NAME_BUFFER_SIZE];
    usize                     total_memory;
    usize                     page_size;
    u16                       logical_processor_count;
    PlatformProcessorFeatures features;
} PlatformInfo;

#if !defined(LD_HEADLESS)

// NOTE(alicia): Surface API

typedef PlatformSurface* PlatformSurfaceCreateFN(
    i32 width, i32 height, const char* name,
    b32 create_hidden, b32 resizeable,
    PlatformSurfaceGraphicsBackend backend );
typedef void PlatformSurfaceDestroyFN( PlatformSurface* surface );
typedef void PlatformSurfaceSetCallbacksFN(
    PlatformSurface* surface, PlatformSurfaceCallbacks* callbacks );
typedef void PlatformSurfaceClearCallbacksFN( PlatformSurface* surface );
typedef void PlatformSurfaceSetVisibleFN(
    PlatformSurface* surface, b32 is_visible );
typedef b32 PlatformSurfaceQueryVisibilityFN( PlatformSurface* surface );
typedef void PlatformSurfaceSetDimensionsFN(
    PlatformSurface* surface, i32 width, i32 height );
typedef void PlatformSurfaceQueryDimensionsFN(
    PlatformSurface* surface, i32* out_width, i32* out_height );
typedef void PlatformSurfaceSetModeFN(
    PlatformSurface* surface, PlatformSurfaceMode mode );
typedef PlatformSurfaceMode PlatformSurfaceQueryModeFN(
    PlatformSurface* surface );
typedef void PlatformSurfaceSetNameFN(
    PlatformSurface* surface, const char* name );
typedef void PlatformSurfaceQueryNameFN(
    PlatformSurface* surface, usize* buffer_size, char* buffer );
typedef void PlatformSurfaceCenterFN( PlatformSurface* surface );
typedef void PlatformSurfaceCenterCursorFN( PlatformSurface* surface );
typedef b32  PlatformSurfaceGLInitFN( PlatformSurface* surface );
typedef void PlatformSurfaceGLSwapBuffersFN( PlatformSurface* surface );
typedef void PlatformSurfaceGLSwapIntervalFN(
    PlatformSurface* surface, int interval );
typedef void PlatformSurfacePumpEventsFN(void);

typedef struct {
    PlatformSurfaceCreateFN*          create;
    PlatformSurfaceDestroyFN*         destroy;
    PlatformSurfaceSetCallbacksFN*    set_callbacks;
    PlatformSurfaceClearCallbacksFN*  clear_callbacks;
    PlatformSurfaceSetVisibleFN*      set_visible;
    PlatformSurfaceQueryVisibilityFN* query_visibility;
    PlatformSurfaceSetDimensionsFN*   set_dimensions;
    PlatformSurfaceQueryDimensionsFN* query_dimensions;
    PlatformSurfaceSetModeFN*         set_mode;
    PlatformSurfaceQueryModeFN*       query_mode;
    PlatformSurfaceSetNameFN*         set_name;
    PlatformSurfaceQueryNameFN*       query_name;
    PlatformSurfaceCenterFN*          center;
    PlatformSurfaceCenterCursorFN*    center_cursor;
    PlatformSurfaceGLInitFN*          gl_init;
    PlatformSurfaceGLSwapBuffersFN*   gl_swap_buffers;
    PlatformSurfaceGLSwapIntervalFN*  gl_swap_interval;
    PlatformSurfacePumpEventsFN*      pump_events;
} PlatformSurfaceAPI;

// NOTE(alicia): Audio API

typedef PlatformAudioContext* PlatformAudioInitializeFN( u64 buffer_length_ms );
typedef void PlatformAudioShutdownFN( PlatformAudioContext* ctx );
typedef PlatformAudioBufferFormat
    PlatformAudioQueryBufferFormatFN( PlatformAudioContext* ctx );
typedef b32 PlatformAudioLockBufferFN(
    PlatformAudioContext* ctx, usize* out_sample_count,
    usize* out_buffer_size, void** out_buffer );
typedef void PlatformAudioUnlockBufferFN(
    PlatformAudioContext* ctx, usize sample_count );
typedef void PlatformAudioStartFN( PlatformAudioContext* ctx );
typedef void PlatformAudioStopFN( PlatformAudioContext* ctx );

typedef struct {
    PlatformAudioInitializeFN*        initialize;
    PlatformAudioShutdownFN*          shutdown;
    PlatformAudioQueryBufferFormatFN* query_buffer_format;
    PlatformAudioLockBufferFN*        lock_buffer;
    PlatformAudioUnlockBufferFN*      unlock_buffer;
    PlatformAudioStartFN*             start;
    PlatformAudioStopFN*              stop;
} PlatformAudioAPI;

#endif /* if not headless */

// NOTE(alicia): IO API

#if !defined(LD_HEADLESS)

typedef void PlatformIOReadGamepadsFN( PlatformGamepad gamepads[4] );
typedef void PlatformIOSetGamepadRumbleFN(
    u32 gamepad_index, u16 normalized_motor_left, u16 normalized_motor_right );
typedef void PlatformIOSetMouseVisibleFN( b32 is_visible );

typedef struct {
    PlatformIOReadGamepadsFN*     read_gamepads;
    PlatformIOSetGamepadRumbleFN* set_gamepad_rumble;
    PlatformIOSetMouseVisibleFN*  set_mouse_visible;
} PlatformIOAPI;

#endif

// NOTE(alicia): Misc

typedef PlatformInfo* PlatformQueryInfoFN(void);
typedef void PlatformLastErrorFN( usize* out_error_len, const char** out_error );

#if !defined(LD_HEADLESS)
typedef void* PlatformGLLoadProcFN(const char* function_name);
typedef void PlatformFatalMessageBoxFN(
    const char* title, const char* message );
#endif // if not headless

typedef struct PlatformAPI {

#if !defined(LD_HEADLESS)
    PlatformSurfaceAPI         surface;
    PlatformAudioAPI           audio;
    PlatformGLLoadProcFN*      gl_load_proc;
    PlatformFatalMessageBoxFN* fatal_message_box;
    PlatformIOAPI              io;
#endif // if not headless

    PlatformQueryInfoFN* query_info;
    PlatformLastErrorFN* last_error;

} PlatformAPI;

#endif // header guard
