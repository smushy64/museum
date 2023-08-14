#if !defined(LEPKG_PATH_HPP)
#define LEPKG_PATH_HPP
// * Description:  Path Processing
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "defines.h"
#include "format.h"
#include <stdio.h>

b32 does_file_exist( const char* path );
const char* get_file_ext( const char* path );

typedef struct {
    b32       is_supported;
    AssetType asset_type;
    union {
        MetadataType  metadata_type;
        ImageFormat   image_format;
        FontFormat    font_format;
        AudioFormat   audio_format;
        Model3DFormat model3d_format;
        ShaderFormat  shader_format;
        u8 generic_format;
    };
} FiletypeInfo;

FiletypeInfo get_file_asset_info( const char* path );

usize get_file_size( FILE* file );

#endif // header guard
