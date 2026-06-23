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
}
Player_t;

extern Player_t Player;

//--------------------------------------------------------------
// Ghosts
//--------------------------------------------------------------
#define GHOST_BLINKY 0
#define GHOST_PINKY 1
#define GHOST_INKY 2
#define GHOST_CLYDE 3

//--------------------------------------------------------------
#define GHOST_HOME_X_DIFF - 3
#define GHOST_HOME_Y_DIFF 0

#define GHOST_DEAD_DELAY_MS 10

//--------------------------------------------------------------
#define GHOST_STATUS_ALIVE 0
#define GHOST_STATUS_DEAD 1

//--------------------------------------------------------------
// ghost strategies
//--------------------------------------------------------------
#define GHOST_STRATEGY_RANDOM 0
#define GHOST_STRATEGY_BLINKY 1
#define GHOST_STRATEGY_PINKY 2
#define GHOST_STRATEGY_INKY 3
#define GHOST_STRATEGY_CLYDE 4

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

extern Ghost_t Blinky;
extern Ghost_t Pinky;
extern Ghost_t Inky;
extern Ghost_t Clyde;

//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
uint32_t bot_calc_move_random(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_blinky(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_pinky(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_inky(uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_clyde(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_scatter(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir);
uint32_t bot_calc_move_home(uint32_t ghost, uint32_t xp, uint32_t yp, uint32_t akt_dir);

//--------------------------------------------------------------
#endif // __STM32F4_UB_BOT_H