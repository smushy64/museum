// * Description:  Testbed Status
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include "status.h"
#include "memory.h"
#include "entity.h"

#include <core/math.h>
#include <core/time.h>
#include <core/ecs.h>
#include <core/input.h>
#include <core/memory.h>

void game_state_set_status( GameState* state, GameStatus status ) {
    state->status = status;
    switch( status ) {
        case GAME_STATUS_MAIN_MENU: {
            state->main_menu = {};
        } break;
        case GAME_STATUS_START: {
            state->start = {};
        } break;
        case GAME_STATUS_PLAY: {
            state->play = {};
        } break;
        case GAME_STATUS_GAME_OVER: {

        } break;
    }
}

internal void set_text(
    UIText*    ui_text,
    StringView string,
    vec2       position,
    f32        scale,
    UIAnchorX  anchor_x,
    rgba       color
) {
    ui_text->text     = string;
    ui_text->position = position;
    ui_text->scale    = scale;
    ui_text->anchor_x = anchor_x;
    ui_text->color    = color;
}

b32 status_main_menu(
    struct GameMemory*  memory,
    struct RenderOrder* render_order
) {
    GameState* state = &memory->game_state;
    if( !state->main_menu.initialized ) {
        set_text(
            &memory->text[0],
            "Asteroids",
            v2(0.5f),
            1.0f,
            UI_ANCHOR_X_CENTER,
            RGBA::WHITE
        );
        f32 options_scale = 0.5f;
        set_text(
            &memory->text[1],
            "Start Game",
            v2(0.5f, 0.4f),
            options_scale,
            UI_ANCHOR_X_CENTER,
            RGBA::WHITE
        );
        set_text(
            &memory->text[2],
            "Quit Game",
            v2(0.5f, 0.3f),
            options_scale,
            UI_ANCHOR_X_CENTER,
            RGBA::WHITE
        );
        state->main_menu.initialized = true;
    }

    rgba selected_color     = RGBA::WHITE;
    rgba not_selected_color = RGBA::GRAY;

    memory->text[1].color = state->main_menu.menu_selection == 0 ?
        selected_color : not_selected_color;
    memory->text[2].color = state->main_menu.menu_selection == 1 ?
        selected_color : not_selected_color;

    render_order->text_count = 3;
    render_order->ui_text    = memory->text;

    b32 key_arrow_up   = input_is_key_down( KEY_ARROW_UP );
    b32 key_arrow_down = input_is_key_down( KEY_ARROW_DOWN );
    if(
        key_arrow_up &&
        key_arrow_up != input_was_key_down( KEY_ARROW_UP )
    ) {
        state->main_menu.menu_selection =
            (state->main_menu.menu_selection + 1) %
            GAME_STATE_MAIN_MENU_MAX_SELECTION;
    }
    if(
        key_arrow_down &&
        key_arrow_down != input_was_key_down( KEY_ARROW_DOWN )
    ) {
        state->main_menu.menu_selection =
            (state->main_menu.menu_selection - 1) %
            GAME_STATE_MAIN_MENU_MAX_SELECTION;
    }

    if( input_is_key_down( KEY_ENTER ) ) {
        switch( state->main_menu.menu_selection ) {
            case GAME_STATE_MAIN_MENU_START_GAME: {
                memory->asteroid_count = 0;
                game_state_set_status(
                    &memory->game_state,
                    GAME_STATUS_START
                );
            } break;
            case GAME_STATE_MAIN_MENU_QUIT_GAME: {
                Event event = {};
                event.code = EVENT_CODE_EXIT;
                event_fire( event );
            } break;
            default:
                break;
        }
    }

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

[[maybe_unused]]
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

internal void reset_game(
    struct GameMemory*    memory,
    struct EntityStorage* storage
) {
    Entity* ship_entity = &storage->entities[memory->ship_id];

    entity_set_active( ship_entity, true );

    Ship* ship = (Ship*)ship_entity->bytes;
    ship->transform.position = {};
    ship->transform.rotation = 0.0f;
    ship_entity->matrix = transform(
        ship->transform.position,
        ship->transform.rotation,
        ship->transform.scale
    );

    ship->is_invincible       = false;
    ship->blink_timer         = 0.0f;
    ship->invincibility_timer = 0.0f;

    for( u32 i = 0; i < MAX_TORPEDOES; ++i ) {
        EntityID id = memory->first_torpedo_id + i;
        Entity* entity = entity_storage_get( storage, id );
        entity_set_active( entity, false );
    }

    for( u32 i = 0; i < SHIP_DESTROYED_PIECE_COUNT; ++i ) {
        EntityID id = memory->first_ship_destroyed_id + i;
        Entity* entity = entity_storage_get( storage, id );
        entity_set_active( entity, false );
    }

    EntityStorageQueryResult asteroids =
        entity_storage_query( storage, filter_asteroids );
    QueryResultIterator iterator = &asteroids;
    EntityID id;
    while( iterator.next( &id ) ) {
        entity_storage_mark_null( storage, id );
    }

}

internal void game_set_life(
    GameMemory* memory,
    u32         new_life
) {
    u32 life = clamp( new_life, 0ul, PLAYER_MAX_LIVES );
    memory->ship_lives = life;
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

b32 status_start(
    struct GameMemory*    memory,
    struct EntityStorage* storage,
    struct Timer*         time,
    struct RenderOrder*   render_order
) {
    GameState* state = &memory->game_state;

    if( !state->start.initialized ) {
        reset_game( memory, storage );
        state->start.initialized = true;
        state->start.show_ship   = true;
        state->start.timer       = 0.0f;
        state->start.blink_timer = 0.0f;
    }

    state->start.blink_timer += time->delta_seconds;
    if( state->start.blink_timer >= GAME_STATE_START_BLINK_TIME ) {
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
    if( state->start.timer < GAME_STATE_START_TIME ) {
        return true;
    }

    if( !memory->ship_lives ) {
        game_set_life( memory, PLAYER_MAX_LIVES );
        memory->ship_lives = PLAYER_MAX_LIVES;
    }

    for( u32 i = 0; i < 3; ++i ) {
        game_generate_asteroid( memory, storage );
    }

    game_state_set_status( state, GAME_STATUS_PLAY );

    return true;
}

b32 status_play(
    struct GameMemory*    memory,
    struct EntityStorage* storage,
    struct Timer*         time,
    struct RenderOrder*   render_order,
    union  ivec2*         screen_dimensions
) {
    Entity* entity_ship = &storage->entities[memory->ship_id];
    Ship*   ship = (Ship*)entity_ship->bytes;

    b32 game_is_over = memory->game_state.status == GAME_STATUS_GAME_OVER;
    b32 ship_is_active = CHECK_BITS(
        entity_ship->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE
    ) && memory->ship_lives;

    render_order->ui_text = memory->text;
    if( game_is_over ) {
        set_text(
            &memory->text[1],
            "Game Over",
            v2(0.5f),
            0.5f,
            UI_ANCHOR_X_CENTER,
            RGBA::WHITE
        );

        render_order->text_count = 2;

        memory->game_over_timer += time->delta_seconds;
        if( memory->game_over_timer >= MAX_GAME_OVER_TIME ) {
            memory->game_over_timer = 0.0f;
            game_state_set_status( &memory->game_state, GAME_STATUS_MAIN_MENU );
            return true;
        }
    } else {
        render_order->text_count = 1;
    }

    StringView score_text;
    score_text.buffer = memory->game_state.play.score_text_buffer;
    score_text.len    = GAME_STATE_PLAY_SCORE_TEXT_BUFFER_SIZE;
    score_text.len =
        string_format( score_text, "{i,06}", memory->player_score );

    set_text(
        &memory->text[0],
        score_text,
        v2(0.025f, 0.85f),
        0.3f,
        UI_ANCHOR_X_LEFT,
        RGBA::WHITE
    );

#if DEBUG
    u32 ui_text_index = render_order->text_count++;

    local char memory_usage_buffer[32];
    StringView memory_usage_view;
    memory_usage_view.buffer = memory_usage_buffer;
    memory_usage_view.len    = 32;

    memory_usage_view.len =
        string_format( memory_usage_view, "{u}", memory->asteroid_count );

    set_text(
        &memory->text[ui_text_index],
        memory_usage_view,
        v2( 0.01f, 0.01f ),
        0.25f,
        UI_ANCHOR_X_LEFT,
        RGBA::WHITE
    );
#endif

    render_order->image_count = memory->ship_lives;
    render_order->ui_image    = memory->images;


    if( !game_is_over && ship_is_active ) {

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

    /* Torpedoes */
    if( game_is_over ) {

        QueryResultIterator iterator = &torpedoes;
        EntityID id;
        while( iterator.next( &id ) ) {
            Entity* torpedo_entity = entity_storage_get( storage, id );
            Torpedo* torpedo = (Torpedo*)torpedo_entity->bytes;
            torpedo->life_timer = 0.0f;
            entity_set_active( torpedo_entity, false );
        }

    } else {

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

        f32 aspect_ratio =
            (f32)screen_dimensions->x /
            (f32)screen_dimensions->y;
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
    if( !game_is_over ) {

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

                    game_set_life( memory, memory->ship_lives - 1 );
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

#if defined(DEBUG) && 0
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


