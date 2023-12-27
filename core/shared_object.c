/**
 * Description:  Shared object implementation.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 04, 2023
*/
#include "shared/defines.h"
#include "core/shared_object.h"
#include "core/internal/platform.h"

CORE_API SharedObject* shared_object_open( const char* path ) {
    return platform_shared_object_open( path );
}
CORE_API void shared_object_close( SharedObject* object ) {
    platform_shared_object_close( object );
}
CORE_API void* shared_object_load( SharedObject* object, const char* function_name ) {
    return platform_shared_object_load( object, function_name );
}


