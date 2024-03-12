#if !defined(UNPACK_ERROR_H)
#define UNPACK_ERROR_H
/**
 * Description:  Unpack utility error codes.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: February 05, 2024
*/
#include "core/print.h" // IWYU pragma: keep

typedef enum UnpackError : int {
    UNPACK_SUCCESS,
    UNPACK_ERROR_UNRECOGNIZED_ARGUMENT = 128,
    UNPACK_ERROR_NO_ARGUMENTS,
    UNPACK_ERROR_INVALID_ARGUMENT,
    UNPACK_ERROR_MISSING_PACKAGE_PATH,
    UNPACK_ERROR_INVALID_PATH,
    UNPACK_ERROR_FILE_OPEN,
    UNPACK_ERROR_FILE_READ,
    UNPACK_ERROR_INVALID_RESOURCE,
    UNPACK_ERROR_OUT_OF_MEMORY,
    UNPACK_ERROR_CREATE_SURFACE,
} UnpackError;

#define error( format, ... )\
    println_err( CONSOLE_COLOR_RED format CONSOLE_COLOR_RESET, ##__VA_ARGS__ )

#endif /* header guard */
