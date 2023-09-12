#if !defined(LD_CORE_GRAPHICS_UI_H)
#define LD_CORE_GRAPHICS_UI_H
// * Description:  Graphics UI
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 25, 2023
#include "defines.h"
#include "core/mathf/types.h"
#include "core/strings.h"
#include "core/graphics/types.h"

typedef void UIHandle;

typedef enum UIAnchorX : u16 {
    UI_ANCHOR_X_LEFT,
    UI_ANCHOR_X_CENTER,
    UI_ANCHOR_X_RIGHT
} UIAnchorX;

typedef enum UIAnchorY : u16 {
    UI_ANCHOR_Y_BOTTOM,
    UI_ANCHOR_Y_CENTER,
    UI_ANCHOR_Y_TOP
} UIAnchorY;

typedef enum : u16 {
    UI_COORDINATE_NORMALIZED,
    UI_COORDINATE_PIXEL
} UICoordinate;

#if defined(LD_API_INTERNAL)
struct RenderData;

usize ui_calculate_required_size( usize max_elements );
b32 ui_subsystem_init( usize max_elements, void* buffer );
void ui_subsystem_update_render_data(
    ivec2 surface_dimensions, struct RenderData* render_data );
#endif

/// Query what the max number of ui elements is.
LD_API usize ui_query_max_elements(void);
/// Query how many ui elements are present in the current scene.
LD_API usize ui_query_element_count(void);
/// Draw a simple quad.
/// Returns true if successful.
/// Only fails if exceeded max number of ui elements.
LD_API b32 ui_draw_quad(
    UIHandle*   handle,
    UICoordinate position_coordinate,
    UICoordinate scale_coordinate,
    
    vec2 position,
    vec2 scale,

    UIAnchorX   anchor_x,
    UIAnchorY   anchor_y,
    rgba        color
);

#endif // header guard
