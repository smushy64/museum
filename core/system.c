/**
 * Description:  System implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 03, 2024
*/
#include "shared/defines.h"
#include "core/system.h"
#include "core/internal/platform.h"

CORE_API void system_info_query( SystemInfo* out_info ) {
    platform_system_info_query( out_info );
}


