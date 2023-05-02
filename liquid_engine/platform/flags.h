#if !defined(PLATFORM_FLAGS_HPP)
#define PLATFORM_FLAGS_HPP
/**
 * Description:  Flags for creating a surface
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "defines.h"

typedef u32 SurfaceCreateFlags;
#define SURFACE_CREATE_VISIBLE  ( 1 << 1 )
#define SURFACE_CREATE_CENTERED ( 1 << 2 )

typedef u32 PlatformInitFlags;
#define PLATFORM_DPI_AWARE ( 1 << 0 )

#endif
