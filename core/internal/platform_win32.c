/**
 * Description:  Core library win32 implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 30, 2023
*/
#include "shared/defines.h"

#if defined(LD_PLATFORM_WINDOWS)
#include "shared/constants.h"
#include "core/time.h"
#include "core/internal/platform.h"
#include "core/internal/logging.h"
#include "core/system.h"
#include "core/memory.h"

#define win32_log_note( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_NOTE,\
        sizeof( "[WIN32] " format), "[WIN32] " format, ##__VA_ARGS__ )
#define win32_log_info( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_INFO,\
        sizeof( "[WIN32] " format), "[WIN32] " format, ##__VA_ARGS__ )
#define win32_log_warn( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_WARN,\
        sizeof( "[WIN32] " format), "[WIN32] " format, ##__VA_ARGS__ )
#define win32_log_error( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_ERROR,\
        sizeof( "[WIN32] " format), "[WIN32] " format, ##__VA_ARGS__ )
#define win32_log_fatal( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_FATAL,\
        sizeof( "[WIN32] " format), "[WIN32] " format, ##__VA_ARGS__ )   

#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <intrin.h>

#define WIN32_DECLARE_FUNCTION( ret, fn, ... )\
    typedef ret ___internal_##fn##FN( __VA_ARGS__ );\
    global ___internal_##fn##FN* ___internal_##fn = NULL

WIN32_DECLARE_FUNCTION( BOOL, PathFileExistsA, LPCSTR pszPath );
#define PathFileExistsA ___internal_PathFileExistsA

