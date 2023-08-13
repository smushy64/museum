#if !defined(LEPKG_ERROR_HPP)
#define LEPKG_ERROR_HPP
// * Description:  Error Codes
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 11, 2023
#include "defines.h"

typedef enum : i32 {
    LE_ERROR_NO_ARGUMENTS      = -127,
    LE_ERROR_INVALID_ARGUMENTS,
    LE_ERROR_MISSING_ARGUMENT,
    LE_ERROR_INVALID_PATH,
    LE_ERROR_UNRECOGNIZED_FILETYPE,
    LE_ERROR_VALIDATION_FAILED,
    LE_ERROR_UNRECOGNIZED_VERSION,
    LE_ERROR_INVALID_COMPRESSION,
    LE_ERROR_ALLOCATION_FAILED,
    LE_ERROR_PACKAGE_FAILED,

    LE_ERROR_NO_ERROR = 0
} LEError;

const char* error_to_string( LEError error );

#endif // header guard
