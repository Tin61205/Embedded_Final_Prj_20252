//--------------------------------------------------------------
// File     : stm32_ub_systick.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SYSTICK_H
#define __STM32F4_UB_SYSTICK_H



//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32_ub_usb_hid_host.h"

//#define  SYSTICK_RESOLUTION   1    // 1us Aufl�sung
#define  SYSTICK_RESOLUTION   1000   // 1ms Aufl�sung

extern USB_HID_HOST_STATUS_t  akt_usb_status;
extern uint32_t keyboard_timer;
extern uint32_t  Gui_Touch_Timer_ms;
extern uint32_t  Mode_Systic_Timer_ms;
extern uint32_t  Player_Systick_Timer_ms;
extern uint32_t  Player2_Systick_Timer_ms;
extern uint32_t  Blinky_Systic_Timer_ms;
extern uint32_t  Pinky_Systic_Timer_ms;
extern uint32_t  Inky_Systic_Timer_ms;
extern uint32_t  Clyde_Systic_Timer_ms;
extern uint32_t  UB_Game_Timers_Paused;

//--------------------------------------------------------------
// Hàm khởi tạo
//--------------------------------------------------------------
void UB_Systick_Init(void);
#if SYSTICK_RESOLUTION==1
  void UB_Systick_Pause_us(volatile uint32_t pause);
#endif
void UB_Systick_Pause_ms(volatile uint32_t pause);
void UB_Systick_Pause_s(volatile uint32_t pause);

//--------------------------------------------------------------
#endif // __STM32F4_UB_SYSTICK_H


