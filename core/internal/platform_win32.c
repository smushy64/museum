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
#include "core/string.h"
#include "core/path.h"
#include "core/fs.h"

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
#include <intrin.h>

global LARGE_INTEGER global_performance_frequency = {};
global LARGE_INTEGER global_performance_counter   = {};

// TODO(alicia): 
// - Logging
// - file_read_offset

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

void ___format_message( char* buffer, usize buffer_size, DWORD error_code );

const char* ___make_win32_path( PathSlice path, usize* out_length ) {
    if( path.len <= MAX_PATH && ( !path.buffer[path.len] || !path.buffer[path.len - 1] ) ) {
        *out_length = 0;
        return path.buffer;
    }


    #define PREPEND "\\\\?\\"
    usize buffer_size = path.len + 1 + sizeof(PREPEND);
    char* buffer = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, buffer_size );
    memory_copy( buffer, PREPEND, sizeof(PREPEND) - 1 );

    PathBuffer path_buffer = {};
    path_buffer.buffer   = (buffer + (sizeof(PREPEND) - 1));
    path_buffer.capacity = path.len;

    path_slice_convert_separators( string_buffer_write, &path_buffer, path, false );

    #undef PREPEND

    *out_length = buffer_size;
    return buffer;
}
void ___free_win32_path( usize length, const char* path ) {
    if( !length ) {
        return;
    }

    HeapFree( GetProcessHeap(), 0, (void*)path );
}

