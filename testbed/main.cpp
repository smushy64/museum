/**
 * Description:  Testbed Entry Point
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: April 27, 2023
*/
#include "pch.h"
#include <core/logging.h>
#include <core/string.h>
#include <platform/os.h>
#include <platform/memory.h>
#include <platform/threading.h>
#include <stdio.h>

void print_system_info( SystemInfo* info );

ThreadReturnValue test_proc( void* ) {
    for( usize i = 0; i < 10; ++i ) {
        LOG_DEBUG("test proc!");
    }
    return 0;
}

int main( int, char** ) {
    if(!log_init( LOG_LEVEL_ALL_VERBOSE )) {
        return -1;
    }
    PlatformState platform = {};
    if( !platform_init(
        PLATFORM_INIT_DPI_AWARE,
        &platform
    ) ) {
        return -1;
    }
    
    LOG_NOTE("Liquid Engine Version: %i.%i",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR
    );

    SystemInfo sys_info = query_system_info();
    print_system_info( &sys_info );

    Surface surface = {};
    if( !surface_create(
        "Hello World",
        { 800, 600 },
        SURFACE_FLAG_SHOW_ON_CREATE,
        nullptr,
        &surface
    ) ) {
        return -1;
    }

    b32 running = true;
    while( running ) {
        Event event = {};
        while( next_event( surface.handle, &event ) ) {
            isize buffer_size = format_event(
                0, 0, &event
            );
            if( buffer_size ) {
                buffer_size += 1;
                char event_formatted_buffer[buffer_size];
                format_event(
                    buffer_size,
                    event_formatted_buffer,
                    &event
                );
                LOG_NOTE("%s", event_formatted_buffer);
            }

            if( event.type == EVENT_SURFACE_DESTROY ) {
                running = false;
            }
        }
    }

    platform_shutdown( &platform );
    return 0;
}

void print_system_info( SystemInfo* info ) {
    LOG_NOTE("CPU Info:");
    LOG_NOTE("  Vendor:       %s", info->cpu_name_buffer);
    LOG_NOTE("  Thread Count: %llu", info->thread_count);

    b32 sse    = ARE_SSE_INSTRUCTIONS_AVAILABLE(info->features);
    b32 avx    = IS_AVX_AVAILABLE(info->features);
    b32 avx2   = IS_AVX2_AVAILABLE(info->features);
    b32 avx512 = IS_AVX512_AVAILABLE(info->features);
    if( sse ) {
        LOG_NOTE( "  SSE 1-4:      available");
    } else {
        LOG_ERROR("  SSE 1-4:      not available");
    }
    if( avx ) {
        LOG_NOTE( "  AVX:          available");
    } else {
        LOG_WARN( "  AVX:          not available");
    }
    if( avx2 ) {
        LOG_NOTE( "  AVX2:         available");
    } else {
        LOG_WARN( "  AVX2:         not available");
    }
    if( avx512 ) {
        LOG_NOTE( "  AVX-512:      available");
    } else {
        LOG_WARN( "  AVX-512:      not available");
    }

    LOG_NOTE("Memory Info:");
    LOG_NOTE("  Total RAM:             %5.2f GB",
        MB_TO_GB(KB_TO_MB(BYTES_TO_KB(info->total_memory)))
    );

    char heap_usage_buffer[32];
    format_bytes(
        query_heap_memory_usage(),
        heap_usage_buffer,
        32
    );

    LOG_NOTE("  Heap memory allocated: %s", heap_usage_buffer );
}
