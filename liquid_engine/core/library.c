// * Description:  Core Library Functions Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "library.h"
#include "logging.h"
#include "string.h"
#include "platform/platform.h"

#if defined(LD_LOGGING)
    #include "memory.h"
#endif

#define LOG_NOTE_LIBRARY_LOAD( library_path )\
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_GREEN,\
        LOG_FLAG_NEW_LINE,\
        "[LIBLOAD | {cc}() | {cc}:{i}] "\
        "Library \"{cc}\" loaded successfully.",\
        function, file, line, library_path\
    )

#define LOG_ERROR_LIBRARY_LOAD( library_path )\
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        LOG_COLOR_RED,\
        LOG_FLAG_ALWAYS_PRINT | LOG_FLAG_NEW_LINE,\
        "[LIBLOAD ERROR | {cc}() | {cc}:{i}] "\
        "Failed to load library \"{cc}\"!",\
        function, file, line, library_path\
    )

#define LOG_NOTE_LIBRARY_FREE( library_path )\
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_CYAN,\
        LOG_FLAG_NEW_LINE,\
        "[LIBFREE | {cc}() | {cc}:{i}] "\
        "Library \"{cc}\" freed.",\
        function, file, line, library_path\
    )

#define LOG_NOTE_FUNCTION_LOAD( library_path, function_name )\
    log_formatted_locked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        LOG_COLOR_GREEN,\
        LOG_FLAG_NEW_LINE,\
        "[FNLOAD | {cc}() | {cc}:{i}] "\
        "Function \"{cc}\" loaded from library \"{cc}\" successfully.",\
        function, file, line, function_name, library_path\
    )

#define LOG_ERROR_FUNCTION_LOAD( library_path, function_name )\
    log_formatted_locked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        LOG_COLOR_RED,\
        LOG_FLAG_ALWAYS_PRINT | LOG_FLAG_NEW_LINE,\
        "[FNLOAD ERROR | {cc}() | {cc}:{i}] "\
        "Failed to load function \"{cc}\" from library \"{cc}\"!",\
        function, file, line, function_name, library_path\
    )

LD_API b32 impl::_library_load(
    const char*    library_path,
    LibraryHandle* out_library
) {
    PlatformLibraryHandle handle = platform_library_load( library_path );
    if( !handle ) {
        return false;
    }

    out_library->handle = handle;

    return true;
}
LD_API b32 impl::_library_load_trace(
    const char*    library_path,
    LibraryHandle* out_library,
    [[maybe_unused]]
    const char* function,
    [[maybe_unused]]
    const char* file,
    [[maybe_unused]]
    i32 line
) {
#if defined(LD_LOGGING)
    usize path_len = str_length( library_path ) + 1;
    if( path_len >= LIB_MAX_PATH_STORAGE ) {
        path_len = LIB_MAX_PATH_STORAGE;
    }
    mem_copy( out_library->path_storage, library_path, path_len );
    out_library->path_storage[LIB_MAX_PATH_STORAGE - 1] = 0;
#endif

    b32 load_result = impl::_library_load( library_path, out_library );
    if( load_result ) {
        LOG_NOTE_LIBRARY_LOAD( library_path );
    } else {
        LOG_ERROR_LIBRARY_LOAD( library_path );
    }

    return load_result;
}
LD_API void impl::_library_free( LibraryHandle* library ) {
    platform_library_free( library->handle );
    *library = {};
}
LD_API void impl::_library_free_trace( 
    LibraryHandle* library,
    [[maybe_unused]]
    const char* function,
    [[maybe_unused]]
    const char* file,
    [[maybe_unused]]
    i32 line
) {
    platform_library_free( library->handle );
#if defined(LD_LOGGING)
    LOG_NOTE_LIBRARY_FREE( library->path_storage );
#endif
    *library = {};
}
LD_API void* impl::_library_load_function(
    LibraryHandle* library,
    const char* function_name
) {
    return platform_library_load_function(
        library->handle,
        function_name
    );
}
LD_API void* impl::_library_load_function_trace(
    LibraryHandle* library,
    const char* function_name,
    [[maybe_unused]]
    const char* function,
    [[maybe_unused]]
    const char* file,
    [[maybe_unused]]
    i32 line
) {
    void* load_result = impl::_library_load_function(
        library, function_name
    );

#if defined(LD_LOGGING)
    if( load_result ) {
        LOG_NOTE_FUNCTION_LOAD( library->path_storage, function_name );
    } else {
        LOG_ERROR_FUNCTION_LOAD( library->path_storage, function_name );
    }
#endif

    return load_result;
}