PlatformFile* platform_file_open( PathSlice path, u32 flags ) {
    usize win32_path_length = 0;
    const char* win32_path = ___make_win32_path( path, &win32_path_length );

    PlatformFile* result = NULL;

    DWORD dwDesiredAccess = 0;
    if( bitfield_check( flags, FILE_OPEN_FLAG_READ ) ) {
        dwDesiredAccess |= GENERIC_READ;
    }
    if( bitfield_check( flags, FILE_OPEN_FLAG_WRITE ) ) {
        dwDesiredAccess |= GENERIC_WRITE;
    }
    DWORD dwShareMode = 0;
    if( bitfield_check( flags, FILE_OPEN_FLAG_SHARE_ACCESS_WRITE ) ) {
        dwShareMode |= FILE_SHARE_WRITE;
    }
    if( bitfield_check( flags, FILE_OPEN_FLAG_SHARE_ACCESS_READ ) ) {
        dwShareMode |= FILE_SHARE_READ;
    }

    LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;

    DWORD dwCreationDisposition = OPEN_EXISTING;
    if( bitfield_check( flags, FILE_OPEN_FLAG_WRITE ) ) {
        dwCreationDisposition = CREATE_ALWAYS;
    }
    if( bitfield_check( flags, FILE_OPEN_FLAG_CREATE ) ) {
        dwCreationDisposition = CREATE_ALWAYS;
    }
    if( bitfield_check( flags, FILE_OPEN_FLAG_TRUNCATE ) ) {
        dwCreationDisposition = TRUNCATE_EXISTING;
    }

    DWORD dwFlagsAndAttributes = 0;
    HANDLE hTemplateFile = NULL;

    HANDLE handle = CreateFileA(
        win32_path, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile );
    if( handle == INVALID_HANDLE_VALUE ) {
        if( core_log_enabled() ) {
            DWORD error = GetLastError();
            char  error_log[256] = {};
            ___format_message( error_log, 255, error );

            core_log_error(
                "failed to open file '{s}' | error: {u32,X} {cc}",
                path, error, error_log );
        }
    } else {
        result = handle;
    }

    ___free_win32_path( win32_path_length, win32_path );
    return result;
}
void platform_file_close( PlatformFile* file ) {
    CloseHandle( file );
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
usize platform_file_query_offset( PlatformFile* file ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER offset = {};
    LARGE_INTEGER result = {};
    SetFilePointerEx( file, offset, &result, FILE_CURRENT );
    return result.QuadPart;
#else
    LONG result = 0;
    SetFilePointer( file, 0, &result, FILE_CURRENT );
    return result;
#endif
}
void platform_file_set_offset( PlatformFile* file, usize offset ) {
#if defined(LD_ARCH_64_BIT)
    LARGE_INTEGER large_offset = {};
    large_offset.QuadPart = offset;

    SetFilePointerEx( file, large_offset, NULL, FILE_BEGIN );
#else
    LONG small_offset = offset;
    SetFilePointer( file, small_offset, NULL, FILE_BEGIN );
#endif
}

b32 ___file_read_32bit( PlatformFile* file, u32 buffer_size, void* buffer ) {
    DWORD bytes_read = 0;
    BOOL result = ReadFile(
        file, buffer, buffer_size,
        &bytes_read, NULL );

    if( !result || bytes_read != buffer_size ) {
        if( core_log_enabled() ) {
            DWORD error_code = GetLastError();
            char  error_log[255] = {};
            ___format_message( error_log, 255, error_code );
            core_log_error(
                "failed to read file! | {u32,X} {cc}", error_code, error_log );
        }
        return false;
    }

    return true;
}

b32 platform_file_read( PlatformFile* file, usize buffer_size, void* buffer ) {
    u32 read  = buffer_size;
    u8* bytes = buffer;

#if defined(LD_ARCH_64_BIT)
    if( buffer_size > U32_MAX ) {
        read = U32_MAX;
    } else {
        read = buffer_size;
    }

    if( !___file_read_32bit( file, read, bytes ) ) {
        return false;
    }
    buffer_size -= read;
    if( !buffer_size ) {
        return true;
    }

    bytes += read;
    read   = buffer_size;
#endif

    return ___file_read_32bit( file, read, bytes );
}

b32 ___file_write_32bit( PlatformFile* file, u32 buffer_size, void* buffer ) {
    DWORD bytes_read = 0;
    BOOL result = WriteFile(
        file, buffer, buffer_size,
        &bytes_read, NULL );

    if( !result || bytes_read != buffer_size ) {
        if( core_log_enabled() ) {
            DWORD error_code = GetLastError();
            char  error_log[255] = {};
            ___format_message( error_log, 255, error_code );
            core_log_error(
                "failed to write file! | {u32,X} {cc}", error_code, error_log );
        }
        return false;
    }

    return true;
}

b32 platform_file_write( PlatformFile* file, usize buffer_size, void* buffer ) {
    u32 write = buffer_size;
    u8* bytes = buffer;
#if defined(LD_ARCH_64_BIT)
    if( buffer_size > U32_MAX ) {
        write = U32_MAX;
    } else {
        write = buffer_size;
    }

    if( !___file_write_32bit( file, write, bytes ) ) {
        return false;
    }
    buffer_size -= write;
    if( !buffer_size ) {
        return true;
    }

    bytes += write;
    write  = buffer_size;
#endif
    return ___file_write_32bit( file, write, bytes );
}
b32 platform_delete_file( PathSlice path ) {
    usize path_len = 0;
    const char* win32_path = ___make_win32_path( path, &path_len );

    WINBOOL result = DeleteFileA( win32_path );

    ___free_win32_path( path_len, win32_path );
    return result > 0;
}
b32 platform_file_copy_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    usize dst_path_len = 0;
    usize src_path_len = 0;
    const char* dst_path = ___make_win32_path( dst, &dst_path_len );
    const char* src_path = ___make_win32_path( src, &src_path_len );

    WINBOOL result = CopyFileA( src_path, dst_path, fail_if_dst_exists );

    ___free_win32_path( dst_path_len, dst_path );
    ___free_win32_path( src_path_len, src_path );

    return result > 0;
}
b32 platform_file_move_by_path( PathSlice dst, PathSlice src, b32 fail_if_dst_exists ) {
    b32 dst_exists = fs_check_if_file_exists( dst );
    if( dst_exists ) {
        if( fail_if_dst_exists ) {
            return false;
        } else {
            if( !platform_delete_file( dst ) ) {
                return false;
            }
        }
    }

    usize dst_path_len = 0;
    usize src_path_len = 0;
    const char* dst_path = ___make_win32_path( dst, &dst_path_len );
    const char* src_path = ___make_win32_path( src, &src_path_len );

    WINBOOL result = MoveFileA( src_path, dst_path );

    ___free_win32_path( dst_path_len, dst_path );
    ___free_win32_path( src_path_len, src_path );

    return result > 0;
}
b32 platform_path_is_file( PathSlice path ) {
    usize path_len = 0;
    const char* win32_path = ___make_win32_path( path, &path_len );

    DWORD attributes = GetFileAttributesA( win32_path );

    ___free_win32_path( path_len, win32_path );

    return !bitfield_check( attributes, FILE_ATTRIBUTE_DIRECTORY );
}
b32 platform_path_is_directory( PathSlice path ) {
    usize path_len = 0;
    const char* win32_path = ___make_win32_path( path, &path_len );

    DWORD attributes = GetFileAttributesA( win32_path );

    ___free_win32_path( path_len, win32_path );

    return bitfield_check( attributes, FILE_ATTRIBUTE_DIRECTORY );
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

void platform_system_info_query( SystemInfo* out_info ) {
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

void ___format_message( char* buffer, usize buffer_size, DWORD error_code ) {
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM    |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL, error_code, 0, buffer, buffer_size, NULL );
}


#endif /* Platform Windows */

