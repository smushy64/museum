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

typedef enum LiquidPackageResourceType : u32 {
    LIQUID_PACKAGE_RESOURCE_TYPE_INVALID,
    LIQUID_PACKAGE_RESOURCE_TYPE_AUDIO,
    LIQUID_PACKAGE_RESOURCE_TYPE_MODEL,
    LIQUID_PACKAGE_RESOURCE_TYPE_TEXTURE,
    LIQUID_PACKAGE_RESOURCE_TYPE_TEXT,
    LIQUID_PACKAGE_RESOURCE_TYPE_MAP,
} LiquidPackageResourceType;
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
    if( type >= static_array_count( strings ) ) {
        return strings[0];
    }
    return strings[type];
}

typedef enum LiquidPackageResourceAudioCompression : u32 {
    LIQUID_PACKAGE_RESOURCE_AUDIO_COMPRESSION_NONE,
} LiquidPackageResourceAudioCompression;
header_only const char* liquid_package_resource_audio_compression_to_cstr(
    LiquidPackageResourceAudioCompression compression
) {
    switch( compression ) {
        case LIQUID_PACKAGE_RESOURCE_AUDIO_COMPRESSION_NONE:
            return "LIQUID_PACKAGE_RESOURCE_AUDIO_COMPRESSION_NONE";
    }
}

#define LIQUID_PACKAGE_RESOURCE_AUDIO_BYTES_PER_CHANNEL_SAMPLE (sizeof(i16))
#define LIQUID_PACKAGE_RESOURCE_AUDIO_SAMPLES_PER_SECOND (44100)
struct no_padding LiquidPackageResourceAudio {
    u32 right_channel_buffer_offset;
    u32 sample_count;
    LiquidPackageResourceAudioCompression compression;
};
header_only u32 liquid_package_resource_audio_number_of_channels(
    struct LiquidPackageResourceAudio* resource
) {
    return resource->right_channel_buffer_offset ? 2 : 1;
}
header_only u32 liquid_package_resource_audio_buffer_size(
    struct LiquidPackageResourceAudio* resource
) {
    u32 number_of_channels =
        liquid_package_resource_audio_number_of_channels( resource );
    return
        resource->sample_count *
        LIQUID_PACKAGE_RESOURCE_AUDIO_BYTES_PER_CHANNEL_SAMPLE *
        number_of_channels;
}

struct no_padding LiquidPackageResourceText {
    u32 len;
};

struct no_padding LiquidPackageResource {
    LiquidPackageResourceType type;
    u32 buffer_offset;
    union {
        struct LiquidPackageResourceAudio audio;
        struct LiquidPackageResourceText  text;
    };
};

header_only u32 liquid_package_calculate_resource_file_offset( u32 id ) {
    return sizeof(struct LiquidPackageHeader) +
        ( sizeof( struct LiquidPackageResource ) * id );
}

#endif /* header guard */
