/**
 * Description:  Package Utility Manifest
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 09, 2024
*/
#include "shared/defines.h" // IWYU pragma: keep
#include "core/string.h"
#include "shared/liquid_package.h"
#include "core/path.h"
#include "package/error.h"

#include "package/manifest.h"
#include "package/logging.h"

#include "core/fs.h"
#include "core/memory.h"

#include "generated/package_hashes.h"

#define MANIFEST_LIST_MINIMUM_CAPACITY (12)

ManifestItem* ___manifest_list_next( struct ManifestList* list ) {
    if( list->count == list->capacity ) {
        usize old_size = list->capacity * sizeof(ManifestItem);
        usize new_size =
            old_size + ( MANIFEST_LIST_MINIMUM_CAPACITY * sizeof(ManifestItem) );
        ManifestItem* new_buffer = system_realloc( list->buffer, old_size, new_size );

        if( !new_buffer ) {
            return NULL;
        }

        list->capacity += MANIFEST_LIST_MINIMUM_CAPACITY;
        list->buffer   = new_buffer;
    }

    return list->buffer + list->count++;
}

PackageError manifest_parse( PathSlice manifest_path, Manifest* out_manifest ) {
    usize thread_index = 0;

    /* read manifest file */ {
        FileHandle* manifest_file = fs_file_open(
            manifest_path, FILE_OPEN_FLAG_READ | FILE_OPEN_FLAG_SHARE_ACCESS_READ );
        if( !manifest_file ) {
            log_error( "failed to open manifest file '{s}'!", manifest_path );
            return PACKAGE_ERROR_OPEN_FILE;
        }

        out_manifest->text.len = fs_file_query_size( manifest_file );
        out_manifest->text.str = system_alloc( out_manifest->text.len );
        if( !out_manifest->text.str ) {
            log_error(
                "failed to allocate {f,m,.2} for manifest text!",
                (f64)out_manifest->text.len );
            fs_file_close( manifest_file );
            memory_zero( &out_manifest->text, sizeof(out_manifest->text) );
            return PACKAGE_ERROR_OUT_OF_MEMORY;
        }

        if( !fs_file_read(
            manifest_file, out_manifest->text.len, out_manifest->text.c
        ) ) {
            log_error( "failed to read manifest file '{s}'!", manifest_path );
            fs_file_close( manifest_file );
            system_free( out_manifest->text.c, out_manifest->text.len );
            memory_zero( &out_manifest->text, sizeof(out_manifest->text) );
            return PACKAGE_ERROR_READ_FILE;
        }

        fs_file_close( manifest_file );
    }

    struct ManifestList* list = &out_manifest->items;
    list->capacity = MANIFEST_LIST_MINIMUM_CAPACITY;
    list->buffer   = system_alloc( list->capacity * sizeof(ManifestItem) );
    if( !list->buffer ) {
        log_error(
            "failed to allocate {f,m,.2} for manifest item list!",
            (f64)(list->capacity * sizeof(ManifestItem)) );
        manifest_destroy( out_manifest );
        return PACKAGE_ERROR_OUT_OF_MEMORY;
    }

    #define skip_line() goto manifest_parse_skip_line

    #define error( format, ... ) do {\
        result = PACKAGE_ERROR_PARSE_MANIFEST;\
        log_error( format, ##__VA_ARGS__ );\
        goto manifest_parse_end;\
    } while(0)

    #define error_line( format, ... )\
        error( format CONSOLE_COLOR_RESET "\n{usize}: {s}",\
            ##__VA_ARGS__, line_number, line )

    #define type_check() do {\
        if( !current->type ) {\
            error_line( "field 'type' must be defined before any other field!" );\
        }\
    } while(0)

    #define type_field_check( field, _type ) do {\
        type_check();\
        if( current->type != _type ) {\
            error_line( "field '" #field "' is only valid for type '" #_type "'!" );\
        }\
    } while(0)

    StringSlice text = out_manifest->text;

    PackageError  result  = PACKAGE_SUCCESS;
    ManifestItem* current = NULL;
    usize line_number     = 1;
    while( text.len ) {
        StringSlice line = {};
        usize nl_index   = 0;
        if( string_slice_find_char( text, '\n', &nl_index ) ) {
            line.len = nl_index;
            line.str = text.str;
        } else {
            line = text;
        }

manifest_parse_process_line:

        if( !line.len || ( line.len == 1 && line.str[0] == '\n' ) ) {
            text.str++;
            text.len--;
            continue;
        }

        if( line.str[0] == '#' ) {
            skip_line();
        }

        StringSlice token_left = {}, token_right = {};

        if( current ) {
            goto manifest_parse_fields;
        } else {
            goto manifest_parse_new_item;
        }

manifest_parse_skip_line:
        text.str += line.len;
        text.len -= line.len;
        // NOTE(alicia): remove trailing new line.
        if( line.len > 1 ) {
            text.str++;
            text.len--;
        }
        line_number++;
        continue;

manifest_parse_new_item:

        if( !token_left.str ) {
            if( !string_slice_split_char( line, ':', &token_left, &token_right ) ) {
                error_line( "unrecognized syntax:\n" );
            }
        }
        if( token_right.len ) {
            error_line(
                "unrecognized token! left: {s} right: {s}\n",
                token_left, token_right );
        }

        current = ___manifest_list_next( list );
        if( !current ) {
            log_error( "failed to reallocate manifest item list!" );
            result = PACKAGE_ERROR_OUT_OF_MEMORY;
            goto manifest_parse_end;
        }

        current->identifier = token_left;
        if( current->identifier.len > out_manifest->longest_identifier_length ) {
            out_manifest->longest_identifier_length = current->identifier.len;
        }

        skip_line();

manifest_parse_fields:
        if( !char_is_whitespace( line.str[0] ) ) {
            if( !current->type ) {
                error( "manifest item {usize} requires a type!", list->count - 1 );
            }
            if( !current->path.str ) {
                error( "manifest item {usize} requires a path!", list->count - 1 );
            }
            current = NULL;
            goto manifest_parse_process_line;
        }

        if( !string_slice_split_char( line, ':', &token_left, &token_right ) ) {
            error_line( "unrecognized syntax:" );
        }

        token_left  = string_slice_trim_whitespace( token_left );
        token_right = string_slice_trim_whitespace( token_right );

        u64 token_left_hash = string_slice_hash( token_left );

        switch( token_left_hash ) {
            case HASH_TOKEN_MANIFEST_TYPE: {
                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_TYPE_AUDIO: {
                        current->type = PACKAGE_RESOURCE_TYPE_AUDIO;
                    } break;
                    case HASH_TOKEN_MANIFEST_TYPE_MESH: {
                        current->type = PACKAGE_RESOURCE_TYPE_MESH;
                    } break;
                    case HASH_TOKEN_MANIFEST_TYPE_TEXT: {
                        current->type = PACKAGE_RESOURCE_TYPE_TEXT;
                    } break;
                    case HASH_TOKEN_MANIFEST_TYPE_TEXTURE: {
                        current->type = PACKAGE_RESOURCE_TYPE_TEXTURE;
                    } break;
                    default: error_line( "unrecognized token:" );
                }

                usize type_name_length = 0;
                package_resource_type_to_cstr(
                    current->type, &type_name_length );
                if( out_manifest->longest_type_name_length < type_name_length ) {
                    out_manifest->longest_type_name_length = type_name_length;
                }

            } break;
            case HASH_TOKEN_MANIFEST_PATH: {
                type_check();

                // NOTE(alicia): trim leading and trailing quotes, if path is quoted
                if( token_right.len >= 2 ) {
                    if( token_right.str[token_right.len - 1] == '"' ) {
                        token_right.len--;
                    }
                    if( token_right.str[0] == '"' ) {
                        token_right.str++;
                        token_right.len--;
                    }
                }

                current->path = reinterpret_cast( PathSlice, &token_right );
            } break;
            case HASH_TOKEN_MANIFEST_COMPRESSION: {
                type_check();

                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_COMPRESSION_RLE: {
                        current->compression = PACKAGE_COMPRESSION_RLE;
                    } break;
                    default: error_line( "unrecognized token: {s}", token_right );
                }
            } break;
            case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_X: {
                type_field_check( "wrap_x", PACKAGE_RESOURCE_TYPE_TEXTURE );

                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_CLAMP: {
                        current->texture.flags = bitfield_clear(
                            current->texture.flags,
                            PACKAGE_TEXTURE_FLAG_WRAP_X_REPEAT );
                    } break;
                    case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_REPEAT: {
                        current->texture.flags |=
                            PACKAGE_TEXTURE_FLAG_WRAP_X_REPEAT;
                    } break;
                    default: error_line( "unrecognized token: {s}", token_right );
                }
            } break;
            case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_Y: {
                type_field_check( "wrap_y", PACKAGE_RESOURCE_TYPE_TEXTURE );

                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_CLAMP: {
                        current->texture.flags = bitfield_clear(
                            current->texture.flags,
                            PACKAGE_TEXTURE_FLAG_WRAP_Y_REPEAT );
                    } break;
                    case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_REPEAT: {
                        current->texture.flags |=
                            PACKAGE_TEXTURE_FLAG_WRAP_Y_REPEAT;
                    } break;
                    default: error_line( "unrecognized token: {s}", token_right );
                }
            } break;
            case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_Z: {
                type_field_check( "wrap_z", PACKAGE_RESOURCE_TYPE_TEXTURE );

                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_CLAMP: {
                        current->texture.flags = bitfield_clear(
                            current->texture.flags,
                            PACKAGE_TEXTURE_FLAG_WRAP_Z_REPEAT );
                    } break;
                    case HASH_TOKEN_MANIFEST_TEXTURE_WRAP_REPEAT: {
                        current->texture.flags |=
                            PACKAGE_TEXTURE_FLAG_WRAP_Z_REPEAT;
                    } break;
                    default: error_line( "unrecognized token:" );
                }
            } break;
            case HASH_TOKEN_MANIFEST_TEXTURE_FILTERING: {
                type_field_check( "filtering", PACKAGE_RESOURCE_TYPE_TEXTURE );

                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_TEXTURE_FILTERING_NEAREST: {
                        current->texture.flags = bitfield_clear(
                            current->texture.flags,
                            PACKAGE_TEXTURE_FLAG_BILINEAR_FILTER );
                    } break;
                    case HASH_TOKEN_MANIFEST_TEXTURE_FILTERING_BILINEAR: {
                        current->texture.flags |=
                            PACKAGE_TEXTURE_FLAG_BILINEAR_FILTER;
                    } break;
                    default: error_line( "unrecognized token: {s}", token_right );
                }
            } break;
            case HASH_TOKEN_MANIFEST_TEXTURE_TRANSPARENT: {
                type_field_check( "transparent", PACKAGE_RESOURCE_TYPE_TEXTURE );

                u64 token_right_hash = string_slice_hash( token_right );

                switch( token_right_hash ) {
                    case HASH_TOKEN_MANIFEST_TRUE: {
                        current->texture.flags |= PACKAGE_TEXTURE_FLAG_TRANSPARENT;
                    } break;
                    case HASH_TOKEN_MANIFEST_FALSE: {
                        current->texture.flags = bitfield_clear(
                            current->texture.flags,
                            PACKAGE_TEXTURE_FLAG_TRANSPARENT );
                    } break;
                    default: error_line( "unrecognized token: {s}", token_right );
                }
            } break;
            default: error_line( "unrecognized token: {s}", token_left );
        }
        skip_line();
    }

manifest_parse_end:
    if( result ) {
        manifest_destroy( out_manifest );
    }
    #undef error
    #undef error_line
    #undef type_field_check 
    #undef type_check 
    #undef skip_line
    return result;
}
void manifest_destroy( Manifest* manifest ) {
    if( manifest->text.str ) {
        system_free( manifest->text.v, manifest->text.len );
    }
    if( manifest->items.buffer ) {
        system_free(
            manifest->items.buffer, manifest->items.capacity * sizeof(ManifestItem) );
    }
    memory_zero( manifest, sizeof(*manifest) );
}


