//--------------------------------------------------------------
// File     : skin.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SKIN_H
#define __STM32F4_UB_SKIN_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "maze.h"

//--------------------------------------------------------------
// width and heigth of a room (in pixel)
//--------------------------------------------------------------
#define  ROOM_WIDTH   8
#define  ROOM_HEIGHT  8

//--------------------------------------------------------------
// width and heigth of a bot (in pixel)
//--------------------------------------------------------------
#define  BOTS_WIDTH    14
#define  BOTS_HEIGHT   14


//--------------------------------------------------------------
// x and y position correction for a bot
//--------------------------------------------------------------
#define  BOTS_DIFF_X   -3
#define  BOTS_DIFF_Y   -3

//------------------------------------------------------------------------
// wall skins
//------------------------------------------------------------------------
#define  ROOM_SKIN_WALL_STD             0
#define  ROOM_SKIN_PATH_STD             1
#define  ROOM_SKIN_WALL_UNDEF           2
#define  ROOM_SKIN_WALL_BLACK           3
#define  ROOM_SKIN_WALL_O_TOP           4
#define  ROOM_SKIN_WALL_O_RIGHT         5
#define  ROOM_SKIN_WALL_O_BOTTOM        6
#define  ROOM_SKIN_WALL_O_LEFT          7
#define  ROOM_SKIN_WALL_O_TOPRIGHT1     8
#define  ROOM_SKIN_WALL_O_BOTTOMRIGHT1  9
#define  ROOM_SKIN_WALL_O_BOTTOMLEFT1   10
#define  ROOM_SKIN_WALL_O_TOPLEFT1      11
#define  ROOM_SKIN_WALL_O_TOPRIGHT2     12
#define  ROOM_SKIN_WALL_O_BOTTOMRIGHT2  13
#define  ROOM_SKIN_WALL_O_BOTTOMLEFT2   14
#define  ROOM_SKIN_WALL_O_TOPLEFT2      15
#define  ROOM_SKIN_WALL_O_HORTOP1       16
#define  ROOM_SKIN_WALL_O_HORTOP2       17
#define  ROOM_SKIN_WALL_O_HORBOTTOM1    18
#define  ROOM_SKIN_WALL_O_HORBOTTOM2    19
#define  ROOM_SKIN_WALL_O_VERLEFT1      20
#define  ROOM_SKIN_WALL_O_VERLEFT2      21
#define  ROOM_SKIN_WALL_O_VERRIGHT1     22
#define  ROOM_SKIN_WALL_O_VERRIGHT2     23
#define  ROOM_SKIN_WALL_I_TOP           24
#define  ROOM_SKIN_WALL_I_RIGHT         25
#define  ROOM_SKIN_WALL_I_BOTTOM        26
#define  ROOM_SKIN_WALL_I_LEFT          27
#define  ROOM_SKIN_WALL_I_TOPRIGHT1     28
#define  ROOM_SKIN_WALL_I_BOTTOMRIGHT1  29
#define  ROOM_SKIN_WALL_I_BOTTOMLEFT1   30
#define  ROOM_SKIN_WALL_I_TOPLEFT1      31
#define  ROOM_SKIN_WALL_I_TOPRIGHT2     32
#define  ROOM_SKIN_WALL_I_BOTTOMRIGHT2  33
#define  ROOM_SKIN_WALL_I_BOTTOMLEFT2   34
#define  ROOM_SKIN_WALL_I_TOPLEFT2      35
#define  ROOM_SKIN_HOME_DOOR            36
#define  ROOM_SKIN_POINTS_NONE          37
#define  ROOM_SKIN_POINTS_NORMAL        38
#define  ROOM_SKIN_POINTS_ENERGY        39

#define  ROOM_SKIN_CNT                  40  // last Skin_Nr+1


//------------------------------------------------------------------------
// player skins
//------------------------------------------------------------------------
#define  PLAYER_SKIN_FULL               0
#define  PLAYER_SKIN_UP1                1
#define  PLAYER_SKIN_UP2                2
#define  PLAYER_SKIN_RIGHT1             3
#define  PLAYER_SKIN_RIGHT2             4
#define  PLAYER_SKIN_DOWN1              5
#define  PLAYER_SKIN_DOWN2              6
#define  PLAYER_SKIN_LEFT1              7
#define  PLAYER_SKIN_LEFT2              8

#define  PLAYER_SKIN_CNT                9  // last Skin_Nr+1

//------------------------------------------------------------------------
// bot skins
//------------------------------------------------------------------------
#define  GHOST_SKIN_UP1                0
#define  GHOST_SKIN_UP2                1
#define  GHOST_SKIN_RIGHT1             2
#define  GHOST_SKIN_RIGHT2             3
#define  GHOST_SKIN_DOWN1              4
#define  GHOST_SKIN_DOWN2              5
#define  GHOST_SKIN_LEFT1              6
#define  GHOST_SKIN_LEFT2              7
#define  GHOST_SKIN_FRIGHTEN1          8
#define  GHOST_SKIN_FRIGHTEN2          9
#define  GHOST_SKIN_DEAD               10

#define  GHOST_SKIN_CNT                11  // last Skin_Nr+1


//--------------------------------------------------------------
// width and heigth of a button (in pixel)
//--------------------------------------------------------------
#define  BUTTON_WIDTH    24
#define  BUTTON_HEIGHT   24

//------------------------------------------------------------------------
// button skins
//------------------------------------------------------------------------
#define  BUTTON_SKIN1                  0
#define  BUTTON_SKIN2                  1

#define  BUTTON_SKIN_CNT               2  // last Skin_Nr+1


//------------------------------------------------------------------------
// skin structure
//------------------------------------------------------------------------
typedef struct {
  uint32_t xp;
  uint32_t yp;
}Skin_t;


//------------------------------------------------------------------------
// skin arrays
//------------------------------------------------------------------------
extern Skin_t Room_Skin[ROOM_SKIN_CNT];
extern Skin_t Player_Skin[PLAYER_SKIN_CNT];
/* One sprite sheet row per AI slot (base art); body color recolored by type. */
extern Skin_t Ghost_Skin[4][GHOST_SKIN_CNT];
extern Skin_t Button_Skin[BUTTON_SKIN_CNT];


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void skin_init(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_SKIN_H
