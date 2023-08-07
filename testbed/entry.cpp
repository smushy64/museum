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

enum GameStatus : u32 {
    GAME_STATUS_START,
    GAME_STATUS_PLAY,
    GAME_STATUS_DIED,
    GAME_STATUS_PAUSE,
    GAME_STATUS_STAGE_COMPLETE,
    GAME_STATUS_GAME_OVER,
    GAME_STATUS_TEST
};

#define GAME_STATUS_START_MAX_TIMER (2.0f)
#define GAME_STATUS_BLINK_TIME (GAME_STATUS_START_MAX_TIMER / 12.0f)
struct GameStateStart {
    f32 timer;
    f32 blink_timer;
    b32 show_ship;
    b32 initialized;
};
#define GAME_STATE_PLAY_RESPAWN_TIME (3.0f)
struct GameStatePlay {
    f32 respawn_timer;
};
struct GameState {
    GameStatus status;
    union {
        GameStateStart start;
        GameStatePlay  play;
    };
};
internal inline void game_state_set_status(
    GameState* game_state,
    GameStatus status
) {
    game_state->status = status;
    switch( status ) {
        case GAME_STATUS_START:
            game_state->start = {};
            break;
        case GAME_STATUS_PLAY:
            game_state->play = {};
        default: break;
    }
}

#define PLAYER_MAX_LIVES (3ul)
#define MAX_TORPEDOES (5)
#define MAX_TEXT (3)
struct GameMemory {
    Texture         textures[3];
    RandXOR         rand_xor;
    EntityID        ship_id;
    EntityID        first_torpedo_id;
    EntityID        current_torpedo;
    EntityID        first_ship_destroyed_id;
    EntityID        first_life_ui_id;
    EventListenerID on_exit_listener;

    UIText text[MAX_TEXT];

    u32 player_score;
    u32 asteroid_count;
    u32 ship_lives;

    GameState game_state;
};

internal void game_set_life(
    GameMemory*    memory,
    EntityStorage* storage,
    u32            new_life
) {
    u32 life = clamp( new_life, 0ul, PLAYER_MAX_LIVES );
    memory->ship_lives = life;
    for( u32 i = 0; i < PLAYER_MAX_LIVES; ++i ) {
        EntityID id         = memory->first_life_ui_id + i;
        Entity*  entity     = entity_storage_get( storage, id );
        b32      set_active = i < life;
        entity_set_active( entity, set_active );
    }
}

internal b32 game_generate_asteroid(
    GameMemory* game_memory,
    EntityStorage* storage
) {
    vec2 position   = {
        game_memory->rand_xor.next_f32(),
        game_memory->rand_xor.next_f32()
    };
    Entity asteroid = asteroid_create(
        position, ASTEROID_MAX_LIFE,
        &game_memory->textures[2],
        game_memory->rand_xor
    );
    EntityID asteroid_id = entity_storage_create_entity(
        storage, &asteroid
    );
    if( asteroid_id < 0 ) {
        return false;
    }

    game_memory->asteroid_count++;

    return true;
}

internal b32 game_damage_asteroid(
    GameMemory*    game_memory,
    EntityStorage* storage,
    Entity*        entity
) {
    Asteroid* asteroid = (Asteroid*)entity->bytes;
    u32 asteroid_pieces = 3;
    if( asteroid->life < ASTEROID_MAX_LIFE ) {
        asteroid_pieces = 2;
    }

    if( !asteroid_damage( entity, game_memory->rand_xor ) ) {
        game_memory->asteroid_count--;
        return true;
    }

    vec2 position = asteroid->transform.position;
    for( u32 i = 0; i < asteroid_pieces; ++i ) {
        Entity new_asteroid = asteroid_create(
            position,
            asteroid->life,
            &game_memory->textures[2],
            game_memory->rand_xor
        );
        EntityID new_asteroid_id = entity_storage_create_entity(
            storage, &new_asteroid
        );
        if( new_asteroid_id < 0 ) {
            return false;
        }

        game_memory->asteroid_count++;
    }

    return true;
}

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

    string_format(
        config->application_name,
        "Test Bed {i}.{i}{c}",
        LIQUID_ENGINE_VERSION_MAJOR,
        LIQUID_ENGINE_VERSION_MINOR,
        0
    );

    config->surface_dimensions        = { 800, 600 };
    config->log_level        = LOG_LEVEL_ALL_VERBOSE;
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

    f32 life_ui_y       = 0.9f;
    f32 life_ui_start_x = 1.2f;
    for( u32 i = 0; i < PLAYER_MAX_LIVES; ++i ) {
        f32    life_ui_x = life_ui_start_x - ( 0.15f * (f32)i );
        Entity life_ui = life_ui_create(
            { life_ui_x, life_ui_y },
            &memory->textures[1]
        );
        EntityID id = entity_storage_create_entity( storage, &life_ui );
        ASSERT( id >= 0 );
        if( i == 0 ) {
            memory->first_life_ui_id = id;
        }
    }

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

    memory->on_exit_listener =
        event_subscribe( EVENT_CODE_EXIT, on_exit, memory );

    game_state_set_status( &memory->game_state, GAME_STATUS_START );

    return true;
}

