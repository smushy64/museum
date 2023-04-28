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
        LOG_ERROR("Message Box requires a valid type.");
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
            LOG_ERROR("Message Box returned an unknown result.");
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
