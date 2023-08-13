// * Description:  Testbed Entry Point Implementation
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: June 21, 2023
#include "corecpp.inl"

#include <defines.h>
#include <core/logging.h>
#include <core/engine.h>
#include <core/ecs.h>
#include <core/graphics.h>
#include <core/collections.h>
#include <core/event.h>
#include <core/input.h>
#include <core/math.h>
#include <core/time.h>
#include <core/asset.h>
#include <core/memory.h>

#include <renderer/renderer.h>
#include <renderer/primitives.h>

#include "entity.h"
#include "memory.h"
#include "status.h"

EventCallbackReturn on_exit( Event*, void* generic_memory ) {
    GameMemory* memory = (GameMemory*)generic_memory;
    for( u32 i = 0; i < STATIC_ARRAY_COUNT( memory->textures ); ++i ) {
        if( memory->textures[i].buffer ) {
            mem_free( memory->textures[i].buffer );
        }
    }

    event_unsubscribe( memory->on_exit_listener );

    return EVENT_CALLBACK_NOT_CONSUMED;
}

extern "C"
void application_config( struct EngineConfig* config ) {

    config->application_name.len = string_format(
        config->application_name,
        "Asteroids{c}",
        0
    );

    config->surface_dimensions = { 800, 600 };
    config->log_level          = LOG_LEVEL_ALL_VERBOSE;
    // TODO(alicia): expose these flags somewhere 
    config->platform_flags   = (1 << 0) | (1 << 1);
    config->memory_size = sizeof(GameMemory);
}

extern "C"
b32 application_init( struct EngineContext* ctx, void* generic_memory ) {
    GameMemory* memory = (GameMemory*)generic_memory;

    memory->rand_xor = RandXOR( 53465457 );

    EntityStorage* storage = engine_get_entity_storage( ctx );

    memory->textures[0].id = RendererID( 1u );

    DebugImage ship_image = {};
    if( !debug_load_bmp( "./resources/ship.bmp", &ship_image ) ) {
        return false;
    }

    memory->textures[1].dimensions  = ship_image.dimensions;
    memory->textures[1].format      = ship_image.format;
    memory->textures[1].buffer      = ship_image.buffer;
    memory->textures[1].wrap_x      = TEXTURE_WRAP_CLAMP;
    memory->textures[1].wrap_y      = TEXTURE_WRAP_CLAMP;
    memory->textures[1].filter      = TEXTURE_FILTER_BILINEAR;
    memory->textures[1].use_opacity = true;

    DebugImage asteroid_image = {};
    if( !debug_load_bmp( "./resources/asteroid.bmp", &asteroid_image ) ) {
        return false;
    }

    memory->textures[2].dimensions  = asteroid_image.dimensions;
    memory->textures[2].format      = asteroid_image.format;
    memory->textures[2].buffer      = asteroid_image.buffer;
    memory->textures[2].wrap_x      = TEXTURE_WRAP_CLAMP;
    memory->textures[2].wrap_y      = TEXTURE_WRAP_CLAMP;
    memory->textures[2].filter      = TEXTURE_FILTER_BILINEAR;
    memory->textures[2].use_opacity = true;

    Entity ship = ship_create( &memory->textures[1] );
    memory->ship_id = entity_storage_create_entity( storage, &ship );

    ASSERT( memory->ship_id >= 0 );

    for( u32 i = 0; i < MAX_TORPEDOES; ++i ) {
        Entity   torpedo = torpedo_create( &memory->textures[0] );
        EntityID id      = entity_storage_create_entity( storage, &torpedo );
        ASSERT( id >= 0 );
        if( i == 0 ) {
            memory->first_torpedo_id = id;
        }
    }

    for( u32 i = 0; i < SHIP_DESTROYED_PIECE_COUNT; ++i ) {
        Entity ship_destroyed = ship_destroyed_create(
            &memory->textures[0]
        );
        EntityID id = entity_storage_create_entity(
            storage, &ship_destroyed
        );
        ASSERT( id >= 0 );
        if( i == 0 ) {
            memory->first_ship_destroyed_id = id;
        }

    }

    f32 lives_origin_x = 0.025f;
    f32 lives_origin_y = 0.925f;
    f32 lives_advance  = 0.05f;
    f32 lives_scale    = 0.5f;
    for( u32 i = 0; i < MAX_IMAGES; ++i ) {
        vec2 position = { lives_origin_x, lives_origin_y };
        memory->images[i].position = position;
        memory->images[i].scale    = lives_scale;
        memory->images[i].color    = RGBA::WHITE;
        memory->images[i].anchor_x = UI_ANCHOR_X_LEFT;
        memory->images[i].texture  = &memory->textures[1];

        lives_origin_x += lives_advance;
    }

    memory->on_exit_listener =
        event_subscribe( EVENT_CODE_EXIT, on_exit, memory );

    game_state_set_status( &memory->game_state, GAME_STATUS_MAIN_MENU );

    return true;
}

extern "C"
b32 application_run( struct EngineContext* ctx, void* generic_memory ) {
    GameMemory*    memory  = (GameMemory*)generic_memory;
    EntityStorage* storage = engine_get_entity_storage( ctx );
    Timer*         time    = engine_get_time( ctx );
    RenderOrder*   render_order = engine_get_render_order( ctx );
    ivec2 screen_dimensions = engine_query_surface_size( ctx );

    render_order->storage = storage;

    if( input_is_key_down( KEY_ESCAPE ) ) {
        Event event = {};
        event.code  = EVENT_CODE_EXIT;
        event_fire( event );
    }

    b32 status_result = true;
    switch( memory->game_state.status ) {
        case GAME_STATUS_MAIN_MENU: {
            status_result = status_main_menu(
                memory, render_order
            );
        } break;
        case GAME_STATUS_START: {
            status_result = status_start(
                memory,
                storage,
                time,
                render_order
            );
        } break;

        case GAME_STATUS_GAME_OVER:
        case GAME_STATUS_PLAY: {
            status_result = status_play(
                memory,
                storage,
                time,
                render_order,
                &screen_dimensions
            );
        } break;

        default: break;
    }

    return status_result;
}

