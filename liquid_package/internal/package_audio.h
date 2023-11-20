#if !defined(LP_PACKAGE_AUDIO_H)
#define LP_PACKAGE_AUDIO_H
/**
 * Description:  Liquid Packager audio
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: November 05, 2023
*/
#include "defines.h"

typedef enum : u32 {
    PACKAGE_AUDIO_SUCCESS,
    PACKAGE_AUDIO_ERROR_UNKNOWN,
    PACKAGE_AUDIO_ERROR_UNRECOGNIZED_EXTENSION,
    PACKAGE_AUDIO_ERROR_OPEN_RESOURCE_FILE,
    PACKAGE_AUDIO_ERROR_READ_RESOURCE_FILE,
    PACKAGE_AUDIO_ERROR_INVALID_FILE,
    PACKAGE_AUDIO_ERROR_UNSUPPORTED_WAVE_FORMAT,
    PACKAGE_AUDIO_ERROR_OUT_OF_MEMORY,
} PackageAudioError;

header_only const char* package_audio_error_to_cstr( PackageAudioError error ) {
    switch( error ) {
        case PACKAGE_AUDIO_SUCCESS                       : return "Success.";
        case PACKAGE_AUDIO_ERROR_UNRECOGNIZED_EXTENSION  : return "Unrecognized file extension.";
        case PACKAGE_AUDIO_ERROR_OPEN_RESOURCE_FILE      : return "Failed to open file.";
        case PACKAGE_AUDIO_ERROR_READ_RESOURCE_FILE      : return "Failed to read file.";
        case PACKAGE_AUDIO_ERROR_INVALID_FILE            : return "File is invalid.";
        case PACKAGE_AUDIO_ERROR_UNSUPPORTED_WAVE_FORMAT : return "Wave format is not supported.";
        case PACKAGE_AUDIO_ERROR_OUT_OF_MEMORY           : return "Ran out of memory.";
        default: return "Unknown.";
    }
}

struct StringSlice;
struct LiquidPackageResourceAudio;
PackageAudioError package_audio(
    struct StringSlice* path, struct LiquidPackageResourceAudio* out_resource,
    usize* out_buffer_size, void** out_buffer );

#endif /* header guard */
