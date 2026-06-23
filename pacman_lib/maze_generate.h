#ifndef __STM32F4_UB_MAZE_GEN_H
#define __STM32F4_UB_MAZE_GEN_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "maze.h"
#include "skin.h"

//--------------------------------------------------------------
#define  SEARCH_ROOMS_ALL   0 // set data of all rooms
#define  SEARCH_DOORS_YES   1 // set typ+skin of all rooms with a door
#define  SEARCH_SKIN_PATH   2 // set skin of all "path"
#define  SEARCH_SKIN_UNDEF  3 // set skin of all "undef" rooms


void maze_generate_searchandset(uint32_t mark, Room_t room);
uint32_t maze_generate_digpath_h(uint32_t x,uint32_t y,uint32_t l,uint32_t p);
uint32_t maze_generate_digpath_v(uint32_t x,uint32_t y,uint32_t l,uint32_t p);
uint32_t maze_generate_setportals(uint32_t x,uint32_t y,Room_t room);
uint32_t maze_generate_setgate(uint32_t x,uint32_t y,Room_t room, uint32_t door);
void maze_generate_setwallskin_outside(void);
void maze_generate_setwallskin_inside(void);
uint32_t maze_generate_setskin_h(uint32_t x,uint32_t y,uint32_t l,uint32_t s);
uint32_t maze_generate_setskin_v(uint32_t x,uint32_t y,uint32_t l,uint32_t s);
uint32_t maze_generate_check(void);
uint32_t maze_generate_count_dots(void);


//--------------------------------------------------------------
#endif // __STM32F4_UB_MAZE_GEN_H
