#if !defined(CORE_LIBRARY_HPP)
#define CORE_LIBRARY_HPP
// * Description:  Core Library Functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "defines.h"

typedef void OpaqueDynamicLibrary;

#if defined(LD_LOGGING)
    #define LIB_MAX_PATH_STORAGE (255)
#endif

typedef struct DynamicLibrary {

#if defined(LD_LOGGING)
    char path_storage[LIB_MAX_PATH_STORAGE];
#endif
    OpaqueDynamicLibrary* handle;

} DynamicLibrary;

/// Load local library or from environment path.
LD_API b32 _library_load(
    const char*    library_path,
    DynamicLibrary* out_library
);
/// Load local library or from environment path.
LD_API b32 _library_load_trace(
    const char*    library_path,
    DynamicLibrary* out_library,
    const char* function,
    const char* file,
    i32 line
);
/// Free a library handle.
LD_API void _library_free( DynamicLibrary* library );
/// Free a library handle.
LD_API void _library_free_trace( 
    DynamicLibrary* library,
    const char* function,
    const char* file,
    i32 line
);
/// Load a function from a library.
LD_API void* _library_load_function(
    DynamicLibrary* library,
    const char* function_name
);
/// Load a function from a library.
LD_API void* _library_load_function_trace(
    DynamicLibrary* library,
    const char* function_name,
    const char* function,
    const char* file,
    i32 line
);

#if defined(LD_LOGGING)
    #define library_load( library_path, out_library )\
        _library_load_trace(\
            library_path,\
            out_library,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define library_free( library )\
        _library_free_trace(\
            library,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define library_load_function( library, function_name )\
        _library_load_function_trace(\
            library,\
            function_name,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define library_load( library_path, out_library )\
        _library_load( library_path, out_library )
    #define library_free( library )\
        _library_free( library )
    #define library_load_function( library, function_name )\
        _library_load_function( library, function_name )
#endif

#endif // header guard
