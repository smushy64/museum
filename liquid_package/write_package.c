/**
 * Description:  Write package implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 11, 2023
*/
#include "defines.h"
#include "core/collections.h"
#include "core/thread.h"
#include "core/string.h"

#include "liquid_package.h"
#include "parse_manifest.h"
#include "shared_buffer.h"
#include "logging.h"
#include "write_package.h"
#include "internal/package_audio.h"

volatile usize global_offset = 0;

void write_package( void* user_params ) {
    struct WritePackageParams* params = user_params;

    PlatformFile* output_file = platform->io.file_open(
        PACKAGER_TMP_OUTPUT_PATH, PLATFORM_FILE_WRITE | PLATFORM_FILE_SHARE_WRITE );

    if( !output_file ) {
        lp_error( "failed to open output file!" );

        StringSlice error = {};
        platform->last_error( &error.len, (const char**)&error.buffer );

        lp_error( "{s}", error );
        panic();
    }

    ManifestResource* current =
        list_index( params->list_manifest_resources, params->index );

    usize buffer_offset =
        sizeof( struct LiquidPackageHeader ) + (
            ( sizeof( struct LiquidPackageResource ) *
            params->list_manifest_resources->count )
        );
    usize resource_offset =
        sizeof( struct LiquidPackageHeader ) +
        ( sizeof( struct LiquidPackageResource ) * params->index );

    usize resource_buffer_size = 0;
    void* resource_buffer      = NULL;
    struct LiquidPackageResource resource = {};

    StringSlice resource_path = string_slice_clone( &current->path );

    switch( current->type ) {
        case LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO: {
            PackageAudioError error = package_audio(
                &resource_path,
                &resource.audio,
                &resource_buffer_size, &resource_buffer );

            if( error == PACKAGE_AUDIO_SUCCESS ) {
                resource.type = LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO;
            } else {
                lp_warn(
                    "failed to package '{s}'! error: '{cc}'",
                    resource_path,
                    package_audio_error_to_cstr( error ) );
            }

        } break;
        default: break;
    }

    if( resource_buffer && resource_buffer_size ) {
        usize offset = interlocked_add( &global_offset, resource_buffer_size );
        read_write_fence();

        usize write_offset     = buffer_offset + offset;
        resource.buffer_offset = offset;

        b32 write_result = platform->io.file_write_offset(
            output_file,
            resource_buffer_size, resource_buffer,
            write_offset );
        platform->time.sleep_ms( 2 );
        if( !write_result ) {
            package_shared_buffer_free( resource_buffer, resource_buffer_size );
            lp_error( "failed to write {f,.2,b} at offset {usize} to output file!", (f64)resource_buffer_size, write_offset );
            panic();
        }

        package_shared_buffer_free( resource_buffer, resource_buffer_size );

        lp_note( "{usize}: packaged '{s}'!", params->index, resource_path );
    } else {
        lp_warn(
            "{usize}: failed to allocate memory for package '{s}'!",
            params->index, resource_path );
    }

    platform->io.file_write_offset(
        output_file,
        sizeof( struct LiquidPackageResource ),
        &resource, resource_offset );

    platform->io.file_close( output_file );

    read_write_fence();
    semaphore_signal( params->finished );
}


