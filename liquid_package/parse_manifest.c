/**
 * Description:  Parse manifests.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 19, 2023
*/
#include "defines.h"
#include "core/string.h"
#include "core/memory.h"
#include "core/internal.h"

#include "logging.h"
#include "shared_buffer.h"
#include "parse_manifest.h"

#include "liquid_package.h"

internal
b32 ___next_line( StringSlice* rest_of_file, StringSlice* out_line ) {
    if( !rest_of_file->len ) {
        return false;
    }

    if( rest_of_file->buffer[0] == '\n' ) {
        out_line->buffer = rest_of_file->buffer;
        out_line->len    = 1;

        rest_of_file->buffer++;
        rest_of_file->len--;
        return true;
    }

    usize end_of_line_position = 0;
    if( !string_slice_find_char( rest_of_file, '\n', &end_of_line_position ) ) {
        return false;
    }

    out_line->buffer = rest_of_file->buffer;
    out_line->len    = end_of_line_position;

    rest_of_file->buffer += end_of_line_position;
    rest_of_file->len    -= end_of_line_position;

    return true;
}

b32 packager_manifest_parse(
    List* list_manifest_resources, struct StackAllocator* stack, const char* path
) {
    usize path_len = cstr_len( path );
    string_slice_mut_capacity( directory, 256 );
    memory_copy( directory.buffer, path, path_len + 1 );
    directory.len = path_len;

    /* extract directory */ {
        Iterator directory_iter = string_slice_iterator( &directory );
        char  next = 0;
        usize trim_count = 0;
        while( iterator_reverse_next_value( &directory_iter, &next ) ) {
            if( next == '/' ) {
                directory.len -= trim_count;
                break;
            }

            trim_count++;
        }

        if( !directory.len ) {
            string_slice_const( default_path, "./" );
            memory_copy( directory.buffer, default_path.buffer, default_path.len );
            directory.len = default_path.len;
        }
        lp_note( "manifest '{cc}' directory: '{s}'", path, directory );
    }

    PlatformFile* file = platform->io.file_open(
        path, PLATFORM_FILE_READ |
        PLATFORM_FILE_SHARE_READ | PLATFORM_FILE_ONLY_EXISTING );
    if( !file ) {
        lp_error( "failed to open manifest file '{cc}'!", path );
        return false;
    }
    lp_note( "parsing manifest file '{cc}' . . .", path );

    usize manifest_size   = platform->io.file_query_size( file );
    void* manifest_buffer = package_shared_buffer_allocate( manifest_size );
    // should always work . . .
    assert( manifest_buffer );

    #define manifest_error( format, ... )\
        lp_error( "manifest: " format, ##__VA_ARGS__ );\
        package_shared_buffer_free( manifest_buffer, manifest_size );\
        return false;

    if( !platform->io.file_read( file, manifest_size, manifest_buffer ) ) {
        manifest_error( "failed to read manifest file '{cc}'!", path );
    }
    platform->io.file_close( file );

    StringSlice manifest = {};
    manifest.len      = manifest_size;
    manifest.capacity = manifest.len;
    manifest.buffer   = manifest_buffer;

    StringSlice line = {};
    if( !___next_line( &manifest, &line ) ) {
        manifest_error(
            "file '{cc}' is not a properly formatted manifest file!", path );
    }

    string_slice_const( token_manifest_identifier,   "liquid manifest" );
    string_slice_const( token_manifest_type_audio,   "[audio]" );
    string_slice_const( token_manifest_type_texture, "[texture]" );
    string_slice_const( token_manifest_type_model,   "[model]" );

    if( !string_slice_cmp( &line, &token_manifest_identifier ) ) {
        manifest_error(
            "file '{cc}' is not a properly formatted manifest file!", path );
    }

    // 2 because we already checked line one
    usize line_nr = 2;
    LiquidPackageResourceType type = LIQUID_PACKAGE_RESOURCE_TYPE_INVALID;
    while( ___next_line( &manifest, &line ) && line.len ) {
        char first = line.buffer[0];

        // skip empty lines and comments
        if( char_is_whitespace( first ) || first == '#' ) {
            goto cont;
        }

        if( first == '[' ) {
            if( string_slice_cmp( &line, &token_manifest_type_audio ) ) {
                type = LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO;
            } else if( string_slice_cmp( &line, &token_manifest_type_texture ) ) {
                type = LIQUID_PACKAGE_RESOURCE_TYPE_TEXTURE;
            } else if( string_slice_cmp( &line, &token_manifest_type_model ) ) {
                type = LIQUID_PACKAGE_RESOURCE_TYPE_3D_MODEL;
            }

            goto cont;
        }

        StringSlice resource_id = {}, resource_path = {};
        if( !string_slice_split_char( &line, ':', &resource_id, &resource_path ) ) {
            lp_warn( "improperly formatted line!" );
            lp_warn( "'{cc}':{usize} > {s}", path, line_nr, line );
            goto cont;
        }
        string_slice_trim_leading_whitespace( &resource_path, &resource_path );
        // trim './' from path
        if(
            resource_path.len >= 2         &&
            resource_path.buffer[0] == '.' &&
            resource_path.buffer[1] == '/'
        ){
            resource_path.buffer += 2;
            resource_path.len    -= 2;
        }

        usize required_stack_size =
            resource_id.len + resource_path.len + directory.len + 1;
        char* info_buffer = stack_allocator_push( stack, required_stack_size );

        // reallocate stack if it's not big enough
        // TODO(alicia): figure out how to do this more elegantly
        if( !info_buffer ) {
            usize old_stack_size = stack->buffer_size;
            usize new_stack_size = old_stack_size + required_stack_size + 256;

            void* new_stack_buffer =
                system_realloc( stack->buffer, old_stack_size, new_stack_size );
            if( !new_stack_buffer ) {
                manifest_error( "failed to reallocate stack!" );
            }

            stack->buffer      = new_stack_buffer;
            stack->buffer_size = new_stack_size;

            info_buffer = stack_allocator_push( stack, required_stack_size );
        }

        memory_copy( info_buffer, resource_id.buffer, resource_id.len );
        resource_id.buffer = info_buffer;

        // make path relative to manifest file.
        StringSlice processed_path = {};
        processed_path.buffer   = info_buffer + resource_id.len;
        processed_path.capacity = required_stack_size - resource_id.len - 1;

        string_slice_fmt( &processed_path, "{s}{s}", directory, resource_path );

        ManifestResource resource = {};
        resource.type = type;
        resource.id   = resource_id;
        resource.path = processed_path;

        // TODO(alicia): something better than this
        if( !list_push( list_manifest_resources, &resource ) ) {
            usize old_list_capacity = list_manifest_resources->capacity;
            usize new_list_capacity = old_list_capacity + 10;

            usize old_list_size = old_list_capacity * sizeof(resource);
            usize new_list_size = new_list_capacity * sizeof(resource);

            void* new_list_buffer =
                system_realloc( list_manifest_resources->buffer,
                    old_list_size, new_list_size );
            if( !new_list_buffer ) {
                manifest_error( "failed to reallocate resources list!" );
            }

            list_manifest_resources->buffer   = new_list_buffer;
            list_manifest_resources->capacity = new_list_capacity;

            list_push( list_manifest_resources, &resource );
        }
    cont:
        line_nr++;
        continue;
    }

    package_shared_buffer_free( manifest_buffer, manifest_size );
    return true;
}