#define WIN32_CHECK_FUNC( fn, module ) do {\
    if( !fn ) {\
        if( !global_##module##_object ) {\
            global_##module##_object = LoadLibraryA( #module ".dll" );\
            assert( global_##module##_object );\
        }\
        fn = (___internal_##fn##FN*)GetProcAddress( global_##module##_object, #fn );\
        assert( fn );\
    }\
} while(0)

global LARGE_INTEGER global_performance_frequency = {};
global LARGE_INTEGER global_performance_counter   = {};

// TODO(alicia): 
// - Logging
// - file_read_offset

global SYSTEM_INFO global_system_info  = {};
global b32 global_system_info_obtained = false;
global HANDLE global_shlwapi_object    = NULL;

struct Win32ThreadParams {
    PlatformThreadProc* thread_proc;
    void* user_params;
};
struct Win32Thread {
    HANDLE handle;
    DWORD  id;
    struct Win32ThreadParams params;
};

PlatformFile* platform_get_stdout(void) {
    return (PlatformFile*)GetStdHandle( STD_OUTPUT_HANDLE );
}
PlatformFile* platform_get_stderr(void) {
    return (PlatformFile*)GetStdHandle( STD_ERROR_HANDLE );
}
PlatformFile* platform_get_stdin(void) {
    return (PlatformFile*)GetStdHandle( STD_INPUT_HANDLE );
}

PlatformFile* platform_file_open( const char* path, PlatformFileFlags flags ) {
    DWORD dwDesiredAccess = 0;
    if( bitfield_check( flags, PLATFORM_FILE_READ ) ) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if( bitfield_check( flags, PLATFORM_FILE_WRITE ) ) {
        dwDesiredAccess |= GENERIC_WRITE;
    }

    DWORD dwShareMode = 0;
    if( bitfield_check( flags, PLATFORM_FILE_SHARE_READ ) ) {
        dwShareMode |= FILE_SHARE_READ;
    }
    if( bitfield_check( flags, PLATFORM_FILE_SHARE_WRITE ) ) {
        dwShareMode |= FILE_SHARE_WRITE;
    }

    DWORD dwCreationDisposition = 0;
    if( bitfield_check( flags, PLATFORM_FILE_ONLY_EXISTING ) ) {
        dwCreationDisposition |= OPEN_EXISTING;
    } else {
        dwCreationDisposition |= OPEN_ALWAYS;
    }

    LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;

    DWORD dwFlagsAndAttributes = 0;

    HANDLE hTemplateFile = NULL;

    HANDLE handle = CreateFileA(
        path,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile );
    if( handle == INVALID_HANDLE_VALUE ) {
        return NULL;
    }

    return (PlatformFile*)handle;
}
void platform_file_close( PlatformFile* file ) {
    CloseHandle( (HANDLE)file );
}
usize platform_file_query_offset( PlatformFile* file ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER offset = {};
    LARGE_INTEGER result = {};
    SetFilePointerEx(
        file,
        offset,
        &result,
        FILE_CURRENT
    );

    return result.QuadPart;
#else
    LONG result = 0;
    SetFilePointer(
        file,
        0, &result,
        FILE_CURRENT
    );

    return result;
#endif
}
void platform_file_set_offset( PlatformFile* file, usize offset ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER large_offset = {};
    large_offset.QuadPart = offset;

    SetFilePointerEx(
        (HANDLE)file,
        large_offset,
        NULL,
        FILE_BEGIN
    );
#else
    LONG off = offset;
    SetFilePointer(
        file,
        off, NULL,
        FILE_BEGIN
    );
#endif
}
usize platform_file_query_size( PlatformFile* file ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER result = {};
    GetFileSizeEx( file, &result );
    return result.QuadPart;
#else
    return GetFileSize( file, NULL );
#endif
}
b32 platform_file_write( PlatformFile* file, usize buffer_size, void* buffer ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER write = {};
    write.QuadPart      = buffer_size;
    DWORD bytes_written = 0;

    if( !WriteFile(
        file, buffer,
        write.LowPart,
        &bytes_written,
        NULL
    ) ) {
        return false;
    }

    if( bytes_written != write.LowPart ) {
        return false;
    }

    // early return if no more write.
    if( !write.HighPart ) {
        return true;
    }

    bytes_written = 0;
    if( !WriteFile(
        file, (u8*)buffer + write.LowPart,
        write.HighPart,
        &bytes_written,
        NULL
    ) ) {
        return false;
    }

    if( bytes_written != *(DWORD*)&write.HighPart ) {
        return false;
    }

    return true;
#else
    DWORD bytes_to_write = buffer_size;
    DWORD bytes_written  = 0;

    if( !WriteFile(
        file, buffer,
        bytes_to_write,
        &bytes_written,
        NULL
    ) ) {
        return false;
    }

    if( bytes_to_write != bytes_written ) {
        return false;
    }

    return true;
#endif
}
b32 platform_file_write_offset(
    PlatformFile* file, usize offset_from_start, usize buffer_size, void* buffer
) {
    LARGE_INTEGER offset = {};
    offset.QuadPart = offset_from_start;

    OVERLAPPED overlapped = {};
    overlapped.Offset     = offset.LowPart;
    overlapped.OffsetHigh = offset.HighPart;

    LARGE_INTEGER write = {};
    write.QuadPart      = buffer_size;
    DWORD bytes_written = 0;

    if( WriteFile(
        file, buffer,
        write.LowPart,
        &bytes_written,
        &overlapped
    ) != TRUE ) {
        if( GetLastError() != ERROR_IO_PENDING ) {
            return false;
        }
    }

    if( bytes_written != write.LowPart ) {
        return false;
    }

    return true;
}
b32 platform_file_read( PlatformFile* file, usize buffer_size, void* buffer ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER read = {};
    read.QuadPart = buffer_size;

    DWORD bytes_read = 0;
    if( !ReadFile(
        file, buffer,
        read.LowPart,
        &bytes_read,
        NULL
    ) ) {
        return false;
    }

    if( bytes_read < read.LowPart ) {
        return false;
    }

    // if less than 4GB, early return 
    if( !read.HighPart ) {
        return true;
    }

    bytes_read = 0;
    if( !ReadFile(
        file, (u8*)buffer + read.LowPart,
        read.HighPart,
        &bytes_read,
        NULL
    ) ) {
        return false;
    }

    if( bytes_read < *(DWORD*)&read.HighPart ) {
        return false;
    }

#else
    DWORD bytes_to_read = buffer_size;
    DWORD bytes_read    = 0;
    if( !ReadFile(
        file, buffer,
        bytes_to_read,
        &bytes_to_read,
        NULL
    ) ) {
        return false;
    }

    if( bytes_to_read < bytes_read ) {
        return false;
    }
#endif

    return true;
}
b32 platform_file_read_offset(
    PlatformFile* file, usize offset, usize buffer_size, void* buffer );
b32 platform_file_delete( const char* path ) {
    return DeleteFileA( path ) > 0;
}
b32 platform_file_copy(
    const char* dst_path, const char* src_path, b32 fail_if_dst_exists
) {
    return CopyFileA( src_path, dst_path, fail_if_dst_exists ) > 0;
}
b32 platform_file_move(
    const char* dst_path, const char* src_path, b32 fail_if_dst_exists
) {
    b32 dst_exists = platform_file_check_if_exists( dst_path );
    if( dst_exists ) {
        if( fail_if_dst_exists ) {
            return false;
        } else {
            if( !platform_file_delete( dst_path ) ) {
                return false;
            }
        }
    }

    return MoveFileA( src_path, dst_path ) > 0;
}
b32 platform_file_check_if_exists( const char* path ) {
    WIN32_CHECK_FUNC( PathFileExistsA, shlwapi );
    return PathFileExistsA( path ) == TRUE;
}
void platform_win32_output_debug_string( const char* cstr ) {
    OutputDebugStringA( cstr );
}

PlatformSharedObject* platform_shared_object_open( const char* path ) {
    HMODULE module = LoadLibraryA( path );
    return (PlatformSharedObject*)module;
}
void platform_shared_object_close( PlatformSharedObject* object ) {
    FreeLibrary( (HMODULE)object );
}
void* platform_shared_object_load(
    PlatformSharedObject* object, const char* function_name
) {
    void* function = (void*)GetProcAddress( (HMODULE)object, function_name );
    return function;
}

internal DWORD ___internal_win32_thread_proc( void* in ) {
    struct Win32Thread* thread = in;

    _ReadWriteBarrier();

    int return_code = thread->params.thread_proc( thread->params.user_params );

    ExitThread( return_code );
}

PlatformThread* platform_thread_create(
    PlatformThreadProc* thread_proc, void* thread_proc_params,
    usize stack_size
) {
    struct Win32Thread* thread = HeapAlloc( GetProcessHeap(), 0, sizeof(*thread) );
    if( !thread ) {
        return NULL;
    }

    thread->params.thread_proc = thread_proc;
    thread->params.user_params = thread_proc_params;

    _ReadWriteBarrier();

    DWORD id = 0;
    HANDLE thread_handle = CreateThread(
        NULL, stack_size, ___internal_win32_thread_proc, thread, 0, &id );
    if( !thread_handle ) {
        HeapFree( GetProcessHeap(), 0, thread );
        return NULL;
    }

    _ReadWriteBarrier();

    thread->handle = thread_handle;
    thread->id     = id;

    return thread;
}

PlatformSemaphore* platform_semaphore_create( const char* name, u32 initial_count ) {
    HANDLE result = CreateSemaphoreEx(
        NULL, initial_count, I32_MAX, name, 0, SEMAPHORE_ALL_ACCESS );
    return (PlatformSemaphore*)result;
}
void platform_semaphore_destroy( PlatformSemaphore* semaphore ) {
    CloseHandle( (PlatformSemaphore*)semaphore );
}
void platform_semaphore_signal( PlatformSemaphore* semaphore ) {
    ReleaseSemaphore( (HANDLE)semaphore, 1, NULL );
}
b32 platform_semaphore_wait( PlatformSemaphore* semaphore, u32 ms ) {
    DWORD milliseconds = ms == PLATFORM_INFINITE_TIMEOUT ? INFINITE : ms;
    DWORD result = WaitForSingleObject( (HANDLE)semaphore, milliseconds );

    if( ms != PLATFORM_INFINITE_TIMEOUT ) {
        return result != WAIT_TIMEOUT;
    }
    return true;
}

PlatformMutex* platform_mutex_create( const char* opt_name ) {
    return (PlatformMutex*)CreateMutexA( NULL, false, opt_name );
}
void platform_mutex_destroy( PlatformMutex* mutex ) {
    CloseHandle( (HANDLE)mutex );
}
b32 platform_mutex_lock( PlatformMutex* mutex, u32 ms ) {
    DWORD milliseconds = ms == PLATFORM_INFINITE_TIMEOUT ? INFINITE : ms;
    DWORD result = WaitForSingleObject( (HANDLE)mutex, milliseconds );
    if( ms != PLATFORM_INFINITE_TIMEOUT ) {
        return result != WAIT_TIMEOUT;
    }

    return true;
}
void platform_mutex_unlock( PlatformMutex* mutex ) {
    ReleaseMutex( (HANDLE)mutex );
}

void* platform_heap_alloc( usize size ) {
    return (void*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, size );
}
void* platform_heap_realloc( void* memory, usize old_size, usize new_size ) {
    unused(old_size);
    return (void*)HeapReAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, memory, new_size );
}
void platform_heap_free( void* memory, usize size ) {
    unused(size);
    HeapFree( GetProcessHeap(), 0, memory );
}
void platform_sleep( u32 ms ) {
    Sleep( (DWORD)ms );
}

