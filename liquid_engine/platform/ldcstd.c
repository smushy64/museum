// * Description:  Subset of platform functions implemented with c std
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: September 03, 2023
#include "ldplatform.h"
#include "core/ldmemory.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

// TODO(alicia): log errors!

void* platform_stdout_handle() {
    return stdout;
}
void* platform_stderr_handle() {
    return stderr;
}
void platform_write_console(
    void* output_handle,
    usize write_count,
    const char* buffer
) {
    fwrite( buffer, sizeof(char), write_count, output_handle );
}

PlatformFile* platform_file_open(
    const char* path, PlatformFileOpenFlag flags
) {
    const char* mode = "";

    b32 read     = CHECK_BITS( flags, PLATFORM_FILE_OPEN_READ );
    b32 write    = CHECK_BITS( flags, PLATFORM_FILE_OPEN_WRITE );
    b32 existing = CHECK_BITS( flags, PLATFORM_FILE_OPEN_EXISTING );

    if( read && write ) {
        if( existing ) {
            mode = "rb+";
        } else {
            mode = "wb+";
        }
    } else if( read ) {
        mode = "rb";
    } else if( write ) {
        if( existing ) {
            mode = "wb";
        } else {
            mode = "rb";
        }
    } else {
        return NULL;
    }

    FILE* file = fopen( path, mode );

    if( !read && write && !existing ) {
        if( !file ) {
            return file;
        }

        fclose( file );
        mode = "wb";
        file = fopen( path, mode );
    }
    return file;
}
void platform_file_close( PlatformFile* file_handle ) {
    FILE* file = file_handle;
    fclose( file );
}
b32 platform_file_read(
    PlatformFile* file,
    usize read_size,
    usize buffer_size,
    void* buffer
) {
    if( read_size > buffer_size ) {
        return false;
    }

    usize bytes_read = fread( buffer, sizeof(u8), read_size, file );
    if( bytes_read != read_size ) {
        return false;
    }

    return true;
}
b32 platform_file_write(
    PlatformFile* file,
    usize write_size,
    usize buffer_size,
    void* buffer
) {
    if( write_size > buffer_size ) {
        return false;
    }

    usize bytes_written =
        fwrite( buffer, sizeof(u8), write_size, file );
    if( bytes_written != write_size ) {
        return false;
    }

    return true;
}
usize platform_file_query_size( PlatformFile* file ) {
    fseeko( file, 0, SEEK_END );
    usize size = ftello( file );
    rewind( file );
    return size;
}
usize platform_file_query_offset( PlatformFile* file ) {
    return ftello( file );
}
b32 platform_file_set_offset( PlatformFile* file, usize offset ) {
    int result = fseeko( file, offset, SEEK_SET );
    if( !result ) {
        return true;
    } else {
        return false;
    }
}

void* platform_heap_alloc( usize size ) {
    void* memory = malloc( size );
    if( memory ) {
        mem_zero( memory, size );
    }
    return memory;
}
void* platform_heap_realloc( void* memory, usize old_size, usize new_size ) {
    void* result = realloc( memory, new_size );
    if( result ) {
        mem_zero( (u8*)result + old_size, new_size - old_size );
    }
    return result;
}
void platform_heap_free( void* memory ) {
    free( memory );
}
void* platform_page_alloc( usize size ) {
    return platform_heap_alloc( size );
}
void platform_page_free( void* memory ) {
    platform_heap_free( memory );
}

// internal void cpuid( int* cpu_info, unsigned int i ) {
//     __asm__ volatile (
//         "cpuid"
//         : "=a" (cpu_info[0]),
//         "=b" (cpu_info[1]),
//         "=c" (cpu_info[2]),
//         "=d" (cpu_info[3])
//         : "a" (i), "c"(0)
//     );
// }
//
// internal void cpuinfo( int info[4], int info_type ) {
//     __cpuid_count( info_type, 0, info[0], info[1], info[2], info[3] );
// }
//
// SystemInfo query_system_info() {
//     SystemInfo result = {};
//     result.logical_processor_count = sysconf( _SC_NPROCESSORS_ONLN );
//     struct sysinfo sys_info = {};
//     sysinfo( &sys_info );
//     result.total_memory = sys_info.totalram;
//
// #if defined(LD_ARCH_X86)
//     int info[4];
//     cpuid( info, 0 );
//     int n_ids = info[0];
//
//     if( n_ids >= 0x00000001 ) {
//         cpuid( info, 0x00000001 );
//         if( (info[3] & ((int)1 << 25)) != 0 ) {
//             result.features |= SSE_MASK;
//         }
//         if( (info[3] & ((int)1 << 26)) != 0 ) {
//             result.features |= SSE2_MASK;
//         }
//         if( (info[2] & ((int)1 << 0)) != 0 ) {
//             result.features |= SSE3_MASK;
//         }
//         if( (info[2] & ((int)1 << 9)) != 0 ) {
//             result.features |= SSSE3_MASK;
//         }
//         if( (info[2] & ((int)1 << 19)) != 0 ) {
//             result.features |= SSE4_1_MASK;
//         }
//         if( (info[2] & ((int)1 << 20)) != 0 ) {
//             result.features |= SSE4_2_MASK;
//         }
//         if( (info[2] & ((int)1 << 28)) != 0 ) {
//             result.features |= AVX_MASK;
//         }
//     }
//     if( n_ids >= 0x00000007 ) {
//         cpuid( info, 0x00000007 );
//         if( (info[1] & ((int)1 << 5)) != 0 ) {
//             result.features |= AVX2_MASK;
//         }
//     }
//
//     mem_set(
//         ' ',
//         CPU_NAME_BUFFER_SIZE,
//         result.cpu_name_buffer
//     );
//     result.cpu_name_buffer[CPU_NAME_BUFFER_SIZE - 1] = 0;
//
//     int cpu_info[4];
//     cpuid( cpu_info, 0x80000002 );
//     mem_copy(
//         result.cpu_name_buffer,
//         cpu_info,
//         sizeof(cpu_info)
//     );
//     cpuid( cpu_info, 0x80000003 );
//     mem_copy(
//         result.cpu_name_buffer + 16,
//         cpu_info,
//         sizeof(cpu_info)
//     );
//     cpuid( cpu_info, 0x80000004 );
//     mem_copy(
//         result.cpu_name_buffer + 32,
//         cpu_info,
//         sizeof(cpu_info)
//     );
//
//     StringView cpu_name = result.cpu_name_buffer;
//     string_trim_trailing_whitespace( cpu_name );
// #endif
//
//     return result;
// }
