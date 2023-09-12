// * Description:  Graphics UI Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 25, 2023
#include "core/graphics/ui.h"
#include "core/mathf.h"
#include "core/log.h"
#include "core/mem.h"
#include "renderer.h"

typedef enum {
    UI_ELEMENT_TYPE_QUAD,
    UI_ELEMENT_TYPE_TEXT,
    UI_ELEMENT_TYPE_COUNT
} UIElementType;

typedef struct {
    FontID      font;
    StringSlice text;
} UIText;

typedef struct {
    UIHandle*     handle;
    UIElementType type;

    UIAnchorX     anchor_x;
    UIAnchorY     anchor_y;

    UICoordinate  position_coordinate;
    UICoordinate  scale_coordinate;

    vec2 position;
    vec2 scale;
    rgba color;

    union {
        UIText text;
    };
} UIElement;

typedef struct {
    usize max_elements;
    usize element_count;
    UIElement elements[];
} UIContext;

global UIContext* UI_CONTEXT = NULL;

LD_API usize ui_query_max_elements(void) {
    return UI_CONTEXT->max_elements;
}
LD_API usize ui_query_element_count(void) {
    return UI_CONTEXT->element_count;
}
internal force_inline
b32 ui_check_bounds(void) {
    if( UI_CONTEXT->element_count >= UI_CONTEXT->max_elements ) {
        LOG_WARN( "Exceeded max ui element count!" );
        LOG_WARN( "Max element count: {u64}", (u64)UI_CONTEXT->max_elements );
        return false;
    }
    return true;
}
internal force_inline
void ui_push_element( UIElement element ) {
    UI_CONTEXT->elements[UI_CONTEXT->element_count++] = element;
}
LD_API b32 ui_draw_quad(
    UIHandle*   handle,

    UICoordinate position_coordinate,
    UICoordinate scale_coordinate,
    
    vec2 position,
    vec2 scale,

    UIAnchorX   anchor_x,
    UIAnchorY   anchor_y,
    rgba        color
) {
    if( !ui_check_bounds() ) {
        return false;
    }

    UIElement element = {};
    element.type      = UI_ELEMENT_TYPE_QUAD;
    element.handle    = handle;
    element.position  = position;
    element.scale     = scale;
    element.color     = color;
    element.anchor_x  = anchor_x;
    element.anchor_y  = anchor_y;

    element.position_coordinate = position_coordinate;
    element.scale_coordinate    = scale_coordinate;

    ui_push_element( element );

    return true;
}

usize ui_calculate_required_size( usize max_elements ) {
    return (max_elements * sizeof(UIElement)) + sizeof(UIContext);
}
b32 ui_subsystem_init( usize max_elements, void* buffer ) {
    if( !max_elements ) {
        return false;
    }
    assert( buffer );
    UI_CONTEXT = buffer;
    UI_CONTEXT->max_elements  = max_elements;
    UI_CONTEXT->element_count = 0;
    return true;
}
void ui_subsystem_update_render_data(
    ivec2 surface_dimensions, RenderData* render_data
) {
    vec2 dimensions = v2_iv2( surface_dimensions );

    for( usize i = 0; i < UI_CONTEXT->element_count; ++i ) {
        if( render_data->object_count >= render_data->max_object_count ) {
            break;
        }
        UIElement* element = UI_CONTEXT->elements + i;
        switch( element->type ) {
            case UI_ELEMENT_TYPE_QUAD: {
                RenderObject object = {};
                object.mesh   = RENDER_MESH_QUAD_2D_LOWER_LEFT;

                vec2 position, scale;
                switch( element->position_coordinate ) {
                    case UI_COORDINATE_NORMALIZED: {
                        position = v2_hadamard( element->position, dimensions );
                    } break;
                    case UI_COORDINATE_PIXEL: {
                        position = element->position;
                    } break;
                    default: UNIMPLEMENTED();
                }
                switch( element->scale_coordinate ) {
                    case UI_COORDINATE_NORMALIZED: {
                        scale = v2_hadamard( element->scale, dimensions );
                    } break;
                    case UI_COORDINATE_PIXEL: {
                        scale = element->scale;
                    } break;
                    default: UNIMPLEMENTED();
                }
                switch( element->anchor_x ) {
                    case UI_ANCHOR_X_CENTER: {
                        position.x -= scale.x / 2.0f;
                    } break;
                    case UI_ANCHOR_X_RIGHT: {
                        position.x -= scale.x;
                    } break;
                    default: break;
                }
                switch( element->anchor_y ) {
                    case UI_ANCHOR_Y_CENTER: {
                        position.y -= scale.y / 2.0f;
                    } break;
                    case UI_ANCHOR_Y_TOP: {
                        position.y -= scale.y;
                    } break;
                    default: break;
                }

                Material material          = {};
                material.shader            = RENDER_SHADER_DEBUG_COLOR;
                material.debug_color.color = element->color;
                material.transform         =
                    m4_transform_2d( position, 0.0f, scale );

                object.material = material;

                render_data->objects[render_data->object_count++] = object;
            } break;
            default: continue;
        }
    }

    UI_CONTEXT->element_count = 0;
#if defined(DEBUG)
    mem_zero(
        UI_CONTEXT->elements,
        sizeof(UIElement) * UI_CONTEXT->element_count
    );
#endif
}

