#if !defined(CORE_LIBRARY_HPP)
#define CORE_LIBRARY_HPP
// * Description:  Core Library Functions
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "defines.h"

typedef void* OpaqueLibraryHandle;

#if defined(LD_LOGGING)
    #define LIB_MAX_PATH_STORAGE (255)
#endif
struct LibraryHandle {
#if defined(LD_LOGGING)
    char path_storage[LIB_MAX_PATH_STORAGE];
#endif
    OpaqueLibraryHandle handle;
};
namespace impl {
    /// Load local library or from environment path.
    LD_API b32 _library_load(
        const char*    library_path,
        LibraryHandle* out_library
    );
    /// Load local library or from environment path.
    LD_API b32 _library_load_trace(
        const char*    library_path,
        LibraryHandle* out_library,
        const char* function,
        const char* file,
        i32 line
    );
    /// Free a library handle.
    LD_API void _library_free( LibraryHandle* library );
    /// Free a library handle.
    LD_API void _library_free_trace( 
        LibraryHandle* library,
        const char* function,
        const char* file,
        i32 line
    );
    /// Load a function from a library.
    LD_API void* _library_load_function(
        LibraryHandle* library,
        const char* function_name
    );
    /// Load a function from a library.
    LD_API void* _library_load_function_trace(
        LibraryHandle* library,
        const char* function_name,
        const char* function,
        const char* file,
        i32 line
    );
} // namespace impl

#if defined(LD_LOGGING)
    #define library_load( library_path, out_library )\
        ::impl::_library_load_trace(\
            library_path,\
            out_library,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define library_free( library )\
        ::impl::_library_free_trace(\
            library,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
    #define library_load_function( library, function_name )\
        ::impl::_library_load_function_trace(\
            library,\
            function_name,\
            __FUNCTION__,\
            __FILE__,\
            __LINE__\
        )
#else
    #define library_load( library_path, out_library )\
        ::impl::_library_load( library_path, out_library )
    #define library_free( library )\
        ::impl::_library_free( library )
    #define library_load_function( library, function_name )\
        ::impl::_library_load_function( library, function_name )
#endif

#endif // header guard
