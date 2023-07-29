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

#define ENTITY_TYPE_SHIP     1
#define ENTITY_TYPE_ASTEROID 2
#define ENTITY_TYPE_TORPEDO  3

const char* entity_type_to_string( EntityType type ) {
    switch( type ) {
        case ENTITY_TYPE_SHIP:     return "Ship";
        case ENTITY_TYPE_ASTEROID: return "Asteroid";
        case ENTITY_TYPE_TORPEDO:  return "Torpedo";
        default: return "null";
    }
}

#define SHIP_SPEED          (1.5f)
#define SHIP_ROTATION_SPEED (5.5f)
#define SHIP_SCALE          (0.05f)
struct Ship {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    f32            normal_drag;
    f32            stop_drag;
};
STATIC_ASSERT( sizeof(Ship) <= MAX_ENTITY_SIZE );
Entity ship_create( struct Texture* texture_atlas ) {
    Entity entity    = {};
    entity.type      = ENTITY_TYPE_SHIP;
    entity.state_flags = ENTITY_STATE_FLAG_IS_ACTIVE |
        ENTITY_STATE_FLAG_IS_2D |
        ENTITY_STATE_FLAG_IS_VISIBLE;
    entity.component_flags     =
        ENTITY_COMPONENT_FLAG_PHYSICS   |
        ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER |
        ENTITY_COMPONENT_FLAG_COLLIDER_2D;

    Ship* ship = (Ship*)entity.bytes;

    ship->transform.scale = { SHIP_SCALE, SHIP_SCALE };
    ship->normal_drag          = 1.2f;
    ship->stop_drag            = 2.5f;
    ship->physics.drag         = ship->normal_drag;
    ship->physics.angular_drag = ship->normal_drag;
    ship->sprite_renderer      = sprite_renderer_new( texture_atlas, 1 );
    ship->collider             = collider2d_new_rect(
        SHIP_SCALE * 2.0f,
        SHIP_SCALE * 2.0f
    );

    return entity;
}

#define TORPEDO_SCALE            (0.015f)
#define TORPEDO_LIFETIME_SECONDS (2.0f)
#define TORPEDO_SPEED            (SHIP_SPEED + 0.25f)
struct Torpedo {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    f32 lifetime_timer;
};
STATIC_ASSERT( sizeof(Torpedo) <= MAX_ENTITY_SIZE );
internal Entity torpedo_create( struct Texture* texture_atlas ) {
    Entity entity    = {};
    entity.type      = ENTITY_TYPE_TORPEDO;
    entity.state_flags =
        ENTITY_STATE_FLAG_IS_2D |
        ENTITY_STATE_FLAG_IS_VISIBLE;
    entity.component_flags     =
        ENTITY_COMPONENT_FLAG_PHYSICS   |
        ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER |
        ENTITY_COMPONENT_FLAG_COLLIDER_2D;

    Torpedo* torpedo = (Torpedo*)entity.bytes;

    torpedo->transform.scale = v2( TORPEDO_SCALE );
    torpedo->sprite_renderer = sprite_renderer_new( texture_atlas, 1 );
    torpedo->sprite_renderer.z_index = -1;
    torpedo->collider        = collider2d_new_rect(
        TORPEDO_SCALE * 2.0f,
        TORPEDO_SCALE * 2.0f
    );
    torpedo->lifetime_timer = 0.0f;

    return entity;
}
internal void torpedo_enable( Entity* entity, Ship* ship, vec2 ship_forward ) {
    Torpedo* current_torpedo = (Torpedo*)entity->bytes;
    current_torpedo->transform.position = ship->transform.position;
    current_torpedo->physics.velocity   = ship_forward * TORPEDO_SPEED;
    current_torpedo->lifetime_timer = 0.0f;
    entity_set_active( entity, true );
}

