#if !defined(CORE_THREADING_HPP)
#define CORE_THREADING_HPP
/**
 * Description:  Multi-Threading
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: June 14, 2023
*/
#include "defines.h"

/// Opaque pointer to thread info.
typedef struct ThreadInfo* pThreadInfo;
/// Thread work function.
typedef void (*ThreadWorkProcFN) ( pThreadInfo thread_info, void* user_params );
/// Thread work entry
struct ThreadWorkEntry {
    ThreadWorkProcFN thread_work_proc;
    void*            thread_work_user_params;
};
/// Push a new thread work entry into the work queue.
LD_API void thread_work_queue_push(
    struct ThreadWorkQueue* work_queue,
    ThreadWorkEntry work_entry
);
/// Query the index of the current thread.
LD_API u32 thread_info_read_index( pThreadInfo thread_info );

#if defined(LD_ARCH_X86)
    /// Complete all reads and writes before this.
    #define read_write_fence()\
        __asm__ volatile ("mfence":::"memory")
    /// Complete all reads before this.
    #define read_fence()\
        __asm__ volatile ("lfence":::"memory")
    /// Complete all writes before this.
    #define write_fence()\
        __asm__ volatile ("sfence":::"memory")
#else
    STATIC_ASSERT(false, "Fences not defined for current architecture!")
#endif

#endif // header guard
