#if !defined(LD_SHARED_LIQUID_PACKAGE_H)
#define LD_SHARED_LIQUID_PACKAGE_H
/**
 * Description:  Resource file header.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 07, 2023
*/
#include "defines.h"
#include "constants.h"

#define package_make_id( str4 )\
    ( ( str4[3] << 24u ) | ( str4[2] << 16u ) | ( str4[1] << 8u ) | ( str4[0] ) )

#define RESOURCE_INVALID (0)

#define PACKAGE_ID package_make_id( "LPKG" )
#define PACKAGE_MAX_RESOURCE_COUNT (U32_MAX)
struct no_padding PackageHeader {
    union { char id_c[4]; u32 id; };
    u32 resource_count;
};

#define PACKAGE_RESOURCES_OFFSET (sizeof(struct PackageHeader))
#define package_buffer_offset( header )\
    ( (header)->resource_count * PACKAGE_RESOURCE_OFFSET )

typedef enum PackageResourceType : u8 {
    PACKAGE_RESOURCE_TYPE_INVALID,
    PACKAGE_RESOURCE_TYPE_AUDIO,
    PACKAGE_RESOURCE_TYPE_MESH,
    PACKAGE_RESOURCE_TYPE_TEXTURE,
    PACKAGE_RESOURCE_TYPE_TEXT,
} PackageResourceType;
#define PACKAGE_RESOURCE_TYPE_COUNT (((u8)PACKAGE_RESOURCE_TYPE_TEXT) + 1)

#define result(text)\
    if( opt_out_len ) *opt_out_len = sizeof(text) - 1; return text
header_only const char* package_resource_type_to_cstr(
    PackageResourceType type, usize* opt_out_len
) {
    switch( type ) {
        case PACKAGE_RESOURCE_TYPE_INVALID: result( "Invalid" );
        case PACKAGE_RESOURCE_TYPE_AUDIO:   result( "Audio"   );
        case PACKAGE_RESOURCE_TYPE_MESH:    result( "Mesh"    );
        case PACKAGE_RESOURCE_TYPE_TEXTURE: result( "Texture" );
        case PACKAGE_RESOURCE_TYPE_TEXT:    result( "Text"    );
    }
}

// NOTE(alicia): as a rule, compressed buffers have the original size
// of the buffer written as a u64 at the start of the buffer.
typedef enum PackageCompression : u8 {
    PACKAGE_COMPRESSION_NONE,
    PACKAGE_COMPRESSION_RLE,
} PackageCompression;

header_only const char* package_compression_to_cstr(
    PackageCompression compression, usize* opt_out_len
) {
    switch( compression ) {
        case PACKAGE_COMPRESSION_NONE: result( "None" );
        case PACKAGE_COMPRESSION_RLE:  result( "RLE" );
    }
}

#define PACKAGE_AUDIO_SAMPLES_PER_SECOND (44100)
struct no_padding PackageAudio {
    u8  channel_count;
    u8  bytes_per_sample;
    u32 samples_per_second;
};

typedef enum PackageTextLanguage : u8 {
    PACKAGE_TEXT_LANGUAGE_ENGLISH,
} PackageTextLanguage;
header_only const char* package_text_lang_to_cstr(
    PackageTextLanguage lang, usize* opt_out_len
) {
    switch( lang ) {
        case PACKAGE_TEXT_LANGUAGE_ENGLISH: result( "English" );
    }
}

typedef enum PackageTextEncoding : u8 {
    PACKAGE_TEXT_ENCODING_UTF8,
} PackageTextEncoding;
header_only const char* package_text_encoding_to_cstr(
    PackageTextEncoding encoding, usize* opt_out_len
) {
    switch( encoding ) {
        case PACKAGE_TEXT_ENCODING_UTF8: result( "UTF-8" );
    }
}

struct no_padding PackageText {
    PackageTextLanguage lang;
    PackageTextEncoding encoding;
};

typedef enum PackageTextureChannels : u8 {
    PACKAGE_TEXTURE_CHANNEL_R,
    PACKAGE_TEXTURE_CHANNEL_RG,
    PACKAGE_TEXTURE_CHANNEL_RGB,
    PACKAGE_TEXTURE_CHANNEL_RGBA,
} PackageTextureChannels;
header_only const char* package_texture_channels_to_cstr(
    PackageTextureChannels channels, usize* opt_out_len
) {
    switch( channels ) {
        case PACKAGE_TEXTURE_CHANNEL_R:    result("Red");
        case PACKAGE_TEXTURE_CHANNEL_RG:   result("Red/Green");
        case PACKAGE_TEXTURE_CHANNEL_RGB:  result("Red/Green/Blue");
        case PACKAGE_TEXTURE_CHANNEL_RGBA: result("Red/Green/Blue/Alpha");
    }
}

typedef enum PackageTextureDimensions : u8 {
    PACKAGE_TEXTURE_DIMENSIONS_1,
    PACKAGE_TEXTURE_DIMENSIONS_2,
    PACKAGE_TEXTURE_DIMENSIONS_3,
} PackageTextureDimensions;
header_only const char* package_texture_dimensions_to_cstr(
    PackageTextureDimensions dimensions, usize* opt_out_len
) {
    switch( dimensions ) {
        case PACKAGE_TEXTURE_DIMENSIONS_1: result("1D");
        case PACKAGE_TEXTURE_DIMENSIONS_2: result("2D");
        case PACKAGE_TEXTURE_DIMENSIONS_3: result("3D");
    }
}

