// * Description:  Error Code Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "error.h"

const char* error_to_string( LEError error ) {
    const char* messages[] = {
        "No arguments provided",
        "Invalid arguments provided",
        "Required argument is missing",
        "Path is invalid",
        "Filetype is unrecognized",
        "Package validation failed",
        "Package version is unrecognized",
        "Compression level is invalid",
        "Memory allocation failed",
        "Packager failed"
    };

    if( !error ) {
        return "No error";
    }

    u32 index = (u32)(error - LE_ERROR_NO_ARGUMENTS);
    u32 messages_count = STATIC_ARRAY_COUNT( messages );
    if( index >= messages_count ) {
        return "Unknown error!";
    }

    return messages[index];
}

