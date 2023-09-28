#if !defined(LD_PLATFORM_H)
#define LD_PLATFORM_H
// * Description:  Liquid Engine Platform Layer
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 21, 2023
#include "defines.h"

#if defined(LD_PLATFORM_INTERNAL) && !defined(LIQUID_ENGINE_CORE_LIBRARY_PATH)
    #error "LIQUID_ENGINE_CORE_LIBRARY_PATH must be defined!"
#endif

// NOTE(alicia): Types

struct PlatformAPI;

/// Engine core init function prototype.
typedef int CoreInitFN( int argc, char** argv, struct PlatformAPI* platform );
/// Thread procedure prototype.
typedef b32 ThreadProcFN( void* user_params );

/// Handle to surface.
typedef void PlatformSurface;

/// This keycode corresponds to Liquid Engine core keycodes.
typedef u8 PlatformKeyboardCode;
/// This mouse code corresponds to Liquid Engine core mouse codes.
typedef enum : u8 {
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
    PlatformSurfaceCallbackOnMouseWheelFN* on_mouse_wheel;
    void* on_mouse_wheel_params;
} PlatformSurfaceCallbacks;

/// Handle to library.
typedef void PlatformLibrary;
/// Handle to file.
typedef void PlatformFile;
/// Handle to semaphore.
typedef void PlatformSemaphore;
/// Handle to mutex.
typedef void PlatformMutex;

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
    PLATFORM_SURFACE_GRPAHICS_BACKEND_METAL
};
typedef u32 PlatformSurfaceGraphicsBackend;

enum : u32 {
    PLATFORM_SURFACE_WINDOWED,
    PLATFORM_SURFACE_FULLSCREEN
};
typedef u32 PlatformSurfaceMode;

typedef u32 PlatformFileFlags;
#define PLATFORM_FILE_READ          (1 << 0)
#define PLATFORM_FILE_WRITE         (1 << 1)
#define PLATFORM_FILE_SHARE_READ    (1 << 2) 
#define PLATFORM_FILE_SHARE_WRITE   (1 << 3)
#define PLATFORM_FILE_ONLY_EXISTING (1 << 4)

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

// NOTE(alicia): Time API

typedef f64 PlatformElapsedMillisecondsFN(void);
typedef f64 PlatformSecondsElapsedFN(void);
typedef void PlatformSleepMillisecondsFN( u32 ms );

typedef struct {
    PlatformElapsedMillisecondsFN* elapsed_milliseconds;
    PlatformSecondsElapsedFN*      elapsed_seconds;
    PlatformSleepMillisecondsFN*   sleep_ms;
} PlatformTimeAPI;

// NOTE(alicia): IO API

typedef void PlatformIOReadGamepadsFN( PlatformGamepad gamepads[4] );
typedef void PlatformIOSetGamepadRumbleFN(
    u32 gamepad_index, u16 normalized_motor_left, u16 normalized_motor_right );
typedef PlatformFile* PlatformIOGetStdOutFN(void);
typedef PlatformFile* PlatformIOGetStdErrFN(void);
typedef void PlatformConsoleWriteFN(
    PlatformFile* console, usize buffer_size, const char* buffer );
typedef PlatformFile* PlatformFileOpenFN(
    const char* path, PlatformFileFlags flags );
typedef void PlatformFileCloseFN( PlatformFile* file );
typedef b32 PlatformFileReadFN(
    PlatformFile* file, usize read_size, usize buffer_size, void* buffer );
typedef b32 PlatformFileWriteFN(
    PlatformFile* file, usize buffer_size, void* buffer );
typedef usize PlatformFileQuerySizeFN( PlatformFile* file );
typedef void PlatformFileSetOffsetFN( PlatformFile* file, usize offset );
typedef usize PlatformFileQueryOffsetFN( PlatformFile* file );

#if defined(LD_PLATFORM_WINDOWS)
typedef void PlatformWin32OutputDebugStringFN( const char* cstr );
#endif

