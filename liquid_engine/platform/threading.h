#if !defined(PLATFORM_THREADING_HPP)
#define PLATFORM_THREADING_HPP
/**
 * Description:  Platform multi-threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"
#include "core/threading.h"

#define THREAD_HANDLE_WIN32_SIZE 32
#define THREAD_HANDLE_OTHER_SIZE (sizeof(usize))

/// Handle to a thread.
struct ThreadHandle {
#if defined(LD_PLATFORM_WINDOWS)
    u8 platform[THREAD_HANDLE_WIN32_SIZE];
#else
    u8 platform[THREAD_HANDLE_OTHER_SIZE];
#endif
};

#if defined(LD_PLATFORM_WINDOWS)
    typedef unsigned long ThreadReturnCode;
#else
    typedef int ThreadReturnCode;
#endif

/// Thread Proc definition.
typedef ThreadReturnCode (*ThreadProcFN)( void* user_params );

/// Create a thread.
b32 platform_thread_create(
    struct Platform* platform,
    ThreadProcFN     thread_proc,
    void*            user_params,
    usize            thread_stack_size,
    b32              run_on_create,
    ThreadHandle*    out_thread_handle
);
/// Resume a suspended thread
b32 platform_thread_resume( ThreadHandle* thread_handle );

u32 platform_interlocked_increment( volatile u32* addend );
u32 platform_interlocked_decrement( volatile u32* addend );
u32 platform_interlocked_exchange( volatile u32* target, u32 value );
void* platform_interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange, void* comperand
);
u32 platform_interlocked_compare_exchange(
    u32 volatile* dst,
    u32 exchange, u32 comperand
);

#endif // header guard
