#if !defined(LEPKG_IMAGE_HPP)
#define LEPKG_IMAGE_HPP
// * Description:  Parse Image
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 13, 2023
#include "defines.h"
#include "format.h"
#include <stdio.h>

b32 parse_bmp_0_1(
    usize* buffer_size,
    void** buffer,
    FILE*  image_file,
    AssetTexture_0_1* out_texture
);

#endif // header guard
