#if !defined(LD_CORE_PATH_H)
#define LD_CORE_PATH_H
/**
 * Description:  Path functions.
 * Author:       Alicia Amarilla (smushyaa@gmail.com)
 * File Created: January 05, 2024
*/
#include "shared/defines.h"

/// Slice of a path buffer.
typedef struct PathSlice {
    char* buffer;
    usize len;
} PathSlice;

/// Path buffer.
typedef struct PathBuffer {
    char* buffer;
    usize len;
    usize capacity;
} PathBuffer;

#if !defined(FORMAT_WRITE_FN_DEFINED)
#define FORMAT_WRITE_FN_DEFINED
/// Formatting write function.
/// Target is a pointer to where-ever the formatted string should be written to.
/// Returns 0 if successful, otherwise returns number of characters
/// that could not be written.
typedef usize FormatWriteFN( void* target, usize count, char* characters );
#endif

#define path_slice( literal )\
    (PathSlice){ literal, sizeof(literal) - 1 }

#define path_buffer_to_slice( buffer )\
    (*(PathSlice*)(buffer))

#define path_slice_to_string( slice )\
    (*(StringSlice*)(slice))

#define path_buffer_to_string( buffer )\
    (*(StringBuffer*)(buffer))

#define path_buffer( buffer_name, literal )\
    char buffer_name##_buffer[] = literal;\
    PathBuffer buffer_name = (PathBuffer){ buffer_name##_buffer, sizeof(literal) - 1, sizeof(literal) }

#define path_buffer_empty( buffer_name, capacity )\
    char buffer_name##_buffer[capacity] = {};\
    PathBuffer buffer_name = (PathBuffer){ buffer_name##_buffer, 0, capacity }

/// Create a path slice from null-terminated string.
/// Optionally takes in length to avoid calculating it.
CORE_API PathSlice path_slice_from_cstr( usize opt_len, const char* cstr );
/// Check if path is an absolute path.
CORE_API b32 path_slice_is_absolute( PathSlice path );
/// Check if path is a relative path.
header_only b32 path_slice_is_relative( PathSlice path ) {
    return !path_slice_is_absolute( path );
}
/// Check if item pointed to by path is a file.
CORE_API b32 path_slice_is_file( PathSlice path );
/// Check if item pointed to by path is a directory.
CORE_API b32 path_slice_is_directory( PathSlice path );
/// Get the parent of the item pointed to by path.
/// Returns true if path is a child of a directory.
/// If true, out_parent receives the parent of path (uses the same char buffer as path).
CORE_API b32 path_slice_get_parent( PathSlice path, PathSlice* out_parent );
/// Get number of parent items of path.
CORE_API usize path_slice_ancestor_count( PathSlice path );
/// Separate parents of path into ancestors.
/// Ancestor count must be less-than or equals to path's ancestor count.
/// Ancestor count can be calculated using path_slice_ancestor_count.
CORE_API void path_slice_fill_ancestors(
    PathSlice path, usize ancestor_count, PathSlice* ancestors );
/// Get the file name of the item pointed to by path.
/// Returns true if path ends with a file name.
/// Does not actually check if the item is a file or if it exists on disk.
/// If true, out_file_name is filled in with the file name (includes extension).
CORE_API b32 path_slice_get_file_name( PathSlice path, PathSlice* out_file_name );
/// Get the file stem of item pointed to by path.
/// Returns true if path points to a file.
/// If true, out_file_stem is filled in with the file name (does not include extension).
CORE_API b32 path_slice_get_file_stem( PathSlice path, PathSlice* out_file_stem );
/// Get the extension of the file pointed to by path.
/// Returns true if path points to a file and file has an extension.
/// If true, out_extension is filled in with
/// the extension (does not include preceding dot).
CORE_API b32 path_slice_get_extension( PathSlice path, PathSlice* out_extension );
/// Pop off the last path chunk in path slice.
/// Returns true if path slice had a chunk.
/// If true, opt_out_chunk receives the last path chunk.
/// opt_out_path receives truncated path.
CORE_API b32 path_slice_pop(
    PathSlice path, PathSlice* out_path, PathSlice* opt_out_chunk );
/// Turn relative path into absolute path.
/// Win32 Only: Converts all forward slashes to backslashes.
/// Streams formatted result into item pointed to by target.
/// string_buffer_write can be used to write into PathBuffer.
CORE_API usize path_slice_canonicalize(
    FormatWriteFN* write, void* target, PathSlice path );
/// Converts path separators to either forward or back slashes.
/// Streams formatted result into item pointed to by target.
/// If forward_slash is true, converts backslashes to forward slashes and vice versa.
/// string_buffer_write can be used to write into PathBuffer.
CORE_API usize path_slice_convert_separators(
    FormatWriteFN* write, void* target, PathSlice path, b32 forward_slash );

/// Push a path chunk to end of path buffer.
/// Returns true if path buffer has enough capacity to append chunk.
CORE_API b32 path_buffer_push( PathBuffer* path, PathSlice chunk );
/// Pops off the last path chunk in path buffer.
/// Returns true if path buffer had a chunk.
/// If true, opt_out_chunk receives the last path chunk.
CORE_API b32 path_buffer_pop( PathBuffer* path, PathSlice* opt_out_chunk );
/// Set path extension.
/// Extension should not include preceding dot!
/// Returns true if path has enough capacity to append extension.
/// This function does NOT check if item pointed to by path is a file.
CORE_API b32 path_buffer_set_extension( PathBuffer* path, PathSlice extension );

#endif /* header guard */
