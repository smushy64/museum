#if !defined(LD_CORE_INTERNAL_PLATFORM_H)
#define LD_CORE_INTERNAL_PLATFORM_H
/**
 * Description:  Platform specific functions for core library
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 30, 2023
*/
#include "shared/defines.h"
#include "shared/constants.h"

#include "core/system.h"
#include "core/path.h"

struct TimeRecord;

/// Opaque handle to a shared object.
typedef void PlatformSharedObject;
/// Opaque handle to a file.
typedef void PlatformFile;
/// Opaque handle to a thread.
typedef void PlatformThread;
/// Platform thread function.
typedef int PlatformThreadProc( void* user_params );
/// Opaque handle to a semaphore.
typedef void PlatformSemaphore;
/// Opaque handle to a mutex.
typedef void PlatformMutex;

#define PLATFORM_INFINITE_TIMEOUT (U32_MAX)

/// Get stdout handle.
PlatformFile* platform_get_stdout(void);
/// Get stderr handle.
PlatformFile* platform_get_stderr(void);
/// Get stdin handle.
PlatformFile* platform_get_stdin(void);

/// Open a file.
PlatformFile* platform_file_open( PathSlice path, u32 flags );
/// Close file.
void platform_file_close( PlatformFile* file );
/// Query file size.
usize platform_file_query_size( PlatformFile* file );
/// Query file offset.
usize platform_file_query_offset( PlatformFile* file );
/// Set file offset.
void platform_file_set_offset( PlatformFile* file, usize offset );
/// Read file.
b32 platform_file_read( PlatformFile* file, usize buffer_size, void* buffer );
/// Write file.
b32 platform_file_write( PlatformFile* file, usize buffer_size, void* buffer );
/// Delete file.
b32 platform_delete_file( PathSlice path );
/// Copy by path.
b32 platform_file_copy_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists );
/// Move by path.
b32 platform_file_move_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists );
/// Check if path is file.
b32 platform_path_is_file( PathSlice path );
/// Check if path is directory.
b32 platform_path_is_directory( PathSlice path );

/// Get the current working directory.
/// If buffer is null, returns required size.
/// If buffer is not large enough, returns bytes not written to buffer.
usize platform_get_working_directory(
    usize buffer_size, char* buffer, usize* opt_out_written_bytes );

#if defined(LD_PLATFORM_WINDOWS)
/// Output debug string.
void platform_win32_output_debug_string( const char* cstr );
#else
#define platform_win32_output_debug_string( ... )
#endif

/// Open a shared object.
PlatformSharedObject* platform_shared_object_open( const char* path );
/// Close a shared object.
void platform_shared_object_close( PlatformSharedObject* object );
/// Load a function from shared object.
void* platform_shared_object_load(
    PlatformSharedObject* object, const char* function_name );

/// Create a thread.
PlatformThread* platform_thread_create(
    PlatformThreadProc* thread_proc, void* thread_proc_params, usize stack_size );

/// Create a semaphore.
PlatformSemaphore* platform_semaphore_create( const char* name, u32 initial_count );
/// Destroy a semaphore.
void platform_semaphore_destroy( PlatformSemaphore* semaphore );
/// Signal a semaphore.
void platform_semaphore_signal( PlatformSemaphore* semaphore );
/// Wait for a semaphore to be signaled for specified milliseconds.
/// Returns false if timed out.
b32 platform_semaphore_wait( PlatformSemaphore* semaphore, u32 timeout_ms );

/// Create a mutex.
PlatformMutex* platform_mutex_create( const char* name );
/// Destroy a mutex.
void platform_mutex_destroy( PlatformMutex* mutex );
/// Try to lock a mutex.
/// Returns false if timed out.
b32 platform_mutex_lock( PlatformMutex* mutex, u32 timeout_ms );
/// Unlock a mutex.
void platform_mutex_unlock( PlatformMutex* mutex );

/// Sleep thread for given milliseconds.
void platform_sleep( u32 ms );

/// Allocate memory from the heap.
/// Memory acquired is always zeroed.
void* platform_heap_alloc( usize size );
/// Reallocate memory from the heap.
/// New memory acquired is always zeroed.
void* platform_heap_realloc( void* memory, usize old_size, usize new_size );
/// Free memory allocated from the heap.
void platform_heap_free( void* memory, usize size );

/// Initialize time keeping.
void platform_time_initialize(void);
/// Get time record.
void platform_time_record( struct TimeRecord* out_record );
/// Query elapsed time since initialization.
f64 platform_time_query_elapsed_seconds(void);

/// Query system info.
void platform_system_info_query( SystemInfo* out_info );

#endif /* header guard */
