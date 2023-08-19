// * Description:  Core Library Functions Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "core/ldlibrary.h"
#include "core/ldlog.h"
#include "core/ldstring.h"
#include "ldplatform.h"

#if defined(LD_LOGGING)
    #include "core/ldmemory.h"
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

LD_API b32 _library_load(
    const char*    library_path,
    DynamicLibrary* out_library
) {
    PlatformLibrary* handle = platform_library_load( library_path );
    if( !handle ) {
        return false;
    }

    out_library->handle = handle;

    return true;
}
LD_API b32 _library_load_trace(
    const char*    library_path,
    DynamicLibrary* out_library,
    const char* function,
    const char* file,
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

    b32 load_result = _library_load( library_path, out_library );
    if( load_result ) {
        LOG_NOTE_LIBRARY_LOAD( library_path );
    } else {
        LOG_ERROR_LIBRARY_LOAD( library_path );
    }

    return load_result;
}
LD_API void _library_free( DynamicLibrary* library ) {
    platform_library_free( library->handle );
    mem_zero( library, sizeof(DynamicLibrary) );
}
LD_API void _library_free_trace( 
    DynamicLibrary* library,
    const char* function,
    const char* file,
    i32 line
) {
    platform_library_free( library->handle );
#if defined(LD_LOGGING)
    LOG_NOTE_LIBRARY_FREE( library->path_storage );
#endif
    mem_zero( library, sizeof(DynamicLibrary) );
}
LD_API void* _library_load_function(
    DynamicLibrary* library,
    const char* function_name
) {
    return platform_library_load_function(
        library->handle,
        function_name
    );
}
LD_API void* _library_load_function_trace(
    DynamicLibrary* library,
    const char* function_name,
    const char* function,
    const char* file,
    i32 line
) {
    void* load_result = _library_load_function(
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
