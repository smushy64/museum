// * Description:  Liquid Engine Asset Packer Utility
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 11, 2023
#include "defines.h"
#include "term_color.h"
#include "error.h"
#include "format.h"
#include "fs.h"
#include "validate.h"
#include "package.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "corec.inl"

void print_help();
void print_version();
void print_filetypes();

#define MAX_COMPRESSION_LEVEL (0ul)

#define EARLY_RETURN( code ) return code
#define strchk( a, b ) (strcmp( a, b ) == 0)
#define NEXT_ARG() arg = argv[++i]
int main( int argc, char** argv ) {

    LEError error_code = LE_ERROR_NO_ERROR;

    if( argc <= 1 ) {
        printerrln(
            TC_RED( "Error: " )
            "No arguments provided!"
        );
        print_help();
        EARLY_RETURN( LE_ERROR_NO_ARGUMENTS );
    }

    b32 validate = false;
    const char* validate_path = NULL;

    PackagerParameters params = {0};
    params.major = LEPKG_VERSION_MAJOR;
    params.minor = LEPKG_VERSION_MINOR;
    
    u32 input_path_buffer_size = sizeof( void* ) * argc;

    params.input_paths = malloc( input_path_buffer_size );
    if( !params.input_paths ) {
        usize allocation_size = input_path_buffer_size;
        printerrln(
            TC_RED( "Failed to allocate %llu bytes!" ),
            allocation_size
        );
        EARLY_RETURN( LE_ERROR_ALLOCATION_FAILED );
    }
    memset( params.input_paths, 0, input_path_buffer_size );

    for( i32 i = 1; i < argc; ++i ) {
        const char* arg = argv[i];
        if( arg[0] != '-' ) {
            params.input_paths[params.input_path_count++] = arg;
            continue;
        }

        if(
            strchk( arg, "-h" ) ||
            strchk( arg, "--help" )
        ) {
            print_help();
            EARLY_RETURN( LE_ERROR_NO_ERROR );
        } else if( strchk( arg, "--list-filetypes" ) ) {
            print_filetypes();
            EARLY_RETURN( LE_ERROR_NO_ERROR );
        } else if( strchk( arg, "-v" ) ) {
            print_version();
            EARLY_RETURN( LE_ERROR_NO_ERROR );
        } else if( strchk( arg, "--overwrite" ) ) {
            params.overwrite_existing = true;
        } else if( strchk( arg, "--compress" ) ) {
            if( i + 1 < argc ) {
                NEXT_ARG();
                i32 parsed_compression = 0;
                if( !str_to_i32( arg, &parsed_compression ) ) {
                    error_code = LE_ERROR_INVALID_ARGUMENTS;
                    printerrln(
                        TC_RED( "Error:" )
                        "Argument --compress requires an int argument!"
                    );
                    break;
                }
                params.compression_level = (u32)parsed_compression;
            } else {
                error_code = LE_ERROR_MISSING_ARGUMENT;
                printerrln(
                    TC_RED( "Error: " )
                    "Argument --compress requires an int argument!"
                );
            }
        } else if( strchk( arg, "-o" ) ) {
            if( i + 1 < argc ) {
                NEXT_ARG();
                params.output_path = arg;
            }
        } else if( strchk( arg, "--version" ) ) {
            i32 major = LEPKG_VERSION_MAJOR;
            i32 minor = LEPKG_VERSION_MINOR;

            if( i + 1 < argc ) {
                NEXT_ARG();
                if( str_to_i32( arg, &major ) ) {
                    if( i + 1 < argc ) {
                        NEXT_ARG();
                        str_to_i32( arg, &minor );
                    }
                }
                
            }

            if( !is_version_major_minor_valid( major, minor ) ) {
                error_code = LE_ERROR_UNRECOGNIZED_VERSION;
                printerrln(
                    TC_RED( "Error: " )
                    "Unrecognized version %i.%i",
                    major, minor
                );
                break;
            }

            params.major = major;
            params.minor = minor;

        } else if( strchk( arg, "--outline" ) ) {
            i32 major = LEPKG_VERSION_MAJOR;
            i32 minor = LEPKG_VERSION_MINOR;

            if( i + 1 < argc ) {
                NEXT_ARG();
                if( str_to_i32( arg, &major ) ) {
                    if( i + 1 < argc ) {
                        NEXT_ARG();
                        str_to_i32( arg, &minor );
                    }
                }
                
            }

            if( !is_version_major_minor_valid( major, minor ) ) {
                error_code = LE_ERROR_UNRECOGNIZED_VERSION;
                printerrln(
                    TC_RED( "Error: " )
                    "Unrecognized version %i.%i",
                    major, minor
                );
                break;
            }

            print_outline( major, minor );
            EARLY_RETURN( LE_ERROR_NO_ERROR );
        } else if( strchk( arg, "--validate" ) ) {

            validate = true;

            if( i + 1 < argc ) {
                NEXT_ARG();
                validate_path = arg;
            }

        } else {
            printerrln(
                TC_RED( "Error: " )
                "Unrecognized argument " TC_RED("\"%s\"") "!",
                arg
            );
            error_code = LE_ERROR_INVALID_ARGUMENTS;
        }
    }

    if( !params.input_path_count ) {
        printerrln(
            TC_RED( "Error: " )
            "No paths provided!"
        );
        print_help();
        EARLY_RETURN( LE_ERROR_MISSING_ARGUMENT );
    }

    if( params.compression_level > MAX_COMPRESSION_LEVEL ) {
        printerrln(
            TC_RED( "Error: " )
            "\"%u\" is an invalid compression level!",
            params.compression_level
        );
        print_help();
        EARLY_RETURN( LE_ERROR_INVALID_COMPRESSION );
    }

    if( validate && !validate_path ) {
        printerrln(
            TC_RED( "Error: " )
            "Argument --validate requires a path!"
        );
        print_help();
        EARLY_RETURN( LE_ERROR_MISSING_ARGUMENT );
    }

    if( validate ) {
        if( does_file_exist( validate_path ) ) {
            error_code = LE_ERROR_NO_ERROR;
            if( !is_package_valid( validate_path ) ) {
                error_code = LE_ERROR_VALIDATION_FAILED;
            }
            EARLY_RETURN( error_code );
        } else {
            printerrln(
                TC_RED( "Error: " )
                "Path to package for validation \"%s\" is invalid!",
                validate_path
            );
            error_code = LE_ERROR_INVALID_PATH;
        }
    }

    for( u32 i = 0; i < params.input_path_count; ++i ) {
        if( !does_file_exist( params.input_paths[i] ) ) {
            printerrln(
                TC_RED( "Error: " )
                "Input path \"%s\" is invalid!",
                params.input_paths[i]
            );
            error_code = LE_ERROR_INVALID_PATH;
            break;
        } else {
            println( "%s", params.input_paths[i] );
        }
    }

    if( !params.output_path ) {
        params.output_path = "./untitled.lepkg";
    }

    if( error_code != LE_ERROR_NO_ERROR ) {
        print_help();
        return error_code;
    }

    if( !make_package( &params ) ) {
        error_code = LE_ERROR_PACKAGE_FAILED;
    }

    return error_code;
}

