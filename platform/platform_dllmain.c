/**
 * Description:  DLL main
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: September 23, 2023
*/
#include "shared/defines.h"
#if !defined(DLLMAIN_DISABLED)

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(LD_PLATFORM_LINUX) && defined(SHARED_EXECUTABLE)
const char interp[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";
#endif

#if defined(LD_PLATFORM_WINDOWS)
#include <windows.h>

BOOL WINAPI DllMainCRTStartup(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
) {
    unused(instance);
    unused(reserved);
    switch( reason ) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}

#endif /* Platform Windows */

#if !defined(NO_CSTD)
    #include "shared/custom_cstd.c"
#endif

#if defined(__cplusplus)
} // extern "C"
#endif

#endif /* If DLLMAIN_DISABLED is not defined */

