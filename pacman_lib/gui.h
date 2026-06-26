//--------------------------------------------------------------
// File     : gui.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_GUI_H
#define __STM32F4_UB_GUI_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32_ub_lcd_ili9341.h"
#include "stm32_ub_graphic2d.h"
#include "stm32_ub_font.h"
#include "stm32_ub_systick.h"
#include "stm32_ub_uart.h"
#include "stm32_ub_button.h"
#include "stm32_ub_joystick.h"
#include "pacman.h"
#include "maze.h"
#include "skin.h"
#include "player.h"
#include "blinky.h"
#include "pinky.h"
#include "inky.h"
#include "clyde.h"




//--------------------------------------------------------------
// gui structure
//--------------------------------------------------------------
typedef struct {
  uint32_t refresh_value;
  uint32_t refresh_buttons;
}GUI_t;
extern GUI_t GUI;


//--------------------------------------------------------------
// UART debug
//--------------------------------------------------------------
#define  USE_GUI_UART_DEBUG    0  // 1=enable, 0=disble debug over uart


//--------------------------------------------------------------
// color defines
//--------------------------------------------------------------
#define  BACKGROUND_COL     RGB_COL_BLACK

#define  ROOM_COL        RGB_COL_GREY
#define  WALL_COL        RGB_COL_BLUE
#define  ERR_COL         RGB_COL_RED
#define  BUTTON_COL_OFF  RGB_COL_BLUE
#define  BUTTON_COL_ON   RGB_COL_RED
#define  FONT_COL        RGB_COL_BLUE
#define  FONT_COL2       RGB_COL_YELLOW
#define  FONT_COL3       RGB_COL_GREY
#define  GHOST_P2_COLOR    RGB_COL_GREEN
#define  GHOST_COLOR_CHASE RGB_COL_RED
#define  GHOST_COLOR_AMBUSH RGB_COL_MAGENTA
#define  GHOST_COLOR_TRICKY RGB_COL_CYAN
#define  GHOST_COLOR_SHY   RGB_COL_YELLOW
#define  GHOST_COLOR_DRUNK RGB_COL_WHITE
#define  GHOST_COLOR_LAZY  RGB_COL_GREY
#define  GHOST_COLOR_RANDOM RGB_COL_BLUE





//--------------------------------------------------------------
// screen positions
//--------------------------------------------------------------
#define  GUI_MAZE_STARTX  8
#define  GUI_MAZE_STARTY  5

#define  GUI_BTN_UP_X   170
#define  GUI_BTN_UP_Y   255

#define  GUI_BTN_DOWN_X   170
#define  GUI_BTN_DOWN_Y   280

#define  GUI_BTN_RIGHT_X   200
#define  GUI_BTN_RIGHT_Y   270

#define  GUI_BTN_LEFT_X   140
#define  GUI_BTN_LEFT_Y   270

//--------------------------------------------------------------
#define  GUI_JOY_NONE    0
#define  GUI_JOY_UP      1
#define  GUI_JOY_RIGHT   2
#define  GUI_JOY_DOWN    3
#define  GUI_JOY_LEFT    4


//--------------------------------------------------------------
#define  GUI_TOUCH_INTERVALL_MS    50


//--------------------------------------------------------------
#define  GUI_REFRESH_VALUE   3  // dont change this value

#define  GUI_PAUSE_CONTINUE  0
#define  GUI_PAUSE_EXIT      1

#define  GUI_PAUSE_BOX_X     20
#define  GUI_PAUSE_BOX_Y     70
#define  GUI_PAUSE_BOX_W     200
#define  GUI_PAUSE_BOX_H     180

#define  GUI_PAUSE_CONTINUE_X  30
#define  GUI_PAUSE_EXIT_X      130
#define  GUI_PAUSE_BTN_Y       200
#define  GUI_PAUSE_BTN_W       80
#define  GUI_PAUSE_BTN_H       28


//--------------------------------------------------------------
// Globale Funktionen
//--------------------------------------------------------------
void gui_clear_screen(void);
void gui_draw_maze(void);
void gui_draw_errmaze(void);
void gui_clear_bots(void);
void gui_draw_bots(void);
void gui_draw_gui(uint32_t joy);
uint32_t gui_check_touch(void);
uint32_t gui_check_button(void);
uint32_t gui_check_joystick1(void);
uint32_t gui_check_joystick2(void);
void gui_draw_buttons(uint32_t joy);
void gui_debug_uart(char *ptr);
void gui_show_countdown_text(const char *text, uint8_t scale);
void gui_show_win_screen(uint32_t score);
void gui_show_lost_screen(uint32_t score);
uint32_t gui_run_pause_menu(void);
void gui_resume_from_pause(uint32_t joy);

//--------------------------------------------------------------
#endif // __STM32F4_UB_GUI_H
