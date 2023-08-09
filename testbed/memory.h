#if !defined(TESTBED_MEMORY_HPP)
#define TESTBED_MEMORY_HPP
// * Description:  Testbed Memory
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include <defines.h>
#include <renderer/renderer.h>
#include <core/math/rand.h>
#include <core/event.h>

#include "status.h"

#define PLAYER_MAX_LIVES (3ul)
#define MAX_TORPEDOES (5)
#define MAX_TEXT (3)
#define MAX_IMAGES (3)
#define MAX_GAME_OVER_TIME (2.0f)

struct GameMemory {
    Texture         textures[3];
    RandXOR         rand_xor;
    EntityID        ship_id;
    EntityID        first_torpedo_id;
    EntityID        current_torpedo;
    EntityID        first_ship_destroyed_id;
    EventListenerID on_exit_listener;

    UIText text[MAX_TEXT];
    UIImage images[MAX_IMAGES];

    u32 player_score;
    u32 asteroid_count;
    u32 ship_lives;

    f32 game_over_timer;

    GameState game_state;
};


#endif // header guard
