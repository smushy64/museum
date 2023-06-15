/**
 * Description:  Application Implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "application.h"
#include "platform/platform.h"
#include "platform/io.h"
#include "platform/threading.h"
#include "renderer/renderer.h"
#include "threading.h"
#include "events.h"
#include "logging.h"
#include "input.h"
#include "time.h"
#include "memory.h"
#include "string.h"
#include "math/functions.h"

// TODO(alicia): custom string formatting!
#include <stdio.h>

struct ThreadInfo {
    struct ThreadHandle*    thread_handle;
    struct ThreadWorkQueue* work_queue;
    u32 thread_index;
};
struct ThreadWorkQueue {
    ThreadInfo*      threads;
    ThreadWorkEntry* work_entries;
    SemaphoreHandle  wake_semaphore;

    u32              work_entry_count;
    u32              thread_count;

    volatile u32     push_entry;
    volatile u32     read_entry;
    volatile u32     entry_completion_count;
    volatile u32     pending_work_count;
};

#define SURFACE_TITLE_BUFFER_PADDING 32
struct AppContext {
    Platform        platform;
    SystemInfo      sysinfo;
    ThreadWorkQueue thread_work_queue;
    ThreadHandle*   thread_handles;
    u32             thread_count;

    b32 is_running;
    RendererBackend renderer_backend;

    struct {
        f32 delta_time;
        f32 elapsed_time;
        u64 frame_count;
    } time;

    struct {
        CursorStyle style;
        b32 visible;
        b32 locked;
    } cursor_state;

    AppRunFn application_run;
    void*    application_params;
    RendererContext* renderer_context;

    char* surface_title_buffer;
    u32 surface_title_buffer_size;
    u32 surface_title_writable_offset;

    b32 pause_on_surface_inactive;
};

global AppContext CONTEXT = {};

EventCallbackReturnCode on_app_exit( Event*, void* ) {
    CONTEXT.is_running = false;
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackReturnCode on_destroy( Event*, void* ) {
    CONTEXT.is_running = false;
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackReturnCode on_active( Event* event, void* ) {
    b32 is_active = event->data.surface_active.is_active;
    if( is_active ) {
        LOG_NOTE("Surface activated.");
    } else {
        LOG_NOTE("Surface deactivated.");
    }
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackReturnCode on_resize( Event* event, void* app_ctx ) {
    AppContext* ctx = (AppContext*)app_ctx;
    renderer_on_resize(
        ctx->renderer_context,
        event->data.surface_resize.width,
        event->data.surface_resize.height
    );
    return EVENT_CALLBACK_NOT_CONSUMED;
}

EventCallbackReturnCode on_f4( Event* event, void* ) {
    if( event->data.keyboard.code == KEY_F4 ) {
        if(
            input_is_key_down( KEY_ALT_LEFT ) ||
            input_is_key_down( KEY_ALT_RIGHT )
        ) {
            Event event = {};
            event.code = EVENT_CODE_APP_EXIT;
            event_fire( event );
        }
    }
    return EVENT_CALLBACK_NOT_CONSUMED;
}

internal ThreadReturnCode thread_proc( void* user_params );

b32 app_init( AppConfig* config ) {

#if defined(LD_LOGGING)
    if( !log_init( config->log_level ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize logging subsystem"
        );
        return false;
    }
#endif

    SM_ASSERT(config->application_run);
    CONTEXT.application_run    = config->application_run;
    CONTEXT.application_params = config->application_params;

    LOG_INFO("Liquid Engine Version: %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    if( !platform_init(
        config->opt_surface_icon_path,
        config->surface.dimensions,
        config->platform_flags,
        &CONTEXT.platform
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services."
        );
        return false;
    }

    CONTEXT.pause_on_surface_inactive = ARE_BITS_SET(
        config->platform_flags,
        PLATFORM_PAUSE_ON_SURFACE_INACTIVE
    );

    CONTEXT.renderer_backend = config->renderer_backend;
    surface_set_name( config->surface.name );
    CONTEXT.renderer_context = renderer_init(
        config->surface.name,
        config->renderer_backend,
        &CONTEXT.platform
    );
    if( !CONTEXT.renderer_context ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize rendering subsystem."
        );
        return false;
    }

    CONTEXT.sysinfo = query_system_info();

    u32 thread_count = CONTEXT.sysinfo.logical_processor_count;
    thread_count = (thread_count == 1 ? thread_count : thread_count - 1);

    CONTEXT.thread_work_queue.threads = (ThreadInfo*)mem_alloc(
        sizeof(ThreadInfo) * thread_count,
        MEMTYPE_THREADING
    );
    CONTEXT.thread_work_queue.work_entries = (ThreadWorkEntry*)mem_alloc(
        sizeof(ThreadWorkEntry) * THREAD_WORK_ENTRY_COUNT,
        MEMTYPE_THREADING
    );
    CONTEXT.thread_handles = (ThreadHandle*)mem_alloc(
        sizeof( ThreadHandle ) * thread_count,
        MEMTYPE_THREADING
    );
    if(
        !CONTEXT.thread_work_queue.threads ||
        !CONTEXT.thread_work_queue.work_entries ||
        !CONTEXT.thread_handles
    ) {
        MESSAGE_BOX_FATAL(
            "Subsytem Failure - Out of Memory",
            "Failed to allocate memory for worker threads"
        );
        return false;
    }
    CONTEXT.thread_work_queue.work_entry_count = THREAD_WORK_ENTRY_COUNT;

    if(!semaphore_create(
        0,
        thread_count,
        &CONTEXT.thread_work_queue.wake_semaphore
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to create wake semaphore!"
        );
        return false;
    }

    read_write_fence();
    for( u32 i = 0; i < thread_count; ++i ) {
        ThreadInfo* current_thread_info =
            &CONTEXT.thread_work_queue.threads[i];
        current_thread_info->work_queue    = &CONTEXT.thread_work_queue;
        current_thread_info->thread_handle = &CONTEXT.thread_handles[i];
        current_thread_info->thread_index  = i;

        if(!platform_thread_create(
            &CONTEXT.platform,
            thread_proc,
            current_thread_info,
            THREAD_STACK_SIZE_SAME_AS_MAIN,
            false,
            &CONTEXT.thread_handles[i]
        )) {
            if( i == 0 ) {
                thread_count = 0;
            } else {
                thread_count = i - 1;
            }
            break;
        }
    }

    if( !thread_count ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to create any threads!"
        );
        return false;
    }
    LOG_INFO( "Instantiated %llu threads.", thread_count );

    read_write_fence();

    for( u32 i = 0; i < thread_count; ++i ) {
        platform_thread_resume(
            &CONTEXT.thread_handles[i]
        );
    }

    CONTEXT.thread_count                   = thread_count;
    CONTEXT.thread_work_queue.thread_count = thread_count;

    LOG_NOTE("CPU: %s", CONTEXT.sysinfo.cpu_name_buffer);
    LOG_NOTE("  Logical Processors: %llu",
        CONTEXT.sysinfo.logical_processor_count
    );

#if defined(SM_ARCH_X86)
    b32 sse = system_is_sse_available( CONTEXT.sysinfo.features );
    if( SM_SIMD_WIDTH == 4 && !sse ) {
        local const usize ERROR_MESSAGE_SIZE = 0xFF;
        char error_message_buffer[ERROR_MESSAGE_SIZE];
        snprintf(
            error_message_buffer,
            ERROR_MESSAGE_SIZE,
            "Your CPU does not support SSE instructions!\n"
            "Missing instructions: %s%s%s%s%s%s",
            ARE_BITS_SET(CONTEXT.sysinfo.features, SSE_MASK)    ? "" : "SSE, ",
            ARE_BITS_SET(CONTEXT.sysinfo.features, SSE2_MASK)   ? "" : "SSE2, ",
            ARE_BITS_SET(CONTEXT.sysinfo.features, SSE3_MASK)   ? "" : "SSE3, ",
            ARE_BITS_SET(CONTEXT.sysinfo.features, SSSE3_MASK)  ? "" : "SSSE3, ",
            ARE_BITS_SET(CONTEXT.sysinfo.features, SSE4_1_MASK) ? "" : "SSE4.1, ",
            ARE_BITS_SET(CONTEXT.sysinfo.features, SSE4_2_MASK) ? "" : "SSE4.2"
        );
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            error_message_buffer
        );
        return false;
    }

    b32 avx  = system_is_avx_available( CONTEXT.sysinfo.features );
    b32 avx2 = system_is_avx2_available( CONTEXT.sysinfo.features );

    if( SM_SIMD_WIDTH == 8 && !(avx && avx2) ) {
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            "Your CPU does not support AVX/AVX2 instructions! "
            "This program requires them!"
        );
        return false;
    }

    b32 avx512 = system_is_avx512_available( CONTEXT.sysinfo.features );

    LOG_NOTE(
        "  Features: %s%s%s%s",
        sse    ? "SSE1-4 " : "",
        avx    ? "AVX " : "",
        avx2   ? "AVX2 " : "",
        avx512 ? "AVX-512 " : ""
    );

    LOG_NOTE("Memory: %6.3f GB",
        MB_TO_GB(
            KB_TO_MB(
                BYTES_TO_KB(
                    CONTEXT.sysinfo.total_memory
                )
            )
        )
    );
#endif

    if( !event_init() ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if( !input_init( &CONTEXT.platform ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize input subsystem."
        );
        return false;
    }

    if(!event_subscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_destroy,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_active,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_resize,
        &CONTEXT
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_INPUT_KEY,
        on_f4,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_APP_EXIT,
        on_app_exit,
        nullptr
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem."
        );
        return false;
    }

    CONTEXT.is_running = true;

#if defined(LD_LOGGING) && defined(LD_PROFILING)
    LOG_NOTE("Initial Memory Usage:");
    for( u64 i = 0; i < MEMTYPE_COUNT; ++i ) {
        MemoryType type = (MemoryType)i;
        usize memory_usage = query_memory_usage( type );
        char usage_buffer[32];
        format_bytes(
            memory_usage,
            usage_buffer,
            32
        );
        LOG_NOTE("    %-30s %s", to_string(type), usage_buffer);
    }
    usize total_memory_usage = query_total_memory_usage();
    char usage_buffer[32];
    format_bytes(
        total_memory_usage,
        usage_buffer,
        32
    );
    LOG_NOTE("    %-30s %s", "Total Memory Usage", usage_buffer);
#endif

    CONTEXT.cursor_state.style   = CURSOR_ARROW;
    CONTEXT.cursor_state.visible = true;

    return true;
}
b32 app_run() {

    #define UPDATE_FRAME_RATE_COUNTER_RATE 100

    while( CONTEXT.is_running ) {
        input_swap();
        platform_poll_gamepad( &CONTEXT.platform );
        platform_pump_events( &CONTEXT.platform );

        if(
            !CONTEXT.platform.is_active &&
            CONTEXT.pause_on_surface_inactive
        ) {
            continue;
        }

        if( CONTEXT.cursor_state.locked ) {
            platform_cursor_center( &CONTEXT.platform );
        }

        f64 seconds_elapsed = platform_read_seconds_elapsed(
            &CONTEXT.platform
        );
        CONTEXT.time.delta_time   = seconds_elapsed - CONTEXT.time.elapsed_time;
        CONTEXT.time.elapsed_time = seconds_elapsed;

        RenderOrder draw_order = {};
        draw_order.delta_time  = CONTEXT.time.delta_time;
        if(!CONTEXT.application_run(
            &CONTEXT.thread_work_queue,
            &draw_order,
            CONTEXT.time.delta_time,
            CONTEXT.application_params
        )) {
            return false;
        }
        
        if( !renderer_draw_frame(
            CONTEXT.renderer_context,
            &draw_order
        ) ) {
            MESSAGE_BOX_FATAL(
                "Renderer Failure",
                "Unknown Error!\n"
                LD_CONTACT_MESSAGE
            );
            return false;
        }

        if( (CONTEXT.time.frame_count + 1) % UPDATE_FRAME_RATE_COUNTER_RATE == 0 ) {
            f32 fps = CONTEXT.time.delta_time == 0.0f ? 0.0f : 1.0f / CONTEXT.time.delta_time;

            snprintf(
                CONTEXT.surface_title_buffer + CONTEXT.surface_title_writable_offset - 1,
                CONTEXT.surface_title_buffer_size - CONTEXT.surface_title_writable_offset,
                " | %.1f FPS",
                fps
            );
            platform_surface_set_name(
                &CONTEXT.platform,
                CONTEXT.surface_title_buffer_size,
                CONTEXT.surface_title_buffer
            );
        }


        CONTEXT.time.frame_count++;
    }

    return true;
}
void app_shutdown() {
    event_unsubscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_destroy,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_active,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_resize,
        &CONTEXT
    );
    event_unsubscribe(
        EVENT_CODE_INPUT_KEY,
        on_f4,
        nullptr
    );
    event_unsubscribe(
        EVENT_CODE_APP_EXIT,
        on_app_exit,
        nullptr
    );

    CONTEXT.is_running = false;
    event_shutdown();
    input_shutdown();

    semaphore_destroy(
        &CONTEXT.thread_work_queue.wake_semaphore
    );
    mem_free( CONTEXT.thread_handles );
    mem_free( CONTEXT.thread_work_queue.threads );
    mem_free( CONTEXT.thread_work_queue.work_entries );

    renderer_shutdown( CONTEXT.renderer_context );
    platform_shutdown( &CONTEXT.platform );
    log_shutdown();
    platform_exit();
}
void thread_work_queue_push(
    ThreadWorkQueue* work_queue,
    ThreadWorkEntry work_entry
) {
    work_queue->work_entries[work_queue->push_entry] =
        work_entry;

    read_write_fence();

    work_queue->push_entry = platform_interlocked_increment(
        &work_queue->push_entry
    ) % work_queue->work_entry_count;

    work_queue->pending_work_count = platform_interlocked_increment(
        &work_queue->pending_work_count
    );

    LOG_ASSERT(
        work_queue->pending_work_count < work_queue->work_entry_count,
        "Exceeded thread work entry count!!"
    );

    semaphore_increment(
        &work_queue->wake_semaphore,
        1, nullptr
    );
}
internal b32 thread_work_queue_pop(
    ThreadWorkQueue* work_queue,
    ThreadWorkEntry* out_work_entry
) {
    if(
        work_queue->push_entry ==
        work_queue->read_entry
    ) {
        return false;
    }

    *out_work_entry = work_queue->work_entries[work_queue->read_entry];

    read_write_fence();

    work_queue->read_entry = platform_interlocked_increment(
        &work_queue->read_entry
    ) % work_queue->work_entry_count;

    return true;
}
internal ThreadReturnCode thread_proc( void* user_params ) {
    ThreadInfo* thread_info = (ThreadInfo*)user_params;

    ThreadWorkEntry entry = {};
    loop {
        semaphore_wait(
            &thread_info->work_queue->wake_semaphore,
            true, 0
        );

        if( thread_work_queue_pop(
            thread_info->work_queue,
            &entry
        ) ) {
            entry.thread_work_proc(
                thread_info,
                entry.thread_work_user_params
            );

            read_write_fence();

            platform_interlocked_increment(
                &thread_info->work_queue->entry_completion_count
            );
            platform_interlocked_decrement(
                &thread_info->work_queue->pending_work_count
            );
        }
    }

    return 0;
}

void cursor_set_style( CursorStyle style ) {
    platform_cursor_set_style(
        &CONTEXT.platform,
        style
    );
}
void cursor_set_visibility( b32 visible ) {
    platform_cursor_set_visible(
        &CONTEXT.platform,
        visible
    );
}
void cursor_set_locked( b32 locked ) {
    CONTEXT.cursor_state.locked = locked;
    if( locked ) {
        CONTEXT.cursor_state.visible = false;
    }
}
void cursor_center() {
    platform_cursor_center( &CONTEXT.platform );
}
CursorStyle cursor_query_style() {
    return CONTEXT.cursor_state.style;
}
b32 cursor_query_visibility() {
    return CONTEXT.cursor_state.visible;
}
b32 cursor_query_locked() {
    return CONTEXT.cursor_state.locked;
}
void surface_set_name( const char* name ) {

    #define FATAL_MESSAGE()\
        LOG_FATAL( "Unable to allocate surface title buffer!" );\
        MESSAGE_BOX_FATAL(\
            "Out of Memory",\
            "No memory available for window title buffer!\n"\
            LD_CONTACT_MESSAGE\
        );

    usize name_length = str_length( name );
    usize required_buffer_size = name_length;
    const char* renderer_backend_name = to_string(
        CONTEXT.renderer_backend
    );
    required_buffer_size += str_length( renderer_backend_name );
    required_buffer_size += SURFACE_TITLE_BUFFER_PADDING;

    if( !CONTEXT.surface_title_buffer ) {
        void* surface_title_buffer = mem_alloc(
            required_buffer_size,
            MEMTYPE_APPLICATION
        );
        if( !surface_title_buffer ) {
            FATAL_MESSAGE();
            SM_PANIC();
            return;
        }
        CONTEXT.surface_title_buffer      = (char*)surface_title_buffer;
        CONTEXT.surface_title_buffer_size = required_buffer_size;
    } else if( required_buffer_size >= CONTEXT.surface_title_buffer_size ) {
        void* surface_title_buffer = mem_realloc(
            CONTEXT.surface_title_buffer,
            required_buffer_size
        );
        if( !surface_title_buffer ) {
            FATAL_MESSAGE();
            SM_PANIC();
            return;
        }
        CONTEXT.surface_title_buffer = (char*)surface_title_buffer;
        CONTEXT.surface_title_buffer_size = required_buffer_size;
    }

    snprintf(
        CONTEXT.surface_title_buffer,
        CONTEXT.surface_title_buffer_size,
        "%s | %s",
        name,
        renderer_backend_name
    );

    CONTEXT.surface_title_writable_offset = str_length(
        CONTEXT.surface_title_buffer
    );

    platform_surface_set_name(
        &CONTEXT.platform,
        CONTEXT.surface_title_buffer_size,
        CONTEXT.surface_title_buffer
    );
}
u32 thread_info_read_index( struct ThreadInfo* thread_info ) {
    return thread_info->thread_index;
}

SystemInfo* system_info_read() {
    return &CONTEXT.sysinfo;
}
b32 system_is_sse_available( ProcessorFeatures features ) {
    return ARE_BITS_SET(
        features,
        SSE_MASK | SSE2_MASK | SSE3_MASK | SSE4_1_MASK | SSE4_2_MASK | SSSE3_MASK
    );
}
b32 system_is_avx_available( ProcessorFeatures features ) {
    return ARE_BITS_SET( features, AVX_MASK );
}
b32 system_is_avx2_available( ProcessorFeatures features ) {
    return ARE_BITS_SET( features, AVX2_MASK );
}
b32 system_is_avx512_available( ProcessorFeatures features ) {
    return ARE_BITS_SET( features, AVX512_MASK );
}
