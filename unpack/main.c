/**
 * Description:  Utility for testing liquid package files.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 17, 2024
*/
#include "shared/defines.h"
#include "core/print.h"
#include "core/string.h"
#include "core/path.h"
#include "core/fs.h"
#include "core/memory.h"
#include "core/compression.h"

#include "shared/liquid_package.h"

#include "generated/unpack_hashes.h"

#include "unpack/error.h"

global const char* global_program_name = "unpack";

typedef enum UnpackMode {
    UNPACK_MODE_HELP,
    UNPACK_MODE_HEADER,
    UNPACK_MODE_TEST,
} UnpackMode;

UnpackError mode_header( PathSlice package_path );
UnpackError mode_test( PathSlice package_path, u32 resource_id );
void print_help( UnpackMode mode );

int main( int argc, char** argv ) {
    global_program_name = argv[0];

    if( argc == 1 ) {
        error( "arguments are required!" );
        print_help(0);
        return UNPACK_ERROR_NO_ARGUMENTS;
    }

    PathSlice  package_path = {};
    UnpackMode mode         = UNPACK_MODE_HEADER;
    u32 resource_id         = 1;
    b32 mode_selected       = false;

    unused(resource_id);

    StringSlice arg;
    u64         arg_hash;

    #define get_arg()\
        arg      = string_slice_from_cstr( 0, argv[i] );\
        arg_hash = string_slice_hash( arg )

    for( int i = 1; i < argc; ++i ) {
        get_arg();

        if( mode_selected ) {
            switch( mode ) {
                case UNPACK_MODE_HELP: {
                    switch( arg_hash ) {
                        case HASH_ARG_MODE_HEADER: {
                            print_help( UNPACK_MODE_HEADER );
                        } return UNPACK_SUCCESS;
                        case HASH_ARG_MODE_TEST: {
                            print_help( UNPACK_MODE_TEST );
                        } return UNPACK_SUCCESS;
                        case HASH_ARG_MODE_HELP: {
                            print_help( UNPACK_MODE_HELP );
                        } return UNPACK_SUCCESS;
                    }
                } break;
                case UNPACK_MODE_HEADER: {
                    switch( arg_hash ) {
                        default: {
                            if( !package_path.str ) {
                                package_path = arg;
                                continue;
                            }
                        } break;
                    }
                } break;
                case UNPACK_MODE_TEST: {
                    switch( arg_hash ) {
                        case HASH_ARG_TEST_RESOURCE_ID: {
                            if( i + 1 > argc ) {
                                error( "--resource requires an argument after it!" );
                                print_help(UNPACK_MODE_TEST);
                                return UNPACK_ERROR_INVALID_ARGUMENT;
                            }
                            i++;
                            get_arg();
                            u64 parsed_int = 0;
                            if( !string_slice_parse_uint( arg, &parsed_int ) ) {
                                error( "--resource requires an "
                                      "unsigned integer after it!" );
                                print_help(UNPACK_MODE_TEST);
                                return UNPACK_ERROR_INVALID_ARGUMENT;
                            }
                            resource_id = parsed_int;
                        } continue;

                        default: {
                            if( !package_path.str ) {
                                package_path = arg;
                                continue;
                            }
                        } break;
                    }
                } break;
            }
        } else {
            switch( arg_hash ) {
                case HASH_ARG_MODE_HEADER: {
                    mode = UNPACK_MODE_HEADER;
                    mode_selected = true;
                } continue;
                case HASH_ARG_MODE_TEST: {
                    mode = UNPACK_MODE_TEST;
                    mode_selected = true;
                } continue;
                case HASH_ARG_MODE_HELP: {
                    mode = UNPACK_MODE_HELP;
                    mode_selected = true;
                } continue;
            }
        }

        error( "unrecognized argument: '{s}'", arg );
        print_help(0);
        return UNPACK_ERROR_UNRECOGNIZED_ARGUMENT;
    }
    #undef get_arg

    switch( mode ) {
        case UNPACK_MODE_HEADER: {
            if( !package_path.str ) {
                error( "path to package is required!" );
                print_help(mode);
                return UNPACK_ERROR_INVALID_ARGUMENT;
            }

            UnpackError error = mode_header( package_path );
            if( error ) {
                return error;
            }
        } break;
        case UNPACK_MODE_TEST: {
            if( !package_path.str ) {
                error( "path to package is required!" );
                print_help(mode);
                return UNPACK_ERROR_INVALID_ARGUMENT;
            }

            UnpackError error = mode_test( package_path, resource_id );
            if( error ) {
                return error;
            }
        } break;
        case UNPACK_MODE_HELP: print_help(0);
    }

    return UNPACK_SUCCESS;
}

