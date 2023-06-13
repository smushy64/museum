#if !defined(PLATFORM_FLAGS_HPP)
#define PLATFORM_FLAGS_HPP
/**
 * Description:  Flags for initializing the platform layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: May 02, 2023
*/
#include "defines.h"

typedef u32 PlatformFlags;
/// Try to make application DPI aware.
#define PLATFORM_DPI_AWARE ( 1 << 0 )
/// Pause application when surface is inactive.
#define PLATFORM_PAUSE_ON_SURFACE_INACTIVE ( 1 << 1 )

#endif
