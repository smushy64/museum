/**
 * Description:  Manifest implementation
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 07, 2023
*/
#include "shared/defines.h"
#include "core/string.h"
#include "shared/liquid_package.h"
#include "manifest.h"

#include "logging.h"

#include "core/collections.h"
#include "core/memory.h"
#include "core/fs.h"

global b32 global_manifest_hashes_generated = false;

global u64 global_hash_path;

global u64 global_hash_type;
global u64 global_hash_type_audio;
global u64 global_hash_type_model;
global u64 global_hash_type_texture;
global u64 global_hash_type_text;
global u64 global_hash_type_map;

global u64 global_hash_compression;
global u64 global_hash_compression_rle;

void ___generate_manifest_hashes(void) {
    if( global_manifest_hashes_generated ) {
        return;
    }
    global_manifest_hashes_generated = true;
    global_hash_path         = cstr_hash( "path" );
    global_hash_type         = cstr_hash( "type" );
    global_hash_type_audio   = cstr_hash( "audio" );
    global_hash_type_model   = cstr_hash( "model" );
    global_hash_type_texture = cstr_hash( "texture" );
    global_hash_type_text    = cstr_hash( "text" );
    global_hash_type_map     = cstr_hash( "map" );

    global_hash_compression     = cstr_hash( "compression" );
    global_hash_compression_rle = cstr_hash( "rle" );
}

b32 ___check_for_tab( StringSlice* line ) {
    if( line->buffer[0] == '\t' ) {
        return true;
    }

    if(
        line->len >= 4 &&
        line->buffer[0] == ' ' &&
        line->buffer[1] == ' ' &&
        line->buffer[2] == ' ' &&
        line->buffer[3]
    ) {
        return true;
    }

    return false;
}

b32 ___validate_c_identifier( StringSlice* identifier ) {

    for( usize i = 0; i < identifier->len; ++i ) {
        char current = identifier->buffer[i];
        b32 is_underscore = current == '_';
        b32 is_digit      = char_is_digit( current );
        b32 is_letter     = char_is_latin_letter( current );

        if( !( is_underscore || is_digit || is_letter ) ) {
            return false;
        }
    }
    
    return true;
}

global const char* global_local_directory = "./";

