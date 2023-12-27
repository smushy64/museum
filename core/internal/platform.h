#if !defined(LD_CORE_INTERNAL_PLATFORM_H)
#define LD_CORE_INTERNAL_PLATFORM_H
/**
 * Description:  Platform specific functions for core library
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 30, 2023
*/
#include "shared/defines.h"
#include "shared/constants.h"

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

/// File open flags.
typedef enum PlatformFileFlags : u32 {
    PLATFORM_FILE_READ          = (1 << 0),
    PLATFORM_FILE_WRITE         = (1 << 1),
    PLATFORM_FILE_SHARE_READ    = (1 << 2),
    PLATFORM_FILE_SHARE_WRITE   = (1 << 3),
    PLATFORM_FILE_ONLY_EXISTING = (1 << 4),
} PlatformFileFlags;

/// Get stdout handle.
PlatformFile* platform_get_stdout(void);
/// Get stderr handle.
PlatformFile* platform_get_stderr(void);
/// Get stdin handle.
PlatformFile* platform_get_stdin(void);

/// Open a file.
PlatformFile* platform_file_open( const char* path, PlatformFileFlags flags );
/// Close a file.
void platform_file_close( PlatformFile* file );
/// Query current offset into file.
usize platform_file_query_offset( PlatformFile* file );
/// Set offset into file, from start of file.
void platform_file_set_offset( PlatformFile* file, usize offset );
/// Query file size.
usize platform_file_query_size( PlatformFile* file );
/// Write into file at current offset.
b32 platform_file_write( PlatformFile* file, usize buffer_size, void* buffer );
/// Write into file at custom offset, does not affect current offset.
b32 platform_file_write_offset(
    PlatformFile* file, usize offset, usize buffer_size, void* buffer );
/// Read file at current offset.
b32 platform_file_read( PlatformFile* file, usize buffer_size, void* buffer );
/// Read file at custom offset, does not affect current offset.
b32 platform_file_read_offset(
    PlatformFile* file, usize offset, usize buffer_size, void* buffer );
/// Delete a file by its path.
b32 platform_file_delete( const char* path );
/// Copy a file to another path.
b32 platform_file_copy(
    const char* dst_path, const char* src_path, b32 fail_if_dst_exists );
/// Move a file to another path.
b32 platform_file_move(
    const char* dst_path, const char* src_path, b32 fail_if_dst_exists );
/// Check if a file exists.
b32 platform_file_check_if_exists( const char* path );

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

/// Query how many processors the system has.
usize platform_query_processor_count(void);

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

/// Query the current system's page size.
usize platform_query_page_size(void);

#endif /* header guard */
