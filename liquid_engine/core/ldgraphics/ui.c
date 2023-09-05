// * Description:  Graphics UI Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 25, 2023
#include "core/ldgraphics/ui.h"
#include "core/ldmath.h"
#include "core/ldlog.h"
#include "core/ldmemory.h"
#include "ldrenderer.h"

typedef enum {
    UI_ELEMENT_TYPE_QUAD,
    UI_ELEMENT_TYPE_COUNT
} UIElementType;

typedef struct {
    UIHandle*     handle;
    UIElementType type;
    UIAnchorX     anchor_x;
    UIAnchorY     anchor_y;
    vec2 position;
    vec2 scale;
    rgba color;
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
LD_API b32 ui_draw_quad(
    UIHandle*   handle,
    vec2        normalized_position,
    vec2        normalized_scale,
    UIAnchorX   anchor_x,
    UIAnchorY   anchor_y,
    rgba        color
) {
    if( UI_CONTEXT->element_count >= UI_CONTEXT->max_elements ) {
        LOG_WARN( "Exceeded max ui element count!" );
        LOG_WARN( "Max element count: {u64}", (u64)UI_CONTEXT->max_elements );
        return false;
    }

    UIElement element = {};
    element.type      = UI_ELEMENT_TYPE_QUAD;
    element.handle    = handle;
    element.position  = normalized_position;
    element.scale     = normalized_scale;
    element.color     = color;
    element.anchor_x  = anchor_x;
    element.anchor_y  = anchor_y;

    UI_CONTEXT->elements[UI_CONTEXT->element_count++] = element;

    return true;
}

usize ui_calculate_required_size( usize max_elements ) {
    return (max_elements * sizeof(UIElement)) + sizeof(UIContext);
}
// TODO(alicia): 
b32 ui_subsystem_init( usize max_elements, void* buffer ) {
    if( !max_elements ) {
        return false;
    }
    ASSERT( buffer );
    UI_CONTEXT = buffer;
    UI_CONTEXT->max_elements  = max_elements;
    UI_CONTEXT->element_count = 0;
    return true;
}
void ui_subsystem_update_render_data(
    ivec2 surface_dimensions, RenderData* render_data
) {
    vec2 dimensions = iv2_to_v2( surface_dimensions );

    for( usize i = 0; i < UI_CONTEXT->element_count; ++i ) {
        if( render_data->object_count >= render_data->max_object_count ) {
            break;
        }
        UIElement* element = UI_CONTEXT->elements + i;
        switch( element->type ) {
            case UI_ELEMENT_TYPE_QUAD: {
                switch( element->anchor_x ) {
                    case UI_ANCHOR_X_CENTER: {
                        element->position.x -= element->scale.x / 2.0f;
                    } break;
                    case UI_ANCHOR_X_RIGHT: {
                        element->position.x -= element->scale.x;
                    } break;
                    default: break;
                }
                switch( element->anchor_y ) {
                    case UI_ANCHOR_Y_CENTER: {
                        element->position.y -= element->scale.y / 2.0f;
                    } break;
                    case UI_ANCHOR_Y_TOP: {
                        element->position.y -= element->scale.y;
                    } break;
                    default: break;
                }
                RenderObject object = {};
                object.mesh   = RENDER_MESH_QUAD_2D_LOWER_LEFT;

                vec2 screen_position =
                    v2_hadamard( element->position, dimensions );
                vec2 screen_scale =
                    v2_hadamard( element->scale, dimensions );

                Material material = {};
                material.shader = RENDER_SHADER_DEBUG_COLOR;
                material.debug_color.color = element->color;
                material.transform =
                    m4_transform_2d( screen_position, 0.0f, screen_scale );

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

