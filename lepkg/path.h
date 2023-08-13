#if !defined(LEPKG_PATH_HPP)
#define LEPKG_PATH_HPP
// * Description:  Path Processing
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 12, 2023
#include "defines.h"
#include "format.h"

b32 does_file_exist( const char* path );
const char* get_file_ext( const char* path );

AssetType get_file_asset_type( const char* path );

#endif // header guard
