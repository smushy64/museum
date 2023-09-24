// * Description:  Core Library Functions Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: July 22, 2023
#include "core/library.h"
#include "core/log.h"
#include "core/cstr.h"
#include "internal.h"

#if defined(LD_LOGGING)
    #include "core/mem.h"
#endif

#define LOG_NOTE_LIBRARY_LOAD( library_path )\
    log_formatted_unlocked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        false, true,\
        LOG_COLOR_GREEN\
        "[LIBLOAD | {cc}() | {cc}:{i}] "\
        "Library \"{cc}\" loaded successfully."\
        LOG_COLOR_RESET,\
        function, file, line, library_path\
    )

#define LOG_ERROR_LIBRARY_LOAD( library_path )\
    log_formatted_unlocked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        true, true,\
        LOG_COLOR_RED\
        "[LIBLOAD ERROR | {cc}() | {cc}:{i}] "\
        "Failed to load library \"{cc}\"!"\
        LOG_COLOR_RESET,\
        function, file, line, library_path\
    )

#define LOG_NOTE_LIBRARY_FREE( library_path )\
    log_formatted_unlocked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        false, true,\
        LOG_COLOR_CYAN\
        "[LIBFREE | {cc}() | {cc}:{i}] "\
        "Library \"{cc}\" freed."\
        LOG_COLOR_RESET,\
        function, file, line, library_path\
    )

#define LOG_NOTE_FUNCTION_LOAD( library_path, function_name )\
    log_formatted_unlocked(\
        LOG_LEVEL_INFO | LOG_LEVEL_VERBOSE | LOG_LEVEL_TRACE,\
        false, true,\
        LOG_COLOR_GREEN\
        "[FNLOAD | {cc}() | {cc}:{i}] "\
        "Function \"{cc}\" loaded from library \"{cc}\" successfully."\
        LOG_COLOR_RESET,\
        function, file, line, function_name, library_path\
    )

#define LOG_ERROR_FUNCTION_LOAD( library_path, function_name )\
    log_formatted_unlocked(\
        LOG_LEVEL_ERROR | LOG_LEVEL_TRACE,\
        true, true,\
        LOG_COLOR_RED\
        "[FNLOAD ERROR | {cc}() | {cc}:{i}] "\
        "Failed to load function \"{cc}\" from library \"{cc}\"!"\
        LOG_COLOR_RESET,\
        function, file, line, function_name, library_path\
    )

LD_API b32 _library_load(
    const char*    library_path,
    DynamicLibrary* out_library
) {
    PlatformLibrary* handle = platform->library.open( library_path );
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
    usize path_len = cstr_len( library_path ) + 1;
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
    platform->library.close( library->handle );
    mem_zero( library, sizeof(DynamicLibrary) );
}
LD_API void _library_free_trace( 
    DynamicLibrary* library,
    const char* function,
    const char* file,
    i32 line
) {
    platform->library.close( library->handle );
#if defined(LD_LOGGING)
    LOG_NOTE_LIBRARY_FREE( library->path_storage );
#endif
    mem_zero( library, sizeof(DynamicLibrary) );

    unused(library);
    unused(function);
    unused(file);
    unused(line);
}
LD_API void* _library_load_function(
    DynamicLibrary* library,
    const char* function_name
) {
    return platform->library.load_function( library->handle, function_name );
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

    unused(library);
    unused(function);
    unused(file);
    unused(line);
    return load_result;
}

