// * Description:  Path Processing Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "path.h"
#include <stdio.h>
#include <string.h>

b32 does_file_exist( const char* path ) {
    FILE* file = fopen( path, "rb" );
    if( file ) {
        fclose( file );
        return true;
    } else {
        return false;
    }

}

const char* get_file_ext( const char* path ) {
    if( !path ) {
        return path;
    }
    i32 len = strlen( path );
    for( i32 i = len; i > 0; --i ) {
        if( path[i] == '.' ) {
            return &path[i];
        }
    }

    return NULL;
}

global const char* SUPPORTED_EXTENSIONS[] = {
    ".lescn",
    ".bmp", ".png", ".psd",
    ".ttf",
    ".wav",
    ".obj", ".gltf", ".blend", ".fbx",
    ".spv"
};

global AssetType EXTENSION_ASSET_TYPES[] = {
    ASSET_TYPE_METADATA,
    ASSET_TYPE_IMAGE, ASSET_TYPE_IMAGE, ASSET_TYPE_IMAGE,
    ASSET_TYPE_FONT,
    ASSET_TYPE_AUDIO,
    ASSET_TYPE_MODEL_3D, ASSET_TYPE_MODEL_3D, ASSET_TYPE_MODEL_3D, ASSET_TYPE_MODEL_3D,
    ASSET_TYPE_SHADER
};

AssetType get_file_asset_type( const char* path ) {
    const char* ext = get_file_ext( path );
    if( !ext ) {
        return false;
    }

    u32 ext_count = STATIC_ARRAY_COUNT( SUPPORTED_EXTENSIONS );
    for( u32 i = 0; i < ext_count; ++i ) {
        if( strcmp( ext, SUPPORTED_EXTENSIONS[i] ) == 0 ) {
            return EXTENSION_ASSET_TYPES[i];
        }
    }

    return ASSET_TYPE_UNKNOWN;
}

