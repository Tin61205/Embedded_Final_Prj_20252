//--------------------------------------------------------------
// File     : player.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_PLAYER_H
#define __STM32F4_UB_PLAYER_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "maze.h"
#include "gui.h"
#include "bot.h"
#include "stm32_ub_uart.h"

//--------------------------------------------------------------
#define  PLAYER_START_X     14
#define  PLAYER_START_Y     23   

//--------------------------------------------------------------
#define  PLAYER_START_LIVES   3 // The number of live

//--------------------------------------------------------------
#define  PLAYER_FRIGHTENED_BUF   5  // speed up player
#define  PLAYER_MAX_SPEED       10  // fastest speed of player in ms

//--------------------------------------------------------------
#define  PLAYER_STATUS_ALIVE  0
#define  PLAYER_STATUS_WIN    1
#define  PLAYER_STATUS_DEAD   2

//--------------------------------------------------------------
// Global Function
//--------------------------------------------------------------
void player_init(uint32_t mode);
void player_move(void);
void player_change_direction(uint32_t joy);



//--------------------------------------------------------------
#endif // __STM32F4_UB_PLAYER_H
