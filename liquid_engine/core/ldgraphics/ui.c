// * Description:  Graphics UI Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 25, 2023
#include "core/ldgraphics/ui.h"

typedef enum UIElementType : u32 {
    UI_ELEM_QUAD,

    UI_ELEM_COUNT
} UIElementType;

typedef struct UIElement {
    UIHandle*   handle;
    UITransform transform;
    UIElementType type;
    union {
        struct UIQuad {
            rgba color;
        } quad;
    };
} UIElement;

#define UI_MAX_ELEMENTS (10)

global u32 element_count = 0;
global UIElement elements[UI_MAX_ELEMENTS];

LD_API void ui_quad_draw(
    UIHandle* handle, UITransform transform, rgba color
) {
    ASSERT( element_count < UI_MAX_ELEMENTS );

    UIElement quad = {};
    quad.handle    = handle;
    quad.transform = transform;
    quad.type      = UI_ELEM_QUAD;
    quad.quad.color = color;

    elements[element_count++] = quad;
}