typedef enum PackageTextureBaseType : u8 {
    PACKAGE_TEXTURE_BASE_TYPE_8BIT,
    PACKAGE_TEXTURE_BASE_TYPE_16BIT,
    PACKAGE_TEXTURE_BASE_TYPE_32BIT,
    PACKAGE_TEXTURE_BASE_TYPE_FLOAT,
} PackageTextureBaseType;
header_only const char* package_texture_base_type_to_cstr(
    PackageTextureBaseType type, usize* opt_out_len
) {
    switch( type ) {
        case PACKAGE_TEXTURE_BASE_TYPE_8BIT:  result("8-bit");
        case PACKAGE_TEXTURE_BASE_TYPE_16BIT: result("16-bit");
        case PACKAGE_TEXTURE_BASE_TYPE_32BIT: result("32-bit");
        case PACKAGE_TEXTURE_BASE_TYPE_FLOAT: result("float");
    }
}
header_only u32 package_texture_base_type_size( PackageTextureBaseType type ) {
    switch( type ) {
        case PACKAGE_TEXTURE_BASE_TYPE_8BIT:  return sizeof(u8);
        case PACKAGE_TEXTURE_BASE_TYPE_16BIT: return sizeof(u16);
        case PACKAGE_TEXTURE_BASE_TYPE_32BIT:
        case PACKAGE_TEXTURE_BASE_TYPE_FLOAT: return sizeof(u32);
    }
}

struct PackageTextureType {
    PackageTextureChannels   channels   : 2;
    PackageTextureDimensions dimensions : 2;
    PackageTextureBaseType   base_type  : 2;

    u8 ___padding : 2;
};

typedef u8 PackageTextureFlags;
#define PACKAGE_TEXTURE_FLAG_BILINEAR_FILTER (1 << 0)
#define PACKAGE_TEXTURE_FLAG_WRAP_X_REPEAT   (1 << 1)
#define PACKAGE_TEXTURE_FLAG_WRAP_Y_REPEAT   (1 << 2)
#define PACKAGE_TEXTURE_FLAG_WRAP_Z_REPEAT   (1 << 3)
#define PACKAGE_TEXTURE_FLAG_TRANSPARENT     (1 << 4)

struct no_padding PackageTexture {
    struct PackageTextureType type;
    PackageTextureFlags       flags;
    u32 width, height, depth;
};
header_only u32 package_texture_area( struct PackageTexture* texture ) {
    switch( texture->type.dimensions ) {
        case PACKAGE_TEXTURE_DIMENSIONS_1:
            return texture->width;
        case PACKAGE_TEXTURE_DIMENSIONS_2:
            return texture->width * texture->height;
        case PACKAGE_TEXTURE_DIMENSIONS_3:
            return texture->width * texture->height * texture->depth;
    }
}
header_only u32 package_texture_total_size( struct PackageTexture* texture ) {
    u32 channel_count  = texture->type.channels   + 1;
    u32 base_type_size = package_texture_base_type_size( texture->type.base_type );
    u32 area           = package_texture_area( texture );

    return channel_count * base_type_size * area;
}

typedef enum PackageMeshLayout : u8 {
    PACKAGE_MESH_LAYOUT_3D_DEFAULT,
    PACKAGE_MESH_LAYOUT_2D_DEFAULT,
    PACKAGE_MESH_LAYOUT_UI,
} PackageMeshLayout;

typedef struct no_padding PackageMesh {
    PackageMeshLayout memory_layout;
} PackageMesh;

struct no_padding PackageResource {
    PackageResourceType type;
    PackageCompression  compression;
    u32 offset, size;
    union {
        struct PackageAudio   audio;
        struct PackageText    text;
        struct PackageTexture texture;
        struct PackageMesh    mesh;
        struct { u64 padding_0[2]; } padding;
    };
};

header_only u32 package_resource_offset( u32 id ) {
    return PACKAGE_RESOURCES_OFFSET + ( sizeof( struct PackageResource ) * id );
}

header_only u32 package_audio_channel_size( struct PackageResource* resource ) {
    assert( resource->type == PACKAGE_RESOURCE_TYPE_AUDIO );
    return resource->size / (u32)resource->audio.channel_count;
}
header_only u32 package_audio_channel_offset(
    struct PackageResource* resource, u32 channel
) {
    assert( resource->type == PACKAGE_RESOURCE_TYPE_AUDIO );
    assert( channel < resource->audio.channel_count );

    return resource->offset + ( package_audio_channel_size( resource ) * channel);
}
header_only u32 package_audio_channel_sample_count( struct PackageResource* resource ) {
    assert( resource->type == PACKAGE_RESOURCE_TYPE_AUDIO );

    return package_audio_channel_size( resource ) / resource->audio.bytes_per_sample;
}
header_only f64 package_audio_length_seconds( struct PackageResource* resource ) {
    assert( resource->type == PACKAGE_RESOURCE_TYPE_AUDIO );
    u32 channel_samples = package_audio_channel_sample_count( resource );
    f64 length = (f64)channel_samples / (f64)resource->audio.samples_per_second;
    return length;
}

#undef result
#endif /* header guard */
