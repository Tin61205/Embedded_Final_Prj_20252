//--------------------------------------------------------------
// File     : menu.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_MENU_H
#define __STM32F4_UB_MENU_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "gui.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32_ub_systick.h"
#include "stm32_ub_button.h"
#include "stm32_ub_touch_stmpe811.h"

#include "pacman.h"


//--------------------------------------------------------------
// Menu
//--------------------------------------------------------------

#define  MENUE_COL_ON    RGB_COL_RED
#define  MENUE_COL_OFF   RGB_COL_BLUE
#define  MENUE_COL_VALUE RGB_COL_WHITE

#define  MENUE_STARTX1   10
#define  MENUE_STARTY    40
#define  MENUE_DELTA1    22

#define  MENU_BTN_BACK_X  10
#define  MENU_BTN_NEXT_X  170
#define  MENU_BTN_Y       290
#define  MENU_BTN_W       60
#define  MENU_BTN_H       22

#define  MENU_RESULT_NONE     0
#define  MENU_RESULT_CAMPAIGN 1
#define  MENU_RESULT_CUSTOM   2



//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
uint32_t menu_start(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_MENU_H