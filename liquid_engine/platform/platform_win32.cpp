/**
 * Description:  Win32 Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"

#if defined(SM_PLATFORM_WINDOWS)
#include "os.h"
#include "memory.h"
#include <windows.h>

// MEMORY | BEGIN ----------------------------------------------------

SM_API void* heap_alloc( usize size ) {
    return (void*)HeapAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY,
        size
    );
}
SM_API void* heap_realloc( void* ptr, usize size ) {
    return (void*)HeapReAlloc(
        GetProcessHeap(),
        HEAP_ZERO_MEMORY |
        HEAP_REALLOC_IN_PLACE_ONLY,
        ptr,
        size
    );
}
SM_API void heap_free( void* ptr ) {
    HeapFree(
        GetProcessHeap(),
        0,
        ptr
    );
}

SM_API void* page_alloc( usize size ) {
    return (void*)VirtualAlloc(
        nullptr,
        size,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
}
SM_API void page_free( void* ptr ) {
    VirtualFree(
        ptr,
        0,
        MEM_RELEASE | MEM_DECOMMIT
    );
}

// MEMORY | END   ----------------------------------------------------

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

    if( !uType ) {
        // TODO(alicia): Log error
        return MBRESULT_UNKNOWN_ERROR;
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
            // TODO(alicia): Log error
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

#endif