typedef struct {
    PlatformIOReadGamepadsFN*     read_gamepads;
    PlatformIOSetGamepadRumbleFN* set_gamepad_rumble;
    PlatformIOGetStdOutFN*        stdout_handle;
    PlatformIOGetStdErrFN*        stderr_handle;
    PlatformConsoleWriteFN*       console_write;
    PlatformFileOpenFN*           file_open;
    PlatformFileCloseFN*          file_close;
    PlatformFileReadFN*           file_read;
    PlatformFileWriteFN*          file_write;
    PlatformFileQuerySizeFN*      file_query_size;
    PlatformFileSetOffsetFN*      file_set_offset;
    PlatformFileQueryOffsetFN*    file_query_offset;

#if defined(LD_PLATFORM_WINDOWS)
    PlatformWin32OutputDebugStringFN* output_debug_string;
#endif

} PlatformIOAPI;

// NOTE(alicia): Library API

typedef PlatformLibrary* PlatformLibraryOpenFN( const char* library_path );
typedef void PlatformLibraryCloseFN( PlatformLibrary* library );
typedef void* PlatformLibraryLoadFunctionFN(
    PlatformLibrary* library, const char* function_name );

typedef struct {
    PlatformLibraryOpenFN*         open;
    PlatformLibraryCloseFN*        close;
    PlatformLibraryLoadFunctionFN* load_function;
} PlatformLibraryAPI;

// NOTE(alicia): Threading API

typedef b32 PlatformThreadCreateFN(
    ThreadProcFN* thread_proc, void* params, usize stack_size );
typedef PlatformSemaphore* PlatformSemaphoreCreateFN(
    const char* name, u32 initial_count );
typedef void PlatformSemaphoreDestroyFN( PlatformSemaphore* semaphore );
typedef void PlatformSemaphoreSignalFN( PlatformSemaphore* semaphore );
typedef void PlatformSemaphoreWaitFN( PlatformSemaphore* semaphore );
typedef void PlatformSemaphoreWaitTimedFN(
    PlatformSemaphore* semaphore, u32 timeout_ms );
typedef PlatformMutex* PlatformMutexCreateFN(void);
typedef void PlatformMutexDestroyFN( PlatformMutex* mutex );
typedef void PlatformMutexLockFN( PlatformMutex* mutex );
typedef void PlatformMutexUnlockFN( PlatformMutex* mutex );

typedef struct {
    PlatformThreadCreateFN*       create;
    PlatformSemaphoreCreateFN*    semaphore_create;
    PlatformSemaphoreDestroyFN*   semaphore_destroy;
    PlatformSemaphoreSignalFN*    semaphore_signal;
    PlatformSemaphoreWaitFN*      semaphore_wait;
    PlatformSemaphoreWaitTimedFN* semaphore_wait_timed;
    PlatformMutexCreateFN*        mutex_create;
    PlatformMutexDestroyFN*       mutex_destroy;
    PlatformMutexLockFN*          mutex_lock;
    PlatformMutexUnlockFN*        mutex_unlock;
} PlatformThreadAPI;

// NOTE(alicia): Memory API

typedef void* PlatformHeapAllocFN( usize size );
typedef void* PlatformHeapReallocFN(
    void* memory, usize old_size, usize new_size );
typedef void PlatformHeapFreeFN( usize size, void* memory );
typedef void* PlatformPageAllocFN( usize size );
typedef void PlatformPageFreeFN( usize size, void* memory );

typedef struct {
    PlatformHeapAllocFN*   heap_alloc;
    PlatformHeapReallocFN* heap_realloc;
    PlatformHeapFreeFN*    heap_free;
    PlatformPageAllocFN*   page_alloc;
    PlatformPageFreeFN*    page_free;
} PlatformMemoryAPI;

// NOTE(alicia): Misc

typedef PlatformInfo* PlatformQueryInfoFN(void);
typedef void* PlatformGLLoadProcFN(const char* function_name);
typedef void PlatformFatalMessageBoxFN(
    const char* title, const char* message );
typedef void PlatformLastErrorFN( usize* out_error_len, const char** out_error );

typedef struct PlatformAPI {
    PlatformSurfaceAPI   surface;
    PlatformTimeAPI      time;
    PlatformIOAPI        io;
    PlatformLibraryAPI   library;
    PlatformThreadAPI    thread;
    PlatformMemoryAPI    memory;

    PlatformQueryInfoFN*       query_info;
    PlatformGLLoadProcFN*      gl_load_proc;
    PlatformFatalMessageBoxFN* fatal_message_box;
    PlatformLastErrorFN*       last_error;
} PlatformAPI;

#endif // header guard
