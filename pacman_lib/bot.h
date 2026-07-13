//--------------------------------------------------------------
// File     : bot.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_BOT_H
#define __STM32F4_UB_BOT_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"

#include "maze.h"

#define STRING_FLOAT_FAKTOR 1000 // 1000 = 3 decimal places
#define STRING_FLOAT_FORMAT "%d.%03d" // formatting

//--------------------------------------------------------------
// integer SQRT
//--------------------------------------------------------------
#define SQRT_FAKTOR 100 // dont touch this value
#define INIT_DISTANCE 99999 // dont touch this value

//--------------------------------------------------------------
// movements
//--------------------------------------------------------------
#define MOVE_STOP 0
#define MOVE_UP 1
#define MOVE_RIGHT 2
#define MOVE_DOWN 3
#define MOVE_LEFT 4

//--------------------------------------------------------------
// port states
//--------------------------------------------------------------
#define PORT_DONE 0
#define PORT_UP0 1
#define PORT_UP1 2
#define PORT_RIGHT0 3
#define PORT_RIGHT1 4
#define PORT_DOWN0 5
#define PORT_DOWN1 6
#define PORT_LEFT0 7
#define PORT_LEFT1 8

//--------------------------------------------------------------
// struct ot the player
//--------------------------------------------------------------
typedef struct {
    uint32_t status;
    uint32_t xp;
    uint32_t yp;
    uint32_t skin;
    uint32_t skin_cnt;
    int32_t delta_x; // signed int !!
    int32_t delta_y; // signed int !!
    uint32_t move;
    uint32_t port;
    uint32_t level;
    uint32_t score;
    uint32_t point_dots;
    uint32_t lives;
    uint32_t akt_speed_ms;
    uint32_t frightened_buf;
    // Biến lưu vị trí hồi sinh dự kiến cho chế độ 2 người chơi
    uint32_t respawn_x;
    uint32_t respawn_y;
}
Player_t;

extern Player_t Player;
extern Player_t Player2;

//--------------------------------------------------------------
// Ghosts: GHOST_MAX AI slots + optional human-controlled ghost
//--------------------------------------------------------------
#define GHOST_MAX   4
#define GHOST_0     0
#define GHOST_1     1
#define GHOST_2     2
#define GHOST_3     3
#define GHOST_HUMAN 4

//--------------------------------------------------------------
#define GHOST_HOME_X_DIFF - 3
#define GHOST_HOME_Y_DIFF 0

#define GHOST_DEAD_DELAY_MS 10

//--------------------------------------------------------------
#define GHOST_STATUS_ALIVE 0
#define GHOST_STATUS_DEAD 1

//--------------------------------------------------------------
// Ghost personalities (type) — not tied to a fixed slot
//--------------------------------------------------------------
#define GHOST_TYPE_RANDOM 0
#define GHOST_TYPE_CHASE  1
#define GHOST_TYPE_AMBUSH 2
#define GHOST_TYPE_TRICKY 3
#define GHOST_TYPE_SHY    4
#define GHOST_TYPE_DRUNK  5
#define GHOST_TYPE_LAZY   6

#define GHOST_TYPE_COUNT  7

/* Custom mode exposes the 6 playable personalities (Random is internal only). */
#define GHOST_TYPE_CUSTOM_MIN  GHOST_TYPE_CHASE
#define GHOST_TYPE_CUSTOM_MAX  GHOST_TYPE_LAZY

/* Legacy aliases (strategy naming) */
#define GHOST_STRATEGY_RANDOM  GHOST_TYPE_RANDOM
#define GHOST_STRATEGY_BLINKY  GHOST_TYPE_CHASE
#define GHOST_STRATEGY_PINKY   GHOST_TYPE_AMBUSH
#define GHOST_STRATEGY_INKY    GHOST_TYPE_TRICKY
#define GHOST_STRATEGY_CLYDE   GHOST_TYPE_SHY
#define GHOST_STRATEGY_DRUNK   GHOST_TYPE_DRUNK
#define GHOST_STRATEGY_LAZY    GHOST_TYPE_LAZY
#define GHOST_STRATEGY_COUNT   GHOST_TYPE_COUNT
#define GHOST_STRATEGY_CUSTOM_MIN  GHOST_TYPE_CUSTOM_MIN
#define GHOST_STRATEGY_CUSTOM_MAX  GHOST_TYPE_CUSTOM_MAX

