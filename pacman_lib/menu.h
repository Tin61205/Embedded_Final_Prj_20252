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
#include "stm32_ub_usb_hid_host.h"
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include "stm32_ub_systick.h"
#include "stm32_ub_button.h"

#include "pacman.h"


//--------------------------------------------------------------
// Menu
//--------------------------------------------------------------

#define  MENUE_COL_ON    RGB_COL_RED
#define  MENUE_COL_OFF   RGB_COL_BLUE
#define  MENUE_COL_VALUE RGB_COL_WHITE

#define  MENUE_MAX_POS   2

#define  MENUE_STARTX1   20
#define  MENUE_STARTX2   150
#define  MENUE_STARTY    70

#define  MENUE_DELTA1    20



//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void menu_start(void);



//--------------------------------------------------------------
#endif // __STM32F4_UB_MENU_H
