/**
 * Description:  Liquid Engine Linux Platform Layer
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 23, 2023
*/
#include "shared/defines.h"

#if defined(LD_PLATFORM_LINUX)

#include "api.h"
#include "custom_cstd.h"


#undef CORE_API
#define CORE_API

int main( int argc, char** argv ) {
    PlatformAPI api = {};

    // TODO(alicia): 
    CoreInitFN* core_init = NULL;

    int return_code = core_init( argc, argv, &api );

    return return_code;
}

#include "custom_cstd.c"

#endif /* Platform Linux */

