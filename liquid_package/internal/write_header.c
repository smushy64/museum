/**
 * Description:  Writing package resource header implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 07, 2023
*/
#include "defines.h"
#include "core/collections.h"
#include "core/internal.h"
#include "core/string.h"
#include "liquid_package.h"

#include "logging.h"
#include "parse_manifest.h"
#include "internal/write_header.h"

internal force_inline
const char* ___month_to_cstr( u32 month ) {
    switch( month ) {
        case 1:  return "January";
        case 2:  return "February";
        case 3:  return "March";
        case 4:  return "April";
        case 5:  return "May";
        case 6:  return "June";
        case 7:  return "July";
        case 8:  return "August";
        case 9:  return "September";
        case 10: return "October";
        case 11: return "November";
        case 12: return "December";
        default: return "Invalid Month";
    }
}

#define WRITE_HEADER_TMP_FILE "./header.tmp"

void write_header( void* user_params ) {
    struct WriteHeaderParams* params = user_params;
    if( !params->enum_name ) {
        lp_error( "enum name is null!" );
        panic();
    }

    PlatformFile* file = platform->io.file_open(
        WRITE_HEADER_TMP_FILE, PLATFORM_FILE_WRITE );
    if( !file ) {
        lp_warn( "failed to open header file!" );
        return;
    }

    string_slice_mut_capacity( temp, kilobytes(1) );

    #define write_file( format, ... )\
        temp.len = 0;\
        string_slice_fmt( &temp, format "\n", ##__VA_ARGS__ );\
        platform->io.file_write( file, temp.len, temp.buffer )

    PlatformTime time = platform->time.query_system_time();

    write_file( "#if !defined(LP_{cc}_HEADER)", params->enum_name );
    write_file( "#define LP_{cc}_HEADER", params->enum_name );
    write_file( "/**" );
    write_file( " * Description:  Generated liquid package header." );
    write_file( " * Author:       liquid-package generated file." );
    write_file( " * File Created: {cc} {u,02}, {u,04}",
        ___month_to_cstr( time.month ), time.day, time.year );
    write_file( "*/" );
    write_file( "#include \"defines.h\"" );

    usize resource_count = params->list_manifest_resources->count;

    write_file( " " );
    write_file( "typedef enum {cc} : u32 {{", params->enum_name );

    for( usize i = 0; i < resource_count; ++i ) {
        ManifestResource* current = list_index( params->list_manifest_resources, i );
        write_file(
            "    {cc}_{s},",
            liquid_package_resource_type_to_identifier( current->type ),
            current->id );
    }

    write_file( "    RESOURCE_COUNT," );
    write_file( "} {cc};", params->enum_name );
    write_file( " " );
    write_file( "#endif /* header guard */" );

    platform->io.file_close( file );

    if( platform->io.file_copy_by_path(
        params->output_path, WRITE_HEADER_TMP_FILE, false
    ) ) {
        lp_print( "generated header written to '{cc}'", params->output_path );
        platform->io.file_delete_by_path( WRITE_HEADER_TMP_FILE );
    } else {
        lp_error(
            "failed to open header path '{cc}'!\n"
            "generated header written to '" WRITE_HEADER_TMP_FILE "' instead.",
            params->output_path );
    }

    read_write_fence();

    semaphore_signal( params->finished );
    return;
}