internal b32 filter_active( Entity* entity ) {
    return CHECK_BITS(
        entity->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE |
        ENTITY_STATE_FLAG_IS_2D
    );
}

internal b32 filter_active_visible2d( Entity* entity ) {
    return CHECK_BITS(
        entity->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE  |
        ENTITY_STATE_FLAG_IS_VISIBLE |
        ENTITY_STATE_FLAG_IS_2D      
    );
}

internal b32 filter_active_torpedoes( Entity* entity ) {
    b32 is_torpedo = entity->type == ENTITY_TYPE_TORPEDO;
    return filter_active_visible2d( entity ) && is_torpedo;
}

internal b32 filter_colliders( Entity* entity ) {
    b32 has_collider2d = CHECK_BITS(
        entity->component_flags,
        ENTITY_COMPONENT_FLAG_COLLIDER_2D
    );
    return filter_active_visible2d( entity ) && has_collider2d;
}

internal b32 filter_asteroids( Entity* entity ) {
    b32 is_asteroid = entity->type == ENTITY_TYPE_ASTEROID;
    return filter_active_visible2d( entity ) && is_asteroid;
}

internal b32 filter_sprites( Entity* entity ) {
    b32 has_sprite_renderer = CHECK_BITS(
        entity->component_flags,
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER
    );
    return filter_active_visible2d( entity ) && has_sprite_renderer;
}

#define SCORE_TEXT_BUFFER_SIZE (32)
global char SCORE_TEXT_BUFFER[SCORE_TEXT_BUFFER_SIZE];