#define MAX_ASTEROID_LIFE (3ul)
struct Asteroid {
    Transform2D    transform;
    Physics2D      physics;
    SpriteRenderer sprite_renderer;
    Collider2D     collider;
    u32            life; // 0-2 value
};
STATIC_ASSERT( sizeof(Asteroid) <= MAX_ENTITY_SIZE );
inline void asteroid_set_life( Entity* entity, u32 new_life, RandXOR& rand_xor ) {
    ASSERT( entity->type == ENTITY_TYPE_ASTEROID );

    if( !new_life ) {
        entity_set_active( entity, false );
        return;
    }

    Asteroid* asteroid = (Asteroid*)entity->bytes;

    i32 cell_x = rand_xor.next_u32() % 3;
    i32 cell_y = (rand_xor.next_u32() % 2) + 1;

    asteroid->sprite_renderer.atlas_cell_position = { cell_x, cell_y };
    asteroid->life = clamp(new_life, 0ul, MAX_ASTEROID_LIFE);

    local const f32 ASTEROID_LIFE_SCALES[] = {
        0.0f, 0.25f, 0.6f, 1.0f
    };

    asteroid->transform.scale = (VEC2::ONE * 0.135f) * ASTEROID_LIFE_SCALES[asteroid->life];
    asteroid->collider = collider2d_new_rect(
        asteroid->transform.scale.x * 1.4f,
        asteroid->transform.scale.y * 1.4f
    );

    vec2 velocity = normalize( v2( rand_xor.next_f32(), rand_xor.next_f32() ) );
    asteroid->physics.velocity = velocity;
    asteroid->physics.angular_velocity = rand_xor.next_f32();
}
Entity asteroid_create( vec2 position, struct Texture* texture_atlas, RandXOR& rand_xor ) {
    Entity entity    = {};
    entity.type      = ENTITY_TYPE_ASTEROID;
    entity.state_flags = ENTITY_STATE_FLAG_IS_ACTIVE |
        ENTITY_STATE_FLAG_IS_2D |
        ENTITY_STATE_FLAG_IS_VISIBLE;
    entity.component_flags     =
        ENTITY_COMPONENT_FLAG_PHYSICS   |
        ENTITY_COMPONENT_FLAG_TRANSFORM |
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER |
        ENTITY_COMPONENT_FLAG_COLLIDER_2D;

    Asteroid* asteroid = (Asteroid*)entity.bytes;

    asteroid->transform.position = position;
    asteroid->sprite_renderer    = sprite_renderer_new( texture_atlas, 3 );

    asteroid_set_life( &entity, MAX_ASTEROID_LIFE, rand_xor );

    return entity;
}

#define MAX_TORPEDOES (5)
struct GameMemory {
    Texture         textures[3];
    RandXOR         rand_xor;
    EntityID        ship_id;
    EntityID        first_torpedo_id;
    EntityID        current_torpedo;
    u32             active_asteroid_count;
    u32             max_asteroids;
    EntityID        first_asteroid;
    EventListenerID on_exit_listener;
};

