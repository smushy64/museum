#if !defined(PLATFORM_THREADING_HPP)
#define PLATFORM_THREADING_HPP
/**
 * Description:  Platform multi-threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"
#include "core/threading.h"

#if defined(LD_PLATFORM_WINDOWS)
    #define THREAD_HANDLE_SIZE (32)
    typedef unsigned long ThreadReturnCode;
#elif defined(LD_PLATFORM_LINUX)
    #define THREAD_HANDLE_SIZE (24)
    typedef void* ThreadReturnCode;
#else
    #define THREAD_HANDLE_SIZE (sizeof(usize))
    typedef int ThreadReturnCode;
#endif

/// Opaque handle to a thread.
struct ThreadHandle {
    u8 platform[THREAD_HANDLE_SIZE];
};

/// Thread Proc definition.
typedef ThreadReturnCode (*ThreadProcFN)( void* user_params );

/// Create a thread.
b32 platform_thread_create(
    struct Platform* platform,
    ThreadProcFN     thread_proc,
    void*            user_params,
    usize            thread_stack_size,
    ThreadHandle*    out_thread_handle
);

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
