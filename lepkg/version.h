#if !defined(LEPKG_VERSION_HPP)
#define LEPKG_VERSION_HPP
// * Description:  Version Validation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "defines.h"

/// Liquid Engine Package make version
#define LEPKG_MAKE_VERSION( major_u16, minor_u16 )\
    ( ((u32)major_u16 << 15ul) | (u32)minor_u16 )

/// Liquid Engine Package get major version
#define LEPKG_VERSION_GET_MAJOR( version_u32 )\
    ((u16)( version_u32 >> 15ul ))
/// Liquid Engine Package get minor version
#define LEPKG_VERSION_GET_MINOR( version_u32 )\
    ((u16)( version_u32 & (0x0000FFFF) ))

#if defined( LEPKG_INTERNAL )

b32 is_version_valid( u32 version );
b32 is_version_major_valid( u16 major );
b32 is_version_minor_valid( u16 minor );
b32 is_version_major_minor_valid( u16 major, u16 minor );

#endif

#endif // header guard
