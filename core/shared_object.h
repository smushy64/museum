#if !defined(LD_CORE_SHARED_OBJECT_H)
#define LD_CORE_SHARED_OBJECT_H
/**
 * Description:  Shared object
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: December 04, 2023
*/
#include "shared/defines.h"

/// Opaque handle to a shared object.
typedef void SharedObject;

/// Open a shared object.
CORE_API SharedObject* shared_object_open( const char* path );
/// Close a shared object.
CORE_API void shared_object_close( SharedObject* object );
/// Load a function from a shared object.
CORE_API void* shared_object_load( SharedObject* object, const char* function_name );

#endif /* header guard */
