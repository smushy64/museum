#if !defined(PLATFORM_HPP)
#define PLATFORM_HPP
/**
 * Description:  Operating System related functions
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "defines.h"
#include "core/smath.h"
#include "core/input.h"
#include "core/events.h"

#include "flags.h"

struct PlatformState {
    void* platform_data;
};

SM_API b32 platform_init(
    PlatformInitFlags flags,
    PlatformState* out_state
);
SM_API b32 platform_next_event(
    PlatformState* state,
    Event* out_event
);
SM_API void platform_shutdown(
    PlatformState* state
);

struct Surface {
    union {
        ivec2 position;
        struct { i32 x; i32 y; };
    };
    union {
        ivec2 dimensions;
        struct { i32 width; i32 height; };
    };
    char* name;
    usize name_length;
    void* platform_data;

    b32 is_focused;
    b32 is_visible;
};

SM_API b32 surface_create(
    const char* surface_name,
    ivec2 position,
    ivec2 dimensions,
    SurfaceCreateFlags flags,
    PlatformState* platform_state,
    Surface* opt_parent,
    Surface* out_surface
);
SM_API void surface_destroy(
    Surface* surface
);

SM_API b32 surface_pump_events( Surface* surface );
SM_API void surface_swap_buffers( Surface* surface );
SM_API void surface_set_name( Surface* surface, const char* name );

/// Types of message boxes
enum MessageBoxType : u32 {
    MBTYPE_OK,
    MBTYPE_OKCANCEL,
    MBTYPE_RETRYCANCEL,
    MBTYPE_YESNO,
    MBTYPE_YESNOCANCEL,

    MBTYPE_COUNT
};
inline const char* to_string( MessageBoxType type ) {
    static const char* strings[MBTYPE_COUNT] = {
        "Message Box with OK button.",
        "Message Box with OK and CANCEL buttons.",
        "Message Box with RETRY and CANCEL buttons.",
        "Message Box with YES and NO buttons.",
        "Message Box with YES, NO and CANCEL buttons.",
    };
    if( type >= MBTYPE_COUNT ) {
        return "Unknown Message Box type.";
    }
    return strings[type];
}

/// Message box icons
enum MessageBoxIcon : u32 {
    MBICON_INFORMATION,
    MBICON_WARNING,
    MBICON_ERROR,

    MBICON_COUNT
};
inline const char* to_string( MessageBoxIcon icon ) {
    static const char* strings[MBICON_COUNT] = {
        "Message Box \"information\" icon.",
        "Message Box \"warning\" icon.",
        "Message Box \"error\" icon.",
    };
    if( icon >= MBICON_COUNT ) {
        return "Unknown Message Box icon.";
    }
    return strings[icon];
}

/// User selection from a message box or an error from creating message box.
enum MessageBoxResult : u32 {
    MBRESULT_OK,
    MBRESULT_CANCEL,
    MBRESULT_RETRY,
    MBRESULT_YES,
    MBRESULT_NO,

    MBRESULT_UNKNOWN_ERROR,

    MBRESULT_COUNT
};
inline const char* to_string( MessageBoxResult result ) {
    static const char* strings[MBRESULT_COUNT] = {
        "Message Box OK selected.",
        "Message Box CANCEL selected.",
        "Message Box RETRY selected.",
        "Message Box YES selected.",
        "Message Box NO selected.",
        "An unknown error occurred.",
    };
    if( result >= MBRESULT_COUNT ) {
        return "Unknown Message Box result.";
    }
    return strings[result];
}

/// Create a message box to report urgent information.
SM_API MessageBoxResult message_box(
    const char* window_title,
    const char* message,
    MessageBoxType type,
    MessageBoxIcon icon
);

#define MESSAGE_BOX_FATAL( title, message ) \
    message_box(\
        title,\
        message,\
        MBTYPE_OK,\
        MBICON_ERROR\
    )

/// Put the current thread to sleep for the specified amount of milliseconds.
SM_API void sleep( u32 ms );

typedef u16 ProcessorFeatures;

#define SSE_MASK    (1 << 0)
#define SSE2_MASK   (1 << 1)
#define SSE3_MASK   (1 << 2)
#define SSSE3_MASK  (1 << 3)
#define SSE4_1_MASK (1 << 4)
#define SSE4_2_MASK (1 << 5)
#define AVX_MASK    (1 << 6)
#define AVX2_MASK   (1 << 7)
#define AVX512_MASK (1 << 8)

#define IS_SSE_AVAILABLE(processor_features)\
    ( (processor_features & SSE_MASK) == SSE_MASK )
#define IS_SSE2_AVAILABLE(processor_features)\
    ( (processor_features & SSE2_MASK) == SSE2_MASK )
#define IS_SSE3_AVAILABLE(processor_features)\
    ( (processor_features & SSE3_MASK) == SSE3_MASK )
#define IS_SSSE3_AVAILABLE(processor_features)\
    ( (processor_features & SSSE3_MASK) == SSSE3_MASK )
#define IS_SSE4_1_AVAILABLE(processor_features)\
    ( (processor_features & SSE4_1_MASK) == SSE4_1_MASK )
#define IS_SSE4_2_AVAILABLE(processor_features)\
    ( (processor_features & SSE4_2_MASK) == SSE4_2_MASK )

#define ARE_SSE_INSTRUCTIONS_AVAILABLE(processor_features) (\
    IS_SSE_AVAILABLE(processor_features)    &&\
    IS_SSE2_AVAILABLE(processor_features)   &&\
    IS_SSE3_AVAILABLE(processor_features)   &&\
    IS_SSSE3_AVAILABLE(processor_features)  &&\
    IS_SSE4_1_AVAILABLE(processor_features) &&\
    IS_SSE4_2_AVAILABLE(processor_features)   \
)

#define IS_AVX_AVAILABLE(processor_features)\
    ( (processor_features & AVX_MASK) == AVX_MASK )
#define IS_AVX2_AVAILABLE(processor_features)\
    ( (processor_features & AVX2_MASK) == AVX2_MASK )
#define IS_AVX512_AVAILABLE(processor_features)\
    ( (processor_features & AVX512_MASK) == AVX512_MASK )

#if defined(SM_ARCH_X86)
    #define CPU_NAME_BUFFER_LEN 68
#else
    #error "Architecture is not yet supported!"
#endif

/// System information
struct SystemInfo {
    usize thread_count;
    usize total_memory;
    char  cpu_name_buffer[CPU_NAME_BUFFER_LEN];
    ProcessorFeatures features;
};
/// Get information about the current processor.
SM_API SystemInfo query_system_info();

#if defined(SM_API_INTERNAL)

/// Allocate memory on the heap.
/// All platforms must zero out memory before returning pointer.
void* heap_alloc( usize size );
/// Reallocate memory on the heap.
/// All platforms must zero out new memory before returning pointer.
void* heap_realloc( void* memory, usize new_size );
/// Free heap allocated memory.
void heap_free( void* memory );

/// Page allocate memory.
/// All platforms must zero out memory before returning pointer.
void* page_alloc( usize size );
/// Free page allocated memory.
void page_free( void* memory );

#endif

// TODO(alicia): stop exporting threading functions/types
// when job system is implemented

/// Opaque thread handle.
typedef void* ThreadHandle;

#if defined(SM_PLATFORM_WINDOWS)
    typedef unsigned long ThreadReturnValue;
#else
    typedef int ThreadReThreadReturnValue;
#endif

/// Thread procedure prototype
typedef ThreadReturnValue (*ThreadProc)( void* params );

/// Create a thread.
SM_API ThreadHandle thread_create(
    ThreadProc thread_proc,
    void*      params,
    b32        run_on_creation
);
/// Resume a thread.
SM_API void thread_resume( ThreadHandle thread );

/// read-write fence
SM_API void mem_fence();
/// read fence
SM_API void read_fence();
/// write fence
SM_API void write_fence();

/// Opaque semaphore handle
typedef void* Semaphore;

/// Create a semaphore.
SM_API Semaphore semaphore_create(
    u32 initial_count,
    u32 maximum_count
);
/// Increment a semaphore.
/// Optional: get the semaphore count before incrementing
SM_API void semaphore_increment(
    Semaphore semaphore,
    u32       increment,
    u32*      opt_out_previous_count
);

/// Infinite semaphore timeout
#define TIMEOUT_INFINITE U32::MAX

/// Wait for semaphore to be incremented.
/// Decrements semaphore when it is signaled.
SM_API void semaphore_wait_for(
    Semaphore semaphore,
    u32       timeout_ms
);
/// Wait for multiple semaphores.
/// Set wait for all if waiting for all semaphores.
SM_API void semaphore_wait_for_multiple(
    usize      count,
    Semaphore* semaphores,
    b32        wait_for_all,
    u32        timeout_ms
);
/// Destroy a semaphore handle.
SM_API void semaphore_destroy( Semaphore semaphore );

SM_API u32 interlocked_increment( volatile u32* addend );
SM_API u32 interlocked_decrement( volatile u32* addend );
SM_API u32 interlocked_exchange( volatile u32* target, u32 value );
SM_API void* interlocked_compare_exchange_pointer(
    void* volatile* dst,
    void* exchange,
    void* comperand
);
SM_API u32 interlocked_compare_exchange(
    u32 volatile* dst,
    u32 exchange,
    u32 comperand
);

#endif