internal void ___internal_win32_get_system_info(void) {
    GetSystemInfo( &global_system_info );
    global_system_info_obtained = true;
}

usize platform_query_page_size(void) {
    if( !global_system_info_obtained ) {
        ___internal_win32_get_system_info();
    }
    return global_system_info.dwPageSize;
}
usize platform_query_processor_count(void) {
    if( !global_system_info_obtained ) {
        ___internal_win32_get_system_info();
    }
    return (usize)global_system_info.dwNumberOfProcessors;
}

void platform_time_initialize(void) {
    QueryPerformanceFrequency( &global_performance_frequency );
    QueryPerformanceCounter( &global_performance_counter );
}
f64 platform_time_query_elapsed_seconds(void) {
    LARGE_INTEGER current_counter;
    QueryPerformanceCounter( &current_counter );
    u64 elapsed = current_counter.QuadPart - global_performance_counter.QuadPart;

    return (f64)elapsed / (f64)global_performance_frequency.QuadPart;
}
void platform_time_record( struct TimeRecord* out_record ) {
    SYSTEMTIME system_time;
    GetLocalTime( &system_time );

    out_record->year   = (u32)system_time.wYear;
    out_record->month  = (u32)system_time.wMonth;
    out_record->day    = (u32)system_time.wDay;
    out_record->hour   = (u32)system_time.wHour;
    out_record->minute = (u32)system_time.wMinute;
    out_record->second = (u32)system_time.wSecond;
}

