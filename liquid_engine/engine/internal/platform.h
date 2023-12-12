#if !defined(LD_ENGINE_INTERNAL_PLATFORM_H)
#define LD_ENGINE_INTERNAL_PLATFORM_H
/**
 * Description:  Engine platform 
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 04, 2023
*/
#include "defines.h"

#if defined(LD_API_INTERNAL)

    #include "api.h"
    extern struct PlatformAPI* platform;

#endif

#endif /* header guard */
