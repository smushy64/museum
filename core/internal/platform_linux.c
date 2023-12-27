/**
 * Description:  Core Library Linux implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 23, 2023
*/
#include "shared/defines.h"

// TODO(alicia): flip NOT
#if defined(LD_PLATFORM_LINUX)
#include "constants.h"
#include "core/time.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/math.h"
#include "core/internal/platform.h"
#include "core/internal/logging.h"

// TODO(alicia): get rid of C standard and use
// linux libraries instead
#include <stdio.h>

#include <dlfcn.h>

#define linux_log_note( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_NOTE,\
        sizeof( "[LINUX] " format), "[LINUX] " format, ##__VA_ARGS__ )
#define linux_log_info( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_INFO,\
        sizeof( "[LINUX] " format), "[LINUX] " format, ##__VA_ARGS__ )
#define linux_log_warn( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_WARN,\
        sizeof( "[LINUX] " format), "[LINUX] " format, ##__VA_ARGS__ )
#define linux_log_error( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_ERROR,\
        sizeof( "[LINUX] " format), "[LINUX] " format, ##__VA_ARGS__ )
#define linux_log_fatal( format, ... )\
    ___internal_core_log(\
        CORE_LOGGING_TYPE_FATAL,\
        sizeof( "[LINUX] " format), "[LINUX] " format, ##__VA_ARGS__ )   

PlatformFile* platform_get_stdout(void) {
    return stdout;
}
PlatformFile* platform_get_stderr(void) {
    return stderr;
}
PlatformFile* platform_get_stdin(void) {
    return stdin;
}

PlatformFile* platform_file_open( const char* path, PlatformFileFlags flags ) {
    char cflags_buf[32] = {};
    StringSlice cflags = string_slice_from_cstr( 32, cflags_buf );

    if( bitfield_check( flags, PLATFORM_FILE_READ | PLATFORM_FILE_WRITE ) ) {
        string_slice_push( &cflags, 'r' );
        string_slice_push( &cflags, '+' );
    } else {
        if( bitfield_check( flags, PLATFORM_FILE_READ ) ) {
            string_slice_push( &cflags, 'r' );
        } else if( bitfield_check( flags, PLATFORM_FILE_WRITE ) ) {
            string_slice_push( &cflags, 'w' );
            if( bitfield_check( flags, PLATFORM_FILE_ONLY_EXISTING ) ) {
                if( !platform_file_check_if_exists( path ) ) {
                    return NULL;
                }
            }
        }
    }

    string_slice_push( &cflags, 'b' );
    string_slice_push( &cflags, 0 );

    // TODO(alicia): shared read/write
    FILE* file = fopen( path, cflags.buffer );
    // TODO(alicia): logging!
    
    return file;
}
void platform_file_close( PlatformFile* file ) {
    // TODO(alicia): logging
    fclose( file );
}
usize platform_file_query_offset( PlatformFile* file ) {
    usize result = ftello64( file );
    // TODO(alicia): logging
    return result;
}
void platform_file_set_offset( PlatformFile* file, usize offset ) {
    // TODO(alicia): logging!
    fseeko64( file, offset, SEEK_SET );
}
usize platform_file_query_size( PlatformFile* file ) {
    usize current_offset = platform_file_query_offset( file );
    fseeko( file, 0, SEEK_END );
    usize size = platform_file_query_offset( file );
    platform_file_set_offset( file, current_offset );

    return size;
}
b32 platform_file_write( PlatformFile* file, usize buffer_size, void* buffer ) {
    size_t result = fwrite( buffer, sizeof(u8), buffer_size, file );
    // TODO(alicia): logging
    return result == buffer_size;
}
b32 platform_file_write_offset(
    PlatformFile* file, usize offset, usize buffer_size, void* buffer
) {
    usize current_offset = platform_file_query_offset( file );
    platform_file_set_offset( file, offset );
    b32 result = platform_file_write( file, buffer_size, buffer );
    platform_file_set_offset( file, current_offset );

    return result;
}
b32 platform_file_read( PlatformFile* file, usize buffer_size, void* buffer ) {
    size_t result = fread( buffer, sizeof(u8), buffer_size, file );
    // TODO(alicia): logging
    return result == buffer_size;
}
b32 platform_file_read_offset(
    PlatformFile* file, usize offset, usize buffer_size, void* buffer
) {
    usize current_offset = platform_file_query_offset( file );
    platform_file_set_offset( file, offset );
    b32 result = platform_file_read( file, buffer_size, buffer );
    platform_file_set_offset( file, current_offset );

    return result;
}
b32 platform_file_delete( const char* path ) {
    int result = remove( path );
    // TODO(alicia): logging
    return !result;
}
b32 platform_file_copy(
    const char* dst_path, const char* src_path, b32 fail_if_dst_exists
) {
    #define copy_err( format, ... )\
        linux_log_error(\
            "failed to copy '{cc}' to '{cc}'! " format,\
            src_path, dst_path, ##__VA_ARGS__ )

    if( fail_if_dst_exists ) {
        if( platform_file_check_if_exists( dst_path ) ) {
            copy_err( "dst path already exists!" );
            return false;
        }
    }

    PlatformFile* dst = platform_file_open( dst_path, PLATFORM_FILE_WRITE );


    if( !dst ) {
        copy_err( "failed to open dst!" );
        return false;
    }

    #define TMP_SIZE (kilobytes(1))
    char* tmp = system_alloc( TMP_SIZE );
    if( !tmp ) {
        copy_err( "failed to allocate temp memory!" );
        platform_file_close( dst );
        return false;
    }

    PlatformFile* src = platform_file_open( src_path, PLATFORM_FILE_READ );
    if( !src ) {
        copy_err( "failed to open src!" );
        platform_file_close( dst );
        system_free( tmp, TMP_SIZE );
        return false;
    }

    usize remaining = platform_file_query_size( src );
    b32 result = true;

    while( remaining ) {
        usize read_size = min( remaining, TMP_SIZE );

        if( !platform_file_read( src, read_size, tmp ) ) {
            copy_err( "failed to read {f,.2,m} from src!", (f64)read_size );
            result = false;
            goto platform_file_copy_end;
        }

        if( !platform_file_write( dst, read_size, tmp ) ) {
            copy_err( "failed to write {f,.2,m} to dst!", (f64)read_size );
            result = false;
            goto platform_file_copy_end;
        }

        remaining -= read_size;
    }

platform_file_copy_end:
    platform_file_close( src );
    platform_file_close( dst );
    system_free( tmp, TMP_SIZE );

    #undef TMP_SIZE
    #undef copy_err

    return result;
}
b32 platform_file_move(
    const char* dst_path, const char* src_path, b32 fail_if_dst_exists
) {
    b32 dst_exists = platform_file_check_if_exists( dst_path );
    if( fail_if_dst_exists && dst_exists ) {
        linux_log_error(
            "failed to move '{cc}' to '{cc}' because dst already exists!",
            src_path, dst_path );
        return false;
    }

    if( dst_exists ) {
        platform_file_delete( dst_path );
    }

    b32 copy_result = platform_file_copy( dst_path, src_path, false );

    if( !copy_result ) {
        return copy_result;
    }
    platform_file_delete( src_path );

    return true;
}
b32 platform_file_check_if_exists( const char* path ) {
    FILE* file = fopen( path, "r" );

    if( file ) {
        fclose( file );
        return true;
    } else {
        return false;
    }
}

PlatformSharedObject* platform_shared_object_open( const char* path ) {
    dlopen( path );
}
void platform_shared_object_close( PlatformSharedObject* object );
void* platform_shared_object_load(
    PlatformSharedObject* object, const char* function_name );


#endif /* Platform Linux */

