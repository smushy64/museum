#if !defined(PLATFORM_HPP)
#define PLATFORM_HPP
/**
 * Description:  Operating System related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#include "core/ldmath/types.h"
#include "core/ldinput.h"
#include "core/ldengine.h"
#include "core/ldstring.h"
#include "core/ldthread.h"
#include "flags.h"

#define MAX_PLATFORM_SURFACE_TITLE_SIZE 255

/// Platform state
typedef struct Platform {
    union Surface {
        ivec2  dimensions;
        struct { i32 width, height; };
    } surface;
    b32 is_active;
} Platform;
/// Returns how many bytes the current platform requires.
u32 query_platform_subsystem_size();
/// Initialize platform state. Returns true if successful.
b32 platform_init(
    ivec2 surface_dimensions,
    PlatformFlags flags,
    Platform* out_platform
);
/// Shutdown platform subsystem.
void platform_shutdown( Platform* platform );
/// Get microseconds (μs) elapsed since start of the program.
f64 platform_us_elapsed();
/// Get milliseconds elapsed since start of the program.
f64 platform_ms_elapsed();
/// Get seconds elapsed since start of the program.
f64 platform_s_elapsed();
/// Pump platform events.
b32 platform_pump_events( Platform* platform );
/// Set platform surface name.
/// StringView MUST have a null-terminator
/// Does nothing on platforms that don't use windows.
void platform_surface_set_name(
    Platform* platform,
    StringView name
);
/// Read platform surface name.
/// Returns:
///          >Zero: required buffer size. writes up to buffer size.
///          Zero:  success.
i32 platform_surface_read_name(
    Platform* platform,
    char* buffer, usize max_buffer_size
);
/// Set cursor style.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_set_style( Platform* platform, CursorStyle cursor_style );
/// Set cursor visibility.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_set_visible( Platform* platform, b32 visible );
/// Set cursor position to surface center.
/// Does nothing on platforms that don't use a cursor.
void platform_cursor_center( Platform* platform );
/// Put the current thread to sleep
/// for the specified amount of milliseconds.
void platform_sleep( Platform* platform, u32 ms );
/// Set given gamepad motor state.
void platform_set_pad_motor_state(
    Platform* platform,
    u32 gamepad_index, u32 motor, f32 value
);
/// Poll gamepad.
void platform_poll_gamepad( Platform* platform );
/// Swap buffers. OpenGL only.
void platform_gl_swap_buffers( Platform* platform );
/// Initialize OpenGL.
void* platform_gl_init( Platform* platform );
/// Shutdown OpenGL.
void platform_gl_shutdown( Platform* platform, void* glrc );

/// Initialize Audio backend.
b32 platform_init_audio( Platform* platform );
/// Shutdown Audio backend.
void platform_shutdown_audio( Platform* platform );

void platform_audio_test( Platform* platform, i16 volume );

/// Get stdout handle.
void* platform_stdout_handle();
/// Get stderr handle.
void* platform_stderr_handle();

/// Write a null-terminated string to the console.
/// Get console handle using platform_stdout_handle() or
/// platform_stderr_handle().
void platform_write_console(
    void* output_handle,
    u32 write_count,
    const char* buffer
);

typedef void* PlatformLibraryHandle;

/// Load library from path.
PlatformLibraryHandle platform_library_load( const char* library_path );
/// Free a library handle.
void platform_library_free( PlatformLibraryHandle library );
/// Load a function from a library.
void* platform_library_load_function(
    PlatformLibraryHandle library,
    const char*           function_name
);

#if defined(LD_PLATFORM_WINDOWS) || defined(LD_PLATFORM_LINUX)
    #define FILE_HANDLE_SIZE (sizeof(usize))
#endif

/// Platform file handle
typedef struct PlatformFileHandle {
    u8 platform[FILE_HANDLE_SIZE];
} PlatformFileHandle;

typedef u32 PlatformFileOpenFlags;

#define PLATFORM_FILE_OPEN_READ        ( 1 << 0 )
#define PLATFORM_FILE_OPEN_WRITE       ( 1 << 1 )
#define PLATFORM_FILE_OPEN_SHARE_READ  ( 1 << 2 )
#define PLATFORM_FILE_OPEN_SHARE_WRITE ( 1 << 3 )
#define PLATFORM_FILE_OPEN_EXISTING    ( 1 << 4 )

/// Open file from path.
b32 platform_file_open(
    const char*           path,
    PlatformFileOpenFlags flags,
    PlatformFileHandle*   out_handle
);
/// Close file handle.
void platform_file_close( PlatformFileHandle* handle );
/// Read file into buffer.
b32 platform_file_read(
    PlatformFileHandle* handle,
    usize read_size,
    usize buffer_size,
    void* buffer
);
/// Write buffer into file.
b32 platform_file_write(
    PlatformFileHandle* handle,
    usize write_size,
    usize buffer_size,
    void* buffer
);
/// Query size of file in bytes.
usize platform_file_query_size( PlatformFileHandle* handle );
/// Query where in the file the handle is at.
usize platform_file_query_offset( PlatformFileHandle* handle );
/// Set file offset. Returns true if successful.
b32 platform_file_set_offset( PlatformFileHandle* handle, usize offset );

#if defined(LD_PLATFORM_WINDOWS)
    #define THREAD_HANDLE_SIZE (32)
#elif defined(LD_PLATFORM_LINUX)
    #define THREAD_HANDLE_SIZE (24)
#else
    #define THREAD_HANDLE_SIZE (sizeof(usize))
#endif

/// Opaque handle to a thread.
typedef struct PlatformThreadHandle {
    u8 platform[THREAD_HANDLE_SIZE];
} PlatformThreadHandle;

/// Thread Proc definition.
typedef b32 (*ThreadProcFN)( void* user_params );

/// Create a thread.
b32 platform_thread_create(
    ThreadProcFN thread_proc,
    void*        user_params,
    usize        thread_stack_size,
    b32          create_suspended,
    PlatformThreadHandle* out_thread_handle
);
/// Resume a suspended thread.
void platform_thread_resume( PlatformThreadHandle* thread_handle );
/// Suspend a thread.
void platform_thread_suspend( PlatformThreadHandle* thread_handle );
/// Kill a thread.
void platform_thread_kill( PlatformThreadHandle* thread_handle );

#if defined(LD_PLATFORM_WINDOWS) || defined(LD_PLATFORM_LINUX)
    #define SEMAPHORE_HANDLE_SIZE (sizeof(usize))
#endif

#if defined(LD_PLATFORM_WINDOWS)
    #define MUTEX_HANDLE_SIZE (sizeof(usize))
#elif defined(LD_PLATFORM_LINUX)
    /// sizeof(pthread_mutex_t)
    #define MUTEX_HANDLE_SIZE (40)
#endif

STATIC_ASSERT( SEMAPHORE_HANDLE_SIZE <= MAX_SEMAPHORE_SIZE, "Semaphore handle size must be less than or equals to max size!" );
STATIC_ASSERT( MUTEX_HANDLE_SIZE <= MAX_MUTEX_SIZE, "Mutex handle size must be less than or equals to max size!" );

/// Opaque handle to a semaphore object.
typedef struct PlatformSemaphoreHandle {
    u8 buffer[SEMAPHORE_HANDLE_SIZE];
} PlatformSemaphoreHandle;

/// Opaque handle to a mutex object.
typedef struct PlatformMutexHandle {
    u8 buffer[MUTEX_HANDLE_SIZE];
} PlatformMutexHandle;

/// Create a semaphore.
b32 platform_semaphore_create(
    const char* opt_name, u32 initial_count,
    PlatformSemaphoreHandle* out_semaphore_handle
);
/// Increment a semaphore.
void platform_semaphore_increment(
    PlatformSemaphoreHandle* semaphore_handle
);
/// Wait for semaphore to be incremented.
/// Semaphore is decremented when it is signaled.
/// If infinite timeout is true, timeout ms is ignored.
void platform_semaphore_wait(
    PlatformSemaphoreHandle* semaphore_handle,
    b32 infinite_timeout, u32 opt_timeout_ms
);
/// Destroy a semaphore handle.
void platform_semaphore_destroy(
    PlatformSemaphoreHandle* semaphore_handle
);

/// Create a mutex.
b32 platform_mutex_create( PlatformMutexHandle* out_mutex );
/// Lock a mutex. Stalls until the mutex is available if another
/// thread already has ownership of it.
void platform_mutex_lock( PlatformMutexHandle* mutex );
/// Unlock a mutex.
void platform_mutex_unlock( PlatformMutexHandle* mutex );
/// Destroy a mutex handle.
void platform_mutex_destroy( PlatformMutexHandle* mutex );

/// Multi-Threading safe increment.
u32 platform_interlocked_increment_u32( volatile u32* addend );
/// Multi-Threading safe decrement.
u32 platform_interlocked_decrement_u32( volatile u32* addend );
/// Multi-Threading safe exchange.
u32 platform_interlocked_exchange_u32( volatile u32* target, u32 value );
/// Multi-Threading safe compare and exchange.
u32 platform_interlocked_compare_exchange_u32(
    volatile u32* dst,
    u32 exchange, u32 comperand
);
/// Multi-Threading safe compare and exchange.
void* platform_interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
);

/// Types of message boxes
typedef enum : u32 {
    MESSAGE_BOX_TYPE_OK,
    MESSAGE_BOX_TYPE_OKCANCEL,
    MESSAGE_BOX_TYPE_RETRYCANCEL,
    MESSAGE_BOX_TYPE_YESNO,
    MESSAGE_BOX_TYPE_YESNOCANCEL,
    MESSAGE_BOX_TYPE_COUNT
} MessageBoxType;
inline const char* message_box_type_to_string( MessageBoxType type ) {
    const char* strings[MESSAGE_BOX_TYPE_COUNT] = {
        "Message Box with OK button.",
        "Message Box with OK and CANCEL buttons.",
        "Message Box with RETRY and CANCEL buttons.",
        "Message Box with YES and NO buttons.",
        "Message Box with YES, NO and CANCEL buttons.",
    };
    if( type >= MESSAGE_BOX_TYPE_COUNT ) {
        return "Unknown Message Box type.";
    }
    return strings[type];
}

/// Message box icons
typedef enum : u32 {
    MESSAGE_BOX_ICON_INFORMATION,
    MESSAGE_BOX_ICON_WARNING,
    MESSAGE_BOX_ICON_ERROR,

    MESSAGE_BOX_ICON_COUNT
} MessageBoxIcon;
inline const char* message_box_icon_to_string( MessageBoxIcon icon ) {
    const char* strings[MESSAGE_BOX_ICON_COUNT] = {
        "Message Box \"information\" icon.",
        "Message Box \"warning\" icon.",
        "Message Box \"error\" icon.",
    };
    if( icon >= MESSAGE_BOX_ICON_COUNT ) {
        return "Unknown Message Box icon.";
    }
    return strings[icon];
}

/// User selection from a message box or an error
/// from creating message box.
typedef enum : u32 {
    MESSAGE_BOX_RESULT_OK,
    MESSAGE_BOX_RESULT_CANCEL,
    MESSAGE_BOX_RESULT_RETRY,
    MESSAGE_BOX_RESULT_YES,
    MESSAGE_BOX_RESULT_NO,

    MESSAGE_BOX_RESULT_UNKNOWN_ERROR,

    MESSAGE_BOX_RESULT_COUNT
} MessageBoxResult;
inline const char* message_box_result_to_string( MessageBoxResult result ) {
    const char* strings[MESSAGE_BOX_RESULT_COUNT] = {
        "Message Box OK selected.",
        "Message Box CANCEL selected.",
        "Message Box RETRY selected.",
        "Message Box YES selected.",
        "Message Box NO selected.",
        "An unknown error occurred.",
    };
    if( result >= MESSAGE_BOX_RESULT_COUNT ) {
        return "Unknown Message Box result.";
    }
    return strings[result];
}

/// Create a message box to report urgent information.
MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
);

#define MESSAGE_BOX_FATAL( title, message ) \
    message_box(\
        title,\
        message,\
        MESSAGE_BOX_TYPE_OK,\
        MESSAGE_BOX_ICON_ERROR\
    )

/// Allocate memory on the heap.
/// All platforms must zero out memory before returning pointer.
void* heap_alloc( usize size );
/// Reallocate memory on the heap.
/// All platforms must zero out new memory before returning pointer.
void* heap_realloc( void* memory, usize new_size );
/// Free heap allocated memory.
void heap_free( void* memory );

/// Page allocate memory.
/// All platforms must zero out memory before returning pointer.
void* platform_page_alloc( usize size );
/// Free page allocated memory.
void platform_page_free( void* memory );

#define CPU_NAME_BUFFER_SIZE 68
typedef u16 ProcessorFeatures;
#define SSE_MASK    (1 << 0)
#define SSE2_MASK   (1 << 1)
#define SSE3_MASK   (1 << 2)
#define SSSE3_MASK  (1 << 3)
#define SSE4_1_MASK (1 << 4)
#define SSE4_2_MASK (1 << 5)
#define AVX_MASK    (1 << 6)
#define AVX2_MASK   (1 << 7)
#define AVX512_MASK (1 << 8)
typedef struct SystemInfo {
    usize logical_processor_count;
    usize total_memory;
    char  cpu_name_buffer[CPU_NAME_BUFFER_SIZE];
    ProcessorFeatures features;
} SystemInfo;
/// Query CPU and memory information.
struct SystemInfo query_system_info();

#if defined(LD_PLATFORM_WINDOWS)
void platform_win32_output_debug_string( const char* str );
#endif

#define SURFACE_ICON_PATH "./icon.ico"

#endif
