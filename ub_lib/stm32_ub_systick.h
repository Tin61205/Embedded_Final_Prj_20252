//--------------------------------------------------------------
// File     : stm32_ub_systick.h
//--------------------------------------------------------------
#ifndef __STM32F4_UB_SYSTICK_H
#define __STM32F4_UB_SYSTICK_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"

//#define  SYSTICK_RESOLUTION   1    // 1us resolution
#define  SYSTICK_RESOLUTION   1000   // 1ms resolution

extern uint32_t  Gui_Touch_Timer_ms;
extern uint32_t  Mode_Systic_Timer_ms;
extern uint32_t  Player_Systick_Timer_ms;
extern uint32_t  Player2_Systick_Timer_ms;
extern uint32_t  Blinky_Systic_Timer_ms;
extern uint32_t  Pinky_Systic_Timer_ms;
extern uint32_t  Inky_Systic_Timer_ms;
extern uint32_t  Clyde_Systic_Timer_ms;
extern uint32_t  HumanGhost_Systic_Timer_ms;
extern uint32_t  UB_Game_Timers_Paused;
extern uint32_t  Player_Dying_Timer_ms;
extern uint32_t  Player_Invuln_Timer_ms;
extern uint32_t  Player2_Dying_Timer_ms;
extern uint32_t  Player2_Invuln_Timer_ms;

//--------------------------------------------------------------
// Function prototypes
//--------------------------------------------------------------
void UB_Systick_Init(void);
#if SYSTICK_RESOLUTION==1
  void UB_Systick_Pause_us(volatile uint32_t pause);
#endif
void UB_Systick_Pause_ms(volatile uint32_t pause);
void UB_Systick_Pause_s(volatile uint32_t pause);

#endif // __STM32F4_UB_SYSTICK_H
