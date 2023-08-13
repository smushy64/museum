#if !defined(LEPKG_PACKAGE_HPP)
#define LEPKG_PACKAGE_HPP
// * Description:  Liquid Engine Packager
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "defines.h"

typedef struct {
    union {
        u32 version;
        struct {
            u16 minor;
            u16 major;
        };
    };
    u32          compression_level;
    const char*  output_path;
    u32          input_path_count;
    const char** input_paths;
    b32          overwrite_existing;
} PackagerParameters;

b32 make_package( PackagerParameters* params );

#endif // header guard