b32 status_play(
    GameMemory* memory,
    EntityStorage* storage,
    Timer* time,
    RenderOrder* render_order,
    ivec2 screen_dimensions
) {
    Entity* entity_ship = &storage->entities[memory->ship_id];
    Ship*   ship = (Ship*)entity_ship->bytes;

    b32 ship_is_active = CHECK_BITS(
        entity_ship->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE
    ) && memory->ship_lives;

    render_order->text_count = 1;
    render_order->ui_text    = memory->text;

    StringView score_text;
    score_text.buffer = SCORE_TEXT_BUFFER;
    score_text.len    = SCORE_TEXT_BUFFER_SIZE;
    score_text.len =
        string_format( score_text, "{i,06}", memory->player_score );

    memory->text[0].text     = score_text;
    memory->text[0].position = v2(0.025f, 0.85f);
    memory->text[0].scale    = 0.3f;
    memory->text[0].color    = RGBA::WHITE;

    if( ship_is_active ) {

        vec2 input_direction = {};
        input_direction.x = (f32)input_is_key_down( KEY_ARROW_RIGHT ) -
            (f32)input_is_key_down( KEY_ARROW_LEFT );
        input_direction.y = (f32)input_is_key_down( KEY_ARROW_UP );

        ship->physics.drag = input_is_key_down( KEY_ARROW_DOWN ) ?
            SHIP_STOP_DRAG : SHIP_NORMAL_DRAG;
        ship->physics.angular_drag = ship->physics.drag;

        vec2 ship_forward_direction = rotate(
            VEC2::UP, ship->transform.rotation
        );

        ship->physics.velocity +=
            ship_forward_direction *
            input_direction.y *
            time->delta_seconds *
            SHIP_NORMAL_SPEED;

        ship->physics.angular_velocity +=
            input_direction.x *
            time->delta_seconds *
            SHIP_ROTATION_SPEED;

        b32 z_is_down = input_is_key_down( KEY_Z );
        if(
            z_is_down &&
            z_is_down != input_was_key_down( KEY_Z ) &&
            !ship->is_invincible
        ) {

            EntityID id = memory->first_torpedo_id + memory->current_torpedo;
            Entity* torpedo = &storage->entities[id];
            torpedo_enable( torpedo, ship, ship_forward_direction );
            memory->current_torpedo =
                (memory->current_torpedo + 1) % MAX_TORPEDOES;
        }
    }

    EntityStorageQueryResult torpedoes = entity_storage_query(
        storage, filter_active_torpedoes
    );
    EntityStorageQueryResult asteroids = entity_storage_query(
        storage, filter_asteroids
    );

    /* Torpedoes */ {

        QueryResultIterator iterator = &torpedoes;
        EntityID id;
        while( iterator.next( &id ) ) {
            Entity*  torpedo_entity = entity_storage_get( storage, id );
            Torpedo* torpedo        = (Torpedo*)(torpedo_entity->bytes);
            Entity* hit_result = system_collider2d_solver(
                storage, id, &asteroids
            );

            if( hit_result ) {
                Asteroid* hit_asteroid = (Asteroid*)hit_result->bytes;
                memory->player_score += hit_asteroid->score;
                game_damage_asteroid( memory, storage, hit_result );
                entity_set_active( torpedo_entity, false );
                continue;
            }
            
            torpedo->life_timer += time->delta_seconds;
            if( torpedo->life_timer >= TORPEDO_LIFETIME_SECONDS ) {
                entity_set_active( torpedo_entity, false );
            }

        }
    }

    /* Physics and Wrapping */ {
        EntityStorageQueryResult physics_objects = system_physics2d_solver(
            storage, time->delta_seconds
        );

        f32 aspect_ratio = (f32)screen_dimensions.x / (f32)screen_dimensions.y;
        f32 wrap_padding = SHIP_SCALE;

        QueryResultIterator iterator = &physics_objects;
        EntityID id;
        while( iterator.next( &id ) ) {
            Entity* entity = entity_storage_get( storage, id );

            f32 abs_pos_x = absolute(entity->transform2d.position.x);
            f32 abs_pos_y = absolute(entity->transform2d.position.y);

            // screen wrapping
            if( abs_pos_x >= aspect_ratio + wrap_padding ) {
                entity->transform2d.position.x *= -1.0f;
            }
            if( abs_pos_y >= 1.0f + wrap_padding ) {
                entity->transform2d.position.y *= -1.0f;
            }
        }
    }

    /* Asteroid collision with ship */
    if( ship_is_active && !ship->is_invincible ) {
        Entity* hit_result = system_collider2d_solver(
            storage, memory->ship_id, &asteroids
        );
        if( hit_result ) {

            b32 is_ship_currently_active = CHECK_BITS(
                entity_ship->state_flags,
                ENTITY_STATE_FLAG_IS_ACTIVE
            );
            if( is_ship_currently_active ) {
                // NOTE(alicia): Ship collided with asteroid
                entity_set_active( entity_ship, false );

                for( u32 i = 0; i < SHIP_DESTROYED_PIECE_COUNT; ++i ) {
                    EntityID id = memory->first_ship_destroyed_id + i;
                    Entity* ship_destroyed_piece =
                        entity_storage_get( storage, id );
                    ship_destroyed_enable(
                        ship_destroyed_piece,
                        ship->transform.position,
                        memory->rand_xor
                    );
                }

                ship->physics.velocity = {};
                ship->physics.angular_velocity = 0.0f;
                ship->transform.position = {};
                ship->transform.rotation = 0.0f;

                game_set_life( memory, storage, memory->ship_lives - 1 );
            }
        }
    } else if( !ship_is_active && memory->ship_lives ) {
        memory->game_state.play.respawn_timer += time->delta_seconds;
        if(
            memory->game_state.play.respawn_timer >=
            GAME_STATE_PLAY_RESPAWN_TIME
        ) {
            memory->game_state.play.respawn_timer = 0.0f;
            entity_set_active( entity_ship, true );
            ship->is_invincible = true;
            for( u32 i = 0; i < SHIP_DESTROYED_PIECE_COUNT; ++i ) {
                EntityID id = memory->first_ship_destroyed_id + i;
                Entity* ship_destroyed_piece =
                    entity_storage_get( storage, id );
                entity_set_active( ship_destroyed_piece, false );
            }
        }
    }

    if( ship_is_active && ship->is_invincible ) {
        ship->invincibility_timer += time->delta_seconds;
        ship->blink_timer += time->delta_seconds;
        if( ship->blink_timer >= SHIP_BLINK_TIME ) {
            ship->blink_timer = 0.0f;
            b32 is_visible = CHECK_BITS(
                entity_ship->state_flags,
                ENTITY_STATE_FLAG_IS_VISIBLE
            );
            if( is_visible ) {
                CLEAR_BIT(
                    entity_ship->state_flags,
                    ENTITY_STATE_FLAG_IS_VISIBLE
                );
            } else {
                entity_ship->state_flags |= ENTITY_STATE_FLAG_IS_VISIBLE;
            }
        }
        if( ship->invincibility_timer >= SHIP_INVINCIBILITY_TIME ) {
            ship->is_invincible = false;
            ship->invincibility_timer = 0.0f;
            ship->blink_timer = 0.0f;
            entity_ship->state_flags |= ENTITY_STATE_FLAG_IS_VISIBLE;
        }

    }

    /* calculate transforms */ {
        EntityStorageQueryResult active_objects = entity_storage_query(
            storage, filter_active
        );
        QueryResultIterator iterator = &active_objects;
        EntityID id;
        while( iterator.next( &id ) ) {
            Entity* entity = entity_storage_get( storage, id );
            entity->matrix = transform(
                entity->transform2d.position,
                entity->transform2d.rotation,
                entity->transform2d.scale
            );
        }
    }

    render_order->sprites = entity_storage_query(
        storage, filter_sprites
    );

    if( !memory->asteroid_count ) {
        game_state_set_status( &memory->game_state, GAME_STATUS_START );
    }

    if( !memory->ship_lives ) {
        game_state_set_status( &memory->game_state, GAME_STATUS_GAME_OVER );
    }

    local u32 last_lives = 0;
    if( last_lives != memory->ship_lives ) {
        LOG_DEBUG( "lives: {u}", memory->ship_lives );
    }
    last_lives = memory->ship_lives;

#if defined(DEBUG)
    /* debug draw colliders */ {
        EntityStorageQueryResult collider_objects = entity_storage_query(
            storage, filter_colliders
        );

        QueryResultIterator iterator = &collider_objects;
        EntityID id;
        while( iterator.next( &id ) ) {
            Entity* current = entity_storage_get( storage, id );
            switch( current->collider2D.type ) {
                case COLLIDER_TYPE_2D_RECT: {
                    Rect2D collider_rect = {
                        current->transform2d.position.x -
                            current->collider2D.rect.half_width,
                        current->transform2d.position.x +
                            current->collider2D.rect.half_width,
                        current->transform2d.position.y +
                            current->collider2D.rect.half_height,
                        current->transform2d.position.y -
                            current->collider2D.rect.half_height
                    };
                    debug_draw_rect(
                        render_order,
                        collider_rect,
                        RGBA::BLUE
                    );
                } break;
                case COLLIDER_TYPE_2D_CIRCLE: {
                    Circle2D collider_circle = {
                        current->transform2d.position,
                        current->collider2D.circle.radius
                    };
                    debug_draw_circle(
                        render_order,
                        collider_circle,
                        RGBA::BLUE
                    );
                } break;
                default: break;
            }
        }
    }
#endif

    return true;
}

