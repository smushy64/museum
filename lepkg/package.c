// * Description:  Liquid Engine Packager Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "package.h"
#include "format.h"
#include "version.h"
#include "term_color.h"
#include "path.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

internal b32 make_package_0_1( PackagerParameters* params );

#define FATAL() printerrln( "Something went wrong!" ); PANIC()

b32 make_package( PackagerParameters* params ) {

    if( !params->overwrite_existing ) {
        if( does_file_exist( params->output_path ) ) {
            printerrln(
                TC_RED( "Error: " )
                "Package \"%s\" already exists! "
                "Use --overwrite to overwrite.",
                params->output_path
            );
            return false;
        }
    }

    switch( params->major ) {
        case 0: switch( params->minor ) {
            case 1: {
                return make_package_0_1( params );
            } break;
            default: FATAL();
        }
        default: FATAL();
    }
    return false;
}

internal b32 make_package_0_1( PackagerParameters* params ) {
    FILE* output_file = fopen( params->output_path, "wb+" );
    if( !output_file ) {
        printerrln(
            TC_RED( "Error: " )
            "Failed to open \"%s\" for writing!",
            params->output_path
        );
        return false;
    }

    println( "Creating package at \"%s\" . . .", params->output_path );

    LEPkgHeader_0_1 header = {0};
    header.identifier  = LEPKG_FILE_IDENTIFIER;
    header.version     = LEPKG_MAKE_VERSION( params->major, params->minor );
    header.header_size = sizeof( LEPkgHeader_0_1 );

    u32        asset_count = 0;
    Asset_0_1* assets      =
        malloc( sizeof(Asset_0_1) * params->input_path_count );

    for( u32 i = 0; i < params->input_path_count; ++i ) {
        Asset_0_1*  current      = &assets[asset_count];
        const char* current_path = params->input_paths[i];

        unused(current);
        AssetType type = get_file_asset_type( current_path );
        if( type == ASSET_TYPE_UNKNOWN ) {
            continue;
        }
    }

    fclose( output_file );

    return true;
}