b32 manifest_parse( const char* path, Manifest* out_manifest ) {

    FSFile* file = fs_file_open(
        path, FS_FILE_READ | FS_FILE_SHARE_READ | FS_FILE_ONLY_EXISTING );
    if( !file ) {
        lp_error( "failed to open manifest '{cc}'!", path );
        return false;
    }

    usize file_size = fs_file_query_size( file );
    if( !file_size ) {
        lp_error( "'{cc}' is not a manifest!" );
        return false;
    }
    char* manifest_buffer = system_alloc( file_size );
    if( !manifest_buffer ) {
        lp_error(
            "failed to allocate {f,m,.2} for manifest text buffer!", (f64)file_size );
        return false;
    }
    
    if( !fs_file_read( file, file_size, manifest_buffer ) ) {
        lp_error( "failed to read manifest file!" );
        return false;
    }

    fs_file_close( file );

    StringSlice manifest = {};
    manifest.buffer = manifest_buffer;
    manifest.len    = file_size;
    StringSlice line = {};
    StringSlice token_left = {}, token_right = {};

    #define next_line()\
        string_slice_split_char( &manifest, '\n', &line, &manifest )

    #define missing_token( token )\
        lp_error( "file '{cc}' is missing token: '{s}'!", token )

    #define invalid_manifest()\
        lp_error( "file '{cc}' is not a valid manifest!", path )

    #define trim_whitespace( slice )\
        string_slice_trim_leading_whitespace( (slice), (slice) );\
        string_slice_trim_trailing_whitespace( (slice), (slice) )

    #define split_line( char )\
        string_slice_split_char( &line, char, &token_left, &token_right )

    b32 identifier_found  = false;
    b32 item_count_parsed = false;
    u64 item_count = 0;
    string_slice_const( token_manifest_identifier, "manifest" );
    string_slice_const( token_count, "count" );

    while( !(identifier_found && item_count_parsed) && next_line() ) {
        if( !line.len || line.buffer[0] == '#' ) {
            continue;
        }

        if( identifier_found ) {
            if( !split_line( ':' ) ) {
                break;
            }

            trim_whitespace( &token_left );
            trim_whitespace( &token_right );

            if( !string_slice_cmp( &token_left, &token_count ) ) {
                break;
            }

            u64 parsed_count = 0;
            if( !string_slice_parse_uint( &token_right, &parsed_count ) ) {
                break;
            }

            item_count = parsed_count;
            item_count_parsed = true;
        } else {
            if( string_slice_cmp( &line, &token_manifest_identifier ) ) {
                identifier_found = true;
            } else {
                break;
            }
        }
    }

    if( !identifier_found ) {
        invalid_manifest();
        missing_token( token_manifest_identifier );
        system_free( manifest_buffer, file_size );
        return false;
    }

    if( !item_count_parsed ) {
        invalid_manifest();
        lp_error( "manifest '{cc}' is missing item count!", path );
        system_free( manifest_buffer, file_size );
        return false;
    }

    if( !item_count ) {
        lp_error( "manifest '{cc}' does not have any items?!", path );
        system_free( manifest_buffer, file_size );
        return false;
    }

    usize item_buffer_size = sizeof(ManifestItem) * item_count;
    void* item_buffer      = system_alloc( sizeof( ManifestItem ) * item_count );
    if( !item_buffer ) {
        lp_error(
            "failed to allocate {f,m,.2} for manifest item buffer!",
            (f64)item_buffer_size );
        system_free( manifest_buffer, file_size );
        return false;
    }
    List manifest_items = list_create( item_count, sizeof(ManifestItem), item_buffer );
    unused(manifest_items);

    usize initial_text_buffer_size = 256;
    void* text_buffer              = system_alloc( initial_text_buffer_size );
    if( !text_buffer ) {
        lp_error(
            "failed to allocate {f,m,.2} for manifest text buffer!",
            (f64)initial_text_buffer_size );
        system_free( manifest_buffer, file_size );
        system_free(
            manifest_items.buffer,
            manifest_items.capacity * manifest_items.item_size );
        return false;
    }
    StackAllocator text_buffer_stack =
        stack_allocator_create( initial_text_buffer_size, text_buffer );

    #define manifest_error()\
        system_free( manifest_buffer, file_size );\
        system_free(\
            manifest_items.buffer,\
            manifest_items.capacity * manifest_items.item_size );\
        system_free( text_buffer_stack.buffer, text_buffer_stack.buffer_size )

    #define text_push( slice, with_null ) do {\
        usize buffer_size = (slice)->len + (with_null ? 1 : 0);\
        void* buffer = stack_allocator_push( &text_buffer_stack, buffer_size );\
        if( !buffer ) {\
            usize new_stack_size   = text_buffer_stack.buffer_size + buffer_size + 256;\
            void* new_stack_buffer = system_realloc(\
                text_buffer_stack.buffer,\
                text_buffer_stack.buffer_size, new_stack_size );\
            if( !new_stack_buffer ) {\
                lp_error( "failed to allocate {f,m,.2}!", new_stack_size );\
                manifest_error();\
                return false;\
            }\
            text_buffer_stack.buffer      = new_stack_buffer;\
            text_buffer_stack.buffer_size = new_stack_size;\
            buffer = stack_allocator_push( &text_buffer_stack, buffer_size );\
        }\
        memory_copy( buffer, (slice)->buffer, (slice)->len );\
        (slice)->buffer = buffer;\
    } while(0)

    typedef enum : u32 {
        CTX_NONE,
        CTX_PARSE_FIELDS,
    } ManifestContext;

    #define m_warn( format, ... )\
        lp_warn( "manifest '{cc}': " format, path, ##__VA_ARGS__ )
    #define m_error( format, ... )\
        lp_error( "manifest '{cc}': " format, path, ##__VA_ARGS__ )

    ManifestContext ctx = CTX_NONE;

    u32 running_placeholder_index = 0;
    string_slice_mut_capacity( placeholder_c_identifier, 64 );

    ___generate_manifest_hashes();

    #define try_next_line() if( !next_line() ) break

    ManifestItem item  = {};
    usize excess_items = 0;

    next_line();
    loop {
        if( !line.len || line.buffer[0] == '#' ) {
            try_next_line();
            continue;
        }

        switch( ctx ) {
            case CTX_NONE: {
                usize position = 0;
                if( string_slice_find_char( &line, ':', &position ) ) {
                    token_left.buffer   = line.buffer;
                    token_left.len      = position;
                    token_left.capacity = token_left.len;

                    if( ___validate_c_identifier( &token_left ) ) {
                        text_push( &token_left, false );
                        item.identifier = token_left;
                    } else {
                        m_warn( "'{s}' is not a valid C identifier!", token_left );
                        string_slice_fmt(
                            &placeholder_c_identifier,
                            "ITEM_{u}", running_placeholder_index++ );
                        StringSlice placeholder =
                            string_slice_clone( &placeholder_c_identifier );
                        text_push( &placeholder, false );

                        placeholder_c_identifier.len = 0;

                        item.identifier = placeholder;
                    }
                    ctx = CTX_PARSE_FIELDS;
                }
            } break;
            case CTX_PARSE_FIELDS: {
                if( !___check_for_tab( &line ) ) {
                    if( !list_push( &manifest_items, &item ) ) {
                        m_warn(
                            "excess items in manifest: {usize}", ++excess_items );
                    }
                    memory_zero( &item, sizeof(item) );
                    ctx = CTX_NONE;
                    // deliberately skipping try_next_line()
                    continue;
                }

                if( split_line( ':' ) ) {
                    trim_whitespace( &token_left );
                    trim_whitespace( &token_right );

                    u64 token_left_hash  = string_slice_hash( &token_left );
                    u64 token_right_hash = string_slice_hash( &token_right );

                    if( token_left_hash == global_hash_type ) {
                        if( token_right_hash == global_hash_type_audio ) {
                            item.type = LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO;
                        } else if( token_right_hash == global_hash_type_model ) {
                            item.type = LIQUID_PACKAGE_RESOURCE_TYPE_MODEL;
                        } else if( token_right_hash == global_hash_type_texture ) {
                            item.type = LIQUID_PACKAGE_RESOURCE_TYPE_TEXTURE;
                        } else if( token_right_hash == global_hash_type_text ) {
                            item.type = LIQUID_PACKAGE_RESOURCE_TYPE_TEXT;
                        } else if( token_right_hash == global_hash_type_map ) {
                            item.type = LIQUID_PACKAGE_RESOURCE_TYPE_MAP;
                        } else {
                            m_error( "unrecognized token '{s}'!", token_right );
                        }
                    } else if( token_left_hash == global_hash_path ) {
                        if( token_right.len < 2 ) {
                            m_error( "invalid path: '{s}'!", token_right );
                            break;
                        }
                        string_slice_pop( &token_right, NULL );
                        string_slice_pop_start( &token_right, NULL );

                        text_push( &token_right, true );

                        item.path = token_right.buffer;
                    } else if( token_left_hash == global_hash_compression ) {
                        if( token_right_hash == global_hash_compression_rle ) {
                            item.compression = LIQUID_PACKAGE_COMPRESSION_RLE;
                        } else {
                            m_error( "unrecognized token '{s}'!", token_right );
                        }
                    } else {
                        m_error( "unrecognized token '{s}'!", token_left );
                    }

                } else {
                    m_error( "manifest is invalid!" );
                    manifest_error();
                    return false;
                }
            } break;
        }

        if( !next_line() ) {
            if( item.type ) {
                if( !list_push( &manifest_items, &item ) ) {
                    m_warn(
                        "excess items in manifest: {usize}", ++excess_items );
                }
            }
            break;
        }
    }

    // extract directory of manifest
    StringSlice path_slice = string_slice_from_cstr( 0, path );

    usize last_slash_position = 0;
    for( usize i = path_slice.len; i-- > 0; ) {
        if( path_slice.buffer[i] == '/' ) {
            last_slash_position = i;
            break;
        }
    }

    if( last_slash_position ) {
        path_slice.len      = last_slash_position;
        path_slice.capacity = path_slice.len;
    } else {
        path_slice.buffer = (char*)global_local_directory;
        path_slice.len = path_slice.capacity = 2;
    }

    out_manifest->directory = string_slice_clone( &path_slice );

    out_manifest->item_count       = item_count;
    out_manifest->items            = item_buffer;
    out_manifest->text_buffer      = text_buffer_stack.buffer;
    out_manifest->text_buffer_size = text_buffer_stack.buffer_size;

    return true;
}

void manifest_free( Manifest* manifest ) {
    system_free( manifest->items, sizeof(ManifestItem) * manifest->item_count );
    system_free( manifest->text_buffer, manifest->text_buffer_size );

    memory_zero( manifest, sizeof(*manifest) );
}