/* Legacy slot id aliases */
#define GHOST_BLINKY GHOST_0
#define GHOST_PINKY  GHOST_1
#define GHOST_INKY   GHOST_2
#define GHOST_CLYDE  GHOST_3

#define GHOST_SPAWN_MIN_DIST 64

/* Ghost-house exit tile on the main corridor (walkable by Pacman on all maps). */
#define GHOST_HOUSE_EXIT_X 14
#define GHOST_HOUSE_EXIT_Y 11

#define GHOST_SPEED_SLOW_MS 70
#define GHOST_SPEED_NORMAL_MS 50
#define GHOST_SPEED_FAST_MS 30

//--------------------------------------------------------------
// ghost structure
//--------------------------------------------------------------
typedef struct {
    uint32_t status;
    uint32_t strategy;
    uint32_t xp;
    uint32_t yp;
    uint32_t skin;
    uint32_t skin_cnt;
    int32_t delta_x; // signed int !!
    int32_t delta_y; // signed int !!
    uint32_t move;
    uint32_t next_move;
    uint32_t port;
    uint32_t dot_cnt;
    uint32_t akt_speed_ms;
    uint32_t frightened_buf;
    uint32_t new_mode;
}
Ghost_t;

extern Ghost_t Ghosts[GHOST_MAX];
extern Ghost_t HumanGhost;

//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
uint32_t bot_calc_move_random(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_blinky(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_pinky(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_inky(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_clyde(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_lazy(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_by_strategy(uint32_t ghost, uint32_t strategy, uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_scatter(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_home(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_dead(uint32_t ghost_id, uint32_t xp, uint32_t yp, uint32_t akt_dir);
void bot_ghost_instant_revive(Ghost_t *ghost, uint32_t ghost_id);
void bot_ghost_eaten_by_pacman(Ghost_t *ghost, uint32_t ghost_id);
uint32_t bot_calc_only_exit(uint32_t xp, uint32_t yp);
void bot_apply_custom_ghosts(uint32_t ghost_count, uint32_t strategies[4], uint32_t speed_ms);
const char* bot_strategy_name(uint32_t strategy);
uint32_t bot_is_2p_coop(void);
uint32_t bot_is_2p_vs_ghost(void);
uint32_t bot_custom_ai_ghost_count(void);
uint32_t bot_is_player_controlled_ghost(uint32_t ghost_id);
uint32_t bot_is_human_ghost_active(void);
void bot_init_human_ghost(uint32_t speed_ms, const uint32_t *used_x, const uint32_t *used_y, uint32_t used_cnt);
uint32_t bot_calc_move_player_ghost(uint32_t xp, uint32_t yp, uint32_t akt_dir, uint32_t joy);
void bot_apply_player_ghost_input(Ghost_t *ghost, uint32_t joy);
void bot_get_nearest_player(uint32_t xp, uint32_t yp, uint32_t *txp, uint32_t *typ);
Player_t* bot_get_nearest_player_ptr(uint32_t xp, uint32_t yp);
void bot_kill_pacman(Player_t *p, uint32_t start_x, uint32_t start_y);
void bot_team_kill_pacman(void);
uint32_t bot_coop_is_game_over(void);
void bot_team_win_pacman(void);
void bot_ghost_hit_pacman(uint32_t gxp, uint32_t gyp, Ghost_t *ghost);
uint32_t bot_is_walkable(uint32_t x, uint32_t y, uint32_t for_ghost);
uint32_t bot_ghost_can_harm_pacman(Ghost_t *ghost, uint32_t ghost_id);
uint32_t bot_player_can_turn(uint32_t xp, uint32_t yp, uint32_t dir);
void bot_ghost_validate_position(Ghost_t *ghost);
void bot_ghost_unstick(Ghost_t *ghost, uint32_t ghost_id);
void bot_ghost_try_revive(Ghost_t *ghost, uint32_t ghost_id);
uint32_t bot_ghost_can_turn(uint32_t xp, uint32_t yp, uint32_t dir);
uint32_t bot_should_allow_ghost_move(void);
void bot_release_ghosts_on_pacman_death(void);
uint32_t bot_ghost_get_body_color(uint32_t ghost_id, const Ghost_t *ghost, uint16_t *color);
void bot_find_safe_respawn(uint32_t start_x, uint32_t start_y, uint32_t *respawn_x, uint32_t *respawn_y);

//--------------------------------------------------------------
#endif // __STM32F4_UB_BOT_H