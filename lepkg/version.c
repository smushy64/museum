// * Description:  Version Validation Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "version.h"

#define LEPKG_MIN_MAJOR_VERSION (0)
#define LEPKG_MAX_MAJOR_VERSION (LEPKG_VERSION_MAJOR)

#define LEPKG_MIN_MINOR_VERSION (1)
#define LEPKG_MAX_MINOR_VERSION (LEPKG_VERSION_MINOR)

b32 is_version_major_valid( u16 major ) {
    return
        major >= LEPKG_MIN_MAJOR_VERSION &&
        major <= LEPKG_MAX_MAJOR_VERSION;
}
b32 is_version_minor_valid( u16 minor ) {
    return
        minor >= LEPKG_MIN_MINOR_VERSION &&
        minor <= LEPKG_MAX_MINOR_VERSION;
}
b32 is_version_major_minor_valid( u16 major, u16 minor ) {
    return
        is_version_major_valid( major ) &&
        is_version_minor_valid( minor );
}
b32 is_version_valid( u32 version ) {
    u16 major = LEPKG_VERSION_GET_MAJOR( version );
    u16 minor = LEPKG_VERSION_GET_MINOR( version );
    return is_version_major_minor_valid( major, minor );
}

