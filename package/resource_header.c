/**
 * Description:  Resource header generator implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 11, 2023
*/
#include "shared/defines.h"
#include "core/sync.h"
#include "package/resource_header.h"

#include "package/manifest.h"
#include "package/logging.h"

#include "core/path.h"
#include "core/fs.h"
#include "core/time.h"

#define GENERATE_HEADER_TMP_FILE_PATH "./header.tmp"

void job_generate_header( usize thread_index, void* user_params ) {
    unused(thread_index);

    GenerateHeaderParams* params = user_params;
    PathSlice tmp_path = path_slice( GENERATE_HEADER_TMP_FILE_PATH );

    if( fs_check_if_file_exists( tmp_path ) ) {
        if( !fs_delete_file( tmp_path ) ) {
            lp_error( "failed to delete existing temp header!" );
            goto generate_header_end;
        }
    }

    FileHandle* tmp_file = fs_file_open(
        tmp_path, FILE_OPEN_FLAG_WRITE | FILE_OPEN_FLAG_SHARE_ACCESS_READ );
    if( !tmp_file ) {
        lp_error( "failed to open temp file for generating resource header!" );
        goto generate_header_end;
    }

    #define header_write( format, ... ) do {\
        if( !fs_file_write_fmt( tmp_file, format "\n", ##__VA_ARGS__ ) ) {\
            lp_error( "failed to write to temp resource header file!" );\
            fs_file_close( tmp_file );\
            goto generate_header_end;\
        }\
    } while(0)

    TimeRecord time = time_record();

    header_write( "#if !defined( LP_GENERATED_RESOURCE_HEADER_H )" );
    header_write( "#define LP_GENERATED_RESOURCE_HEADER_H" );

    header_write( "/**" );
    header_write( " * Description:  Generated Liquid Package resource header." );
    header_write( " * Author:       Liquid Engine Utility: lpkg" );
    header_write( " * File Created: {cc} {u,02}, {u,04}",
        time_month_to_cstr( time.month ), time.day, time.year );
    header_write( "*/" );

    header_write( "#include \"defines.h\"\n" );

    header_write( "typedef enum GeneratedResourceID : u32 {{" );
    for( usize i = 0; i < params->manifest->item_count; ++i ) {

        ManifestItem* item = params->manifest->items + i;

        header_write( "    {cc}_{s},",
            liquid_package_resource_type_to_identifier( item->type ),
            item->identifier );

    }
    header_write( "    RESOURCE_COUNT," );
    header_write( "} GeneratedResourceID;\n" );

    header_write( "#endif /* header guard */" );

    fs_file_close( tmp_file );

    if( fs_move_by_path(
        params->header_output_path,
        tmp_path, false
    ) ) {
        lp_note( "generated header written to '{cc}'", params->header_output_path );
        fs_delete_file( tmp_path );
    } else {
        lp_error(
            "failed to move temp header to path '{cc}'!", params->header_output_path );
    }

generate_header_end:
    read_write_fence();
    semaphore_signal( &params->finished );
}