inline void asteroid_activate( Entity* entity, u32 life, GameMemory* game_memory ) {
    game_memory->active_asteroid_count++;
    ASSERT( game_memory->active_asteroid_count <= game_memory->max_asteroids );
    entity_set_active( entity, true );
    asteroid_set_life( entity, life, game_memory->rand_xor );
    Asteroid* asteroid = (Asteroid*)entity->bytes;
    vec2 velocity;
    velocity.x = game_memory->rand_xor.next_f32();
    velocity.y = game_memory->rand_xor.next_f32();

    f32 velocity_magnitude_t = game_memory->rand_xor.next_f32_01();
    f32 velocity_magnitude = lerp( 0.4f, 2.0f, velocity_magnitude_t );

    asteroid->physics.velocity = normalize( velocity ) * velocity_magnitude;
    asteroid->physics.angular_velocity = game_memory->rand_xor.next_f32();
}
inline void asteroid_damage( Entity* entity, GameMemory* game_memory, struct EntityStorage* storage ) {
    Asteroid* asteroid = (Asteroid*)entity->bytes;
    asteroid_set_life( entity, asteroid->life - 1, game_memory->rand_xor );
    if( !asteroid->life ) {
        ASSERT( game_memory->active_asteroid_count );
        game_memory->active_asteroid_count--;
        return;
    }

    if( game_memory->active_asteroid_count < game_memory->max_asteroids ) {
        u32 asteroid_piece_count = asteroid->life;
        u32 asteroids_that_can_be_spawned = game_memory->max_asteroids - game_memory->active_asteroid_count;
        u32 asteroids_to_spawn = min( asteroids_that_can_be_spawned, asteroid_piece_count );

        if( !asteroids_to_spawn ) {
            return;
        }

        for( u32 i = 0; i < asteroids_to_spawn; ++i ) {
            EntityID new_asteroid_id = game_memory->active_asteroid_count +
                game_memory->first_asteroid;
            Entity* entity = entity_storage_get( storage, new_asteroid_id );
            Asteroid* current_asteroid = (Asteroid*)entity->bytes;
            current_asteroid->transform.position = asteroid->transform.position;
            asteroid_activate( entity, asteroid->life, game_memory );
            game_memory->active_asteroid_count++;
        }

    }
}
inline void asteroid_spawn_new( GameMemory* game_memory, struct EntityStorage* storage ) {
    if( game_memory->active_asteroid_count >= game_memory->max_asteroids ) {
        LOG_WARN( "active asteroids: {u}", game_memory->active_asteroid_count );
        LOG_WARN( "max asteroids:    {u}", game_memory->max_asteroids );
        LOG_WARN( "maximum asteroids exceeded!" );
        return;
    }

    EntityID new_asteroid_id = game_memory->first_asteroid + game_memory->active_asteroid_count;
    Entity* entity = entity_storage_get( storage, new_asteroid_id );
    Asteroid* asteroid = (Asteroid*)entity->bytes;

    vec2 position;
    position.x = game_memory->rand_xor.next_f32_01();
    position.y = game_memory->rand_xor.next_f32_01();

    asteroid->transform.position = position;

    asteroid_activate( entity, MAX_ASTEROID_LIFE, game_memory );
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

    memory->rand_xor = RandXOR( 463457457 );

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

    u32 max_asteroids = MAX_ENTITIES - MAX_TORPEDOES - 1;
    for( u32 i = 0; i < max_asteroids; ++i ) {
        Entity asteroid = asteroid_create( VEC2::ZERO, &memory->textures[2], memory->rand_xor );
        entity_set_active( &asteroid, false );
        EntityID id = entity_storage_create_entity( storage, &asteroid );
        ASSERT( id >= 0 );
        if( i == 0 ) {
            memory->first_asteroid = id;
        }
    }
    memory->max_asteroids = max_asteroids;

    for( u32 i = 0; i < 3; ++i ) {
        asteroid_spawn_new( memory, storage );
    }

    memory->on_exit_listener =
        event_subscribe( EVENT_CODE_EXIT, on_exit, memory );

    return true;
}

internal b32 is_entity_active_visible_2d( Entity* entity ) {
    return CHECK_BITS(
        entity->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE  |
        ENTITY_STATE_FLAG_IS_VISIBLE | 
        ENTITY_STATE_FLAG_IS_2D 
    );
}

[[maybe_unused]]
internal b32 filter_active_torpedoes( Entity* entity ) {
    b32 is_torpedo = entity->type == ENTITY_TYPE_TORPEDO;
    if( !is_torpedo ) {
        return false;
    }

    return is_entity_active_visible_2d( entity );
}
[[maybe_unused]]
internal b32 filter_colliders( Entity* entity ) {
    b32 has_collider2d = CHECK_BITS(
        entity->component_flags,
        ENTITY_COMPONENT_FLAG_COLLIDER_2D
    );
    return is_entity_active_visible_2d( entity ) && has_collider2d;
}

internal b32 filter_asteroids( Entity* entity ) {
    if( entity->type != ENTITY_TYPE_ASTEROID ) {
        return false;
    }
    return is_entity_active_visible_2d( entity );
}

internal b32 filter_sprites( Entity* entity ) {
    b32 is_active_visible_2d = CHECK_BITS(
        entity->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE |
        ENTITY_STATE_FLAG_IS_VISIBLE |
        ENTITY_STATE_FLAG_IS_2D
    );
    b32 has_sprite_renderer = CHECK_BITS(
        entity->component_flags,
        ENTITY_COMPONENT_FLAG_SPRITE_RENDERER
    );
    return is_active_visible_2d && has_sprite_renderer;
}

