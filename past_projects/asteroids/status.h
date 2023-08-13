#if !defined(TESTBED_STATUS_HPP)
#define TESTBED_STATUS_HPP
// * Description:  Testbed Status
// * Author:       Alicia Amarilla (smushyaa@gmail.com)
// * File Created: August 09, 2023
#include "defines.h"

enum GameStatus : u32 {
    GAME_STATUS_MAIN_MENU,
    GAME_STATUS_START,
    GAME_STATUS_PLAY,
    GAME_STATUS_GAME_OVER,
};

#define GAME_STATE_MAIN_MENU_MAX_SELECTION (2ul)
#define GAME_STATE_MAIN_MENU_START_GAME (0ul)
#define GAME_STATE_MAIN_MENU_QUIT_GAME  (1ul)
struct GameStateMainMenu {
    u32 menu_selection;
    b32 initialized;
};

#define GAME_STATE_START_TIME (2.0f)
#define GAME_STATE_START_BLINK_TIME (GAME_STATE_START_TIME / 12.0f)
struct GameStateStart {
    f32 timer;
    f32 blink_timer;
    b32 show_ship;
    b32 initialized;
};

#define GAME_STATE_PLAY_RESPAWN_TIME (3.0f)
#define GAME_STATE_PLAY_SCORE_TEXT_BUFFER_SIZE (32ul)
struct GameStatePlay {
    f32  respawn_timer;
    char score_text_buffer[GAME_STATE_PLAY_SCORE_TEXT_BUFFER_SIZE];
};

struct GameState {
    GameStatus status;
    union {
        GameStateMainMenu main_menu;
        GameStateStart    start;
        GameStatePlay     play;
    };
};
void game_state_set_status( GameState* state, GameStatus status );

b32 status_main_menu(
    struct GameMemory*  memory,
    struct RenderOrder* render_order
);

b32 status_start(
    struct GameMemory*    memory,
    struct EntityStorage* storage,
    struct Timer*         time,
    struct RenderOrder*   render_order
);

b32 status_play(
    struct GameMemory*    memory,
    struct EntityStorage* storage,
    struct Timer*         time,
    struct RenderOrder*   render_order,
    union  ivec2*         screen_dimensions
);

#endif // header guard

