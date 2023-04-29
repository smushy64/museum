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
#include <windows.h>
#include <stdio.h>

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
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
            LOG_COLOR_RESET, 0,\
            "[NOTE WIN32  | %s | %s:%i] ",\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        );\
        log_formatted_locked(\
            LOG_LEVEL_INFO | LOG_LEVEL_TRACE,\
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

// MEMORY | BEGIN ---------------------------------------------------------

static usize HEAP_MEMORY_USAGE = 0;
SM_API usize query_heap_memory_usage() {
    return HEAP_MEMORY_USAGE;
}

SM_API MemoryBlock heap_alloc( usize size ) {
    MemoryBlock result = {};
    void* pointer = (void*)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        size
    );
    if( pointer ) {
        HEAP_MEMORY_USAGE += size;
        result.pointer = pointer;
        result.size    = size;
    }
    return result;
}
SM_API b32 heap_realloc( MemoryBlock* memory, usize new_size ) {
    void* new_pointer = (void*)HeapReAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY |
        HEAP_REALLOC_IN_PLACE_ONLY,
        memory->pointer,
        new_size
    );
    if( !new_pointer ) {
        return false;
    }

    usize size_diff = new_size - memory->size;
    HEAP_MEMORY_USAGE += size_diff;

    memory->pointer = new_pointer;
    memory->size    = new_size;

    return true;
}
SM_API void heap_free( MemoryBlock* memory ) {
    HeapFree( GetProcessHeap(), 0, memory->pointer );
    HEAP_MEMORY_USAGE -= memory->size;
    *memory = {};
}

SM_API MemoryBlock page_alloc( usize size ) {
    MemoryBlock result = {};
    void* pointer = (void*)VirtualAlloc(
        nullptr,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
    if( pointer ) {
        result.pointer = pointer;
        result.size    = size;
    }

    return result;
}
SM_API void page_free( MemoryBlock* memory ) {
    VirtualFree(
        memory->pointer,
        0,
        MEM_RELEASE | MEM_DECOMMIT
    );
    *memory = {};
}

// MEMORY | END   ---------------------------------------------------------

// MULTI-THREADING | BEGIN ------------------------------------------------

struct Win32ThreadHandle {
    HANDLE     handle;
    ThreadProc proc;
    void*      params;
    DWORD      id;
};

DWORD WINAPI win32_thread_proc( void* params ) {
    Win32ThreadHandle* thread_handle = (Win32ThreadHandle*)params;

    DWORD return_value = thread_handle->proc(
        thread_handle->params
    );

    HeapFree(
        GetProcessHeap(),
        0,
        params
    );

    return return_value;
}

#define THREAD_STACK_SIZE_SAME_AS_MAIN 0
#define THREAD_RUN_ON_CREATE 0
SM_API ThreadHandle thread_create(
    ThreadProc thread_proc,
    void*      params,
    b32        run_on_creation
) {
    void* handle_buffer = HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        sizeof(Win32ThreadHandle)
    );
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
        win32_thread_proc,
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
        default: break;
    }

    if( !uType ) {
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

#endif