CORE_API void system_info_query( SystemInfo* out_info ) {
    SYSTEM_INFO info = {};
    GetSystemInfo( &info );

    out_info->page_size = info.dwPageSize;
    out_info->cpu_count = info.dwNumberOfProcessors;

    if( IsProcessorFeaturePresent(
        PF_XMMI_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_SSE;
    }
    if( IsProcessorFeaturePresent(
        PF_XMMI64_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_SSE2;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_SSE3;
    }
    if( IsProcessorFeaturePresent(
        PF_SSSE3_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_SSSE3;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_1_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_SSE4_1;
    }
    if( IsProcessorFeaturePresent(
        PF_SSE4_2_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_SSE4_2;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_AVX;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX2_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_AVX2;
    }
    if( IsProcessorFeaturePresent(
        PF_AVX512F_INSTRUCTIONS_AVAILABLE
    ) ) {
        out_info->feature_flags |= CPU_FEATURE_AVX_512;
    }

    MEMORYSTATUSEX memory_status = {};
    memory_status.dwLength = sizeof( memory_status );
    GlobalMemoryStatusEx( &memory_status );

    out_info->total_memory = memory_status.ullTotalPhys;

#if defined(LD_ARCH_X86)
    {
        memory_set( out_info->cpu_name, ' ', SYSTEM_INFO_CPU_NAME_CAPACITY - 1 );

        int cpu_info[4] = {};
        char* chunk = out_info->cpu_name;

        __cpuid( cpu_info, 0x80000002 );
        memory_copy( chunk, cpu_info, sizeof(cpu_info) );
        chunk += sizeof(cpu_info);

        __cpuid( cpu_info, 0x80000003 );
        memory_copy( chunk, cpu_info, sizeof(cpu_info) );
        chunk += sizeof(cpu_info);

        __cpuid( cpu_info, 0x80000004 );
        memory_copy( chunk, cpu_info, sizeof(cpu_info) );
    }
#endif /* Arch x86 */

}

#endif /* Platform Windows */