b32 status_start(
    GameMemory*    memory,
    EntityStorage* storage,
    Timer*         time,
    RenderOrder*   render_order
) {
    GameState* state = &memory->game_state;
    ASSERT( state->status == GAME_STATUS_START );

    if( !state->start.initialized ) {
        Entity* ship_entity = &storage->entities[memory->ship_id];
        Ship* ship = (Ship*)ship_entity->bytes;
        ship->transform.position = {};
        ship->transform.rotation = 0.0f;
        ship_entity->matrix = transform(
            ship->transform.position,
            ship->transform.rotation,
            ship->transform.scale
        );
        state->start.initialized = true;
        state->start.show_ship   = true;
        state->start.timer       = 0.0f;
        state->start.blink_timer = 0.0f;
    }

    EntityStorageQueryResult torpedoes = entity_storage_query(
        storage, filter_active_torpedoes
    );
    QueryResultIterator iterator = &torpedoes;
    EntityID id;
    while( iterator.next( &id ) ) {
        Entity*  torpedo_entity = entity_storage_get( storage, id );
        entity_set_active( torpedo_entity, false );
    }

    state->start.blink_timer += time->delta_seconds;
    if( state->start.blink_timer >= GAME_STATUS_BLINK_TIME ) {
        state->start.show_ship = !state->start.show_ship;
        state->start.blink_timer = 0.0f;
    }

    if( state->start.show_ship ) {
        EntityStorageQueryResult sprites;
        sprites.count  = 1;
        sprites.ids[0] = memory->ship_id;

        render_order->sprites = sprites;
    }

    state->start.timer += time->delta_seconds;
    if( state->start.timer < GAME_STATUS_START_MAX_TIMER ) {
        return true;
    }

    if( !memory->ship_lives ) {
        game_set_life( memory, storage, PLAYER_MAX_LIVES );
        memory->ship_lives = PLAYER_MAX_LIVES;
    }

    for( u32 i = 0; i < 3; ++i ) {
        game_generate_asteroid( memory, storage );
    }

    state->start.initialized = false;
    game_state_set_status( state, GAME_STATUS_PLAY );
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
        case GAME_STATUS_START: {
            status_result = status_start(
                memory,
                storage,
                time,
                render_order
            );
        } break;
        case GAME_STATUS_PAUSE:
        case GAME_STATUS_DIED:
        case GAME_STATUS_GAME_OVER:
        case GAME_STATUS_STAGE_COMPLETE:
        case GAME_STATUS_PLAY: {
            status_result = status_play(
                memory,
                storage,
                time,
                render_order,
                screen_dimensions
            );
        } break;

        default: break;
    }

    return status_result;
}

