// * Description:  Engine Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 18, 2023
#include "engine.h"
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
#include "time.h"
#include "math.h"

// TODO(alicia): custom string formatting!
#include <stdio.h>

#define THREAD_WORK_ENTRY_COUNT 256
struct ThreadInfo {
    struct ThreadHandle*    thread_handle;
    struct ThreadWorkQueue* work_queue;
    u32 thread_index;
};
struct ThreadWorkQueue {
    ThreadInfo*      threads;
    ThreadWorkEntry* work_entries;
    SemaphoreHandle  wake_semaphore;

    u32 work_entry_count;
    u32 thread_count;

    volatile u32 push_entry;
    volatile u32 read_entry;
    volatile u32 entry_completion_count;
    volatile u32 pending_work_count;
};

#define SURFACE_TITLE_BUFFER_PADDING 32
struct EngineContext {
    SystemInfo       system_info;       // 88 
    ThreadWorkQueue  thread_work_queue; // 48
    Platform         platform;          // 32
    Time             time;              // 16
    RendererContext* renderer_context;  // 8

    char* application_title_buffer;

    ThreadHandle* thread_handles; // 8
    u32 thread_count; // 4
    RendererBackend renderer_backend; // 4
    
    u32 application_title_buffer_size; // 4
    u32 application_title_buffer_writable_offset; // 4
    
    CursorStyle cursor_style;

    b8 cursor_is_visible; // 1
    b8 cursor_is_locked;  // 1
    b8 is_running; // 1
    b8 pause_on_surface_inactive; // 1
};

EventCallbackReturnCode on_app_exit( Event*, void* void_ctx ) {
    EngineContext* ctx = (EngineContext*)void_ctx;
    ctx->is_running    = false;
    return EVENT_CALLBACK_CONSUMED;
}

