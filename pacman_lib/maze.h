#ifndef __STM32F4_UB_MAZE_H
#define __STM32F4_UB_MAZE_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "bot.h"

#define ABS(X)  ((X) > 0 ? (X) : -(X))

//------------------------------------------------------------------------
#define  ROOM_CNT_X  28 // [0...27]
#define  ROOM_CNT_Y  31 // [0...30]



//------------------------------------------------------------------------
#define  ROOM_TYP_WALL    0
#define  ROOM_TYP_PATH    1


//------------------------------------------------------------------------
#define  ROOM_SPEC_NONE    0
#define  ROOM_SPEC_PORTAL  1
#define  ROOM_SPEC_GATE    2

//------------------------------------------------------------------------
#define  ROOM_DOOR_NONE     0x00
#define  ROOM_DOOR_U        0x01 // Bit0
#define  ROOM_DOOR_R        0x02 // Bit1
#define  ROOM_DOOR_D        0x04 // Bit2
#define  ROOM_DOOR_L        0x08 // Bit3
#define  ROOM_BGATE_U       0x10 // Bit4
#define  ROOM_BGATE_R       0x20 // Bit5
#define  ROOM_BGATE_D       0x40 // Bit6
#define  ROOM_BGATE_L       0x80 // Bit7
#define  ROOM_PGATE_U      0x100 // Bit8
#define  ROOM_PGATE_R      0x200 // Bit9
#define  ROOM_PGATE_D      0x400 // Bit10
#define  ROOM_PGATE_L      0x800 // Bit11
#define  ROOM_IGATE_U     0x1000 // Bit12
#define  ROOM_IGATE_R     0x2000 // Bit13
#define  ROOM_IGATE_D     0x4000 // Bit14
#define  ROOM_IGATE_L     0x8000 // Bit15
#define  ROOM_CGATE_U    0x10000 // Bit16
#define  ROOM_CGATE_R    0x20000 // Bit17
#define  ROOM_CGATE_D    0x40000 // Bit18
#define  ROOM_CGATE_L    0x80000 // Bit19



//------------------------------------------------------------------------
#define  ROOM_POINTS_NONE    0
#define  ROOM_POINTS_NORMAL  1
#define  ROOM_POINTS_ENERGY  2

//------------------------------------------------------------------------
#define  ROOM_DEB_OK     0x00
#define  ROOM_DEB_BORDER 0x01 // Bit0
#define  ROOM_DEB_PATH   0x02 // Bit1
#define  ROOM_DEB_DOOR   0x04 // Bit2
#define  ROOM_DEB_SKIN   0x08 // Bit3

 
//------------------------------------------------------------------------
// structure for one room
//------------------------------------------------------------------------
typedef struct {
  uint32_t typ;
  uint32_t special;
  uint32_t door; 
  uint32_t skin;
  uint32_t points;
  uint32_t deb_err;  // for debug
}Room_t;


//------------------------------------------------------------------------
// structure for the maze
//------------------------------------------------------------------------
typedef struct {
  uint32_t point_dots;
  Room_t Room[ROOM_CNT_X][ROOM_CNT_Y];
}Maze_t;
extern Maze_t Maze;





//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void maze_build(void);


//--------------------------------------------------------------
#endif // __STM32F4_UB_MAZE_H