void print_filetypes() {
    println( "List of file types recognized by lepkg:" );
    println( "Metadata: lescn" );
    println( "Texture:  bmp, png, psd" );
    println( "Font:     ttf" );
    println( "Audio:    wav" );
    println( "3D Model: obj, gltf, blend, fbx" );
    println( "Shader:   spv" );
}

void print_version() {
    println(
        "Liquid Engine Asset Packer Utility "
        "Version %i.%i",
        LEPKG_VERSION_MAJOR, LEPKG_VERSION_MINOR
    );
    // TODO(alicia): replace with actual current platform
    println( "Built for x86_64-win32" );
    println( "Software written by smushy (Alicia Amarilla)" );
}

void print_help() {
    println( "OVERVIEW: Liquid Engine Asset Packer Utility\n" );
    println( "USAGE: lepkg [options] path...\n" );
    println( "OPTIONS:" );
    println( "  [path]                        paths to files" );
    println( "                                  lepkg uses file extension to determine how to interpret file" );
    println( "  -o [path]                     define output file path" );
    println( "  --version [int] [int]         set package version" );
    println( "  --compress [int]              set compression level for following images and audio" );
    println( "                                  valid compression levels: 0" );
    println( "  --overwrite                   overwrite package if it already exists" );
    println( "  --validate [path] [int] [int] validate lepkg file pointed to by [path]" );
    println( "                                  optional: validate for specific version" );
    println( "  --list-filetypes              list all file types that lepkg recognizes" );
    println( "  --outline [int] [int]         print outline of lepkg file format" );
    println( "                                  optional: specify version to outline" );
    println( "  -h, --help                    print this help message" );
    println( "  -v                            print version information" );
}

