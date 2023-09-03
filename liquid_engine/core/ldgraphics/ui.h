#if !defined(LD_CORE_GRAPHICS_UI_HPP)
#define LD_CORE_GRAPHICS_UI_HPP
// * Description:  Graphics UI
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 25, 2023
#include "defines.h"
#include "core/ldmath/types.h"

typedef void UIHandle;

typedef struct UITransform {
    vec2 normalized_position;
    vec2 normalized_scale;
    f32  rotation_radians;
} UITransform;

LD_API void ui_quad_draw(
    UIHandle* handle, UITransform transform, rgba color );

#endif // header guard