internal b32 filter_active_visible2d( Entity* entity ) {
    b32 is_active_visible_2d = CHECK_BITS(
        entity->state_flags,
        ENTITY_STATE_FLAG_IS_ACTIVE  |
        ENTITY_STATE_FLAG_IS_VISIBLE |
        ENTITY_STATE_FLAG_IS_2D      
    );
    return is_active_visible_2d;
}

extern "C"
b32 application_run( struct EngineContext* ctx, void* generic_memory ) {
    GameMemory*    memory  = (GameMemory*)generic_memory;
    EntityStorage* storage = engine_get_entity_storage( ctx );
    Timer*         time    = engine_get_time( ctx );

    if( input_is_key_down( KEY_ESCAPE ) ) {
        Event event = {};
        event.code  = EVENT_CODE_EXIT;
        event_fire( event );
    }

    Entity* entity_ship = &storage->entities[memory->ship_id];
    Ship*   ship = (Ship*)entity_ship->bytes;

    vec2 input_direction = {};
    input_direction.x = (f32)input_is_key_down( KEY_ARROW_RIGHT ) -
        (f32)input_is_key_down( KEY_ARROW_LEFT );
    input_direction.y = (f32)input_is_key_down( KEY_ARROW_UP );

    ship->physics.drag = input_is_key_down( KEY_ARROW_DOWN ) ?
        ship->stop_drag : ship->normal_drag;
    ship->physics.angular_drag = ship->physics.drag;

    vec2 ship_forward_direction = rotate(
        VEC2::UP, ship->transform.rotation
    );

    ship->physics.velocity +=
        ship_forward_direction *
        input_direction.y *
        time->delta_seconds *
        SHIP_SPEED;

    ship->physics.angular_velocity +=
        input_direction.x *
        time->delta_seconds *
        SHIP_ROTATION_SPEED;

    b32 z_is_down = input_is_key_down( KEY_Z );
    if(
        z_is_down &&
        z_is_down != input_was_key_down( KEY_Z )
    ) {

        EntityID id = memory->first_torpedo_id + memory->current_torpedo;
        Entity* torpedo = &storage->entities[id];
        torpedo_enable( torpedo, ship, ship_forward_direction );
        memory->current_torpedo = (memory->current_torpedo + 1) % MAX_TORPEDOES;
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
            Entity*  entity  = entity_storage_get( storage, id );
            Torpedo* torpedo = (Torpedo*)(entity->bytes);
            Entity* hit_result = system_collider2d_solver( storage, id, &asteroids );

            if( hit_result ) {
                asteroid_damage( hit_result, memory, storage );
                entity_set_active( entity, false );
                continue;
            }
            
            torpedo->lifetime_timer += time->delta_seconds;
            if( torpedo->lifetime_timer >= TORPEDO_LIFETIME_SECONDS ) {
                entity_set_active( entity, false );
            }

        }
    }

    /* Physics and Wrapping */ {
        EntityStorageQueryResult physics_objects = system_physics2d_solver(
            storage, time->delta_seconds
        );

        ivec2 dimensions = engine_query_surface_size( ctx );
        f32 aspect_ratio = (f32)dimensions.x / (f32)dimensions.y;
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

    /* calculate transforms */ {
        EntityStorageQueryResult active_objects = entity_storage_query(
            storage, filter_active_visible2d
        );
        QueryResultIterator iterator = &active_objects;
        EntityID id;
        while( iterator.next( &id ) ) {
            Entity* entity = entity_storage_get( storage, id );
            entity->matrix =
                translate( entity->transform2d.position ) *
                rotate( entity->transform2d.rotation ) *
                scale( entity->transform2d.scale );
        }
    }
    RenderOrder* render_order = engine_get_render_order( ctx );

    EntityStorageQueryResult sprites = entity_storage_query(
        storage, filter_sprites
    );
    render_order->storage = storage;
    render_order->sprites = sprites;

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