EventCallbackReturnCode on_destroy( Event*, void* void_ctx ) {
    EngineContext* ctx = (EngineContext*)void_ctx;
    ctx->is_running = false;
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

EventCallbackReturnCode on_resize( Event* event, void* void_ctx ) {
    EngineContext* ctx = (EngineContext*)void_ctx;
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

b32 engine_run(
    int argc, char** argv,
    ApplicationRunFN application_run,
    void* application_run_user_params,
    EngineConfig* config
) {
    // TODO(alicia): parse arguments!
    SM_UNUSED(argc);
    SM_UNUSED(argv);

#if defined(LD_LOGGING)
    if( !log_init( config->log_level ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize logging subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
#endif

    SM_ASSERT( application_run );
    LOG_INFO("Liquid Engine Version: %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    EngineContext ctx = {};

    if( !platform_init(
        config->opt_application_icon_path,
        { config->surface_dimensions.width, config->surface_dimensions.height },
        config->platform_flags,
        &ctx.platform
    ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize platform services!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    ctx.pause_on_surface_inactive = ARE_BITS_SET(
        config->platform_flags,
        PLATFORM_PAUSE_ON_SURFACE_INACTIVE
    );
    ctx.renderer_backend = config->renderer_backend;
    engine_set_application_name( &ctx, config->application_name );
    ctx.renderer_context = renderer_init(
        config->application_name,
        config->renderer_backend,
        &ctx.platform
    );
    if( !ctx.renderer_context ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize rendering subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    ctx.system_info = query_system_info();

    u32 thread_count = ctx.system_info.logical_processor_count;
    thread_count = (thread_count == 1 ? thread_count : thread_count - 1);

    ctx.thread_work_queue.threads = (ThreadInfo*)mem_alloc(
        sizeof(ThreadInfo) * thread_count,
        MEMTYPE_THREADING
    );
    ctx.thread_work_queue.work_entries = (ThreadWorkEntry*)mem_alloc(
        sizeof(ThreadWorkEntry) * THREAD_WORK_ENTRY_COUNT,
        MEMTYPE_THREADING
    );
    ctx.thread_handles = (ThreadHandle*)mem_alloc(
        sizeof( ThreadHandle ) * thread_count,
        MEMTYPE_THREADING
    );
    if(
        !ctx.thread_work_queue.threads ||
        !ctx.thread_work_queue.work_entries ||
        !ctx.thread_handles
    ) {
        MESSAGE_BOX_FATAL(
            "Subsytem Failure - Out of Memory",
            "Failed to allocate memory for worker threads!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    ctx.thread_work_queue.work_entry_count = THREAD_WORK_ENTRY_COUNT;

    if(!semaphore_create(
        0,
        thread_count,
        &ctx.thread_work_queue.wake_semaphore
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to create wake semaphore!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    read_write_fence();
    for( u32 i = 0; i < thread_count; ++i ) {
        ThreadInfo* current_thread_info =
            &ctx.thread_work_queue.threads[i];
        current_thread_info->work_queue    = &ctx.thread_work_queue;
        current_thread_info->thread_handle = &ctx.thread_handles[i];
        current_thread_info->thread_index  = i;

        if(!platform_thread_create(
            &ctx.platform,
            thread_proc,
            current_thread_info,
            THREAD_STACK_SIZE_SAME_AS_MAIN,
            false,
            &ctx.thread_handles[i]
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
            "Failed to create any threads!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    LOG_NOTE( "Instantiated %llu threads.", thread_count );

    read_write_fence();

    for( u32 i = 0; i < thread_count; ++i ) {
        platform_thread_resume(
            &ctx.thread_handles[i]
        );
    }

    ctx.thread_count                   = thread_count;
    ctx.thread_work_queue.thread_count = thread_count;

    LOG_NOTE("CPU: %s", ctx.system_info.cpu_name_buffer);
    LOG_NOTE("  Logical Processors: %llu",
        ctx.system_info.logical_processor_count
    );

#if defined(SM_ARCH_X86)
    b32 sse  = engine_query_is_sse_available( &ctx );
    b32 avx  = engine_query_is_avx_available( &ctx );
    b32 avx2 = engine_query_is_avx2_available( &ctx );
    b32 avx512 = engine_query_is_avx512_available( &ctx );
    ProcessorFeatures features = ctx.system_info.features;
    if( SM_SIMD_WIDTH == 4 && !sse ) {
        local const usize ERROR_MESSAGE_SIZE = 0xFF;
        char error_message_buffer[ERROR_MESSAGE_SIZE];
        snprintf( 
            error_message_buffer,
            ERROR_MESSAGE_SIZE,
            "Your CPU does not support SSE instructions!\n"
            "Missing instructions: %s%s%s%s%s%s",
            ARE_BITS_SET(features, SSE_MASK)    ? "" : "SSE, ",
            ARE_BITS_SET(features, SSE2_MASK)   ? "" : "SSE2, ",
            ARE_BITS_SET(features, SSE3_MASK)   ? "" : "SSE3, ",
            ARE_BITS_SET(features, SSSE3_MASK)  ? "" : "SSSE3, ",
            ARE_BITS_SET(features, SSE4_1_MASK) ? "" : "SSE4.1, ",
            ARE_BITS_SET(features, SSE4_2_MASK) ? "" : "SSE4.2"
        );
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            error_message_buffer
        );
        return false;
    }

    if( SM_SIMD_WIDTH == 8 && !(avx && avx2) ) {
        MESSAGE_BOX_FATAL(
            "Missing instructions.",
            "Your CPU does not support AVX/AVX2 instructions! "
            "This program requires them!"
        );
        return false;
    }


    LOG_NOTE(
        "  Features: %s%s%s%s",
        sse    ? "SSE1-4 " : "",
        avx    ? "AVX " : "",
        avx2   ? "AVX2 " : "",
        avx512 ? "AVX-512 " : ""
    );

    LOG_NOTE("Memory: %6.3f GB",
        MB_TO_GB( KB_TO_MB( BYTES_TO_KB( ctx.system_info.total_memory ) ) )
    );
#endif

    if( !event_init() ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if( !input_init( &ctx.platform ) ) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize input subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }

    if(!event_subscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_destroy,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_active,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_resize,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_INPUT_KEY,
        on_f4,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    if(!event_subscribe(
        EVENT_CODE_APP_EXIT,
        on_app_exit,
        &ctx
    )) {
        MESSAGE_BOX_FATAL(
            "Subsystem Failure",
            "Failed to initialize event subsystem!\n "
            LD_CONTACT_MESSAGE
        );
        return false;
    }
    ctx.is_running = true;

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

    ctx.cursor_style      = CURSOR_ARROW;
    ctx.cursor_is_visible = true;

    #define UPDATE_FRAME_RATE_COUNTER_RATE 100

    while( ctx.is_running ) {
        input_swap();
        platform_poll_gamepad( &ctx.platform );
        platform_pump_events( &ctx.platform );

        if(
            !ctx.platform.is_active &&
            ctx.pause_on_surface_inactive
        ) {
            continue;
        }

        if( ctx.cursor_is_locked ) {
            platform_cursor_center( &ctx.platform );
        }

        f64 seconds_elapsed = platform_read_seconds_elapsed(
            &ctx.platform
        );
        ctx.time.delta_seconds =
            seconds_elapsed - ctx.time.elapsed_seconds;
        ctx.time.elapsed_seconds = seconds_elapsed;

        RenderOrder draw_order = {};
        draw_order.time  = &ctx.time;
        if( !application_run(
            &ctx,
            &ctx.thread_work_queue,
            &draw_order,
            &ctx.time,
            application_run_user_params
        ) ) {
            return false;
        }
        
        if( !renderer_draw_frame(
            ctx.renderer_context,
            &draw_order
        ) ) {
            MESSAGE_BOX_FATAL(
                "Renderer Failure",
                "Unknown Error!\n"
                LD_CONTACT_MESSAGE
            );
            return false;
        }

        if( (ctx.time.frame_count + 1) % UPDATE_FRAME_RATE_COUNTER_RATE == 0 ) {
            f32 fps = ctx.time.delta_seconds == 0.0f ?
                0.0f : 1.0f / ctx.time.delta_seconds;
            char* stream = ctx.application_title_buffer +
                ctx.application_title_buffer_writable_offset - 1;
            size_t n = ctx.application_title_buffer_size -
                ctx.application_title_buffer_writable_offset;

            snprintf(
                stream, n,
                " | %.1f FPS",
                fps
            );
            platform_surface_set_name(
                &ctx.platform,
                ctx.application_title_buffer_size,
                ctx.application_title_buffer
            );
        }


        ctx.time.frame_count++;
    }

    event_unsubscribe(
        EVENT_CODE_SURFACE_DESTROY,
        on_destroy,
        &ctx
    );
    event_unsubscribe(
        EVENT_CODE_SURFACE_ACTIVE,
        on_active,
        &ctx
    );
    event_unsubscribe(
        EVENT_CODE_SURFACE_RESIZE,
        on_resize,
        &ctx
    );
    event_unsubscribe(
        EVENT_CODE_INPUT_KEY,
        on_f4,
        &ctx
    );
    event_unsubscribe(
        EVENT_CODE_APP_EXIT,
        on_app_exit,
        &ctx
    );

    ctx.is_running = false; 
    event_shutdown();
    input_shutdown();

    semaphore_destroy(
        &ctx.thread_work_queue.wake_semaphore 
    );
    mem_free( ctx.thread_handles ); 
    mem_free( ctx.thread_work_queue.threads ); 
    mem_free( ctx.thread_work_queue.work_entries ); 

    renderer_shutdown( ctx.renderer_context ); 
    platform_shutdown( &ctx.platform ); 
    log_shutdown();
    platform_exit();

    return true;
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

void engine_set_cursor_style( struct EngineContext* ctx, u32 style ) {
    ctx->cursor_style = (CursorStyle)style;
    platform_cursor_set_style(
        &ctx->platform,
        (CursorStyle)style
    );
}
void engine_set_cursor_visibility( struct EngineContext* ctx, b32 visible ) {
    ctx->cursor_is_visible = visible;
    platform_cursor_set_visible(
        &ctx->platform,
        visible
    );
}
void engine_center_cursor( struct EngineContext* ctx ) {
    platform_cursor_center( &ctx->platform );
}
void engine_lock_cursor( struct EngineContext* ctx, b32 locked ) {
    ctx->cursor_is_locked = locked;
    if( locked ) {
        ctx->cursor_is_visible = false;
    }
}
u32 engine_query_cursor_style( struct EngineContext* ctx ) {
    return (u32)ctx->cursor_style;
}
b32 engine_query_cursor_visibility( struct EngineContext* ctx ) {
    return ctx->cursor_is_visible;
}
b32 engine_query_cursor_locked( struct EngineContext* ctx ) {
    return ctx->cursor_is_locked;
}
void engine_set_application_name( struct EngineContext* ctx, const char* name ) {
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
        ctx->renderer_backend
    );
    required_buffer_size += str_length( renderer_backend_name );
    required_buffer_size += SURFACE_TITLE_BUFFER_PADDING;

    if( !ctx->application_title_buffer ) {
        void* surface_title_buffer = mem_alloc(
            required_buffer_size,
            MEMTYPE_APPLICATION
        );
        if( !surface_title_buffer ) {
            FATAL_MESSAGE();
            SM_PANIC();
            return;
        }
        ctx->application_title_buffer      = (char*)surface_title_buffer;
        ctx->application_title_buffer_size = required_buffer_size;
    } else if( required_buffer_size >= ctx->application_title_buffer_size ) {
        void* surface_title_buffer = mem_realloc(
            ctx->application_title_buffer,
            required_buffer_size
        );
        if( !surface_title_buffer ) {
            FATAL_MESSAGE();
            SM_PANIC();
            return;
        }
        ctx->application_title_buffer = (char*)surface_title_buffer;
        ctx->application_title_buffer_size = required_buffer_size;
    }

    snprintf(
        ctx->application_title_buffer,
        ctx->application_title_buffer_size,
        "%s | %s",
        name,
        renderer_backend_name
    );

    ctx->application_title_buffer_writable_offset = str_length(
        ctx->application_title_buffer
    );
    platform_surface_set_name(
        &ctx->platform,
        ctx->application_title_buffer_size,
        ctx->application_title_buffer
    );
}
const char* engine_query_application_name( struct EngineContext* ctx ) {
    return ctx->application_title_buffer;
}
usize engine_query_logical_processor_count( struct EngineContext* ctx ) {
    return ctx->system_info.logical_processor_count;
}
usize engine_query_total_system_memory( struct EngineContext* ctx ) {
    return ctx->system_info.total_memory;
}
const char* engine_query_processor_name( struct EngineContext* ctx ) {
    return ctx->system_info.cpu_name_buffer;
}
b32 engine_query_is_sse_available( struct EngineContext* ctx ) {
    return ARE_BITS_SET(
        ctx->system_info.features,
        SSE_MASK | SSE2_MASK | SSE3_MASK | SSE4_1_MASK | SSE4_2_MASK | SSSE3_MASK
    );
}
b32 engine_query_is_avx_available( struct EngineContext* ctx ) {
    return ARE_BITS_SET( ctx->system_info.features, AVX_MASK );
}
b32 engine_query_is_avx2_available( struct EngineContext* ctx ) {
    return ARE_BITS_SET( ctx->system_info.features, AVX2_MASK );
}
b32 engine_query_is_avx512_available( struct EngineContext* ctx ) {
    return ARE_BITS_SET( ctx->system_info.features, AVX512_MASK );
}

u32 thread_info_read_index( struct ThreadInfo* thread_info ) {
    return thread_info->thread_index;
}

