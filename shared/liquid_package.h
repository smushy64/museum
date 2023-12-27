#if !defined(LD_SHARED_LIQUID_PACKAGE_H)
#define LD_SHARED_LIQUID_PACKAGE_H
/**
 * Description:  Resource file header.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 07, 2023
*/
#include "defines.h"

#define LIQUID_PACKAGE_FILE_IDENTIFIER ( ( 'G' << 24u ) | ( 'K' << 16u ) |  ( 'P' << 8u ) | 'L' )

struct no_padding LiquidPackageHeader {
    /// must == LIQUID_PACKAGE_FILE_IDENTIFIER
    u32 identifier;
    u32 resource_count;
};
#define LIQUID_PACKAGE_RESOURCE_BUFFER_OFFSET (sizeof(struct LiquidPackageHeader))
#define liquid_package_buffer_offset( header )\
    ( header->resource_count * sizeof( struct LiquidPackageResource ) )

typedef enum LiquidPackageResourceType : u8 {
    LIQUID_PACKAGE_RESOURCE_TYPE_INVALID,
    LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO,
    LIQUID_PACKAGE_RESOURCE_TYPE_MODEL,
    LIQUID_PACKAGE_RESOURCE_TYPE_TEXTURE,
    LIQUID_PACKAGE_RESOURCE_TYPE_TEXT,
    LIQUID_PACKAGE_RESOURCE_TYPE_MAP,
} LiquidPackageResourceType;
header_only const char* liquid_package_resource_type_to_cstr(
    LiquidPackageResourceType type
) {
    const char* strings[] = {
        "Invalid",
        "Audio",
        "Model",
        "Texture",
        "Text",
        "Map"
    };
    return strings[type];
}
header_only const char* liquid_package_resource_type_to_identifier(
    LiquidPackageResourceType type
) {
    const char* strings[] = {
        "RESOURCE_INVALID",
        "RESOURCE_AUDIO",
        "RESOURCE_MODEL",
        "RESOURCE_TEXTURE",
        "RESOURCE_TEXT",
        "RESOURCE_MAP"
    };
    return strings[type];
}

typedef enum LiquidPackageCompression : u8 {
    LIQUID_PACKAGE_COMPRESSION_NONE,
    LIQUID_PACKAGE_COMPRESSION_RLE,
} LiquidPackageCompression;
header_only const char* liquid_package_compression_to_cstr(
    LiquidPackageCompression compression
) {
    const char* strings[] = {
        "None",
        "Run-Length Encoding"
    };
    return strings[compression];
}

#define LIQUID_PACKAGE_RESOURCE_AUDIO_BYTES_PER_CHANNEL_SAMPLE (sizeof(i16))
#define LIQUID_PACKAGE_RESOURCE_AUDIO_SAMPLES_PER_SECOND (44100)
struct no_padding LiquidPackageResourceAudio {
    u32 right_channel_buffer_offset;
    u32 sample_count;
};
header_only u32 liquid_package_resource_audio_number_of_channels(
    struct LiquidPackageResourceAudio* resource
) {
    return resource->right_channel_buffer_offset ? 2 : 1;
}

struct no_padding LiquidPackageResource {
    LiquidPackageResourceType type;
    LiquidPackageCompression  compression;
    u32 offset;
    u32 size;
    union {
        struct LiquidPackageResourceAudio audio;
    };
};

header_only u32 liquid_package_calculate_resource_file_offset( u32 id ) {
    return sizeof( struct LiquidPackageHeader ) +
        ( sizeof( struct LiquidPackageResource ) * id );
}

#endif /* header guard */