void print_help( UnpackMode mode ) {
    println( "OVERVIEW: Unpack - Testing utility for LPKG\n" );
    switch( mode ) {
        case UNPACK_MODE_HELP: {
            println( "USAGE: {cc} <mode> [arguments]\n", global_program_name );
            println( "ARGUMENTS: " );
            println( "  <mode>  set mode ( header, test, help ) (default=header)" );
        } break;
        case UNPACK_MODE_HEADER: {
            println( "USAGE: {cc} header [arguments]\n", global_program_name );
            println( "ARGUMENTS: " );
            println( "  <path>                set path to package (required)" );
            // TODO(alicia): implement this!!!
            println( "  --range <uint>,<uint> only display resources in inclusive-exclusive range" );
        } break;
        case UNPACK_MODE_TEST: {
            println( "USAGE: {cc} test [arguments]\n", global_program_name );
            println( "ARGUMENTS: " );
            println( "  <path>             set path to package (required)" );
            println( "  --resource <uint>  set which resource to test. (default=0)" );
        } break;
    }
}

UnpackError mode_header( PathSlice package_path ) {
    FileHandle* package_file = fs_file_open( package_path, FILE_OPEN_FLAG_READ );
    if( !package_file ) {
        error( "package path provided is invalid! path: '{p}'", package_path );
        return UNPACK_ERROR_INVALID_PATH;
    }

    usize package_file_size = fs_file_query_size( package_file );

    struct PackageHeader header = {};
    if( fs_file_read( package_file, sizeof(header), &header ) ) {
        StringSlice id = {};
        id.str = header.id_c;
        id.len = static_array_count( header.id_c );

        const char* result = CONSOLE_COLOR_GREEN "(OK)" CONSOLE_COLOR_RESET;
        if( header.id != PACKAGE_ID ) {
            result = CONSOLE_COLOR_RED "(ERROR)" CONSOLE_COLOR_RESET;
        }

        println( "Total Package Size: {usize}({f,m,.2})",
            package_file_size, (f64)package_file_size );
        println( "Package Header: {cc}", result );
        println( "  identifier:     {u32}('{s}')", header.id, id );
        println( "  resource count: {u32}", header.resource_count );

        for( u32 i = 0; i < header.resource_count; ++i ) {
            u32 resource_id = i + 1;
            struct PackageResource resource = {};
            if( !fs_file_read( package_file, sizeof(resource), &resource ) ) {
                println(
                    CONSOLE_COLOR_RED
                    "Resource {u32} ERROR"
                    CONSOLE_COLOR_RESET, resource_id );
                continue;
            }

            StringSlice type = {};
            type.str = package_resource_type_to_cstr(
                resource.type, &type.len );

            StringSlice compression = {};
            compression.str = package_compression_to_cstr(
                resource.compression, &compression.len );

            print( "Resource {u32}:", resource_id );
            if( !resource.type ) {
                println( CONSOLE_COLOR_RED " Invalid" CONSOLE_COLOR_RESET );
                continue;
            } else {
                print( "\n" );
            }

            #define field( name, format, ... )\
                println(\
                    "  {s,-20}" format,\
                    string_slice( name ":" ), ##__VA_ARGS__ )

            field( "type", "{s}", type );
            field( "compression", "{s}", compression );
            field( "offset", "{u32}({f,m,.2})",
                resource.offset, (f64)resource.offset );
            field( "size", "{u32}({f,m,.2})",
                resource.size, (f64)resource.size );

            switch( resource.type ) {
                case PACKAGE_RESOURCE_TYPE_AUDIO: {
                    field( "channels", "{u8}", resource.audio.channel_count );
                    field( "bytes/sample", "{u8}",
                        resource.audio.bytes_per_sample );
                    field( "sample/sec", "{u32}",
                        resource.audio.samples_per_second );
                } break;
                case PACKAGE_RESOURCE_TYPE_TEXT: {
                    StringSlice lang;
                    lang.str = package_text_lang_to_cstr(
                        resource.text.lang, &lang.len );

                    StringSlice enc;
                    enc.str = package_text_encoding_to_cstr(
                        resource.text.encoding, &enc.len );

                    field( "lang", "{s}", lang );
                    field( "encoding", "{s}", enc );
                } break;
                case PACKAGE_RESOURCE_TYPE_TEXTURE: {
                    StringSlice channels = {};
                    channels.str = package_texture_channels_to_cstr(
                        resource.texture.type.channels, &channels.len );

                    StringSlice base_type = {};
                    base_type.str = package_texture_base_type_to_cstr(
                        resource.texture.type.base_type, &base_type.len );

                    field( "channels", "{s}", channels );
                    field( "base type", "{s}", base_type );

                    #define get_wrapping( dim )\
                        bitfield_check(\
                            resource.texture.flags,\
                            PACKAGE_TEXTURE_FLAG_WRAP_##dim##_REPEAT ) ?\
                            string_slice("Repeat") : string_slice("Clamp")

                    switch( resource.texture.type.dimensions ) {
                        case PACKAGE_TEXTURE_DIMENSIONS_3: {
                            StringSlice wrap_x = get_wrapping(X);
                            StringSlice wrap_y = get_wrapping(Y);
                            StringSlice wrap_z = get_wrapping(Z);

                            field( "dimensions", "{u32}x{u32}x{u32}",
                                resource.texture.width,
                                resource.texture.height,
                                resource.texture.depth );
                            field( "wrap x", "{s}", wrap_x );
                            field( "wrap y", "{s}", wrap_y );
                            field( "wrap z", "{s}", wrap_z );
                        } break;
                        case PACKAGE_TEXTURE_DIMENSIONS_2: {
                            StringSlice wrap_x = get_wrapping(X);
                            StringSlice wrap_y = get_wrapping(Y);
                            field( "dimensions", "{u32}x{u32}",
                                resource.texture.width,
                                resource.texture.height );
                            field( "wrap x", "{s}", wrap_x );
                            field( "wrap y", "{s}", wrap_y );
                        } break;
                        case PACKAGE_TEXTURE_DIMENSIONS_1: {
                            StringSlice wrap_x = get_wrapping(X);
                            field( "dimensions", "{u32}",
                                resource.texture.width );
                            field( "wrap x", "{s}", wrap_x );
                        } break;
                    }
                    #undef get_wrapping

                    StringSlice filtering =
                        bitfield_check( resource.texture.flags,
                            PACKAGE_TEXTURE_FLAG_BILINEAR_FILTER ) ?
                        string_slice( "Bilinear" ) :
                        string_slice( "Nearest" );

                    field( "filtering", "{s}", filtering );
                    field(
                        "transparent", "{b}",
                        bitfield_check( resource.texture.flags,
                            PACKAGE_TEXTURE_FLAG_TRANSPARENT ) );

                } break;
                default: break;
            }

            #undef field
        }
    } else {
        error( "failed to read package '{p}'!", package_path );
    }

    fs_file_close( package_file );
    return UNPACK_SUCCESS;
}

usize index = 0;
usize decompress_to_console( void* target, usize count, void* data ) {
    unused(target);
    print_string_stdout( count, data );
    return 0;
}

UnpackError test_texture(
    FileHandle* package, struct PackageHeader pkg_header,
    struct PackageResource pkg_resource, u32 resource_id );

UnpackError mode_test( PathSlice package_path, u32 resource_id ) {
    if( !resource_id ) {
        error( "resource id is invalid!" );
        return UNPACK_ERROR_INVALID_RESOURCE;
    }

    FileHandle* package = fs_file_open(
        package_path, FILE_OPEN_FLAG_READ | FILE_OPEN_FLAG_SHARE_ACCESS_READ );
    if( !package ) {
        error( "failed to open package '{p}'!", package_path );
        return UNPACK_ERROR_FILE_OPEN;
    }

    usize buffer_size = 0;
    void* buffer      = NULL;

    #define read( src, size, buffer ) do {\
        if( !fs_file_read( src, size, buffer ) ) {\
            fs_file_close( package );\
            if( buffer ) {\
                system_free( buffer, buffer_size );\
            }\
            return UNPACK_ERROR_FILE_READ;\
        }\
    } while(0)

    struct PackageHeader header = {};
    read( package, sizeof(header), &header );

    if( (resource_id - 1) > header.resource_count ) {
        error( "resource id {u32} is invalid!", resource_id );
        fs_file_close( package );
        return UNPACK_ERROR_INVALID_RESOURCE;
    }

    usize resource_offset =
        sizeof(header) + (sizeof(struct PackageResource) * (resource_id - 1) );

    struct PackageResource resource = {};
    fs_file_set_offset( package, resource_offset, false );
    read( package, sizeof(resource), &resource );

    UnpackError err = UNPACK_SUCCESS;
    switch( resource.type ) {
        case PACKAGE_RESOURCE_TYPE_INVALID: {
            error( "resource {u32} is invalid!", resource_id );
        } break;
        case PACKAGE_RESOURCE_TYPE_TEXT: {
            usize offset =
                sizeof(header) + (sizeof(resource) * header.resource_count) +
                resource.offset;
            fs_file_set_offset( package, offset, false );

            buffer_size = megabytes(1);
            buffer      = system_alloc( buffer_size );

            if( !buffer ) {
                error( "failed to display resource, ran out of memory!" );
                fs_file_close( package );
                return UNPACK_ERROR_OUT_OF_MEMORY;
            }

            usize remaining = resource.size;

            println( "resource text contents:" );
            if( resource.compression ) {
                // NOTE(alicia): skip original size
                fs_file_set_offset( package, sizeof(u64), true );
                remaining -= sizeof(u64);
            }
            while( remaining ) {
                usize read_size = buffer_size;
                if( read_size > remaining ) {
                    read_size = remaining;
                }

                read( package, read_size, buffer );

                switch( resource.compression ) {
                    case PACKAGE_COMPRESSION_NONE: {
                        print_string_stdout( read_size, buffer );
                    } break;
                    case PACKAGE_COMPRESSION_RLE: {
                        compression_rle_decode(
                            decompress_to_console, NULL,
                            read_size, buffer, NULL );
                    } break;
                }

                remaining -= read_size;
            }

            system_free( buffer, buffer_size );
        } break;
        case PACKAGE_RESOURCE_TYPE_TEXTURE: {
            err = test_texture( package, header, resource, resource_id );
        } break;
        case PACKAGE_RESOURCE_TYPE_AUDIO:
        case PACKAGE_RESOURCE_TYPE_MESH:
        {
            StringSlice type_name = {};
            type_name.str = package_resource_type_to_cstr(
                resource.type, &type_name.len );
            error( "testing resource type {s,l} is unimplemented!", type_name );
        } break;
    }

    #undef read
    fs_file_close( package );
    return err;
}

#include "generated_dependencies.inl"

